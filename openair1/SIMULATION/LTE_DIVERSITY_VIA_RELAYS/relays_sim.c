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


#ifdef EMOS
#include "SCHED/phy_procedures_emos.h"
#endif
#include "PHY/LTE_TRANSPORT/mcs_tbs_tools.h"


#define BW 7.68
#define Td 1.0




//#define OUTPUT_DEBUG 1

//#define COLLABRATIVE_SCHEME // When Collbarative scheme is used i.e. Distribute Alamouti or Delay Diversity

#define N_TRIALS 10000



#define RBmask0 0x00fc00fc
#define RBmask1 0x0
#define RBmask2 0x0
#define RBmask3 0x0

unsigned char dlsch_cqi;
unsigned char current_dlsch_cqi=7;

DCI0_5MHz_TDD0_t          UL_alloc_pdu;
DCI1A_5MHz_TDD_1_6_t      CCCH_alloc_pdu;
DCI2_5MHz_2A_L10PRB_TDD_t DLSCH_alloc_pdu1;
DCI2_5MHz_2A_M10PRB_TDD_t DLSCH_alloc_pdu2;


#define UL_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,18);
#define CCCH_RB_ALLOC computeRIV(lte_frame_parms->N_RB_UL,0,2)
#define DLSCH_RB_ALLOC 0x1fff // igore DC component,RB13


int main(int argc, char **argv)
{
  int i,aa,s,ind,Kr,Kr_bytes;
  int **txdata_eNB0, **txdata_UE0;
#ifdef COLLABRATIVE_SCHEME
  int **txdata_UE1;
#endif
  int channel_length;
  int eNb_id = 0, eNb_id_i = 1;
  int result,ii;
  int sync_pos, sync_pos_slot;
  int freq_offset,subframe_offset,subframe;
  int j=0,first_call0_0 = 1,first_call1_0 = 1;
#ifdef COLLABRATIVE_SCHEME
  int first_call0_1 = 1,first_call1_1 = 1;
  int channel_offset=0;// Channel_offset for Delay Diversity Scheme
#endif
  int re_allocated;
  
  unsigned int TBS;
  unsigned int dci_cnt_0,dlsch_active_0 = 0,decode_error_0 = 1,dci_errors_0 = 0,n_errors_dl_0 = 0;
#ifdef COLLABRATIVE_SCHEME
  unsigned int dci_cnt_1,dlsch_active_1 = 0,decode_error_1 = 1,dci_errors_1 = 0,n_errors_dl_1 = 0;
#endif
  unsigned int ret_0,ret_ul;
#ifdef COLLABRATIVE_SCHEME
  unsigned int ret_1;
#endif
  unsigned int coded_bits_per_codeword_dl,nsymb;
  unsigned int trials;
  unsigned int tx_lev_dl,tx_lev_ul,tx_lev_dl_dB,tx_lev_ul_dB,num_layers;
  unsigned int n_errors_dl = 0,trials_dl = 0;
  unsigned int n_errors_ul = 0,trials_ul = 0;



  
  


  double sigma2_dl,sigma2_ul, sigma2_dB_dl=0,sigma2_dB_ul=0,SNR_dl=2.0,SNR_ul,snr0_ul =-10.0,snr1_ul,SNRmeas_0,SNRmeas_1;
  double **s_re,**s_im,**r_re0_0,**r_im0_0;
  double **s_re_0,**s_im_0,**r_re1_0,**r_im1_0;
#ifdef COLLABRATIVE_SCHEME
  double **r_re0_1,**r_im0_1;
  double **s_re_1,**s_im_1,**r_re1_1,**r_im1_1;
#endif
  double amps[8] = {0.3868472 , 0.3094778 , 0.1547389 , 0.0773694 , 0.0386847 , 0.0193424 , 0.0096712 , 0.0038685};
  double aoa=.03,ricean_factor=1/(1+pow(10,0.1*(20)));
  double nf[2] = {3.0,3.0}; //currently unused
  double ip =0.0,rate_dl;
  double N0W, path_loss, path_loss_dB;
  double harq_adjust;
  double relay_delay;
  
  char fname[40], vname[40],bler_fname[20],bler_fname_dl[20],bler_fname_ul[20];

  
  

  unsigned char dual_stream_UE = 0,mcs_dl=1,mcs_ul=1;
  unsigned char Ns,l,m;
  unsigned char pbch_pdu[6];
  unsigned char rag_flag;
  unsigned char *input_buffer_eNB0,*input_buffer_UE0,*input_buffer_UE0_temp,harq_pid_dl,harq_pid_ul,transmission_mode = 2;
  unsigned char *input_data_eNB0,*decoded_output;
  unsigned char *input_buffer_UE1,*input_buffer_UE1_temp;
  //unsigned char round_dl,round_ul,ulx;

  

  //relay_flag = 0 for no relays,1 for 1 relay,2 for 2 relays
  unsigned char relay_flag =1;


  //diversity_scheme = 0 for no scheme,1 for delay diversity,2 for distributed alamouti
  unsigned char diversity_scheme=0;

  
  MIMO_mode_t mimo_mode;
  
  
  FILE *rx_frame_file,*bler_fd,*bler_fd_dl,*bler_fd_ul;

  
  DCI_ALLOC_t dci_alloc[8],dci_alloc_rx_0[8];
#ifdef COLLABRATIVE_SCHEME
  DCI_ALLOC_t dci_alloc_rx_1[8];
#endif
  
  unsigned short input_buffer_length_eNB0,input_buffer_length_UE0;
#ifdef COLLABRATIVE_SCHEME
  unsigned short input_buffer_length_UE1;
#endif
  unsigned short NB_RB_dl= conv_nprb(0,DLSCH_RB_ALLOC);
 
  

#ifdef COLLABRATIVE_SCHEME
  struct complex **ch[4];
#else
  struct complex **ch[2];
#endif
  
  
  
#ifdef IFFT_FPGA
  int **txdataF2_eNB0, **txdataF2_UE0;
#ifdef COLLABRATIVE_SCHEME
  int **txdataF2_UE1;
#endif
#endif
  
  

#ifdef COLLABRATIVE_SCHEME
  // Channel ID for distinguishing between the 4 Independent Channels
  enum CH_ID
  {
    SeFu,  // From First eNB to First UE
    SeSu,  // From First eNB to Second UE
    FuDe,  // From First UE to Second eNB
    SuDe  // From Second UE to Second eNB
  };
#else
  // Channel ID for distinguishing between the 2 Independent Channels
  enum CH_ID
  {
    SeFu,  // From First eNB to First UE
    FuDe,  // From First UE to Second eNB
  };
#endif
  

  SRS_param_t SRS_parameters;
 

  dci_alloc[0].dci_length = sizeof_DCI0_5MHz_TDD_0_t;
  
  channel_length = (int) 11+2*BW*Td;
  


#ifdef EMOS
  fifo_dump_emos emos_dump;
#endif
  



  PHY_config = malloc(sizeof(PHY_CONFIG));
  mac_xface = malloc(sizeof(MAC_xface));



  PHY_VARS_UE *PHY_vars_ue[2];
  PHY_vars_ue[0] = malloc(sizeof(PHY_VARS_UE));
  PHY_vars_ue[1] = malloc(sizeof(PHY_VARS_UE));



  PHY_VARS_eNB *PHY_vars_eNB[2];
 

  PHY_vars_eNB[0] = malloc(sizeof(PHY_VARS_eNB));
  PHY_vars_eNB[1] = malloc(sizeof(PHY_VARS_eNB));
  

  
  randominit(0);
  set_taus_seed(0);
  
  crcTableInit();
  


  lte_frame_parms = &(PHY_vars_eNB[0]->lte_frame_parms);
  
  
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
  lte_frame_parms->tdd_config = 3;
  lte_frame_parms->mode1_flag = (transmission_mode == 1)? 1 : 0;
  
  SRS_parameters.Csrs = 2;
  SRS_parameters.Bsrs = 0;
  SRS_parameters.kTC = 0;
  SRS_parameters.n_RRC = 0;
  SRS_parameters.Ssrs = 1;

  PHY_vars_ue[0]->SRS_parameters = SRS_parameters;
  PHY_vars_ue[1]->SRS_parameters = SRS_parameters;
  PHY_vars_eNB[0]->eNB_UE_stats[0].SRS_parameters = SRS_parameters;
  PHY_vars_eNB[0]->eNB_UE_stats[1].SRS_parameters = SRS_parameters;
  PHY_vars_eNB[1]->eNB_UE_stats[0].SRS_parameters = SRS_parameters;
  PHY_vars_eNB[1]->eNB_UE_stats[1].SRS_parameters = SRS_parameters;
  
  
  PHY_vars_ue[1]->SRS_parameters.Ssrs = 2;
  PHY_vars_eNB[0]->eNB_UE_stats[1].SRS_parameters.Ssrs = 2;
  PHY_vars_eNB[1]->eNB_UE_stats[1].SRS_parameters.Ssrs = 2;
  


  init_frame_parms(lte_frame_parms);
  
  copy_lte_parms_to_phy_framing(lte_frame_parms, &(PHY_config->PHY_framing));
  
  phy_init_top(NB_ANTENNAS_TX);
 
  
  lte_frame_parms->twiddle_fft      = twiddle_fft;
  lte_frame_parms->twiddle_ifft     = twiddle_ifft;
  lte_frame_parms->rev              = rev;
  

  //phy_init_lte_top(lte_frame_parms);
  lte_gold(lte_frame_parms);
  generate_ul_ref_sigs();
  generate_ul_ref_sigs_rx();
  generate_64qam_table();
  generate_16qam_table();
  generate_RIV_tables();
  

  //use same frame parameters for UE as for eNb
  PHY_vars_ue[0]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_ue[1]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_eNB[0]->lte_frame_parms = *lte_frame_parms;
  PHY_vars_eNB[1]->lte_frame_parms = *lte_frame_parms;
  



  
  
  generate_pcfich_reg_mapping(lte_frame_parms);
  generate_phich_reg_mapping_ext(lte_frame_parms);
  
  
  
  /*************************************************Initialization for both the UEs & eNBs*************************************************/
  /*************************************In the initialization of eNB, 2 additional flags are added*****************************************/
  /***********relay_flag for indicating the relaying & diversity scheme for indicating Delay Diversity scheme or Distributed Alamouti******/

  for (i=0; i<2; i++)
    {
      phy_init_lte_ue(lte_frame_parms,
		      &PHY_vars_ue[i]->lte_ue_common_vars,
		      PHY_vars_ue[i]->lte_ue_dlsch_vars,
		      PHY_vars_ue[i]->lte_ue_dlsch_vars_SI,
		      PHY_vars_ue[i]->lte_ue_dlsch_vars_ra,
		      //PHY_vars_ue[i]->lte_ue_dlsch_vars_1A,
		      PHY_vars_ue[i]->lte_ue_pbch_vars,
		      PHY_vars_ue[i]->lte_ue_pdcch_vars,
		      PHY_vars_ue[i]);
    }


  for(i=0;i<2;i++)
    {
      phy_init_lte_eNB(lte_frame_parms,
		       &PHY_vars_eNB[i]->lte_eNB_common_vars,
		       PHY_vars_eNB[i]->lte_eNB_ulsch_vars,
		       0,
		       PHY_vars_eNB[i],
		       relay_flag,
		       diversity_scheme);
    }
      


  num_layers = 1;
  
  
  //printf("argc %d\n",argc);
  

  
  
  printf("NPRB_dl = %d\n",NB_RB_dl);
  
  



#ifdef IFFT_FPGA
  //for eNB0
  
  txdata_eNB0    = (int **)malloc16(2*sizeof(int*));
  txdata_eNB0[0] = (int *)malloc16(FRAME_LENGTH_BYTES);
  txdata_eNB0[1] = (int *)malloc16(FRAME_LENGTH_BYTES);

  bzero(txdata_eNB0[0],FRAME_LENGTH_BYTES);
  bzero(txdata_eNB0[1],FRAME_LENGTH_BYTES);

  txdataF2_eNB0    = (int **)malloc16(2*sizeof(int*));
  txdataF2_eNB0[0] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
  txdataF2_eNB0[1] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
  
  bzero(txdataF2_eNB0[0],FRAME_LENGTH_BYTES_NO_PREFIX);
  bzero(txdataF2_eNB0[1],FRAME_LENGTH_BYTES_NO_PREFIX)
    
    // for UE0
    
    txdata_UE0    = (int **)malloc16(2*sizeof(int*));
  txdata_UE0[0] = (int *)malloc16(FRAME_LENGTH_BYTES);
  txdata_UE0[1] = (int *)malloc16(FRAME_LENGTH_BYTES);

  bzero(txdata_UE0[0],FRAME_LENGTH_BYTES);
  bzero(txdata_UE0[1],FRAME_LENGTH_BYTES);

  txdataF2_UE0    = (int **)malloc16(2*sizeof(int*));
  txdataF2_UE0[0] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
  txdataF2_UE0[1] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);

  bzero(txdataF2_UE0[0],FRAME_LENGTH_BYTES_NO_PREFIX);
  bzero(txdataF2_UE0[1],FRAME_LENGTH_BYTES_NO_PREFIX);

  // for UE1
#ifdef COLLABRATIVE_SCHEME

  txdata_UE1    = (int **)malloc16(2*sizeof(int*));
  txdata_UE1[0] = (int *)malloc16(FRAME_LENGTH_BYTES);
  txdata_UE1[1] = (int *)malloc16(FRAME_LENGTH_BYTES);

  bzero(txdata_UE1[0],FRAME_LENGTH_BYTES);
  bzero(txdata_UE1[1],FRAME_LENGTH_BYTES);

  txdataF2_UE1    = (int **)malloc16(2*sizeof(int*));
  txdataF2_UE1[0] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
  txdataF2_UE1[1] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);

  bzero(txdataF2_UE1[0],FRAME_LENGTH_BYTES_NO_PREFIX);
  bzero(txdataF2_UE1[1],FRAME_LENGTH_BYTES_NO_PREFIX);
#endif
#else
  txdata_eNB0 = PHY_vars_eNB[0]->lte_eNB_common_vars.txdata[0];
  txdata_UE0 = &PHY_vars_ue[0]->lte_ue_common_vars.txdata[0];
#ifdef COLLABRATIVE_SCHEME
  txdata_UE1 = &PHY_vars_ue[1]->lte_ue_common_vars.txdata[0];
#endif
#endif
  // eNB0 to UE0 
  s_re = malloc(2*sizeof(double*));
  s_im = malloc(2*sizeof(double*));
  r_re0_0 = malloc(2*sizeof(double*));
  r_im0_0 = malloc(2*sizeof(double*));


  //UE 0 to eNB1
  s_re_0 = malloc(2*sizeof(double*));
  s_im_0 = malloc(2*sizeof(double*));
  r_re1_0 = malloc(2*sizeof(double*));
  r_im1_0 = malloc(2*sizeof(double*));


#ifdef COLLABRATIVE_SCHEME
  // eNB0 to UE1
  r_re0_1 = malloc(2*sizeof(double*));
  r_im0_1 = malloc(2*sizeof(double*));
  
  //UE 1 to eNB 1
  s_re_1 = malloc(2*sizeof(double*));
  s_im_1 = malloc(2*sizeof(double*));
  r_re1_1 = malloc(2*sizeof(double*));
  r_im1_1 = malloc(2*sizeof(double*));
#endif
   
  
  //snr0_ul = -1.0;// 0.25;
  //snr1_ul = snr0_ul;
  snr1_ul = snr0_ul+20;

  
  nsymb = (lte_frame_parms->Ncp == 0) ? 14 : 12;
  
  coded_bits_per_codeword_dl = NB_RB_dl * (12 * get_Qm(mcs_dl)) * (lte_frame_parms->num_dlsch_symbols);


#ifdef TBS_FIX
  rate_dl = (double)3*dlsch_tbs25[get_I_TBS(mcs_dl)][NB_RB_dl-1]/(4*coded_bits_per_codeword_dl);
#else
  rate_dl = (double)dlsch_tbs25[get_I_TBS(mcs_dl)][NB_RB_dl-1]/(coded_bits_per_codeword_dl);
#endif



  //printf("Rate = %f (mod %d)\n",(double)dlsch_tbs25[get_I_TBS(mcs)][NB_RB-1]/coded_bits_per_codeword,get_Qm(mcs));
  //rate_dl *= get_Qm(mcs_dl);


  sprintf(bler_fname_dl,"bler_dl_%d.m",mcs_dl);
  bler_fd_dl = fopen(bler_fname_dl,"w");
  fprintf(bler_fd_dl,"bler_dl=[");

  sprintf(bler_fname_ul,"bler_ul_%d.m",mcs_ul);
  bler_fd_ul = fopen(bler_fname_ul,"w");
  fprintf(bler_fd_ul,"bler_ul=[");


  sprintf(bler_fname,"bler_%d_%d.m",mcs_dl,mcs_ul);
  bler_fd = fopen(bler_fname,"w");
  fprintf(bler_fd,"bler=[");

  

  
  
  for (i=0;i<2;i++)// loop over 2 antennas
    { 
      // eNB0 to UE0 
      s_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_re[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      s_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_im[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_re0_0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re0_0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im0_0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im0_0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));

#ifdef COLLABRATIVE_SCHEME
      r_re0_1[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re0_1[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im0_1[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im0_1[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
#endif
      
      
      //UE 0
      s_re_0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_re_0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      s_im_0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_im_0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_re1_0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re1_0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im1_0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im1_0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      
#ifdef COLLABRATIVE_SCHEME
      //UE 1
      s_re_1[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_re_1[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      s_im_1[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_im_1[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_re1_1[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re1_1[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im1_1[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im1_1[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
#endif
    }
  
  
  
  
  PHY_vars_ue[0]->lte_ue_pdcch_vars[0]->crnti = C_RNTI;// UE 0

#ifdef COLLABRATIVE_SCHEME
  PHY_vars_ue[1]->lte_ue_pdcch_vars[0]->crnti = C_RNTI;// UE 1
#endif
  
  
  
  // init DCI structures for testing
  UL_alloc_pdu.type    = 0;
  UL_alloc_pdu.hopping = 0;
  UL_alloc_pdu.rballoc = UL_RB_ALLOC;
  UL_alloc_pdu.mcs     = mcs_ul;
  UL_alloc_pdu.ndi     = 1;
  UL_alloc_pdu.TPC     = 0;
  UL_alloc_pdu.cqi_req = 1;

  CCCH_alloc_pdu.type               = 0;
  CCCH_alloc_pdu.vrb_type           = 0;
  CCCH_alloc_pdu.rballoc            = CCCH_RB_ALLOC;
  CCCH_alloc_pdu.ndi      = 1;
  CCCH_alloc_pdu.rv       = 1;
  CCCH_alloc_pdu.mcs      = 1;
  CCCH_alloc_pdu.harq_pid = 0;
  
  DLSCH_alloc_pdu2.rah              = 0;
  DLSCH_alloc_pdu2.rballoc          = DLSCH_RB_ALLOC;
  DLSCH_alloc_pdu2.TPC              = 0;
  DLSCH_alloc_pdu2.dai              = 0;
  DLSCH_alloc_pdu2.harq_pid         = 0;
  DLSCH_alloc_pdu2.tb_swap          = 0;
  DLSCH_alloc_pdu2.mcs1             = mcs_dl;
  DLSCH_alloc_pdu2.ndi1             = 1;
  DLSCH_alloc_pdu2.rv1              = 0;
  // Forget second codeword
  DLSCH_alloc_pdu2.tpmi             = 1;
  
  




  
  // Create transport channel structures for SI pdus
  
  //loop over eNBs
  for (aa=0;aa<2;aa++) {
    PHY_vars_eNB[aa]->dlsch_eNb[0] = (LTE_eNb_DLSCH_t**) malloc16(2*sizeof(LTE_eNb_DLSCH_t*));
    PHY_vars_eNB[aa]->dlsch_eNb[1] = (LTE_eNb_DLSCH_t**) malloc16(2*sizeof(LTE_eNb_DLSCH_t*));
    PHY_vars_eNB[aa]->ulsch_eNb = (LTE_eNb_ULSCH_t**) malloc16(2*sizeof(LTE_eNb_ULSCH_t*));
    PHY_vars_eNB[aa]->dlsch_eNb_SI = (LTE_eNb_DLSCH_t*) malloc16(sizeof(LTE_eNb_DLSCH_t*));
    PHY_vars_eNB[aa]->dlsch_eNb_ra = (LTE_eNb_DLSCH_t*) malloc16(sizeof(LTE_eNb_DLSCH_t*));
    //loop over number of UEs
    for(j=0;j<2;j++){
      //loop over transport channels
      for (i=0;i<2;i++) {
	PHY_vars_eNB[aa]->dlsch_eNb[j][i] = new_eNb_dlsch(1,8);
	if (!PHY_vars_eNB[aa]->dlsch_eNb[j][i]) {
	  msg("Can't get eNb ulsch structures\n");
	  exit(-1);
	} else {
	  msg("PHY_vars_eNb[%d]->dlsch_eNb[%d][%d] = %p\n",aa,j,i,PHY_vars_eNB[aa]->dlsch_eNb[j][i]);
	}
      }


      PHY_vars_eNB[aa]->ulsch_eNb[j] = new_eNb_ulsch(3);
      if (!PHY_vars_eNB[aa]->ulsch_eNb[j]) {
	msg("Can't get eNb ulsch structures \n");
	exit(-1);
      } else {
	msg("PHY_vars_eNb[%d]->ulsch_eNb[%d] = %p\n",aa,j,PHY_vars_eNB[aa]->ulsch_eNb[j]);
      }

    }
    PHY_vars_eNB[aa]->dlsch_eNb_SI = new_eNb_dlsch(1,1);
    PHY_vars_eNB[aa]->dlsch_eNb_ra = new_eNb_dlsch(1,1);
  }


  //loop over UEs
  for (l=0;l<2;l++) {
    PHY_vars_ue[l]->dlsch_ue[0] = (LTE_UE_DLSCH_t**) malloc16(2*sizeof(LTE_UE_DLSCH_t*));
    PHY_vars_ue[l]->dlsch_ue[1] = (LTE_UE_DLSCH_t**) malloc16(2*sizeof(LTE_UE_DLSCH_t*));
    PHY_vars_ue[l]->ulsch_ue = (LTE_UE_ULSCH_t**) malloc16(2*sizeof(LTE_UE_ULSCH_t*));
    PHY_vars_ue[l]->dlsch_ue_SI  = (LTE_UE_DLSCH_t**) malloc16(2*sizeof(LTE_UE_DLSCH_t*));
    PHY_vars_ue[l]->dlsch_ue_ra  = (LTE_UE_DLSCH_t**) malloc16(2*sizeof(LTE_UE_DLSCH_t*));
    //loop over number of eNBs
    for(j=0;j<2;j++){
      //loop over transport channels 
      for (i=0;i<2;i++) {
	PHY_vars_ue[l]->dlsch_ue[j][i] = new_ue_dlsch(1,8);
	if (!PHY_vars_ue[l]->dlsch_ue[j][i]) {
	  msg("Can't get ue ulsch structures\n");
	  exit(-1);
	} else {
	  msg("PHY_vars_UE [%d]->dlsch_ue[%d][%d] = %p\n",l,j,i,PHY_vars_ue[l]->dlsch_ue[j][i]);
	}
      }
      PHY_vars_ue[l]->dlsch_ue_SI[j]  = new_ue_dlsch(1,1);
      PHY_vars_ue[l]->dlsch_ue_ra[j]  = new_ue_dlsch(1,1);

      PHY_vars_ue[l]->ulsch_ue[j] = new_ue_ulsch(3);
      if (!PHY_vars_ue[l]->ulsch_ue[j]) {
	msg("Can't get ue ulsch structures\n");
	exit(-1);
      } else {
	msg("PHY_vars_UE [%d]->ulsch_ue [%d] = %p\n",l,j,PHY_vars_ue[l]->ulsch_ue[j]);
      }
    }

  }
  
  
 



  
  generate_eNb_dlsch_params_from_dci(0,
				     (DCI2_5MHz_2A_M10PRB_TDD_t *)&DLSCH_alloc_pdu2,                                     
				     C_RNTI,
				     format2_2A_M10PRB,
				     PHY_vars_eNB[0]->dlsch_eNb[0],
				     lte_frame_parms,
				     SI_RNTI,
				     RA_RNTI,
				     P_RNTI,
				     0);
  


  generate_eNb_dlsch_params_from_dci(0,
				     (DCI2_5MHz_2A_M10PRB_TDD_t *)&DLSCH_alloc_pdu2,                                     
				     C_RNTI,
				     format2_2A_M10PRB,
				     PHY_vars_eNB[0]->dlsch_eNb[1],
				     lte_frame_parms,
				     SI_RNTI,
				     RA_RNTI,
				     P_RNTI,
				     0);

  





  // DCI
  
  memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu2,sizeof(DCI2_5MHz_2A_M10PRB_TDD_t));
  dci_alloc[0].dci_length = sizeof_DCI2_5MHz_2A_M10PRB_TDD_t;
  dci_alloc[0].L          = 3;
  dci_alloc[0].rnti       = C_RNTI;
  
  memcpy(&dci_alloc[1].dci_pdu[0],&UL_alloc_pdu,sizeof(DCI0_5MHz_TDD0_t));
  dci_alloc[1].dci_length = sizeof_DCI0_5MHz_TDD_0_t;
  dci_alloc[1].L          = 3;
  dci_alloc[1].rnti       = C_RNTI;
  
  
  


  // DLSCH
    
  input_buffer_length_eNB0 = PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->TBS/8;
  
  printf("dlsch0: TBS      %d\n",PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->TBS);
  
  TBS = PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->TBS;

  printf("Input buffer size dl %d bytes\n",input_buffer_length_eNB0);
  
  
  input_buffer_eNB0 = (unsigned char *)malloc(input_buffer_length_eNB0+4);
  input_buffer_UE0_temp = (unsigned char *)malloc(input_buffer_length_eNB0+4);
  input_buffer_UE1_temp = (unsigned char *)malloc(input_buffer_length_eNB0+4);
  
  
  
  for (i=0;i<input_buffer_length_eNB0;i++) {
    input_buffer_eNB0[i]= (unsigned char)(taus()&0xff);
    //printf("input %d : %x\n",i,input_buffer[i]);
  }


#ifdef COLLABRATIVE_SCHEME 
  ch[SeFu] = (struct complex**) malloc(4 * sizeof(struct complex*));
  ch[SeSu] = (struct complex**) malloc(4 * sizeof(struct complex*));
  ch[FuDe] = (struct complex**) malloc(4 * sizeof(struct complex*));
  ch[SuDe] = (struct complex**) malloc(4 * sizeof(struct complex*));
  for (i = 0; i<4; i++)
    {
      ch[SeFu][i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
      ch[SeSu][i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
      ch[FuDe][i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
      ch[SuDe][i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
    }
#else
  ch[SeFu] = (struct complex**) malloc(4 * sizeof(struct complex*));
  ch[FuDe] = (struct complex**) malloc(4 * sizeof(struct complex*));
  for (i = 0; i<4; i++)
    {
      ch[SeFu][i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
      ch[FuDe][i] = (struct complex*) malloc(channel_length * sizeof(struct complex));
    }
#endif
  

#ifdef COLLABRATIVE_SCHEME
  if((relay_flag == 2)&&(diversity_scheme == 1))
    channel_offset = lte_frame_parms->nb_prefix_samples>>=1;
  //printf("channel offset %d",channel_offset);
#endif
      
      
	dlsch_encoding(input_buffer_eNB0,
		       lte_frame_parms,
		       PHY_vars_eNB[0]->dlsch_eNb[0][0]);
      
      
      
      
#ifdef OUTPUT_DEBUG
	for (s=0;s<PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->C;s++) {
	  if (s<PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->Cminus)
	    Kr = PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->Kminus;
	  else
	    Kr = PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->Kplus;
	
	  Kr_bytes = Kr>>3;
	
	  for (i=0;i<Kr_bytes;i++)
	    printf("%d : (%x)\n",i,PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->c[s][i]);
	}
      
#endif 
      
      
      
	re_allocated = dlsch_modulation(PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id],
					1024,
					0,
					lte_frame_parms,
					PHY_vars_eNB[0]->dlsch_eNb[0][0]);
      
      
	//   printf("RB count dl %d (%d,%d)\n",re_allocated,re_allocated/lte_frame_parms->num_dlsch_symbols/12,lte_frame_parms->num_dlsch_symbols);
      


	if ((re_allocated/(lte_frame_parms->num_dlsch_symbols*12)) != NB_RB_dl)
	  //	printf("Bad RB count dl %d (%d,%d)\n",re_allocated,re_allocated/lte_frame_parms->num_dlsch_symbols/12,lte_frame_parms->num_dlsch_symbols);
      



	  if (num_layers>1)
	    re_allocated = dlsch_modulation(PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id],
					    1024,
					    i,
					    lte_frame_parms,
					    PHY_vars_eNB[0]->dlsch_eNb[0][1]);
      

  
	generate_pss(PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id],
		     1024,
		     lte_frame_parms,
		     eNb_id,
		     6-lte_frame_parms->Ncp,
		     0);
  
  
  
	for (i=0;i<6;i++)
	  pbch_pdu[i] = i;
  
  
  
	generate_pbch(PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id],
		      1024,
		      lte_frame_parms,
		      pbch_pdu);
  
  
  
  
	generate_dci_top(1,
			 0,
			 dci_alloc,
			 0,
			 1024,
			 lte_frame_parms,
			 PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id],
			 0);
  
  
	generate_pilots(PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id],
			1024,
			lte_frame_parms,
			eNb_id,
			LTE_NUMBER_OF_SUBFRAMES_PER_FRAME);
  
 


  


#ifdef IFFT_FPGA
#ifdef OUTPUT_DEBUG  
	write_output("txsigF0_eNB0.m","txsF0_eNB0",PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[0][0],300*120,1,4);
	write_output("txsigF1_eNB0.m","txsF1_eNB0",PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[0][1],300*120,1,4);
#endif
  
  




	// do talbe lookup and write results to txdataF2
	for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {
	  ind = 0;
	  for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++) 
	    if (((i%512)>=1) && ((i%512)<=150))
	      txdataF2_eNB0[aa][i] = ((int*)mod_table)[PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id][aa][ind++]];
	    else if ((i%512)>=362)
	      txdataF2_eNB0[aa][i] = ((int*)mod_table)[PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id][aa][ind++]];
	    else 
	      txdataF2_eNB0[aa][i] = 0;
	  printf("ind=%d\n",ind);
	}
  
  



#ifdef OUTPUT_DEBUG  
	write_output("txsigF20_eNB0.m","txsF20_eNB0", txdataF2_eNB0[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
  



	tx_lev_dl = 0;
	for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	  PHY_ofdm_mod(txdataF2_eNB0[aa],        // input
		       txdata_eNB0[aa],         // output
		       lte_frame_parms->log2_symbol_size,                // log2_fft_size
		       NUMBER_OF_SYMBOLS_PER_FRAME,                 // number of symbols
		       lte_frame_parms->nb_prefix_samples,               // number of prefix samples
		       lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
		       lte_frame_parms->rev,           // bit-reversal permutation
		       CYCLIC_PREFIX);
    
	  tx_lev_dl += signal_energy(&txdata_eNB0[aa][4*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],
				     OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	}
  
#else //IFFT_FPGA
  
#ifdef OUTPUT_DEBUG  
	write_output("txsigF0_eNB0.m","txsF0_eNB0",PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id][0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX/5,1,1);
	write_output("txsigF1_eNB0.m","txsF1_eNB0",PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id][1],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX/5,1,1);
#endif
  
  
  
  
	tx_lev_dl = 0;

	for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	  PHY_ofdm_mod(PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[eNb_id][aa],        // input
		       txdata_eNB0[aa],         // output
		       lte_frame_parms->log2_symbol_size,                // log2_fft_size
		       NUMBER_OF_SYMBOLS_PER_FRAME,                 // number of symbols
		       lte_frame_parms->nb_prefix_samples,               // number of prefix samples
		       lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
		       lte_frame_parms->rev,           // bit-reversal permutation
		       CYCLIC_PREFIX);
    
	  tx_lev_dl += signal_energy(&txdata_eNB0[aa][OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*2],
				     OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
  
	}  
  
  
#endif //IFFT_FPGA
  
  

	tx_lev_dl_dB = (unsigned int) dB_fixed(tx_lev_dl);
  
  
  
  
#ifdef OUTPUT_DEBUG  
	write_output("txsig0_eNB0.m","txs0_eNB0", txdata_eNB0[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
#endif


  

	// multipath channel


  
	for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
	  for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {
	    s_re[aa][i] = ((double)(((short *)txdata_eNB0[aa]))[(i<<1)]);
	    s_im[aa][i] = ((double)(((short *)txdata_eNB0[aa]))[(i<<1)+1]);
	  }
	}
  
	//printf("tx_lev_dl_dB = %d\n",tx_lev_dl_dB);
  
  
 
	sigma2_dB_dl = tx_lev_dl_dB +10*log10(25/NB_RB_dl) - SNR_dl;
	// printf("**********************SNR_dl = %f dB (tx_lev_dl %f, sigma2_dB_dl %f)**************************\n",
	// SNR_dl,
	//   (double)tx_lev_dl_dB+10*log10(25/NB_RB),
	//   sigma2_dB_dl);


    

	// Loop over Uplink SNR
	for(SNR_ul = snr0_ul;SNR_ul<=snr1_ul;SNR_ul+=0.25){
      
      
	  n_errors_dl = 0;
	  n_errors_ul = 0;

	  dci_errors_0 = 0;
#ifdef COLLABRATIVE_SCHEME
	  dci_errors_1 = 0;
#endif
	  trials_dl = 0;
	  trials_ul = 0;

	  for(trials = 0;trials<N_TRIALS;trials++){

	    trials_dl++;

	    decode_error_0 = 1;

#ifdef COLLABRATIVE_SCHEME
	    decode_error_1 = 1;
#endif

	//eNB0 to UE0
	multipath_channel(ch[SeFu],s_re,s_im,r_re0_0,r_im0_0,
			  amps,Td,BW,ricean_factor,aoa,
			  lte_frame_parms->nb_antennas_tx,
			  lte_frame_parms->nb_antennas_rx,
			  FRAME_LENGTH_COMPLEX_SAMPLES,
			  channel_length,
			  0,1//forgetting factor
			  ,((first_call0_0 == 1)?1:0),0,SeFu,0);
	if(first_call0_0 == 1)
	  first_call0_0 = 0;

     
      
      
      
#ifdef OUTPUT_DEBUG
	write_output("channel0_dl0.m","chan0_dl0",ch[SeFu][0],channel_length,1,8);//UE 0
#endif
      
    
	//AWGN
	sigma2_dl = pow(10,sigma2_dB_dl/10);
      
	for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
	  for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
	    //UE 0
	    ((short*)PHY_vars_ue[0]->lte_ue_common_vars.rxdata[aa])[2*i] = (short) (r_re0_0[aa][i] + sqrt(sigma2_dl/2)*gaussdouble(0.0,1.0));
	    ((short*)PHY_vars_ue[0]->lte_ue_common_vars.rxdata[aa])[2*i+1] = (short) (r_im0_0[aa][i] + sqrt(sigma2_dl/2)*gaussdouble(0.0,1.0));
	 
	  }
	}    
      
      
	//UE 0
	//    lte_sync_time_init(lte_frame_parms,PHY_vars_ue[0]->lte_ue_common_vars);
	//    lte_sync_time(PHY_vars_ue[0]->lte_ue_common_vars.rxdata, lte_frame_parms);
	//    lte_sync_time_free();
      

      
      
      
      
#ifdef OUTPUT_DEBUG
      
	// UE 0
	printf("RX level in null symbol UE0 %d\n",dB_fixed(signal_energy(&PHY_vars_ue[0]->lte_ue_common_vars.rxdata[0][160+OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
	printf("RX level in data symbol UE0 %d\n",dB_fixed(signal_energy(&PHY_vars_ue[0]->lte_ue_common_vars.rxdata[0][160+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
	printf("rx_level Null symbol UE0 %f\n",10*log10(signal_energy_fp(r_re0_0,r_im0_0,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
	printf("rx_level data symbol UE0 %f\n",10*log10(signal_energy_fp(r_re0_0,r_im0_0,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));

      
#endif
      
      
	SNRmeas_0 = 10*log10((signal_energy_fp(r_re0_0,r_im0_0,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))/signal_energy_fp(r_re0_0,r_im0_0,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(1*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))) - 1);// UE 0
      
   
      
    
      

	// Inner receiver scheduling for 3 slots for UE 0
      
	for (Ns=0;Ns<3;Ns++) {
	  for (l=0;l<6;l++) {
	  
	    slot_fep(lte_frame_parms,
		     &PHY_vars_ue[0]->lte_ue_common_vars,
		     l,
		     Ns%20,
		     0,
		     0);// UE 0

	  
	  

	    lte_ue_measurements(PHY_vars_ue[0],
				lte_frame_parms,
				0,
				1,
				0);// UE 0

	 	  
	  
	  
	    if ((Ns==0) && (l==3)) {// process symbols 0,1,2
	    
	      rx_pdcch(&PHY_vars_ue[0]->lte_ue_common_vars,
		       PHY_vars_ue[0]->lte_ue_pdcch_vars,
		       lte_frame_parms,
		       eNb_id,
		       2,
		       (lte_frame_parms->mode1_flag == 1) ? SISO : ALAMOUTI,
		       0);// UE 0
	    
	    
	    
	      //UE 0
	      dci_cnt_0 = dci_decoding_procedure(PHY_vars_ue[0]->lte_ue_pdcch_vars,dci_alloc_rx_0,eNb_id,lte_frame_parms,SI_RNTI,RA_RNTI);
	      if(dci_cnt_0 == 0)
		{
		  dlsch_active_0 = 0;
		  dci_errors_0++;
		}
	    
	      for (i=0;i<dci_cnt_0;i++)
		if ((dci_alloc_rx_0[i].rnti == C_RNTI) && (dci_alloc_rx_0[i].format == format2_2A_M10PRB))
		  {
		    generate_ue_dlsch_params_from_dci(0,
						      (DCI2_5MHz_2A_M10PRB_TDD_t *)&dci_alloc_rx_0[i].dci_pdu,
						      C_RNTI,
						      format2_2A_M10PRB,
						      PHY_vars_ue[0]->dlsch_ue[0],
						      lte_frame_parms,
						      SI_RNTI,
						      RA_RNTI,
						      P_RNTI);
		  
		    dlsch_active_0 = 1;
		  }
		else {
		  dlsch_active_0 = 0;
		  dci_errors_0++;
		}
	    
	      /*
		else if ((dci_alloc_rx_0[i].rnti == SI_RNTI) && (dci_alloc_rx_0[i].format == format1A))
		generate_ue_dlsch_params_from_dci(0,
		(DCI1A_5MHz_TDD_1_6_t *)&dci_alloc_rx_0[i].dci_pdu,
		SI_RNTI,
		format1A,
		&PHY_vars_ue[0]->dlsch_ue_cntl, 
		lte_frame_parms,
		SI_RNTI,
		RA_RNTI,
		P_RNTI);
	      */
	      //  msg("dci_cnt UE0 = %d\n",dci_cnt_0);

	   
	    }// process symbols 0,1,2
	  
	
	  
	    /*
	      for (m=lte_frame_parms->first_dlsch_symbol;m<3;m++)
	      rx_dlsch(PHY_vars_ue[0]->lte_ue_common_vars,
	      PHY_vars_ue[0]->lte_ue_dlsch_vars,
	      lte_frame_parms,
	      eNb_id,
	      eNb_id_i,
	      m,
	      rb_alloc,
	      mod_order,
	      mimo_mode,
	      dual_stream_UE);// UE 0
	    */
	  
	 	  
	  
	    // UE 0
	    if (dlsch_active_0 == 1)
	      {
		if ((Ns==1) && (l==0)) // process symbols 3,4,5
		  {
		    for (m=4;m<6;m++)
		      rx_dlsch(&PHY_vars_ue[0]->lte_ue_common_vars,
			       PHY_vars_ue[0]->lte_ue_dlsch_vars,
			       lte_frame_parms,
			       eNb_id,
			       eNb_id_i,
			       PHY_vars_ue[0]->dlsch_ue[0],
			       m,
			       dual_stream_UE,
			       &PHY_vars_ue[0]->PHY_measurements,
			       0);
		  }
	      
	      
		if ((Ns==1) && (l==3)) 
		  {// process symbols 6,7,8
		    /*
		    //UE 0
		    if (rx_pbch(&PHY_vars_ue[0]->lte_ue_common_vars,
		    PHY_vars_ue[0]->lte_ue_pbch_vars[0],
		    lte_frame_parms,
		    0,
		    SISO)) {
		    msg("UE 0 pbch decoded sucessfully!\n");
		    }
		    else {
		    msg("UE 0 pbch not decoded!\n");
		    }*/
		  
		  
		    for (m=7;m<9;m++)
		      rx_dlsch(&PHY_vars_ue[0]->lte_ue_common_vars,
			       PHY_vars_ue[0]->lte_ue_dlsch_vars,
			       lte_frame_parms,
			       eNb_id,
			       eNb_id_i,
			       PHY_vars_ue[0]->dlsch_ue[0],
			       m,
			       dual_stream_UE,
			       &PHY_vars_ue[0]->PHY_measurements,
			       0);
		  }// process symbols 6,7,8
	      
	      
		if ((Ns==2) && (l==0))  // process symbols 10,11, do deinterleaving for TTI
		  { 
		    for (m=10;m<12;m++)
		      rx_dlsch(&PHY_vars_ue[0]->lte_ue_common_vars,
			       PHY_vars_ue[0]->lte_ue_dlsch_vars,
			       lte_frame_parms,
			       eNb_id,
			       eNb_id_i,
			       PHY_vars_ue[0]->dlsch_ue[0],
			       m,
			       dual_stream_UE,
			       &PHY_vars_ue[0]->PHY_measurements,
			       0);
		  }
	      }//dlsch_active_0 == 1
	  
	  
	  }//loop over l
	}//loop over Ns
      
      
      
	if (dlsch_active_0 == 1)
	  {
	    // UE 0
#ifdef OUTPUT_DEBUG      
	  
	    write_output("UE0_rxsig0.m","UE0_rxs0",PHY_vars_ue[0]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
	    write_output("UE0_dlsch00_ch0.m","UE0_dl00_ch0",&(PHY_vars_ue[0]->lte_ue_common_vars.dl_ch_estimates[eNb_id][0][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	  
	  
	    if (num_layers>1) {
	      write_output("UE0_dlsch01_ch0.m","UE0_dl01_ch0",&(PHY_vars_ue[0]->lte_ue_common_vars.dl_ch_estimates[eNb_id][1][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	      write_output("UE0_dlsch10_ch0.m","UE0_dl10_ch0",&(PHY_vars_ue[0]->lte_ue_common_vars.dl_ch_estimates[eNb_id][2][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	      write_output("UE0_dlsch11_ch0.m","UE0_dl11_ch0",&(PHY_vars_ue[0]->lte_ue_common_vars.dl_ch_estimates[eNb_id][3][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	    }
	  
	  
	    write_output("UE0_rxsigF0.m","UE0_rxsF0",PHY_vars_ue[0]->lte_ue_common_vars.rxdataF[0],2*12*lte_frame_parms->ofdm_symbol_size,2,1);
	    write_output("UE0_rxsigF0_ext.m","UE0_rxsF0_ext",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->rxdataF_ext[0],2*12*lte_frame_parms->ofdm_symbol_size,1,1);
	    write_output("UE0_dlsch00_ch0_ext.m","UE0_dl00_ch0_ext",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[0],300*12,1,1);
	    write_output("UE0_pdcchF0_ext.m","UE0_pdcchF_ext",PHY_vars_ue[0]->lte_ue_pdcch_vars[eNb_id]->rxdataF_ext[0],2*3*lte_frame_parms->ofdm_symbol_size,1,1);
	    write_output("UE0_pdcch00_ch0_ext.m","UE0_pdcch00_ch0_ext",PHY_vars_ue[0]->lte_ue_pdcch_vars[eNb_id]->dl_ch_estimates_ext[0],300*3,1,1);
	  
	  
	    if (num_layers>1) {
	      write_output("UE0_dlsch01_ch0_ext.m","UE0_dl01_ch0_ext",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[1],300*12,1,1);
	      write_output("UE0_dlsch10_ch0_ext.m","UE0_dl10_ch0_ext",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[2],300*12,1,1);
	      write_output("UE0_dlsch11_ch0_ext.m","UE0_dl11_ch0_ext",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[3],300*12,1,1);
	      write_output("UE0_dlsch_rho.m","UE0_dl_rho",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->rho[0],300*12,1,1);
	    }
	  
	  
	    write_output("UE0_dlsch_rxF_comp0.m","UE0_dlsch0_rxF_comp0",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->rxdataF_comp[0],300*12,1,1);
	    write_output("UE0_pdcch_rxF_comp0.m","UE0_pdcch0_rxF_comp0",PHY_vars_ue[0]->lte_ue_pdcch_vars[eNb_id]->rxdataF_comp[0],4*300,1,1);
	    write_output("UE0_dlsch_rxF_llr.m","UE0_dlsch_llr",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->llr[0],coded_bits_per_codeword,1,0);
	    write_output("UE0_pdcch_rxF_llr.m","UE0_pdcch_llr",PHY_vars_ue[0]->lte_ue_pdcch_vars[eNb_id]->llr,2400,1,4);
	    write_output("UE0_dlsch_mag1.m","UE0_dlschmag1",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->dl_ch_mag,300*12,1,1);
	    write_output("UE0_dlsch_mag2.m","UE0_dlschmag2",PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->dl_ch_magb,300*12,1,1);
	  
#endif //OUTPUT_DEBUG
	  

	    //printf("Calling decoding for UE0\n");
	  
	  
	    ret_0 = dlsch_decoding(PHY_vars_ue[0]->lte_ue_dlsch_vars[eNb_id]->llr[0],		 
				   lte_frame_parms,
				   PHY_vars_ue[0]->dlsch_ue[0][0],
				   0);
	  
	    if (ret_0 <= MAX_TURBO_ITERATIONS) {
	      decode_error_0 = 0;
#ifdef OUTPUT_DEBUG  
	      printf("UE0 No DLSCH errors found\n");
#endif
	    }	
	    else {
	      decode_error_0 = 1;
#ifdef OUTPUT_DEBUG  
	      printf("DLSCH in error for UE0\n");
#endif
	    }
	  
	    //#ifdef OUTPUT_DEBUG  
	    j=0;
	    for (s=0;s<PHY_vars_ue[0]->dlsch_ue[0][0]->harq_processes[0]->C;s++) {
	      if (s<PHY_vars_ue[0]->dlsch_ue[0][0]->harq_processes[0]->Cminus)
		Kr = PHY_vars_ue[0]->dlsch_ue[0][0]->harq_processes[0]->Kminus;
	      else
		Kr = PHY_vars_ue[0]->dlsch_ue[0][0]->harq_processes[0]->Kplus;
	    
	      Kr_bytes = Kr>>3;
	    
	      // printf("UE0 Decoded_output (Segment %d):\n",s);
	      for (i=0;i<Kr_bytes;i++){
		input_buffer_UE0_temp[j] = PHY_vars_ue[0]->dlsch_ue[0][0]->harq_processes[0]->c[s][i];
		j++;
		// printf("%d : %x (%x)\n",i,PHY_vars_ue[0]->dlsch_ue[0][0]->harq_processes[0]->c[s][i],PHY_vars_ue[0]->dlsch_ue[0][0]->harq_processes[0]->c[s][i]^PHY_vars_eNB[0]->dlsch_eNb[0][0]->harq_processes[0]->c[s][i]);
	      }
	    }
	  
	    //#endif
	  
	  
	  

	  }// dlsch_active_0 == 1
      
      


#ifdef COLLABRATIVE_SCHEME
	//eNB0 to UE1
	multipath_channel(ch[SeSu],s_re,s_im,r_re0_1,r_im0_1,
			  amps,Td,BW,ricean_factor,aoa,
			  lte_frame_parms->nb_antennas_tx,
			  lte_frame_parms->nb_antennas_rx,
			  FRAME_LENGTH_COMPLEX_SAMPLES,
			  channel_length,
			  0,1//forgetting factor
			  ,((first_call0_1 == 1)?1:0),0,SeSu,0);
			  
	if (first_call0_1 == 1)
	  first_call0_1 = 0;
      

#ifdef OUTPUT_DEBUG
	write_output("channel0_dl1.m","chan0_dl1",ch[SeSu][0],channel_length,1,8);//UE 1 
#endif
      

	//AWGN
	sigma2_dl = pow(10,sigma2_dB_dl/10);
      
	for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
	  for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
	 	  
	    //UE 1
	    ((short*)PHY_vars_ue[1]->lte_ue_common_vars.rxdata[aa])[2*i] = (short) (r_re0_1[aa][i] + sqrt(sigma2_dl/2)*gaussdouble(0.0,1.0));
	    ((short*)PHY_vars_ue[1]->lte_ue_common_vars.rxdata[aa])[2*i+1] = (short) (r_im0_1[aa][i] + sqrt(sigma2_dl/2)*gaussdouble(0.0,1.0));
	  }
	}    


	//UE 1
	//    lte_sync_time_init(lte_frame_parms,PHY_vars_ue[0]->lte_ue_common_vars);
	//    lte_sync_time(PHY_vars_ue[0]->lte_ue_common_vars.rxdata, lte_frame_parms);
	//    lte_sync_time_free();
      


     
      
#ifdef OUTPUT_DEBUG
      
	// UE 1
	printf("RX level in null symbol UE1 %d\n",dB_fixed(signal_energy(&PHY_vars_ue[1]->lte_ue_common_vars.rxdata[0][160+OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
	printf("RX level in data symbol UE1 %d\n",dB_fixed(signal_energy(&PHY_vars_ue[1]->lte_ue_common_vars.rxdata[0][160+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
	printf("rx_level Null symbol UE1 %f\n",10*log10(signal_energy_fp(r_re0_1,r_im0_1,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
	printf("rx_level data symbol UE1 %f\n",10*log10(signal_energy_fp(r_re0_1,r_im0_1,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
      
#endif
      
      
      
	SNRmeas_1 = 10*log10((signal_energy_fp(r_re0_1,r_im0_1,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))/signal_energy_fp(r_re0_1,r_im0_1,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(1*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))) - 1);// UE 1
      
    


	// Inner receiver scheduling for 3 slots for UE 1
      
	for (Ns=0;Ns<3;Ns++) {
	  for (l=0;l<6;l++) {
	  
	  
	    slot_fep(lte_frame_parms,
		     &PHY_vars_ue[1]->lte_ue_common_vars,
		     l,
		     Ns%20,
		     0,
		     0);// UE 1
	  
	  
	  

	    lte_ue_measurements(PHY_vars_ue[1],
				lte_frame_parms,
				0,
				1,
				0);// UE 1
	  
	  
	  
	    if ((Ns==0) && (l==3)) {// process symbols 0,1,2
	    
	   
	      //UE 1
	      rx_pdcch(&PHY_vars_ue[1]->lte_ue_common_vars,
		       PHY_vars_ue[1]->lte_ue_pdcch_vars,
		       lte_frame_parms,
		       eNb_id,
		       2,
		       (lte_frame_parms->mode1_flag == 1) ? SISO : ALAMOUTI,
		       0);// UE 1
	    
	      dci_cnt_1 = dci_decoding_procedure(PHY_vars_ue[1]->lte_ue_pdcch_vars,dci_alloc_rx_1,eNb_id,lte_frame_parms,SI_RNTI,RA_RNTI);
	      if (dci_cnt_1 == 0)
		{
		  dlsch_active_1 = 0;
		  dci_errors_1++;
		}


	      for (i=0;i<dci_cnt_1;i++)
		if ((dci_alloc_rx_1[i].rnti == C_RNTI) && (dci_alloc_rx_1[i].format == format2_2A_M10PRB))
		  {
		    generate_ue_dlsch_params_from_dci(0,
						      (DCI2_5MHz_2A_M10PRB_TDD_t *)&dci_alloc_rx_1[i].dci_pdu,
						      C_RNTI,
						      format2_2A_M10PRB,
						      PHY_vars_ue[1]->dlsch_ue[0],
						      lte_frame_parms,
						      SI_RNTI,
						      RA_RNTI,
						      P_RNTI);
		    dlsch_active_1 = 1;
		  }
		else {
		  dlsch_active_1 = 0;
		  dci_errors_1++;
		}
	    
	      /*	else if ((dci_alloc_rx_1[i].rnti == SI_RNTI) && (dci_alloc_rx_1[i].format == format1A))
			generate_ue_dlsch_params_from_dci(0,
			(DCI1A_5MHz_TDD_1_6_t *)&dci_alloc_rx_1[i].dci_pdu,
			SI_RNTI,
			format1A,
			&PHY_vars_ue[1]->dlsch_ue_cntl, 
			lte_frame_parms,
			SI_RNTI,
			RA_RNTI,
			P_RNTI);
	      */
	      //  msg("dci_cnt UE1 = %d\n",dci_cnt_1);
	    
	    }// process symbols 0,1,2
	  
	  
	  
	 
	  
	    /*
	      for (m=lte_frame_parms->first_dlsch_symbol;m<3;m++)
	      rx_dlsch(PHY_vars_ue[0]->lte_ue_common_vars,
	      PHY_vars_ue[1]->lte_ue_dlsch_vars,
	      lte_frame_parms,
	      eNb_id,
	      eNb_id_i,
	      m,
	      rb_alloc,
	      mod_order,
	      mimo_mode,
	      dual_stream_UE);// UE 1
	    */
	  
	  
	  
	 

	    // UE 1
	    if (dlsch_active_1 == 1)
	      {
		if ((Ns==1) && (l==0)) // process symbols 3,4,5
		  {
		    for (m=4;m<6;m++)
		      rx_dlsch(&PHY_vars_ue[1]->lte_ue_common_vars,
			       PHY_vars_ue[1]->lte_ue_dlsch_vars,
			       lte_frame_parms,
			       eNb_id,
			       eNb_id_i,
			       PHY_vars_ue[1]->dlsch_ue[0],
			       m,
			       dual_stream_UE,
			       &PHY_vars_ue[1]->PHY_measurements,
			       0);
		  }
	      
	      
		if ((Ns==1) && (l==3)) 
		  {// process symbols 6,7,8
		  
		  
		  
		    for (m=7;m<9;m++)
		      rx_dlsch(&PHY_vars_ue[1]->lte_ue_common_vars,
			       PHY_vars_ue[1]->lte_ue_dlsch_vars,
			       lte_frame_parms,
			       eNb_id,
			       eNb_id_i,
			       PHY_vars_ue[1]->dlsch_ue[0],
			       m,
			       dual_stream_UE,
			       &PHY_vars_ue[1]->PHY_measurements,
			       0);
		  }// process symbols 6,7,8
	      
	      
		if ((Ns==2) && (l==0))  // process symbols 10,11, do deinterleaving for TTI
		  { 
		    for (m=10;m<12;m++)
		      rx_dlsch(&PHY_vars_ue[1]->lte_ue_common_vars,
			       PHY_vars_ue[1]->lte_ue_dlsch_vars,
			       lte_frame_parms,
			       eNb_id,
			       eNb_id_i,
			       PHY_vars_ue[1]->dlsch_ue[0],
			       m,
			       dual_stream_UE,
			       &PHY_vars_ue[1]->PHY_measurements,
			       0);
		  }
	      }//dlsch_active_1 == 1
	  
	  
	  }//loop over l
	}//loop over Ns
      
    
    
      
      
	// UE 1
	if (dlsch_active_1 == 1)
	  {
	  
#ifdef OUTPUT_DEBUG      
	    write_output("UE1_rxsig0.m","UE1_rxs0",PHY_vars_ue[1]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
	    write_output("UE1_dlsch00_ch0.m","UE1_dl00_ch0",&(PHY_vars_ue[1]->lte_ue_common_vars.dl_ch_estimates[eNb_id][0][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	  
	  
	  
	    if (num_layers>1) {
	      write_output("UE1_dlsch01_ch0.m","UE1_dl01_ch0",&(PHY_vars_ue[1]->lte_ue_common_vars.dl_ch_estimates[eNb_id][1][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	      write_output("UE1_dlsch10_ch0.m","UE1_dl10_ch0",&(PHY_vars_ue[1]->lte_ue_common_vars.dl_ch_estimates[eNb_id][2][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	      write_output("UE1_dlsch11_ch0.m","UE1_dl11_ch0",&(PHY_vars_ue[1]->lte_ue_common_vars.dl_ch_estimates[eNb_id][3][0]),(6*(lte_frame_parms->ofdm_symbol_size)),1,1);
	    }
	  
	  
	    write_output("UE1_rxsigF0.m","UE1_rxsF0",PHY_vars_ue[1]->lte_ue_common_vars.rxdataF[0],2*12*lte_frame_parms->ofdm_symbol_size,2,1);
	    write_output("UE1_rxsigF0_ext.m","UE1_rxsF0_ext",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->rxdataF_ext[0],2*12*lte_frame_parms->ofdm_symbol_size,1,1);
	    write_output("UE1_dlsch00_ch0_ext.m","UE1_dl00_ch0_ext",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[0],300*12,1,1);
	    write_output("UE1_pdcchF0_ext.m","UE1_pdcchF_ext",PHY_vars_ue[1]->lte_ue_pdcch_vars[eNb_id]->rxdataF_ext[0],2*3*lte_frame_parms->ofdm_symbol_size,1,1);
	    write_output("UE1_pdcch00_ch0_ext.m","UE1_pdcch00_ch0_ext",PHY_vars_ue[1]->lte_ue_pdcch_vars[eNb_id]->dl_ch_estimates_ext[0],300*3,1,1);
	  
	  
	    if (num_layers>1) {
	      write_output("UE1_dlsch01_ch0_ext.m","UE1_dl01_ch0_ext",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[1],300*12,1,1);
	      write_output("UE1_dlsch10_ch0_ext.m","UE1_dl10_ch0_ext",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[2],300*12,1,1);
	      write_output("UE1_dlsch11_ch0_ext.m","UE1_dl11_ch0_ext",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->dl_ch_estimates_ext[3],300*12,1,1);
	      write_output("UE1_dlsch_rho.m","UE1_dl_rho",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->rho[0],300*12,1,1);
	    }
	  
	  
	    write_output("UE1_dlsch_rxF_comp0.m","UE1_dlsch0_rxF_comp0",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->rxdataF_comp[0],300*12,1,1);
	    write_output("UE1_pdcch_rxF_comp0.m","UE1_pdcch0_rxF_comp0",PHY_vars_ue[1]->lte_ue_pdcch_vars[eNb_id]->rxdataF_comp[0],4*300,1,1);
	    write_output("UE1_dlsch_rxF_llr.m","UE1_dlsch_llr",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->llr[0],coded_bits_per_codeword,1,0);
	    write_output("UE1_pdcch_rxF_llr.m","UE1_pdcch_llr",PHY_vars_ue[1]->lte_ue_pdcch_vars[eNb_id]->llr,2400,1,4);
	    write_output("UE1_dlsch_mag1.m","UE1_dlschmag1",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->dl_ch_mag,300*12,1,1);
	    write_output("UE1_dlsch_mag2.m","UE1_dlschmag2",PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->dl_ch_magb,300*12,1,1);
	  
#endif //OUTPUT_DEBUG
	  



	    //printf("Calling decoding for UE1\n");
	  
	    ret_1 = dlsch_decoding(PHY_vars_ue[1]->lte_ue_dlsch_vars[eNb_id]->llr[0],		 
				   lte_frame_parms,
				   PHY_vars_ue[1]->dlsch_ue[0][0],
				   0);
	  
	    if (ret_1 <= MAX_TURBO_ITERATIONS) {
	      decode_error_1 = 0;
#ifdef OUTPUT_DEBUG  
	      printf("UE1 No DLSCH errors found\n");
#endif
	    }	
	    else {
	      decode_error_1 = 1;
#ifdef OUTPUT_DEBUG  
	      printf("UE1 DLSCH in error\n");
#endif
	    }
	  
	
	    //#ifdef OUTPUT_DEBUG  
	    j=0;
	    for (s=0;s<PHY_vars_ue[1]->dlsch_ue[0][0]->harq_processes[0]->C;s++) {
	      if (s<PHY_vars_ue[1]->dlsch_ue[0][0]->harq_processes[0]->Cminus)
		Kr = PHY_vars_ue[1]->dlsch_ue[0][0]->harq_processes[0]->Kminus;
	      else
		Kr = PHY_vars_ue[1]->dlsch_ue[0][0]->harq_processes[0]->Kplus;
	  
	      Kr_bytes = Kr>>3;
	  
	      //  printf("UE1 Decoded_output (Segment %d):\n",s);
	      for (i=0;i<Kr_bytes;i++){
		input_buffer_UE1_temp[j] = PHY_vars_ue[1]->dlsch_ue[0][0]->harq_processes[0]->c[s][i];
		j++;
		// printf("%d : %x (%x)\n",i,PHY_vars_ue[1]->dlsch_ue[0][0]->harq_processes[0]->c[s][i],PHY_vars_ue[1]->dlsch_ue[0][0]->harq_processes[0]->c[s][i]^PHY_vars_eNB[0]->dlsch_eNb[1][0]->harq_processes[0]->c[s][i]);
	      }
	    }
	  
	    //#endif
	

	  }  // dlsch_active_1 == 1
#endif


      
#ifdef COLLABRATIVE_SCHEME
	if((dlsch_active_0 == 0) && (dlsch_active_1 == 0)){
	  n_errors_dl++;
	}
	else 	if(((dlsch_active_0 == 1) && (decode_error_0 == 1)) && ((dlsch_active_1 == 1) && (decode_error_1 == 1))){
	  n_errors_dl++;
	}
	else if ((dlsch_active_0 == 0) && ((dlsch_active_1 == 1) && (decode_error_1 == 1))){
	  n_errors_dl++;;
	}
	else if (((dlsch_active_0 == 1) && (decode_error_0 == 1)) && (dlsch_active_1 == 0)){
	  n_errors_dl++;
	}
#else
	if(dlsch_active_0 == 0){
	  n_errors_dl++;
	}
	else 	if((dlsch_active_0 == 1) && (decode_error_0 == 1)){
	  n_errors_dl++;
	}
#endif
      

    

      

#ifdef COLLABRATIVE_SCHEME
	  if(((dlsch_active_0 == 1) && (decode_error_0 == 0)) && ((dlsch_active_1 == 1) && (decode_error_1 == 0))){ 



	    trials_ul++;

	    // UE0 ULSCH params
	    generate_ue_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					      C_RNTI,
					      8,
					      format0,
					      PHY_vars_ue[0]->ulsch_ue[1],
					      PHY_vars_ue[0]->dlsch_ue[1],
					      &PHY_vars_ue[0]->PHY_measurements,
					      lte_frame_parms,
					      SI_RNTI,
					      RA_RNTI,
					      P_RNTI,
					      0,
					      0);//current_dlsch_cqi);


	    //UE0 to eNB1: eNB1 ULSCH params
	    generate_eNb_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					       SI_RNTI,
					       8,
					       format0,
					       PHY_vars_eNB[1]->ulsch_eNb[0],
					       lte_frame_parms,
					       SI_RNTI,
					       RA_RNTI,
					       P_RNTI);
	  	  


	  
	  

	  
	  
	    // UL SImulation for UE0 to destination eNB
	  
	  
	 
	    subframe = 2;

	  
	    generate_srs_tx(lte_frame_parms,
			    &PHY_vars_ue[0]->SRS_parameters,
			    PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],
			    AMP,subframe);


	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    generate_drs_pusch(lte_frame_parms,
			       PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],
			       AMP,subframe,
			       PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[0]->first_rb,
			       PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[0]->nb_rb,
			       0,relay_flag,diversity_scheme);
	  



	    harq_pid_ul = subframe2harq_pid_tdd(lte_frame_parms->tdd_config,subframe);
	    input_buffer_length_UE0 = PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[harq_pid_ul]->TBS/8;
	    //	printf("Input buffer size UE0 %d bytes\n",input_buffer_length_UE0);

	    input_buffer_UE0 = (unsigned char *)malloc(input_buffer_length_UE0+4);

	    for (i=0;i<input_buffer_length_UE0;i++) {
	      //input_buffer_UE0[i]= (unsigned char)(taus()&0xff);
	      input_buffer_UE0[i] = input_buffer_UE0_temp[i];
	      // printf("input UE0%d : %x\n",i,input_buffer_UE0[i]);
	    }


	  
	    ulsch_encoding(input_buffer_UE0,
			   lte_frame_parms,
			   PHY_vars_ue[0]->ulsch_ue[1],
			   harq_pid_ul);
	  

	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    ulsch_modulation(PHY_vars_ue[0]->lte_ue_common_vars.txdataF,
			     AMP,subframe,
			     lte_frame_parms,
			     PHY_vars_ue[0]->ulsch_ue[1],
			     relay_flag,diversity_scheme,0);
	  
	  
#ifdef IFFT_FPGA
	  
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF0.m","UE0_txsF0",PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],300*12,1,4);
#endif
	  
	  
	  
	    // do talbe lookup and write results to txdataF2
	  
	    for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++)
	      {
		l = 0;
		for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++)
		  {
		    if ((i%512>=1) && (i%512<=150))
		      txdataF2_UE0[aa][i] = ((int*)mod_table)[PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa][l++]];
		    else if (i%512>=362)
		      txdataF2_UE0[aa][i] = ((int*)mod_table)[PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa][l++]];
		    else 
		      txdataF2_UE0[aa][i] = 0;
		    printf("l=%d\n",l);
		  }
	      }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF20.m","UE0_txsF20", txdataF2_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(txdataF2_UE0[aa],        // input
			   txdata_UE0[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE0[aa][4*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsig0.m","UE0_txs0", txdata_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
#endif     
	  
	  
#else //IFFT_FPGA
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF0.m","UE0_txsF0", PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa],        // input
			   txdata_UE0[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE0[aa][OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*subframe*12],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsig0.m","UE0_txs0", txdata_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);  
#endif     
#endif

	    // printf("tx_lev_ul = %d\n",tx_lev_ul);
	    tx_lev_ul_dB = (unsigned int) dB_fixed(tx_lev_ul);
	  

	    // UE1 ULSCH params
	    generate_ue_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					      C_RNTI,
					      8,
					      format0,
					      PHY_vars_ue[1]->ulsch_ue[1],
					      PHY_vars_ue[1]->dlsch_ue[1],
					      &PHY_vars_ue[1]->PHY_measurements,
					      lte_frame_parms,
					      SI_RNTI,
					      RA_RNTI,
					      P_RNTI,
					      0,
					      0);


	    //UE1 to eNB1: eNB1 ULSCH params
	    generate_eNb_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					       SI_RNTI,
					       8,
					       format0,
					       PHY_vars_eNB[1]->ulsch_eNb[1],
					       lte_frame_parms,
					       SI_RNTI,
					       RA_RNTI,
					       P_RNTI);
	  	  


	  
	  

	  
	  
	    // UL SImulation for UE1 to destination eNB
	  
	  
	 
	    subframe = 2;

	  
	    generate_srs_tx(lte_frame_parms,
			    &PHY_vars_ue[1]->SRS_parameters,
			    PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],
			    AMP,subframe);


	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    generate_drs_pusch(lte_frame_parms,
			       PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],
			       AMP,subframe,
			       PHY_vars_ue[1]->ulsch_ue[1]->harq_processes[0]->first_rb,
			       PHY_vars_ue[1]->ulsch_ue[1]->harq_processes[0]->nb_rb,
			       1,relay_flag,diversity_scheme);
	  



	    harq_pid_ul = subframe2harq_pid_tdd(lte_frame_parms->tdd_config,subframe);
	    input_buffer_length_UE1 = PHY_vars_ue[1]->ulsch_ue[1]->harq_processes[harq_pid_ul]->TBS/8;
	    //	printf("Input buffer size UE1 %d bytes\n",input_buffer_length_UE1);

	    input_buffer_UE1 = (unsigned char *)malloc(input_buffer_length_UE1+4);


	    for (i=0;i<input_buffer_length_UE1;i++) {
	      //input_buffer_UE1[i]= (unsigned char)(taus()&0xff);
	      input_buffer_UE1[i] = input_buffer_UE1_temp[i];
	      //printf("input UE1 %d : %x\n",i,input_buffer_UE1[i]);
	    } 
	
  
	  
	    ulsch_encoding(input_buffer_UE1,
			   lte_frame_parms,
			   PHY_vars_ue[1]->ulsch_ue[1],
			   harq_pid_ul);
	  

	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    ulsch_modulation(PHY_vars_ue[1]->lte_ue_common_vars.txdataF,
			     AMP,subframe,
			     lte_frame_parms,
			     PHY_vars_ue[1]->ulsch_ue[1],
			     relay_flag,diversity_scheme,1);
	  
	  
#ifdef IFFT_FPGA
	  
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsigF0.m","UE1_txsF0",PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],300*12,1,4);
#endif
	  
	  
	  
	    // do talbe lookup and write results to txdataF2
	  
	    for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++)
	      {
		l = 0;
		for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++)
		  {
		    if ((i%512>=1) && (i%512<=150))
		      txdataF2_UE1[aa][i] = ((int*)mod_table)[PHY_vars_ue[1]->lte_ue_common_vars.txdataF[aa][l++]];
		    else if (i%512>=362)
		      txdataF2_UE1[aa][i] = ((int*)mod_table)[PHY_vars_ue[1]->lte_ue_common_vars.txdataF[aa][l++]];
		    else 
		      txdataF2_UE1[aa][i] = 0;
		    printf("l=%d\n",l);
		  }
	      }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsigF20.m","UE1_txsF20", txdataF2_UE1[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(txdataF2_UE1[aa],        // input
			   txdata_UE1[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE1[aa][4*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsig0.m","UE1_txs0", txdata_UE1[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
#endif     
	  
	  
#else //IFFT_FPGA
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsigF0.m","UE1_txsF0", PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(PHY_vars_ue[1]->lte_ue_common_vars.txdataF[aa],        // input
			   txdata_UE1[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE1[aa][OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*subframe*12],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsig0.m","UE1_txs0", txdata_UE1[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);  
#endif     
#endif

	    // printf("tx_lev_ul = %d\n",tx_lev_ul);
	    tx_lev_ul_dB = (unsigned int) dB_fixed(tx_lev_ul);
	  }//both relays work
	  else if((dlsch_active_0 == 1) && (decode_error_0 == 0)){


	    trials_ul++;

	    // UE0 ULSCH params
	    generate_ue_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					      C_RNTI,
					      8,
					      format0,
					      PHY_vars_ue[0]->ulsch_ue[1],
					      PHY_vars_ue[0]->dlsch_ue[1],
					      &PHY_vars_ue[0]->PHY_measurements,
					      lte_frame_parms,
					      SI_RNTI,
					      RA_RNTI,
					      P_RNTI,
					      0,
					      0);//current_dlsch_cqi);


	    //UE0 to eNB1: eNB1 ULSCH params
	    generate_eNb_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					       SI_RNTI,
					       8,
					       format0,
					       PHY_vars_eNB[1]->ulsch_eNb[0],
					       lte_frame_parms,
					       SI_RNTI,
					       RA_RNTI,
					       P_RNTI);
	  	  


	  
	  

	  
	  
	    // UL SImulation for UE0 to destination eNB
	  
	  
	 
	    subframe = 2;

	  
	    generate_srs_tx(lte_frame_parms,
			    &PHY_vars_ue[0]->SRS_parameters,
			    PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],
			    AMP,subframe);


	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    generate_drs_pusch(lte_frame_parms,
			       PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],
			       AMP,subframe,
			       PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[0]->first_rb,
			       PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[0]->nb_rb,
			       0,1,0);
	  



	    harq_pid_ul = subframe2harq_pid_tdd(lte_frame_parms->tdd_config,subframe);
	    input_buffer_length_UE0 = PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[harq_pid_ul]->TBS/8;
	    //	printf("Input buffer size UE0 %d bytes\n",input_buffer_length_UE0);

	    input_buffer_UE0 = (unsigned char *)malloc(input_buffer_length_UE0+4);

	    for (i=0;i<input_buffer_length_UE0;i++) {
	      //input_buffer_UE0[i]= (unsigned char)(taus()&0xff);
	      input_buffer_UE0[i] = input_buffer_UE0_temp[i];
	      // printf("input UE0%d : %x\n",i,input_buffer_UE0[i]);
	    }
	  

	  
	    ulsch_encoding(input_buffer_UE0,
			   lte_frame_parms,
			   PHY_vars_ue[0]->ulsch_ue[1],
			   harq_pid_ul);
	  

	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    ulsch_modulation(PHY_vars_ue[0]->lte_ue_common_vars.txdataF,
			     AMP,subframe,
			     lte_frame_parms,
			     PHY_vars_ue[0]->ulsch_ue[1],
			     1,0,0);
	  
	  
#ifdef IFFT_FPGA
	  
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF0.m","UE0_txsF0",PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],300*12,1,4);
#endif
	  
	  
	  
	    // do talbe lookup and write results to txdataF2
	  
	    for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++)
	      {
		l = 0;
		for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++)
		  {
		    if ((i%512>=1) && (i%512<=150))
		      txdataF2_UE0[aa][i] = ((int*)mod_table)[PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa][l++]];
		    else if (i%512>=362)
		      txdataF2_UE0[aa][i] = ((int*)mod_table)[PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa][l++]];
		    else 
		      txdataF2_UE0[aa][i] = 0;
		    printf("l=%d\n",l);
		  }
	      }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF20.m","UE0_txsF20", txdataF2_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(txdataF2_UE0[aa],        // input
			   txdata_UE0[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE0[aa][4*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsig0.m","UE0_txs0", txdata_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
#endif     
	  
	  
#else //IFFT_FPGA
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF0.m","UE0_txsF0", PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa],        // input
			   txdata_UE0[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE0[aa][OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*subframe*12],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsig0.m","UE0_txs0", txdata_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);  
#endif     
#endif

	    // printf("tx_lev_ul = %d\n",tx_lev_ul);
	    tx_lev_ul_dB = (unsigned int) dB_fixed(tx_lev_ul);
	  
	  }//when only relay 0 works
	  else if((dlsch_active_1 == 1) && (decode_error_1 == 0)){


	    trials_ul++;

	    // UE1 ULSCH params
	    generate_ue_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					      C_RNTI,
					      8,
					      format0,
					      PHY_vars_ue[1]->ulsch_ue[1],
					      PHY_vars_ue[1]->dlsch_ue[1],
					      &PHY_vars_ue[1]->PHY_measurements,
					      lte_frame_parms,
					      SI_RNTI,
					      RA_RNTI,
					      P_RNTI,
					      0,
					      0);


	    //UE1 to eNB1: eNB1 ULSCH params
	    generate_eNb_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					       SI_RNTI,
					       8,
					       format0,
					       PHY_vars_eNB[1]->ulsch_eNb[1],
					       lte_frame_parms,
					       SI_RNTI,
					       RA_RNTI,
					       P_RNTI);
	  	  


	  
	  

	  
	  
	    // UL SImulation for UE1 to destination eNB
	  
	  
	 
	    subframe = 2;

	  
	    generate_srs_tx(lte_frame_parms,
			    &PHY_vars_ue[1]->SRS_parameters,
			    PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],
			    AMP,subframe);


	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    generate_drs_pusch(lte_frame_parms,
			       PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],
			       AMP,subframe,
			       PHY_vars_ue[1]->ulsch_ue[1]->harq_processes[0]->first_rb,
			       PHY_vars_ue[1]->ulsch_ue[1]->harq_processes[0]->nb_rb,
			       1,1,0);
	  



	    harq_pid_ul = subframe2harq_pid_tdd(lte_frame_parms->tdd_config,subframe);
	    input_buffer_length_UE1 = PHY_vars_ue[1]->ulsch_ue[1]->harq_processes[harq_pid_ul]->TBS/8;
	    //	printf("Input buffer size UE1 %d bytes\n",input_buffer_length_UE1);

	    input_buffer_UE1 = (unsigned char *)malloc(input_buffer_length_UE1+4);


	    for (i=0;i<input_buffer_length_UE1;i++) {
	      //input_buffer_UE1[i]= (unsigned char)(taus()&0xff);
	      input_buffer_UE1[i] = input_buffer_UE1_temp[i];
	      //printf("input UE1 %d : %x\n",i,input_buffer_UE1[i]);
	    } 
	

 
	  
	    ulsch_encoding(input_buffer_UE1,
			   lte_frame_parms,
			   PHY_vars_ue[1]->ulsch_ue[1],
			   harq_pid_ul);
	  

	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    ulsch_modulation(PHY_vars_ue[1]->lte_ue_common_vars.txdataF,
			     AMP,subframe,
			     lte_frame_parms,
			     PHY_vars_ue[1]->ulsch_ue[1],
			     1,0,1);
	  
	  
#ifdef IFFT_FPGA
	  
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsigF0.m","UE1_txsF0",PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],300*12,1,4);
#endif
	  
	  
	  
	    // do talbe lookup and write results to txdataF2
	  
	    for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++)
	      {
		l = 0;
		for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++)
		  {
		    if ((i%512>=1) && (i%512<=150))
		      txdataF2_UE1[aa][i] = ((int*)mod_table)[PHY_vars_ue[1]->lte_ue_common_vars.txdataF[aa][l++]];
		    else if (i%512>=362)
		      txdataF2_UE1[aa][i] = ((int*)mod_table)[PHY_vars_ue[1]->lte_ue_common_vars.txdataF[aa][l++]];
		    else 
		      txdataF2_UE1[aa][i] = 0;
		    printf("l=%d\n",l);
		  }
	      }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsigF20.m","UE1_txsF20", txdataF2_UE1[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(txdataF2_UE1[aa],        // input
			   txdata_UE1[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE1[aa][4*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsig0.m","UE1_txs0", txdata_UE1[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
#endif     
	  
	  
#else //IFFT_FPGA
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsigF0.m","UE1_txsF0", PHY_vars_ue[1]->lte_ue_common_vars.txdataF[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(PHY_vars_ue[1]->lte_ue_common_vars.txdataF[aa],        // input
			   txdata_UE1[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE1[aa][OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*subframe*12],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE1_txsig0.m","UE1_txs0", txdata_UE1[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);  
#endif     
#endif

	    // printf("tx_lev_ul = %d\n",tx_lev_ul);
	    tx_lev_ul_dB = (unsigned int) dB_fixed(tx_lev_ul);
	  }// when only relay 1 works
#else
	  if((dlsch_active_0 == 1) && (decode_error_0 == 0)){


	    trials_ul++;


	    // UE0 ULSCH params
	    generate_ue_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					      C_RNTI,
					      8,
					      format0,
					      PHY_vars_ue[0]->ulsch_ue[1],
					      PHY_vars_ue[0]->dlsch_ue[1],
					      &PHY_vars_ue[0]->PHY_measurements,
					      lte_frame_parms,
					      SI_RNTI,
					      RA_RNTI,
					      P_RNTI,
					      0,
					      0);//current_dlsch_cqi);


	    //UE0 to eNB1: eNB1 ULSCH params
	    generate_eNb_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
					       SI_RNTI,
					       8,
					       format0,
					       PHY_vars_eNB[1]->ulsch_eNb[0],
					       lte_frame_parms,
					       SI_RNTI,
					       RA_RNTI,
					       P_RNTI);
	  	  


	  
	  

	  
	  
	    // UL SImulation for UE0 to destination eNB
	  
	  
	 
	    subframe = 2;

	  
	    generate_srs_tx(lte_frame_parms,
			    &PHY_vars_ue[0]->SRS_parameters,
			    PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],
			    AMP,subframe);


	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    generate_drs_pusch(lte_frame_parms,
			       PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],
			       AMP,subframe,
			       PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[0]->first_rb,
			       PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[0]->nb_rb,
			       0,relay_flag,diversity_scheme);
	  



	    harq_pid_ul = subframe2harq_pid_tdd(lte_frame_parms->tdd_config,subframe);
	    input_buffer_length_UE0 = PHY_vars_ue[0]->ulsch_ue[1]->harq_processes[harq_pid_ul]->TBS/8;
	    //	printf("Input buffer size UE0 %d bytes\n",input_buffer_length_UE0);

	    input_buffer_UE0 = (unsigned char *)malloc(input_buffer_length_UE0+4);

	    for (i=0;i<input_buffer_length_UE0;i++) {
	      //input_buffer_UE0[i]= (unsigned char)(taus()&0xff);
	      input_buffer_UE0[i] = input_buffer_UE0_temp[i];
	      // printf("input UE0%d : %x\n",i,input_buffer_UE0[i]);
	    }
	  

	  
	    ulsch_encoding(input_buffer_UE0,
			   lte_frame_parms,
			   PHY_vars_ue[0]->ulsch_ue[1],
			   harq_pid_ul);
	  

	    // 3 Additional Flags are used: Indicating the UE ID, Relay Flag & Diversity Scheme
	    ulsch_modulation(PHY_vars_ue[0]->lte_ue_common_vars.txdataF,
			     AMP,subframe,
			     lte_frame_parms,
			     PHY_vars_ue[0]->ulsch_ue[1],
			     relay_flag,diversity_scheme,0);
	  
	  
#ifdef IFFT_FPGA
	  
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF0.m","UE0_txsF0",PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],300*12,1,4);
#endif
	  
	  
	  
	    // do talbe lookup and write results to txdataF2
	  
	    for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++)
	      {
		l = 0;
		for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++)
		  {
		    if ((i%512>=1) && (i%512<=150))
		      txdataF2_UE0[aa][i] = ((int*)mod_table)[PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa][l++]];
		    else if (i%512>=362)
		      txdataF2_UE0[aa][i] = ((int*)mod_table)[PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa][l++]];
		    else 
		      txdataF2_UE0[aa][i] = 0;
		    printf("l=%d\n",l);
		  }
	      }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF20.m","UE0_txsF20", txdataF2_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(txdataF2_UE0[aa],        // input
			   txdata_UE0[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE0[aa][4*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsig0.m","UE0_txs0", txdata_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
#endif     
	  
	  
#else //IFFT_FPGA
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsigF0.m","UE0_txsF0", PHY_vars_ue[0]->lte_ue_common_vars.txdataF[0],FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,1,1);
#endif
	  
	  
	    tx_lev_ul = 0;
	    for (aa=0; aa<lte_frame_parms->nb_antennas_tx; aa++) {
	      PHY_ofdm_mod(PHY_vars_ue[0]->lte_ue_common_vars.txdataF[aa],        // input
			   txdata_UE0[aa],         // output
			   lte_frame_parms->log2_symbol_size,                // log2_fft_size
			   12*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME,                 // number of symbols
			   lte_frame_parms->nb_prefix_samples,               // number of prefix samples
			   lte_frame_parms->twiddle_ifft,  // IFFT twiddle factors
			   lte_frame_parms->rev,           // bit-reversal permutation
			   CYCLIC_PREFIX);
	      tx_lev_ul += signal_energy(&txdata_UE0[aa][OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES*subframe*12],
					 OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }
	  
#ifdef OUTPUT_DEBUG
	    write_output("UE0_txsig0.m","UE0_txs0", txdata_UE0[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);  
#endif     
#endif

	    // printf("tx_lev_ul = %d\n",tx_lev_ul);
	    tx_lev_ul_dB = (unsigned int) dB_fixed(tx_lev_ul);
	  }//single relay case
#endif
		
	  
	


   
#ifdef COLLABRATIVE_SCHEME
	  if(((dlsch_active_0 == 1)&& (decode_error_0 == 0)) && ((dlsch_active_1 == 1)&& (decode_error_1 == 0)))// When both the relays transmit
	    {


	      //UE0
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {
		  s_re_0[aa][i] = ((double)(((short *)txdata_UE0[aa]))[(i<<1)]);
		  s_im_0[aa][i] = ((double)(((short *)txdata_UE0[aa]))[(i<<1)+1]);
		}
	      }
	  
	      //UE1
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {
		  s_re_1[aa][i] = ((double)(((short *)txdata_UE1[aa]))[(i<<1)]);
		  s_im_1[aa][i] = ((double)(((short *)txdata_UE1[aa]))[(i<<1)+1]);
		}
	      }
	      
		
		 
      
	
	  

	
	      // multipath channel

	  
	  
	      // UE 0
	      multipath_channel(ch[FuDe],s_re_0,s_im_0,r_re1_0,r_im1_0,
				amps,Td,BW,ricean_factor,aoa,
				lte_frame_parms->nb_antennas_tx,
				lte_frame_parms->nb_antennas_rx,
				FRAME_LENGTH_COMPLEX_SAMPLES,
				channel_length,
				0,1//forgetting factor
				,((first_call1_0 == 1)?1:0),0,FuDe,0);
	  
	      if(first_call1_0 == 1)
		first_call1_0 = 0;
	    
	      //write_output("channel0.m","chan0",ch[0],channel_length,1,8);
	    
	      // scale by path_loss = NOW - P_noise
	      //sigma2_ul       = pow(10,sigma2_dB_ul/10);
	      //N0W          = -95.87;
	      //path_loss_dB = N0W - sigma2_ul;
	      //path_loss    = pow(10,path_loss_dB/10);
	      path_loss_dB = 0;
	      path_loss = 1;
	    
	    
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
		  r_re1_0[aa][i]=r_re1_0[aa][i]*sqrt(path_loss); 
		  r_im1_0[aa][i]=r_im1_0[aa][i]*sqrt(path_loss); 
		}
	      }
	    
	    

	      //UE 1
	      multipath_channel(ch[SuDe],s_re_1,s_im_1,r_re1_1,r_im1_1,
				amps,Td,BW,ricean_factor,aoa,
				lte_frame_parms->nb_antennas_tx,
				lte_frame_parms->nb_antennas_rx,
				FRAME_LENGTH_COMPLEX_SAMPLES,
				channel_length,
				0,1//forgetting factor
				,((first_call1_1 == 1)?1:0),0,SuDe,0);

	      if(first_call1_1 == 1)
		first_call1_1 = 1;
    
	      //write_output("channel0.m","chan0",ch[0],channel_length,1,8);
	    
	      // scale by path_loss = NOW - P_noise
	      //sigma2_ul       = pow(10,sigma2_dB_ul/10);
	      //N0W          = -95.87;
	      //path_loss_dB = N0W - sigma2_ul;
	      //path_loss    = pow(10,path_loss_dB/10);
	      path_loss_dB = 0;
	      path_loss = 1;
	    
	    
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
		  if(i < channel_offset){
		    r_re1_1[aa][i]=0; 
		    r_im1_1[aa][i]=0; 
		  }
		  else{
		    r_re1_1[aa][i]=r_re1_1[aa][i-channel_offset]*sqrt(path_loss); 
		    r_im1_1[aa][i]=r_im1_1[aa][i-channel_offset]*sqrt(path_loss); 
		  }
		}
	      }
	    
   
	    
	      //AWGN

	      sigma2_dB_ul = tx_lev_ul_dB  - SNR_ul;
	      // printf("**********************SNR_ul = %f dB (tx_lev_ul_dB %f, sigma2_dB_ul %f)**************************\n",SNR_ul,(double)tx_lev_ul_dB+10*log10(25/NB_RB),sigma2_dB_ul);
	      sigma2_ul = pow(10,sigma2_dB_ul/10);
	      //printf("sigma2_ul = %g\n",sigma2_ul);
	      for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
		  // For 2 UEs 
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i] = (short) ((r_re1_0[aa][i]) + sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0)+ (r_re1_1[aa][i]));
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i+1] = (short) ((r_im1_0[aa][i]) + sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0)+(r_im1_1[aa][i]));
		}
	      }
	  
      
      
      
	    


	      for (l=subframe*lte_frame_parms->symbols_per_tti;l<((1+subframe)*lte_frame_parms->symbols_per_tti);l++)
		{
		
		  slot_fep_ul(lte_frame_parms,
			      &PHY_vars_eNB[1]->lte_eNB_common_vars,
			      l%(lte_frame_parms->symbols_per_tti/2),
			      l/(lte_frame_parms->symbols_per_tti/2),
			      0,
			      0);
		}



	      rx_ulsch(&PHY_vars_eNB[1]->lte_eNB_common_vars,
		       PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0],
		       lte_frame_parms,
		       subframe,
		       0,  
		       PHY_vars_eNB[1]->ulsch_eNb[0],
		       relay_flag,
		       diversity_scheme);
		
		
		
	      ret_ul = ulsch_decoding(PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0]->llr,
				      lte_frame_parms,
				      PHY_vars_eNB[1]->ulsch_eNb[0],
				      subframe);

    
	      if (ret_ul == (1+MAX_TURBO_ITERATIONS)) {
		n_errors_ul++;
	      }
 
	    }   // When both relays forward
	  else if((dlsch_active_0 == 1)&& (decode_error_0 == 0))// When only relay 0 (UE0) transmits
	    {

	    
	      //UE0
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {
		  s_re_0[aa][i] = ((double)(((short *)txdata_UE0[aa]))[(i<<1)]);
		  s_im_0[aa][i] = ((double)(((short *)txdata_UE0[aa]))[(i<<1)+1]);
		}
	      }


	      // multipath channel
	
	
	
	
	      // UE 0
	      multipath_channel(ch[FuDe],s_re_0,s_im_0,r_re1_0,r_im1_0,
				amps,Td,BW,ricean_factor,aoa,
				lte_frame_parms->nb_antennas_tx,
				lte_frame_parms->nb_antennas_rx,
				FRAME_LENGTH_COMPLEX_SAMPLES,
				channel_length,
				0,1//forgetting factor
				,((first_call1_0 == 1)?1:0),0,FuDe,0);

	      if(first_call1_0 == 1)
		first_call1_0 = 0;

	    
	    
	      //write_output("channel0.m","chan0",ch[0],channel_length,1,8);
	    
	      // scale by path_loss = NOW - P_noise
	      //sigma2_ul       = pow(10,sigma2_dB_ul/10);
	      //N0W          = -95.87;
	      //path_loss_dB = N0W - sigma2_ul;
	      //path_loss    = pow(10,path_loss_dB/10);
	      path_loss_dB = 0;
	      path_loss = 1;
	    
	    
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
		  r_re1_0[aa][i]=r_re1_0[aa][i]*sqrt(path_loss); 
		  r_im1_0[aa][i]=r_im1_0[aa][i]*sqrt(path_loss); 
		}
	      }
	    
	    

   
	    
	      //AWGN

	      sigma2_dB_ul = tx_lev_ul_dB  - SNR_ul;
	      // printf("**********************SNR_ul = %f dB (tx_lev_ul_dB %f, sigma2_dB_ul %f)**************************\n",SNR_ul,(double)tx_lev_ul_dB+10*log10(25/NB_RB),sigma2_dB_ul);
	      sigma2_ul = pow(10,sigma2_dB_ul/10);
	      //printf("sigma2_ul = %g\n",sigma2_ul);
	      for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
	
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i] = (short) ((r_re1_0[aa][i]) + sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0));
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i+1] = (short) ((r_im1_0[aa][i]) + sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0));
		}
	      }
	  
      
      
      
	    


	      for (l=subframe*lte_frame_parms->symbols_per_tti;l<((1+subframe)*lte_frame_parms->symbols_per_tti);l++)
		{
		
		  slot_fep_ul(lte_frame_parms,
			      &PHY_vars_eNB[1]->lte_eNB_common_vars,
			      l%(lte_frame_parms->symbols_per_tti/2),
			      l/(lte_frame_parms->symbols_per_tti/2),
			      0,
			      0);
		}



	      rx_ulsch(&PHY_vars_eNB[1]->lte_eNB_common_vars,
		       PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0],
		       lte_frame_parms,
		       subframe,
		       0,  
		       PHY_vars_eNB[1]->ulsch_eNb[0],
		       1,
		       0);
		
		
		
	      ret_ul = ulsch_decoding(PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0]->llr,
				      lte_frame_parms,
				      PHY_vars_eNB[1]->ulsch_eNb[0],
				      subframe);


	      if (ret_ul == (1+MAX_TURBO_ITERATIONS)) {
		n_errors_ul++;
	      }
	    }// When only relay 0 (UE0) transmits
	  else if ((dlsch_active_1 == 1) && (decode_error_1 == 0))// When only relay 1 (UE1) transmits
	    {
	  

	      //UE1
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {
		  s_re_1[aa][i] = ((double)(((short *)txdata_UE1[aa]))[(i<<1)]);
		  s_im_1[aa][i] = ((double)(((short *)txdata_UE1[aa]))[(i<<1)+1]);
		}
	      }


	      // multipath channel


	      //UE 1
	      multipath_channel(ch[SuDe],s_re_1,s_im_1,r_re1_1,r_im1_1,
				amps,Td,BW,ricean_factor,aoa,
				lte_frame_parms->nb_antennas_tx,
				lte_frame_parms->nb_antennas_rx,
				FRAME_LENGTH_COMPLEX_SAMPLES,
				channel_length,
				0,1//forgetting factor
				,((first_call1_1 == 1)?1:0),0,SuDe,0);

	    
	      if(first_call1_1 == 1)
		first_call1_1 = 0;
	    
	      //write_output("channel0.m","chan0",ch[0],channel_length,1,8);
	    
	      // scale by path_loss = NOW - P_noise
	      //sigma2_ul       = pow(10,sigma2_dB_ul/10);
	      //N0W          = -95.87;
	      //path_loss_dB = N0W - sigma2_ul;
	      //path_loss    = pow(10,path_loss_dB/10);
	      path_loss_dB = 0;
	      path_loss = 1;
	    
	    
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
		  r_re1_1[aa][i]=r_re1_1[aa][i]*sqrt(path_loss); 
		  r_im1_1[aa][i]=r_im1_1[aa][i]*sqrt(path_loss); 
		}
	      }
	    
   
	    
	      //AWGN

	      sigma2_dB_ul = tx_lev_ul_dB - SNR_ul;
	      // printf("**********************SNR_ul = %f dB (tx_lev_ul_dB %f, sigma2_dB_ul %f)**************************\n",SNR_ul,(double)tx_lev_ul_dB+10*log10(25/NB_RB),sigma2_dB_ul);
	      sigma2_ul = pow(10,sigma2_dB_ul/10);
	      //printf("sigma2_ul = %g\n",sigma2_ul);
	      for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i] = (short)(sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0)+ (r_re1_1[aa][i]));
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i+1] = (short) (sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0)+(r_im1_1[aa][i]));
		}
	      }
	  
      
      
      
	    


	      for (l=subframe*lte_frame_parms->symbols_per_tti;l<((1+subframe)*lte_frame_parms->symbols_per_tti);l++)
		{
		
		  slot_fep_ul(lte_frame_parms,
			      &PHY_vars_eNB[1]->lte_eNB_common_vars,
			      l%(lte_frame_parms->symbols_per_tti/2),
			      l/(lte_frame_parms->symbols_per_tti/2),
			      0,
			      0);
		}



	      rx_ulsch(&PHY_vars_eNB[1]->lte_eNB_common_vars,
		       PHY_vars_eNB[1]->lte_eNB_ulsch_vars[1],
		       lte_frame_parms,
		       subframe,
		       0,  
		       PHY_vars_eNB[1]->ulsch_eNb[1],
		       1,
		       0);
		
		
		
	      ret_ul = ulsch_decoding(PHY_vars_eNB[1]->lte_eNB_ulsch_vars[1]->llr,
				      lte_frame_parms,
				      PHY_vars_eNB[1]->ulsch_eNb[1],
				      subframe);


	      if (ret_ul == (1+MAX_TURBO_ITERATIONS)) {
		n_errors_ul++;
	      }
	    }// when only UE1 transmits
#else
	  if((dlsch_active_0 == 1)&& (decode_error_0 == 0))// When only relay 0 (UE0) transmits
	    {


	      //UE0
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_tx;aa++) {
		  s_re_0[aa][i] = ((double)(((short *)txdata_UE0[aa]))[(i<<1)]);
		  s_im_0[aa][i] = ((double)(((short *)txdata_UE0[aa]))[(i<<1)+1]);
		}
	      }


	

	      // multipath channel

  
	      // UE 0
	      multipath_channel(ch[FuDe],s_re_0,s_im_0,r_re1_0,r_im1_0,
				amps,Td,BW,ricean_factor,aoa,
				lte_frame_parms->nb_antennas_tx,
				lte_frame_parms->nb_antennas_rx,
				FRAME_LENGTH_COMPLEX_SAMPLES,
				channel_length,
				0,1//forgetting factor
				,((first_call1_0 == 1)?1:0),0,FuDe,0);

	      if(first_call1_0 == 1)
		first_call1_0 = 0;
	    
	      //write_output("channel0.m","chan0",ch[0],channel_length,1,8);
	    
	      // scale by path_loss = NOW - P_noise
	      //sigma2_ul       = pow(10,sigma2_dB_ul/10);
	      //N0W          = -95.87;
	      //path_loss_dB = N0W - sigma2_ul;
	      //path_loss    = pow(10,path_loss_dB/10);
	      path_loss_dB = 0;
	      path_loss = 1;
	    
	    
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES;i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
		  r_re1_0[aa][i]=r_re1_0[aa][i]*sqrt(path_loss); 
		  r_im1_0[aa][i]=r_im1_0[aa][i]*sqrt(path_loss); 
		}
	      }
	    
	    

   
	    
	      //AWGN

	      sigma2_dB_ul = tx_lev_ul_dB - SNR_ul;
	      // printf("**********************SNR_ul = %f dB (tx_lev_ul_dB %f, sigma2_dB_ul %f)**************************\n",SNR_ul,(double)tx_lev_ul_dB+10*log10(25/NB_RB),sigma2_dB_ul);
	      sigma2_ul = pow(10,sigma2_dB_ul/10);
	      //printf("sigma2_ul = %g\n",sigma2_ul);
	      for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++) {
		for (aa=0;aa<lte_frame_parms->nb_antennas_rx;aa++) {
	
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i] = (short) ((r_re1_0[aa][i]) + sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0));
		  ((short*)PHY_vars_eNB[1]->lte_eNB_common_vars.rxdata[0][aa])[2*i+1] = (short) ((r_im1_0[aa][i]) + sqrt(sigma2_ul/2)*gaussdouble(0.0,1.0));
		}
	      }
	  
      
      
      
	    


	      for (l=subframe*lte_frame_parms->symbols_per_tti;l<((1+subframe)*lte_frame_parms->symbols_per_tti);l++)
		{
		
		  slot_fep_ul(lte_frame_parms,
			      &PHY_vars_eNB[1]->lte_eNB_common_vars,
			      l%(lte_frame_parms->symbols_per_tti/2),
			      l/(lte_frame_parms->symbols_per_tti/2),
			      0,
			      0);
		}



	      rx_ulsch(&PHY_vars_eNB[1]->lte_eNB_common_vars,
		       PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0],
		       lte_frame_parms,
		       subframe,
		       0,  
		       PHY_vars_eNB[1]->ulsch_eNb[0],
		       relay_flag,
		       diversity_scheme);
		
		
		
	      ret_ul = ulsch_decoding(PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0]->llr,
				      lte_frame_parms,
				      PHY_vars_eNB[1]->ulsch_eNb[0],
				      subframe);


	      if (ret_ul == (1+MAX_TURBO_ITERATIONS)) {
		n_errors_ul++;
	      }
	    }// When only relay 0 (UE0) transmits
#endif


	  if(((((double)(n_errors_dl + n_errors_ul)/trials_dl) <= (1-((1-((double)(n_errors_dl)/trials_dl))*(1-(1e-2))))) && (trials_ul >100))|| (n_errors_ul > 100))
	    break;

	  /*
	  if(((((double)(n_errors_ul)/trials_ul)<1e-2)&&(trials_ul>100)) || (n_errors_ul > 100))
	  break;*/


	  }//trials


    //BLER Downlink
    fprintf(bler_fd_dl,"%f,%d,%d,%e;\n",SNR_dl,
	    n_errors_dl,trials_dl,(double)(n_errors_dl)/trials_dl);
	   

    //BLER Uplink
    fprintf(bler_fd_ul,"%f,%d,%d,%e;\n",SNR_ul,
	    n_errors_ul,trials_ul,(double)(n_errors_ul)/trials_ul);
	   

    relay_delay = 0;
    relay_delay = (double)(trials_dl)/(trials_dl + trials_ul);
  
    //BLER and Throughput
    fprintf(bler_fd,"%f,%d,%d,%d,%e,%e,%e;\n",SNR_ul,n_errors_dl,trials_dl,n_errors_ul,relay_delay,(double)(n_errors_dl + n_errors_ul)/trials_dl,(1-((double)(n_errors_dl + n_errors_ul)/trials_dl))*relay_delay*TBS*6*100);
    
		
    if(((double)(n_errors_dl + n_errors_ul)/trials_dl) <= (1-((1-((double)(n_errors_dl)/trials_dl))*(1-(1e-2)))))
      break;


    /*
	if((double)(n_errors_ul)/(trials_ul)<1e-2)
	break;*/

}//SNR_ul
    
    

  fprintf(bler_fd_dl,"];");
  fclose(bler_fd_dl);
  fprintf(bler_fd_ul,"];");
  fclose(bler_fd_ul);  
  fprintf(bler_fd,"];");
  fclose(bler_fd);
  
  
  printf("Freeing dlsch structures\n");
  for(aa=0;j<2;j++)//loop over eNBs
    {
      for(j=0;j<2;j++){
	for (i=0;i<2;i++)
	  {
	    free_eNb_dlsch(PHY_vars_eNB[aa]->dlsch_eNb[j][i]);
	    free_ue_dlsch(PHY_vars_ue[aa]->dlsch_ue[j][i]);
	  }
      }
    }  
    
  
  
  
#ifdef IFFT_FPGA
  printf("Freeing transmit signals\n");
  free(txdataF2_eNB0[0]);
  free(txdataF2_eNB0[1]);
  free(txdataF2_eNB0);
  free(txdata_eNB0[0]);
  free(txdata_eNB0[1]);
  free(txdata_eNB0);
  free(txdataF2_UE0[0]);
  free(txdataF2_UE0[1]);
  free(txdataF2_UE0);
  free(txdata_UE0[0]);
  free(txdata_UE0[1]);
  free(txdata_UE0);
#ifdef COLLABRATIVE_SCHEME
  free(txdataF2_UE1[0]);
  free(txdataF2_UE1[1]);
  free(txdataF2_UE1);
  free(txdata_UE1[0]);
  free(txdata_UE1[1]);
  free(txdata_UE1);
#endif

#endif 
  

  printf("Freeing transmit signals\n");
  for (i=0;i<2;i++) {
    free(s_re[i]);
    free(s_im[i]);
    free(r_re0_0[i]);
    free(r_im0_0[i]);
#ifdef COLLABRATIVE_SCHEME
    free(r_re0_1[i]);
    free(r_im0_1[i]);
#endif
    free(s_re_0[i]);
    free(s_im_0[i]);
    free(r_re1_0[i]);
    free(r_im1_0[i]);
#ifdef COLLABRATIVE_SCHEME
    free(s_re_1[i]);
    free(s_im_1[i]);
    free(r_re1_1[i]);
    free(r_im1_1[i]);
#endif
  }
  free(s_re);
  free(s_im);
  free(r_re0_0);
  free(r_im0_0);
#ifdef COLLABRATIVE_SCHEME
  free(r_re0_1);
  free(r_im0_1);
#endif
  free(s_re_0);
  free(s_im_0);
  free(r_re1_0);
  free(r_im1_0);
#ifdef COLLABRATIVE_SCHEME
  free(s_re_1);
  free(s_im_1);
  free(r_re1_1);
  free(r_im1_1);
#endif
  
  lte_sync_time_free();
  
  printf("Finshed!\n");
  
  return(0);
 
}
