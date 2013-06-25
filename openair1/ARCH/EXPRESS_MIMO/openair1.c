#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "PHY/CONFIG/vars.h"
#include "MAC_INTERFACE/vars.h"

main() {

  char chbch_tx_power;
  int chbch_size,i,ii,j;
  int nb_antennas_tx=1;
  unsigned int sync_pos;
  char fname[40],vname[40];

  unsigned char *chbch_pdu_tx[2],*chbch_pdu_rx[2];

  int ret[2];
  int tx_energy;

  PHY_vars = malloc(sizeof(PHY_VARS));
  PHY_config = malloc(sizeof(PHY_CONFIG));
  mac_xface = malloc(sizeof(MAC_xface));
  
  if((config = fopen("config.cfg","r")) == NULL) // this can be configured
    {
      printf("[OPENAIR][SIM][CHBCH] The openair configuration file <config.cfg> could not be found!");
      exit(0);
    }		
  
  if ((scenario= fopen("scenario.scn","r")) ==NULL)
    {
      printf("[OPENAIR][SIM][CHBCH] The openair scenario file <scenario.scn> could not be found!");
      exit(0);
    }
  
  printf("[OPENAIR][SIM][CHBCH] Opened configuration files\n");

  reconfigure_MACPHY(scenario);

  dump_config();

  mac_xface->is_cluster_head = 1;

  phy_init(nb_antennas_tx);
  printf("[OPENAIR][SIM][CHBCH] Initialized PHY variables\n");
  PHY_vars->rx_vars[0].rx_total_gain_dB=120;

  chbch_size = (NUMBER_OF_CARRIERS_PER_GROUP*(NUMBER_OF_CHBCH_SYMBOLS)*16)>>3;
  //printf("[OPENAIR][SIM][CHBCH] chbch_size = %d\n",chbch_size);
  chbch_pdu_tx[0]  = malloc(chbch_size);
  chbch_pdu_tx[1]  = malloc(chbch_size);

  for (i=0;i<chbch_size-4;i++) {
    chbch_pdu_tx[0][i] = i;
    chbch_pdu_tx[1][i] = chbch_size-i;
  }

  mac_xface->frame = 0;


  //clear the tx buffer
  for (j=0;j<nb_antennas_tx;j++)      
    Zero_Buffer(PHY_vars->tx_vars[j].TX_DMA_BUFFER,FRAME_LENGTH_SAMPLES*2);

  chbch_tx_power = phy_generate_chbch(1,
				      1,
				      nb_antennas_tx,
				      chbch_pdu_tx[0]);
  
  tx_energy=0;
  for (j=0;j<nb_antennas_tx;j++) {
    tx_energy += signal_energy(&PHY_vars->tx_vars[j].TX_DMA_BUFFER[SAMPLE_OFFSET_CHBCH_NO_PREFIX],
			       (NUMBER_OF_CHBCH_SYMBOLS)*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES_NO_PREFIX);

  }


#ifdef DEBUG_PHY    
      for (ii=0; ii<nb_antennas_tx; ii++) {
	sprintf(fname,"txsig%d.m",ii);
	sprintf(vname,"txs%d",ii);
      
	write_output(fname,vname,
		     (s16 *)&PHY_vars->tx_vars[ii].TX_DMA_BUFFER[0],
		     TX_RX_SWITCH_SYMBOL*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,
		     1,
		     1);
      }
#endif //DEBUG_PHY

    printf("[OPENAIR][SIM][CHBCH] Starting RX\n");	       

    mac_xface->is_cluster_head = 0;
    
    phy_synch_time((short*) PHY_vars->rx_vars[0].RX_DMA_BUFFER,
		   &sync_pos,
		   TX_RX_SWITCH_SYMBOL*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,
		   768,
		   CHSCH,
		   1);

    msg("[OPENAIR][SIM][CHBCH] sync_pos = %d\n",sync_pos);


  return(tx_energy);

}
