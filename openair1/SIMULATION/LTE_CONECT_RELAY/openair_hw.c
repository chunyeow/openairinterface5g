/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "SIMULATION/TOOLS/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "MAC_INTERFACE/extern.h"
#ifdef IFFT_FPGA
#include "PHY/LTE_REFSIG/mod_table.h"
#endif

#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#include "ARCH/COMMON/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "SCHED/defs.h"
#include "SCHED/extern.h"
#include "LAYER2/MAC/extern.h"


TX_RX_VARS dummy_tx_rx_vars;


int pci_buffers[2*NB_ANTENNAS_RX];
int openair_fd,fc;
unsigned int     bigphys_top;
unsigned int mem_base;


int setup_oai_hw(LTE_DL_FRAME_PARMS *frame_parms,
		 PHY_VARS_UE  *phy_vars_ue,
		 PHY_VARS_eNB *phy_vars_eNB) {
  int i,j;

  frame_parms->dual_tx = 0;
  frame_parms->freq_idx = 0;
  fc = 0;
  
  printf("Opening /dev/openair0\n");
  if ((openair_fd = open("/dev/openair0", O_RDWR)) <0) {
    fprintf(stderr,"Error %d opening /dev/openair0\n",openair_fd);
    exit(-1);
  }
  
  ioctl(openair_fd,openair_DUMP_CONFIG,frame_parms);
  sleep(1);
  //    ioctl(openair_fd,openair_GET_BUFFER,(void *)&fc);
  ioctl(openair_fd,openair_GET_VARS,&dummy_tx_rx_vars);
  ioctl(openair_fd,openair_GET_BIGPHYSTOP,(void *)&bigphys_top);
  
  if (dummy_tx_rx_vars.TX_DMA_BUFFER[0]==NULL) {
    printf("pci_buffers not allocated\n");
    close(openair_fd);
    exit(-1);
  }
  
  printf("BIGPHYS top 0x%x\n",bigphys_top);
  printf("RX_DMA_BUFFER[0] %p\n",dummy_tx_rx_vars.RX_DMA_BUFFER[0]);
  printf("TX_DMA_BUFFER[0] %p\n",dummy_tx_rx_vars.TX_DMA_BUFFER[0]);
  
  mem_base = (unsigned int) mmap(0,
				 BIGPHYS_NUMPAGES*4096,
				 PROT_READ|PROT_WRITE,
				 MAP_SHARED|MAP_FIXED,//MAP_SHARED,
				 openair_fd,
				 0);
  
  if (mem_base != -1)
    msg("MEM base= 0x%x\n",mem_base);
  else {
    msg("Could not map physical memory\n");
    close(openair_fd);
    exit(-1);
  }
  
  if (phy_vars_ue) {
    
    // replace RX signal buffers with mmaped HW versions
    for (i=0;i<frame_parms->nb_antennas_rx;i++) {
      free(phy_vars_ue->lte_ue_common_vars.rxdata[i]);
      phy_vars_ue->lte_ue_common_vars.rxdata[i] = (int32_t*)((int)dummy_tx_rx_vars.RX_DMA_BUFFER[0]-bigphys_top+mem_base);
      printf("rxdata[%d] @ %p\n",i,phy_vars_ue->lte_ue_common_vars.rxdata[i]);
    }
    for (i=0;i<frame_parms->nb_antennas_tx;i++) {
      free(phy_vars_ue->lte_ue_common_vars.txdata[i]);
      phy_vars_ue->lte_ue_common_vars.txdata[i] = (int32_t*)((int)dummy_tx_rx_vars.TX_DMA_BUFFER[0]-bigphys_top+mem_base);
      printf("txdata[%d] @ %p\n",i,phy_vars_ue->lte_ue_common_vars.txdata[i]);
    }
  }

  if (phy_vars_eNB) {
    // replace RX signal buffers with mmaped HW versions
    for (i=0;i<frame_parms->nb_antennas_rx;i++) {
      free(phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i]);
      phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i] = (int32_t*)((int)dummy_tx_rx_vars.RX_DMA_BUFFER[0]-bigphys_top+mem_base);
      printf("rxdata[%d] @ %p\n",i,phy_vars_eNB->lte_eNB_common_vars.rxdata[0][i]);
    }
    for (i=0;i<frame_parms->nb_antennas_tx;i++) {
      free(phy_vars_eNB->lte_eNB_common_vars.txdata[0][i]);
      phy_vars_eNB->lte_eNB_common_vars.txdata[0][i] = (int32_t*)((int)dummy_tx_rx_vars.TX_DMA_BUFFER[0]-bigphys_top+mem_base);
      printf("txdata[%d] @ %p\n",i,phy_vars_eNB->lte_eNB_common_vars.txdata[0][i]);
      for (j=0;j<16;j++) {
	printf("txbuffer %d: %x\n",j,phy_vars_eNB->lte_eNB_common_vars.txdata[0][i][j]);
	phy_vars_eNB->lte_eNB_common_vars.txdata[0][i][j] = 16-j;
      }
      //      msync(openair_fd);
    }
  }

  return(openair_fd);
}
