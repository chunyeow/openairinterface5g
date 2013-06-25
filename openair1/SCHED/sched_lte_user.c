/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file sched_lte_fw2011.c
* \brief RTAI scheduler for LTE UE/eNB procedures (CBMIMO1 2011 FPGA firmware)
* \author R. Knopp, M. Guillaud, F. Kaltenberger
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr
* \note
* \warning
*/ 
/*
 *  @{ 
 */

/*
* @addtogroup _physical_layer_ref_implementation_
\section _process_scheduling_ Process Scheduling
This section deals with real-time process scheduling for PHY and synchronization with certain hardware targets (CBMIMO1).
*/

#ifndef USER_MODE
#define __NO_VERSION__

/*
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/netdevice.h>

#include <asm/io.h>
#include <asm/bitops.h>
 
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <asm/page.h>
*/


#ifdef RTAI_ISNT_POSIX
#include "rt_compat.h"
#endif /* RTAI_ISNT_POSIX */

#include "MAC_INTERFACE/extern.h"

#ifdef CBMIMO1
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#endif // CBMIMO1

#ifdef RTAI_ENABLED
#include <rtai.h>
#include <rtai_posix.h>
#include <rtai_fifos.h>
#endif //


#include <linux/pci.h>

#else
#include <stdio.h>
#include <stdlib.h>
#endif //  /* USER_MODE */


#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "defs.h"
#include "extern.h"

#ifdef CBMIMO1
#include "ARCH/CBMIMO1/DEVICE_DRIVER/from_grlib_softregs.h"
#endif //CBMIMO1

#ifdef SERIAL_IO
#include "rtai_serial.h"
#endif

#ifdef EMOS
#include "phy_procedures_emos.h"
extern  fifo_dump_emos_UE emos_dump_UE;
#endif

/// Mutex for instance count on MACPHY scheduling 
pthread_mutex_t openair_mutex;
/// Condition variable for MACPHY thread
pthread_cond_t openair_cond;
/// Threads
pthread_t         threads[openair_SCHED_NB_THREADS]={NULL,NULL,NULL};
/// Thread Attributes
pthread_attr_t    attr_threads[openair_SCHED_NB_THREADS];
/// RX Signal FIFO for Testing without hardware
#define RX_SIG_FIFO_NUMBER 59 
int rx_sig_fifo = RX_SIG_FIFO_NUMBER;
/// RX Control FIFO for testing without hardware
#define RF_CNTL_FIFO_NUMBER 60 
int rf_cntl_fifo = RF_CNTL_FIFO_NUMBER;

#ifdef NOCARD_TEST

/// RX Signal FIFO Mutex
pthread_mutex_t openair_rx_fifo_mutex;
/// RX Signal FIFO Condition variable
pthread_cond_t openair_rx_fifo_cond;

/// Packet for RX Signal Control
RF_CNTL_PACKET rf_cntl_packet;
#endif //NOCARD_TEST

/// Global exit variable (exit on error or manual stop via IOCTL)
int exit_openair = 0;

extern int init_dlsch_threads(void);
extern void cleanup_dlsch_threads(void);

extern int dlsch_instance_cnt[8];
extern pthread_mutex_t dlsch_mutex[8];
extern pthread_cond_t dlsch_cond[8];

#define NO_SYNC_TEST 1

#ifdef CBMIMO1
#define NUMBER_OF_CHUNKS_PER_SLOT NUMBER_OF_OFDM_SYMBOLS_PER_SLOT
#define NUMBER_OF_CHUNKS_PER_FRAME (NUMBER_OF_CHUNKS_PER_SLOT * LTE_SLOTS_PER_FRAME)
#define SYNCH_WAIT_TIME 4000  // number of symbols between SYNCH retries
#define SYNCH_WAIT_TIME_RUNNING 100  // number of symbols between SYNCH retries
#define DRIFT_OFFSET 300
#define NS_PER_SLOT 500000
#define MAX_DRIFT_COMP 3000
#endif // CBMIMO1
 
#define MAX_SCHED_CNT 50000000


rtheap_t rt_heap;

unsigned int sync_slot_cnt=0;
unsigned int sync_getting_frame = 0;

//static int hw_frame = 0;
static int intr_cnt = 0;
int intr_cnt2 = 0;

int intr_in = 0;


void openair1_restart(void) {

  int i;
  
  for (i=0;i<number_of_cards;i++)
    openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);
  //  openair_daq_vars.tx_test=0;
  openair_daq_vars.sync_state = 0;
  if (openair_daq_vars.is_eNB==0)
    PHY_vars_eNB_g[0]->frame = 0;
  else
    PHY_vars_UE_g[0]->frame = 0; 
  /*
  if ((mac_xface->is_cluster_head) && (mac_xface->is_primary_cluster_head)) {
    openair_daq_vars.mode = openair_SYNCHED_TO_MRSCH;
  }
  else {
    openair_daq_vars.mode = openair_NOT_SYNCHED;
  }
  */

#ifdef OPENAIR2
  msg("[openair][SCHED][SYNCH] Clearing MAC Interface\n");
  //mac_resynch();
#endif //OPENAIR2

}

int slot_irq_handler(int irq, void *cookie) {

  unsigned int adac_cnt;
  unsigned short irqval;
  LTE_DL_FRAME_PARMS *frame_parms=lte_frame_parms_g;
  int rc;
  RTIME             tv;
  struct timespec   ts;
  u32 irqcmd;
  static int busy=0;
  unsigned int slot_interrupt = 0;

  intr_in = 1;
  //  intr_cnt++;



  if (vid != XILINX_VENDOR) { //CBMIMO1

    // check interrupt status register
    pci_read_config_word(pdev[0],6 , &irqval);
    
    if ((irqval&8) != 0)  {

      intr_cnt++;
      slot_interrupt = 1;
      //msg("got interrupt for CBMIMO1, intr_cnt=%d, node_configured=%d\n",intr_cnt,openair_daq_vars.node_configured);

      // RESET PCI IRQ
      openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, FROM_GRLIB_BOOT_HOK|FROM_GRLIB_PCI_IRQ_ACK);
      openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, FROM_GRLIB_BOOT_HOK);
	


      rt_ack_irq(irq);
      rt_unmask_irq(irq);
      rt_enable_irq(irq);
      intr_in = 0;
      
    } 
    else {  // CBMIMO is not source of interrupt
      
      rt_pend_linux_irq(irq);
      intr_in = 0;
      return IRQ_NONE;
    }
       
    if (oai_semaphore && inst_cnt_ptr && lxrt_task && (slot_interrupt == 1)) {
      rt_sem_wait(oai_semaphore);
      if ((intr_cnt2%2000)==0) rt_printk("***intr_cnt %d, intr_cnt2 %d, inst_cnt_ptr %p, inst_cnt %d\n",intr_cnt,intr_cnt2,inst_cnt_ptr,*inst_cnt_ptr);
      if (*inst_cnt_ptr==0) {
	rt_sem_signal(oai_semaphore); //now the mutex should have vaule 1
	if (busy==0) { 
	  rt_printk("intr_cnt %d, worker thread busy!\n", intr_cnt);
	  busy = 1;
	} //else no need to repeat this message
      }
      else {
	(*inst_cnt_ptr)++;
	//rt_printk("*inst_cnt_ptr %d\n",*inst_cnt_ptr);
	rt_sem_signal(oai_semaphore); //now the mutex should have vaule 1
	rt_send_if(lxrt_task,intr_cnt);
	if (busy==1) {
	  rt_printk("intr_cnt %d, resuming worker thread!\n", intr_cnt);
	  busy = 0;
	} //else no need to repeat this message
      }
      intr_cnt2++;
    }
  }
  else { //EXPRESS MIMO
    
    //    msg("Got Exmimo PCIe interrupt ...\n");

    irqval = ioread32(bar[0]);

     
    if ((irqval&0x80) != 0) {
      // clear PCIE interrupt bit (bit 7 of register 0x0)
      iowrite32(irqval&0xffffff7f,bar[0]);
      irqcmd = ioread32(bar[0]+0x4);

   
      if (irqcmd == SLOT_INTERRUPT) {
	//	process_slot_interrupt();

	intr_cnt++;
	slot_interrupt = 1;
	rt_ack_irq(irq);
	rt_unmask_irq(irq);
	rt_enable_irq(irq);
	intr_in = 0;
	if ((intr_cnt%2000)==0) rt_printk("intr_cnt %d\n",intr_cnt);

      }
      else if (irqcmd == PCI_PRINTK) {
	//	msg("Got PCIe interrupt for printk ...\n");
	pci_fifo_printk();
	rt_ack_irq(irq);
	rt_unmask_irq(irq);
	rt_enable_irq(irq);
	intr_in = 0;
	return IRQ_HANDLED;	
      }
      else if (irqcmd == GET_FRAME_DONE) {
	msg("Got PCIe interrupt for GET_FRAME_DONE ...\n");
	openair_daq_vars.get_frame_done=1;
	rt_ack_irq(irq);
	rt_unmask_irq(irq);
	rt_enable_irq(irq);
	intr_in = 0;
	return IRQ_HANDLED;
      }

    }
    else {
      // RESET PCI IRQ
      rt_printk("EXMIMO: got %x from control register (intr_cnt %d)\n",irqval,intr_cnt);
      irqval = ioread32(bar[0]);
      rt_printk("EXMIMO: retry got %x from control register (intr_cnt %d)\n",irqval,intr_cnt);

      rt_pend_linux_irq(irq);
      intr_in = 0;
      return IRQ_NONE;
    }
  }

 



  // clear PCIE interrupt bit (bit 7 of register 0x0)


  return IRQ_HANDLED;

}




s32 openair_sched_init(void) {


  int error_code;

  int* tmp;
  
  LTE_DL_FRAME_PARMS *frame_parms = lte_frame_parms_g;
  
  
  openair_daq_vars.scheduler_interval_ns=NS_PER_SLOT;        // initial guess
  
  openair_daq_vars.last_adac_cnt=-1;            
  
  exit_openair=0;
  
  //pthread_mutex_init(&openair_mutex,NULL);
  
  //pthread_cond_init(&openair_cond,NULL);
  
  /*
  if (openair_daq_vars.is_eNB==1){
    printk("[openair][SCHED][init] Configuring primary clusterhead\n");
    PHY_vars_eNB_g[0]->frame=0;
  }
  else {
    PHY_vars_UE_g[0]->frame=0;
    printk("[openair][SCHED][init] Configuring regular node\n");
  }
  */
  openair_daq_vars.mode = openair_NOT_SYNCHED;
  
#ifdef EMOS
  error_code = rtf_create(CHANSOUNDER_FIFO_MINOR,CHANSOUNDER_FIFO_SIZE);
  printk("[OPENAIR][SCHED][INIT] Created EMOS FIFO, error code %d\n",error_code);
#endif

  openair_daq_vars.instance_cnt = -1;

  error_code = rtf_create(rx_sig_fifo, NB_ANTENNAS_RX*FRAME_LENGTH_BYTES);
  printk("[openair][SCHED][INIT] Created rx_sig_fifo (%d bytes), error_code %d\n",
      NB_ANTENNAS_RX*FRAME_LENGTH_BYTES,
      error_code); 

  tmp = rt_malloc(sizeof(int));
  printk("[openair][SCHED][INIT] tmp= %p\n",tmp);
  rt_free(tmp);

  return(error_code);

}

void openair_sched_cleanup() {

#ifdef EMOS
  int error_code;
#endif
  exit_openair = 1;
  openair_daq_vars.mode = openair_SCHED_EXIT;

#ifdef EMOS
  error_code = rtf_destroy(CHANSOUNDER_FIFO_MINOR);
  printk("[OPENAIR][SCHED][CLEANUP] EMOS FIFO closed, error_code %d\n", error_code);
#endif
 
  //if (mac_xface->is_cluster_head == 0)
  //FK: mac_xface->is_cluster_head not initialized at this stage
  //  cleanup_dlsch_threads();

  if (openair_irq_enabled==1) {
    rt_disable_irq(pdev[0]->irq);
    rt_release_irq(pdev[0]->irq);
  }

  printk("[openair][SCHED][CLEANUP] Done!\n");

  
}


void openair_sched_exit(char *str) {
  u8 i;

  msg("%s\n",str);
  msg("[OPENAIR][SCHED] openair_sched_exit() called, preparing to exit ...\n");
  
  exit_openair = 1;
  openair_daq_vars.mode = openair_SCHED_EXIT;
  for (i=0;i<number_of_cards;i++)
    openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_DMA_STOP);

}



/*@}*/


