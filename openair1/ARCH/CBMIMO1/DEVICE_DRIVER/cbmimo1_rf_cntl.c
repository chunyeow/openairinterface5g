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
#include "defs.h"
#include "extern.h"
#include "cbmimo1_device.h"
#include "cbmimo1_pci.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "SCHED/defs.h"
#include "SCHED/extern.h"


void openair_set_rx_rf_mode(unsigned char card_id,unsigned int arg) {

#ifndef NOCARD_TEST
  printk("[openair][RF_CNTL] Setting RX_RF MODE on card %d to %d\n",card_id,arg);

  openair_daq_vars.rx_rf_mode = arg;

  if (pci_interface[card_id]) 
    pci_interface[card_id]->rx_rf_mode  = arg;
  else
    printk("[openair][RF_CNTL] rx_rf_mode not configured\n");


  //  openair_dma(SET_RX_RF_MODE);
#endif

}





void openair_set_tcxo_dac(unsigned char card_id,unsigned int arg) {

#ifndef NOCARD_TEST  
  printk("[openair][RF_CNTL] Setting TCXO_DAC to %d\n",arg);

  openair_daq_vars.tcxo_dac = arg;
  if (pci_interface[card_id]) 
    pci_interface[card_id]->tcxo_dac = openair_daq_vars.tcxo_dac;
  else
    printk("[openair][RF_CNTL] TCXO_DAC not configured\n");

  //  openair_writel(arg,bar[0]+REG_BAR+0x4);
  // PA Gain control line is connected to TCXO tuning frequency input
  //  openair_dma(SET_PA_GAIN);
#endif

}

void openair_set_tx_gain_openair(unsigned char card_id,unsigned char txgain00,unsigned char txgain10,unsigned char txgain01, unsigned char txgain11) {

#ifndef NOCARD_TEST
  printk("[openair][RF_CNTL] Setting TX gains to %d,%d,%d,%d\n",txgain00,txgain10,txgain01,txgain11);

  if (pci_interface[card_id]) {
    pci_interface[card_id]->tx_gain00 = (unsigned int)txgain00;
    pci_interface[card_id]->tx_gain01 = (unsigned int)txgain01;
    pci_interface[card_id]->tx_gain10 = (unsigned int)txgain10;
    pci_interface[card_id]->tx_gain11 = (unsigned int)txgain11;
  }
  else
    printk("[openair][RF_CNTL] TX gains not configured\n");

  //  openair_writel((unsigned int)txgain00,bar[0]+REG_BAR+0x4);
  //  openair_writel((unsigned int)txgain10,bar[0]+REG_BAR+0x8);
  //  openair_writel((unsigned int)txgain01,bar[0]+REG_BAR+0xc);
  //  openair_writel((unsigned int)txgain11,bar[0]+REG_BAR+0x10);
  //  openair_dma(SET_TX_GAIN);
#endif

} 
 
void openair_set_rx_gain_openair(unsigned char card_id,unsigned char rxgain00,unsigned char rxgain01,unsigned char rxgain10,unsigned char rxgain11) {

#ifndef NOCARD_TEST
  unsigned int rxgain;

  // Concatenate the 4 gain values into one 32-bit register (flip byte endian)

  rxgain = rxgain00 | (rxgain01 << 8) | (rxgain10 << 16) | (rxgain11 << 24);
  printk("[openair][RF_CNTL] Setting RX gains to %d,%d,%d,%d -> %x\n",rxgain00,rxgain01,rxgain10,rxgain11,rxgain);

  // Store the result in shared PCI memory so that the FPGA can detect and read the new value
  openair_daq_vars.rx_gain_val  = rxgain;
  if (pci_interface[card_id]) {
    pci_interface[card_id]->rx_gain_val = openair_daq_vars.rx_gain_val;
    pci_interface[card_id]->rx_gain_cval = 0;
  }
  else
    printk("[openair][RF_CNTL] rxgainreg not configured\n");


#endif
}

void openair_set_rx_gain_cal_openair(unsigned char card_id,unsigned int gain_dB) {

#ifndef NOCARD_TEST

  //printk("[openair][RF_CNTL] Setting RX gains to %d dB \n",gain_dB);
  
  // Store the result in shared PCI memory so that the FPGA can detect and read the new value
  if (pci_interface[card_id]) {
    pci_interface[card_id]->rx_gain_cval  = gain_dB;
    pci_interface[card_id]->rx_gain_val = 0;
  }
  else
    printk("[openair][RF_CNTL] rxgainreg not configured\n");

#endif
}

void openair_set_lo_freq_openair(unsigned char card_id,char freq0,char freq1) {
#ifndef NOCARD_TEST
  printk("[openair][RF_CNTL] Setting LO frequencies to %d,%d\n",freq0,freq1);
  //  openair_writel(freq0,bar[0]+0x4);
  //  openair_writel(freq1,bar[0]+0x8);
  //  openair_dma(SET_LO_FREQ);
  openair_daq_vars.freq_info = 1 + (freq0<<1) + (freq1<<4);
  if (pci_interface[card_id]) 
    pci_interface[card_id]->freq_info = openair_daq_vars.freq_info;
  else
    printk("[openair][RF_CNTL] frequency not configures\n");

#endif

}

int openair_set_freq_offset(unsigned char card_id,int freq_offset) {
  unsigned int val;

  if (pci_interface[card_id]) {
      if (abs(freq_offset) > 7680000) {
	printk("[openair][RF_CNTL] Frequency offset must be smaller than 7.68e6!\n");
	return(-1);
      } else {
	val = (((unsigned int) abs(freq_offset))<<8)/1875; //abs(freq_offset)*pow2(20)/7.68e6
	if (freq_offset < 0)
	  val ^= (1<<20); // sign bit to position 20
	// bit21 = 0 negative freq offset at TX, positive freq offset at RX	    
	// bit21 = 1 positive freq offset at TX, negative freq offset at RX

	pci_interface[card_id]->freq_offset = val;
	//	printk("[openair][RF_CNTL] Setting frequency offset to %d Hz (%x)\n",freq_offset,val);
	//	printk("[openair][RF_CNTL] WARNING:  Setting frequency disabled!!!\n",freq_offset,val);
	return(0);
      }
  } 
  else {
    printk("[openair][RF_CNTL] pci_interface not initialized\n");
    return(-1);
  }

}





