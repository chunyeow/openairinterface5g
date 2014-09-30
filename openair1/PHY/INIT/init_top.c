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
/*!\brief Initilization and reconfiguration routines for LTE PHY */
#ifndef USER_MODE
#define __NO_VERSION__
#endif
 
#include "defs.h"
#include "PHY/extern.h"
#include "MAC_INTERFACE/extern.h"
//#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"

#ifdef CBMIMO1
#include "ARCH/COMMON/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/from_grlib_softconfig.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_pci.h"
//#include "pci_commands.h"
#endif //CBMIMO1

/*!
* @addtogroup _PHY_STRUCTURES_
* Memory Initializaion and Cleanup for LTE MODEM.
* @{
\section _Memory_init_ Memory Initialization for LTE MODEM

*/

//#define DEBUG_PHY

/*
#ifndef USER_MODE
#include "SCHED/defs.h"
#endif //USER_MODE
*/


#ifndef USER_MODE

dma_addr_t dma_handle[4];

// Get from HW addresses
int init_signal_buffers(LTE_DL_FRAME_PARMS *frame_parms) {

  unsigned char card_id,i;

  int *tmp_ptr;
  mod_sym_t *tmp_ptr_tx;
  unsigned int tx_dma_buffer_size_bytes;

  for (card_id=0;card_id<number_of_cards;card_id++) {
    for (i=0;i<NB_ANTENNAS_TX;i++) {
      
      // Allocate memory for TX DMA Buffer
      
#ifdef IFFT_FPGA
      tx_dma_buffer_size_bytes = NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SYMBOLS_PER_FRAME*sizeof(mod_sym_t);
#else
#ifdef BIT8_TX
      tx_dma_buffer_size_bytes = FRAME_LENGTH_BYTES>>1;
#else
      tx_dma_buffer_size_bytes = FRAME_LENGTH_BYTES;
#endif
#endif
      
      tmp_ptr_tx = (mod_sym_t *)bigmalloc16(tx_dma_buffer_size_bytes+2*PAGE_SIZE);
      
      if (tmp_ptr_tx==NULL) {
	msg("[PHY][INIT] Could not allocate TX_DMA %d (%x bytes)\n",i, 
	    (unsigned int)(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t) + 2*PAGE_SIZE));
	return(-1);
      }
      else {
	bzero(tmp_ptr_tx,tx_dma_buffer_size_bytes+2*PAGE_SIZE);
	pci_buffer[card_id][(2*i)] = (unsigned int)tmp_ptr_tx;
	tmp_ptr_tx = (mod_sym_t*)(((unsigned int)tmp_ptr_tx + PAGE_SIZE -1) & PAGE_MASK);
	//      reserve_mem(tmp_ptr_tx,FRAME_LENGTH_BYTES+2*PAGE_SIZE);
#ifdef DEBUG_PHY
	msg("[PHY][INIT] TX_DMA_BUFFER %d at %p (%p), size 0x%x\n",i,
	    (void *)tmp_ptr_tx,
	    (void *)virt_to_phys(tmp_ptr_tx),
	    (unsigned int)(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(mod_sym_t)+2*PAGE_SIZE));
#endif
      }
      
      
      TX_DMA_BUFFER[card_id][i] = (int) tmp_ptr_tx;
    }
      
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      // RX DMA Buffers
      tmp_ptr = (int *)bigmalloc16(FRAME_LENGTH_BYTES+OFDM_SYMBOL_SIZE_BYTES+2*PAGE_SIZE);
      
      if (tmp_ptr==NULL) {
#ifdef DEBUG_PHY
	msg("[PHY][INIT] Could not allocate RX_DMA %d (%x bytes)\n",i, 
	    FRAME_LENGTH_BYTES+2*OFDM_SYMBOL_SIZE_BYTES + 2*PAGE_SIZE);
#endif
	return(-1);
      }
      else {
	bzero(tmp_ptr,FRAME_LENGTH_BYTES+OFDM_SYMBOL_SIZE_BYTES+2*PAGE_SIZE);
	pci_buffer[card_id][1+(2*i)] = (int) tmp_ptr;
	tmp_ptr = (int*) (((unsigned long)tmp_ptr + PAGE_SIZE -1) & PAGE_MASK);
	//          reserve_mem(tmp_ptr,FRAME_LENGTH_BYTES+2*PAGE_SIZE);
	
#ifdef DEBUG_PHY
	msg("[PHY][INIT] RX_DMA_BUFFER %d at %p (%p), size 0x%x\n",i,
	    (void *)tmp_ptr,
	    (void *)virt_to_phys(tmp_ptr),(unsigned int)(FRAME_LENGTH_BYTES+OFDM_SYMBOL_SIZE_BYTES+2*PAGE_SIZE));
#endif
      }
      
      
      RX_DMA_BUFFER[card_id][i] = (int) tmp_ptr;
    }
        
  }    

#ifndef NOCARD_TEST
  for (card_id=0;card_id<number_of_cards;card_id++) {
    // Allocate memory for PCI interface and store pointers to dma buffers

    if (vid != XILINX_VENDOR) {
      msg("[PHY][INIT] Setting up Leon PCI interface structure\n");
      pci_interface[card_id] = (PCI_interface_t *)bigmalloc16(sizeof(PCI_interface_t));
      msg("[PHY][INIT] PCI interface %d at %p\n",card_id,pci_interface[card_id]);
      openair_writel(pdev[card_id],FROM_GRLIB_CFG_GRPCI_EUR_CTRL0_OFFSET+4,(unsigned int)virt_to_phys((volatile void*)pci_interface[card_id]));  
      
      for (i=0;i<NB_ANTENNAS_RX;i++) {
	pci_interface[card_id]->adc_head[i] = (unsigned int)virt_to_phys((volatile void*)RX_DMA_BUFFER[card_id][i]);
	pci_interface[card_id]->dac_head[i] = (unsigned int)virt_to_phys((volatile void*)TX_DMA_BUFFER[card_id][i]);
      }

      mbox = (unsigned int)(&pci_interface[0]->adac_cnt);
      msg("[PHY][INIT] mbox = %p\n",mbox);
    }
    else {
      msg("[PHY][INIT] PCIe interface %d at %p\n",card_id,exmimo_pci_interface);
      //      openair_writel(pdev[card_id],FROM_GRLIB_CFG_GRPCI_EUR_CTRL0_OFFSET+4,(unsigned int)virt_to_phys((volatile void*)pci_interface[card_id]));  
      DAQ_MBOX = (unsigned int)bigmalloc16(16);
      exmimo_pci_interface->rf.mbox        = (unsigned int)virt_to_phys((volatile void*)DAQ_MBOX);
      
      for (i=0;i<NB_ANTENNAS_RX;i++) {
	exmimo_pci_interface->rf.adc_head[i] = (unsigned int)virt_to_phys((volatile void*)RX_DMA_BUFFER[card_id][i]);
	exmimo_pci_interface->rf.dac_head[i] = (unsigned int)virt_to_phys((volatile void*)TX_DMA_BUFFER[card_id][i]);
      }
    }
  }
#endif //NOCARD_TEST

  return(0);

}
#endif // USER_MODE
/*
int init_frame_parms(LTE_DL_FRAME_PARMS *frame_parms) {

  if (frame_parms->Ncp==1) {
    frame_parms->nb_prefix_samples0=512;
    frame_parms->nb_prefix_samples = 512;
    frame_parms->symbols_per_tti = 12;
  }
  else {
    frame_parms->nb_prefix_samples0 = 160;
    frame_parms->nb_prefix_samples = 144;
    frame_parms->symbols_per_tti = 14;
  }
  
  switch (frame_parms->N_RB_DL) {
  case 100:
    frame_parms->ofdm_symbol_size = 2048;
    frame_parms->log2_symbol_size = 11;
    frame_parms->samples_per_tti = 30720;
    frame_parms->first_carrier_offset = 1448;
    break;
  case 50:
    frame_parms->ofdm_symbol_size = 1024;
    frame_parms->log2_symbol_size = 10;
    frame_parms->samples_per_tti = 15360;
    frame_parms->first_carrier_offset = 724; 
    frame_parms->nb_prefix_samples>>=1;
    frame_parms->nb_prefix_samples0>>=1;
   break;
  case 25:
    frame_parms->ofdm_symbol_size = 512;
    frame_parms->log2_symbol_size = 9;
    frame_parms->samples_per_tti = 7680;
    frame_parms->first_carrier_offset = 362;
    frame_parms->nb_prefix_samples>>=2;
    frame_parms->nb_prefix_samples0>>=2;
    break;
  case 15:
    frame_parms->ofdm_symbol_size = 256;
    frame_parms->log2_symbol_size = 8;
    frame_parms->samples_per_tti = 3840;
    frame_parms->first_carrier_offset = 166;
    frame_parms->nb_prefix_samples>>=3;
    frame_parms->nb_prefix_samples0>>=1;
    break;
  case 6:
    frame_parms->ofdm_symbol_size = 128;
    frame_parms->log2_symbol_size = 7;
    frame_parms->samples_per_tti = 1920;
    frame_parms->first_carrier_offset = 92;
    frame_parms->nb_prefix_samples>>=4;
    frame_parms->nb_prefix_samples0>>=1;
    break;

  default:
    msg("init_frame_parms: Error: Number of resource blocks (N_RB_DL %d) undefined, frame_parms = %p \n",frame_parms->N_RB_DL, frame_parms);
    return(-1);
    break;
  }

  //  frame_parms->tdd_config=3;
  return(0);
}
*/

int phy_init_top(LTE_DL_FRAME_PARMS *frame_parms) {

  // bzero((void *)PHY_vars,sizeof(PHY_VARS));
  LOG_I(PHY,"[INIT] OFDM size             : %d\n",NUMBER_OF_OFDM_CARRIERS);
  LOG_I(PHY,"[INIT] FRAME_LENGTH_SAMPLES  : %d\n",FRAME_LENGTH_SAMPLES);
  LOG_I(PHY,"[INIT] NUMBER_OF_SYMBOLS_PER_FRAME  : %d\n",NUMBER_OF_SYMBOLS_PER_FRAME);
  LOG_I(PHY,"[INIT] Initializing FFT engine using %d point fft (%d, %p)\n",NUMBER_OF_OFDM_CARRIERS,LOG2_NUMBER_OF_OFDM_CARRIERS,rev );

#ifndef USER_MODE
  init_signal_buffers(frame_parms);
#endif
   
  // Initialize fft variables
  init_fft(NUMBER_OF_OFDM_CARRIERS,LOG2_NUMBER_OF_OFDM_CARRIERS,rev);   // TX/RX
  init_fft(4*NUMBER_OF_OFDM_CARRIERS,2+LOG2_NUMBER_OF_OFDM_CARRIERS,rev_times4);   // Synch
  init_fft(NUMBER_OF_OFDM_CARRIERS/2,LOG2_NUMBER_OF_OFDM_CARRIERS-1,rev_half);   // for interpolation of channel est

  init_fft(8192,13,rev8192); 
  init_fft(4096,12,rev4096);
  init_fft(2048,11,rev2048);
  init_fft(1024,10,rev1024);
  init_fft(512,9,rev512);
  
  twiddle_fft = (short *)malloc16(4095*4*2);
  twiddle_ifft = (short *)malloc16(4095*4*2);
  twiddle_fft_times4 = (short*)malloc16(4095*4*2);
  twiddle_ifft_times4 = (short*)malloc16(4095*4*2);
  twiddle_fft_half = (short*)malloc16(4095*4*2);
  twiddle_ifft_half = (short*)malloc16(4095*4*2);

  LOG_I(PHY,"[INIT] twiddle_fft= %p, twiddle_ifft=%p, twiddle_fft_times4=%p,twiddle_ifft_times4=%p\n",
	 (void *)twiddle_fft,(void *)twiddle_ifft,(void *)twiddle_fft_times4,(void *)twiddle_ifft_times4);

  switch (NUMBER_OF_OFDM_CARRIERS) {
	  
  case 64:
    memcpy(twiddle_fft,&twiddle_fft64[0],63*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft64[0],63*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft256[0],255*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft256[0],255*4*2);
    //memcpy(twiddle_fft_half,&twiddle_fft32[0],31*4*2);
    //memcpy(twiddle_ifft_half,&twiddle_ifft32[0],31*4*2);
    break;
  case 128:
    memcpy(twiddle_fft,&twiddle_fft128[0],127*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft128[0],127*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft512[0],511*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft512[0],511*4*2);
    memcpy(twiddle_fft_half,&twiddle_fft64[0],63*4*2);
    memcpy(twiddle_ifft_half,&twiddle_ifft64[0],63*4*2);
    break;
  case 256:
    memcpy(twiddle_fft,&twiddle_fft256[0],255*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft256[0],255*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft1024[0],1023*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft1024[0],1023*4*2);
    memcpy(twiddle_fft_half,&twiddle_fft128[0],127*4*2);
    memcpy(twiddle_ifft_half,&twiddle_ifft128[0],127*4*2);
    break;
  case 512:
    memcpy(twiddle_fft,&twiddle_fft512[0],511*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft512[0],511*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft2048[0],2047*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft2048[0],2047*4*2);
    memcpy(twiddle_fft_half,&twiddle_fft256[0],255*4*2);
    memcpy(twiddle_ifft_half,&twiddle_ifft256[0],255*4*2);
    break;
  case 1024:
    memcpy(twiddle_fft,&twiddle_fft1024[0],1023*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft1024[0],1023*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft4096[0],4095*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft4096[0],4095*4*2);
    memcpy(twiddle_fft_half,&twiddle_fft512[0],511*4*2);
    memcpy(twiddle_ifft_half,&twiddle_ifft512[0],511*4*2);
    break;
  case 2048:
    memcpy(twiddle_fft,&twiddle_fft2048[0],2047*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft2048[0],2047*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft8192[0],8191*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft8192[0],8191*4*2);
    memcpy(twiddle_fft_half,&twiddle_fft1024[0],1023*4*2);
    memcpy(twiddle_ifft_half,&twiddle_ifft1024[0],1023*4*2);
    break;
  default:
    memcpy(twiddle_fft,&twiddle_fft64[0],63*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft64[0],63*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft256[0],255*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft256[0],255*4*2);
    //memcpy(twiddle_fft_half,&twiddle_fft32[0],31*4*2);
    //memcpy(twiddle_ifft_half,&twiddle_ifft32[0],31*4*2);
    break;
  }

  frame_parms->twiddle_fft      = twiddle_fft;
  frame_parms->twiddle_ifft     = twiddle_ifft;
  frame_parms->rev              = rev;

  return(1);
}

void phy_cleanup(void) {

#ifndef USER_MODE
  unsigned int dummy_ptr;
  unsigned char card_id,i;
#endif //USER_MODE

  // stop PHY_thread


  LOG_I(PHY,"[INIT] cleanup\n");

  
#ifndef USER_MODE

  for (card_id=0;card_id<number_of_cards;card_id++) {


    
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      
      if (pci_buffer[card_id][2*i]) {
#ifdef DEBUG_PHY    
	msg("[openair][PHY][INIT] pci_buffer card %d %d\n",card_id,2*i);
#endif    
	bigfree(pci_buffer[card_id][2*i],FRAME_LENGTH_BYTES+2*PAGE_SIZE);
	//      free_pages(pci_buffer[2*i],8);
#ifdef DEBUG_PHY    
	msg("[openair][PHY][INIT] Freed TX_DMA_BUFFER %d\n",i);
#endif
      }

      if (pci_buffer[card_id][1+(2*i)]) {
#ifdef DEBUG_PHY    
	msg("[openair][PHY][INIT] pci_buffer %d %d\n",card_id,1+(2*i));
#endif
	dummy_ptr = virt_to_phys(pci_buffer[card_id][1+(2*i)]);
	bigfree(pci_buffer[card_id][1+(2*i)],FRAME_LENGTH_BYTES+2*PAGE_SIZE);
	
	
	//      free_pages(pci_buffer[1+(2*i)],8);
#ifdef DEBUG_PHY    
	msg("[openair][PHY][INIT] Freed RX_DMA_BUFFER %d\n",i);
#endif
      }
    }
  }  

#else
      // Do USER_MODE cleanup here
#endif // USER_MODE
  msg("[openair][CLEANUP] Done!\n");
}


/*
 * @}*/
