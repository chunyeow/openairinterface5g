#define TARGET_RX_POWER 55		// Target digital power for the AGC
#define TARGET_RX_POWER_MAX 55		// Maximum digital power for AGC
#define TARGET_RX_POWER_MIN 50		// Minimum digital power for AGC

void gain_control_all (unsigned int rx_power_fil_dB, unsigned int card);
void gain_control (unsigned int rx_power_fil_dB, unsigned int ant, unsigned int card);
