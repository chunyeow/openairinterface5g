#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "MAC_INTERFACE/defs.h"
#include "MAC_INTERFACE/extern.h"

#ifdef EXMIMO
#ifdef DRIVER2013
#include "openair0_lib.h"
extern int card;
#else
#include "ARCH/CBMIMO1/DEVICE_DRIVER/cbmimo1_device.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/defs.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/extern.h"
#endif
#endif

void
phy_adjust_gain (PHY_VARS_UE *phy_vars_ue, uint8_t eNB_id) {

    uint16_t rx_power_fil_dB;
#ifdef EXMIMO
#ifdef DRIVER2013
  exmimo_config_t *p_exmimo_config = openair0_exmimo_pci[card].exmimo_config_ptr;
#endif
#endif
#if defined(EXMIMO) || defined(CBMIMO1)
  uint16_t i;
#endif

  //rx_power_fil_dB = dB_fixed(phy_vars_ue->PHY_measurements.rssi);
  rx_power_fil_dB = phy_vars_ue->PHY_measurements.rx_power_avg_dB[eNB_id];

  // Gain control with hysterisis
  // Adjust gain in phy_vars_ue->rx_vars[0].rx_total_gain_dB

  if (rx_power_fil_dB < TARGET_RX_POWER - 5) //&& (phy_vars_ue->rx_total_gain_dB < MAX_RF_GAIN) )
    phy_vars_ue->rx_total_gain_dB+=5;
  else if (rx_power_fil_dB > TARGET_RX_POWER + 5) //&& (phy_vars_ue->rx_total_gain_dB > MIN_RF_GAIN) )
    phy_vars_ue->rx_total_gain_dB-=5;

  if (phy_vars_ue->rx_total_gain_dB>MAX_RF_GAIN) {
    /*
    if ((openair_daq_vars.rx_rf_mode==0) && (openair_daq_vars.mode == openair_NOT_SYNCHED)) {
      openair_daq_vars.rx_rf_mode=1;
      phy_vars_ue->rx_total_gain_dB = max(MIN_RF_GAIN,MAX_RF_GAIN-25);
    }
    else {
    */
    phy_vars_ue->rx_total_gain_dB = MAX_RF_GAIN;
  }
  else if (phy_vars_ue->rx_total_gain_dB<MIN_RF_GAIN) {
    /*
    if ((openair_daq_vars.rx_rf_mode==1) && (openair_daq_vars.mode == openair_NOT_SYNCHED)) {
      openair_daq_vars.rx_rf_mode=0;
      phy_vars_ue->rx_total_gain_dB = min(MAX_RF_GAIN,MIN_RF_GAIN+25);
    }
    else {
    */
    phy_vars_ue->rx_total_gain_dB = MIN_RF_GAIN;
  }


#ifdef CBMIMO1
  for (i=0;i<number_of_cards;i++) {
    //openair_set_rx_rf_mode(i,openair_daq_vars.rx_rf_mode);
    openair_set_rx_gain_cal_openair(i,phy_vars_ue->rx_total_gain_dB);
  }
#else
#ifdef EXMIMO

  //switch (phy_vars_ue->rx_gain_mode[0]) {
  //case max_gain:
      if (phy_vars_ue->rx_total_gain_dB>phy_vars_ue->rx_gain_max[0]) {
          phy_vars_ue->rx_total_gain_dB = phy_vars_ue->rx_gain_max[0];
#ifdef DRIVER2013
          for (i=0;i<phy_vars_ue->lte_frame_parms.nb_antennas_rx;i++) {
              p_exmimo_config->rf.rx_gain[i][0] = 30;
          }
#else
	  exmimo_pci_interface->rf.rx_gain00 = 30;
	  exmimo_pci_interface->rf.rx_gain10 = 30;
#endif

      }
      else if (phy_vars_ue->rx_total_gain_dB<(phy_vars_ue->rx_gain_max[0]-30)) {
          // for the moment we stay in max gain mode
          phy_vars_ue->rx_total_gain_dB = phy_vars_ue->rx_gain_max[0] - 30;
#ifdef DRIVER2013
          for (i=0;i<phy_vars_ue->lte_frame_parms.nb_antennas_rx;i++) {
              p_exmimo_config->rf.rx_gain[i][0] = 0;
          }
#else
	  exmimo_pci_interface->rf.rx_gain00 = 0;
	  exmimo_pci_interface->rf.rx_gain10 = 0;
#endif
          /*
            phy_vars_ue->rx_gain_mode[0] = byp;
            phy_vars_ue->rx_gain_mode[1] = byp;
            exmimo_pci_interface->rf.rf_mode0 = 22991; //bypass
            exmimo_pci_interface->rf.rf_mode1 = 22991; //bypass
            
            if (phy_vars_ue->rx_total_gain_dB<(phy_vars_ue->rx_gain_byp[0]-50)) {
            exmimo_pci_interface->rf.rx_gain00 = 0;
            exmimo_pci_interface->rf.rx_gain10 = 0;
            }
              */
      }
      else {
#ifdef DRIVER2013
          for (i=0;i<phy_vars_ue->lte_frame_parms.nb_antennas_rx;i++) {
              p_exmimo_config->rf.rx_gain[i][0] =  30 - phy_vars_ue->rx_gain_max[0] + phy_vars_ue->rx_total_gain_dB;
          }
#else
	  exmimo_pci_interface->rf.rx_gain00 = 30 - phy_vars_ue->rx_gain_max[0] + phy_vars_ue->rx_total_gain_dB;
	  exmimo_pci_interface->rf.rx_gain10 = 30 - phy_vars_ue->rx_gain_max[1] + phy_vars_ue->rx_total_gain_dB;
#endif
      }
      /*
        break;
      case med_gain:
      case byp_gain:
          if (phy_vars_ue->rx_total_gain_dB>phy_vars_ue->rx_gain_byp[0]) {
              phy_vars_ue->rx_gain_mode[0]   = max_gain;
              phy_vars_ue->rx_gain_mode[1]   = max_gain;
              exmimo_pci_interface->rf.rf_mode0 = 55759; //max gain
              exmimo_pci_interface->rf.rf_mode1 = 55759; //max gain
              
              if (phy_vars_ue->rx_total_gain_dB>phy_vars_ue->rx_gain_max[0]) {
                  exmimo_pci_interface->rf.rx_gain00 = 50;
                  exmimo_pci_interface->rf.rx_gain10 = 50;
              }
              else {
                  exmimo_pci_interface->rf.rx_gain00 = 50 - phy_vars_ue->rx_gain_max[0] + phy_vars_ue->rx_total_gain_dB;
                  exmimo_pci_interface->rf.rx_gain10 = 50 - phy_vars_ue->rx_gain_max[1] + phy_vars_ue->rx_total_gain_dB;
              }
          }
          else if (phy_vars_ue->rx_total_gain_dB<(phy_vars_ue->rx_gain_byp[0]-50)) {
              exmimo_pci_interface->rf.rx_gain00 = 0;
              exmimo_pci_interface->rf.rx_gain10 = 0;
          }
          else {
              exmimo_pci_interface->rf.rx_gain00 = 50 - phy_vars_ue->rx_gain_byp[0] + phy_vars_ue->rx_total_gain_dB;
              exmimo_pci_interface->rf.rx_gain10 = 50 - phy_vars_ue->rx_gain_byp[1] + phy_vars_ue->rx_total_gain_dB;
          }
          break;
      default:
          exmimo_pci_interface->rf.rx_gain00 = 50;
          exmimo_pci_interface->rf.rx_gain10 = 50;
          break;
      }
          */
#endif
#endif

#ifdef DEBUG_PHY
      /*  if ((phy_vars_ue->frame%100==0) || (phy_vars_ue->frame < 10))
    msg("[PHY][ADJUST_GAIN] frame %d,  rx_power = %d, rx_power_fil = %d, rx_power_fil_dB = %d, coef=%d, ncoef=%d, rx_total_gain_dB = %d (%d,%d,%d)\n",
        phy_vars_ue->frame,rx_power,rx_power_fil,rx_power_fil_dB,coef,ncoef,phy_vars_ue->rx_total_gain_dB,
	TARGET_RX_POWER,MAX_RF_GAIN,MIN_RF_GAIN);
      */
#endif //DEBUG_PHY
	
}


