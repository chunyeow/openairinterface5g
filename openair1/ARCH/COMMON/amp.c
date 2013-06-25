#include "SCHED/defs.h"
#include "SCHED/extern.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"

#ifdef PLATON
#include "daq.h"
#include "daq_vars_extern.h"
#endif PLATON

void openair_set_tx_gain(int ant_index,int gain_dB) {

#ifdef PLATON

  writegain (TX_PA_GAIN, 
	     inv_tx_gain_table[ant_index][gain_dB + 120 + 200],
	     ant_index);

  rt_busy_sleep ((DAQ_PCI_WRITE_DELAY_NS));

  writegain (TX_MED_GAIN, 
	     inv_tx_gain_table[ant_index][gain_dB + 120 + 100],
	     ant_index);
  rt_busy_sleep ((DAQ_PCI_WRITE_DELAY_NS));

  if (mac_xface->frame% 100 == 0)
     msg("[OPENAIR][RF][TX_GAIN] ant %d: PA->%d,MED->%d\n",ant_index,inv_tx_gain_table[ant_index][gain_dB + 120 + 200],inv_tx_gain_table[ant_index][gain_dB + 120 + 100]);
  
#endif

}

void openair_set_rx_gain(int ant_index,int gain_dB) {

#ifdef PLATON

        writegain (RX_IF_GAIN, 
		   inv_rx_gain_table[ant_index][gain_dB], 
		   ant_index);
        rt_busy_sleep ((DAQ_PCI_WRITE_DELAY_NS));
#endif

}

void openair_set_rx_gain_cal_openair(unsigned int gain_dB) {

#ifndef NOCARD_TEST
#ifndef PLATON

  //  printk("[openair][RF_CNTL] Setting RX gains to %d dB \n",gain_dB);



  
  // Store the result in shared PCI memory so that the FPGA can detect and read the new value
  if (pci_interface) {
		pci_interface->rx_gain_cval  = gain_dB;
  }
  else
    printk("[openair][RF_CNTL] rxgainreg not configured\n");

#endif //PLATON
#endif
}

#ifndef USER_MODE
EXPORT_SYMBOL(openair_set_tx_gain);
EXPORT_SYMBOL(openair_set_rx_gain);
EXPORT_SYMBOL(openair_set_rx_gain_cal_openair);
#endif USER_MODE
