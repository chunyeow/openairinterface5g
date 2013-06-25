// EMOS simulator for testing of the routines to be used in the real-time
// acquisition mode.
//
// Leonardo S. Cardoso 20070808 - sampaio@eurecom.fr
// 

#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "PHY/CONFIG/vars.h"
#include "MAC_INTERFACE/vars.h"
#include "PHY/TOOLS/defs.h"

//#include "dlc_engine.h"
//#include "MAC_defs.h"

#include <stdio.h>
//#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>

#include <time.h>

// Leosam 08/08: What are these?
#define BW 5.0
#define Td 1.0

void help ();
void error ();

// Leosam 08/08: What are the arguments?
int
main (int argc, char **argv)
{

	// Command line parsing
	int input_val = 1000;	// Value serving as an input relatad to the SNR
	int max_num_ofdm_sym = 64;	// Index of the last frame for the channel estimation
	int be_verbose = 0;	// Operate in verbose mode
	int index;
	int c;
	unsigned int rx_energy[NB_ANTENNAS], n0_energy[NB_ANTENNAS];

	if (argc == 1)
	{
		error ();
		exit (-1);
	}

	while ((c = getopt (argc, argv, "hVv:n:")) != -1)
	{
		switch (c)
		{
		case 'h':
			help ();
			exit (1);
		case 'V':
			be_verbose = 1;
			break;
		case 'v':
			input_val = atoi (optarg);
			break;
			return 1;
		case 'n':
			max_num_ofdm_sym = atoi (optarg);
			break;
			return 1;
		default:
			error ();
			exit (-1);
		}
	}

	//
	// Initialization stuff
	//

	int i, ii, j, ret, delay, l;
	short seed[3];

	// Leosam 08/08: What is this amps?
	double amps[8] = { 1.0, .8, .4, .2, .1, .05, .025, .01 };
	struct complex ch[NB_ANTENNAS * NB_ANTENNAS][10 + (int) (1 + 2 * BW * Td)];
	struct complex rx_tmp, tx, n, phase;

	char *chbch_pdu;
	int chbch_size;
	int extension;
	unsigned char dummy_mac_pdu[120];

#ifdef USER_MODE
  char fname[40], vname[40];
#endif // USER_MODE

	if (be_verbose)
		printf ("Allocating memory for PHY_VARS\n");

	// Memory allocation for PHY and MAC structures
	PHY_vars = malloc (sizeof (PHY_VARS));
	PHY_config = malloc (sizeof (PHY_CONFIG));
	mac_xface = malloc (sizeof (MAC_xface));

	// Loading of the configuration data
	if ((config = fopen ("config.cfg", "r")) == NULL)	// this can be configured
	{
		if (be_verbose)
			printf ("[Main USER] The openair configuration file <config.cfg> could not be found!\n");
		exit (0);
	}

	if ((scenario = fopen ("scenario.scn", "r")) == NULL)
	{
		if (be_verbose)
			printf ("[Main USER] The openair scenario file <scenario.scn> could not be found!\n");
		exit (0);
	}

	if (be_verbose)
		printf ("Opened configuration files\n");

	reconfigure_MACPHY (scenario);

	if (be_verbose)
		dump_config ();

	//  Leosam 08/08: This is repeated bellow. Is it necessary?
	mac_xface->is_cluster_head = 0;

	// Initialize the PHY and MAC variables
	phy_init (NB_ANTENNAS_TX);
	if (be_verbose)
		printf ("Initialized PHY variables\n");


	// Fill MAC PDU buffer for CHBCH
	seed[0] = (short) time (NULL);
	seed[1] = (short) time (NULL);
	seed[2] = (short) time (NULL);
	seed48 (&seed[0]);

	randominit ();

	/*
	 * for (i=0;i<mac_xface->mac_tch->bch_tx[0].size-4;i++) {
	 * mac_xface->mac_tch->bch_tx[0].data[i] = i;//(u8)lrand48();
	 * }
	 * 
	 * 
	 * printf("Filled CHBCH PDU with random data\n");
	 * 
	 * // Generate one CHBCH
	 * phy_generate_chbch(0);
	 * 
	 */

	//
	//  Preparation for the TX procedure
	// 

	// Creation of the CHBCH
	chbch_size = (NUMBER_OF_CARRIERS_PER_GROUP * (NUMBER_OF_CHBCH_SYMBOLS) * 16) >> 3;
	if (be_verbose)
		printf ("chbch_size = %d\n", chbch_size);
	chbch_pdu = malloc (chbch_size);

	for (i = 0; i < chbch_size - 4; i++)
	{
		chbch_pdu[i] = i;
	}

	if (be_verbose)
		printf ("Filled CHBCH PDU (%d bytes) with data\n", chbch_size);

	// Leosam 08/08: O.o
	delay = 1032;
	//	delay = 0;

	// Generation of the CHBCH
	phy_generate_chbch (0, 1, NB_ANTENNAS_TX, chbch_pdu);

	// Generation of the pilot symbols
	for (i = 16; i < max_num_ofdm_sym; i++)
	{
		phy_generate_sch (0, i, 0xFFFF, 1, NB_ANTENNAS_TX);
	}
	
	mac_xface->is_cluster_head = 0;
	
	if (be_verbose)
	{
		for (ii=0; ii<NB_ANTENNAS; ii++)
		{
			sprintf (fname, "txsig%d.m", ii);
			sprintf (vname, "txs%d", ii);

			write_output (fname, vname,
										(s16 *) PHY_vars->tx_vars[ii].TX_DMA_BUFFER,
										NUMBER_OF_SYMBOLS_PER_FRAME * OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES, 1, 1);
		}
	}


	// generate channels
	if (be_verbose)
		printf ("Generating MIMO Channels\n");

	phase.r = 0;
	phase.i = 0;
	
	printf("Phase pointer %0x\n",&phase);

	for (i = 0; i < NB_ANTENNAS; i++)
	{
		for (j = 0; j < NB_ANTENNAS; j++)
		{
			random_channel (amps, Td, 8, BW, ch[j + (i * NB_ANTENNAS)],0.0,&phase);
		}
	}

	if (be_verbose)
		printf ("chbch_pdu %x (%d)\n", PHY_vars->chbch_data[0].demod_pdu, chbch_size);

	if (be_verbose)
	{
		for (l = 0; l < (1 + 2 * BW * Td); l++)
		{
			printf ("(%f,%f)\n", ch[0][l]);
		}
	}

	//
	// TX procedure
	//

	// Transmission 
	//
	//////////////////////////
	
	// Foreach symbol
	for (i = 0; i < (max_num_ofdm_sym) * OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++)
	{
		// Foreach RX antenna
		for (ii = 0; ii < NB_ANTENNAS; ii++)
		{
			rx_tmp.r = 0;
			rx_tmp.i = 0;
			n.r = gaussdouble (0.0, 10.0);
			n.i = gaussdouble (0.0, 10.0);
			
			// Foreach TX antenna
			for (j = 0; j < NB_ANTENNAS; j++)
			{
				
				// Foreach symbol
				for (l = 0; l < (1 + 2 * BW * Td); l++)
				{

					tx.r = (double) (((s16 *) & PHY_vars->tx_vars[j].
							  TX_DMA_BUFFER[0 *
									OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES])
							 [2 * (i - l)]) / sqrt (1.0 * input_val);
					tx.i = (double) (((s16 *) & PHY_vars->tx_vars[j].
							  TX_DMA_BUFFER[0 *
									OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES])
							 [1 +
							  (2 * (i - l))]) / sqrt (1.0 * input_val);

					rx_tmp.r +=
						(tx.r * ch[j + (ii * NB_ANTENNAS)][l].r) -
						(tx.i * ch[j + (ii * NB_ANTENNAS)][l].i);
					rx_tmp.i +=
						(tx.i * ch[j + (ii * NB_ANTENNAS)][l].r) +
						(tx.r * ch[j + (ii * NB_ANTENNAS)][l].i);

				}
			}
 
			((s16 *) & PHY_vars->rx_vars[ii].
			 RX_DMA_BUFFER[delay + (0 * OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)])[2 * i] =
					(short) (rx_tmp.r + n.r);
			((s16 *) & PHY_vars->rx_vars[ii].
			 RX_DMA_BUFFER[delay + (0 * OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)])[1 +
											(2 * i)] =
					(short) (rx_tmp.i + n.i);
		}
	}

	if (be_verbose)
	{
		for (ii=0; ii<NB_ANTENNAS; ii++)
		{
			sprintf (fname, "rxsig%d.m", ii);
			sprintf (vname, "rxs%d", ii);

			write_output (fname, vname,
			      (s16 *) PHY_vars->rx_vars[ii].RX_DMA_BUFFER,
						NUMBER_OF_SYMBOLS_PER_FRAME * OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES, 1, 1);
		}
	}

	//
	// RX procedure
	//  

	if (be_verbose)
		printf ("Starting RX\n");

	// Sync
	phy_synch_time (PHY_vars->rx_vars[0].RX_DMA_BUFFER,
			&sync_pos, 
			FRAME_LENGTH_COMPLEX_SAMPLES, 
			768, 
			CHSCH, 
			0);

	// Forcing sync to 0 since were running offline
	if (be_verbose)
		msg ("sync_pos = %d\n", sync_pos);
	PHY_vars->rx_vars[0].offset = 0;	//sync_pos;
	
	// estamte the signal and noise energy
	for (ii=0; ii < NB_ANTENNAS; ii++)
	{
		rx_energy[ii] = signal_energy((int *)&PHY_vars->rx_vars[ii].RX_DMA_BUFFER[PHY_vars->rx_vars[0].offset+CYCLIC_PREFIX_LENGTH],
																	OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
		n0_energy[ii] = signal_energy((int *)&PHY_vars->rx_vars[ii].RX_DMA_BUFFER[PHY_vars->rx_vars[0].offset+CYCLIC_PREFIX_LENGTH + 
																	(NUMBER_OF_CHSCH_SYMBOLS+NUMBER_OF_CHBCH_SYMBOLS+1) * OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],
																	OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
		//msg("CYCLIC_PREFIX_LENGTH=%d, NUMBER_OF_CHSCH_SYMBOLS=%d, NUMBER_OF_CHBCH_SYMBOLS=%d, OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES=%d\n", CYCLIC_PREFIX_LENGTH, NUMBER_OF_CHSCH_SYMBOLS, NUMBER_OF_CHBCH_SYMBOLS, OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
		msg("symbol = %d\n", PHY_vars->rx_vars[ii].RX_DMA_BUFFER[PHY_vars->rx_vars[0].offset+CYCLIC_PREFIX_LENGTH + 
				(NUMBER_OF_CHSCH_SYMBOLS+NUMBER_OF_CHBCH_SYMBOLS) * OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES]);
		msg("SNR Ant %d = %d / %d \n", ii, rx_energy[ii], n0_energy[ii]);
	}	

	// CHBCH channel estimation
	phy_channel_estimation_top(PHY_vars->rx_vars[0].offset,0,0,0,NB_ANTENNAS,0);
	//phy_channel_estimation_top (PHY_vars->rx_vars[0].offset, 0, 0, 0);

	phy_decode_chbch (0, &dummy_mac_pdu[0], NB_ANTENNAS, NB_ANTENNAS_TX, 120);

	if (be_verbose)
	{
		for (i = 0; i < chbch_size; i++)
		{
			msg ("Data %x : %x\n", i, PHY_vars->chbch_data[0].demod_pdu[i]);
		}
	}
	
	printf("PERROR_SHIFT: %d\n", PERROR_SHIFT);
	
	//
	// Channel estimation procedure
	//
// 	for (i = 0; i < max_num_ofdm_sym; i++)
// 	{
// 		phy_channel_estimation_top(PHY_vars->rx_vars[0].offset,i,0,0,1);
// 	}
	  
	phy_channel_est_emos(16, 16, max_num_ofdm_sym-1, TRUE, 0);

	phy_cleanup ();
	if (be_verbose)
		printf ("Exiting\n");

}

void
help ()
{
	printf ("Usage: emos_sim [OPTIONS] [ARGUMENTS]\n");
	printf ("Simulates the transmission of frames using the EMOS system\n\n");
	printf ("-h shows this help\n");
	printf ("-V be verbose\n");
	printf ("-v value related to SNR\n");
	printf ("-n number of OFDM symbols to average (1~48)\n");
	printf ("\nLeonardo S. Cardoso <sampaio@eurecom.fr>\n");
}

void
error ()
{
	printf ("Wrong option or argument. Try -h to see the valid options.\n");
}
