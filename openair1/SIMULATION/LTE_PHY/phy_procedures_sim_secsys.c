#include <string.h>
#include <math.h>
#include "SIMULATION/TOOLS/defs.h"
#include "SIMULATION/RF/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"
#ifdef IFFT_FPGA
#include "PHY/LTE_REFSIG/mod_table.h"
#endif
#include "SCHED/defs.h"
#include "SCHED/vars.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"

#include "PHY/LTE_TRANSPORT/mcs_tbs_tools.h"
#define SECONDARY_SYSTEM
#ifdef SECONDARY_SYSTEM
#endif
#ifndef PBS_SIM
//#define PBS_SIM
#endif //PBS_SIM
#ifdef PBS_SIM
#define PBS_OUTPUT_DIR "." //"/homes/sorby/Devel/simDataOutput/" //"/home/Local_Data/sorby_dl_sim_secsys/"
#endif
//#define SKIP_RF_CHAIN
//#define CHANNEL_FROM_FILE   //--Unstable, doesn't work properly. Possibly how the channel is written to file.
//#define FLAT_CHANNEL
//#define SKIP_RF_RX
//#define DISABLE_SECONDARY
#define BW 7.68 //Fs in MHz
#define Td 1
#define N_TRIALS_MAX 2
#define KdB 10 /// Ricean factor in dB
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#define MAX(a,b) (((a) > (b)) ? (a) : (b))

//#define DEBUG_PHY

/*
DCI0_5MHz_TDD0_t          UL_alloc_pdu;
DCI1A_5MHz_TDD_1_6_t      CCCH_alloc_pdu;
DCI2_5MHz_2A_L10PRB_TDD_t DLSCH_alloc_pdu1;
DCI2_5MHz_2A_M10PRB_TDD_t DLSCH_alloc_pdu2;
*/

#define UL_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,24)
#define CCCH_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,3)
#define RA_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,3)
#define DLSCH_RB_ALLOC 0x1fff

int main(int argc, char **argv) {

  int i,j,l,aa,sector,i_max,l_max,aa_max,aatx,aarx;
  double SE,target_rx_pwr_dB = 35;
  double sigma2, sigma2_dB=0;
  int n_frames = 2;
  number_of_cards = 1;
  mod_sym_t **txdataF;
#ifdef SECONDARY_SYSTEM
  mod_sym_t **txdataF_ext;
#endif
#ifdef IFFT_FPGA
  int **txdataF2;
#ifdef SECONDARY_SYSTEM
  int **txdataF2_secsys;
#endif
#endif
   int **txdata,**rxdata;
#ifdef SECONDARY_SYSTEM
   int **txdata_ext, **rxdata_ext[2];
#endif
   double **s_re,**s_im,**r_re,**r_im;
   //double amps[8] = {0.999,0.001,0.0,0.0,0.0,0.0,0.0,0.0};
   double amps[8] = {0.3868472 , 0.3094778 , 0.1547389 , 0.0773694 , 0.0386847 , 0.0193424 , 0.0096712 , 0.0038685};
   double aoa=0.1;
   double rice_k = KdB;
   double ricean_factor=(1/(1 + pow(10,.1*rice_k))); //0.0000005;
   double forgetting_factor = 0; // high forgetting factor -> slow temporal variation 
   int channel_length;
   int meas_length, meas_offset;
   struct complex **ch;
   unsigned char pbch_pdu[6];
   int sync_pos, sync_pos_slot;
   FILE *rx_frame_file;
   int result;
   int freq_offset;
   int subframe_offset;
   char fname[200], vname[100];
   int trial, n_errors=0;
   double STxGain = 0;
   double SNR = 0;
   unsigned char plot_flag=0;
#ifdef PBS_SIM
   FILE *er_data_fd, *turboIter_fd;//, *er_cause_fd;
   char er_data_fname[120], turboIter_fname[120];//, er_cause_fname[60];
  // for file output
  char pbs_output_dir[100] = "";
  unsigned char pbs_output_dir_length = 0;
  strcat(pbs_output_dir,PBS_OUTPUT_DIR);
  pbs_output_dir_length = 100;
  char tempChar[100] = "";
#endif //PBS_SIM
#define N_STxG (int)((stxg1-stxg0+stxgStepSize)/stxgStepSize)
#define N_SNR (int)((snr1-snr0+snrStepSize)/snrStepSize)
  double stxg0,stxg1;
  double snr0,snr1;
  /******************************************************************
                     INITIALIZE SIMULATION PARAMETERS

   *****************************************************************/
  unsigned char stxg_ind = 0; ///index for STxG to be written out in loop
  unsigned char snr_ind = 0; ///index for SNR to be written out in loop
/*
  float snrStepSize = 0.25; //step size in dB  -- will be fixed
  float stxgStepSize = 0.5; //step size in dB -- will be fixed
  */
  float snrStepSize = 0.5; //step size in dB
  float stxgStepSize = 1; //step size in dB
  n_frames = N_TRIALS_MAX; //maximum length of simulation in number of frames
  stxg0 = 0;
  stxg1 = 0; //set = stxg0 to keep fixed
  snr0 = 10;
  snr1 = 11; //set = stxg0 to keep fixed
  if (argc==2) {
    snr0 = atof(argv[1]);
    snr1 = snr0; //set = stxg0 to keep fixed
  }
  if (argc>2) {
    snr0 = atof(argv[1]);
    snr1 = atof(argv[2]); //set = stxg0 to keep fixed
  }
  if (argc>3)
    n_frames = atoi(argv[3]);
  if (argc>4){
    rice_k = atoi(argv[4]);
    ricean_factor=(1/(1 + pow(10,.1*rice_k)));
  }
  if (argc>5) {
    stxg0 = atof(argv[5]);
    stxg1 = stxg0; //set = stxg0 to keep fixed
  }
  if (argc>6) {
    stxg1 = atof(argv[6]);
  }
  if (argc>7) {
    snrStepSize = atof(argv[7]);
  }
  if (argc>8) {
    stxgStepSize = atof(argv[8]);
  }
  else {
    SE = 1;
  }
  int ch_min =400, ch_max=0;
  double stxg_act=0;
  int dl_er[2], dci_er[2];
  unsigned int nb_rb = 25;
  unsigned int first_rb = 0;
  unsigned int eNb_id = 0;
  unsigned int UE_id = 0;
#ifdef SECONDARY_SYSTEM
  unsigned int eNb_id_secsys = 0;
  unsigned int UE_id_secsys = 0;
#endif
  unsigned int slot_offset, slot_offset_time;

  int slot,last_slot, next_slot;

  double path_loss_dB_def = -60;
  unsigned int rx_gain[3];
  double rx_gain_lin;

  double nf[2] = {3.0,3.0}; //currently unused
  double ip =0.0;
  double N0W, path_loss, path_loss_dB, tx_pwr, rx_pwr;
#ifdef SECONDARY_SYSTEM
  enum UNIT_ID {
    PeNb,    /// PeNb
    SeNb,    /// SeNb
    P_UE,    /// P_UE
    S_UE    /// S_UE
  };
  enum CH_ID {
    PeSe,    /// PeNb <--> SeNb
    PeSu,    /// PeNb <--> S_UE
    SePu,    /// SeNb <--> P_UE
    PuSu,    /// P_UE <--> S_UE
    PePu,    /// PeNb <--> P_UE
    SeSu     /// SeNb <--> S_UE
  };
#ifdef SECONDARY_SYSTEM
  //   double aoa_ar[6]={M_PI*1.2,M_PI/2,M_PI/2,M_PI*1.1,0,0}; with the old equation
  double aoa_ar[6]={(M_PI/2)*1.05,(M_PI/4)*.95,(M_PI/4)*1.05,(M_PI/2)*.95,0,0.05*(M_PI/4)};
#endif
  double path_loss_ar_dB[6], path_loss_ar[6];
  struct complex **ch_ar[6];
  double tx_pwr_secsys, rx_pwr_sec[2], STxG, SePu_pwr, rx_pwr_pre[3], tx_pwr_post[2], tx_pwr_SRS, rx_pwr_SRS;
  int STxGdB = 0;
  double **s_re_secsys,**s_im_secsys,**r_re_ext[2],**r_im_ext[2];
  double **r_re_crossLink[6],**r_im_crossLink[6]; /// indexed by enum CH_I
  FILE *channel_file;
  char has_channel=0;
  char channel_output[] = "channel_output.txt";
#endif //SECONDARY_SYSTEM
  int rx_pwr2;
  double norm_const=1;

  unsigned char first_call = 1,first_call_secsys = 1;

  char stats_buffer[4096];
  int len;

#ifdef EMOS
  fifo_dump_emos emos_dump;
#endif

  channel_length = (int) 11+2*BW*Td;
#ifdef SECONDARY_SYSTEM
  double ch_tmp[2*channel_length*4*6]; //(real/imag)*channel_length*(nbRx*nbTx)*#ofChannels
#endif

  PHY_config = malloc(sizeof(PHY_CONFIG));
  mac_xface = malloc(sizeof(MAC_xface));
#ifndef SECONDARY_SYSTEM
  PHY_VARS_eNB *PHY_vars_eNb[1]; // 1 eNB
  PHY_vars_eNb[0] = malloc(sizeof(PHY_VARS_eNB));
  PHY_VARS_UE *PHY_vars_UE[1]; // 1 UEs
  PHY_vars_UE[0] = malloc(sizeof(PHY_VARS_UE));
#else //SECONDARY_SYSTEM
  PHY_VARS_eNB *PHY_vars_eNb[3]; // 3 eNBs
  PHY_vars_eNb[0] = malloc(sizeof(PHY_VARS_eNB));
  PHY_vars_eNb[1] = malloc(sizeof(PHY_VARS_eNB));
  PHY_vars_eNb[2] = malloc(sizeof(PHY_VARS_eNB)); //virtual eNb
  PHY_VARS_UE *PHY_vars_UE[3]; // 3 UEs
  PHY_vars_UE[0] = malloc(sizeof(PHY_VARS_UE));
  PHY_vars_UE[1] = malloc(sizeof(PHY_VARS_UE));
  PHY_vars_UE[2] = malloc(sizeof(PHY_VARS_UE)); //virtual UE
  //  PHY_config_secsys = malloc(sizeof(PHY_CONFIG));
  //  mac_xfaec_secsys = malloc(sizeof(MAC_xface));
#endif

  //lte_frame_parms = &(PHY_config->lte_frame_parms);
  lte_frame_parms = &(PHY_vars_eNb[0]->lte_frame_parms);
  
  lte_frame_parms->N_RB_DL            = 25;
  lte_frame_parms->N_RB_UL            = 25;
  lte_frame_parms->Ng_times6          = 1;
  lte_frame_parms->Ncp                = 1;
  lte_frame_parms->Nid_cell           = 0;
  lte_frame_parms->nushift            = 0;
  lte_frame_parms->nb_antennas_tx     = 2;
  lte_frame_parms->nb_antennas_rx     = 2;
  lte_frame_parms->first_dlsch_symbol = 4;
  lte_frame_parms->num_dlsch_symbols  = 6;
  lte_frame_parms->mode1_flag         = 1;
  lte_frame_parms->Csrs = 2;
  lte_frame_parms->Bsrs = 0;
  lte_frame_parms->kTC = 0;
  lte_frame_parms->n_RRC = 0;

  init_frame_parms(lte_frame_parms);
  
  copy_lte_parms_to_phy_framing(lte_frame_parms, &(PHY_config->PHY_framing));
  
  phy_init_top(NB_ANTENNAS_TX);
  
  lte_frame_parms->twiddle_fft      = twiddle_fft;
  lte_frame_parms->twiddle_ifft     = twiddle_ifft;
  lte_frame_parms->rev              = rev;
  
  lte_gold(lte_frame_parms);
  generate_ul_ref_sigs();
  generate_ul_ref_sigs_rx();
  generate_64qam_table();
  generate_16qam_table();
  generate_RIV_tables();

#ifdef DEBUG_PHY
  msg("[PHY_vars_UE] = %p",PHY_vars_UE);
#endif //DEBUG_PHY

  lte_sync_time_init(lte_frame_parms);

  //*****************************************************************
  //              INITIALIZE TOP STRUCTURES
  //*****************************************************************

  //use same frame parameters for UE as for eNb
  PHY_vars_UE[0]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx = 1;
  PHY_vars_UE[0]->lte_frame_parms.nb_antennas_tx = 1;
  PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx = 1;
  PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx = 1;
#ifdef SECONDARY_SYSTEM
  //use same frame parameters for secondary system
  PHY_vars_eNb[1]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx = 2;
  PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx = 2;
  //use same frame parameters for UE as for eNb
  PHY_vars_UE[1]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx = 2;
  PHY_vars_UE[1]->lte_frame_parms.nb_antennas_tx = 2;

  /// VIRTUAL UNITS, used for comparative measurements
  //virtual user used as a copy of the primary receiver, with only interference for comparative measurements
  PHY_vars_UE[2]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_UE[2]->lte_frame_parms.nb_antennas_rx = 1;
  PHY_vars_UE[2]->lte_frame_parms.nb_antennas_tx = 1;
  PHY_vars_eNb[2]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_rx = 1;
  PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_tx = 1;
#endif
 
  PHY_vars_UE[0]->is_secondary_ue = 0;
  phy_init_lte_ue(&PHY_vars_UE[0]->lte_frame_parms,
		  &PHY_vars_UE[0]->lte_ue_common_vars,
		  PHY_vars_UE[0]->lte_ue_dlsch_vars,
		  PHY_vars_UE[0]->lte_ue_dlsch_vars_cntl,
		  PHY_vars_UE[0]->lte_ue_dlsch_vars_ra,
		  PHY_vars_UE[0]->lte_ue_dlsch_vars_1A,
		  PHY_vars_UE[0]->lte_ue_pbch_vars,
		  PHY_vars_UE[0]->lte_ue_pdcch_vars,
		  PHY_vars_UE[0]);
  PHY_vars_UE[0]->lte_ue_dlsch_vars[eNb_id]->log2_maxh = 4;
  PHY_vars_UE[0]->dlsch_ue_active = 0;
  PHY_vars_UE[0]->dlsch_ue_cntl_active = 0;
  PHY_vars_UE[0]->dlsch_errors = 0;

  PHY_vars_eNb[0]->is_secondary_eNb = 0; 
  phy_init_lte_eNB(&PHY_vars_eNb[0]->lte_frame_parms,
		   &PHY_vars_eNb[0]->lte_eNB_common_vars,
		   PHY_vars_eNb[0]->lte_eNB_ulsch_vars,
		   PHY_vars_eNb[0]->is_secondary_eNb,
		   PHY_vars_eNb[1],
		   0,
		   0);
  PHY_vars_eNb[0]->is_init_sync = 0; // not used for primary eNb
  PHY_vars_eNb[0]->dlsch_eNb_active = 0;
  PHY_vars_eNb[0]->dlsch_eNb_cntl_active = 0;
#ifdef SECONDARY_SYSTEM
  PHY_vars_UE[1]->is_secondary_ue = 1;
  PHY_vars_UE[1]->has_valid_precoder = 0;
  // exploit pilot shift to have orthogonal pilots in frequency
  //NOTE: If this is done in SeNb only, then the only difference at receiver is that its channel estimates of xchannel is in buffer with index PeNb_id and the wanted signals channel is in buffer with index (PeNb_id+nu_shift(1))
  //PHY_vars_UE[1]->lte_frame_parms.nushift = 1;
  phy_init_lte_ue(&PHY_vars_UE[1]->lte_frame_parms,
		  &PHY_vars_UE[1]->lte_ue_common_vars,
		  PHY_vars_UE[1]->lte_ue_dlsch_vars,
		  PHY_vars_UE[1]->lte_ue_dlsch_vars_cntl,
		  PHY_vars_UE[1]->lte_ue_dlsch_vars_ra,
		  PHY_vars_UE[1]->lte_ue_dlsch_vars_1A,
		  PHY_vars_UE[1]->lte_ue_pbch_vars,
		  PHY_vars_UE[1]->lte_ue_pdcch_vars,
		  PHY_vars_UE[1]);
  PHY_vars_UE[1]->lte_ue_dlsch_vars[eNb_id_secsys]->log2_maxh = 4;
  PHY_vars_UE[1]->dlsch_ue_active = 0;
  PHY_vars_UE[1]->dlsch_ue_cntl_active = 0;
  PHY_vars_UE[1]->dlsch_errors = 0;

  PHY_vars_UE[2]->is_secondary_ue = 0;
  phy_init_lte_ue(&PHY_vars_UE[2]->lte_frame_parms,
		  &PHY_vars_UE[2]->lte_ue_common_vars,
		  PHY_vars_UE[2]->lte_ue_dlsch_vars,
		  PHY_vars_UE[2]->lte_ue_dlsch_vars_cntl,
		  PHY_vars_UE[2]->lte_ue_dlsch_vars_ra,
		  PHY_vars_UE[2]->lte_ue_dlsch_vars_1A,
		  PHY_vars_UE[2]->lte_ue_pbch_vars,
		  PHY_vars_UE[2]->lte_ue_pdcch_vars,
		  PHY_vars_UE[2]);
  PHY_vars_UE[2]->lte_ue_dlsch_vars[eNb_id_secsys]->log2_maxh = 4;
  PHY_vars_UE[2]->dlsch_ue_active = 0;
  PHY_vars_UE[2]->dlsch_ue_cntl_active = 0;
  PHY_vars_UE[2]->dlsch_errors = 0;

  PHY_vars_eNb[1]->is_secondary_eNb = 1;
  // exploit pilot shift to have orthogonal pilots in frequency
  PHY_vars_eNb[1]->lte_frame_parms.nushift = 1;
  phy_init_lte_eNB(&PHY_vars_eNb[1]->lte_frame_parms,
		   &PHY_vars_eNb[1]->lte_eNB_common_vars,
		   PHY_vars_eNb[1]->lte_eNB_ulsch_vars,
		   PHY_vars_eNb[1]->is_secondary_eNb,
		   PHY_vars_eNb[1],
		   0,
		   0);
  PHY_vars_eNb[1]->is_init_sync = 1; //synchronization is not simulated yet, hence synch is assumed
  PHY_vars_eNb[1]->has_valid_precoder = 0;
  PHY_vars_eNb[1]->dlsch_eNb_active = 0;
  PHY_vars_eNb[1]->dlsch_eNb_cntl_active = 0;
  //Virtual eNb
  PHY_vars_eNb[2]->is_secondary_eNb = 0;
  phy_init_lte_eNB(&PHY_vars_eNb[2]->lte_frame_parms,
		   &PHY_vars_eNb[2]->lte_eNB_common_vars,
		   PHY_vars_eNb[2]->lte_eNB_ulsch_vars,
		   PHY_vars_eNb[2]->is_secondary_eNb,
		   PHY_vars_eNb[2],
		   0,
		   0);
  PHY_vars_eNb[2]->is_init_sync = 0;
  PHY_vars_eNb[2]->has_valid_precoder = 0;
  PHY_vars_eNb[2]->dlsch_eNb_active = 0;
  PHY_vars_eNb[2]->dlsch_eNb_cntl_active = 0;
#endif
  //  UE_mode = PRACH;
  //PHY_vars_UE[0]->UE_mode = PRACH;
  //PHY_vars_eNb[0]->eNB_UE_stats[0].mode[0] = PRACH;
  PHY_vars_UE[0]->UE_mode = PUSCH;
  PHY_vars_eNb[0]->eNB_UE_stats[0].mode[0] = PUSCH;
  PHY_vars_eNb[0]->eNB_UE_stats[0].UE_id[0] = 0xBA82;
  PHY_vars_UE[0]->lte_ue_pdcch_vars[0]->crnti = 0xBA82;
#ifdef SECONDARY_SYSTEM
  //PHY_vars_UE[1]->UE_mode = PRACH;
  //PHY_vars_eNb[1]->eNB_UE_stats[0].mode[0] = PRACH;
  PHY_vars_UE[1]->UE_mode = PUSCH;
  PHY_vars_eNb[1]->eNB_UE_stats[0].mode[0] = PUSCH;
  PHY_vars_eNb[1]->eNB_UE_stats[0].UE_id[0] = 0xBD17;
  PHY_vars_UE[1]->lte_ue_pdcch_vars[0]->crnti = 0xBD17;
#endif //SECONDARY_SYSTEM

#ifndef SECONDARY_SYSTEM
  aa_max = 1; //number of eNBs
  l_max = 1; //number of UEs
#else //SECONDARY_SYSTEM
  aa_max = 3; //number of eNBs
  l_max = 3; //number of UEs
#endif
  
  //loop over eNBs
  for (aa=0;aa<aa_max;aa++) {
    PHY_vars_eNb[aa]->dlsch_eNb = (LTE_eNb_DLSCH_t**) malloc16(2*sizeof(LTE_eNb_DLSCH_t*));
    //loop over transport channels
    for (i=0;i<2;i++) {
      PHY_vars_eNb[aa]->dlsch_eNb[i] = new_eNb_dlsch(1,8);
      if (!PHY_vars_eNb[aa]->dlsch_eNb[i]) {
	msg("Can't get eNb ulsch structures\n");
	exit(-1);
      } else {
	msg("PHY_vars_eNb[%d]->dlsch_eNb[%d] = %p\n",aa,i,PHY_vars_eNb[aa]->dlsch_eNb[i]);
      }
    }
    PHY_vars_eNb[aa]->ulsch_eNb = (LTE_eNb_ULSCH_t**) malloc16(sizeof(LTE_eNb_ULSCH_t*));
    PHY_vars_eNb[aa]->ulsch_eNb[0] = new_eNb_ulsch(3);
    if (!PHY_vars_eNb[aa]->ulsch_eNb[0]) {
      msg("Can't get eNb ulsch structures\n");
      exit(-1);
    } else {
      msg("PHY_vars_eNb[%d]->ulsch_eNb[%d] = %p\n",aa,0,PHY_vars_eNb[aa]->ulsch_eNb[0]);
    }
    PHY_vars_eNb[aa]->dlsch_eNb_cntl = new_eNb_dlsch(1,1);
    PHY_vars_eNb[aa]->dlsch_eNb_1A = new_eNb_dlsch(1,1);
    PHY_vars_eNb[aa]->dlsch_eNb_ra = new_eNb_dlsch(1,1);
  }
  //loop over UEs
  for (l=0;l<l_max;l++) {
    PHY_vars_UE[l]->dlsch_ue = (LTE_UE_DLSCH_t**) malloc16(2*sizeof(LTE_UE_DLSCH_t*));
    //loop over transport channels
    for (i=0;i<2;i++) {
      PHY_vars_UE[l]->dlsch_ue[i] = new_ue_dlsch(1,8);
      if (!PHY_vars_UE[l]->dlsch_ue[i]) {
	msg("Can't get ue ulsch structures\n");
	exit(-1);
      } else {
	msg("PHY_vars_UE [%d]->dlsch_ue [%d] = %p\n",i,aa,PHY_vars_UE[l]->dlsch_ue[i]);
      }
    }
    PHY_vars_UE[l]->ulsch_ue = (LTE_UE_ULSCH_t**) malloc16(2*sizeof(LTE_UE_ULSCH_t*));
    PHY_vars_UE[l]->ulsch_ue[0] = new_ue_ulsch(3);
    if (!PHY_vars_UE[l]->ulsch_ue[0]) {
      msg("Can't get ue ulsch structures\n");
      exit(-1);
    } else {
      msg("PHY_vars_UE [%d]->ulsch_ue [%d] = %p\n",l,0,PHY_vars_UE[l]->ulsch_ue[0]);
    }
    PHY_vars_UE[l]->dlsch_ue_cntl  = new_ue_dlsch(1,1);
    PHY_vars_UE[l]->dlsch_ue_1A  = new_ue_dlsch(1,1);
    PHY_vars_UE[l]->dlsch_ue_ra  = new_ue_dlsch(1,1);
  }
  
  
#ifdef SECONDARY_SYSTEM

  txdataF_rep_tmp = (int **)malloc(2*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx)*sizeof(int*));
  for (aa=0; aa<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx); aa++) {
    txdataF_rep_tmp[aa] = (int *)malloc16(2*sizeof(int)*(PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size)); // repeated format (hence the '2*')
    if (txdataF_rep_tmp[aa]) {
#ifdef DEBUG_PHY
	msg("[openair][LTE_PHY][INIT] txdataF_rep_tmp[%d] allocated at %p\n",aa,txdataF_rep_tmp[aa]);
#endif
    bzero(txdataF_rep_tmp[aa],2*(sizeof(int))*(PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size));
    }
    else {
      msg("[openair][LTE_PHY][INIT] txdataF_rep_tmp[%d] not allocated\n",aa);
      return(-1);
    }
#ifdef DEBUG_PHY
    msg("[openair][LTE_PHY][INIT] txdataF_rep_tmp[%d] = %p, length = %d\n",aa,txdataF_rep_tmp[aa],2*(sizeof(int))*(PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size));
#endif
  }
#endif //SECONDARY_SYSTEM
 

  unsigned char m_mcs,m_I_tbs;
              //SE = 1
  m_I_tbs = SE2I_TBS(SE, lte_frame_parms->N_RB_DL, lte_frame_parms->num_dlsch_symbols);
  m_mcs = I_TBS2I_MCS(m_I_tbs);

  // init DCI structures for testing
  UL_alloc_pdu.type    = 0;
  UL_alloc_pdu.hopping = 0;
  UL_alloc_pdu.rballoc = UL_RB_ALLOC;
  UL_alloc_pdu.mcs     = 0;
  UL_alloc_pdu.ndi     = 1;
  UL_alloc_pdu.TPC     = 0;
  UL_alloc_pdu.cqi_req = 1;

  CCCH_alloc_pdu.type               = 1;
  CCCH_alloc_pdu.vrb_type           = 0;
  CCCH_alloc_pdu.rballoc            = CCCH_RB_ALLOC;
  CCCH_alloc_pdu.ndi      = 1;
  CCCH_alloc_pdu.rv       = 1;
  CCCH_alloc_pdu.mcs      = 1;
  CCCH_alloc_pdu.harq_pid = 0;
  CCCH_alloc_pdu.TPC      = 1;

  DLSCH_alloc_pdu1A.type             = 1;
  DLSCH_alloc_pdu1A.vrb_type         = 0;
  DLSCH_alloc_pdu1A.rballoc          = CCCH_RB_ALLOC;
  DLSCH_alloc_pdu1A.ndi              = 1;
  DLSCH_alloc_pdu1A.rv               = 1;
  DLSCH_alloc_pdu1A.mcs              = 0;
  DLSCH_alloc_pdu1A.harq_pid         = 0;
  DLSCH_alloc_pdu1A.TPC              = 1;

  RA_alloc_pdu.type             = 1;
  RA_alloc_pdu.vrb_type         = 0;
  RA_alloc_pdu.rballoc          = RA_RB_ALLOC;
  RA_alloc_pdu.ndi              = 1;
  RA_alloc_pdu.rv               = 1;
  RA_alloc_pdu.mcs              = 1;
  RA_alloc_pdu.harq_pid         = 0;
  RA_alloc_pdu.TPC              = 1;

  DLSCH_alloc_pdu2.rah              = 0;
  DLSCH_alloc_pdu2.rballoc          = DLSCH_RB_ALLOC;
  DLSCH_alloc_pdu2.TPC              = 0;
  DLSCH_alloc_pdu2.dai              = 0;
  DLSCH_alloc_pdu2.harq_pid         = 0;
  DLSCH_alloc_pdu2.tb_swap          = 0;
  DLSCH_alloc_pdu2.mcs1             = 4;
  DLSCH_alloc_pdu2.ndi1             = 1;
  DLSCH_alloc_pdu2.rv1              = 0;
  // Forget second codeword
  DLSCH_alloc_pdu2.tpmi             = 0;


#ifdef IFFT_FPGA
  txdata    = (int **)malloc16(2*sizeof(int*));
  txdata[0] = (int *)malloc16(FRAME_LENGTH_BYTES);
  txdata[1] = (int *)malloc16(FRAME_LENGTH_BYTES);

  bzero(txdata[0],FRAME_LENGTH_BYTES);
  bzero(txdata[1],FRAME_LENGTH_BYTES);

  rxdata    = (int **)malloc16(2*sizeof(int*));
  rxdata[0] = (int *)malloc16(FRAME_LENGTH_BYTES);
  rxdata[1] = (int *)malloc16(FRAME_LENGTH_BYTES);

  bzero(rxdata[0],FRAME_LENGTH_BYTES);
  bzero(rxdata[1],FRAME_LENGTH_BYTES);
  txdataF2    = (int **)malloc16(2*sizeof(int*));
  txdataF2[0] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
  txdataF2[1] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);

  bzero(txdataF2[0],FRAME_LENGTH_BYTES_NO_PREFIX);
  bzero(txdataF2[1],FRAME_LENGTH_BYTES_NO_PREFIX);
#endif
  
  s_re = malloc(2*sizeof(double*));
  s_im = malloc(2*sizeof(double*));
  r_re = malloc(2*sizeof(double*));
  r_im = malloc(2*sizeof(double*));
#ifdef SECONDARY_SYSTEM
  s_re_secsys = malloc(2*sizeof(double*));
  s_im_secsys = malloc(2*sizeof(double*));
  for (i=0; i<2; i++) {
    r_re_ext[i] = malloc(2*sizeof(double*));
    r_im_ext[i] = malloc(2*sizeof(double*));
  }
  for (l=0; l<6; l++) {
    r_re_crossLink[l] = malloc(2*sizeof(double*));
    r_im_crossLink[l] = malloc(2*sizeof(double*));
  }
#endif
  
  for (i=0;i<2;i++) { //loop over antennas

    s_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(s_re[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    s_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(s_im[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    r_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(r_re[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    r_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(r_im[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));

#ifdef SECONDARY_SYSTEM
    s_re_secsys[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(s_re_secsys[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    s_im_secsys[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    bzero(s_im_secsys[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    for (j=0; j<2; j++) {
      r_re_ext[j][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re_ext[j][i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im_ext[j][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im_ext[j][i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    }
    for (l=0; l<6; l++) { //loop over channel index
      r_re_crossLink[l][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re_crossLink[l][i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im_crossLink[l][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im_crossLink[l][i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    } // channel index
#endif
  }

  ch = (struct complex**) malloc(4 * sizeof(struct complex*));
  for (i = 0; i<4; i++)
    ch[i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
#ifdef SECONDARY_SYSTEM
  for (l=0; l<6; l++) {
    ch_ar[l] = (struct complex**) malloc(4 * sizeof(struct complex*));
    for (i = 0; i<4; i++)
      ch_ar[l][i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
  }

  randominit(0);
  set_taus_seed(0);

#ifdef CHANNEL_FROM_FILE
  channel_file = fopen(channel_output,"r");
  if (channel_file) {
    if (fscanf(channel_file,"%d",&has_channel) != EOF) {
      if (has_channel == 1) {
	for (aa=0; aa<6; aa++) { //loop over channel index
	  for (i=0; i<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx; i++) {
	    for (j=0; j<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx); j++) {
	      for (l=0; l<channel_length; l++) {
		if (fscanf(channel_file,"%lf %lf",&ch_ar[aa][i + j*2][l].r, &ch_ar[aa][i + j*2][l].i) != 2) break;
	      }
	    }
	  }
	}
      }
    }
    fclose(channel_file);
  }
#else //CHANNEL_FROM_FILE
  has_channel = 0;
  first_call = 1;
  first_call_secsys = 1;
#endif //CHANNEL_FROM_FILE


#ifdef FLAT_CHANNEL

  struct complex ch_const[6][PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx)];
  /*
  double realCh_const[4];
  double imagCh_const[4];

  for (i=0; i<4; i++) {
    realCh_const[i] = gaussdouble(0.0,1.0);
    imagCh_const[i] = gaussdouble(0.0,1.0);
  }
  */
  for (aa=0; aa<6; aa++) { //loop over channel index
    for (i=0; i<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx; i++) {
      for (j=0; j<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx); j++) {
	ch_const[aa][i +j*2].r = gaussdouble(0.0,1.0);
	ch_const[aa][i +j*2].i = gaussdouble(0.0,1.0);
	if (has_channel) {
	  ch_const[aa][i +j*2].r = ch_ar[aa][i + j*2][0].r;
	  ch_const[aa][i +j*2].i = ch_ar[aa][i + j*2][0].i;
	} else {
	  ch_ar[aa][i + j*2][0].r = ch_const[aa][i +j*2].r;
	  ch_ar[aa][i + j*2][0].i = ch_const[aa][i +j*2].i;
	}
	//	msg("ch_const[%i][%i]->r = %lf \n",aa,i +j*2,ch_const[aa][i +j*2].r);
	//	msg("ch_const[%i][%i]->i = %lf \n",aa,i +j*2,ch_const[aa][i +j*2].i);
	for (l=1; l<channel_length; l++) {
	  ch_ar[aa][i + j*2][l].r = 0;
	  ch_ar[aa][i + j*2][l].i = 0;
	}
      }
    }
    
    norm_const = pow(
		     pow(ch_ar[aa][0][0].r,2) 
		     + pow(ch_ar[aa][0][0].i,2) +
		     pow(ch_ar[aa][1][0].r,2)
		     + pow(ch_ar[aa][1][0].i,2)
		     ,.5);
    ch_ar[aa][0][0].r = (ch_ar[aa][0][0].r/norm_const);
    ch_ar[aa][0][0].i = (ch_ar[aa][0][0].i/norm_const);
    ch_ar[aa][1][0].r = (ch_ar[aa][1][0].r/norm_const);
    ch_ar[aa][1][0].i = (ch_ar[aa][1][0].i/norm_const);
  }
  has_channel = 1;
  first_call = 0;
  first_call_secsys = 0;

    int knownData[PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size*PHY_vars_eNb[1]->lte_frame_parms.symbols_per_tti>>1][20][2][2];//[samples][slot][antennas][real-0/imag-1]
    char knownDataFlag = 0;
    int check =0;
  channel_file = fopen("knownData.txt","r");
  
  if (channel_file) {
    if (fscanf(channel_file,"%d",&knownDataFlag) != EOF) {
      if (knownDataFlag == 1) {
	for (aa=0; aa<2; aa++) { //loop over antennas
	  for (j=0; j<20; j++) {
	    for (l=0; l<(PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size*PHY_vars_eNb[1]->lte_frame_parms.symbols_per_tti>>1); l++) {
	      check = fscanf(channel_file,"%d %d",&knownData[l][j][aa][0], &knownData[l][j][aa][1]);
	      if (check == 2) {
	      }
	      else {
		msg("read wrong!\n");
		break;
	      }
	    }
	  }
	}
      }
    }
    fclose(channel_file);
  }

#endif //FLAT_CHANNEL
#endif //SECONDARY_SYSTEM


  openair_daq_vars.tdd = 1;
  openair_daq_vars.dlsch_transmission_mode = 1;
  openair_daq_vars.rx_gain_mode = DAQ_AGC_OFF;
  openair_daq_vars.rx_rf_mode = 1;
  openair_daq_vars.timing_advance = TIMING_ADVANCE_INIT;
  openair_daq_vars.target_ue_dl_mcs = 4;
  openair_daq_vars.target_ue_ul_mcs = 4;
  openair_daq_vars.dlsch_rate_adaptation = 0;
  openair_daq_vars.ue_ul_nb_rb = 2;

  PHY_vars_UE[0]->rx_total_gain_dB = 140;
  PHY_vars_eNb[0]->rx_total_gain_eNB_dB = 140;
#ifdef SECONDARY_SYSTEM			
  PHY_vars_UE[1]->rx_total_gain_dB = 140;
  PHY_vars_eNb[1]->rx_total_gain_eNB_dB = 140;
  PHY_vars_UE[2]->rx_total_gain_dB = 140;
  PHY_vars_eNb[2]->rx_total_gain_eNB_dB = 140;
#endif			


#ifdef PBS_SIM
  strncpy(tempChar,pbs_output_dir,100);
#ifdef MU_RECEIVER
#ifndef DISABLE_SECONDARY
  strcat(tempChar,"er_data_%d_%d_K%d.m");
#else
  strcat(tempChar,"er_data_%d_%d_K%d_UB.m");
#endif //DISABLE_SECONDARY
#else
  strcat(tempChar,"er_data_%d_%d_K%d_noIC.m");
#endif //MU_RECEIVER
  sprintf(er_data_fname,tempChar,(int)(snr0*10 + 200),(int)(snr1*10 + 200),(int)rice_k); // + 200 for offset to get positive integer
  er_data_fd = fopen(er_data_fname,"w");
  if (er_data_fd) {
    printf("Opened er_data_fd file successfully\n");
  } else {
    printf("Could not open er_data_fd file\n");
  }
  strncpy(tempChar,pbs_output_dir,100);
#ifdef MU_RECEIVER
#ifndef DISABLE_SECONDARY
  strcat(tempChar,"turboIter_%d_%d_K%d.m");
#else
  strcat(tempChar,"turboIter_%d_%d_K%d_UB.m");
#endif //DISABLE_SECONDARY
#else
  strcat(tempChar,"turboIter_%d_%d_K%d_noIC.m");
#endif //MU_RECEIVER
  sprintf(turboIter_fname,tempChar,(int)(snr0*10 + 200),(int)(snr1*10 + 200),(int)rice_k);
  turboIter_fd = fopen(turboIter_fname,"w");
  if (turboIter_fd) {
    printf("Opened turboIter_fd file successfully\n");
  } else {
    printf("Could not open turboIter_fd file\n");
  }
  /*
    strncpy(tempChar,pbs_output_dir,100);
    strcat(tempChar,"er_cause_%d_%d_K%d.csv");
    sprintf(er_cause_fname,tempChar,(int)(snr0*10 + 200),(int)(snr1*10 + 200),(int)rice_k);
    er_cause_fd = fopen(er_cause_fname,"w");
  */
  fprintf(er_data_fd,"er_data_fd = zeros(%i,%i,%i,%i);\n",N_STxG,N_SNR,2,7);
  fprintf(turboIter_fd,"turboIter_fd = ones(%i,%i,%i);\n",N_STxG,N_SNR,2);
  //  fprintf(er_cause_fd,"dci/dlsch,Pri/Sec,stxg_ind,snr_ind,frame,rx_pwr,int_pwr,E[|ch_SePu|^2],stxg_act/tx_pwr,log2(pmax)\n");
#endif //PBS_SIM
  
  /*-------------------------------------------------------------
    SIMULATION START
    ---------------------------------------------------------------*/
  for (SNR = snr0; SNR<=snr1; SNR+=snrStepSize) {
    snr_ind++;
    if ((SNR >= 5) && (snrStepSize < 1)) {
      snrStepSize = 1;
    }
    if ((SNR >= 10) && (snrStepSize < 2)) {
      snrStepSize = 2;
    }
    path_loss_dB_def = -105 + SNR;
#ifndef PBS_SIM
    printf("path_loss_dB_def _pwr: %f\n",path_loss_dB_def);
#endif //PBS_SIM
    stxg_ind=0;
    for (STxGain = stxg0; STxGain<=stxg1; STxGain+=stxgStepSize) {
      stxg_ind++; // initialized with 0, first index 1 (for MatLab/Octave)
      PHY_vars_UE[0]->dlsch_errors=0;
      PHY_vars_UE[0]->dlsch_errors_last=0;
      PHY_vars_UE[0]->dlsch_received=0;
      PHY_vars_UE[0]->dlsch_received_last=0;
      PHY_vars_UE[0]->dlsch_fer=0;
      PHY_vars_UE[0]->dlsch_cntl_errors=0;
      PHY_vars_UE[0]->dlsch_cntl_received=0;
      PHY_vars_UE[0]->dlsch_ra_errors=0;
      PHY_vars_UE[0]->current_dlsch_cqi=0;
      PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_errors=0;
      PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_received=0;
      PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_false=0;
      PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_missed=0;
      PHY_vars_UE[0]->lte_ue_pbch_vars[eNb_id]->pdu_errors=0;
      PHY_vars_UE[0]->lte_ue_pbch_vars[eNb_id]->pdu_errors_last=0;
      PHY_vars_UE[0]->lte_ue_pbch_vars[eNb_id]->pdu_errors_conseq=0;
      PHY_vars_UE[0]->lte_ue_pbch_vars[eNb_id]->pdu_fer=0;
      PHY_vars_UE[0]->turbo_iterations=0;
      PHY_vars_UE[0]->first_run_timing_advance=0;
      
#ifdef SECONDARY_SYSTEM
      PHY_vars_UE[1]->dlsch_errors=0;
      PHY_vars_UE[1]->dlsch_errors_last=0;
      PHY_vars_UE[1]->dlsch_received=0;
      PHY_vars_UE[1]->dlsch_received_last=0;
      PHY_vars_UE[1]->dlsch_fer=0;
      PHY_vars_UE[1]->dlsch_cntl_errors=0;
      PHY_vars_UE[1]->dlsch_cntl_received=0;
      PHY_vars_UE[1]->dlsch_ra_errors=0;
      PHY_vars_UE[1]->current_dlsch_cqi=0;
      PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_errors=0;
      PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_received=0;
      PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_false=0;
      PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_missed=0;
      PHY_vars_UE[1]->lte_ue_pbch_vars[eNb_id]->pdu_errors=0;
      PHY_vars_UE[1]->lte_ue_pbch_vars[eNb_id]->pdu_errors_last=0;
      PHY_vars_UE[1]->lte_ue_pbch_vars[eNb_id]->pdu_errors_conseq=0;
      PHY_vars_UE[1]->lte_ue_pbch_vars[eNb_id]->pdu_fer=0;
      PHY_vars_UE[1]->turbo_iterations=0;
      PHY_vars_UE[1]->first_run_timing_advance=0;
#endif

      dl_er[0] = 0;
      dci_er[0] = 0;
      first_call = 1;
      
#ifdef SECONDARY_SYSTEM
      dl_er[1] = 0;
      dci_er[1] = 0;
      first_call_secsys = 1;
#endif
      
      PHY_vars_UE[0]->UE_mode = PUSCH;
      PHY_vars_eNb[0]->eNB_UE_stats[0].mode[0] = PUSCH;
      PHY_vars_eNb[0]->eNB_UE_stats[0].UE_id[0] = 0xBA82;
      PHY_vars_UE[0]->lte_ue_pdcch_vars[0]->crnti = 0xBA82;
#ifdef SECONDARY_SYSTEM
      PHY_vars_UE[1]->UE_mode = PUSCH;
      PHY_vars_eNb[1]->eNB_UE_stats[0].mode[0] = PUSCH;
      PHY_vars_eNb[1]->eNB_UE_stats[0].UE_id[0] = 0xBD17;
      PHY_vars_UE[1]->lte_ue_pdcch_vars[0]->crnti = 0xBD17;
#endif
      
#ifdef SECONDARY_SYSTEM
      printf("STxGain ::  %f dB\n",STxGain);
      printf("SNR     ::  %f dB\n",SNR);
#endif
      
#ifdef SKIP_RF_CHAIN
      STxG = pow(10,.1*STxGain);
#endif //SKIP_RF_CHAIN
      
      for (mac_xface->frame=0; mac_xface->frame<n_frames; mac_xface->frame++) {

#ifndef PBS_SIM
	if (mac_xface->frame==2)
	  plot_flag = 1;
	else 
	  plot_flag = 0;
#endif //PBS_SIM   
#ifdef RANDOM_BF
	//	if (mac_xface->frame%20==0) {
	  for (aa=0; aa<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx; aa++) {
	    for (j=0; j<2; j++) { //loop over real/imaginary
	      PHY_vars_eNb[1]->const_ch[aa][j] = gaussdouble(0.0,1.0);
	      /*
	      if ((PHY_vars_eNb[1]->const_ch[aa][j] < 0.2) && (PHY_vars_eNb[1]->const_ch[aa][j] >= 0)) 
		PHY_vars_eNb[1]->const_ch[aa][j] = 0.2;
	      if (PHY_vars_eNb[1]->const_ch[aa][j] > .5) 
		PHY_vars_eNb[1]->const_ch[aa][j] = .5;
	      if (PHY_vars_eNb[1]->const_ch[aa][j] < -.5) 
		PHY_vars_eNb[1]->const_ch[aa][j] = -.5;
	      if ((PHY_vars_eNb[1]->const_ch[aa][j] > -0.2) && (PHY_vars_eNb[1]->const_ch[aa][j] <= 0)) 
		PHY_vars_eNb[1]->const_ch[aa][j] = -0.2;
	      */

	      PHY_vars_UE[1]->const_ch[aa][j] = gaussdouble(0.0,1.0);
	      /*
	      if ((PHY_vars_UE[1]->const_ch[aa][j] < 0.2) && (PHY_vars_UE[1]->const_ch[aa][j] >= 0)) 
		PHY_vars_UE[1]->const_ch[aa][j] = 0.2;
	      if (PHY_vars_UE[1]->const_ch[aa][j] > .5) 
		PHY_vars_UE[1]->const_ch[aa][j] = .5;
	      if (PHY_vars_UE[1]->const_ch[aa][j] < -.5) 
		PHY_vars_UE[1]->const_ch[aa][j] = -.5;
	      if ((PHY_vars_UE[1]->const_ch[aa][j] > -0.2) && (PHY_vars_UE[1]->const_ch[aa][j] <= 0)) 
		PHY_vars_UE[1]->const_ch[aa][j] = -0.2;
	      */
	    }
	  }
	  norm_const = pow(
			   pow(PHY_vars_eNb[1]->const_ch[0][0],2) 
			   + pow(PHY_vars_eNb[1]->const_ch[0][1],2) +
			   pow(PHY_vars_eNb[1]->const_ch[1][0],2)
			   + pow(PHY_vars_eNb[1]->const_ch[1][1],2)
			   ,.5);
	  PHY_vars_eNb[1]->const_ch[0][0] = (short)((PHY_vars_eNb[1]->const_ch[0][0]/norm_const)*pow(2,10));//512;//
	  ch_min = MIN(abs(ch_min),abs(PHY_vars_eNb[1]->const_ch[0][0]));
	  ch_max = MAX(abs(ch_max),abs(PHY_vars_eNb[1]->const_ch[0][0]));
	  PHY_vars_eNb[1]->const_ch[0][1] = (short)((PHY_vars_eNb[1]->const_ch[0][1]/norm_const)*pow(2,10));//238;//
	  ch_min = MIN(abs(ch_min),abs(PHY_vars_eNb[1]->const_ch[0][1]));
	  ch_max = MAX(abs(ch_max),abs(PHY_vars_eNb[1]->const_ch[0][1]));
	  PHY_vars_eNb[1]->const_ch[1][0] = (short)((PHY_vars_eNb[1]->const_ch[1][0]/norm_const)*pow(2,10));//-102;//
	  ch_min = MIN(abs(ch_min),abs(PHY_vars_eNb[1]->const_ch[1][0]));
	  ch_max = MAX(abs(ch_max),abs(PHY_vars_eNb[1]->const_ch[1][0]));
	  PHY_vars_eNb[1]->const_ch[1][1] = (short)((PHY_vars_eNb[1]->const_ch[1][1]/norm_const)*pow(2,10));//512;//
	  ch_min = MIN(abs(ch_min),abs(PHY_vars_eNb[1]->const_ch[1][1]));
	  ch_max = MAX(abs(ch_max),abs(PHY_vars_eNb[1]->const_ch[1][1]));
	  norm_const = pow(
			   pow(PHY_vars_UE[1]->const_ch[0][0],2) 
			   + pow(PHY_vars_UE[1]->const_ch[0][1],2) +
			   pow(PHY_vars_UE[1]->const_ch[1][0],2)
			   + pow(PHY_vars_UE[1]->const_ch[1][1],2)
			   ,.5);
	  PHY_vars_UE[1]->const_ch[0][0] = (short)((PHY_vars_UE[1]->const_ch[0][0]/norm_const)*pow(2,10));
	  PHY_vars_UE[1]->const_ch[0][1] = (short)((PHY_vars_UE[1]->const_ch[0][1]/norm_const)*pow(2,10));
	  PHY_vars_UE[1]->const_ch[1][0] = (short)((PHY_vars_UE[1]->const_ch[1][0]/norm_const)*pow(2,10));
	  PHY_vars_UE[1]->const_ch[1][1] = (short)((PHY_vars_UE[1]->const_ch[1][1]/norm_const)*pow(2,10));
	  //	}
#endif //RANDOM_BF
	  
	  for (slot=0 ; slot<20 ; slot++) {
	    last_slot = (slot - 1)%20;
	    if (last_slot <0)
	      last_slot+=20;
	    next_slot = (slot + 1)%20;
	    
#ifdef SECONDARY_SYSTEM
	    path_loss_dB = path_loss_dB_def;
	    path_loss    = pow(10,.1*path_loss_dB);
	    /*
	      if ((next_slot > 2) && (next_slot < 10)) {
	      if (PHY_vars_UE[1]->UE_mode == PRACH) // 6 RBs, 23 dBm
	      path_loss_dB += (-20+6.2);  // UE
	      else
	      path_loss_dB += (-20+(double)PHY_vars_UE[1]->ulsch_ue[0]->power_offset);
	      }*/
	    path_loss_ar_dB[SeSu] = path_loss_dB;
	    path_loss_ar[SeSu]    = pow(10,.1*path_loss_ar_dB[SeSu]);
	    path_loss_ar_dB[PeSu] = path_loss_dB;
	    path_loss_ar[PeSu]    = pow(10,.1*path_loss_ar_dB[PeSu]);
#endif //SECONDARY_SYSTEM
	    
	    path_loss_dB = path_loss_dB_def;
	    path_loss    = pow(10,.1*path_loss_dB);
#ifdef SECONDARY_SYSTEM
	    path_loss_ar_dB[PeSe] = path_loss_dB;
	    path_loss_ar[PeSe]    = pow(10,.1*path_loss_ar_dB[PeSe]);
#endif //SECONDARY_SYSTEM
	    
	    /* 
	       if (PHY_vars_UE[0]->UE_mode == PRACH) // 6 RBs, 23 dBm
	       path_loss_dB += (-20+6.2);  // UE
	       else
	       path_loss_dB += (-20+(double)PHY_vars_UE[0]->ulsch_ue[0]->power_offset);
	    */
#ifdef SECONDARY_SYSTEM
	    path_loss_ar_dB[SePu] = path_loss_dB;
	    path_loss_ar[SePu]    = pow(10,.1*path_loss_ar_dB[SePu]);
	    path_loss_ar_dB[PePu] = path_loss_dB;
#endif //SECONDARY_SYSTEM
#ifndef PBS_SIM
	    printf("path_losses, next_slot %d: \n",next_slot);
	    for (i=1;i<6;i++) {
	      printf("path_loss_ar_dB[%d]: %f\n",i,path_loss_ar_dB[i]);
	    }
#endif //PBS_SIM
	      /*
		if ((next_slot > 2) && (next_slot < 10)) { //UL
		PHY_vars_eNb[0]->rx_total_gain_eNB_dB = - (MIN(path_loss_ar_dB[PePu],path_loss_ar_dB[PeSu]));
		PHY_vars_eNb[1]->rx_total_gain_eNB_dB = - (MIN(path_loss_ar_dB[SePu],path_loss_ar_dB[SeSu]));
		PHY_vars_eNb[2]->rx_total_gain_eNB_dB = PHY_vars_eNb[0]->rx_total_gain_eNB_dB;
		for (i=0 ; i<3; i++) {
		rx_gain[i] = PHY_vars_eNb[i]->rx_total_gain_eNB_dB;
		#ifndef PBS_SIM
		printf("[RF RX] Slot: %d: rx_gain (eNB) %d\n",next_slot, rx_gain[i])
		#endif //PBS_SIM
		}
		} else { //D
		PHY_vars_UE[0]->rx_total_gain_dB = - (MIN(path_loss_ar_dB[PePu],path_loss_ar_dB[SePu]));
		PHY_vars_UE[1]->rx_total_gain_dB = - (MIN(path_loss_ar_dB[SeSu],path_loss_ar_dB[PeSu]));
		PHY_vars_UE[2]->rx_total_gain_dB = PHY_vars_UE[0]->rx_total_gain_dB;
		for (i=0 ; i<3; i++) 
		rx_gain[i] = PHY_vars_UE[i]->rx_total_gain_dB;
		#ifndef PBS_SIM
		printf("[RF RX] Slot: %d: rx_gain (UE) %d\n",next_slot, rx_gain[i]);
		#endif //PBS_SIM
		}
		}
	      */

      /*-------------------------------------------------------------
	                   ALL LTE PROCESSING
      ---------------------------------------------------------------*/
    
      if (next_slot==2) {
	//channel just changed and the precoders are no longer valid.
	PHY_vars_eNb[1]->has_valid_precoder = 0;
	PHY_vars_UE[1]->has_valid_precoder = 0;
      }
#ifndef PBS_SIM
      printf("\n");
      printf("Frame %d, slot %d : eNB procedures\n",mac_xface->frame,slot);
#endif //PBS_SIM
      mac_xface->is_cluster_head = 1;
      phy_procedures_eNb_lte(last_slot,next_slot,PHY_vars_eNb[0]);
#ifndef DISABLE_SECONDARY
#ifdef SECONDARY_SYSTEM
#ifndef PBS_SIM
      printf("\n");
      printf("Frame %d, slot %d : Secondary eNB procedures\n",mac_xface->frame,slot);
#endif //PBS_SIM
      phy_procedures_eNb_lte(last_slot,next_slot,PHY_vars_eNb[1]);
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_UL) || last_slot==3) {
	//printf("\n");
	//printf("Frame %d, slot %d : Virtual Primary eNB procedures\n",mac_xface->frame,slot);
	//phy_procedures_eNb_lte(last_slot,next_slot,PHY_vars_eNb[2]);
      }
#endif //SECONDARY_SYSTEM
#ifndef PBS_SIM
      printf("\n\n");
      printf("Frame %d, slot %d : UE procedures\n",mac_xface->frame,slot);
#endif //PBS_SIM
#endif //DISABLE_SECONDARY
      mac_xface->is_cluster_head = 0;      
      phy_procedures_ue_lte(last_slot,next_slot,PHY_vars_UE[0]);
#ifndef DISABLE_SECONDARY
#ifdef SECONDARY_SYSTEM
#ifndef PBS_SIM
      printf("\n");
      printf("Frame %d, slot %d : Secondary UE procedures\n",mac_xface->frame,slot);
#endif //PBS_SIM
      phy_procedures_ue_lte(last_slot,next_slot,PHY_vars_UE[1]);
#ifndef PBS_SIM
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || last_slot==2) {
	//printf("\n");
	//printf("Frame %d, slot %d : Virtual Primary UE procedures\n",mac_xface->frame,slot);
	//phy_procedures_ue_lte(last_slot,next_slot,PHY_vars_UE[2]);
      }
#endif //PBS_SIM
#endif //SECONDARY_SYSTEM
#ifndef PBS_SIM
      printf("\n");
#endif //PBS_SIM
#endif //DISABLE_SECONDARY


      /*-------------------------------------------------------------
	ASSIGN POINTERS TO CORRECT BUFFERS ACCORDING TO TDD-STRUCTURE
	                  -----  TX PART  -----
                 and perform OFDM modulation ifndef IFFT_FPGA
      ---------------------------------------------------------------*/

      if (subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) {
	txdataF = PHY_vars_eNb[0]->lte_eNB_common_vars.txdataF[eNb_id];
#ifdef SECONDARY_SYSTEM
	txdataF_ext = PHY_vars_eNb[1]->lte_eNB_common_vars.txdataF[eNb_id_secsys];
#endif
#ifndef IFFT_FPGA
	txdata = PHY_vars_eNb[0]->lte_eNB_common_vars.txdata[eNb_id];
#ifdef SECONDARY_SYSTEM
	txdata_ext = PHY_vars_eNb[1]->lte_eNB_common_vars.txdata[eNb_id_secsys];
#endif
#endif
      }
      else if (subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_UL) {
	txdataF = PHY_vars_UE[0]->lte_ue_common_vars.txdataF;
#ifdef SECONDARY_SYSTEM
	txdataF_ext = PHY_vars_UE[1]->lte_ue_common_vars.txdataF;
#endif
#ifndef IFFT_FPGA
	txdata = PHY_vars_UE[0]->lte_ue_common_vars.txdata;
#ifdef SECONDARY_SYSTEM
	txdata_ext = PHY_vars_UE[1]->lte_ue_common_vars.txdata;
#endif
#endif
      }
      else {//it must be a special subframe
	//which also means that SECONDARY system must listen, and synchronize as an UE, every x(=10) frame(s). PBS located in the 3rd symbol in this slot.
	if (next_slot%2==0) {//DL part
	  txdataF = PHY_vars_eNb[0]->lte_eNB_common_vars.txdataF[eNb_id];
#ifdef SECONDARY_SYSTEM // SEC_SYS should be in Rx-mode here, primary will transmit
	  txdataF_ext = PHY_vars_eNb[1]->lte_eNB_common_vars.txdataF[eNb_id_secsys]; // should point to NULL (though this will now just point to a lot of zeros, since phy_procedures routine will not generate PSS).
#endif
#ifndef IFFT_FPGA
	  txdata = PHY_vars_eNb[0]->lte_eNB_common_vars.txdata[eNb_id];
#ifdef SECONDARY_SYSTEM // SEC_SYS should be in Rx-mode here, primary will transmit
	  txdata_ext = PHY_vars_eNb[1]->lte_eNB_common_vars.txdata[eNb_id_secsys]; // should point to NULL (though this will now just point to a lot of zeros, since phy_procedures routine will not generate PSS).
#endif
#endif
	}
	else {// UL part
	  txdataF = PHY_vars_UE[0]->lte_ue_common_vars.txdataF;
#ifdef SECONDARY_SYSTEM // S_eNb should be in Rx-mode here
	  txdataF_ext = PHY_vars_UE[1]->lte_ue_common_vars.txdataF;
#endif
#ifndef IFFT_FPGA
	  txdata = PHY_vars_UE[0]->lte_ue_common_vars.txdata;
#ifdef SECONDARY_SYSTEM // S_eNb should be in Rx-mode here
	  txdata_ext = PHY_vars_UE[1]->lte_ue_common_vars.txdata;
#endif
#endif
	}
      }

#ifdef IFFT_FPGA

      slot_offset = (next_slot)*(lte_frame_parms->N_RB_DL*12)*((lte_frame_parms->Ncp==1) ? 6 : 7);

      //write_output("eNb_txsigF0.m","eNb_txsF0", PHY_vars_eNb->lte_eNB_common_vars->txdataF[eNb_id][0],300*120,1,4);
      //write_output("eNb_txsigF1.m","eNb_txsF1", PHY_vars_eNb->lte_eNB_common_vars->txdataF[eNb_id][1],300*120,1,4);

      
      // do table lookup and write results to txdataF2
      for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {

	l = slot_offset;	
	for (i=0;i<NUMBER_OF_OFDM_CARRIERS*((lte_frame_parms->Ncp==1) ? 6 : 7);i++) 
	  if ((i%512>=1) && (i%512<=150))
	    txdataF2[aa][i] = ((int*)mod_table)[txdataF[aa][l++]];
	  else if (i%512>=362)
	    txdataF2[aa][i] = ((int*)mod_table)[txdataF[aa][l++]];
	  else 
	    txdataF2[aa][i] = 0;
#ifdef SECONDARY_SYSTEM
	  if ((i%512>=1) && (i%512<=150))
	    txdataF2_secsys[aa][i] = ((int*)mod_table)[txdataF_ext[aa][l++]];
	  else if (i%512>=362)
	    txdataF2_secsys[aa][i] = ((int*)mod_table)[txdataF_ext[aa][l++]];
	  else 
	    txdataF2_secsys[aa][i] = 0;
#endif
      }
      
      for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) 
	PHY_ofdm_mod(txdataF2[aa],        // input
		     txdata[aa],         // output
		     lte_frame_parms->log2_symbol_size,                // log2_fft_size
		     (lte_frame_parms->Ncp==1) ? 6 : 7,                 // number of symbols
		     lte_frame_parms->nb_prefix_samples,               // number of prefix samples
		     lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
		     lte_frame_parms->rev,           // bit-reversal permutation
		     CYCLIC_PREFIX);
#ifdef SECONDARY_SYSTEM
      for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) 
	PHY_ofdm_mod(txdataF2_secsys[aa],        // input
		     txdata_ext[aa],         // output
		     lte_frame_parms->log2_symbol_size,
		     (lte_frame_parms->Ncp==1) ? 6 : 7,
		     lte_frame_parms->nb_prefix_samples,
		     lte_frame_parms->twiddle_ifft,
		     lte_frame_parms->rev,
		     CYCLIC_PREFIX);
#endif //SECONDARY_SYSTE
      
#else //IFFT_FPGA

      slot_offset = (next_slot)*(lte_frame_parms->ofdm_symbol_size)*((lte_frame_parms->Ncp==1) ? 6 : 7);
#ifndef PBS_SIM
      //      printf("Copying TX buffer for slot %d (%d)\n",next_slot,slot_offset);		     CYCLIC_PREFIX);
#endif //PBS_SIM
      slot_offset_time = (next_slot)*(lte_frame_parms->samples_per_tti>>1);

      for (aa=0; aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx; aa++) {
	PHY_ofdm_mod(&txdataF[aa][slot_offset],        // input
		     &txdata[aa][slot_offset_time],    // output
		     lte_frame_parms->log2_symbol_size,                // log2_fft_size
		     (lte_frame_parms->Ncp==1) ? 6 : 7,                 // number of symbols
		     lte_frame_parms->nb_prefix_samples,               // number of prefix samples
		     lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
		     lte_frame_parms->rev,           // bit-reversal permutation
		     CYCLIC_PREFIX);
      }
#ifndef PBS_SIM
      if (next_slot==7) {
	write_output("txdata_f0.m","txs_f0",&txdataF[0][slot_offset],(lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti>>1),1,1);
	if (PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx == 2)
	  write_output("txdata_f1.m","txs_f1",&txdataF[1][slot_offset],(lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti>>1),1,1);
	write_output("txdata_t0.m","txs_t0",&txdata[0][slot_offset_time],lte_frame_parms->samples_per_tti>>1,1,1);
	if (PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx == 2)
	  write_output("txdata_t1.m","txs_t1",&txdata[1][slot_offset_time],lte_frame_parms->samples_per_tti>>1,1,1);
      }
#endif //PBS_SIM
 
#ifdef SECONDARY_SYSTEM
	
	for (aa=0; aa<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx); aa++) {
	  PHY_ofdm_mod(&txdataF_ext[aa][slot_offset],// input
		       &txdata_ext[aa][slot_offset_time],// output
		       lte_frame_parms->log2_symbol_size,
		       (lte_frame_parms->Ncp==1) ? 6 : 7,
		       lte_frame_parms->nb_prefix_samples,
		       lte_frame_parms->twiddle_ifft,
		       lte_frame_parms->rev, CYCLIC_PREFIX);
	}  
#ifndef PBS_SIM
	if (next_slot==7) {
	  write_output("txdata_f0.m","txs_f0",&txdataF_ext[0][slot_offset],(lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti>>1),1,1);
	  if ((PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx) == 2)
	  write_output("txdata_f1.m","txs_f1",&txdataF_ext[1][slot_offset],(lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti>>1),1,1);
	  write_output("txdata_t0.m","txs_t0",&txdata_ext[0][slot_offset_time],lte_frame_parms->samples_per_tti>>1,1,1);
	  if ((PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx) == 2)
	  write_output("txdata_t1.m","txs_t1",&txdata_ext[1][slot_offset_time],lte_frame_parms->samples_per_tti>>1,1,1);
	}
#endif //PBS_SIM
#endif //SECONDARY_SYSTEM
#endif //IFFT_FPGA

    
      /*-------------------------------------------------------------
	ASSIGN POINTERS TO CORRECT BUFFERS ACCORDING TO TDD-STRUCTURE
	                  -----  RX PART  -----
      ---------------------------------------------------------------*/
  if (subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL)
    rxdata = PHY_vars_UE[0]->lte_ue_common_vars.rxdata;
  else if (subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_UL)
    rxdata = PHY_vars_eNb[0]->lte_eNB_common_vars.rxdata[eNb_id];
  else //it must be a special subframe
    if (next_slot%2==0) //DL part
      rxdata = PHY_vars_UE[0]->lte_ue_common_vars.rxdata;
    else // UL part
      rxdata = PHY_vars_eNb[0]->lte_eNB_common_vars.rxdata[eNb_id];
 

#ifdef SECONDARY_SYSTEM

  if (subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) {
    rxdata_ext[0] = PHY_vars_UE[1]->lte_ue_common_vars.rxdata;
    rxdata_ext[1] = PHY_vars_UE[2]->lte_ue_common_vars.rxdata;
  } 
  else if (subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_UL) {
    rxdata_ext[0] = PHY_vars_eNb[1]->lte_eNB_common_vars.rxdata[eNb_id_secsys];
    rxdata_ext[1] = PHY_vars_eNb[2]->lte_eNB_common_vars.rxdata[eNb_id];
  }
  else {//it must be a special subframe
    //which also means that SECONDARY system must listen, and synchronize as an UE, every x(=100) frame(s) or every frame on power up. PSS is located in the 3rd symbol in this slot.
    if (PHY_vars_eNb[1]->is_init_sync && mac_xface->frame%100>0) {
      if (next_slot%2==0) { //DL part
	rxdata_ext[0] = PHY_vars_UE[1]->lte_ue_common_vars.rxdata;
	//make sure SeNb rx_buffer is empty from the time when it listened and synchronized	
	for (aa=0;aa<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx;aa++)
	  memset(&PHY_vars_eNb[1]->lte_eNB_common_vars.rxdata[eNb_id_secsys][aa][next_slot*((PHY_vars_eNb[1]->lte_frame_parms.nb_prefix_samples+PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size)*(PHY_vars_eNb[1]->lte_frame_parms.symbols_per_tti>>1))],0,(PHY_vars_eNb[1]->lte_frame_parms.nb_prefix_samples+PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size)*(PHY_vars_eNb[1]->lte_frame_parms.symbols_per_tti>>1)*sizeof(mod_sym_t));
      }
      else {  //UL part
	rxdata_ext[0] = PHY_vars_eNb[1]->lte_eNB_common_vars.rxdata[eNb_id_secsys];
      }
    } 
    else { //Listen (in both SF slots) and synchronize 
      	rxdata_ext[0] = PHY_vars_eNb[1]->lte_eNB_common_vars.rxdata[eNb_id_secsys];
    }
    // virtual primary system applies regular configuration
    if (next_slot%2==0) { //DL part
      rxdata_ext[1] = PHY_vars_UE[2]->lte_ue_common_vars.rxdata;
    }
    else {  //UL part
      rxdata_ext[1] = PHY_vars_eNb[2]->lte_eNB_common_vars.rxdata[eNb_id];
    }
  }


#endif //SECONDARY_SYSTEM
    
      /*-------------------------------------------------------------
	                 TRANSMISSION SIMULATION
      ---------------------------------------------------------------*/
  
#ifdef SKIP_RF_CHAIN
      // get pointer to data ready to be transmitted
      for (i=0;i<(PHY_vars_eNb[0]->lte_frame_parms.samples_per_tti>>1);i++) {
	for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx;aa++) {
	  s_re[aa][i] = (double)(((short *)txdata[aa])[(slot_offset_time+i)<<1]);
	  s_im[aa][i] = (double)(((short *)txdata[aa])[((slot_offset_time+i)<<1)+1]);
	}
#ifdef SECONDARY_SYSTEM
	for (aa=0;aa<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx);aa++) {
	  s_re_secsys[aa][i] = (double)(((short *)txdata_ext[aa])[(slot_offset_time+i)<<1]);
	  s_im_secsys[aa][i] = (double)(((short *)txdata_ext[aa])[((slot_offset_time+i)<<1)+1]);
	}
#endif //SECONDARY_SYSTEM
      }

      for (i=0;i<(PHY_vars_eNb[0]->lte_frame_parms.samples_per_tti>>1);i++) {
	for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx;aa++) {
	  for (j=0; j<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx;j++) {
	    r_re[aa][i] = s_re[aa][i]*ch_ar[PePu][aa][0].r - s_im[aa][i]*ch_ar[PePu][aa][0].i + 0;
	    r_im[aa][i] = s_im[aa][i]*ch_ar[PePu][aa][0].r + s_re[aa][i]*ch_ar[PePu][aa][0].i + 0;
	  }
	}
      }
#ifdef SECONDARY_SYSTEM
      for (i=0;i<(PHY_vars_eNb[0]->lte_frame_parms.samples_per_tti>>1);i++) {
	for (aa=0;aa<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx);aa++) {
	  for (j=0; j<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx;j++) {
	    r_re_ext[0][aa][i] += s_re_secsys[j][i]*ch_ar[SeSu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].r - s_im_secsys[j][i]*ch_ar[SeSu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    r_im_ext[0][aa][i] += s_im_secsys[j][i]*ch_ar[SeSu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].r + s_re_secsys[j][i]*ch_ar[SeSu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	  }
	}
	
	// signal from primary system to secondary system
	for (aa=0;aa<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx);aa++) {
	  for (j=0; j<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx;j++) {
	    if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
	    r_re_ext[0][aa][i] += s_re[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx)][0].r - s_im[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    r_im_ext[0][aa][i] += s_im[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx)][0].r + s_re[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    }
	    else { //UL
	    r_re_ext[0][aa][i] += s_re[j][i]*ch_ar[SePu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].r - s_im[j][i]*ch_ar[SePu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    r_im_ext[0][aa][i] += s_im[j][i]*ch_ar[SePu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].r + s_re[j][i]*ch_ar[SePu][aa + j*(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    }
	  }
	}
	
	// only signal from secondary to primary (to the virtual receivers)
	for (aa=0;aa<PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_rx;aa++) { //can be outer loop as long as eNb and UE share the same number of Rx antennas
	  for (j=0; j<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx;j++) {
	    if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
	      r_re_ext[1][aa][i] += s_re_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[2]->lte_frame_parms.nb_antennas_rx)][0].r - s_im_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[2]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	      r_im_ext[1][aa][i] += s_im_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[2]->lte_frame_parms.nb_antennas_rx)][0].r + s_re_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[2]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    }
	    else { //UL
	      r_re_ext[1][aa][i] += s_re_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_rx)][0].r - s_im_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	      r_im_ext[1][aa][i] += s_im_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_rx)][0].r + s_re_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    }
	  }
	}

	// signal from secondary to primary
	for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx;aa++) { //can be outer loop as long as eNb and UE share the same number of Rx antennas
	  for (j=0; j<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx;j++) {
	    if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
	      r_re[aa][i] += s_re_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx)][0].r - s_im_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	      r_im[aa][i] += s_im_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx)][0].r + s_re_secsys[j][i]*ch_ar[SePu][aa + j*(PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    }
	    else { //UL
	      r_re[aa][i] += s_re_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx)][0].r - s_im_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	      r_im[aa][i] += s_im_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx)][0].r + s_re_secsys[j][i]*ch_ar[PeSu][aa + j*(PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx)][0].i + 0;
	    }
	  }
	}
      }
#endif //SECONDARY_SYSTEM

  slot_offset_time = next_slot*(PHY_vars_eNb[0]->lte_frame_parms.samples_per_tti>>1);
      for (i=0;i<(PHY_vars_eNb[0]->lte_frame_parms.samples_per_tti>>1);i++) {
	for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx;aa++) {
	  ((short *)rxdata[aa])[((i+slot_offset_time)<<1)]   = (short)(r_re[aa][i+0]*1);
	  ((short *)rxdata[aa])[1+((i+slot_offset_time)<<1)] = (short)(r_im[aa][i+0]*1);
#ifdef SECONDARY_SYSTEM
	  ((short *)rxdata_ext[1][aa])[((i+slot_offset_time)<<1)]   = (short)(r_re_ext[1][aa][i+0]*1);
	  ((short *)rxdata_ext[1][aa])[1+((i+slot_offset_time)<<1)] = (short)(r_im_ext[1][aa][i+0]*1);
	}
	for (aa=0;aa<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx;aa++) {
	  ((short *)rxdata_ext[0][aa])[((i+slot_offset_time)<<1)]   = (short)(r_re_ext[0][aa][i+0]*1);
	  ((short *)rxdata_ext[0][aa])[1+((i+slot_offset_time)<<1)] = (short)(r_im_ext[0][aa][i+0]*1);
#endif
	}
      }
    
#else //SKIP_RF_CHAIN

      /*-------------------------------------------------------------
                                   D/A
      ---------------------------------------------------------------*/

      // convert to floating point
      tx_pwr = dac_fixed_gain(s_re,
			      s_im,
			      txdata,
			      slot_offset_time,
			      PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,
			      lte_frame_parms->samples_per_tti>>1,
			      14,
			      18+5); // -(20log10(1024)-20log10(2^13)) ~= 18, +5 to adjust DCI to have ~0dBm
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
	tx_pwr = 6*signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,0); // RSs only on every sixth --> *6
      } else { // UL
	if (next_slot%2==0)
	  tx_pwr = signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640*2,640*3);
	else 
	  tx_pwr = 2*signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640*5); // SRSs only on every second --> *2
      }
#ifndef PBS_SIM
      printf("tx_pwr Primary   %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr),next_slot,next_slot>>1);
#endif //PBS_SIM

      if (next_slot == 7) {
	tx_pwr_SRS = tx_pwr;
      }
#ifndef PBS_SIM
      if (next_slot>>1 == 6 || next_slot>>1 == 0) {
	tx_pwr_SRS = 6*signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,0);
	printf("tx_pwr %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_SRS),next_slot,next_slot>>1);
	tx_pwr_SRS = signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640);
	printf("tx_pwr %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_SRS),next_slot,next_slot>>1);
	tx_pwr_SRS = signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640*2);
	printf("tx_pwr %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_SRS),next_slot,next_slot>>1);
	tx_pwr_SRS = 6*signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640*3);
	printf("tx_pwr %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_SRS),next_slot,next_slot>>1);
	tx_pwr_SRS = signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640*4);
	printf("tx_pwr %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_SRS),next_slot,next_slot>>1);
	tx_pwr_SRS = signal_energy_fp(s_re,s_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640*5);
	printf("tx_pwr %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_SRS),next_slot,next_slot>>1);
      }
      if (next_slot>>1 == 6 || next_slot>>1 == 0) {
	printf("tx_pwr %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr),next_slot,next_slot>>1);
      }
#endif //PBS_SIM
      
#ifdef SECONDARY_SYSTEM
      // convert to floating point
      dac_fixed_gain(s_re_secsys,
		     s_im_secsys,
		     txdata_ext,
		     slot_offset_time,
		     PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx,
		     lte_frame_parms->samples_per_tti>>1,
		     14,
		     18+5-3); // -(20log10(1024)-20log10(2^13)) ~= 18, +5 to adjust DCI to have ~0dBm
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
      tx_pwr_secsys = 6*signal_energy_fp(s_re_secsys,s_im_secsys,1,640,0); // RSs only on every sixth --> *6
      tx_pwr_secsys += 6*signal_energy_fp(&s_re_secsys[1],&s_im_secsys[1],1,640,0); // RSs only on every sixth --> *6
      } else { // UL
	if (next_slot%2==0) {
	  tx_pwr_secsys = signal_energy_fp(s_re_secsys,s_im_secsys,1,640*2,640*3);
	  tx_pwr_secsys += signal_energy_fp(&s_re_secsys[1],&s_im_secsys[1],1,640*2,640*3);
	}
	else {
	  tx_pwr_secsys = 2*signal_energy_fp(s_re_secsys,s_im_secsys,1,640,640*5); // SRSs only on every second --> *2
	  tx_pwr_secsys += 2*signal_energy_fp(&s_re_secsys[1],&s_im_secsys[1],1,640,640*5); // SRSs only on every second --> *2
	}
      }
#ifndef PBS_SIM
      printf("tx_pwr Secondary before %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_secsys),next_slot,next_slot>>1);
#endif //PBS_SIM
      
      if (next_slot == 9) {
	//printf("tx pwr secondary UE  = %lf dB\n", 10*log10(tx_pwr_secsys)); //indicates the power of the B/F vector
	tx_pwr = tx_pwr_SRS;
	//printf("tx pwr ratio = %lf dB\n", 10*log10(tx_pwr/tx_pwr_secsys));
      }
      // for convenience call same function for power scaling, but only if there is data to send i.e. tx_pwr_secsys>0
      if (tx_pwr_secsys>0) {
	if (tx_pwr_secsys > tx_pwr)
	  tx_pwr_secsys = dac_fixed_gain(s_re_secsys,
					 s_im_secsys,
					 txdata_ext,
					 slot_offset_time,
					 PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx,
					 lte_frame_parms->samples_per_tti>>1,
					 14,
					 18+5-3+STxGain- ((tx_pwr) ? (10*log10(tx_pwr_secsys/tx_pwr)) : 0)); // -(20log10(1024)-20log10(2^13)) ~= 18, +5 to adjust DCI to have ~0dBm, -3 because of two tx antennas
	else
	  tx_pwr_secsys = dac_fixed_gain(s_re_secsys,
					 s_im_secsys,
					 txdata_ext,
					 slot_offset_time,
					 PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx,
					 lte_frame_parms->samples_per_tti>>1,
					 14,
					 18+5-3+STxGain+ ((tx_pwr_secsys) ? (10*log10(tx_pwr/tx_pwr_secsys)) : 0)); // -(20log10(1024)-20log10(2^13)) ~= 18, +5 to adjust DCI to have ~0dBm, -3 because of two tx antennas
      } //else if(next_slot==10 || next_slot==9) {
      //plot_flag = 1;
      //}
#ifndef PBS_SIM
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
      tx_pwr_secsys = 6*signal_energy_fp(s_re_secsys,s_im_secsys,1,640,0); // RSs only on every sixth --> *6
      tx_pwr_secsys += 6*signal_energy_fp(&s_re_secsys[1],&s_im_secsys[1],1,640,0); // RSs only on every sixth --> *6
      } else { // UL
	if (next_slot%2==0) {
	  tx_pwr_secsys = signal_energy_fp(s_re_secsys,s_im_secsys,1,640*2,640*3);
	  tx_pwr_secsys += signal_energy_fp(&s_re_secsys[1],&s_im_secsys[1],1,640*2,640*3);
	}
	else {
	  tx_pwr_secsys = 2*signal_energy_fp(s_re_secsys,s_im_secsys,1,640,640*5); // SRSs only on every second --> *2
	  tx_pwr_secsys += 2*signal_energy_fp(&s_re_secsys[1],&s_im_secsys[1],1,640,640*5); // SRSs only on every second --> *2
	}
      }
      
      printf("tx_pwr Secondary %f dB for slot %d (subframe %d)\n",10*log10(tx_pwr_secsys),next_slot,next_slot>>1);
	/*
	if (next_slot==11) {
	  write_output("txdata_t_a0.m","txs_t_a0",&txdata_ext[0][slot_offset_time],lte_frame_parms->samples_per_tti>>1,1,1);
	  write_output("txdata_t_a1.m","txs_t_a1",&txdata_ext[1][slot_offset_time],lte_frame_parms->samples_per_tti>>1,1,1);
	  write_output("txdata_t_real_a0.m","txs_t_r_a0",s_re_secsys[0],lte_frame_parms->samples_per_tti>>1,1,7);
	  write_output("txdata_t_real_a1.m","txs_t_r_a1",s_re_secsys[1],lte_frame_parms->samples_per_tti>>1,1,7);
	}
	*/
#endif //PBS_SIM
#endif //SECONDARY_SYSTEM
   
    
      /*-------------------------------------------------------------
	                     CHANNEL MODEL 
                         ANTENNA(s) TO ANTENNA(s)
      ---------------------------------------------------------------*/
#ifndef PBS_SIM
	if ((last_slot==1 || last_slot==10) && mac_xface->frame>0) {
	  for (aarx=0; aarx<PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx; aarx++) {
	    for (aatx=0; aatx<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx; aatx++) {
	      sprintf(fname,"dl_ch_estimate_prim_%i%i.m",aatx,aarx);
	      sprintf(vname,"dl_ch_est_p_%i%i",aatx,aarx);
	      write_output(fname,vname,PHY_vars_UE[1]->lte_ue_common_vars.dl_ch_estimates[0][(2*aatx)+aarx],PHY_vars_UE[1]->lte_frame_parms.ofdm_symbol_size*PHY_vars_UE[1]->lte_frame_parms.symbols_per_tti,1,1);
	    }
	  }
	  for (aarx=0; aarx<PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx; aarx++) {
	    for (aatx=0; aatx<(PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx); aatx++) {
	      sprintf(fname,"dl_ch_estimate_seco_%i%i.m",aatx,aarx);
	      sprintf(vname,"dl_ch_est_s_%i%i",aatx,aarx);
	      write_output(fname,vname,PHY_vars_UE[1]->lte_ue_common_vars.dl_ch_estimates[1][(2*aatx)+aarx],PHY_vars_UE[1]->lte_frame_parms.ofdm_symbol_size*PHY_vars_UE[1]->lte_frame_parms.symbols_per_tti,1,1);
	    }
	  }
	}
 
#endif //PBS_SIM    
    
	multipath_channel(
#ifdef SECONDARY_SYSTEM
			  ch_ar[PePu],
#else
			  ch,
#endif
			  s_re,s_im,r_re,r_im,
			  amps,Td,BW,ricean_factor,
#ifdef SECONDARY_SYSTEM
			  aoa_ar[PePu],
#else
			  aoa,
#endif
			  PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,
			  PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx,
			  OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			  channel_length,
			  0,
			  forgetting_factor, //forgetting factor (temporal variation, block stationary)
			  ((first_call == 1) ? 1 : 0),
#ifdef SECONDARY_SYSTEM
			  (has_channel) ? 1 : 
#endif			
			  ((next_slot==2 || first_call==1) ? 0 : 1),
#ifdef SECONDARY_SYSTEM
			  PePu,
#else
			  0,
#endif	    
			  0);
	if (first_call == 1)
	  first_call = 0;

#ifdef SECONDARY_SYSTEM 
#ifndef PBS_SIM  
	//      printf("channel for slot %d (subframe %d)\n",next_slot,next_slot>>1);
#endif //PBS_SIM
	// SeNb to S_UE
	multipath_channel(ch_ar[SeSu],s_re_secsys,s_im_secsys,
			  r_re_ext[0],r_im_ext[0],
			  amps,Td,BW,ricean_factor,aoa_ar[SeSu],
			  PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx,
			  PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx,
			  OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			  channel_length,
			  0,
			  forgetting_factor, //forgetting factor (temporal variation, block stationary)
			  ((first_call_secsys == 1) ? 1 : 0),
			  (has_channel) ? 1 : ((next_slot==2 || first_call_secsys==1) ? 0 : 1),
			  SeSu,0);
#ifdef DEBUG_PHY
      msg("ch_ar[%i][0][0].r = %lf\n",SeSu,ch_ar[SeSu][0][0].r);
#endif //DEBUG_PHY

      // channel models for interference paths
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
	// from PeNb to SeNb
	multipath_channel(ch_ar[PeSe],s_re,s_im,
			  r_re_crossLink[PeSe],r_im_crossLink[PeSe],
			  amps,Td,BW,ricean_factor,aoa_ar[PeSe],
			  PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,
			  PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx,
			  OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			  channel_length,
			  0,
			  forgetting_factor, //forgetting factor (temporal variation, block stationary)
			  ((first_call_secsys == 1) ? 1 : 0),
			  (has_channel) ? 1 : ((next_slot==2 || first_call_secsys==1) ? 0 : 1),
			  PeSe,
			  0);
#ifdef DEBUG_PHY
      msg("ch_ar[%i][0][0].r = %lf\n",PeSe,ch_ar[PeSe][0][0].r);
#endif //DEBUG_PHY
	// from PeNb to S_UE
	multipath_channel(ch_ar[PeSu],s_re,s_im,
			  r_re_crossLink[PeSu],r_im_crossLink[PeSu],
			  amps,Td,BW,ricean_factor,aoa_ar[PeSu],
			  PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,
			  PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx,
			  OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			  channel_length,
			  0,
			  forgetting_factor, //forgetting factor (temporal variation, block stationary)
			  ((first_call_secsys == 1) ? 1 : 0),
			  (has_channel) ? 1 : ((next_slot==2 || first_call_secsys==1) ? 0 : 1),
			  PeSu,
			  0);
#ifdef DEBUG_PHY
      msg("ch_ar[%i][0][0].r = %lf\n",PeSu,ch_ar[PeSu][0][0].r);
#endif //DEBUG_PHY
	// from SeNb to P_UE
	multipath_channel(ch_ar[SePu],s_re_secsys,s_im_secsys,
			  r_re_crossLink[SePu],r_im_crossLink[SePu],
			  amps,Td,BW,ricean_factor,aoa_ar[SePu],
			  PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_tx,
			  PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx,
			  OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			  channel_length,
			  0,
			  forgetting_factor, //forgetting factor (temporal variation, block stationary)
			  ((first_call_secsys == 1) ? 1 : 0),
			  (has_channel) ? 1 : ((next_slot==2 || first_call_secsys==1) ? 0 : 1),
			  SePu,
			  0);
#ifdef DEBUG_PHY
      msg("ch_ar[i][0][0].r = %lf\n",SePu,ch_ar[SePu][0][0].r);
#endif //DEBUG_PHY
      } else { //UL
	// from P_UE to SeNb
	multipath_channel(ch_ar[SePu],s_re,s_im,
			  r_re_crossLink[SePu],r_im_crossLink[SePu],
			  amps,Td,BW,ricean_factor,aoa_ar[SePu],
			  PHY_vars_UE[0]->lte_frame_parms.nb_antennas_tx,
			  PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx,
			  OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			  channel_length,
			  0,
			  forgetting_factor, //forgetting factor (temporal variation, block stationary)
			  ((first_call_secsys == 1) ? 1 : 0),
			  (has_channel) ? 1 : ((next_slot==2 || first_call_secsys==1) ? 0 : 1),
			  SePu,
			  0);
#ifdef DEBUG_PHY
      msg("ch_ar[%i][0][0].r = %lf\n",SePu,ch_ar[SePu][0][0].r);
#endif //DEBUG_PHY
	// from P_UE to S_UE
      multipath_channel(ch_ar[PuSu],s_re,s_im,
			r_re_crossLink[PuSu],r_im_crossLink[PuSu],
			amps,Td,BW,ricean_factor,aoa_ar[PuSu],
			PHY_vars_UE[0]->lte_frame_parms.nb_antennas_tx,
			PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx,
			OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			channel_length,
			0,
			forgetting_factor, //forgetting factor (temporal variation, block stationary)
			((first_call_secsys == 1) ? 1 : 0),
			(has_channel) ? 1 : ((next_slot==2 || first_call_secsys==1) ? 0 : 1),
			PuSu,
			0);
#ifdef DEBUG_PHY
      msg("ch_ar[%i][0][0].r = %lf\n",PuSu,ch_ar[PuSu][0][0].r);
#endif //DEBUG_PHY
      // from S_UE to P_eNb
      multipath_channel(ch_ar[PeSu],s_re_secsys,s_im_secsys,
			r_re_crossLink[PeSu],r_im_crossLink[PeSu],
			amps,Td,BW,ricean_factor,aoa_ar[PeSu],
			PHY_vars_UE[1]->lte_frame_parms.nb_antennas_tx,
			PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx,
			OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*(7-lte_frame_parms->Ncp),
			channel_length,
			0,
			forgetting_factor, //forgetting factor (temporal variation, block stationary)
			((first_call_secsys == 1) ? 1 : 0),
			(has_channel) ? 1 : ((next_slot==2 || first_call_secsys==1) ? 0 : 1),
			PeSu,
			0);
#ifdef DEBUG_PHY
      msg("ch_ar[%i][0][0].r = %lf\n",PeSu,ch_ar[PeSu][0][0].r);
#endif //DEBUG_PHY
      }
      if (first_call_secsys == 1)
	first_call_secsys = 0;

#endif //SECONDARY_SYSTEM 
    
#ifdef DEBUG_PHY
  if (next_slot==10) {    
    write_output("UE1_rxs_pilot_cl_re.m","UE1_rxs_cl_p_r",r_re_crossLink[PeSu][0],640,1,7);
    write_output("UE1_rxs_pilot_cl_im.m","UE1_rxs_cl_p_i",r_im_crossLink[PeSu][0],640,1,7);    
    write_output("UE1_txs_pilot_re.m","UE1_txs_p_r",s_re_secsys[0],640,1,7);
    write_output("UE1_txs_pilot_im.m","UE1_txs_p_i",s_im_secsys[0],640,1,7);    
    write_output("UE1_rxs_pilot_re.m","UE1_rxs_p_r",r_re_ext[0][0],640,1,7);
    write_output("UE1_rxs_pilot_im.m","UE1_rxs_p_i",r_im_ext[0][0],640,1,7);
    write_output("eNb1_txF_pilot_a0.m","eNb_txF_p0",&PHY_vars_eNb[1]->lte_eNB_common_vars.txdataF[eNb_id][0][next_slot*PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size*(PHY_vars_eNb[1]->lte_frame_parms.symbols_per_tti>>1)],PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size,1,1);
    write_output("eNb1_txF_pilot_a1.m","eNb_txF_p1",&PHY_vars_eNb[1]->lte_eNB_common_vars.txdataF[eNb_id][1][next_slot*PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size*(PHY_vars_eNb[1]->lte_frame_parms.symbols_per_tti>>1)],PHY_vars_eNb[1]->lte_frame_parms.ofdm_symbol_size,1,1);
  }
#endif //DEBUG_PHY
      for (i=0;i<(lte_frame_parms->samples_per_tti>>1);i++) {
	for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx;aa++) {
	  r_re[aa][i]=r_re[aa][i]*sqrt(path_loss); 
	  r_im[aa][i]=r_im[aa][i]*sqrt(path_loss);
	}
#ifdef SECONDARY_SYSTEM
	for (aa=0;aa<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx;aa++) {
	  r_re_ext[0][aa][i] = r_re_ext[0][aa][i]*sqrt(path_loss_ar[SeSu]); 
	  r_im_ext[0][aa][i] = r_im_ext[0][aa][i]*sqrt(path_loss_ar[SeSu]);
	}
	if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0)) { // Special SF - DL
	  for (aa=0;aa<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx;aa++) {
	    if ((mac_xface->frame%100)==0) { // r_*_ext[0] buffer busy for SeNb --> no primary PSS available for secondary system
	      // from PeNb to SeNb
	      r_re_ext[0][aa][i] += r_re_crossLink[PeSe][aa][i]*sqrt(path_loss_ar[PeSe]); 
	      r_im_ext[0][aa][i] += r_im_crossLink[PeSe][aa][i]*sqrt(path_loss_ar[PeSe]);
	    } else {
	      // from PeNb to S_UE
	      r_re_ext[0][aa][i] += r_re_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	      r_im_ext[0][aa][i] += r_im_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]);
	    }
	  }
	  for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx;aa++) {
	    // from SeNb to P_UE
	    r_re[aa][i] += r_re_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	    r_im[aa][i] += r_im_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	    // from SeNb to virtual P_UE
	    r_re_ext[1][aa][i]  = r_re_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	    r_im_ext[1][aa][i]  = r_im_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	  }
	} 
	else if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==1)) { //Special SF - UL
	  for (aa=0;aa<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx;aa++) {
	    // from P_UE to SeN
	    r_re_ext[0][aa][i] += r_re_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]);
	    r_im_ext[0][aa][i] += r_im_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]);
		}
	  for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx;aa++) {
	    // from S_UE to PeNb
	    r_re[aa][i] += r_re_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	    r_im[aa][i] += r_im_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	    // from S_UE to Virtual PeNb
	    r_re_ext[1][aa][i] = r_re_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	    r_im_ext[1][aa][i] = r_im_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	  }
	} 
	else if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL)) { // DL
	  for (aa=0;aa<PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx;aa++) {
	    // from PeNb to S_UE
	    r_re_ext[0][aa][i] += r_re_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	    r_im_ext[0][aa][i] += r_im_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]);
	  }
	  for (aa=0;aa<PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx;aa++) {
	    // from SeNb to P_UE
	    r_re[aa][i] += r_re_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	    r_im[aa][i] += r_im_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	    // from SeNb to virtual P_UE
	    r_re_ext[1][aa][i] = r_re_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	    r_im_ext[1][aa][i] = r_im_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	  }
	} 
	else { // UL
	  for (aa=0;aa<PHY_vars_eNb[1]->lte_frame_parms.nb_antennas_rx;aa++) {
	    // from P_UE to SeNb
	    //if ((next_slot>>1) != 5) {
	      r_re_ext[0][aa][i] += r_re_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	      r_im_ext[0][aa][i] += r_im_crossLink[SePu][aa][i]*sqrt(path_loss_ar[SePu]); 
	      //}
	  }
	  for (aa=0;aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx;aa++) {
	    // from S_UE to PeNb
	    r_re[aa][i] += r_re_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	    r_im[aa][i] += r_im_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	    // from S_UE to Virtual PeNb
	    r_re_ext[1][aa][i] = r_re_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	    r_im_ext[1][aa][i] = r_im_crossLink[PeSu][aa][i]*sqrt(path_loss_ar[PeSu]); 
	  }
	}
        
#endif //SECONDARY_SYSTEM
      }

      /*-------------------------------------------------------------
	                       RF MODELLING 
                                 RX PART
      ---------------------------------------------------------------*/

      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
#ifdef DISABLE_SECONDARY
	rx_pwr_pre[0] = 6*signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,0); // RSs only on one of three when Secondary is disabled --> *3
#else //DISABLE_SECONDARY
	rx_pwr_pre[0] = (6/2)*signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,0); // RSs only on two of three when Secondary is enabled --> *3/2   (This would be based on the ZBF not working...)
#endif //DISABLE_SECONDARY
	rx_gain[0] = target_rx_pwr_dB - 10*log10(rx_pwr_pre[0]);
	PHY_vars_UE[0]->rx_total_gain_dB = rx_gain[0];
      } else { // UL
	if (next_slot%2==0)
	  rx_pwr_pre[0] = signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640*2,640*3);
	else 
	  rx_pwr_pre[0] = 2*signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640*5); // SRSs only on every second --> *2
	rx_gain[0] = target_rx_pwr_dB - 10*log10(rx_pwr_pre[0]);
	PHY_vars_eNb[0]->rx_total_gain_eNB_dB = rx_gain[0];
      }

#ifndef PBS_SIM
      printf("rx_pwr_pre[%d] (RF in) %f dB for slot %d (subframe %d)\n",0,10*log10(rx_pwr_pre[0]),next_slot,next_slot>>1);
#endif //PBS_SIM

#ifndef SKIP_RF_RX
      rf_rx(r_re,
	    r_im,
	    NULL,
	    NULL,
	    0,
	    PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx,
	    lte_frame_parms->samples_per_tti>>1,
	    1.0/7.68e6 * 1e9,      // sampling time (ns)
	    2,            // freq offset (Hz) (-20kHz..20kHz)
	    0.0,            // drift (Hz) NOT YET IMPLEMENTED
	    nf,             // noise_figure NOT YET IMPLEMENTED
	    (double)rx_gain[0]-66.227,            // rx_gain (dB)
	    200,            // IP3_dBm (dBm)
	    &ip,            // initial phase
	    30.0e3,         // pn_cutoff (kHz)
	    -500.0,          // pn_amp (dBc) default: 50
	    0.0,           // IQ imbalance (dB),
	    0.0);           // IQ phase imbalance (rad)

#else //SKIP_RF_RX
      N0W = pow(10.0,.1*(-174.0 - 10*log10((1.0/7.68e6 * 1e9)*1e-9)));
      rx_gain_lin = pow(10.0,.05*((double)rx_gain[0]-66.227));
      for (aa=0; aa<PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx; aa++) {
	for (i=0; i<PHY_vars_eNb[0]->lte_frame_parms.samples_per_tti>>1; i++) {
	  r_re[aa][i] = rx_gain_lin*(r_re[aa][i] + (sqrt(.5*N0W)*gaussdouble(0.0,1.0)));
	  r_im[aa][i] = rx_gain_lin*(r_im[aa][i] + (sqrt(.5*N0W)*gaussdouble(0.0,1.0)));
	}
      }
#endif //SKIP_RF_RX

      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
#ifdef DISABLE_SECONDARY	
	rx_pwr = 6*signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,0); // RSs only on one of six when Secondary is disabled --> *6
#else //DISABLE_SECONDARY	
	rx_pwr = (6/2)*signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,0);
	 // RSs only on two of six when Secondary is enabled --> *6/2  (This would be based on the ZBF not working...)
#endif //DISABLE_SECONDARY
      } else { // UL
	if (next_slot%2==0)
	  rx_pwr = signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640*2,640*3);
	else 
	  rx_pwr = 2*signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx,640,640*5); // SRSs only on every second --> *2
      }
#ifndef PBS_SIM
      printf("rx_pwr Primary   (ADC in) %f dB for slot %d (subframe %d)\n",10*log10(rx_pwr),next_slot,next_slot>>1);
      rx_pwr = signal_energy_fp(r_re,r_im,PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx,(lte_frame_parms->ofdm_symbol_size+lte_frame_parms->nb_prefix_samples),2*(lte_frame_parms->ofdm_symbol_size+lte_frame_parms->nb_prefix_samples));
#endif //PBS_SIM

#ifdef SECONDARY_SYSTEM
      for (j=0; j<2; j++) {
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
	rx_pwr_pre[j+1] = (6/2)*signal_energy_fp(r_re_ext[j],r_im_ext[j],1,640,0); // RSs only on every sixth --> *6
	if (j==0)
	  rx_pwr_pre[j+1] += (6/2)*signal_energy_fp(&r_re_ext[j][1],&r_im_ext[j][1],1,640,0); // RSs only on every third --> *6
	rx_gain[j+1] = target_rx_pwr_dB - 10*log10(rx_pwr_pre[j+1]) + 3 + 10; // gain should be per antenna, compensate by 3dB
	PHY_vars_UE[j+1]->rx_total_gain_dB = rx_gain[j+1];
      } else { // UL
	if (next_slot%2==0) {
	  rx_pwr_pre[j+1] = signal_energy_fp(r_re_ext[j],r_im_ext[j],1,640*2,640*3);
	  if (j==0)
	    rx_pwr_pre[j+1] += signal_energy_fp(&r_re_ext[j][1],&r_im_ext[j][1],1,640*2,640*3);
	} else {
	  rx_pwr_pre[j+1] = 2*signal_energy_fp(r_re_ext[j],r_im_ext[j],1,640,640*5); // SRSs only on every second --> *2
	  if (j==0)
	    rx_pwr_pre[j+1] += 2*signal_energy_fp(&r_re_ext[j][1],&r_im_ext[j][1],1,640,640*5); // SRSs only on every second --> *2
	}
	rx_gain[j+1] = target_rx_pwr_dB - 10*log10(rx_pwr_pre[j+1]) + 3 + 10; // gain should be per antenna, compensate by 3dB
	PHY_vars_eNb[j+1]->rx_total_gain_eNB_dB = rx_gain[j+1];
      }

#ifndef PBS_SIM
	printf("rx_pwr_pre[%d] (RF in) %f dB for slot %d (subframe %d)\n",j+1,10*log10(rx_pwr_pre[j+1]),next_slot,next_slot>>1);
#endif //PBS_SIM      
	// RF model
#ifndef SKIP_RF_RX
	rf_rx(r_re_ext[j],
	      r_im_ext[j],
	      NULL,
	      NULL,
	      0,
	      PHY_vars_eNb[j+1]->lte_frame_parms.nb_antennas_rx,
	      lte_frame_parms->samples_per_tti>>1,
	      1.0/7.68e6 * 1e9,      // sampling time (ns)
	      2,            // freq offset (Hz) (-20kHz..20kHz)
	      0.0,            // drift (Hz) NOT YET IMPLEMENTED
	      nf,             // noise_figure NOT YET IMPLEMENTED
	      (double)rx_gain[j+1]-66.227,            // rx_gain (dB)
	      200,            // IP3_dBm (dBm)
	      &ip,            // initial phase
	      30.0e3,         // pn_cutoff (kHz)
	      -500.0,          // pn_amp (dBc) default: 50
	      0.0,           // IQ imbalance (dB),
	      0.0);           // IQ phase imbalance (rad)
      
#else //SKIP_RF_RX
	N0W = pow(10.0,.1*(-174.0 - 10*log10((1.0/7.68e6 * 1e9)*1e-9)));
	rx_gain_lin = pow(10.0,.05*((double)rx_gain[j+1]-66.227));
	for (aa=0; aa<PHY_vars_eNb[j+1]->lte_frame_parms.nb_antennas_rx; aa++) {
	  for (i=0; i<PHY_vars_eNb[j+1]->lte_frame_parms.samples_per_tti>>1; i++) {
	    r_re_ext[j][aa][i] = rx_gain_lin*(r_re_ext[j][aa][i] + (sqrt(.5*N0W)*gaussdouble(0.0,1.0)));
	    r_im_ext[j][aa][i] = rx_gain_lin*(r_im_ext[j][aa][i] + (sqrt(.5*N0W)*gaussdouble(0.0,1.0)));
	  }
	}
	
#endif //SKIP_RF_RX
#ifndef PBS_SIM
	if (j==0) {
	  printf("Rx Gain Primary   : %d (slot %d)\n",rx_gain[0],next_slot);
	  printf("Rx Gain Secondary : %d (slot %d)\n",rx_gain[1],next_slot);
	}
#endif //PBS_SIM

      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
	rx_pwr_sec[j] = (6/2)*signal_energy_fp(r_re_ext[j],r_im_ext[j],1,640,0); // RSs only on every sixth --> *6
	if (j==0)
	  rx_pwr_sec[j] += (6/2)*signal_energy_fp(&r_re_ext[j][1],&r_im_ext[j][1],1,640,0); // RSs only on every sixth --> *6
      } else { // UL
	if (next_slot%2==0) {
	  rx_pwr_sec[j] = signal_energy_fp(r_re_ext[j],r_im_ext[j],1,640*2,640*3);
	  if (j==0)
	    rx_pwr_sec[j] += signal_energy_fp(&r_re_ext[j][1],&r_im_ext[j][1],1,640*2,640*3);
	} else {
	  rx_pwr_sec[j] = 2*signal_energy_fp(r_re_ext[j],r_im_ext[j],1,640,640*5); // SRSs only on every second --> *2
	  if (j==0)
	    rx_pwr_sec[j] += 2*signal_energy_fp(&r_re_ext[j][1],&r_im_ext[j][1],1,640,640*5); // SRSs only on every second --> *2
	}
      }

#ifndef PBS_SIM 
      if (j==0)
	printf("rx_pwr Secondary (ADC in) %f dB for slot %d (subframe %d)\n",10*log10(rx_pwr_sec[j]),next_slot,next_slot>>1);
#endif //PBS_SIM
      } //loop over secondary transmission simulations
#endif //SECONDARY_SYSTEM

      /*-------------------------------------------------------------
	                     A/D CONVERSION
			     (QUANTIZATION)
      ---------------------------------------------------------------*/

  adc(r_re,
      r_im,
      0,
      slot_offset_time,
      rxdata,
      PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx,
      lte_frame_parms->samples_per_tti>>1,
      12);
  
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL
#ifdef DISABLE_SECONDARY	
	rx_pwr2 = 6*signal_energy(rxdata[0]+slot_offset_time,640); // RSs only on one of three when Secondary is disabled --> *3
#else //DISABLE_SECONDARY	
	rx_pwr2 = (6/2)*signal_energy(rxdata[0]+slot_offset_time,640);
	 // RSs only on two of three when Secondary is enabled --> *3/2  (This would be based on the ZBF not working...)
#endif //DISABLE_SECONDARY
      } else { // UL
	if (next_slot%2==0)
	  rx_pwr2 = signal_energy(rxdata[0]+slot_offset_time+640*3,640*2);
	else 
	  rx_pwr2 = 2*signal_energy(rxdata[0]+slot_offset_time+5*(lte_frame_parms->samples_per_tti>>1)/6,640); // SRSs only on every second --> *2
      }
#ifndef PBS_SIM 
    printf("rx_pwr Primary   (ADC out) %f dB (%d) for slot %d (subframe %d)\n",10*log10((double)rx_pwr2),rx_pwr2,next_slot,next_slot>>1); 
#endif //PBS_SIM   
    if (next_slot == 12)
      printf("rx_pwr Primary   (ADC out) %f dB (%d) for slot %d (subframe %d)\n",10*log10((double)rx_pwr2),rx_pwr2,next_slot,next_slot>>1); 
  
#ifdef SECONDARY_SYSTEM
  for (j=0; j<2; j++) {
    adc(r_re_ext[j],
	r_im_ext[j],
	0,
	slot_offset_time,
	rxdata_ext[j],
	PHY_vars_eNb[j+1]->lte_frame_parms.nb_antennas_rx,
	lte_frame_parms->samples_per_tti>>1,
	12);
    
      if ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_DL) || ((subframe_select_tdd(lte_frame_parms->tdd_config,next_slot>>1) == SF_S) && (next_slot%2==0))) { // DL	
	rx_pwr2 = (6/2)*signal_energy(rxdata_ext[j][0]+slot_offset_time,640);
      } else { // UL
	if (next_slot%2==0)
	  rx_pwr2 = signal_energy(rxdata_ext[j][0]+slot_offset_time+640*3,640*2);
	else 
	  rx_pwr2 = 2*signal_energy(rxdata_ext[j][0]+slot_offset_time+5*(lte_frame_parms->samples_per_tti>>1)/6,640); // SRSs only on every second --> *2
      }
#ifndef PBS_SIM
      if (j==0)
      printf("rx_pwr Secondary (ADC out) %f dB (%d) for slot %d (subframe %d)\n",10*log10((double)rx_pwr2),rx_pwr2,next_slot,next_slot>>1);
#endif //PBS_SIM
  }
#endif //SECONDARY_SYSTEM
#endif //SKIP_RF_CHAIN

  if (plot_flag) {
  if (next_slot == 19) {
    write_output("UE_rxsig0.m","UE_rxs0", PHY_vars_UE[0]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    if (PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx == 2)
      write_output("UE_rxsig0.m","UE_rxs0", PHY_vars_UE[0]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("eNb_rxsig0.m","eNb_rxs0", PHY_vars_eNb[0]->lte_eNB_common_vars.rxdata[eNb_id][0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    if (PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_rx == 2)
      write_output("eNb_rxsig1.m","eNb_rxs1", PHY_vars_eNb[0]->lte_eNB_common_vars.rxdata[eNb_id][1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
 
    write_output("UE_txsig0.m","UE_txs0", PHY_vars_UE[0]->lte_ue_common_vars.txdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    if (PHY_vars_UE[0]->lte_frame_parms.nb_antennas_tx == 2)
      write_output("UE_txsig1.m","UE_txs1", PHY_vars_UE[0]->lte_ue_common_vars.txdata[1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("eNb_txsig0.m","eNb_txs0", PHY_vars_eNb[0]->lte_eNB_common_vars.txdata[eNb_id][0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    if (PHY_vars_eNb[0]->lte_frame_parms.nb_antennas_tx == 2)
      write_output("eNb_txsig1.m","eNb_txs1", PHY_vars_eNb[0]->lte_eNB_common_vars.txdata[eNb_id][1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    }
  

#ifdef SECONDARY_SYSTEM
  
if (next_slot == 19) {
    write_output("UE_rxsig0_1.m","UE_rxs0_1", PHY_vars_UE[1]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("UE_rxsig1_1.m","UE_rxs1_1", PHY_vars_UE[1]->lte_ue_common_vars.rxdata[1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("UE_rxsig0_2.m","UE_rxs0_2", PHY_vars_UE[2]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    if (PHY_vars_UE[2]->lte_frame_parms.nb_antennas_rx == 2)
      write_output("UE_rxsig1_2.m","UE_rxs1_2", PHY_vars_UE[2]->lte_ue_common_vars.rxdata[1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("eNb_rxsig0_1.m","eNb_rxs0_1", PHY_vars_eNb[1]->lte_eNB_common_vars.rxdata[eNb_id_secsys][0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("eNb_rxsig1_1.m","eNb_rxs1_1", PHY_vars_eNb[1]->lte_eNB_common_vars.rxdata[eNb_id_secsys][1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("eNb_rxsig0_2.m","eNb_rxs0_2", PHY_vars_eNb[2]->lte_eNB_common_vars.rxdata[eNb_id][0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    if (PHY_vars_eNb[2]->lte_frame_parms.nb_antennas_rx == 2)
      write_output("eNb_rxsig1_2.m","eNb_rxs1_2", PHY_vars_eNb[2]->lte_eNB_common_vars.rxdata[eNb_id][1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
 
    write_output("UE_txsig0_1.m","UE_txs0_1", PHY_vars_UE[1]->lte_ue_common_vars.txdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("UE_txsig1_1.m","UE_txs1_1", PHY_vars_UE[1]->lte_ue_common_vars.txdata[1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("eNb_txsig0_1.m","eNb_txs0_1", PHY_vars_eNb[1]->lte_eNB_common_vars.txdata[eNb_id_secsys][0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    write_output("eNb_txsig1_1.m","eNb_txs1_1", PHY_vars_eNb[1]->lte_eNB_common_vars.txdata[eNb_id_secsys][1],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);

    write_output("eNb_txsigF0_1.m","eNb_txsF0_1", PHY_vars_eNb[1]->lte_eNB_common_vars.txdataF[eNb_id_secsys][0],lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti*10,1,1);
    write_output("eNb_txsigF1_1.m","eNb_txsF1_1", PHY_vars_eNb[1]->lte_eNB_common_vars.txdataF[eNb_id_secsys][1],lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti*10,1,1);
    write_output("eNb_txsigF0.m","eNb_txsF0", PHY_vars_eNb[0]->lte_eNB_common_vars.txdataF[eNb_id_secsys][0],lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti*10,1,1);
    if (PHY_vars_UE[2]->lte_frame_parms.nb_antennas_tx == 2)
      write_output("eNb_txsigF1.m","eNb_txsF1", PHY_vars_eNb[0]->lte_eNB_common_vars.txdataF[eNb_id_secsys][1],lte_frame_parms->ofdm_symbol_size*lte_frame_parms->symbols_per_tti*10,1,1);
 }
#endif //SECONDARY_SYSTEM
  } // if(plot_flag)
  
  /*
  // optional: read rx_frame from file
  if ((rx_frame_file = fopen("rx_frame.dat","r")) == NULL)
  {
  printf("[openair][CHBCH_TEST][INFO] Cannot open rx_frame.m data file\n");
  exit(0);
  }
  
  fclose(rx_frame_file);
  */
  } //for(slot...
  
	  //#ifndef PBS_SIM
    if ((mac_xface->frame%200 == 0) && (mac_xface->frame>=200)) {
      printf("Primary rate:   %f (at frame %d), %f (DCI)\n",
	     ((double)(PHY_vars_UE[0]->dlsch_received - PHY_vars_UE[0]->dlsch_errors)/(mac_xface->frame+1)),
	     mac_xface->frame,
	     ((double)(PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_received - PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_errors)/((mac_xface->frame+1))));
      printf("Secondary rate: %f (at frame %d), %f (DCI)\n",
	     ((double)(PHY_vars_UE[1]->dlsch_received - PHY_vars_UE[1]->dlsch_errors)/(mac_xface->frame+1)),
	     mac_xface->frame,
	     ((double)(PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_received - PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_errors)/((mac_xface->frame+1))));
    }
    //#endif //PBS_SIM
    
    if (((PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_received - PHY_vars_UE[0]->lte_ue_pdcch_vars[eNb_id]->dci_errors)<(mac_xface->frame*.0025) && (mac_xface->frame > 400))) {
      mac_xface->frame++; // too bad, no need to try more frames
      printf("Breaking (bad) before dumping, Frame %d, STxG %f, SNR %f\n", mac_xface->frame,STxGain,SNR);
      break;
    }
#ifndef DISABLE_SECONDARY
    if ((PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_received - PHY_vars_UE[1]->lte_ue_pdcch_vars[eNb_id]->dci_errors)<(mac_xface->frame*.0025 && (mac_xface->frame > 400))) {
      mac_xface->frame++; // too bad, no need to try more frames
      printf("Breaking (bad) before dumping, Frame %d, STxG %f, SNR %f\n", mac_xface->frame,STxGain,SNR);
      break;
    }
#endif //DISABLE_SECONDARY
    
    if (((PHY_vars_UE[0]->dlsch_received - PHY_vars_UE[0]->dlsch_errors)>(mac_xface->frame*.999) && (mac_xface->frame > 500))) {
#ifndef DISABLE_SECONDARY
      if ((PHY_vars_UE[1]->dlsch_received - PHY_vars_UE[1]->dlsch_errors)>(mac_xface->frame*.999))
#endif //DISABLE_SECONDARY
	{
	  mac_xface->frame++; // too good, no need to continu
	  printf("Breaking (good) before dumping, Frame %d, STxG %f, SNR %f\n", mac_xface->frame,STxGain,SNR);
	  break;
	}
    }
    
} //for(mac_xface->frame...
  
#ifdef PBS_SIM
  for (j=0;j<2;j++) {
    printf("Dumping data, Frame %d, STxG %f, SNR %f\n", mac_xface->frame,STxGain,SNR);
    fprintf(er_data_fd,"er_data_fd(%i,%i,%i,:) = [%i,%i,%i,%i,%i,%f,%f];\n",stxg_ind,snr_ind,j+1,
	    PHY_vars_UE[j]->dlsch_errors, 
	    PHY_vars_UE[j]->lte_ue_pdcch_vars[eNb_id]->dci_errors,
	    PHY_vars_UE[j]->dlsch_received, 
	    PHY_vars_UE[j]->lte_ue_pdcch_vars[eNb_id]->dci_received,
	    mac_xface->frame,STxGain,SNR);
    fprintf(turboIter_fd,"turboIter_fd(%i,%i,%i) = %i;\n",stxg_ind,snr_ind,j+1,PHY_vars_UE[j]->turbo_iterations);
  }

  // break if error rate is below threshold (if too early --> increase MCS)
#endif //PBS_SIM

  } //for(stxg_ind...
  } //for(snr_ind...
#ifdef CHANNEL_FROM_FILE
#ifdef SECONDARY_SYSTEM
  if (!fopen(channel_output,"r")) {
    channel_file = fopen(channel_output,"w");
    if (channel_file) {
      fprintf(channel_file,"%d\n",1);
      for (aa=0; aa<6; aa++) { //loop over channel index
	for (i=0; i<2; i++) {
	  for (j=0; j<2; j++) {
	    for (l=0; l<channel_length; l++) {
	      fprintf(channel_file,"%lf %lf\n",ch_ar[aa][i + j*2][l].r, ch_ar[aa][i + j*2][l].i);
	    }
	  }
	}
      }
      fclose(channel_file);
    }
  }
#endif //SECONDARY_SYSTEM
#endif //CHANNEL_FROM_FILE

#ifdef PBS_SIM
  fclose(turboIter_fd);
  fclose(er_data_fd);
  //  fclose(er_cause_fd);
#endif //PBS_SIM

#ifdef IFFT_FPGA
  free(txdataF2[0]);
  free(txdataF2[1]);
  free(txdataF2);
  free(txdata[0]);
  free(txdata[1]);
  free(txdata);
#endif 

  for (i=0;i<2;i++) {
    free(s_re[i]);
    free(s_im[i]);
    free(r_re[i]);
    free(r_im[i]);
  }

#ifdef SECONDARY_SYSTEM
  for (i=0;i<2;i++) {
    free(s_re_secsys[i]);
    free(s_im_secsys[i]);
    for (j=0; j<2; j++) {
      free(r_re_ext[j][i]);
      free(r_im_ext[j][i]);
    }
    free(r_re_ext[i]);
    free(r_im_ext[i]);
  }
  for (l=0; l<6; l++) {
      free(r_re_crossLink[l]);
      free(r_im_crossLink[l]);
  }
#endif //SECONDARY_SYSTEM
  
#ifndef SECONDARY_SYSTE
  free(PHY_vars_eNb[0]);
  free(PHY_vars_UE[0]);
#else //SECONDARY_SYSTEM
  free(PHY_vars_eNb[0]);
  free(PHY_vars_eNb[1]);
  free(PHY_vars_UE[0]);
  free(PHY_vars_UE[1]);
#endif
  
  for (i = 0; i<4; i++)
    free(ch[i]);
  free(ch);
#ifdef SECONDARY_SYSTEM
  for (l=0; l<6; l++) {
    for (i = 0; i<4; i++)
      free(ch_ar[l][i]);
    free(ch_ar[l]);
  }
#endif //SECONDARY_SYSTEM

  lte_sync_time_free();

  return(n_errors);
}
