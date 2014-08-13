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
#ifndef USER_MODE
#define __NO_VERSION__

//#include "rt_compat.h"

#endif

#include "cbmimo1_device.h"
#include "defs.h"
#include "extern.h"
#include "cbmimo1_pci.h"
#include "SCHED/defs.h"
#include "SCHED/extern.h"

#include "from_grlib_softconfig.h"
#include "from_grlib_softregs.h"

#ifdef RTAI_ENABLED
#include "PHY/defs.h"
#include "PHY/extern.h"
#endif //RTAI_ENABLED

/*
#ifdef RTAI_ENABLED
//------------------------------------------------------------------------------
int create_rt_fifo(int rt_fifoP, int sizeP) {
  //------------------------------------------------------------------------------
  rtf_destroy(rt_fifoP);
  switch (rtf_create(rt_fifoP, sizeP)) {
  case -ENODEV:
    printk("[WCDMA][ERROR] create_rt_fifo() %d fifo is greater than or equal to RTF_NO\n", rt_fifoP);
    return  -ENODEV;
    break;
  case -EBUSY:
    printk("[WCDMA][ERROR] create_rt_fifo() %d fifo is already in use. Choose a different ID\n", rt_fifoP);
    return  -EBUSY;
    break;
  case -ENOMEM:
    printk("[WCDMA][ERROR] create_rt_fifo() %d bytes could not be allocated for the RT-FIFO %d\n", sizeP, rt_fifoP);
    return  -ENOMEM;
    break;
  case 0:
    printk("[WCDMA] RT-FIFO %d CREATED\n", rt_fifoP);
    rtf_flush(rt_fifoP);
    return rt_fifoP; // not necessary, but...
    break;
  default:
    printk("[WCDMA] create_rt_fifo() returned ???\n");
    return -1;
  }
}

#endif //RTAI_ENABLED
*/

//------------------------------------------------------------------------------
int setup_regs(unsigned char card_id, LTE_DL_FRAME_PARMS *frame_parms) {

  //------------------------------------------------------------------------------


  int i;

#ifdef RTAI_ENABLED
  
#ifndef NOCARD_TEST    


  if (vid != XILINX_VENDOR) {
    
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      pci_interface[card_id]->adc_head[i] = (unsigned int)virt_to_phys((volatile void*)RX_DMA_BUFFER[card_id][i]);
    }
    for (i=0;i<NB_ANTENNAS_TX;i++){
      pci_interface[card_id]->dac_head[i] = (unsigned int)virt_to_phys((volatile void*)TX_DMA_BUFFER[card_id][i]);
    }
    
#ifndef FW2011
    pci_interface[card_id]->ofdm_symbols_per_frame = NUMBER_OF_SYMBOLS_PER_FRAME;
    //printk("[openair][INIT] NUMBER_OF_SYMBOLS_PER_FRAME = %d\n",pci_interface[card_id]->ofdm_symbols_per_frame);
    pci_interface[card_id]->log2_ofdm_symbol_size = LOG2_NUMBER_OF_OFDM_CARRIERS; 
    pci_interface[card_id]->cyclic_prefix_length  = CYCLIC_PREFIX_LENGTH;
    //printk("[openair][INIT] CYCLIC_PREFIX_LENGTH = %d\n",card_id,pci_interface[card_id]->cyclic_prefix_length);
#endif
    
    pci_interface[card_id]->samples_per_frame = FRAME_LENGTH_COMPLEX_SAMPLES;
    printk("[openair][INIT] samples_per_frame = %d\n",pci_interface[card_id]->samples_per_frame);
    
#ifndef FW2011
    pci_interface[card_id]->tx_rx_switch_point = openair_daq_vars.tx_rx_switch_point;
#else
    pci_interface[card_id]->tdd_config = frame_parms->tdd_config;
#endif
    
    pci_interface[card_id]->timing_advance = openair_daq_vars.timing_advance;
    
    pci_interface[card_id]->dual_tx = frame_parms->dual_tx;
    pci_interface[card_id]->tdd     = frame_parms->frame_type;
    pci_interface[card_id]->node_id = frame_parms->node_id;
    printk("[openair][INIT] node_id %d, dual_tx %d, tdd %d, tdd_config %d\n",frame_parms->node_id, frame_parms->dual_tx, frame_parms->frame_type, frame_parms->tdd_config );

    
    pci_interface[card_id]->freq_info = openair_daq_vars.freq_info;
    //printk("[openair][INIT] freq0 = %d, freq1 = %d\n",(pci_interface[card_id]->freq_info>>1)&3,(pci_interface[card_id]->freq_info>>3)&3);
    
    pci_interface[card_id]->rx_rf_mode = openair_daq_vars.rx_rf_mode;
    
    pci_interface[card_id]->rx_gain_val = openair_daq_vars.rx_gain_val;
    
    pci_interface[card_id]->tcxo_dac = openair_daq_vars.tcxo_dac;
    
    pci_interface[card_id]->mast_flag = (card_id==0)? 1 : 0;
    
  }
  else {
    
    exmimo_pci_interface->framing.eNB_flag   = (frame_parms->node_id==0) ?  1 : 0;
    exmimo_pci_interface->framing.tdd        = frame_parms->frame_type;
    exmimo_pci_interface->framing.tdd_config = frame_parms->tdd_config;
    printk("exmimo_pci_interface->frameing.eNB_flag = %d\n",exmimo_pci_interface->framing.eNB_flag);
    for (i=0;i<NB_ANTENNAS_RX;i++) {
      exmimo_pci_interface->rf.adc_head[i] = (unsigned int)virt_to_phys((volatile void*)RX_DMA_BUFFER[card_id][i]);
      printk("exmimo_pci_interface->rf.adc_head[%d] = %x\n",i,exmimo_pci_interface->rf.adc_head[i]);
    }
    for (i=0;i<NB_ANTENNAS_TX;i++){
      exmimo_pci_interface->rf.dac_head[i] = (unsigned int)virt_to_phys((volatile void*)TX_DMA_BUFFER[card_id][i]);
    }

    printk("Freq %d,%d,%d,%d, Gain %d,%d,%d,%d, RFmode %d, RXDC %d, RF_local %d, rf_vcocal %d\n",
	   frame_parms->carrier_freq[0],frame_parms->carrier_freq[1],frame_parms->carrier_freq[2],frame_parms->carrier_freq[3],
	   frame_parms->rxgain[0],frame_parms->rxgain[1],frame_parms->rxgain[2],frame_parms->rxgain[3],
	   frame_parms->rfmode[0],frame_parms->rflocal[0],
	   frame_parms->rxdc[0],frame_parms->rfvcolocal[0]);
    exmimo_pci_interface->rf.rf_freq_rx0          = frame_parms->carrier_freq[0];
    exmimo_pci_interface->rf.rf_freq_tx0          = frame_parms->carrier_freqtx[0];
    exmimo_pci_interface->rf.rx_gain00            = frame_parms->rxgain[0];
    exmimo_pci_interface->rf.tx_gain00            = frame_parms->txgain[0];
    exmimo_pci_interface->rf.rf_freq_rx1          = frame_parms->carrier_freq[1];
    exmimo_pci_interface->rf.rf_freq_tx1          = frame_parms->carrier_freqtx[1];
    exmimo_pci_interface->rf.rx_gain10            = frame_parms->rxgain[1];
    exmimo_pci_interface->rf.tx_gain10            = frame_parms->txgain[1];
    exmimo_pci_interface->rf.rf_freq_rx2          = frame_parms->carrier_freq[2];
    exmimo_pci_interface->rf.rf_freq_tx2          = frame_parms->carrier_freqtx[2];
    exmimo_pci_interface->rf.rx_gain20            = frame_parms->rxgain[2];
    exmimo_pci_interface->rf.tx_gain20            = frame_parms->txgain[2];
    exmimo_pci_interface->rf.rf_freq_rx3          = frame_parms->carrier_freq[3];
    exmimo_pci_interface->rf.rf_freq_tx3          = frame_parms->carrier_freqtx[3];
    exmimo_pci_interface->rf.rx_gain30            = frame_parms->rxgain[3];
    exmimo_pci_interface->rf.tx_gain30            = frame_parms->txgain[3];
    exmimo_pci_interface->rf.rf_mode0             = frame_parms->rfmode[0];
    exmimo_pci_interface->rf.rf_local0            = frame_parms->rflocal[0];
    exmimo_pci_interface->rf.rf_rxdc0             = frame_parms->rxdc[0];
    exmimo_pci_interface->rf.rf_vcocal0           = frame_parms->rfvcolocal[0];
    exmimo_pci_interface->rf.rf_mode1             = frame_parms->rfmode[1];
    exmimo_pci_interface->rf.rf_local1            = frame_parms->rflocal[1];
    exmimo_pci_interface->rf.rf_rxdc1             = frame_parms->rxdc[1];
    exmimo_pci_interface->rf.rf_vcocal1           = frame_parms->rfvcolocal[1];
    exmimo_pci_interface->rf.rf_mode2             = frame_parms->rfmode[2];
    exmimo_pci_interface->rf.rf_local2            = frame_parms->rflocal[2];
    exmimo_pci_interface->rf.rf_rxdc2             = frame_parms->rxdc[2];
    exmimo_pci_interface->rf.rf_vcocal2           = frame_parms->rfvcolocal[2];
    exmimo_pci_interface->rf.rf_mode3             = frame_parms->rfmode[3];
    exmimo_pci_interface->rf.rf_local3            = frame_parms->rflocal[3];
    exmimo_pci_interface->rf.rf_rxdc3             = frame_parms->rxdc[3];
    exmimo_pci_interface->rf.rf_vcocal3           = frame_parms->rfvcolocal[3];
  }
#endif // RTAI_ENABLED
    
  //  printk("[openair][INIT] : Returning\n");
  return(0);
#else //NOCARD_TEST
  return(0);
#endif //NOCARD_TEST
}
