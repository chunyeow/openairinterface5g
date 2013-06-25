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

void openair_sync(void) {
  int i;
  //int size,j;
  int status;
  int length;
  int ret;
  //  static unsigned char clear=1, clear2=1;
  //  int Nsymb, sync_pos, sync_pos_slot;
  //  int Ns;
  // int l;
  //  int rx_power;
  //  unsigned int adac_cnt;
  //  int pbch_decoded = 0;
  //  int frame_mod4,pbch_tx_ant;
  //  u8  dummy;

  //  RTIME time;

  LTE_DL_FRAME_PARMS *frame_parms = lte_frame_parms_g;

  openair_daq_vars.mode = openair_NOT_SYNCHED;

  //openair_set_lo_freq_openair(openair_daq_vars.freq,openair_daq_vars.freq);	


  // sleep during acquisition of frame

  /*
    time = rt_get_cpu_time_ns();

    for (i=0;i<3*NUMBER_OF_CHUNKS_PER_FRAME;i++) {
    rt_sleep(nano2count(NS_PER_SLOT/NUMBER_OF_OFDM_SYMBOLS_PER_SLOT));
    }
  
    time = rt_get_cpu_time_ns();

  */

  if (openair_daq_vars.one_shot_get_frame == 1) { // we're in a non real-time mode so just grab a frame and write to fifo
    if (sync_getting_frame == 0) {
      for (i=0;i<number_of_cards;i++) {
	ret = setup_regs(i,frame_parms);
	  
	openair_get_frame(i); //received frame is stored in PHY_vars->RX_DMA_BUFFER
      }
      sync_getting_frame = 1;
      sync_slot_cnt = 0;
    }
      
    if (sync_slot_cnt == 40) {  // wait 2 frames worth of samples
	
      msg("[PHY][SYNC] Getting frame, slot %d, adac %d\n",sync_slot_cnt,openair_get_mbox());
      status=rtf_reset(rx_sig_fifo);
	  
      for (i=0;i<NB_ANTENNAS_RX;i++) {
	length=rtf_put(rx_sig_fifo,(unsigned char*)RX_DMA_BUFFER[0][i],FRAME_LENGTH_BYTES);
	if (length < FRAME_LENGTH_BYTES) {
	  msg("[openair][sched][rx_sig_fifo_handler] Didn't put %d bytes for antenna %d (put %d)\n",FRAME_LENGTH_BYTES,i,length);
	}
	else {
	  msg("[openair][sched][rx_sig_fifo_handler] Worte %d bytes for antenna %d to fifo (put %d)\n",FRAME_LENGTH_BYTES,i,length);
	}
      }    
      
      // signal that acquisition is done in control fifo
      rtf_put(rf_cntl_fifo,&openair_daq_vars.sched_cnt,sizeof(int));
      openair_daq_vars.one_shot_get_frame = 0;

      sync_getting_frame = 0;
      sync_slot_cnt = 0;
    }
    sync_slot_cnt++;
  }     //(openair_daq_vars.one_shot_get_frame == 0)  

#ifdef OPENAIR1
  if ((openair_daq_vars.do_synch==1) && (openair_daq_vars.node_configured == 3)) {// the node has been cofigured as a UE and we do sync

    if (sync_getting_frame == 0) {
      ret = setup_regs(0,frame_parms);
      openair_get_frame(0); //received frame is stored in PHY_vars->RX_DMA_BUFFER
      sync_getting_frame = 1;
      sync_slot_cnt = 0;
    }
      
    if (sync_slot_cnt == 40) {  // wait 2 frames worth of samples

      //memcpy((void *)(RX_DMA_BUFFER[0][0]+FRAME_LENGTH_COMPLEX_SAMPLES),(void*)RX_DMA_BUFFER[0][0],OFDM_SYMBOL_SIZE_BYTES);
      //memcpy((void *)(RX_DMA_BUFFER[0][1]+FRAME_LENGTH_COMPLEX_SAMPLES),(void*)RX_DMA_BUFFER[0][1],OFDM_SYMBOL_SIZE_BYTES);
	
#ifdef DEBUG_PHY
      for (i=0;i<number_of_cards;i++) 
	msg("[openair][SCHED][SYNC] card %d freq %d:%d, RX_DMA ADR 0 %x, RX_DMA ADR 1 %x,  RX_GAIN_VAL %x, TDD_CONFIG %d, TCXO %d, NODE_ID %d\n",
	    (pci_interface[i]->freq_info>>1)&3,
	    (pci_interface[i]->freq_info>>3)&3,
	    pci_interface[i]->adc_head[0],
	    pci_interface[i]->adc_head[1],
	    //pci_interface[i]->ofdm_symbols_per_frame,
	    pci_interface[i]->rx_gain_val,
	    pci_interface[i]->tdd_config,
	    pci_interface[i]->tcxo_dac,
	    pci_interface[i]->node_id);        
	  
#endif

      msg("[openair][SCHED][SYNCH] starting sync\n");

      
      if (initial_sync(PHY_vars_UE_g[0]) == 0) {
		
	if (openair_daq_vars.node_running == 1) {
		  
	  for (i=0;i<number_of_cards;i++) 
	    pci_interface[i]->frame_offset = PHY_vars_UE_g[0]->rx_offset;
	  
	  openair_daq_vars.mode = openair_SYNCHED;
	  
	  /*
	    lte_adjust_synch(&PHY_vars_UE_g[0]->lte_frame_parms,
	    PHY_vars_UE_g[0],
	    0,
	    clear2,
	    16384);
	    if (clear2 == 1)
	    clear2 = 0;
	  */
	  
	  msg("[openair][SCHED][SYNCH] Starting RT aquisition\n");
	  for (i=0;i<number_of_cards;i++) 
	    openair_dma(i,FROM_GRLIB_IRQ_FROM_PCI_IS_ACQ_START_RT_ACQUISITION);
	  
	}
	
      }
      else {
	msg("[openair][SCHED][SYNCH] PBCH not decoded!\n");
      }
      
      /*
      // Do AGC
      if (openair_daq_vars.rx_gain_mode == DAQ_AGC_ON) {
      phy_adjust_gain(clear, 512, 0, PHY_vars_UE_g[0]);
      if (clear == 1)
      clear = 0;
      }
      */
      
      openair_daq_vars.do_synch=0;
      
      sync_getting_frame = 0;
      sync_slot_cnt = 0;
    }
    sync_slot_cnt++;
  }
#endif
}






//-----------------------------------------------------------------------------
/** MACPHY Thread */
static void * openair_thread(void *param) {
  //-----------------------------------------------------------------------------ddd

  //------------------------------
#ifndef USER_MODE
  struct sched_param p;
#endif // USER_MODE

  u8           next_slot, last_slot;
  unsigned int time_in,time_out,i;
  unsigned int aa,slot_offset, slot_offset_F;
  int diff;
  
  LTE_DL_FRAME_PARMS *frame_parms = lte_frame_parms_g;

  RTIME rt_time_in, rt_time_out, rt_diff;

  int rc;
  RTIME             tv;
  struct timespec   ts;

  // run on CPU 1, which should be reserved only for this (by adding isolcpus=1 noirqbalance to the kernel options). Also use IsolCpusMaks=0x2 when loading rtai_hal
  // rt_set_runnable_on_cpuid(pthread_self(),1);
  rt_sleep(nano2count(NS_PER_SLOT));


  p.sched_priority = OPENAIR_THREAD_PRIORITY;
  pthread_attr_setschedparam  (&attr_threads[OPENAIR_THREAD_INDEX], &p);
#ifndef RTAI_ISNT_POSIX
  pthread_attr_setschedpolicy (&attr_threads[OPENAIR_THREAD_INDEX], SCHED_FIFO);
#endif
  
  printk("[openair][SCHED][openair_thread] openair_thread started with id %x, fpu_flag = %x, cpuid = %d\n",(unsigned int)pthread_self(),pthread_self()->uses_fpu,rtai_cpuid());

  if (openair_daq_vars.is_eNB == 1) {
    msg("[openair][SCHED][openair_thread] Configuring openair_thread for primary clusterhead\n");
  }
  else {
    msg("[openair][SCHED][openair_thread] Configuring OPENAIR THREAD for regular node\n");
  }
  
#ifdef OPENAIR1
  lte_sync_time_init(frame_parms);
#endif

  openair_daq_vars.sync_state=0;

  exit_openair = 0;
  
  sync_slot_cnt = 0;
  sync_getting_frame = 0;

  // Inner thread endless loop
  // exits on error or normally
  
  openair_daq_vars.sched_cnt = 0;
  openair_daq_vars.instance_cnt = -1;

  for (i=0;i<number_of_cards;i++) 
    Zero_Buffer((void*)TX_DMA_BUFFER[i][0],
		4*SLOT_LENGTH_BYTES_NO_PREFIX);

  while (exit_openair == 0) {
    
    tv = rt_get_time();
    count2timespec(tv, &ts);
    ts.tv_nsec += 100000;
    
    rc = pthread_mutex_timedlock(&openair_mutex,&ts);
    if (rc==ETIMEDOUT) 
      msg("[SCHED][openair_thread] pthread_mutex_timedlock timed out, exiting\n");
    else if (rc != 0)
      msg("[SCHED][openair_thread] ERROR pthread_mutex_lock\n"); 
    else {
      //msg("[SCHED][openair_thread] Starting PHY processing for slot %d, frame %d\n",last_slot,mac_xface->frame);
      //msg("[SCHED][openair_thread] Locked openair_mutex, instance_cnt=%d\n",openair_daq_vars.instance_cnt);
        
      while (openair_daq_vars.instance_cnt < 0) {
	// wait for the irq handler to signal
	// this also unlocks the mutex
	// when signal is received, mutex is automatically locked again
	pthread_cond_wait(&openair_cond,&openair_mutex);
      }
      /*
      if (exit_openair==1) {
	msg("Exiting openair_thread ... \n");
	break;
      }
      */
      //msg("[SCHED][openair_thread] unlocking openair_mutex, instance_cnt=%d\n",openair_daq_vars.instance_cnt);
      if (pthread_mutex_unlock(&openair_mutex) !=0)
	msg("[SCHED][openair_thread] ERROR unlocking openair_mutex\n");	
    }

    next_slot = (openair_daq_vars.slot_count + 1 ) % LTE_SLOTS_PER_FRAME;
    if (openair_daq_vars.slot_count==0) 
      last_slot = LTE_SLOTS_PER_FRAME-1;
    else
      last_slot = (openair_daq_vars.slot_count - 1 ) % LTE_SLOTS_PER_FRAME; 

    //msg("[SCHED][Thread] Mode = %d (openair_NOT_SYNCHED=%d), slot_count = %d, instance_cnt = %d\n",openair_daq_vars.mode,openair_NOT_SYNCHED,openair_daq_vars.slot_count,openair_daq_vars.instance_cnt);

    if ((openair_daq_vars.mode != openair_NOT_SYNCHED) && (openair_daq_vars.node_running == 1)) {
      time_in = openair_get_mbox();
      rt_time_in = rt_get_time_ns();

#ifdef DEBUG_PHY
      //debug_msg("[SCHED][OPENAIR_THREAD] frame = %d, slot_count %d, last %d, next %d\n", mac_xface->frame, openair_daq_vars.slot_count, last_slot, next_slot);
#endif

#ifdef OPENAIR1
      if (openair_daq_vars.is_eNB==1) {
	if (PHY_vars_eNB_g && PHY_vars_eNB_g[0]) {
	  PHY_vars_eNB_g[0]->frame = openair_daq_vars.hw_frame;
	  phy_procedures_eNB_lte(last_slot,next_slot,PHY_vars_eNB_g[0],0);
#ifndef IFFT_FPGA
	  slot_offset_F = (next_slot)*
	    (PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size)*
	    ((PHY_vars_eNB_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7);
	  slot_offset = (next_slot)*
	    (PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti>>1);

	  for (aa=0; aa<PHY_vars_eNB_g[0]->lte_frame_parms.nb_antennas_tx; aa++) {
	    if (PHY_vars_eNB_g[0]->lte_frame_parms.Ncp == 1) {
	      
	      /*
	      if ((openair_daq_vars.hw_frame%100) == 0)
		msg("[SCHED][OPENAIR_THREAD] frame = %d, slot_offset_F %d, slot_offset %d, input %p, output %p, samples_per_tti %d\n", 
		    openair_daq_vars.hw_frame, 
		    slot_offset_F,
		    slot_offset,
		    &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F],
		    &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][slot_offset>>1],
		    PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti);
	      */
	      /*
	      for (i=0;i<6;i++) {
		memset(&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F+i*PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size],0,PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size*sizeof(int));
		((short*)(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa]))[(slot_offset_F+i*512+75)*2]=1024;
	      }
	      */
	      /*	      
	      for (i=0;i<PHY_vars_eNB_g[0]->lte_frame_parms.ofdm_symbol_size;i++)
		((short*)(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa]))[(slot_offset_F+i)*2]=i;
	      */
	      /*
	      for (i=0;i<PHY_vars_eNB_g[0]->lte_frame_parms.samples_per_tti>>1;i++)
		((char*)(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa]))[(slot_offset+i)*2] = (char) 0; //(i/30);
	      */

	      PHY_ofdm_mod(&PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa][slot_offset_F],        // input
#ifdef BIT8_TX
			   &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][slot_offset>>1],         // output
#else
			   &PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa][slot_offset],         // output
#endif
			   PHY_vars_eNB_g[0]->lte_frame_parms.log2_symbol_size,                // log2_fft_size
			   (PHY_vars_eNB_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7,                 // number of symbols
			   PHY_vars_eNB_g[0]->lte_frame_parms.nb_prefix_samples,               // number of prefix samples
			   PHY_vars_eNB_g[0]->lte_frame_parms.twiddle_ifft,  // IFFT twiddle factors
			   PHY_vars_eNB_g[0]->lte_frame_parms.rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);

	      if (((openair_daq_vars.hw_frame%1000)==0) && (next_slot==0) && (aa==0)) {
		/*
		for (i=0;i<511;i++) {
		  msg("twiddle_ifft(%d) = (%d, %d)\n", i, 
		      PHY_vars_eNB_g[0]->lte_frame_parms.twiddle_ifft[4*i], 
		      PHY_vars_eNB_g[0]->lte_frame_parms.twiddle_ifft[4*i+1]);
		}
		*/
		for (i=0;i<512;i++)
		  msg("X(%d)=(%d,%d), ",i,((short*)(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa]))[(slot_offset_F+i)*2],
			    ((short*)(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdataF[0][aa]))[(slot_offset_F+i)*2+1]);
		msg("\n");
		for (i=0;i<640;i++)
		  msg("x(%d)=(%d,%d), ",i,((char*)(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa]))[(slot_offset+i)*2],
			    ((char*)(PHY_vars_eNB_g[0]->lte_eNB_common_vars.txdata[0][aa]))[(slot_offset+i)*2+1]);
		msg("\n");
		//exit_openair=1;
	      }

	    }
	    else {
	      msg("[SCHED][OPENAIR_THREAD] Normal prefix not implemented yet!!!\n");
	    }
	  }
#endif //IFFT_FPGA
	}
      }
      else {
	if (PHY_vars_UE_g && PHY_vars_UE_g[0])
	  phy_procedures_UE_lte(last_slot,next_slot,PHY_vars_UE_g[0],0,0);
#ifndef IFFT_FPGA
	  slot_offset_F = (next_slot)*
	    (PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)*
	    ((PHY_vars_UE_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7);
	  slot_offset = (next_slot)*
	    (PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size + PHY_vars_UE_g[0]->lte_frame_parms.nb_prefix_samples)*
	    ((PHY_vars_UE_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7);
	  for (aa=0; aa<PHY_vars_UE_g[0]->lte_frame_parms.nb_antennas_tx; aa++) {
	    if (PHY_vars_UE_g[0]->lte_frame_parms.Ncp == 1) {
	      /*
	      debug_msg("[SCHED][OPENAIR_THREAD] frame = %d, slot_offset_F %d, slot_offset %d, input %p, output %p\n", 
			mac_xface->frame, 
			slot_offset_F,
			slot_offset,
			&PHY_vars_UE_g[0]->lte_ue_common_vars.txdataF[aa][slot_offset_F],
			&PHY_vars_UE_g[0]->lte_ue_common_vars.txdata[aa][slot_offset>>1]);
	      */
	      PHY_ofdm_mod(&PHY_vars_UE_g[0]->lte_ue_common_vars.txdataF[aa][slot_offset_F],        // input
#ifdef BIT8_TX
			   &PHY_vars_UE_g[0]->lte_ue_common_vars.txdata[aa][slot_offset>>1],         // output
#else
			   &PHY_vars_UE_g[0]->lte_ue_common_vars.txdata[aa][slot_offset],         // output
#endif
			   PHY_vars_UE_g[0]->lte_frame_parms.log2_symbol_size,                // log2_fft_size
			   (PHY_vars_UE_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7,                 // number of symbols
			   PHY_vars_UE_g[0]->lte_frame_parms.nb_prefix_samples,               // number of prefix samples
			   PHY_vars_UE_g[0]->lte_frame_parms.twiddle_ifft,  // IFFT twiddle factors
			   PHY_vars_UE_g[0]->lte_frame_parms.rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	    }
	    else {
	      msg("[SCHED][OPENAIR_THREAD] Normal prefix not implemented yet!!!\n");
	    }
	  }
#endif //IFFT_FPGA
      }
#endif //OPENAIR1

      time_out = openair_get_mbox();
      rt_time_out = rt_get_time_ns();


      diff = (((int) time_out) - ((int) time_in)) % ((int) 160);
      rt_diff = rt_time_out-rt_time_in;

      /*
      debug_msg("[SCHED][OPENAIR_THREAD] Frame %d: last_slot %d, macphy_scheduler time_in %d, time_out %d, diff %d, time_in %llu, time_out %llu, diff %llu\n", 
		mac_xface->frame, last_slot,
		time_in,time_out,diff,
		rt_time_in,rt_time_out,rt_diff);
      */
      if (rt_diff > 500000) 
	msg("[SCHED][OPENAIR_THREAD] last_slot %d, macphy_scheduler time_in %d, time_out %d, diff %d, time_in %llu, time_out %llu, diff %llu\n", 
	    last_slot,
	    time_in,time_out,diff,
	    rt_time_in,rt_time_out,rt_diff);
      

      /*
      if ((mac_xface->is_primary_cluster_head == 0) && (last_slot == 0)) {
	if (openair_daq_vars.first_sync_call == 1)
	  openair_daq_vars.first_sync_call = 0;
      }
      */
    } //  daq_mode != NOT_SYNCHED


    else {   // synchronization and get frame

      if ((openair_daq_vars.hw_frame%100==0)
	  && (openair_daq_vars.slot_count==0) 
	  && (openair_daq_vars.mode != openair_SYNCHED)) {
	msg("[SCHED][OPENAIR_THREAD] Frame %d: last_slot %d: calling openair_sync\n",
	    openair_daq_vars.hw_frame,openair_daq_vars.slot_count);
	openair_daq_vars.do_synch=1;
      }
      openair_sync();

    }

    // signal that PHY has finished by decreasing instance count
    // don't forget to protect access with mutex
    tv = rt_get_time();
    count2timespec(tv, &ts);
    ts.tv_nsec += 100000;

    rc = pthread_mutex_timedlock(&openair_mutex,&ts);
    if (rc==ETIMEDOUT) 
      msg("[SCHED][openair_thread] pthread_mutex_timedlock timed out, exiting\n");
    else if (rc != 0)
      msg("[SCHED][openair_thread] ERROR pthread_mutex_lock\n"); 
    else {
      openair_daq_vars.instance_cnt--;
      //msg("[SCHED][openair_thread] Finished PHY processing for slot %d, frame %d\n",last_slot,mac_xface->frame);
      // msg("[SCHED][openair_thread] unlocking openair_mutex, instance_cnt=%d\n",openair_daq_vars.instance_cnt);
      if (pthread_mutex_unlock(&openair_mutex) !=0)
	msg("[SCHED][openair_thread] ERROR unlocking openair_mutex\n");	
    }

  } // end while (1)
  
  // report what happened

  msg ("[SCHED][OPENAIR_THREAD] Exited : openair_daq_vars.slot_count = %d, MODE = %d\n", openair_daq_vars.slot_count, openair_daq_vars.mode);
  
  // rt_task_delete(rt_whoami);
  /*  pthread_exit(NULL); */
#ifdef SERIAL_IO
  rt_spclose(COM1);
#endif

  pthread_mutex_destroy(&openair_mutex);
  pthread_cond_destroy(&openair_cond);
 
  return(0);
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

  intr_in = 1;
  intr_cnt++;

  if (oai_semaphore && inst_cnt_ptr && lxrt_task) {
    rt_sem_wait(oai_semaphore);
    //if ((intr_cnt2%2000)<10) rt_printk("intr_cnt %d, inst_cnt_ptr %p, inst_cnt %d\n",intr_cnt,inst_cnt_ptr,*inst_cnt_ptr);
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

  if (vid != XILINX_VENDOR) { //CBMIMO1

    // check interrupt status register
    pci_read_config_word(pdev[0],6 , &irqval);
    
    if ((irqval&8) != 0)  {

      //msg("got interrupt for CBMIMO1, intr_cnt=%d, node_configured=%d\n",intr_cnt,openair_daq_vars.node_configured);

      // RESET PCI IRQ
      openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, FROM_GRLIB_BOOT_HOK|FROM_GRLIB_PCI_IRQ_ACK);
      openair_writel(pdev[0], FROM_GRLIB_CFG_GRPCI_EUR_CTRL_OFFSET, FROM_GRLIB_BOOT_HOK);
	
      if (openair_daq_vars.node_configured > 0) {
	
	adac_cnt = (*(unsigned int *)mbox);
	
	openair_daq_vars.slot_count=intr_cnt % LTE_SLOTS_PER_FRAME;
	//openair_daq_vars.slot_count=adac_cnt>>3;

	//if ((adac_cnt>>3) == 0)
	//if (((int) adac_cnt - (int) openair_daq_vars.last_adac_cnt)<0)    // This is a new frame
	if (openair_daq_vars.slot_count==0)
	  openair_daq_vars.hw_frame++;
	
	if (((openair_daq_vars.hw_frame %100) == 0) && (openair_daq_vars.hw_frame>0)) {
	  tv = rt_get_time_ns();
	  msg("[SCHED][slot_irq_handler] time %llu, interrupt count %d, adac %d, last %d, HW Frame %d, slot %d\n",
	      tv,intr_cnt,adac_cnt,openair_daq_vars.last_adac_cnt,openair_daq_vars.hw_frame,openair_daq_vars.slot_count);
	}
	
	openair_daq_vars.last_adac_cnt=adac_cnt;
	
	/*
	  if ((openair_daq_vars.hw_frame %100) == 0)
	  printk("[SCHED][slot_irq_handler] Current HW Frame %d, interrupt cnt %d\n",openair_daq_vars.hw_frame,intr_cnt);
	*/
	
	
	tv = rt_get_time();
	count2timespec(tv, &ts);
	ts.tv_nsec += 100000;
	
	if (exit_openair==0) { 
	  // Schedule openair_thread
	  
	  //first lock the mutex that protects the counter that indicates if PHY is still busy 
	  //rc = pthread_mutex_lock (&openair_mutex); // lock before accessing shared resource
	  rc = pthread_mutex_timedlock (&openair_mutex,&ts); // lock before accessing shared resource
	  if (rc==ETIMEDOUT) {
	    msg("[SCHED][slot_irq_handler] pthread_mutex_timedlock timed out, exiting\n");
	    //exit_openair = 1;
	  }
	  else if (rc != 0) {
	    msg("[SCHED][slot_irq_handler] ERROR pthread_mutex_lock, exiting\n");
	    //exit_openair = 1;
	  }
	  else {
	    // we locked the mutex successfully, so now we can check its value
	    //msg("[SCHED][slot_irq_handler] Locked openair_mutex, instance_cnt=%d\n",openair_daq_vars.instance_cnt);
	    
	    if (openair_daq_vars.instance_cnt == 0)   {// PHY is still busy
	      msg("[SCHED][slot_irq_handler] ERROR slot interrupt while processing, instance_cnt=%d\n",
		  openair_daq_vars.instance_cnt);
	      
	      // unlock the mutex
	      if (pthread_mutex_unlock (&openair_mutex) != 0) {
		msg("[SCHED][slot_irq_handler] ERROR pthread_mutex_unlock\n");
		//exit_openair=1;
	      }
	    }
	    else { // schedule L2/L1H TX thread
	      openair_daq_vars.instance_cnt++; //now it should be 0
	      
	      // Signal MAC_PHY Scheduler
	      /*
	      if ((openair_daq_vars.is_eNB==1) && 
		  (PHY_vars_eNB_g[0]->frame<100))
		msg("[SCHED][slot_irq_handler] Signaling eNB MACPHY scheduler for slot %d\n",openair_daq_vars.slot_count);
	      else if ((openair_daq_vars.is_eNB==0) && 
		       (PHY_vars_UE_g[0]->frame<100))
		msg("[SCHED][slot_irq_handler] Signaling UE MACPHY scheduler for slot %d\n",openair_daq_vars.slot_count);
	      */

	      // unlock the mutex
	      if (pthread_mutex_unlock (&openair_mutex) != 0) {
		msg("[SCHED][slot_irq_handler] ERROR pthread_mutex_unlock\n");
		//exit_openair=1;
	      }
	      
	      if (pthread_cond_signal(&openair_cond) != 0) {
		msg("[SCHED][slot_irq_handler] ERROR pthread_cond_signal\n");// schedule L2/L1H TX thread
		//exit_openair = 1;
	      }
	      
	    }
	  }
	}
      } // node_configured > 0
      rt_ack_irq(irq);
      rt_unmask_irq(irq);
      rt_enable_irq(irq);
      intr_in = 0;
      return IRQ_HANDLED;
      
    } 
    else {  // CBMIMO is not source of interrupt
      
      rt_pend_linux_irq(irq);
      intr_in = 0;
      return IRQ_NONE;
    }
       
    // CBMIMO1 is not activated yet (no interrupts!)
    rt_pend_linux_irq(irq);
    intr_in = 0;
    return IRQ_NONE;
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
      }
      else if (irqcmd == PCI_PRINTK) {
	//	msg("Got PCIe interrupt for printk ...\n");
	pci_fifo_printk();
	
      }
      else if (irqcmd == GET_FRAME_DONE) {
	msg("Got PCIe interrupt for GET_FRAME_DONE ...\n");
	openair_daq_vars.get_frame_done=1;
      }
      rt_ack_irq(irq);
      rt_unmask_irq(irq);
      rt_enable_irq(irq);
      intr_in = 0;
      return IRQ_HANDLED;
    }
    else {
      // RESET PCI IRQ
      rt_pend_linux_irq(irq);
      intr_in = 0;
      return IRQ_NONE;
    }
  }

}




s32 openair_sched_init(void) {


  int error_code;

  int* tmp;
  
  LTE_DL_FRAME_PARMS *frame_parms = lte_frame_parms_g;
  
  
  openair_daq_vars.scheduler_interval_ns=NS_PER_SLOT;        // initial guess
  
  openair_daq_vars.last_adac_cnt=-1;            
  
  exit_openair=0;
  
  pthread_mutex_init(&openair_mutex,NULL);
  
  pthread_cond_init(&openair_cond,NULL);
  
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



 
  pthread_attr_init (&attr_threads[OPENAIR_THREAD_INDEX]);
  pthread_attr_setstacksize(&attr_threads[OPENAIR_THREAD_INDEX],OPENAIR_THREAD_STACK_SIZE);
  
  attr_threads[OPENAIR_THREAD_INDEX].priority = 1;
  
  openair_daq_vars.instance_cnt = -1;

  // Create openair_thread
  error_code = pthread_create(&threads[OPENAIR_THREAD_INDEX],
  			      &attr_threads[OPENAIR_THREAD_INDEX],
  			      openair_thread,
  			      (void *)0);
  

  if (error_code!= 0) {
    printk("[OPENAIR][SCHED][INIT] Could not allocate openair_thread, error %d\n",error_code);
    return(error_code);
  }
  else {
    printk("[OPENAIR][SCHED][INIT] Allocate openair_thread successful\n");
  }

  // Create interrupt service routine for PCI 
   

  
  

  error_code = rtf_create(rx_sig_fifo, NB_ANTENNAS_RX*FRAME_LENGTH_BYTES);
  printk("[openair][SCHED][INIT] Created rx_sig_fifo (%d bytes), error_code %d\n",
      NB_ANTENNAS_RX*FRAME_LENGTH_BYTES,
      error_code); 


  //if (mac_xface->is_cluster_head == 0) 
  //FK mac_xface->is_cluster_head not initialized at this stage
  //  error_code = init_dlsch_threads();

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


