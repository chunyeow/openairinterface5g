#ifdef PLATON

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "daq.h"

// LG WARNING : the 2-3 following arrays must be the same
// as those declared in file protocol_vars.h because
// they are copied by copy_from_user() kernel routine
int             inv_tx_gain_table[4][300];
int             inv_rx_gain_table[4][200];
int             rx_nf_table[4][200];

//------------------------------------------------------------------------------------------------
int             readln (int file_desc, int *value);
int             usr_load_calibration_data (unsigned int daq_fdP, unsigned int nb_rf_cardsP, char **argvP);
//------------------------------------------------------------------------------------------------
int
readln (int file_desc, int *value)
{
//------------------------------------------------------------------------------------------------
  char            ascii_buffer[20];
  int             status;
  int             nb_cars = 0;

  do {
    if (nb_cars >= 20) {
      return -1;
    }
    status = read (file_desc, &ascii_buffer[nb_cars], 1);
    if (status != 1) {
      return -1;
    }
  } while (ascii_buffer[nb_cars++] != '\n');
  ascii_buffer[nb_cars - 1] = 0;
  *value = atoi (ascii_buffer);
  printf ("[AS] loading gain %d\n", *value);
  return 0;
}

//-----------------------------------------------------------------------------
int
usr_load_calibration_data (unsigned int daq_fdP, unsigned int nb_rf_cardsP, char **argvP)
{
//-----------------------------------------------------------------------------
  HARDWARE_CONFIGURATION hardware_configuration;
  char            file_name[] = "                                                  ";
  char            file_name_start[] = "../arch/platon/rf/PLATON_";
  char            file_tx_extension[] = "TX.cal";
  char            file_rx_extension[] = "RX.cal";
  const char     *calibration_file_tx_name;
  const char     *calibration_file_rx_name;
  unsigned int    index, offset, fd;


  memset (inv_tx_gain_table, 0, sizeof (int) * 4 * 300);
  memset (inv_rx_gain_table, 0, sizeof (int) * 4 * 200);
  memset (rx_nf_table, 0, sizeof (int) * 4 * 200);

    printf ("[AS][INFO][LOAD CALIB DATA]  nb_rf_cards %d \n", nb_rf_cardsP);
    ioctl (daq_fdP, DAQ_GET_HARDWARE_CONFIGURATION, &hardware_configuration);

  //printf ("[AS][INFO][LOAD CALIB DATA] Configured for %d antennas, master_id = %d\n", hardware_configuration.number_of_DAQ_cards, hardware_configuration.master_id);

  //-----------------------------------------------------------------------
  // WRITE GAINS
  //-----------------------------------------------------------------------
  for (index = 0; index < nb_rf_cardsP; index++) {
    file_name[0] = '\0';
    strcat (file_name, file_name_start);

    strcat (file_name, argvP[1]);
    strcat (file_name, "_");
    strcat (file_name, argvP[4 + index]);
    strcat (file_name, file_tx_extension);
    calibration_file_tx_name = file_name;

    
    offset = 0;
    printf ("[AS][INFO][LOAD CALIB DATA]  Loading gains from file %s for antenna %d\n", calibration_file_tx_name, index);
    
    if ((fd = open (calibration_file_tx_name, O_RDONLY)) < 0) {
      printf ("[AS][ERROR][LOAD CALIB DATA] Could not open %s\n", calibration_file_tx_name);
      close (daq_fdP);
      return(-1);
     
    }
    while ((readln (fd, &inv_tx_gain_table[index][offset++]) == 0) && (offset < 300));
    close (fd);


    file_name[0] = '\0';
    strcat (file_name, file_name_start);
    
    strcat (file_name, argvP[1]);

    strcat (file_name, "_");

    strcat (file_name, argvP[4 + index]);

    strcat (file_name, file_rx_extension);
    calibration_file_rx_name = file_name;
    offset = 0;
    printf ("[AS][INFO][LOAD CALIB DATA]  Loading gains from file %s for antenna %d\n", calibration_file_rx_name, index);
    if ((fd = open (calibration_file_rx_name, O_RDONLY)) < 0) {
      printf ("[AS][ERROR][LOAD CALIB DATA] Could not open %s\n", calibration_file_rx_name);
      close (daq_fdP);
      return (-1);
    }

    while ((readln (fd, &inv_rx_gain_table[index][offset++]) == 0) && (offset < 200));
    offset = 0;
    while ((readln (fd, &rx_nf_table[index][offset++]) == 0) && (offset < 200));
    close (fd);
  }
  ioctl (daq_fdP, DAQ_SET_CALIBRATION_TX_DATA, &inv_tx_gain_table[0][0]);       
  ioctl (daq_fdP, DAQ_SET_CALIBRATION_RX_DATA, &inv_rx_gain_table[0][0]);        
  ioctl (daq_fdP, DAQ_SET_NOISE_FACTOR_DATA, &rx_nf_table[0][0]);    
  printf ("[AS][INFO][LOAD CALIB DATA]  DONE\n");
  return 0;
}
#endif PLATON
