/*!\brief Initilization and reconfiguration routines for PHY */
#ifndef USER_MODE
#define __NO_VERSION__

/*

* @addtogroup _PHY_STRUCTURES_
* Memory Initializaion and Cleanup.
* @{
\section _Memory_init_ Memory Initialization
Blah Blah
*/

#ifdef RTAI_ENABLED
#include <rtai.h>
#include <rtai_posix.h>
#include <rtai_fifos.h>
#endif //

#else
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#define PAGE_SIZE 4096
#endif // USER_MODE

//#include "types.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
//#include "MAC_INTERFACE/defs.h"
#include "MAC_INTERFACE/extern.h"

#ifndef USER_MODE
#include "SCHED/defs.h"
#ifdef PLATON

#endif //PLATON


#ifdef CBMIMO1
#include "ARCH/CBMIMO1/DEVICE_DRIVER/from_grlib_softconfig.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_pci.h"
//#include "pci_commands.h"

#endif //CBMIMO1
#endif //USER_MODE

#ifdef USER_MODE
#define printk printf
#endif //USER_MODE

int phy_init(unsigned char nb_antennas_tx) {


  int *tmp_ptr;
  unsigned int tx_dma_buffer_size_bytes;

  int i,j,n,tb;

  bzero((void *)PHY_vars,sizeof(PHY_VARS));



  if (mac_xface->is_cluster_head == 1)
    printk("[openair][PHY][INIT]TERMINODE is preconfigured as a Cluster Head\n");
  printk("[openair][PHY][INIT]OFDM size             : %d\n",NUMBER_OF_OFDM_CARRIERS);
  printk("[openair][PHY][INIT]FRAME_LENGTH_SAMPLES  : %d\n",FRAME_LENGTH_SAMPLES);
  printk("[openair][PHY][INIT]NUMBER_OF_SYMBOLS_PER_FRAME : %d\n",NUMBER_OF_SYMBOLS_PER_FRAME);


    
  crcTableInit();
  
  ccodedot11_init();
  ccodedot11_init_inv();

  ccodelte_init();
  ccodelte_init_inv();
  
#ifndef EXPRESSMIMO_TARGET
  phy_generate_viterbi_tables();
#endif //EXPRESSMIMO_TARGET

  for (i=0;i<NB_ANTENNAS_RX;i++) {


    // Allocate memory for TX DMA Buffer

#ifdef IFFT_FPGA
    tx_dma_buffer_size_bytes = NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SYMBOLS_PER_FRAME*sizeof(mod_sym_t);
#else
    tx_dma_buffer_size_bytes = FRAME_LENGTH_BYTES;
#endif
    tmp_ptr = (mod_sym_t*) bigmalloc16(tx_dma_buffer_size_bytes+2*PAGE_SIZE);
			  
    if (tmp_ptr==NULL) {
      printk("[PHY][INIT] Could not allocate TX_DMA %d (%x bytes)\n",i, 
	  (unsigned int)(tx_dma_buffer_size_bytes+2*PAGE_SIZE));
      return(-1);
    }
    else {
#ifndef USER_MODE
      pci_buffer[(2*i)] = (unsigned int)tmp_ptr;
      tmp_ptr = (unsigned int*)(((unsigned int)tmp_ptr + PAGE_SIZE -1) & PAGE_MASK);
      //      reserve_mem(tmp_ptr,FRAME_LENGTH_BYTES+2*PAGE_SIZE);
#endif // //USER_MODE
#ifdef DEBUG_PHY
      printk("[PHY][INIT] TX_DMA_BUFFER %d at %p (%p), size 0x%x\n",i,
	  (void *)tmp_ptr,
	  (void *)virt_to_phys(tmp_ptr),
	  (unsigned int)(tx_dma_buffer_size+2*PAGE_SIZE));
#endif
    }
 
    PHY_vars->tx_rx_switch_point = TX_RX_SWITCH_SYMBOL;


    PHY_vars->tx_vars[i].TX_DMA_BUFFER = tmp_ptr;
#ifdef DEBUG_PHY
    printk("[PHY][INIT] PHY_vars->tx_vars[%d].TX_DMA_BUFFER = %p\n",i,(void *)PHY_vars->tx_vars[i].TX_DMA_BUFFER);
#endif


#ifndef USER_MODE
    TX_DMA_BUFFER[i] = tmp_ptr;
#endif //USER_MODE



#ifndef USER_MODE

#ifdef PLATON
    tx_dma_buffer[i]=tmp_ptr;
    tx_mbox[i] = tmp_ptr + (FRAME_LENGTH_BYTES>>2);
#endif // //USER_MODE
#endif // // PLATON


    // RX DMA Buffers
    tmp_ptr = (int *)bigmalloc16(FRAME_LENGTH_BYTES+OFDM_SYMBOL_SIZE_BYTES+2*PAGE_SIZE);
					  
    if (tmp_ptr==NULL) {
#ifdef DEBUG_PHY
      printk("[PHY][INIT] Could not allocate RX_DMA %d (%x bytes)\n",i, 
	  FRAME_LENGTH_BYTES+2*OFDM_SYMBOL_SIZE_BYTES + 2*PAGE_SIZE);
#endif
      return(-1);
    }
    else {
#ifndef USER_MODE
      pci_buffer[1+(2*i)] = tmp_ptr;

      tmp_ptr = (((unsigned long)tmp_ptr + PAGE_SIZE -1) & PAGE_MASK);
      //          reserve_mem(tmp_ptr,FRAME_LENGTH_BYTES+2*PAGE_SIZE);

#endif //USER_MODE
#ifdef DEBUG_PHY
      printk("[PHY][INIT] RX_DMA_BUFFER %d at %p (%p), size %x\n",i,
	  (void *)tmp_ptr,
	  (void *)virt_to_phys(tmp_ptr),(unsigned int)(FRAME_LENGTH_BYTES+OFDM_SYMBOL_SIZE_BYTES+2*PAGE_SIZE));
#endif
    }



    PHY_vars->rx_vars[i].RX_DMA_BUFFER = tmp_ptr;
#ifdef PLATON
#ifndef USER_MODE
    rx_mbox[i] = tmp_ptr + ((FRAME_LENGTH_BYTES+OFDM_SYMBOL_SIZE_BYTES)>>2);
    rx_dma_buffer[i] = tmp_ptr;
#endif // //USER_MODE
#endif // //PLATON
    //#endif //


#ifndef USER_MODE
    RX_DMA_BUFFER[i] = tmp_ptr;
#endif // //USER_MODE
  }


#ifndef USER_MODE
#ifndef NOCARD_TEST
#ifndef PLATON
  // Allocate memory for PCI interface and store pointers to dma buffers
  printk("[PHY][INIT] Setting up Leon PCI interface structure\n");
  pci_interface = (PCI_interface_t *)((unsigned int)(tmp_ptr + ((OFDM_SYMBOL_SIZE_BYTES+FRAME_LENGTH_BYTES+PAGE_SIZE)>>2)));
  printk("[PHY][INIT] PCI interface at %p\n",pci_interface);
  if (vid != XILINX_VENDOR)
    openair_writel(pdev[0],FROM_GRLIB_CFG_GRPCI_EUR_CTRL0_OFFSET+4,(unsigned int)virt_to_phys((volatile void*)pci_interface));  
  else
    iowrite32((unsigned int)virt_to_phys((volatile void*)pci_interface),(bar[0]+0x1c));

  mbox = (unsigned int)(&pci_interface->adac_cnt);
  for (i=0;i<NB_ANTENNAS_RX;i++) {
    pci_interface->adc_head[i] = (unsigned int)virt_to_phys((volatile void*)RX_DMA_BUFFER[i]);
    pci_interface->dac_head[i] = (unsigned int)virt_to_phys((volatile void*)TX_DMA_BUFFER[i]);
  }
#endif //PLATON
#endif //NOCARD_TEST
#endif // USER_MODE


#ifdef PLATON
#ifndef USER_MODE
  PHY_vars->mbox = tx_mbox[0];
#endif //// USER_MODE
#endif // //PLATON
#ifdef CBMIMO1
#ifndef USER_MODE
  PHY_vars->mbox = mbox;
#endif //// USER_MODE 
#endif // // CBMIMO1

  //  printk("[PHY][INIT] mbox = %p,rxgainreg = %p\n",PHY_vars->mbox,rxgainreg);


  //#ifndef USER_MODE
  //  rt_sleep(nano2count(100000000));
  //#endif
	// Loop over all different SCHs
  for (n=0;n<8;n++) {
#ifdef DEBUG_PHY    
    printk("[openair][PHY][INIT] CHSCH %d:\n",n);
    printk("[openair][PHY][INIT] ******************\n");
#endif
    PHY_vars->chsch_data[n].CHSCH_conj_f       = (int *)malloc16(2*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS);
    if (PHY_vars->chsch_data[n].CHSCH_conj_f){
#ifdef DEBUG_PHY
      printk("[openair][PHY][INIT] Allocated %d bytes for chsch receive filter %p\n",
	  2*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,
	  PHY_vars->chsch_data[n].CHSCH_conj_f );
#endif
  }
    else
      return(-1);

    PHY_vars->chsch_data[n].CHSCH_f            = (int *)malloc16(2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS);
    if (PHY_vars->chsch_data[n].CHSCH_f) {
#ifdef DEBUG_PHY
      printk("[openair][PHY][INIT] Allocated %d bytes for chsch transmit signal (%p)\n",
	  2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,PHY_vars->chsch_data[n].CHSCH_f);
#endif
    }
    else
      return(-1);
    


    for (i=0;i<4;i++) {
      PHY_vars->chsch_data[n].CHSCH_f_sync[i]       = (int *)malloc16(2*4*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS);
      if (PHY_vars->chsch_data[n].CHSCH_f_sync[i]) {
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch synch filter %d (%p)\n",
	    2*4*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,i,PHY_vars->chsch_data[n].CHSCH_f_sync[i]);
#endif
      }
      else
	return(-1);
    }

    for (i=0;i<NB_ANTENNAS_RX;i++) {

      PHY_vars->chsch_data[n].CHSCH_f_tx[i]    = (int *)malloc16(2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS);
      if (PHY_vars->chsch_data[n].CHSCH_f_tx[i]) {
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch transmit signal %d (%p)\n",
	    2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,i,PHY_vars->chsch_data[n].CHSCH_f_tx[i]);
#endif
      }
      else
	return(-1);

      PHY_vars->chsch_data[n].rx_sig_f[i]   = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_CHSCH_SYMBOLS));
      
      if (PHY_vars->chsch_data[n].rx_sig_f[i]) {
	
	if (((unsigned int)PHY_vars->chsch_data[n].rx_sig_f[i] & 0x0000000f) != 0)
	  PHY_vars->chsch_data[n].rx_sig_f[i] =  PHY_vars->chsch_data[n].rx_sig_f[i] + 2;
#ifdef DEBUG_PHY	
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch rx_sig_f, antenna %d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,
	    i,
	    PHY_vars->chsch_data[n].rx_sig_f[i]);
#endif
#ifndef USER_MODE

	reserve_mem(PHY_vars->chsch_data[n].rx_sig_f[i],
		    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS);
#endif // USER_MODE
      }
      else
	return(-1);

      PHY_vars->chsch_data[n].channel_f[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->chsch_data[n].channel_f[i]) {
	
	if (((unsigned int)PHY_vars->chsch_data[n].channel_f[i] & 0x0000000f) != 0)
	  PHY_vars->chsch_data[n].channel_f[i] =  PHY_vars->chsch_data[n].channel_f[i] + 4;
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch channel_f %d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,i,PHY_vars->chsch_data[n].channel_f[i]);
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch channel_f %d%d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,n,i,PHY_vars->chsch_data[n].channel_f[i]);
#ifndef USER_MODE

	reserve_mem(PHY_vars->chsch_data[n].channel_f[i],8*NUMBER_OF_OFDM_CARRIERS);
#endif
#endif //
      }
      else
	return(-1);

      PHY_vars->chsch_data[n].mag_channel_f[i]    = (int *)malloc16(4*NUMBER_OF_SACH_SYMBOLS_MAX*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->chsch_data[n].mag_channel_f[i]) {
	
	if (((unsigned int)PHY_vars->chsch_data[n].mag_channel_f[i] & 0x0000000f) > 0)
	  PHY_vars->chsch_data[n].mag_channel_f[i] = PHY_vars->chsch_data[n].mag_channel_f[i] + 4;
#ifdef DEBUG_PHY	
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch mag_channel_f (%p)\n",
	       32*NUMBER_OF_OFDM_CARRIERS,PHY_vars->chsch_data[n].mag_channel_f[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->chsch_data[n].mag_channel_f[i],32*NUMBER_OF_OFDM_CARRIERS);
#endif // USER_MODE
      }
      else
	return(-1);

      PHY_vars->chsch_data[n].channel_matched_filter_f[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->chsch_data[n].channel_matched_filter_f[i]) {
	
	if (((unsigned int)PHY_vars->chsch_data[n].channel_matched_filter_f[i] & 0x0000000f) != 0)
	  PHY_vars->chsch_data[n].channel_matched_filter_f[i] =  PHY_vars->chsch_data[n].channel_matched_filter_f[i] + 4;
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch channel_matched_filter_f %d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,i,PHY_vars->chsch_data[n].channel_matched_filter_f[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->chsch_data[n].channel_matched_filter_f[i],8*NUMBER_OF_OFDM_CARRIERS);
#endif //
      }
      else
	return(-1);

      PHY_vars->chsch_data[n].channel[i]    = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->chsch_data[n].channel[i]) {
	
	if (((unsigned int)PHY_vars->chsch_data[n].channel[i] & 0x0000000f) > 0)
	  PHY_vars->chsch_data[n].channel[i] = PHY_vars->chsch_data[n].channel[i] + 4;
#ifdef DEBUG_PHY	
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch channel %d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,i,PHY_vars->chsch_data[n].channel[i]);
#endif
#ifndef USER_MODE

	reserve_mem(PHY_vars->chsch_data[n].channel[i],8*NUMBER_OF_OFDM_CARRIERS);
#endif // USER_MODE
      }
      else
	return(-1);
    }

      PHY_vars->chsch_data[n].mag_channel    = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->chsch_data[n].mag_channel) {
	
	if (((unsigned int)PHY_vars->chsch_data[n].mag_channel & 0x0000000f) > 0)
	  PHY_vars->chsch_data[n].mag_channel = PHY_vars->chsch_data[n].mag_channel + 4;
#ifdef DEBUG_PHY	
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch mag_channel (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,PHY_vars->chsch_data[n].mag_channel);
#endif
#ifndef USER_MODE

	reserve_mem(PHY_vars->chsch_data[n].mag_channel,8*NUMBER_OF_OFDM_CARRIERS);
#endif // USER_MODE
      }
      else
	return(-1);



    for (i=0;i<4;i++) {

      PHY_vars->chsch_data[n].CHSCH_f_txr[i]    = (int *)malloc16(2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS);
      if (PHY_vars->chsch_data[n].CHSCH_f_txr[i]) {
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch transmit signal for receiver %d (%p)\n",
	    2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,i,PHY_vars->chsch_data[n].CHSCH_f_txr[i]);
#endif
      }
      else
	return(-1);
    }


    for (i=0;i<NB_ANTENNAS_TXRX;i++)
      for (j=0;j<NB_ANTENNAS_RX;j++) {
	PHY_vars->chsch_data[n].channel_f_interp[i][j]= (int*) malloc16(8*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS);
	if (PHY_vars->chsch_data[n].channel_f_interp[i][j]){
#ifdef DEBUG_PHY
	  printk("[openair][PHY][INIT] Allocated %d bytes for chsch channel_f_interp[%d][%d] (%p)\n",
		 8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,i,j,PHY_vars->chsch_data[n].channel_f_interp[i][j]);
#endif
	}
	else
	  return(-1);
      }

    for (i=0;i<NB_ANTENNAS_TXRX;i++)
      for (j=0;j<NB_ANTENNAS_RX;j++) {
	PHY_vars->chsch_data[n].channel_mmse_filter_f[i][j]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
	if (PHY_vars->chsch_data[n].channel_mmse_filter_f[i][j]) {
	
	  if (((unsigned int)PHY_vars->chsch_data[n].channel_mmse_filter_f[i][j] & 0x0000000f) != 0)
	    PHY_vars->chsch_data[n].channel_mmse_filter_f[i][j] =  PHY_vars->chsch_data[n].channel_mmse_filter_f[i][j] + 4;
#ifdef DEBUG_PHY
	  printk("[openair][PHY][INIT] Allocated %d bytes for chsch channel_mmse_filter_f[%d][%d] (%p)\n",
		 8*NUMBER_OF_OFDM_CARRIERS,i,j,PHY_vars->chsch_data[n].channel_mmse_filter_f[i][j]);
#endif
#ifndef USER_MODE
	  reserve_mem(PHY_vars->chsch_data[n].channel_mmse_filter_f[i][j],8*NUMBER_OF_OFDM_CARRIERS);
#endif //
	}
      else
	return(-1);
      }

    PHY_vars->chsch_data[n].det  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
    if (PHY_vars->chsch_data[n].det) {
      if (((unsigned int)PHY_vars->chsch_data[n].det & 0x0000000f) != 0)
	PHY_vars->chsch_data[n].det =  PHY_vars->chsch_data[n].det + 4;
#ifdef DEBUG_PHY
      printk("[openair][PHY][INIT] Allocated %d bytes for chsch det (%p)\n",
	     8*NUMBER_OF_OFDM_CARRIERS,PHY_vars->chsch_data[n].det);
#endif
    }

    PHY_vars->chsch_data[n].idet  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
    if (PHY_vars->chsch_data[n].idet) {
      if (((unsigned int)PHY_vars->chsch_data[n].idet & 0x0000000f) != 0)
	PHY_vars->chsch_data[n].idet =  PHY_vars->chsch_data[n].idet + 4;
#ifdef DEBUG_PHY
      printk("[openair][PHY][INIT] Allocated %d bytes for chsch idet (%p)\n",
	     8*NUMBER_OF_OFDM_CARRIERS,PHY_vars->chsch_data[n].idet);
#endif
    }
      
#ifdef DEBUG_PHY      
    printk("[openair][PHY][INIT] SCH %d:\n",n);
    printk("[openair][PHY][INIT] ******************\n");
#endif
    PHY_vars->sch_data[n].SCH_conj_f       = (int *)malloc16(2*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS);
    if (PHY_vars->sch_data[n].SCH_conj_f) {
#ifdef DEBUG_PHY
      printk("[openair][PHY][INIT] Allocated %d bytes for sch receive filter (%p)\n",
	     2*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS,
	     PHY_vars->sch_data[n].SCH_conj_f);

#endif
    }
    else
      return(-1);

    PHY_vars->sch_data[n].SCH_f            = (int *)malloc16(2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS);
    if (PHY_vars->sch_data[n].SCH_f) {
#ifdef DEBUG_PHY
      printk("[openair][PHY][INIT] Allocated %d bytes for sch transmit signal (%p)\n",
	  2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHSCH_SYMBOLS,PHY_vars->sch_data[n].SCH_f);
#endif
    }
    else
      return(-1);
    

    for (i=0;i<4;i++) {
      PHY_vars->sch_data[n].SCH_f_sync[i]       = (int *)malloc16(2*4*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS);
      if (PHY_vars->sch_data[n].SCH_f_sync[i]) {
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for chsch synch filter %d (%p)\n",
	    2*4*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS,i,PHY_vars->sch_data[n].SCH_f_sync[i]);
#endif
      }
      else
	return(-1);
    }

    for (i=0;i<NB_ANTENNAS_TX;i++) {
      PHY_vars->sch_data[n].SCH_f_tx[i]    = (int *)malloc16(2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS);
      if (PHY_vars->sch_data[n].SCH_f_tx[i]!=NULL){
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for sch transmit signal (%p), %d\n",
	    2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS,PHY_vars->sch_data[n].SCH_f_tx[i],i);
#endif
      }
      else
	return(-1);
    
      PHY_vars->sch_data[n].rx_sig_f[i]   = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_SCH_SYMBOLS));
      
      if (PHY_vars->sch_data[n].rx_sig_f[i]) {
	
	if (((unsigned int)PHY_vars->sch_data[n].rx_sig_f[i] & 0x0000000f) != 0)
	  PHY_vars->sch_data[n].rx_sig_f[i] =  PHY_vars->sch_data[n].rx_sig_f[i] + 2;
#ifdef DEBUG_PHY	
	printk("[openair][PHY][INIT] Allocated %d bytes for sch rx_sig_f, antenna %d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS,
	    i,
	    PHY_vars->sch_data[n].rx_sig_f[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->sch_data[n].rx_sig_f[i],
		    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS);
#endif // USER_MODE
      }
      else
	return(-1);
      
      PHY_vars->sch_data[n].channel_f[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sch_data[n].channel_f[i]) {
	
	if (((unsigned int)PHY_vars->sch_data[n].channel_f[i] & 0x0000000f) != 0)
	  PHY_vars->sch_data[n].channel_f[i] =  PHY_vars->sch_data[n].channel_f[i] + 4;
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for sch channel_f (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,PHY_vars->sch_data[n].channel_f[i]);
#ifndef USER_MODE
	reserve_mem(PHY_vars->sch_data[n].channel_f[i],8*NUMBER_OF_OFDM_CARRIERS);
#endif
#endif //
      }
      else
	return(-1);
      
      PHY_vars->sch_data[n].channel_matched_filter_f[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sch_data[n].channel_matched_filter_f[i]) {
	
	if (((unsigned int)PHY_vars->sch_data[n].channel_matched_filter_f[i] & 0x0000000f) != 0)
	  PHY_vars->sch_data[n].channel_matched_filter_f[i] =  PHY_vars->sch_data[n].channel_matched_filter_f[i] + 4;
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for sch channel_matched_filter_f (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,PHY_vars->sch_data[n].channel_matched_filter_f[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->sch_data[n].channel_matched_filter_f[i],8*NUMBER_OF_OFDM_CARRIERS);
#endif //
      }
      else
	return(-1);
      
      PHY_vars->sch_data[n].channel[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sch_data[n].channel[i]) {
	
	if (((unsigned int)PHY_vars->sch_data[n].channel[i] & 0x0000000f) != 0)
	  PHY_vars->sch_data[n].channel[i] =  PHY_vars->sch_data[n].channel[i] + 4;
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for sch channel (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,PHY_vars->sch_data[n].channel[i]);
#endif
#ifndef USER_MODE

	reserve_mem(PHY_vars->sch_data[n].channel[i],8*NUMBER_OF_OFDM_CARRIERS);
#endif //
      }
      else
	return(-1);
      
      PHY_vars->sch_data[n].mag_channel_f[i]    = (int *)malloc16(4*NUMBER_OF_SACH_SYMBOLS_MAX*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sch_data[n].mag_channel_f[i]) {
	
	if (((unsigned int)PHY_vars->sch_data[n].mag_channel_f[i] & 0x0000000f) > 0)
	  PHY_vars->sch_data[n].mag_channel_f[i] = PHY_vars->sch_data[n].mag_channel_f[i] + 4;
#ifdef DEBUG_PHY	
	printk("[openair][PHY][INIT] Allocated %d bytes for sch mag_channel_f (%p)\n",
	       32*NUMBER_OF_OFDM_CARRIERS,PHY_vars->sch_data[n].mag_channel_f[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->sch_data[n].mag_channel_f[i],32*NUMBER_OF_OFDM_CARRIERS);
#endif // USER_MODE
      }
      else
	return(-1);
    }

      PHY_vars->sch_data[n].mag_channel    = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sch_data[n].mag_channel) {
	
	if (((unsigned int)PHY_vars->sch_data[n].mag_channel & 0x0000000f) > 0)
	  PHY_vars->sch_data[n].mag_channel = PHY_vars->sch_data[n].mag_channel + 4;
#ifdef DEBUG_PHY	
	printk("[openair][PHY][INIT] Allocated %d bytes for sch mag_channel (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS,PHY_vars->sch_data[n].mag_channel);
#endif
#ifndef USER_MODE

	reserve_mem(PHY_vars->sch_data[n].mag_channel,8*NUMBER_OF_OFDM_CARRIERS);
#endif // USER_MODE
      }
      else
	return(-1);

    for (i=0;i<4;i++) {
      
      PHY_vars->sch_data[n].SCH_f_txr[i]    = (int *)malloc16(2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS);
      if (PHY_vars->sch_data[n].SCH_f_txr[i]) {
#ifdef DEBUG_PHY
	printk("[openair][PHY][INIT] Allocated %d bytes for sch transmit signal for receiver (%p), %d\n",
	       2*2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SCH_SYMBOLS,PHY_vars->sch_data[n].SCH_f_txr[i],i);
#endif
      }
      else
	return(-1);
    }

#ifdef DEBUG_PHY
    printk("[openair][PHY][INIT] CHBCH %d:\n",n);
    printk("[openair][PHY][INIT] ******************\n");
#endif

    PHY_vars->chbch_data[n].encoded_data[0]       = (unsigned char *)malloc16(2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
    
    if (PHY_vars->chbch_data[n].encoded_data[0]){
#ifdef DEBUG_PHY
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch encoded bits\n",
	  2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#endif
    }
    else
      return(-1);
#ifdef DEBUG_PHY    
    printk("[openair][PHY][INIT] Number of bytes for chbch PDU : %d\n", NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS/8);
#endif

  for (i=0;i<NB_ANTENNAS_TX;i++) {
      PHY_vars->chbch_data[n].fft_input[i]    = (int *)malloc16(NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_CHBCH_SYMBOLS);
      if (PHY_vars->chbch_data[n].fft_input[i]) {
	
	if (((unsigned int)PHY_vars->chbch_data[n].fft_input[i] & 0x0000000f) != 0)
	  PHY_vars->chbch_data[n].fft_input[i] =  PHY_vars->chbch_data[n].fft_input[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for chbch fft input, antenna %d (%p)\n",
	       NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_CHBCH_SYMBOLS,i,PHY_vars->chbch_data[n].fft_input[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->chbch_data[n].fft_input[i],NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_CHBCH_SYMBOLS);
#endif //
      }
      else
	return(-1);
    

  }
  for (i=0;i<NB_ANTENNAS_RX;i++){
    
    
    PHY_vars->chbch_data[n].rx_sig_f[i]   = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_CHBCH_SYMBOLS));
      
    if (PHY_vars->chbch_data[n].rx_sig_f[i]) {
	
      if (((unsigned int)PHY_vars->chbch_data[n].rx_sig_f[i] & 0x0000000f) != 0)
	PHY_vars->chbch_data[n].rx_sig_f[i] =  PHY_vars->chbch_data[n].rx_sig_f[i] + 2;
#ifdef DEBUG_PHY    	
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch rx_sig_f, antenna %d (%p)\n",
	     8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS,
	     i,
	     PHY_vars->chbch_data[n].rx_sig_f[i]);
#endif
#ifndef USER_MODE
      reserve_mem(PHY_vars->chbch_data[n].rx_sig_f[i],
		  8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#endif // USER_MODE
    }
    else
      return(-1);
    
    
    
    PHY_vars->chbch_data[n].rx_sig_f2[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_CHBCH_SYMBOLS));
    
    if (PHY_vars->chbch_data[n].rx_sig_f2[i]) {
      
      if (((unsigned int)PHY_vars->chbch_data[n].rx_sig_f2[i] & 0x0000000f) != 0)
	PHY_vars->chbch_data[n].rx_sig_f2[i] = PHY_vars->chbch_data[n].rx_sig_f2[i] +2;
#ifdef DEBUG_PHY    	
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch rx_sig_f2, antenna %d (%p)\n",
	     8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS,
	     i,
	     PHY_vars->chbch_data[n].rx_sig_f2[i]);
#endif
#ifndef USER_MODE
      reserve_mem(PHY_vars->chbch_data[n].rx_sig_f2[i],
		  8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#endif // USER_MODE
    }
    else
      return(-1);
    
    PHY_vars->chbch_data[n].rx_sig_f3[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_CHBCH_SYMBOLS));
    
    if (PHY_vars->chbch_data[n].rx_sig_f3[i]) {
      
      if (((unsigned int)PHY_vars->chbch_data[n].rx_sig_f3[i] & 0x0000000f) != 0)
	PHY_vars->chbch_data[n].rx_sig_f3[i] = PHY_vars->chbch_data[n].rx_sig_f3[i] +2;
#ifdef DEBUG_PHY    	
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch rx_sig_f3, antenna %d (%p)\n",
	     8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS,
	     i,
	     PHY_vars->chbch_data[n].rx_sig_f3[i]);
#endif
#ifndef USER_MODE
      reserve_mem(PHY_vars->chbch_data[n].rx_sig_f3[i],
		  8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#endif // USER_MODE
    }
    else
      return(-1);
    
    
  }
  
  PHY_vars->chbch_data[n].rx_sig_f4  = (short *)malloc16(2*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_CHBCH_SYMBOLS));
  
  if (PHY_vars->chbch_data[n].rx_sig_f4) {
    
    if (((unsigned int)PHY_vars->chbch_data[n].rx_sig_f4 & 0x0000000f) != 0)
      PHY_vars->chbch_data[n].rx_sig_f4 = PHY_vars->chbch_data[n].rx_sig_f4 +2;
#ifdef DEBUG_PHY          
    printk("[openair][PHY][INIT] Allocated %d bytes for chbch rx_sig_f4(%p)\n",
	   2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS,
	   PHY_vars->chbch_data[n].rx_sig_f4);
#endif
#ifndef USER_MODE
    reserve_mem(PHY_vars->chbch_data[n].rx_sig_f4,
		2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#endif // USER_MODE
    }
    else
	return(-1);
    

    PHY_vars->chbch_data[n].demod_data = (char *)malloc16(4*NUMBER_OF_CHBCH_SYMBOLS*NUMBER_OF_USEFUL_CARRIERS);  
    if (PHY_vars->chbch_data[n].demod_data) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch demod_data (%p)\n",
	  4*NUMBER_OF_CHBCH_SYMBOLS*NUMBER_OF_OFDM_CARRIERS,PHY_vars->chbch_data[n].demod_data);
#endif
    }
    else
      return(-1);
  
#ifdef DEBUG_PHY          
    printk("Number of bytes in CHBCH PDU : %d ( %d,%d)\n",NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS/8,
	NUMBER_OF_USEFUL_CARRIERS,
	NUMBER_OF_CHBCH_SYMBOLS);
#endif
    PHY_vars->chbch_data[n].demod_pdu = (unsigned char*)malloc16(8+(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS/8));
    if (PHY_vars->chbch_data[n].demod_pdu) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch demod_pdu (%p)\n",
	  8+(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS/8),PHY_vars->chbch_data[n].demod_pdu);
#endif
    }
    else
      return(-1);

    PHY_vars->chbch_data[n].tx_pdu[0] = (unsigned char*)malloc16(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS/8);
    if (PHY_vars->chbch_data[n].tx_pdu[0]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch tx_pdu (%p)\n",
	  NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS/8,PHY_vars->chbch_data[n].tx_pdu[0]);
#endif
    }
    else
      return(-1);


    for (i=0;i<NB_ANTENNAS_TXRX;i++)
      for (j=0;j<NB_ANTENNAS_RX;j++) {
	PHY_vars->chbch_data[n].channel_f_full[i][j]= (int*) malloc16(8*NUMBER_OF_CHBCH_SYMBOLS*NUMBER_OF_USEFUL_CARRIERS);
	if (PHY_vars->chbch_data[n].channel_f_full[i][j]) {
#ifdef DEBUG_PHY    
	  printk("[openair][PHY][INIT] Allocated %d bytes for chbch channel_f_full[%d][%d] (%p)\n",
		 8*NUMBER_OF_CHBCH_SYMBOLS*NUMBER_OF_OFDM_CARRIERS,i,j,PHY_vars->chbch_data[n].channel_f_full[i][j]);
#endif
	}
	else
	  return(-1);
      }
#ifdef DEBUG_PHY    
    printk("[openair][PHY][INIT] MRBCH %d:\n",n);
    printk("[openair][PHY][INIT] ******************\n");
#endif
    PHY_vars->mrbch_data[n].encoded_data[0]       = (unsigned char *)malloc16(2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
    
    if (PHY_vars->mrbch_data[n].encoded_data[0]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch encoded bits\n",
	  2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#endif
    }
    else
      return(-1);
#ifdef DEBUG_PHY        
    printk("[openair][PHY][INIT] Number of bytes for mrbch PDU : %d\n", NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS/8);
#endif

    PHY_vars->mrbch_data[n].interleaver_tx[0]       = (unsigned short *)malloc16(2*2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
    
    PHY_vars->mrbch_data[n].interleaver_rx[0]       = (unsigned short *)malloc16(2*2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
    
    if (PHY_vars->mrbch_data[n].interleaver_tx[0]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch interleaver\n",
	  2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#endif
    }
    else
      return(-1);
    
    if (PHY_vars->mrbch_data[n].interleaver_rx[0]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch interleaver\n",
	  2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#endif
    }
    else
      return(-1);
    
    PHY_vars->mrbch_data[n].pilot_indices     = (unsigned short *)malloc16(NUMBER_OF_USEFUL_CARRIERS);

    if (PHY_vars->mrbch_data[n].pilot_indices) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch pilot indices vector\n",
	  NUMBER_OF_USEFUL_CARRIERS);
#endif
    }
    else
      return(-1);
 
    for (i=0;i<NB_ANTENNAS_TX;i++) {
      PHY_vars->mrbch_data[n].fft_input[i]    = (int *)malloc16(NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_MRBCH_SYMBOLS);
      if (PHY_vars->mrbch_data[n].fft_input[i]) {
	
	if (((unsigned int)PHY_vars->mrbch_data[n].fft_input[i] & 0x0000000f) != 0)
	  PHY_vars->mrbch_data[n].fft_input[i] =  PHY_vars->mrbch_data[n].fft_input[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for mrbch fft input, antenna %d (%p)\n",
	       NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_MRBCH_SYMBOLS,i,PHY_vars->mrbch_data[n].fft_input[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->mrbch_data[n].fft_input[i],NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_MRBCH_SYMBOLS);
#endif //
      }
      else
	return(-1);
    
    }
    for (i=0;i<NB_ANTENNAS_RX;i++){
    
    
    
      PHY_vars->mrbch_data[n].rx_sig_f[i]   = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_MRBCH_SYMBOLS));
      
      if (PHY_vars->mrbch_data[n].rx_sig_f[i]) {
	
	if (((unsigned int)PHY_vars->mrbch_data[n].rx_sig_f[i] & 0x0000000f) != 0)
	  PHY_vars->mrbch_data[n].rx_sig_f[i] =  PHY_vars->mrbch_data[n].rx_sig_f[i] + 2;
#ifdef DEBUG_PHY    	
	printk("[openair][PHY][INIT] Allocated %d bytes for mrbch rx_sig_f, antenna %d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS,
	    i,
	    PHY_vars->mrbch_data[n].rx_sig_f[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->mrbch_data[n].rx_sig_f[i],
		    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#endif // USER_MODE
      }
      else
	return(-1);
      
      
      
      PHY_vars->mrbch_data[n].rx_sig_f2[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_MRBCH_SYMBOLS));
      
      if (PHY_vars->mrbch_data[n].rx_sig_f2[i]) {
	
	if (((unsigned int)PHY_vars->mrbch_data[n].rx_sig_f2[i] & 0x0000000f) != 0)
	  PHY_vars->mrbch_data[n].rx_sig_f2[i] = PHY_vars->mrbch_data[n].rx_sig_f2[i] +2;
#ifdef DEBUG_PHY    	
	printk("[openair][PHY][INIT] Allocated %d bytes for mrbch rx_sig_f2, antenna %d (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS,
	    i,
	    PHY_vars->mrbch_data[n].rx_sig_f2[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->mrbch_data[n].rx_sig_f2[i],
		    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#endif // USER_MODE
      }
      else
	return(-1);


    }
    
    PHY_vars->mrbch_data[n].rx_sig_f4  = (short *)malloc16(2*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_MRBCH_SYMBOLS));
    
    if (PHY_vars->mrbch_data[n].rx_sig_f4) {
      
      if (((unsigned int)PHY_vars->mrbch_data[n].rx_sig_f4 & 0x0000000f) != 0)
	PHY_vars->mrbch_data[n].rx_sig_f4 = PHY_vars->mrbch_data[n].rx_sig_f4 +2;
#ifdef DEBUG_PHY          
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch rx_sig_f4(%p)\n",
	     2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS,
	     PHY_vars->mrbch_data[n].rx_sig_f4);
#endif
#ifndef USER_MODE
      reserve_mem(PHY_vars->mrbch_data[n].rx_sig_f4,
		  2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#endif // USER_MODE
    }
    else
	return(-1);
    

    PHY_vars->mrbch_data[n].demod_data = (char *)malloc16(4*NUMBER_OF_MRBCH_SYMBOLS*NUMBER_OF_USEFUL_CARRIERS);  
    if (PHY_vars->mrbch_data[n].demod_data) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch demod_data (%p)\n",
	  4*NUMBER_OF_MRBCH_SYMBOLS*NUMBER_OF_OFDM_CARRIERS,PHY_vars->mrbch_data[n].demod_data);
#endif
    }
    else
      return(-1);
  
#ifdef DEBUG_PHY          
    printk("Number of bytes in MRBCH PDU : %d ( %d,%d)\n",NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS/8,
	NUMBER_OF_USEFUL_CARRIERS,
	NUMBER_OF_MRBCH_SYMBOLS);
#endif

    PHY_vars->mrbch_data[n].demod_pdu = (unsigned char *)malloc16(8+(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS/8));
    if (PHY_vars->mrbch_data[n].demod_pdu) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch demod_pdu (%p)\n",
	  8+(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS/8),PHY_vars->mrbch_data[n].demod_pdu);
#endif
    }
    else
      return(-1);

    PHY_vars->mrbch_data[n].tx_pdu[0] = (unsigned char*)malloc16(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS/8);
    if (PHY_vars->mrbch_data[n].tx_pdu[0]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for mrbch tx_pdu (%p)\n",
	  NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS/8,PHY_vars->mrbch_data[n].tx_pdu[0]);
#endif
    }
    else
      return(-1);

    //#ifndef USER_MODE
    //    rt_sleep(nano2count(100000000));
    //#endif
  
  }

  


  // INITIALIZE SACH
  for (n=0;n<NUMBER_OF_SACH;n++) {

#ifdef DEBUG_PHY    
    printk("[OPENAIR][PHY][INIT] ******************************\n");
    printk("[OPENAIR][PHY][INIT] SACH %d\n",n);
#endif

    for (tb=0;tb<MAX_NUM_TB;tb++) {
      PHY_vars->sach_data[n].encoded_data[tb]       = (unsigned char *)malloc16((MAX_TB_SIZE_BYTES+4)*16);
      
      if (PHY_vars->sach_data[n].encoded_data[tb]) {
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach encoded bits\n",
	       (MAX_TB_SIZE_BYTES+4)*6);
#endif
      }
      else {
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Could not allocate memory for sach encoded bits\n");
#endif
	return(-1);
      }
    }

    PHY_vars->sacch_data[n].encoded_data[0]       = (unsigned char *)malloc16(2*SACCH_SIZE_BITS);
    if (PHY_vars->sacch_data[n].encoded_data[0]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for sacch encoded bits\n",
	  2*SACCH_SIZE_BITS);
#endif
    }
    else {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Could not allocate memory for sacch encoded bits\n");
#endif
      return(-1);
    }

    for (i=0;i<NB_ANTENNAS_TX;i++) {
#ifdef DEBUG_PHY    
      printk("[OPENAIR][PHY][INIT] ******************************\n");
      printk("[OPENAIR][PHY][INIT] Antenna %d\n",i);      
#endif
      PHY_vars->sach_data[n].fft_input[i]    = (int *)malloc16(NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_SACH_SYMBOLS_MAX);
      if (PHY_vars->sach_data[n].fft_input[i]) {
	
	if (((unsigned int)PHY_vars->sach_data[n].fft_input[i] & 0x0000000f) != 0)
	  PHY_vars->sach_data[n].fft_input[i] =  PHY_vars->sach_data[n].fft_input[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach fft input (%p)\n",
	    NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_SACH_SYMBOLS_MAX,PHY_vars->sach_data[n].fft_input[i]);
#endif
#ifndef USER_MODE
	reserve_mem(PHY_vars->sach_data[n].fft_input[i],NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_SACH_SYMBOLS_MAX);
#endif //
      }
      else
	return(-1);
    }

    for (i=0;i<NB_ANTENNAS_RX;i++) {
      
      
      PHY_vars->sach_data[n].rx_sig_f[i]   = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
      if (PHY_vars->sach_data[n].rx_sig_f[i]) {
	if (((unsigned int)PHY_vars->sach_data[n].rx_sig_f[i] & 0x0000000f) != 0)
	  PHY_vars->sach_data[n].rx_sig_f[i] = PHY_vars->sach_data[n].rx_sig_f[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach rx_sig_f (%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX,
	    PHY_vars->sach_data[n].rx_sig_f[i]);
#endif
      }
      else
	return(-1);
      
      PHY_vars->sach_data[n].rx_sig_f2[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
      if (PHY_vars->sach_data[n].rx_sig_f2[i]) {
	
	if (((unsigned int)PHY_vars->sach_data[n].rx_sig_f2[i] & 0x0000000f) != 0)
	  PHY_vars->sach_data[n].rx_sig_f2[i] = PHY_vars->sach_data[n].rx_sig_f2[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach rx_sig_f2(%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX,
	    PHY_vars->sach_data[n].rx_sig_f2[i]);
#endif
      }
      else
	return(-1);

     PHY_vars->sach_data[n].rx_sig_f3[i]  = (int *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
      if (PHY_vars->sach_data[n].rx_sig_f3[i]) {
	
	if (((unsigned int)PHY_vars->sach_data[n].rx_sig_f3[i] & 0x0000000f) != 0)
	  PHY_vars->sach_data[n].rx_sig_f3[i] = PHY_vars->sach_data[n].rx_sig_f3[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach rx_sig_f3(%p)\n",
	    8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX,
	    PHY_vars->sach_data[n].rx_sig_f3[i]);
#endif
      }
      else
	return(-1);
    
  
      PHY_vars->sach_data[n].mag_channel_f_16qam[i]  = (int *)malloc16(4*NUMBER_OF_SACH_SYMBOLS_MAX*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sach_data[n].mag_channel_f_16qam[i]) {
	
	if (((unsigned int)PHY_vars->sach_data[n].mag_channel_f_16qam[i] & 0x0000000f) != 0)
	  PHY_vars->sach_data[n].mag_channel_f_16qam[i] = PHY_vars->sach_data[n].mag_channel_f_16qam[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach mag_channel_f_16qam(%p)\n",
	       8*NUMBER_OF_OFDM_CARRIERS,
	       PHY_vars->sach_data[n].mag_channel_f_16qam[i]);
#endif
      }
      else
	return(-1);

      PHY_vars->sach_data[n].mag_channel_f_64qama[i]  = (int *)malloc16(4*NUMBER_OF_SACH_SYMBOLS_MAX*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sach_data[n].mag_channel_f_64qama[i]) {
	
	if (((unsigned int)PHY_vars->sach_data[n].mag_channel_f_64qama[i] & 0x0000000f) != 0)
	  PHY_vars->sach_data[n].mag_channel_f_64qama[i] = PHY_vars->sach_data[n].mag_channel_f_64qama[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach mag_channel_f_64qam(%p)\n",
	       8*NUMBER_OF_OFDM_CARRIERS,
	       PHY_vars->sach_data[n].mag_channel_f_64qama[i]);
#endif
      }
      else
	return(-1);

      PHY_vars->sach_data[n].mag_channel_f_64qamb[i]  = (int *)malloc16(4*NUMBER_OF_SACH_SYMBOLS_MAX*NUMBER_OF_OFDM_CARRIERS);
      if (PHY_vars->sach_data[n].mag_channel_f_64qamb[i]) {
	
	if (((unsigned int)PHY_vars->sach_data[n].mag_channel_f_64qamb[i] & 0x0000000f) != 0)
	  PHY_vars->sach_data[n].mag_channel_f_64qamb[i] = PHY_vars->sach_data[n].mag_channel_f_64qamb[i] + 2;
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach mag_channel_f_64qam(%p)\n",
	       8*NUMBER_OF_OFDM_CARRIERS,
	       PHY_vars->sach_data[n].mag_channel_f_64qamb[i]);
#endif
      }
      else
	return(-1);
    }
  
    PHY_vars->sach_data[n].rx_sig_f4  = (short *)malloc16(2*NUMBER_OF_OFDM_CARRIERS*(NUMBER_OF_SACH_SYMBOLS_MAX));
    
    if (PHY_vars->sach_data[n].rx_sig_f4) {
      
      if (((unsigned int)PHY_vars->sach_data[n].rx_sig_f4 & 0x0000000f) != 0)
	PHY_vars->sach_data[n].rx_sig_f4 = PHY_vars->sach_data[n].rx_sig_f4 +2;
#ifdef DEBUG_PHY          
      printk("[openair][PHY][INIT] Allocated %d bytes for sach rx_sig_f4(%p)\n",
	     2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX,
	     PHY_vars->sach_data[n].rx_sig_f4);
#endif
#ifndef USER_MODE
      reserve_mem(PHY_vars->sach_data[n].rx_sig_f4,
		  2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
#endif // USER_MODE
    }
    else
	return(-1);
    /*
    PHY_vars->sach_data[n].rx_16qam_thres  = (short *)malloc16(2*NUMBER_OF_OFDM_CARRIERS);
    
    if (PHY_vars->sach_data[n].rx_16qam_thres) {
      
      if (((unsigned int)PHY_vars->sach_data[n].rx_16qam_thres & 0x0000000f) != 0)
	PHY_vars->sach_data[n].rx_16qam_thres = PHY_vars->sach_data[n].rx_16qam_thres +2;
#ifdef DEBUG_PHY          
      printk("[openair][PHY][INIT] Allocated %d bytes for chbch rx_16qam_thres(%p)\n",
	     2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX,
	     PHY_vars->sach_data[n].rx_16qam_thres);
#endif
#ifndef USER_MODE
      reserve_mem(PHY_vars->sach_data[n].rx_16qam_thres,
		  2*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
#endif // USER_MODE
    }
    else
	return(-1);
    */

    PHY_vars->sach_data[n].demod_data = (char *)malloc16(NUMBER_OF_SACH_SYMBOLS_MAX*NUMBER_OF_USEFUL_CARRIERS*12);  
    if (PHY_vars->sach_data[n].demod_data) {
#ifdef DEBUG_PHY          
      printk("[openair][PHY][INIT] Allocated %d bytes for sach demod_data\n",
	  NUMBER_OF_SACH_SYMBOLS_MAX*NUMBER_OF_USEFUL_CARRIERS*4);
#endif
    }
    else
      return(-1);

    PHY_vars->sacch_data[n].demod_data = (char *)malloc16(8*NUMBER_OF_USEFUL_CARRIERS);  
    if (PHY_vars->sacch_data[n].demod_data){
#ifdef DEBUG_PHY          
      printk("[openair][PHY][INIT] Allocated %d bytes for sacch demod_data\n",
	  8*NUMBER_OF_OFDM_CARRIERS);
#endif
    }
    else
      return(-1);
#ifdef DEBUG_PHY        
    printk("Number of bytes in SACH PDU (MAX) : %d ( %d,%d)\n",NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX,
	NUMBER_OF_USEFUL_CARRIERS,
	NUMBER_OF_SACH_SYMBOLS_MAX);
#endif

    PHY_vars->sach_data[n].demod_pdu = (unsigned char *)malloc16(8+(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX));
    if (PHY_vars->sach_data[n].demod_pdu) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for sach demod_pdu (%p)\n",
	  8+(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX),PHY_vars->sach_data[n].demod_pdu);
#endif
    }
    else
      return(-1);

    PHY_vars->sacch_data[n].demod_pdu = (unsigned char *)malloc16(SACCH_SIZE_BYTES+8);
    if (PHY_vars->sacch_data[n].demod_pdu) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for sacch demod_pdu (%p)\n",
	  8+SACCH_SIZE_BYTES,PHY_vars->sacch_data[n].demod_pdu);
#endif
    }
    else
      return(-1);
    for (tb=0;tb<MAX_NUM_TB;tb++) {
      PHY_vars->sach_data[n].tx_pdu[tb] = (unsigned char *)malloc16(NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX/8);
      if (PHY_vars->sach_data[n].tx_pdu[tb]) {
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach tx_pdu (%p)\n",
	       NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX/8,PHY_vars->sach_data[n].tx_pdu[tb]);
#endif
      }
      else
	return(-1);
    }
  

  
    PHY_vars->sacch_data[n].tx_pdu[0] = (unsigned char*)malloc16(SACCH_SIZE_BYTES);
    if (PHY_vars->sacch_data[n].tx_pdu[0]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Allocated %d bytes for sacch tx_pdu (%p)\n",
	  SACCH_SIZE_BYTES,PHY_vars->sacch_data[n].tx_pdu[0]);
#endif
    }
    else
      return(-1);
   
    //#ifndef USER_MODE
    //    rt_sleep(nano2count(100000000));
    //#endif

  }

#ifdef DEBUG_PHY    
  printk("[openair][PHY][INIT] Initializing SACH Diagnostics\n");
#endif
  
  for (n=0;n<4;n++) {

    for (i=0;i<5;i++) {

      PHY_vars->Sach_diagnostics[n][i].sach_demod_data = (short *)malloc16(8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);

      if (PHY_vars->Sach_diagnostics[n][i].sach_demod_data) {
#ifdef DEBUG_PHY    
	printk("[openair][PHY][INIT] Allocated %d bytes for sach_diagnostics[%d][%d] (%p)\n",
	       8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX,n,i,PHY_vars->Sach_diagnostics[n][i].sach_demod_data);
#endif

      PHY_vars->Sach_diagnostics[n][i].sacch_demod_data = (short *)malloc16(2*4*SACCH_SIZE_BITS);

      if (PHY_vars->Sach_diagnostics[n][i].sacch_demod_data) {
#ifdef DEBUG_PHY   
	printk("[openair][PHY][INIT] Allocated %d bytes for sacch_diagnostics[%d][%d] (%p)\n",
	       2*4*SACCH_SIZE_BITS,n,i,PHY_vars->Sach_diagnostics[n][i].sacch_demod_data);
#endif
    }
    else
      return(-1);
      }
    }
  }

#ifdef DEBUG_PHY    
  printk("[openair][PHY][INIT] Initializing FFT engine\n");
#endif

#ifndef EXPRESSMIMO_TARGET
  // Initialize fft variables
  init_fft(NUMBER_OF_OFDM_CARRIERS,LOG2_NUMBER_OF_OFDM_CARRIERS,rev);   // TX/RX
  init_fft(4*NUMBER_OF_OFDM_CARRIERS,2+LOG2_NUMBER_OF_OFDM_CARRIERS,rev_times4);   // Synch
  init_fft(NUMBER_OF_OFDM_CARRIERS/2,LOG2_NUMBER_OF_OFDM_CARRIERS-1,rev_half);   // for interpolation of channel est
#endif //EXPRESSMIMO_TARGET

#ifdef DEBUG_PHY    
  printk("[openair][PHY][INIT] Using %d point fft\n",NUMBER_OF_OFDM_CARRIERS);
#endif

  twiddle_fft = (short *)malloc16(4095*4*2);
  twiddle_ifft = (short *)malloc16(4095*4*2);
  twiddle_fft_times4 = (short*)malloc16(4095*4*2);
  twiddle_ifft_times4 = (short*)malloc16(4095*4*2);
  twiddle_fft_half = (short*)malloc16(4095*4*2);
  twiddle_ifft_half = (short*)malloc16(4095*4*2);

#ifdef DEBUG_PHY    
  printk("[openair][PHY][INIT] twiddle_fft= %p, twiddle_ifft=%p, twiddle_fft_times4=%p,twiddle_ifft_times4=%p\n",
	 (void *)twiddle_fft,(void *)twiddle_ifft,(void *)twiddle_fft_times4,(void *)twiddle_ifft_times4);
#endif

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
  default:
    memcpy(twiddle_fft,&twiddle_fft64[0],63*4*2);
    memcpy(twiddle_ifft,&twiddle_ifft64[0],63*4*2);
    memcpy(twiddle_fft_times4,&twiddle_fft256[0],255*4*2);
    memcpy(twiddle_ifft_times4,&twiddle_ifft256[0],255*4*2);
    //memcpy(twiddle_fft_half,&twiddle_fft32[0],31*4*2);
    //memcpy(twiddle_ifft_half,&twiddle_ifft32[0],31*4*2);
    break;
  }

/*
  for (n=0;n<PHY_config->total_no_chbch;n++){
	printk("[openair][PHY][INIT] Initializing CHBCH Interleaver\n");
	//  phy_interleaver_init_chbch(0);
	printk("[openair][PHY][INIT] Initializing CHBCH %d Pilot Positioning\n",n);
 	//phy_chbch_pilot_init(n,nb_antennas_tx);
  }
*/
  
  //  phy_sach_pilot_init();

/*
  for (i =0; i<8*2; i++) { //
    phy_interleaver_init_mch(i);
    phy_pilot_interleaver_init_mch(i);
  }
*/


  for (n=0;n<PHY_config->total_no_chsch;n++){
#ifdef DEBUG_PHY    
    printk("[openair][PHY][INIT] Initializing CHSCH Sequence %d\n",n);
#endif
    phy_chsch_init(n,nb_antennas_tx);
  }
	
  for (n=0;n<PHY_config->total_no_sch;n++){
#ifdef DEBUG_PHY    
    printk("[openair][PHY][INIT] Initializing SCH Sequence %d\n",n);
#endif
    phy_sch_init(n,nb_antennas_tx);
  }

#ifdef DEBUG_PHY    
  printk("[openair][PHY][INIT] Initializing Timing Acquisition Buffers\n");
#endif

  phy_synch_time_init();


  generate_sach_64qam_table();

  printk("[openair][PHY][INIT] Done\n");

  return(1);
}

void phy_cleanup(void) {

  int i,n,tb;
#ifndef USER_MODE
  unsigned int dummy_ptr;
#endif //USER_MODE
  // stop PHY_thread


  printk("[openair][PHY][INIT] cleanup\n");

  for (i=0;i<NB_ANTENNAS_RX;i++) {

#ifndef USER_MODE

    if (pci_buffer[2*i]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] pci_buffer %d\n",2*i);
#endif    
      bigfree(pci_buffer[2*i],FRAME_LENGTH_BYTES+2*PAGE_SIZE);
      //      free_pages(pci_buffer[2*i],8);
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Freed TX_DMA_BUFFER %d\n",i);
#endif
    }
#ifdef DEBUG_PHY    
    printk("[openair][PHY][INIT] pci_buffer %d\n",1+(2*i));
#endif
    if (pci_buffer[1+(2*i)]) {
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] pci_buffer %d\n",1+(2*i));
#endif
      dummy_ptr = virt_to_phys(pci_buffer[1+(2*i)]);
      bigfree(pci_buffer[1+(2*i)],FRAME_LENGTH_BYTES+2*PAGE_SIZE);

      
      //      free_pages(pci_buffer[1+(2*i)],8);
#ifdef DEBUG_PHY    
      printk("[openair][PHY][INIT] Freed RX_DMA_BUFFER %d\n",i);
#endif
    }
#else
    if (PHY_vars->tx_vars[i].TX_DMA_BUFFER) {
      free(PHY_vars->tx_vars[i].TX_DMA_BUFFER);
#ifdef DEBUG_PHY    
      printk("[openair][PHY] Freed PHY_vars->tx_vars[%d]\n",i);
#endif
    }
#endif // USER_MODE
  }
  
  //#ifndef USER_MODE

  if (PHY_vars) {

    for (n=0;n<8;n++) {

      if (PHY_vars->chbch_data[n].encoded_data[0]) {
	free16(PHY_vars->chbch_data[n].encoded_data[0],2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	printk("[openair][PHY] Free16d PHY_vars->chbch_data[%d].encoded_data\n",n);
#endif
      }
      for (i=0;i<NB_ANTENNAS_TX;i++){
	if (PHY_vars->chbch_data[n].fft_input[i]) {
	  free16(PHY_vars->chbch_data[n].fft_input[i],NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_CHBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->chbch_data[%d].fft_input[%d]\n",n,i);
#endif
	}      
      }
      for (i=0;i<NB_ANTENNAS_RX;i++){
	if (PHY_vars->chbch_data[n].rx_sig_f[i]) {
	  free16(PHY_vars->chbch_data[n].rx_sig_f[i],8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->chbch_data[%d].rx_sig_f\n",n);
#endif
	}
	if (PHY_vars->chbch_data[n].rx_sig_f2[i]) {
	  free16(PHY_vars->chbch_data[n].rx_sig_f2[i],8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_CHBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->chbch_data[%d].rx_sig_f2\n",n);
#endif
	}
      }

    if (PHY_vars->chbch_data[n].pilot_indices) {
	  free16(PHY_vars->chbch_data[n].pilot_indices,2*NUMBER_OF_USEFUL_CARRIERS);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->chbch_data[%d].pilot_indices\n",n);
#endif
	}
	
    if (PHY_vars->chbch_data[n].pilot) {
	  free16(PHY_vars->chbch_data[n].pilot,2*2*PHY_config->PHY_chbch[n].Npilot);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->chbch_data[%d].pilot\n",n);
#endif
    }  


   
      if (PHY_vars->mrbch_data[n].encoded_data[0]) {
	free16(PHY_vars->mrbch_data[n].encoded_data[0],2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	printk("[openair][PHY] Free16d PHY_vars->mrbch_data[%d].encoded_data\n",n);
#endif
      }

    
      for (i=0;i<NB_ANTENNAS_TX;i++){
	if (PHY_vars->mrbch_data[n].fft_input[i]) {
	  free16(PHY_vars->mrbch_data[n].fft_input[i],NUMBER_OF_OFDM_CARRIERS_BYTES*NUMBER_OF_MRBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->mrbch_data[%d].fft_input[%d]\n",n,i);
#endif
	}      
      }
      for (i=0;i<NB_ANTENNAS_RX;i++){
	if (PHY_vars->mrbch_data[n].rx_sig_f[i]) {
	  free16(PHY_vars->mrbch_data[n].rx_sig_f[i],8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->mrbch_data[%d].rx_sig_f\n",n);
#endif
	}
	if (PHY_vars->mrbch_data[n].rx_sig_f2[i]) {
	  free16(PHY_vars->mrbch_data[n].rx_sig_f2[i],8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_MRBCH_SYMBOLS);
#ifdef DEBUG_PHY    
	  printk("[openair][PHY] Free16d PHY_vars->mrbch_data[%d].rx_sig_f2\n",n);
#endif
	}
      }      
    }
    for (n=0;n<NUMBER_OF_SACH;n++) {
      for (tb=0;tb<MAX_NUM_TB;tb++)
	if (PHY_vars->sach_data[n].encoded_data[tb]) {
	  free16(PHY_vars->sach_data[n].encoded_data[tb],2*NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
	}
      if (PHY_vars->sach_data[n].demod_data) {
	free16(PHY_vars->sach_data[n].demod_data,32*NUMBER_OF_USEFUL_CARRIERS);
      }
      if (PHY_vars->sach_data[n].rx_sig_f) {
	free16(PHY_vars->sach_data[n].rx_sig_f,8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
      }
      if (PHY_vars->sach_data[n].rx_sig_f2) {
	free16(PHY_vars->sach_data[n].rx_sig_f2,8*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_SACH_SYMBOLS_MAX);
	printk("[openair][PHY] Free16d PHY_vars->sach[%d]\n",i);
      }
    }
        

  }
  //#endif //USER_MODE

#ifndef USER_MODE
  
  //this is already called in openair_cleanup()
  //openair_sched_cleanup();
  
#endif // USER_MODE

  printk("[openair][CLEANUP] Done!\n");
}

/*
 * @}*/
