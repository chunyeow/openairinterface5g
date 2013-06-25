#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "SIMULATION/TOOLS/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"
#ifdef IFFT_FPGA
#include "PHY/LTE_REFSIG/mod_table.h"
#endif

#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"
#include "SCHED/defs.h"
#include "SCHED/vars.h"

//#define AWGN
//#define NO_DCI

#define BW 7.68

//#define OUTPUT_DEBUG 1

#define RBmask0 0x00fc00fc
#define RBmask1 0x0
#define RBmask2 0x0
#define RBmask3 0x0

unsigned char dlsch_cqi;

#define NUM_OF_RN 2

PHY_VARS_eNB *PHY_vars_eNb;
PHY_VARS_UE *PHY_vars_UE[NUM_OF_RN];   // this variable is modified to enable multiple relay nodes (# Relay Node = "NUM_OF_RN"); 

// In the following function the first parameter ("unsigned char numRN") is added for # RN in the Parallel Relay Network (PRN); 
void lte_param_init(unsigned char N_tx, unsigned char N_rx, unsigned char transmission_mode, u8 extended_prefix_flag) {
	
	unsigned int j;	
	LTE_DL_FRAME_PARMS *lte_frame_parms;

	printf("Start lte_param_init\n");
  	PHY_vars_eNb = (PHY_VARS_eNB *)malloc(sizeof(PHY_VARS_eNB));
  	for(j=0; j < NUM_OF_RN; j++){
  		PHY_vars_UE[j] = (PHY_VARS_UE *)malloc(sizeof(PHY_VARS_UE));  	
  	}
    PHY_config   = (PHY_CONFIG *)malloc(sizeof(PHY_CONFIG));
  	mac_xface    = (MAC_xface *)malloc(sizeof(MAC_xface));

  	randominit(0);
  	set_taus_seed(0);
  
  	lte_frame_parms = &(PHY_vars_eNb->lte_frame_parms);

  	lte_frame_parms->N_RB_DL            = 25;   //50 for 10MHz and 25 for 5 MHz
  	lte_frame_parms->N_RB_UL            = 25;   
  	lte_frame_parms->Ncp                = extended_prefix_flag;
  	lte_frame_parms->Nid_cell           = 0;
  	lte_frame_parms->nushift            = 0;
  	lte_frame_parms->nb_antennas_tx     = N_tx;
  	lte_frame_parms->nb_antennas_rx     = N_rx;
  	lte_frame_parms->first_dlsch_symbol = 4;
  	lte_frame_parms->num_dlsch_symbols  = (lte_frame_parms->Ncp==0) ? 8: 6; // why "8" but not "7" ? 
  	lte_frame_parms->Ng_times6          = 1;
  	//  lte_frame_parms->Csrs = 2;
  	//  lte_frame_parms->Bsrs = 0;
  	//  lte_frame_parms->kTC = 0;
  	//  lte_frame_parms->n_RRC = 0;
  	lte_frame_parms->mode1_flag = (transmission_mode == 1)? 1 : 0;

  	init_frame_parms(lte_frame_parms);  
  	copy_lte_parms_to_phy_framing(lte_frame_parms, &(PHY_config->PHY_framing));
  	phy_init_top(N_tx, lte_frame_parms); //allocation
  
  	lte_frame_parms->twiddle_fft   = twiddle_fft;
  	lte_frame_parms->twiddle_ifft  = twiddle_ifft;
  	lte_frame_parms->rev           = rev;
  
  	for(j=0; j < NUM_OF_RN; j++){
  		PHY_vars_UE[j]->lte_frame_parms = *lte_frame_parms;
	}
  	/*  
  	lte_gold(lte_frame_parms);
  	generate_ul_ref_sigs();
  	generate_ul_ref_sigs_rx();
  	generate_64qam_table();
  	generate_16qam_table();
  	generate_RIV_tables();
  	generate_pcfich_reg_mapping(lte_frame_parms);
  	generate_phich_reg_mapping(lte_frame_parms);
  	*/

  	phy_init_lte_top(lte_frame_parms);
  	
  	for(j=0; j < NUM_OF_RN; j++){
  		phy_init_lte_ue(&PHY_vars_UE[j]->lte_frame_parms,
			  		  	&PHY_vars_UE[j]->lte_ue_common_vars,
			  		  	PHY_vars_UE[j]->lte_ue_dlsch_vars,
			  		  	PHY_vars_UE[j]->lte_ue_dlsch_vars_SI,
			  		  	PHY_vars_UE[j]->lte_ue_dlsch_vars_ra,
			  		  	PHY_vars_UE[j]->lte_ue_pbch_vars,
			  		  	PHY_vars_UE[j]->lte_ue_pdcch_vars,
			  		  	PHY_vars_UE[j]);
	}
  	phy_init_lte_eNB(&PHY_vars_eNb->lte_frame_parms,
		   		   	 &PHY_vars_eNb->lte_eNB_common_vars,
		   		   	 PHY_vars_eNb->lte_eNB_ulsch_vars,
		   		     0,
		   		     PHY_vars_eNb,
		   		     0,
		   		     0);

    printf("Done lte_param_init\n");
}

DCI0_5MHz_TDD0_t           UL_alloc_pdu;
DCI1A_5MHz_TDD_1_6_t       CCCH_alloc_pdu;
DCI2_5MHz_2A_L10PRB_TDD_t  DLSCH_alloc_pdu1;
DCI2_5MHz_2A_M10PRB_TDD_t  DLSCH_alloc_pdu2;


#define UL_RB_ALLOC    0x1ff;
#define CCCH_RB_ALLOC  computeRIV(PHY_vars_eNb->lte_frame_parms.N_RB_UL, 0, 2)
#define DLSCH_RB_ALLOC 0x1fbf    // igore DC component,RB13
//#define DLSCH_RB_ALLOC 0x1f0f // igore DC component,RB13


int main(int argc, char **argv) {

	char c;
	int i, j, aa;
	#ifdef OUTPUT_DEBUG
		int s, Kr, Kr_bytes;
	#endif
	double sigma2, sigma2_dB = 10, SNR, snr0 = -2.0, snr1, rate;
	int **txdata;
	#ifdef IFFT_FPGA
	  int **txdataF2;
	#endif
	
	LTE_DL_FRAME_PARMS *frame_parms;
	
	double **s_re, **s_im; 
	double **r_re[NUM_OF_RN], **r_im[NUM_OF_RN]; //3D received signal matrices in the form of r_re[# of RN][][], r_im[# of RN][][];
	
	double amps[8] = {0.3868472 , 0.3094778 , 0.1547389 , 0.0773694 , 0.0386847 , 0.0193424 , 0.0096712 , 0.0038685};
	double aoa = .03;
	double ricean_factor = 0.0000005;
	double Td = 0.8;
	double iqim = 0.0;
	u8 channel_length, nb_taps=8;
	u8 extended_prefix_flag=0, transmission_mode=1, n_tx=1, n_rx=1;

	int eNb_id = 0, eNb_id_i = 1;
	unsigned char mcs, dual_stream_UE=0, awgn_flag=0, round, dci_flag=1;
	unsigned short NB_RB = conv_nprb(0, DLSCH_RB_ALLOC);
	unsigned char Ns, l, m;

	unsigned char *input_buffer;
	unsigned short input_buffer_length;
	
	unsigned int ret;
		
	unsigned int coded_bits_per_codeword, nsymb, dci_cnt;	 
	unsigned int tx_lev, tx_lev_dB, dlsch_active=0, num_layers;
	unsigned int trials, dci_errors=0, round_trials[4]={0}, error_tot[4]={0}, decode_error=1;
	int re_allocated;
	
	FILE *bler_fd;
	char bler_fname[20];

	DCI_ALLOC_t  dci_alloc[8], dci_alloc_rx[NUM_OF_RN][8]; // where 1st dimension of "dci_alloc_rx" will hold "# of RNs (UEs)" in the system;
  
    int n_frames;

	channel_desc_t *eNB2UE[NUM_OF_RN];  //which is a pointer array whose size will be the "# of RNs (UEs)" in the system;

	u8 num_pdcch_symbols;
	u8 pilot1, pilot2, pilot3;
	
	//unsigned int NUM_OF_RN = 1;
		
	dci_alloc[0].dci_length = sizeof_DCI0_5MHz_TDD_0_t;
	channel_length = (int) 11+2*BW*Td;

	//NUM_OF_RN  = 1; // by default this program acts exactly as 'dlsim.c'; 
	num_layers = 1;
	mcs        = 0;
	n_frames   = 1000;
	snr0       = 10;
	
		 
	while ((c = getopt (argc, argv, "hadpm:n:s:t:c:x:y:z:")) != -1) {
		switch (c) {
			case 'a':
				awgn_flag = 1;
				break;
		  	case 'd':
				dci_flag = 1;
				break;
		  	case 'm':
				mcs = atoi(optarg);
				break;
		  	case 'n':
				n_frames = atoi(optarg);
				break;
		 	case 's':
				snr0 = atoi(optarg);
				break;
		  	case 't':
				Td = atof(optarg);
				break;
		  	case 'p':
				extended_prefix_flag = 1;
				break;
		  	case 'c':
				num_pdcch_symbols = atoi(optarg);
				break;
		  	case 'x':
				transmission_mode = atoi(optarg);
				if ((transmission_mode!=1) || (transmission_mode!=2) || (transmission_mode!=6)) {
					msg("Unsupported transmission mode %d\n", transmission_mode);
				  	exit(-1);
				}
				break;
		  	case 'y':
				n_tx = atoi(optarg);
				if ((n_tx == 0) || (n_tx > 2)) {
		  			msg("Unsupported number of tx antennas %d\n", n_tx);
		  			exit(-1);
				}
				break;
		  	case 'z':
				n_rx = atoi(optarg);
				if ((n_rx == 0) || (n_rx > 2)) {
		  			msg("Unsupported number of rx antennas %d\n", n_rx);
		  			exit(-1);
				}
				break;
		  	case 'h':
		  	default:
				printf("%s -h(elp) -a(wgn on) -d(ci decoding on) -p(extended prefix on) -m mcs -n n_frames -r NUM_OF_RN -s snr0 -t Delayspread -x transmission mode (1,2,6) -y TXant -z RXant\n", argv[0]);
				printf("-h This message\n");
				printf("-a Use AWGN channel and not multipath\n");
				printf("-d Transmit the DCI and compute its error statistics and the overall throughput\n");
				printf("-p Use extended prefix mode\n");
				printf("-n Number of frames to simulate\n");
				printf("-s Starting SNR, runs from SNR to SNR + 5 dB.  If n_frames is 1 then just SNR is simulated and MATLAB/OCTAVE output is generated\n");
				printf("-t Delay spread for multipath channel\n");
				printf("-x Transmission mode (1,2,6 for the moment)\n");
				printf("-y Number of TX antennas used in eNB\n");
				printf("-z Number of RX antennas used in UE\n");
				exit(1);
				break;
		}
	}

	lte_param_init(n_tx, n_rx, transmission_mode, extended_prefix_flag);  
	printf("Setting mcs = %d\n", mcs);
	printf("NPRB = %d\n", NB_RB);
	printf("n_frames = %d\n", n_frames);
	printf("Transmission mode %d with %dx%d antenna configuration\n", transmission_mode, n_tx, n_rx);


	snr1 = snr0 + 2.0;  // simulated SNR range: [snr0:2.0:snr1];
	printf("SNR0 %f, SNR1 %f\n", snr0, snr1);

	frame_parms = &PHY_vars_eNb->lte_frame_parms;

	#ifdef IFFT_FPGA
	  	txdata    = (int **)malloc16(2*sizeof(int*));
	  	txdata[0] = (int *)malloc16(FRAME_LENGTH_BYTES);
	  	txdata[1] = (int *)malloc16(FRAME_LENGTH_BYTES);

	  	bzero(txdata[0], FRAME_LENGTH_BYTES);
		bzero(txdata[1], FRAME_LENGTH_BYTES);

	  	txdataF2    = (int **)malloc16(2*sizeof(int*));
	  	txdataF2[0] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
	  	txdataF2[1] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);

	  	bzero(txdataF2[0], FRAME_LENGTH_BYTES_NO_PREFIX);
	  	bzero(txdataF2[1], FRAME_LENGTH_BYTES_NO_PREFIX);
	#else
	  	txdata = PHY_vars_eNb->lte_eNB_common_vars.txdata[eNb_id];
	#endif


	// Allocating memory and test the dynamic allocations;
	s_re = malloc(2*sizeof(double*));
	s_im = malloc(2*sizeof(double*));

//	r_re = (double ***)malloc(NUM_OF_RN*sizeof(double**));
//	r_im = (double ***)malloc(NUM_OF_RN*sizeof(double**));
	
//	dci_alloc_rx = (DCI_ALLOC_t **)malloc(NUM_OF_RN*sizeof(DCI_ALLOC_t*));

//	eNB2UE = (channel_desc_t **)malloc(NUM_OF_RN*sizeof(channel_desc_t*));
	
	if (!(s_re && s_im)){
		printf("Cannot allocate memory!\n");
		exit(EXIT_FAILURE);
	}
	
	for(j=0; j < NUM_OF_RN; j++){
		r_re[j] = (double **)malloc(2*sizeof(double*));
		r_im[j] = (double **)malloc(2*sizeof(double*));
		//dci_alloc_rx[j] = (DCI_ALLOC_t *)malloc(8*sizeof(DCI_ALLOC_t));
		
		if (!(r_re[j] && r_im[j])){
			printf("Cannot allocate memory!\n");
			exit(EXIT_FAILURE);
		}
	}

	for (i=0; i<2; i++) {
		s_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
		s_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));		
		if (!(s_re[i] && s_im[i])){
			printf("Cannot allocate memory!\n");
			exit(EXIT_FAILURE);
		}		
		for(j=0; j<NUM_OF_RN; j++){
			r_re[j][i] = (double *)malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
			r_im[j][i] = (double *)malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
			if (!(r_re[j][i] && r_im[j][i])){
				printf("Cannot allocate memory!\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	nsymb = (PHY_vars_eNb->lte_frame_parms.Ncp == 0) ? 14 : 12;
	coded_bits_per_codeword = get_G(&PHY_vars_eNb->lte_frame_parms, NB_RB, get_Qm(mcs), num_pdcch_symbols);

	#ifdef TBS_FIX
	  	rate = (double)3*dlsch_tbs25[get_I_TBS(mcs)][NB_RB-1]/(4*coded_bits_per_codeword);
	#else
	  	rate = (double)dlsch_tbs25[get_I_TBS(mcs)][NB_RB-1]/(coded_bits_per_codeword);
	#endif

	printf("Rate = %f (mod %d)\n",(((double)dlsch_tbs25[get_I_TBS(mcs)][NB_RB-1])*3/4)/coded_bits_per_codeword, get_Qm(mcs));
	sprintf(bler_fname,"bler_relay_DF_%d_%d.m", mcs, NUM_OF_RN);
	bler_fd = fopen(bler_fname,"w");

	for(j=0; j < NUM_OF_RN; j++){
		PHY_vars_UE[j]->lte_ue_pdcch_vars[0]->crnti = 0x1234;
	}
	
	
	// Fill in UL_alloc
	UL_alloc_pdu.type    = 0;
	UL_alloc_pdu.hopping = 0;
	UL_alloc_pdu.rballoc = UL_RB_ALLOC;
	UL_alloc_pdu.mcs     = 1;
	UL_alloc_pdu.ndi     = 1;
	UL_alloc_pdu.TPC     = 0;
	UL_alloc_pdu.cqi_req = 1;

	CCCH_alloc_pdu.type     = 0;
	CCCH_alloc_pdu.vrb_type = 0;
	CCCH_alloc_pdu.rballoc  = CCCH_RB_ALLOC;
	CCCH_alloc_pdu.ndi      = 1;
	CCCH_alloc_pdu.mcs      = 1;
	CCCH_alloc_pdu.harq_pid = 0;
 
    DLSCH_alloc_pdu2.rah        = 0;
	DLSCH_alloc_pdu2.rballoc    = DLSCH_RB_ALLOC;
	DLSCH_alloc_pdu2.TPC        = 0;
	DLSCH_alloc_pdu2.dai        = 0;
	DLSCH_alloc_pdu2.harq_pid   = 0;
	DLSCH_alloc_pdu2.tb_swap    = 0;
	DLSCH_alloc_pdu2.mcs1       = mcs;  
	DLSCH_alloc_pdu2.ndi1       = 1;
	DLSCH_alloc_pdu2.rv1        = 0;
	// Forget second codeword
	DLSCH_alloc_pdu2.tpmi       = 5 ;  // precoding

	  
	// Create transport channel structures for SI pdus
	PHY_vars_eNb->dlsch_eNb_SI        = new_eNb_dlsch(1,1);
	PHY_vars_eNb->dlsch_eNb_SI->rnti  = SI_RNTI;	
	
	for(j=0; j<NUM_OF_RN; j++){
		PHY_vars_UE[j]->dlsch_ue_SI[0]       = new_ue_dlsch(1,1);
		PHY_vars_UE[j]->dlsch_ue_SI[0]->rnti = SI_RNTI;		
	}

	// Create random Channels coefficients;	
	for(j=0; j<NUM_OF_RN; j++){
		eNB2UE[j] = new_channel_desc(1, 1,
			 				  nb_taps,
						      channel_length,
							  amps,
							  NULL,
							  NULL,
							  Td,
							  BW,
							  ricean_factor,
							  aoa,
							  .999,
							  0, 0, 0);
	}
	  
	// Create transport channel structures for 2 transport blocks (MIMO)
	for (i=0; i<2; i++) {
		PHY_vars_eNb->dlsch_eNb[0][i] = new_eNb_dlsch(1,8);				
		if (!PHY_vars_eNb->dlsch_eNb[0][i]) {
		  	printf("Can't get eNb dlsch structures\n");
		  	exit(EXIT_FAILURE);
		}		
		PHY_vars_eNb->dlsch_eNb[0][i]->rnti = 0x1234;
		
		for(j=0; j<NUM_OF_RN; j++){
			PHY_vars_UE[j]->dlsch_ue[0][i] = new_ue_dlsch(1,8); 			
			if (!PHY_vars_UE[j]->dlsch_ue[0][i]) {   
		  		printf("Can't get ue dlsch structures\n");
		  		exit(EXIT_FAILURE);
			}			
			PHY_vars_UE[j]->dlsch_ue[0][i]->rnti = 0x1234; 
		}		
	}
	  
	if (DLSCH_alloc_pdu2.tpmi == 5) {
		PHY_vars_eNb->dlsch_eNb[0][0]->pmi_alloc    = (unsigned short)(taus()&0xffff);
		PHY_vars_eNb->eNB_UE_stats[0].DL_pmi_single = PHY_vars_eNb->dlsch_eNb[0][0]->pmi_alloc;
		for(j=0; j < NUM_OF_RN; j++){
			PHY_vars_UE[j]->dlsch_ue[0][0]->pmi_alloc = PHY_vars_eNb->dlsch_eNb[0][0]->pmi_alloc; 
		}		
	}
	  
	generate_eNb_dlsch_params_from_dci(0,
	                                   &DLSCH_alloc_pdu2,
					 				   0x1234,
					 				   format2_2A_M10PRB,
					 				   PHY_vars_eNb->dlsch_eNb[0],
					 				   &PHY_vars_eNb->lte_frame_parms,
					 				   SI_RNTI,
					 				   RA_RNTI,
					 				   P_RNTI,
					 				   0); //change this later
						 
	   
	// DCI
	  
	memcpy(&dci_alloc[0].dci_pdu[0], &DLSCH_alloc_pdu2, sizeof(DCI2_5MHz_2A_M10PRB_TDD_t));
	  
	dci_alloc[0].dci_length = sizeof_DCI2_5MHz_2A_M10PRB_TDD_t;
	dci_alloc[0].L          = 3;
	dci_alloc[0].rnti       = 0x1234;
	 
	memcpy(&dci_alloc[1].dci_pdu[0], &UL_alloc_pdu, sizeof(DCI0_5MHz_TDD0_t));
	  
	dci_alloc[1].dci_length = sizeof_DCI0_5MHz_TDD_0_t;
	dci_alloc[1].L          = 3;
	dci_alloc[1].rnti       = 0x1234;


	input_buffer_length = PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->TBS/8;
	  
	printf("dlsch0: TBS      %d\n", PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->TBS);
	printf("Input buffer size %d bytes\n", input_buffer_length);
		  
	input_buffer = (unsigned char *)malloc(input_buffer_length+4);
	  
	for (i=0; i < input_buffer_length; i++){
		 input_buffer[i] = (unsigned char)(taus()&0xff);
  	}      
     
      
    // Start of the simulation over different SNR values;  
	for (SNR=snr0; SNR < snr1; SNR +=.5) {
		
		dci_errors = 0;			
		for(i=0; i < 4; i++){
			round_trials[i] = 0;
			error_tot[i]    = 0;		
		}
					
		for (trials=0; trials < n_frames; trials++) {
			
			//printf("Trial %d\n", trials);
		  	fflush(stdout);
		  	round = 0;
		  	
		  	while (round < 4) {
				//printf("Trial %d : Round %d \n",trials, round);
				round_trials[round]++;
				
				if (round == 0) {
		  			PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->Ndi = 1;
		  			PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->rvidx = round>>1;
		  			DLSCH_alloc_pdu2.ndi1             = 1;
		  			DLSCH_alloc_pdu2.rv1              = 0;
		  			memcpy(&dci_alloc[0].dci_pdu[0], &DLSCH_alloc_pdu2, sizeof(DCI2_5MHz_2A_M10PRB_TDD_t));
				}
				else {
		  			PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->Ndi = 0;
		  			PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->rvidx = round>>1;
		  			DLSCH_alloc_pdu2.ndi1             = 0;
		  			DLSCH_alloc_pdu2.rv1              = round>>1;
		  			memcpy(&dci_alloc[0].dci_pdu[0], &DLSCH_alloc_pdu2, sizeof(DCI2_5MHz_2A_M10PRB_TDD_t));
				}

				dlsch_encoding(input_buffer,
						   	   &PHY_vars_eNb->lte_frame_parms,
			   			       num_pdcch_symbols,
				   			   PHY_vars_eNb->dlsch_eNb[0][0]);
		
	
				#ifdef OUTPUT_DEBUG
					for (s=0; s<PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->C; s++) {
						if (s < PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->Cminus)
							Kr = PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->Kminus;
						else
							Kr = PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->Kplus;
		  
		  				Kr_bytes = Kr>>3;
		  
		  				for (i=0; i<Kr_bytes; i++)
							printf("%d : (%x)\n", i, PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->c[s][i]);
					}
				#endif 

				re_allocated = dlsch_modulation(PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id],
												1024,
												0,
												&PHY_vars_eNb->lte_frame_parms,
												num_pdcch_symbols,
												PHY_vars_eNb->dlsch_eNb[0][0]);
	
				#ifdef OUTPUT_DEBUG    
					printf("RB count %d (%d,%d)\n",re_allocated, re_allocated/PHY_vars_eNb->lte_frame_parms.num_dlsch_symbols/12, PHY_vars_eNb->lte_frame_parms.num_dlsch_symbols);
				#endif    
	
				if (num_layers > 1)
					re_allocated = dlsch_modulation(PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id],
								    				1024,
								    				i,									// i = Kr_bytes ==> This might be '1' !?
								    				&PHY_vars_eNb->lte_frame_parms,
								    				num_pdcch_symbols,
								    				PHY_vars_eNb->dlsch_eNb[0][1]);
	
				num_pdcch_symbols = generate_dci_top(1,
													 0,
													 dci_alloc,
													 0,
										  			 1024,
													 &PHY_vars_eNb->lte_frame_parms,
													 PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id],
													 0);
	
				generate_pilots(PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id],
				    			1024,
				    			&PHY_vars_eNb->lte_frame_parms,
				    			eNb_id,
				    			LTE_NUMBER_OF_SUBFRAMES_PER_FRAME);
	
	
				#ifdef IFFT_FPGA
					#ifdef OUTPUT_DEBUG  
						write_output("txsigF0.m","txsF0", PHY_vars_eNb->lte_eNB_common_vars->txdataF[0][0],300*120,1,4);
						write_output("txsigF1.m","txsF1", PHY_vars_eNb->lte_eNB_common_vars->txdataF[0][1],300*120,1,4);
					#endif
	
					// do table lookup and write results to txdataF2
					for (aa=0; aa < PHY_vars_eNb->lte_frame_parms.nb_antennas_tx; aa++) {
						ind = 0;
					 	for (i=0; i < 2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES_NO_PREFIX; i++) 
							if (((i%512) >= 1) && ((i%512) <= 150))
						  		txdataF2[aa][i] = ((int*)mod_table)[PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id][aa][ind++]];
							else if ((i%512) >= 362)
						  		txdataF2[aa][i] = ((int*)mod_table)[PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id][aa][ind++]];
							else 
						  		txdataF2[aa][i] = 0;
					}
	
					tx_lev = 0;
					for (aa=0; aa < PHY_vars_eNb->lte_frame_parms.nb_antennas_tx; aa++) {
						if (frame_parms->Ncp == 1)
							PHY_ofdm_mod(txdataF2[aa],                                     // input
				 					  	 txdata[aa],                                       // output
				 						 PHY_vars_eNb->lte_frame_parms.log2_symbol_size,   // log2_fft_size
							 			 2*nsymb,                                          //NUMBER_OF_SYMBOLS_PER_FRAME, // number of symbols
							 			 PHY_vars_eNb->lte_frame_parms.nb_prefix_samples,  // number of prefix samples
							 			 PHY_vars_eNb->lte_frame_parms.twiddle_ifft,       // IFFT twiddle factors
							 			 PHY_vars_eNb->lte_frame_parms.rev,                // bit-reversal permutation
							 			 CYCLIC_PREFIX);
		  				else {
							normal_prefix_mod(txdataF2[aa], txdata[aa], 2*nsymb, frame_parms);
		  				}

		  				tx_lev += signal_energy(&txdata[aa][0], OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
					}
	
				#else //IFFT_FPGA
	
					#ifdef OUTPUT_DEBUG  
						write_output("txsigF0.m","txsF0", PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id][0],PHY_vars_eNb->lte_frame_parms.samples_per_tti,1,1);
						if (PHY_vars_eNb->lte_frame_parms.nb_antennas_tx > 1)
		  					write_output("txsigF1.m","txsF1", PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id][1],PHY_vars_eNb->lte_frame_parms.samples_per_tti,1,1);
					#endif
	
					tx_lev = 0;
					for (aa=0; aa < PHY_vars_eNb->lte_frame_parms.nb_antennas_tx; aa++) {
						if (frame_parms->Ncp == 1) 
							PHY_ofdm_mod(PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id][aa],        // input
							 			 txdata[aa],                                                    // output
							 			 PHY_vars_eNb->lte_frame_parms.log2_symbol_size,                // log2_fft_size
							 			 2*nsymb,                      //NUMBER_OF_SYMBOLS_PER_FRAME,   // number of symbols
							 			 PHY_vars_eNb->lte_frame_parms.nb_prefix_samples,               // number of prefix samples
							 			 PHY_vars_eNb->lte_frame_parms.twiddle_ifft,                    // IFFT twiddle factors
							 			 PHY_vars_eNb->lte_frame_parms.rev,                             // bit-reversal permutation
							 			 CYCLIC_PREFIX);
		  				else {
							normal_prefix_mod(PHY_vars_eNb->lte_eNB_common_vars.txdataF[eNb_id][aa], txdata[aa], 2*nsymb, frame_parms);
		  				}
		  				tx_lev += signal_energy(&txdata[aa][0], frame_parms->ofdm_symbol_size);
					}  
				#endif //IFFT_FPGA
	
				tx_lev_dB = (unsigned int) dB_fixed(tx_lev);
	
				#ifdef OUTPUT_DEBUG  
					write_output("txsig0.m", "txs0", txdata[0], FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
				#endif

				for (i=0; i < 2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++) {
		  			for (aa=0; aa < PHY_vars_eNb->lte_frame_parms.nb_antennas_tx; aa++) {
						if (awgn_flag == 0) {
		    	    		s_re[aa][i] = ((double)(((short *)txdata[aa]))[(i<<1)]);
			  				s_im[aa][i] = ((double)(((short *)txdata[aa]))[(i<<1)+1]);
		        		}
		        		else {
		        			for(j=0; j<NUM_OF_RN; j++){
		           				r_re[j][aa][i] = ((double)(((short *)txdata[aa]))[(i<<1)]);
		        				r_im[j][aa][i] = ((double)(((short *)txdata[aa]))[(i<<1)+1]);
		        			}
		        		}
		  			}
				}

				if (awgn_flag == 0) {						
					for(j=0; j<NUM_OF_RN; j++){ 
						multipath_channel(eNB2UE[j], s_re, s_im, r_re[j], r_im[j], 2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES, 0);
					}
				}
				sigma2_dB = tx_lev_dB + 10*log10(PHY_vars_eNb->lte_frame_parms.ofdm_symbol_size/(NB_RB*12)) - SNR;

				//AWGN
				sigma2 = pow(10, sigma2_dB/10);
				for (i=0; i < 2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++) {
					for (aa=0; aa < PHY_vars_eNb->lte_frame_parms.nb_antennas_rx; aa++) {
						for (j=0; j < NUM_OF_RN; j++) { // loop over all Relay nodes;
							((short*)PHY_vars_UE[j]->lte_ue_common_vars.rxdata[aa])[2*i]  =(short)(r_re[j][aa][i] + sqrt(sigma2/2)*gaussdouble(0.0,1.0));
							((short*)PHY_vars_UE[j]->lte_ue_common_vars.rxdata[aa])[2*i+1]=(short)(r_im[j][aa][i]+(iqim*r_re[j][aa][i])+sqrt(sigma2/2)*gaussdouble(0.0,1.0));
				  		}
				  	}
				}    
				//    lte_sync_time_init(PHY_vars_eNb->lte_frame_parms,lte_ue_common_vars);
				//    lte_sync_time(lte_ue_common_vars->rxdata, PHY_vars_eNb->lte_frame_parms);
				//    lte_sync_time_free();
	
				#ifdef OUTPUT_DEBUG
					for (j=0; j < NUM_OF_RN; j++) { // loop over all Relay nodes;
						printf("RX level in null symbol %d\n", dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+OFDM_SYM BOL_SIZE_COMPLEX_SAMPLES], OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
						printf("RX level in data symbol %d\n", dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)], OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
					}
					printf("rx_level Null symbol %f\n", 10*log10(signal_energy_fp(r_re, r_im, 1, OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2, 256+(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
					printf("rx_level data symbol %f\n", 10*log10(signal_energy_fp(r_re, r_im, 1, OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2, 256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
				#endif
	
				if (PHY_vars_eNb->lte_frame_parms.Ncp == 0) {  // normal prefix
					pilot1 = 4;
		  			pilot2 = 7;
				  	pilot3 = 11;
				}
				else {  // extended prefix
		  			pilot1 = 3;
		  			pilot2 = 6;
		  			pilot3 = 9;
				}

				/* Once, at one of the RS, the DLSCH pkt has been decoded, then we continue with the following trial! 
				Declare an error iff all RNs fail to decode!*/
				for (j=0; j < NUM_OF_RN; j++) { // loop over all Relay nodes;
					// Inner receiver scheduling for 3 slots
					for (Ns=0; Ns < 3; Ns++) {
						for (l=0; l < pilot2; l++) {
							slot_fep(&PHY_vars_UE[j]->lte_frame_parms,
						    		 &PHY_vars_UE[j]->lte_ue_common_vars,
					 				 l,
					 				 Ns%20,
					 		 		 0, 0);
							if (l == 0)
					  			lte_ue_measurements(PHY_vars_UE[j],
										  			&PHY_vars_UE[j]->lte_frame_parms,
										  			0,
							  						1,
							  						0);
						
							//	printf("rx_avg_power_dB %d\n",PHY_vars->PHY_measurements.rx_avg_power_dB[0]);
							//	printf("n0_power_dB %d\n",PHY_vars->PHY_measurements.n0_power_dB[0]);

							if ((Ns==0) && (l==pilot1)) {// process symbols 0,1,2
								if (dci_flag == 1) { // this flag shows whether DCI packets will be decoded or not; if 1 then decode DCI; otherwise continue with DLSCH;
									PHY_vars_UE[j]->lte_ue_pdcch_vars[0]->crnti = 0x1234;
									PHY_vars_UE[j]->lte_ue_pdcch_vars[0]->num_pdcch_symbols = num_pdcch_symbols;
									rx_pdcch(&PHY_vars_UE[j]->lte_ue_common_vars,
						 					 PHY_vars_UE[j]->lte_ue_pdcch_vars,
						 					 &PHY_vars_UE[j]->lte_frame_parms,
						 					 eNb_id,
						 					 (PHY_vars_UE[j]->lte_frame_parms.mode1_flag == 1) ? SISO : ALAMOUTI,
						 					 0);
		
									dci_cnt = dci_decoding_procedure(PHY_vars_UE[j]->lte_ue_pdcch_vars,
							   										 dci_alloc_rx[j],
																	 eNb_id,
																	 &PHY_vars_UE[j]->lte_frame_parms,
																	 SI_RNTI,
																	 RA_RNTI); // what does the return value 'dci_cnt' hold ?
									if (dci_cnt == 0) {
					  					dlsch_active = 0;	
					  					if(round == 0){  // if an error in the first DCI, then contunue with the next transmission block !?;
											if (j == (NUM_OF_RN-1)){
												dci_errors++;
												error_tot[0]++;
												round_trials[0]++;
					  							round = 5;
					  							//printf("DCI error trial %d errs[0] %d\n",trials,errs[0]);					  							
					  						}
					  					}					
					  				}
		
									for (i=0; i < dci_cnt; i++) {
					  					if ((dci_alloc_rx[j][i].rnti == C_RNTI) && (dci_alloc_rx[j][i].format == format2_2A_M10PRB) &&
						  					(generate_ue_dlsch_params_from_dci(0,
																			   (DCI2_5MHz_2A_M10PRB_TDD_t *)&dci_alloc_rx[j][i].dci_pdu,
																			   C_RNTI,
										 									   format2_2A_M10PRB,
										 									   PHY_vars_UE[j]->dlsch_ue[0],
										 									   &PHY_vars_UE[j]->lte_frame_parms,
										 									   SI_RNTI,
										 									   RA_RNTI,
										 									   P_RNTI) == 0)) {
											dlsch_active = 1;
					  					}
					  					else {
											dlsch_active = 0;
											if(round == 0){  // if an error in the first DCI, then contunue with the next transmission block !?;
												if (j == (NUM_OF_RN-1)){
													dci_errors++;
													error_tot[0]++;
													round_trials[0]++;
					  								#ifdef OUTPUT_DEBUG
					  									printf("DCI misdetection trial %d\n", trials);
					  									round = 5;
													#endif					  									
					  							}
					  						}						 											
						  				}
									}
					  			}  // if (dci_flag == 1)
								else { // if (dci_flag == 0)
									generate_ue_dlsch_params_from_dci(0,
																	  &DLSCH_alloc_pdu2,
																	  C_RNTI,
																	  format2_2A_M10PRB,
																	  PHY_vars_UE[j]->dlsch_ue[0],
											 						  &PHY_vars_UE[j]->lte_frame_parms,
																	  SI_RNTI,
																	  RA_RNTI,
											 						  P_RNTI);
									dlsch_active = 1;
					  			} // if (dci_flag == 1)
							}

							if (dlsch_active == 1) {
								if ((Ns==1) && (l==0)) {// process symbols 3,4,5
									for (m=num_pdcch_symbols; m < pilot2; m++) {
							  			// printf("Demodulating DLSCH for symbol %d (pilot 2 %d)\n", m, pilot2);
							  			if (rx_dlsch(&PHY_vars_UE[j]->lte_ue_common_vars,
									   				 PHY_vars_UE[j]->lte_ue_dlsch_vars,
									   				 &PHY_vars_UE[j]->lte_frame_parms,
									   				 eNb_id,
									   				 eNb_id_i,
									   				 PHY_vars_UE[j]->dlsch_ue[0],
									   				 m,
									   				 (m==num_pdcch_symbols)?1:0,
									   				 dual_stream_UE,
									   				 &PHY_vars_UE[j]->PHY_measurements,
									   				 0) == -1) {
											dlsch_active = 0;
											break;
							  			}
									}
							   	}
							  	if ((Ns==1) && (l==pilot1)) {// process symbols 6,7,8
									for (m=pilot2; m < pilot3; m++)
							  			if (rx_dlsch(&PHY_vars_UE[j]->lte_ue_common_vars,
													 PHY_vars_UE[j]->lte_ue_dlsch_vars,
									   				 &PHY_vars_UE[j]->lte_frame_parms,
									   				 eNb_id,
									   				 eNb_id_i,
									   				 PHY_vars_UE[j]->dlsch_ue[0],
									   				 m,
									   				 0,
									   				 dual_stream_UE,
									   				 &PHY_vars_UE[j]->PHY_measurements,
									   				 0) == -1) {
											dlsch_active = 0;
											break;
							  			}
							  	}
								if ((Ns==2) && (l==0))  // process symbols 10,11, do deinterleaving for TTI
									for (m=pilot3; m < PHY_vars_UE[j]->lte_frame_parms.symbols_per_tti; m++)
							  			if (rx_dlsch(&PHY_vars_UE[j]->lte_ue_common_vars,
									   				 PHY_vars_UE[j]->lte_ue_dlsch_vars,
									   				 &PHY_vars_UE[j]->lte_frame_parms,
									   				 eNb_id,
									   				 eNb_id_i,
									   				 PHY_vars_UE[j]->dlsch_ue[0],
									   				 m,
									   				 0,
									   				 dual_stream_UE,
									   				 &PHY_vars_UE[j]->PHY_measurements,
									   				 0) == -1) {
											dlsch_active = 0;
											break;
							  			}	
							}	
						} // loop over l;
					} // loop over Ns;
				
								
					if (dlsch_active == 1) {
						#ifdef OUTPUT_DEBUG      
			  				write_output("rxsig0.m","rxs0", PHY_vars_UE[j]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
			  				write_output("dlsch00_ch0.m","dl00_ch0", &(PHY_vars_UE[j]->lte_ue_common_vars.dl_ch_estimates[eNb_id][0][0]), (6*(PHY_vars_UE->lte_frame_parms.ofdm_symbol_size)), 1,1);
			  
							write_output("rxsigF0.m","rxsF0", PHY_vars_UE[j]->lte_ue_common_vars.rxdataF[0], 2*12*PHY_vars_UE[j]->lte_frame_parms.ofdm_symbol_size,2,1);
							write_output("rxsigF0_ext.m","rxsF0_ext", PHY_vars_UE[j]->lte_ue_dlsch_vars[eNb_id]->rxdataF_ext[0], 2*12*PHY_vars_UE[j->lte_frame_parms.ofdm_symbol_size,1,1);
							write_output("dlsch00_ch0_ext.m","dl00_ch0_ext", PHY_vars_UE[j]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[0],300*12,1,1);
							write_output("pdcchF0_ext.m","pdcchF_ext", PHY_vars_UE[j]->lte_ue_pdcch_vars[eNb_id]->rxdataF_ext[0], 2*3*PHY_vars_UE[j]->lte_frame_parms.ofdm_symbol_size,1,1);
							write_output("pdcch00_ch0_ext.m","pdcch00_ch0_ext", PHY_vars_UE[j]->lte_ue_pdcch_vars[eNb_id]->dl_ch_estimates_ext[0],300*3,1,1);
	
							write_output("dlsch_rxF_comp0.m","dlsch0_rxF_comp0", PHY_vars_UE[j]->lte_ue_dlsch_vars[eNb_id]->rxdataF_comp[0],300*(-(PHY_vars_UE[j]->lte_frame_parms.Ncp*2)+14),1,1);
							write_output("pdcch_rxF_comp0.m","pdcch0_rxF_comp0", PHY_vars_UE[j]->lte_ue_pdcch_vars[eNb_id]->rxdataF_comp[0],4*300,1,1);
							write_output("dlsch_rxF_llr.m","dlsch_llr", PHY_vars_UE[j]->lte_ue_dlsch_vars[eNb_id]->llr[0], coded_bits_per_codeword,1,0);
							write_output("pdcch_rxF_llr.m","pdcch_llr",PHY_vars_UE[j]->lte_ue_pdcch_vars[eNb_id]->llr,2400,1,4);
					  
							write_output("dlsch_mag1.m","dlschmag1",PHY_vars_UE[j]->lte_ue_dlsch_vars[eNb_id]->dl_ch_mag,300*12,1,1);
							write_output("dlsch_mag2.m","dlschmag2",PHY_vars_UE[j]->lte_ue_dlsch_vars[eNb_id]->dl_ch_magb,300*12,1,1);
						#endif //OUTPUT_DEBUG
			  
			 		 	//	printf("Calling decoding (Ndi %d, harq_pid %d)\n", dlsch_ue[0]->harq_processes[0]->Ndi, dlsch_ue[0]->current_harq_pid);
			  		     
			 		 	ret = dlsch_decoding(PHY_vars_UE[j]->lte_ue_dlsch_vars[eNb_id]->llr[0],		 
											   &PHY_vars_UE[j]->lte_frame_parms,
											   PHY_vars_UE[j]->dlsch_ue[0][0],
								   			   0,
								   			   num_pdcch_symbols);
			  
			 		 	if (ret <= MAX_TURBO_ITERATIONS) {
							//decode_error = 0;  // decode error indicator;
							
							round = 5;  // one of the RN has successfully decoded the messages;
					  		#ifdef OUTPUT_DEBUG  
								printf("No DLSCH errors found\n");
							#endif
							break;      // no need to wait for other relay nodes to decode!; (time saving in processing!)
						}	
			 		 	else {
							//decode_error = 1;  // decode error indicator;
							#ifdef OUTPUT_DEBUG  
								printf("DLSCH in error in round %d at Relay %d\n", round, j);
								for (s=0; s < PHY_vars_UE[j]->dlsch_ue[0][0]->harq_processes[0]->C; s++) {
						  			if (s < PHY_vars_UE[j]->dlsch_ue[0][0]->harq_processes[0]->Cminus)
										Kr = PHY_vars_UE[j]->dlsch_ue[0][0]->harq_processes[0]->Kminus;
						  			else
										Kr = PHY_vars_UE[j]->dlsch_ue[0][0]->harq_processes[0]->Kplus;
				  
						  			Kr_bytes = Kr>>3;
				  
						  			printf("Decoded_output (Segment %d):\n", s);
						  			for (i=0; i < Kr_bytes; i++)
										printf("%d : %x (%x)\n", i, PHY_vars_UE[j]->dlsch_ue[0][0]->harq_processes[0]->c[s][i], PHY_vars_UE[j]->dlsch_ue[0][0]->harq_processes[0]->c[s][i]^PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->c[s][i]);
								}
								exit(-1);
							#endif
							if (j == (NUM_OF_RN-1)){
								//printf("DLSCH Active, but Decode Error...\n");				  	
					  			error_tot[round]++;
					  			round++;
					  		}													
			  			}					
					}  // end of if (dlsch_active == 1);


					/* //+++++++++++++++++++++++++++++ ERROR EVENTS ++++++++++++++++++++++++++++++++++++++++++++++++++
					
					if((dlsch_active == 1) && (decode_error == 0)){
						round = 5;  // one of the RN has successfully decoded the messages;
					  	break;      // no need to wait for other relay nodes to decode!; (time saving in processing!)
					}
					else if(dlsch_active == 0){
						//printf("DLSCH NON Active...\n");					  	
					  	if(round == 0){  // if an error in the first DCI, then contunue with the next transmission block !?;
							if (j == (NUM_OF_RN-1)){
								dci_errors++;
								error_tot[0]++;
								round_trials[0]++;
					  			round = 5;
					  			break;
					  		}
					  	}
					  	else{
							if (j == (NUM_OF_RN-1)){
								error_tot[round]++;
								round++;
							}
					  	}
					}					
					else if(((dlsch_active == 1) && (decode_error == 1))){
						if (j == (NUM_OF_RN-1)){
							//printf("DLSCH Active, but Decode Error...\n");				  	
					  		error_tot[round]++;
					  		round++;
					  	}
					} 				
					//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
											
				}	// loop over number of Relay nodes (or UEs in Downlink);

								
			} // loop for  numner of rounds;
			
			
			// printf("\n");		
			if ((error_tot[0] >= 100) && (trials > (n_frames/2)))
				break; 
				
	  	}   // trials

		for (j=0; j < NUM_OF_RN; j++){
			printf("\n**********************SNR = %f dB (tx_lev %f, sigma2_dB %f)**************************\n", SNR,
				   (double)tx_lev_dB+10*log10(PHY_vars_UE[j]->lte_frame_parms.ofdm_symbol_size/(NB_RB*12)), sigma2_dB);
		}
		printf("Errors (%d/%d %d/%d %d/%d %d/%d), Pe = (%e,%e,%e,%e), dci_errors %d/%d, Pe = %e => effective rate %f (%f), normalized delay %f (%f)\n",
				   error_tot[0],
				   round_trials[0],
				   error_tot[1],
				   round_trials[1],
				   error_tot[2],
				   round_trials[2],
				   error_tot[3],
				   round_trials[3],
				   (double)error_tot[0]/(round_trials[0]),
				   (double)error_tot[1]/(round_trials[1]),
				   (double)error_tot[2]/(round_trials[2]),
				   (double)error_tot[3]/(round_trials[3]),
				   dci_errors,
				   round_trials[0],
				   (double)dci_errors/(round_trials[0]),
				   rate*((double)(round_trials[0]-dci_errors)/((double)round_trials[0] + round_trials[1] + round_trials[2] + round_trials[3])),
				   rate,
				   (1.0*(round_trials[0]-error_tot[0])+2.0*(round_trials[1]-error_tot[1])+3.0*(round_trials[2]-error_tot[2])+4.0*(round_trials[3]-error_tot[3]))/((double)round_trials[0])/(double)PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->TBS,
		 		   (1.0*(round_trials[0]-error_tot[0])+2.0*(round_trials[1]-error_tot[1])+3.0*(round_trials[2]-error_tot[2])+4.0*(round_trials[3]-error_tot[3]))/((double)round_trials[0]));

		fprintf(bler_fd,"%f;%d;%d;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
			    SNR,
			    mcs,
			    PHY_vars_eNb->dlsch_eNb[0][0]->harq_processes[0]->TBS,
			    rate,
				error_tot[0],
				round_trials[0],
				error_tot[1],
				round_trials[1],
				error_tot[2],
				round_trials[2],
				error_tot[3],
				round_trials[3],
				dci_errors);
		
		if (((double)error_tot[0]/(round_trials[0]))<1e-2) 
		  	break;
	  
	}  // loop for SNR;
	  
	fclose(bler_fd);
	  
	printf("Freeing dlsch structures\n");
	for (i=0; i<2; i++) {
		printf("eNb %d\n", i);
		free_eNb_dlsch(PHY_vars_eNb->dlsch_eNb[0][i]);
		
		printf("UE %d\n", i);
		for (j=0; j < NUM_OF_RN; j++) {
			free_ue_dlsch(PHY_vars_UE[j]->dlsch_ue[0][i]);
		}
	}
	
	#ifdef IFFT_FPGA
	  	printf("Freeing transmit signals\n");
	  	free(txdataF2[0]);
	  	free(txdataF2[1]);
	  	free(txdataF2);
	  	free(txdata[0]);
	  	free(txdata[1]);
	  	free(txdata);
	#endif

	printf("Freeing channel I/O\n");
	
	for (j=0; j < NUM_OF_RN; j++) {
		for (i=0; i<2; i++) {
			free(r_re[j][i]);
			free(r_im[j][i]);
		}
		free(r_re[j]);
		free(r_im[j]);		
	}
	
	for (i=0; i<2; i++) {
		free(s_re[i]);
		free(s_im[i]);
	}
	free(s_re);
	free(s_im);
		
	  
	//  lte_sync_time_free();

	return(0);
}	
   

