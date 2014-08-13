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
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <execinfo.h>
#include <sys/stat.h>

#include "SIMULATION/TOOLS/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"
#include "SCHED/defs.h"
#include "SCHED/vars.h"
#include "LAYER2/MAC/vars.h"
#include "OCG_vars.h"
#include "SCHED/defs.h"
#include "femtoUtils.h"
#include "UTIL/LOG/log.h"

#include "RadioResourceConfigCommonSIB.h"
#include "RadioResourceConfigDedicated.h"


#ifdef XFORMS
#include "forms.h"
#include "../../USERSPACE_TOOLS/SCOPE/lte_scope.h"
PHY/TOOLS/lte_phy_scope.h
#include "UTIL/LOG/vcd_signal_dumper.h" //TVT:Navid


void do_forms(FD_lte_scope *form, LTE_DL_FRAME_PARMS *frame_parms, short **channel,
	      short **channel_f, short **rx_sig, short **rx_sig_f, short *dlsch_comp, 
	      short* dlsch_comp_i, short* dlsch_rho, short *dlsch_llr, int coded_bits_per_codeword);
#endif



double BW = 7.68;					//TVT: 20MHz BW 7.68
#define N_RB  25		         //TVT: 50 for 10MHz and 25 for 5 MHz 100 for 20MHz
#define UL_RB_ALLOC 0x1ff;
uint32_t DLSCH_RB_ALLOC = 0x1fff;  // TVT: 0x1fff for 5MHz 0x1ffffff for 20MHz
uint32_t DLSCH_RB_ALLOC2[4]; //TVT: RB_ALLOC per round

#define CCCH_RB_ALLOC computeRIV(PHY_vars_eNB->lte_frame_parms.N_RB_UL,0,2)

PHY_VARS_eNB   *PHY_vars_eNB;
PHY_VARS_UE    *PHY_vars_UE;
PHY_VARS_eNB   **interf_PHY_vars_eNB;


channel_desc_t *eNB2UE;
channel_desc_t **interf_eNB2UE;
DCI1_5MHz_TDD_t DLSCH_alloc_pdu2_1; // DCI format 1 for BW MHz
uint64_t DLSCH_alloc_pdu_1;
LTE_DL_FRAME_PARMS *frame_parms; 				  //WARNING if you don't put this variable, some functions don't work

int WRITE_FILES =1;
int NOISE=1;

int x=0;
int totErrors=0;
int totBits=0;

#ifdef XFORMS
FD_lte_scope *form;
char title[255];
#endif
	
int main(int argc,char **argv)
{

  options_t opts;
  data_t data;
  uint16_t NB_RB;
  uint32_t NB_RB2[4];  
  uint8_t num_pdcch_symbols=1;
  
        

  DCI_ALLOC_t dci_alloc[8],dci_alloc_rx[8];

  //Init LOG
  
  logInit();
  //vcd_signal_dumper_init();
  set_comp_log(PHY,LOG_DEBUG,LOG_LOW,1);

  //Parse options
  _initDefaults(&opts);
  _parseOptions(&opts,argc,argv);
  _printOptions(&opts);

  _makeOutputDir(&opts);

  //Init Lte Params

  frame_parms=_lte_param_init(opts);
    
    
    
  /*printf("frame_parms.pdsch_config_common.p_b:%d\n",frame_parms->pdsch_config_common.p_b);    
    printf("PHY_vars_UE->pdsch_config_dedicated.p_a:%d\n",PHY_vars_UE->pdsch_config_dedicated[0].p_a);
    printf("PHY_vars_eNB->pdsch_config_dedicated.p_a:%d\n",PHY_vars_eNB->pdsch_config_dedicated[0].p_a);    
    printf("PHY_vars_UE->cqi_report_config.nomPDSCH_RS_EPRE_Offset:%d\n",PHY_vars_UE->cqi_report_config[0].nomPDSCH_RS_EPRE_Offset);
    printf("PHY_vars_eNB->cqi_report_config.nomPDSCH_RS_EPRE_Offset:%d\n",PHY_vars_eNB->cqi_report_config[0].nomPDSCH_RS_EPRE_Offset);
    
  */
    
  opts.SIZE_TXDATAF=FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;
  opts.SIZE_TXDATA=FRAME_LENGTH_COMPLEX_SAMPLES;	
  opts.SIZE_RXDATAF= (2*(frame_parms->ofdm_symbol_size*opts.nsymb));
  opts.SIZE_RXDATA=FRAME_LENGTH_COMPLEX_SAMPLES;
	
    
  printf("Size txdataF:\t%d \tsymbols x 512 subcarriers = %d OFDBM 	symbols\n",opts.SIZE_TXDATAF/512,opts.SIZE_TXDATAF);
  printf("Size txdata: \t%d \tsamples x frame\n",opts.SIZE_TXDATA);
  printf("Size rxdataF:\t%d \tsymbols x 512 subcarriers = %d OFDM symbols just 2 subframes\n", opts.SIZE_RXDATAF/512, opts.SIZE_RXDATAF);
  printf("Size rxdata: \t%d \tsamples x frame\n",opts.SIZE_RXDATA);
  
  
  //****************************************************
  if (opts.common_flag == 0) { 	 
    switch (opts.N_RB_DL) {
    case 6:
      if (opts.rballocset==0) 
      {DLSCH_RB_ALLOC = 0x3f;}
      BW = 1.25;
      num_pdcch_symbols = 4;
      break;
    case 25:
      if (opts.rballocset==0) {  		 
		  DLSCH_RB_ALLOC2[0]=_allocRBs(&opts,0); 	 
		  DLSCH_RB_ALLOC2[1]= _allocRBs(&opts,1);}
		  //DLSCH_RB_ALLOC2[0] = 0x1f80;		 
		  //DLSCH_RB_ALLOC2[1] = 0x7f;}	  
		  BW = 7.68;
      break;
    case 50:
      if (opts.rballocset==0) 
      {//DLSCH_RB_ALLOC = 0x1ffff;
       DLSCH_RB_ALLOC2[0] = 0x1fe00;		 
	   DLSCH_RB_ALLOC2[1] = 0x1ff;}
      BW = 10.00;
      break;
    case 100:
      if (opts.rballocset==0) 
      {//DLSCH_RB_ALLOC = 0x1ffffff;
       DLSCH_RB_ALLOC2[0] = 0x1ffe000;		 
	   DLSCH_RB_ALLOC2[1] = 0x1fff;}
      BW = 20.00;
      break;
    }  
    DLSCH_RB_ALLOC2[2]=DLSCH_RB_ALLOC2[0];
    DLSCH_RB_ALLOC2[3]=DLSCH_RB_ALLOC2[1];
    
			  NB_RB2[0]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[0],opts.N_RB_DL);
			  NB_RB2[1]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[1],opts.N_RB_DL);
			  NB_RB2[2]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[2],opts.N_RB_DL);
			  NB_RB2[3]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[3],opts.N_RB_DL);	
			//NB_RB=conv_nprb(0,DLSCH_RB_ALLOC,opts.N_RB_DL);
  }
  else 
    NB_RB = 4;
  	  
  	
	 NB_RB2[0]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[0],opts.N_RB_DL);
	 NB_RB2[1]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[1],opts.N_RB_DL);
	 NB_RB2[2]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[2],opts.N_RB_DL);
	 NB_RB2[3]=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC2[3],opts.N_RB_DL);
	  
  NB_RB=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC,opts.N_RB_DL);		
   //****************************************************  
      
#ifdef XFORMS
  fl_initialize (&argc, argv, NULL, 0, 0);
  form = create_form_lte_scope();
  sprintf (title, "LTE DLSIM SCOPE");
  fl_show_form (form->lte_scope, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);
#endif

  _allocData(opts,&data,opts.n_tx,opts.n_rx,FRAME_LENGTH_COMPLEX_SAMPLES);

  _fill_Ul_CCCH_DLSCH_Alloc(opts);

  _generatesRandomChannel(opts);

  _allocDLSChannel(opts); // ??

  _generateDCI(opts,dci_alloc,dci_alloc_rx);


  fprintf(opts.outputFile,"s%d=[",opts.testNumber);
  fprintf(opts.outputBer,"b%d=[",opts.testNumber);
  fprintf(opts.outputTrougput,"t%d=[",opts.testNumber);


  _makeSimulation(data,opts,dci_alloc,dci_alloc_rx,NB_RB2,frame_parms,num_pdcch_symbols);

  _freeMemory(data,opts);

  fprintf(opts.outputFile,"];\n");
  fprintf(opts.outputBer,"];\n");
  fprintf(opts.outputTrougput,"];\n");
  fclose(opts.outputFile);
  fclose(opts.outputBler);
  fclose(opts.outputBer);
  fclose(opts.outputTrougput);

  return 0;
}



void _initDefaults(options_t *opts) {

  opts->snr_init =0;
  opts->snr_max=20;
  opts->snr_step=0.1;
  opts->nframes=1;
  
  opts->nsymb=14;
  opts->frame_type=1;				//1 FDD
  opts->transmission_mode=1;		//
  opts->n_tx=1;
  opts->n_rx=1;
  opts->nInterf=0;
  opts->Nid_cell=0;
  opts->oversampling=1;			
  opts->channel_model=Rayleigh1;
  opts->channel_modeli=Rayleigh1;
  opts->dbInterf=NULL;
  opts->awgn_flag=0;
  opts->awgn_flagi=0;
  opts->common_flag=0;
  opts->TPC=0;
    
  opts->n_rnti=0x1234;			//Ratio Network Temporary Identifiers
  opts->mcs=0;	
  opts->nprb2=25;
  opts->search_prb2=0;				

  opts->extended_prefix_flag=0; 	//false
  opts->nsymb=14;					// Prefix normal
  opts->pilot1 = 4;
  opts->pilot2 = 7;
  opts->pilot3 = 11;

  opts->num_rounds=4;
  opts->fix_rounds=0;
  opts->subframe=7;     
  opts->amp=AMP;					//1024
  opts->dci_flag=0;
  opts->N_RB_DL=25;
  opts->rballocset=0;
  opts->DLSCH_RB_ALLOC = 0x1fff;
  opts->ratio=1;
  opts->prob_flag=0;

  opts->testNumber=0;	
  sprintf(opts->interfLevels," ");
	
  opts->n_adj_cells=0;
  opts->dual_stream_UE = 0;
	
  opts->perfect_ce=0;
  opts->p_b=0;
  opts->p_a=dB0;   					//  To be 0 need 
  opts->d_offset=0;

}

LTE_DL_FRAME_PARMS* _lte_param_init(options_t opts) {
  int i;
  printf("Start lte_param_init\n");

  PHY_vars_eNB = malloc(sizeof(PHY_VARS_eNB));
  PHY_vars_UE = malloc(sizeof(PHY_VARS_UE));
  mac_xface = malloc(sizeof(MAC_xface));
    
  LTE_DL_FRAME_PARMS *lte_frame_parms = &(PHY_vars_eNB->lte_frame_parms);


  lte_frame_parms->N_RB_DL            = opts.N_RB_DL;
  lte_frame_parms->N_RB_UL            = opts.N_RB_DL;
  lte_frame_parms->Ncp                = opts.extended_prefix_flag;
  lte_frame_parms->Nid_cell           = opts.Nid_cell;
  lte_frame_parms->nushift            = (opts.Nid_cell)%6;
  lte_frame_parms->nb_antennas_tx     = opts.n_tx;
  lte_frame_parms->nb_antennas_rx     = opts.n_rx;
  lte_frame_parms->nb_antennas_tx_eNB = opts.n_tx;
  lte_frame_parms->phich_config_common.phich_resource         = oneSixth;  //TODO Why??
  lte_frame_parms->tdd_config = 3;
  lte_frame_parms->frame_type         = opts.frame_type;
  lte_frame_parms->mode1_flag = (opts.transmission_mode == 1)? 1 : 0;
  
  srand(1);
  randominit(0);
  set_taus_seed(0);

  init_frame_parms(lte_frame_parms,opts.oversampling);
  phy_init_top(lte_frame_parms);

  //para que se usan estos ??
  lte_frame_parms->twiddle_fft      = twiddle_fft;		
  lte_frame_parms->twiddle_ifft     = twiddle_ifft;		
  lte_frame_parms->rev              = rev; 				

  PHY_vars_UE->is_secondary_ue = 0;

  PHY_vars_UE->lte_frame_parms = *lte_frame_parms;
  PHY_vars_eNB->lte_frame_parms = *lte_frame_parms;

  phy_init_lte_top(lte_frame_parms);
  dump_frame_parms(lte_frame_parms); //print


  //set number of adjacent cell for channel estimation
  PHY_vars_UE->PHY_measurements.n_adj_cells=opts.n_adj_cells;
  //Init de Cell Id of adjacent cells to estimate
  for(i=1;i<=opts.n_adj_cells;i++)
    {
      PHY_vars_UE->PHY_measurements.adj_cell_id[i-1] = (opts.Nid_cell+i)%6;	  
    }	
	
  for (i=0; i<3; i++)
    lte_gold(lte_frame_parms,PHY_vars_UE->lte_gold_table[i],opts.Nid_cell+i);          
    
  phy_init_lte_ue(PHY_vars_UE,2,0);
  phy_init_lte_eNB(PHY_vars_eNB,0,0,0);

  // Set  p_a and p_b
  PHY_vars_eNB->lte_frame_parms.pdsch_config_common.p_b=opts.p_b;
  PHY_vars_eNB->pdsch_config_dedicated->p_a=opts.p_a;    

  PHY_vars_UE->lte_frame_parms.pdsch_config_common.p_b=opts.p_b;
  PHY_vars_UE->pdsch_config_dedicated->p_a=opts.p_a;
  PHY_vars_UE->n_connected_eNB=2;

	  

  //Init interference nodes
  interf_PHY_vars_eNB=null;
  if (opts.nInterf>0)
    {
      interf_PHY_vars_eNB = (PHY_VARS_eNB **)malloc(opts.nInterf*sizeof(PHY_VARS_eNB *));
      for (i=0; i<opts.nInterf; i++)
        {
	  interf_PHY_vars_eNB[i]=malloc(sizeof(PHY_VARS_eNB));
            
	  memcpy(&interf_PHY_vars_eNB[i]->lte_frame_parms,lte_frame_parms,sizeof(LTE_DL_FRAME_PARMS));            
	  interf_PHY_vars_eNB[i]->lte_frame_parms.Nid_cell=opts.Nid_cell+i+1;
	  interf_PHY_vars_eNB[i]->lte_frame_parms.nushift=(opts.Nid_cell+i+1)%6;
            
	  //printf("NRB: %d\n", interf_PHY_vars_eNB[i]->lte_frame_parms.N_RB_DL);
                      
	  interf_PHY_vars_eNB[i]->Mod_id=i+1;
	  phy_init_lte_eNB(interf_PHY_vars_eNB[i],0,0,0);            		
            
	  interf_PHY_vars_eNB[i]->lte_frame_parms.pdsch_config_common.p_b=opts.p_b;
	  interf_PHY_vars_eNB[i]->pdsch_config_dedicated->p_a=opts.p_a;  

        }
    }
    

  // DL power control init
  /*pdsch_config_dedicated->p_a = opts.p_a; // 4 = 0dB
    pdsch_config_common->p_b = opts.p_b;*/
    
    

  printf("Done lte_param_init\n");
    

  return &PHY_vars_eNB->lte_frame_parms;


}

void _fill_Ul_CCCH_DLSCH_Alloc(options_t opts) {

  PHY_vars_UE->lte_ue_pdcch_vars[0]->crnti = opts.n_rnti;

  UL_alloc_pdu.type    = 0;
  UL_alloc_pdu.hopping = 0;
  UL_alloc_pdu.rballoc = UL_RB_ALLOC;
  UL_alloc_pdu.mcs     = 1;
  UL_alloc_pdu.ndi     = 1;
  UL_alloc_pdu.TPC     = 0;
  UL_alloc_pdu.cqi_req = 1;

  CCCH_alloc_pdu.type               = 0;
  CCCH_alloc_pdu.vrb_type           = 0;
  CCCH_alloc_pdu.rballoc            = CCCH_RB_ALLOC;
  CCCH_alloc_pdu.ndi      = 1;
  CCCH_alloc_pdu.mcs      = 1;
  CCCH_alloc_pdu.harq_pid = 0;  


  DLSCH_alloc_pdu2_1.rah              = 0;
  DLSCH_alloc_pdu2_1.rballoc          = DLSCH_RB_ALLOC;
  DLSCH_alloc_pdu2_1.TPC              = 0;
  DLSCH_alloc_pdu2_1.dai              = 0;
  DLSCH_alloc_pdu2_1.harq_pid         = 0;
  //DLSCH_alloc_pdu2_1E.tb_swap          = 0;
  DLSCH_alloc_pdu2_1.mcs             = opts.mcs;
  DLSCH_alloc_pdu2_1.ndi             = 1;
  DLSCH_alloc_pdu2_1.rv              = 0;
  // Forget second codeword
  //TVT: is this needed? DLSCH_alloc_pdu2_1.tpmi             = (opts.transmission_mode>=5 ? 5 : 0);  // precoding
  //TVT: is this needed? DLSCH_alloc_pdu2_1.dl_power_off     = (opts.transmission_mode==5 ? 0 : 1);


}

void _generatesRandomChannel(options_t opts) {
  int i;
  eNB2UE = new_channel_desc_scm(PHY_vars_eNB->lte_frame_parms.nb_antennas_tx,
				PHY_vars_UE->lte_frame_parms.nb_antennas_rx,
				opts.channel_model,
				BW,0.0,0,0);
  if (eNB2UE==NULL)
    {
      msg("Problem generating channel model. Exiting.\n");
      exit(-1);
    }
  //Channel of interferents 
  interf_eNB2UE=malloc(opts.nInterf*sizeof(channel_desc_t));
    
  for(i=0;i<opts.nInterf;i++)
    {	
		
      interf_eNB2UE[i]=new_channel_desc_scm(PHY_vars_eNB->lte_frame_parms.nb_antennas_tx,
					    PHY_vars_UE->lte_frame_parms.nb_antennas_rx,
					    opts.channel_modeli,BW,0,0,0);
		
      if (interf_eNB2UE[i]==NULL)
	{
	  msg("Problem generating channel model. For interferent %d Exiting.\n",i+1);
	  exit(-1);
	}
    }		 

	    
}


void _allocDLSChannel(options_t opts) {
  int i,j;        
    
  for (i=0; i<2; i++)
    {   
      //eNB     
      PHY_vars_eNB->dlsch_eNB[0][i] = new_eNB_dlsch(1,8,opts.N_RB_DL,0);  
      PHY_vars_eNB->dlsch_eNB[0][i]->dl_power_off =1;  
               
      if (!PHY_vars_eNB->dlsch_eNB[0][i]) {
	printf("Can't get eNB dlsch structures\n");
	exit(-1);
      }        
		
      PHY_vars_eNB->dlsch_eNB[0][i]->rnti = opts.n_rnti;
        
      //computeRhoA_eNB(PHY_vars_eNB->pdsch_config_dedicated,PHY_vars_eNB->dlsch_eNB[0][i]);
      //	computeRhoB_eNB(PHY_vars_eNB->pdsch_config_dedicated,&PHY_vars_eNB->lte_frame_parms.pdsch_config_common,PHY_vars_eNB->lte_frame_parms.nb_antennas_tx,PHY_vars_eNB->dlsch_eNB[0][i]);
    
        
      for(j=0;j<opts.nInterf;j++)
        {
	  interf_PHY_vars_eNB[j]->dlsch_eNB[0][i]=new_eNB_dlsch(1,8,opts.N_RB_DL,0);
	  interf_PHY_vars_eNB[j]->dlsch_eNB[0][i]->dl_power_off =1;  
	  if (!interf_PHY_vars_eNB[j]->dlsch_eNB[0][i]) {
	    printf("Can't get interferer eNB dlsch structures\n");
	    exit(-1);
	  } 
			
	  interf_PHY_vars_eNB[j]->dlsch_eNB[0][i]->rnti = opts.n_rnti;
			
	  //	computeRhoA_eNB(interf_PHY_vars_eNB[j]->pdsch_config_dedicated,interf_PHY_vars_eNB[j]->dlsch_eNB[0][i]);
	  //	computeRhoB_eNB(interf_PHY_vars_eNB[j]->pdsch_config_dedicated,&interf_PHY_vars_eNB[j]->lte_frame_parms.pdsch_config_common,interf_PHY_vars_eNB[j]->lte_frame_parms.nb_antennas_tx,interf_PHY_vars_eNB[j]->dlsch_eNB[0][i]);
	}
                
      //UE
      PHY_vars_UE->dlsch_ue[0][i]  = new_ue_dlsch(1,8,MAX_TURBO_ITERATIONS,opts.N_RB_DL,0);
     // PHY_vars_UE->dlsch_ue[0][i]  = new_ue_dlsch(1,8,MAX_TURBO_ITERATIONS,0);
      if (!PHY_vars_UE->dlsch_ue[0][i]) {
	printf("Can't get ue dlsch structures\n");
	exit(-1);
      }
      PHY_vars_UE->dlsch_ue[0][i]->rnti   = opts.n_rnti; 
        
      //  computeRhoA_UE(PHY_vars_UE->pdsch_config_dedicated,PHY_vars_UE->dlsch_ue[0][i]);
      //	computeRhoB_UE(PHY_vars_UE->pdsch_config_dedicated,&PHY_vars_UE->lte_frame_parms.pdsch_config_common,PHY_vars_UE->lte_frame_parms.nb_antennas_tx,PHY_vars_UE->dlsch_ue[0][i]);
        
    }

    //TVT: this is for DCI format 1B,D and 2 ----------------------
  //if (DLSCH_alloc_pdu2_1.tpmi == 5)
    //PHY_vars_eNB->eNB_UE_stats[0].DL_pmi_single = (unsigned short)(taus()&0xffff);//DL PMI Single Stream.  (precoding matrix indicator)
  //else
    //PHY_vars_eNB->eNB_UE_stats[0].DL_pmi_single = 0;

  //	compute_sqrt_RhoAoRhoB( PHY_vars_eNB->pdsch_config_dedicated,
  //                  &PHY_vars_eNB->lte_frame_parms.pdsch_config_common,opts.n_tx,PHY_vars_UE->dlsch_ue[0][0]);
//---------------------------------
}

void _generateDCI(options_t opts,DCI_ALLOC_t *dci_alloc,DCI_ALLOC_t *dci_alloc_rx)
{
  int  num_dci = 0,i;
  int dci_length_bytes=0,dci_length=0;
                  
  // UE specific DCI
//*******************************************************


	if (opts.common_flag == 0) {
	  
	  if (PHY_vars_eNB->lte_frame_parms.frame_type == TDD) {
	    
	    switch (PHY_vars_eNB->lte_frame_parms.N_RB_DL) {
	    case 6:
	      dci_length = sizeof_DCI1_1_5MHz_TDD_t;
	      dci_length_bytes = sizeof(DCI1_1_5MHz_TDD_t);
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->dai              = 0;
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      break;
	    case 25:
	      dci_length = sizeof_DCI1_5MHz_TDD_t;
	      dci_length_bytes = sizeof(DCI1_5MHz_TDD_t);
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->dai              = 0;
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      break;
	    case 50:
	      dci_length = sizeof_DCI1_10MHz_TDD_t;
	      dci_length_bytes = sizeof(DCI1_10MHz_TDD_t);
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->dai              = 0;
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      break;
	    case 100:
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->dai              = 0;
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      dci_length = sizeof_DCI1_20MHz_TDD_t;
	      dci_length_bytes = sizeof(DCI1_20MHz_TDD_t);
	      break;
	    }
	  }
	  else {
	    switch (PHY_vars_eNB->lte_frame_parms.N_RB_DL) {
	    case 6:
	      dci_length = sizeof_DCI1_1_5MHz_FDD_t;
	      dci_length_bytes = sizeof(DCI1_1_5MHz_FDD_t);
	      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      break;
	    case 25:
	      dci_length = sizeof_DCI1_5MHz_FDD_t;
	      dci_length_bytes = sizeof(DCI1_5MHz_FDD_t);
	      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      break;
	    case 50:
	      dci_length = sizeof_DCI1_10MHz_FDD_t;
	      dci_length_bytes = sizeof(DCI1_10MHz_FDD_t);
	      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      break;
	    case 100:
	      dci_length = sizeof_DCI1_20MHz_FDD_t;
	      dci_length_bytes = sizeof(DCI1_20MHz_FDD_t);
	      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rah              = 0;
	      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rballoc          = DLSCH_RB_ALLOC;
	      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->TPC              = 0;
	      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->harq_pid         = 0;
	      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->mcs             = opts.mcs;  
	      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
	      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
	      break;
	    }	  
	  }

//*******************************************************
  memcpy(&dci_alloc[num_dci].dci_pdu[0],&DLSCH_alloc_pdu_1,dci_length_bytes);
  dci_alloc[num_dci].dci_length = dci_length;
  dci_alloc[num_dci].L          = 2;
  dci_alloc[num_dci].rnti       = opts.n_rnti;
  dci_alloc[num_dci].nCCE       = 0;
  dci_alloc[num_dci].format     = format1;//TVT: E_2A_M10PRB; for format 1 instead of 1E
  }
        
generate_eNB_dlsch_params_from_dci(0,						
				     &DLSCH_alloc_pdu_1,	
				     opts.n_rnti,
				     format1,//TVT:E_2A_M10PRB,		
				     PHY_vars_eNB->dlsch_eNB[0],
				     &PHY_vars_eNB->lte_frame_parms, PHY_vars_eNB->pdsch_config_dedicated,  
				     SI_RNTI,
				     0,
				     P_RNTI,
				     PHY_vars_eNB->eNB_UE_stats[0].DL_pmi_single);
				     
  for(i=0;i<opts.nInterf;i++)
    {
      generate_eNB_dlsch_params_from_dci(0,						
					 &DLSCH_alloc_pdu_1,	
					 opts.n_rnti,
					 format1,//TVT: E_2A_M10PRB,		
					 interf_PHY_vars_eNB[i]->dlsch_eNB[0],
					 &(interf_PHY_vars_eNB[i])->lte_frame_parms,   PHY_vars_eNB->pdsch_config_dedicated,
					 SI_RNTI,
					 0,
					 P_RNTI,
					 interf_PHY_vars_eNB[i]->eNB_UE_stats[0].DL_pmi_single);
		
    }
    


}

void _freeMemory(data_t data,options_t opts)
{
  int i;
  printf("Freeing channel I/O\n");
  for (i=0; i<opts.n_tx; i++) {
    free(data.s_re[i]);
    free(data.s_im[i]);
    free(data.r_re[i]);
    free(data.r_im[i]);
  }
  free(data.s_re);
  free(data.s_im);
  free(data.r_re);
  free(data.r_im);

  printf("Freeing dlsch structures\n");
  for (i=0; i<2; i++) {
    printf("eNB %d\n",i);
    free_eNB_dlsch(PHY_vars_eNB->dlsch_eNB[0][i]);
    printf("UE %d\n",i);
    free_ue_dlsch(PHY_vars_UE->dlsch_ue[0][i]);
  }



}


void _printResults(uint32_t *errs,uint32_t *round_trials,uint32_t dci_errors,double rate)
{
  printf("\tErrors/trials (%d/%d, %d/%d ,%d/%d ,%d/%d) Pe = (%e,%e,%e,%e) \n\tdci_errors %d/%d, Pe = %e  \n\teffective rate \t%f (%f) \n\tnormalized delay\t %f (%f)\n",
	 errs[0],
	 round_trials[0],
	 errs[1],
	 round_trials[1],
	 errs[2],
	 round_trials[2],
	 errs[3],
	 round_trials[3],
	 (double)errs[0]/(round_trials[0]),
	 (double)errs[1]/(round_trials[0]+round_trials[1]),
	 (double)errs[2]/(round_trials[0]+round_trials[1]+round_trials[2]),
	 (double)errs[3]/(round_trials[0]+round_trials[1]+round_trials[2]+round_trials[3]),
	 dci_errors,
	 round_trials[0],
	 (double)dci_errors/(round_trials[0]),
	 rate*((double)(round_trials[0]-dci_errors)/((double)round_trials[0] + round_trials[1] + round_trials[2] + round_trials[3])),
	 rate,
	 (1.0*(round_trials[0]-errs[0])+2.0*(round_trials[1]-errs[1])+3.0*(round_trials[2]-errs[2])+4.0*(round_trials[3]-errs[3]))/((double)round_trials[0])/(double)PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->TBS,
	 (1.0*(round_trials[0]-errs[0])+2.0*(round_trials[1]-errs[1])+3.0*(round_trials[2]-errs[2])+4.0*(round_trials[3]-errs[3]))/((double)round_trials[0]));

}

void _printFileResults(double SNR,double rate1, double rate2, double rate,uint32_t  *errs,uint32_t  *round_trials,uint32_t dci_errors,options_t opts,double BER)
{
  double pout1=0.0,pout2=0.0,spec_eff;
  fprintf(opts.outputFile,"%f %f;\n", SNR, (float)errs[0]/round_trials[0]);
  pout1=(double)errs[0]/(round_trials[0]);
  pout2=(double)errs[1]/(round_trials[1]);
  spec_eff=((1-pout1)*rate1)+(pout1*(1-pout2)*rate2);
  fprintf(opts.outputBler,"%f;%f;%f;%d;%d;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f\n",
	  SNR,
	  rate1,
	  rate2,
	  opts.mcs,
	  PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->TBS,
	  rate,
	  errs[0],
	  round_trials[0],
	  errs[1],
	  round_trials[1],
	  errs[2],
	  round_trials[2],
	  errs[3],
	  round_trials[3],
	  dci_errors,
	  opts.nprb1,
	  opts.nprb2,
	  spec_eff);
            
  fprintf(opts.outputBer,"%f %f;\n",SNR, BER);
		
}

void _initErrsRoundsTrials(uint32_t **errs,uint32_t **trials,int allocFlag,options_t opts)
{

  int i=0;
  if (allocFlag==1)
    {
      *errs=(uint32_t*)malloc(4*sizeof(uint32_t));
      *trials=(uint32_t*)malloc(4*sizeof(uint32_t));
    }

  for (i=0; i<4; i++)
    {

      (*errs)[i]=0;
      (*trials)[i]=0;
    }
}

void _fillData(options_t opts,data_t data,int numSubFrames)
{
  uint32_t aux=2*opts.subframe*PHY_vars_UE->lte_frame_parms.samples_per_tti;
  int i,aa,j;        		
  //Copy numframes
  for (i=0; i<numSubFrames*frame_parms->samples_per_tti; i++) // Size of one subframe * numframes
    {
      for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx; aa++)
        {			
	  data.s_re[aa][i] = ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[0][aa]))[aux + (i<<1)]);
	  data.s_im[aa][i] = ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[0][aa]))[aux +(i<<1)+1]);
	  for(j=0;j<opts.nInterf;j++)
            {
	      data.is_re[j][aa][i] = ((double)(((short *)interf_PHY_vars_eNB[j]->lte_eNB_common_vars.txdata[0][aa]))[aux + (i<<1)]);
	      data.is_im[j][aa][i] = ((double)(((short *)interf_PHY_vars_eNB[j]->lte_eNB_common_vars.txdata[0][aa]))[aux +(i<<1)+1]);																	
	    }
        }
    }

}
void _applyInterference(options_t *opts,data_t data,double sigma2,double iqim,int numSubFrames,int round)
{	
  int i,aa,j,Intf[opts->nInterf];
  if(opts->nInterf<=0)
    return;
    for(j=0;j<opts->nInterf;j++)
	    {	
				if(opts->probabilityInterf[j]>((double)rand() / (double)RAND_MAX) ) {				
				if(j==0){
					opts->interf_count[round]++;
					//printf("round: %d, counter:%d \n",round,opts->interf_count[round]);
				}
				Intf[j]=1;}
				else
					Intf[j]=0;
		}
				
  for (i=0; i<numSubFrames*frame_parms->samples_per_tti; i++)
    {
      for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_rx; aa++) 
        {
	  for(j=0;j<opts->nInterf;j++)
	    {		
	//prob_flag=1 means that interference is active with a probability of opts->probabilityInterf[i]
		  if(opts->prob_flag){	
			  //printf("\n interf probability: %f",opts.probabilityInterf[j]);
		  		if(Intf[j]==1) {					

					data.r_re[aa][i] += (pow(10.0,.05*opts->dbInterf[j])*data.ir_re[j][aa][i]);				
					data.r_im[aa][i] += (pow(10.0,.05*opts->dbInterf[j])*data.ir_im[j][aa][i]);}
		    }
		  else{
			  //printf("caso anterior \n");
				data.r_re[aa][i] += (pow(10.0,.05*opts->dbInterf[j])*data.ir_re[j][aa][i]);				
				data.r_im[aa][i] += (pow(10.0,.05*opts->dbInterf[j])*data.ir_im[j][aa][i]);
			  }  
			  //printf("no aplica interf \n");	
	    }
        }
    }
}

void _applyNoise(options_t *opts, data_t data,double sigma2,double iqim,int numSubFrames)
{
  uint32_t aux=2*opts->subframe*PHY_vars_UE->lte_frame_parms.samples_per_tti;
  // printf("\naux:%d\n",aux);
  int i,aa;
  for (i=0; i<numSubFrames*frame_parms->samples_per_tti; i++)
    {
      for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_rx; aa++) {

	if( NOISE)
	  {
	    ((short*) PHY_vars_UE->lte_ue_common_vars.rxdata[aa])[(aux)+2*i]   = (short) (data.r_re[aa][i] + sqrt(sigma2/2)*gaussdouble(0.0,1.0));
	    ((short*) PHY_vars_UE->lte_ue_common_vars.rxdata[aa])[(aux)+2*i+1] = (short) (data.r_im[aa][i] + (iqim*data.r_re[aa][i]) + sqrt(sigma2/2)*gaussdouble(0.0,1.0));
	  }else
	  {				    
	    ((short*) PHY_vars_UE->lte_ue_common_vars.rxdata[aa])[(aux)+2*i]   = (short) (data.r_re[aa][i]);
	    ((short*) PHY_vars_UE->lte_ue_common_vars.rxdata[aa])[(aux)+2*i+1] = (short) (data.r_im[aa][i]) ;
	  }
      }
    }

}

uint8_t _generate_dci_top(int num_ue_spec_dci,int num_common_dci,DCI_ALLOC_t *dci_alloc,options_t opts,uint8_t num_pdcch_symbols)
{

  uint8_t num_pdcch_symbols_2=0,aux=0;
  int i;
  //TODO: We apply rho_b to DCI information because we part that in this simulation we 
  //use just the first symbol, but  in other simulation that use more than one, it's necesary
  //change the function to apply rho_b or rho_a in the correct symbol
    
  //This routine codes an set of DCI PDUs and performs PDCCH modulation, interleaving and mapping. 
  num_pdcch_symbols_2= generate_dci_top(num_ue_spec_dci,
					num_common_dci,
					dci_alloc,
					0,
					1024,//(int16_t)(((int32_t)AMP*PHY_vars_eNB->dlsch_eNB[0][0]->sqrt_rho_b)>>13),
					&PHY_vars_eNB->lte_frame_parms,
					PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell],
					opts.subframe);
//
//printf("num_pdcch_symbols %d , num_pdcch_symbols_2 %d=> ",num_pdcch_symbols,num_pdcch_symbols_2);

  if (num_pdcch_symbols_2 > num_pdcch_symbols) {
    msg("Error: given num_pdcch_symbols not big enough\n");
    exit(-1);
  }
    
  for(i=0;i<opts.nInterf;i++)
    {
      aux=generate_dci_top(num_ue_spec_dci,
			   num_common_dci,
			   dci_alloc,
			   0,
			   0,//(int16_t)(((int32_t)AMP*PHY_vars_eNB->dlsch_eNB[0][0]->sqrt_rho_b)>>13),
			   &PHY_vars_eNB->lte_frame_parms,
			   interf_PHY_vars_eNB[i]->lte_eNB_common_vars.txdataF[0],
			   opts.subframe);
      if (aux > num_pdcch_symbols) {
	msg("Error: given num_pdcch_symbols not big enough ...interferer %d\n",i);
	exit(-1);
      }   
    }
    
    
  return num_pdcch_symbols_2;
}

uint32_t _allocRBs(options_t *opts,int ind)
{
	static uint32_t allocRB;
	static uint32_t allocRBs[25]={1,2,3,6,7,14,15,30,31,62,63,126,127,254,255,510,511,1022,1023,2046,2047,4094,4095,8190,8191};
	switch (opts->N_RB_DL) {
    case 6:   
		break;
    case 25:  //search_prb2 is a flag that means nprb2 will be optimized
    if (opts->search_prb2){		
		switch(ind)
		{
		case 0:
		case 2:
		case 4:
		case 6:
		allocRB=allocRBs[opts->nprb1-1]; 	
		//printf("nprb1: %d, mcs: %d, allocRB: %X\n",opts->nprb1,opts->mcs,allocRB);					
		break;
		case 1:
		case 3:
		case 5:
		case 7:			
		allocRB=allocRBs[opts->nprb2-1];
		opts->mcs2=29;
		//printf("nprb2: %d, mcs: %d, allocRB: %X\n",opts->nprb2,opts->mcs,allocRB);	
		break;
		}
	}
	else{
	    switch (opts->ratio){
	    case 1: // # of dimensions per round: 13/12
	    if (ind==0)	{	   
			allocRB=0x7f; 
			opts->mcs=4;}
			else{
			allocRB=0x1f80;
			opts->mcs2=opts->mcs;}
	    break;
	    case 2: // 12/13
	    if (ind==0)	{
			allocRB=0x1f80;
			opts->mcs=5;}
			else{
			allocRB=0x7f;
			opts->mcs2=opts->mcs;}
	    break;
	    case 3: // 10/15
	    if (ind==0)	{
			allocRB=0x1f00;
			opts->mcs=6;}
			else{
			allocRB=0xff;
			opts->mcs2=opts->mcs;}
	    break;
	    case 4: // 8/17
	     if (ind==0)	{
			allocRB=0x1e00;
			opts->mcs=7;}
			else{
			allocRB=0x1ff;
			opts->mcs2=opts->mcs;}
	    break;
	    case 5: // 6/19
	     if (ind==0)	{
			allocRB=0x1c00;
			opts->mcs=11;}
			else{
			allocRB=0x3ff;
			opts->mcs2=opts->mcs;}
	    break;
	    case 6: // 4/21
	     if (ind==0)	{
			allocRB=0x1800;
			opts->mcs=14;}
			else{
			allocRB=0x7ff;
			opts->mcs2=opts->mcs;}
	    break;
	    case 7: // 2/23
			if (ind==0)	{
			allocRB=0x1000;
			opts->mcs=23;}
			else{
			allocRB=0xfff;
			opts->mcs2=opts->mcs;}
		break;
		case 8: // 15/10
			if (ind==0)	{
			allocRB=0xff;
			opts->mcs=4;}
			else{
			allocRB=0x1f00;
			opts->mcs2=opts->mcs;}
		break;
		case 9: // 17/8
			if (ind==0)	{
			allocRB=0x1ff;
			opts->mcs=3;}
			else{
			allocRB=0x1e00;
			opts->mcs2=opts->mcs;}
		break;
		case 91: // 2/23 force the 2nd round with QPSK and llrclear
			if (ind==0)	{
			allocRB=0x1000;
			opts->mcs=23;}
			else{
			allocRB=0xfff;
			opts->mcs2=29;}
		break;
		case 92: // 6/19 force the 2nd round with QPSK
			if (ind==0)	{
			allocRB=0x1c00;
			opts->mcs=11;}
			else{
			allocRB=0x3ff;
			opts->mcs2=29;}
		break;
		case 93: // 4/21 force the 2nd round with QPSK
			if (ind==0)	{
			allocRB=0x1800;
			opts->mcs=14;}
			else{
			allocRB=0x7ff;
			opts->mcs2=29;}
		break;
		case 94: // 2/23 force the 2nd round with 16QAM
			if (ind==0)	{
			allocRB=0x1000;
			opts->mcs=23;}
			else{
			allocRB=0xfff;
			opts->mcs2=30;}
		break;
		case 10: // 19/6
			if (ind==0)	{
			allocRB=0x3ff;
			opts->mcs=3;}
			else{
			allocRB=0x1c00;
			opts->mcs2=opts->mcs;}
		break;
		case 11: // 21/4
			if (ind==0)	{
			allocRB=0x7ff;
			opts->mcs=2;}
			else{
			allocRB=0x1800;
			opts->mcs2=opts->mcs;}
		break;
		case 12: // 5/25
			if (ind==0)	{
			allocRB=0x1801;
			opts->mcs=12;}
			else{
			allocRB=0x7fe;
			opts->mcs2=opts->mcs;}
		break;
		default:
			allocRB = 0x1fff;
			opts->mcs=0;
		break;}
		}
		break;
    case 50:
		break;
    case 100:    
		break;
    }  
  return allocRB;
}

void _get_nprb1(options_t *opts)
{
	static uint32_t nprb1[28]={25,23,18,14,12,10,8,7,6,6,6,5,4,4,4,3,3,3,3,3,3,2,2,2,2,2,2,1};
	opts->nprb1=nprb1[opts->mcs-1];	
}

void _makeSimulation(data_t data,options_t opts,DCI_ALLOC_t *dci_alloc,DCI_ALLOC_t *dci_alloc_rx,uint32_t *NB_RB2,LTE_DL_FRAME_PARMS  *frame_parms,uint8_t num_pdcch_symbols)
{ 
  uint32_t  *errs,*round_trials;
  unsigned char *input_buffer;
  unsigned char **interferer_input_buffer=null;    
  unsigned short input_buffer_length;
  double raw_ber;
  double rawberT;
  int numresults;
	

	
  //Index and counters
  int aa;				//Antennas index
  int i,j;//ind,mcsi[2]={3,4}; 			//General index for arrays
  uint32_t round;
  double SNR;
  uint32_t dci_errors=0;
  uint32_t cont_frames=0;
  uint8_t Ns,l,m;


  //Variables
  uint32_t tbs,tbs1,coded_bits_per_codeword,coded_bits_per_codeword1;
  int num_common_dci=0,num_ue_spec_dci=1,aux;
  double rate=0,rate1=0,rate2=0.0, sigma2, sigma2_dB=10,uncoded_ber,avg_ber;
  short *uncoded_ber_bit;
  unsigned int dci_cnt,dlsch_active=0;
  unsigned int tx_lev,tx_lev_dB=0,*itx_lev=null,*itxlev_dB=null; // Signal Power



  //Other defaults values
    
  uint8_t i_mod = 2,i_mod1=2;
  //uint8_t num_pdcch_symbols=1,num_pdcch_symbols_2=0;
  uint8_t num_pdcch_symbols_2=0;
  int eNB_id_i = 1;//Id Interferer;
  int idUser=0;   //index of  number of user, this program use just one user allowed in position 0 of  PHY_vars_eNB->dlsch_eNB
  //Just allow transmision mode 1
  double numOFDMSymbSubcarrier;
  

  //Status flags
  int32_t status;
  uint32_t ret;
  int re_allocated;

  //Init Pointers to 8 HARQ processes for the DLSCH
  //printf("PHY_vars_eNB->dlsch_eNB[idUser][0]->harq_processes[0]->TBS: %d\n",(PHY_vars_eNB->dlsch_eNB[idUser][0]->harq_processes[0]->TBS));      
  input_buffer_length = PHY_vars_eNB->dlsch_eNB[idUser][0]->harq_processes[0]->TBS/8; 
  input_buffer = (unsigned char *)malloc(input_buffer_length+4);
  memset(input_buffer,0,input_buffer_length+4);
 
  for (i=0; i<input_buffer_length; i++) {      
    input_buffer[i]= (unsigned char)(unsigned char)(taus()&0xff);
  }
    
  if(opts.nInterf>0)
    {
      interferer_input_buffer=(unsigned char**)malloc(opts.nInterf);
      itx_lev=(unsigned int*)malloc(opts.nInterf*sizeof(unsigned int));
      itxlev_dB=(unsigned int*)malloc(opts.nInterf*sizeof(unsigned  int));
		
      for(j=0;j<opts.nInterf;j++)
	{
	  interferer_input_buffer[j]=(unsigned char *)malloc(input_buffer_length+4);
	  memset(interferer_input_buffer[j],0,input_buffer_length+4);					
	}
    }
    

  /*********************************************************************************/
/* TVT: this has to be done per round since NB_RB2 will be different
  numOFDMSymbSubcarrier=PHY_vars_UE->lte_frame_parms.ofdm_symbol_size/(NB_RB2[0]*12);
  printf("numOFDMSymbSubcarrier: %d\n",numOFDMSymbSubcarrier);*/ 

  _initErrsRoundsTrials(&errs,&round_trials,1, opts);
  
  //for (ind=1; ind<3; ind++)	
  //{
	 // opts.mcs=mcsi[ind-1];
	  _get_nprb1(&opts);
	  opts.nprb2=PHY_vars_eNB->lte_frame_parms.N_RB_DL;
  for (SNR=opts.snr_init; SNR<=opts.snr_max; SNR+=opts.snr_step)
    {
	  //opts.nprb2=PHY_vars_eNB->lte_frame_parms.N_RB_DL;
	  	
	  while(opts.nprb2>0){ 
	  printf("\n\nsnr: %f, nprb1: %d, nprb2: %d, mcs: %d\n",SNR,opts.nprb1,opts.nprb2,opts.mcs);
      _initErrsRoundsTrials(&errs,&round_trials,0,opts);

      dci_errors=0;
      numresults=0;
      raw_ber=0;
      rawberT=0;
      x=0;
      totBits=0;
      totErrors=0;
      avg_ber = 0;
      for(aux=0;aux<8;aux++){
      opts.interf_count[aux]=0;
		}
      
      
       for (cont_frames = 0; cont_frames<opts.nframes; cont_frames++)
        {
	  round=0;
	  eNB2UE->first_run = 1;

	  while (round < opts.num_rounds)
            {			
				
		//printf("interf_counter:%d\n",opts.interf_count[0]);
	      round_trials[round]++;
	      tx_lev = 0;
	      for(i=0;i<opts.nInterf;i++)
                {
		  itx_lev[i]=0;
		  itxlev_dB[i]=0;
		}
	      //Clear the the transmit data in the frequency domain for principal eNB and interferer eNB               				
	      for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx; aa++) {					
		memset(&PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell][aa][0],0,FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX*sizeof(mod_sym_t));                    				
		for(i=0;i<opts.nInterf;i++)
		  {							
		    memset(interf_PHY_vars_eNB[i]->lte_eNB_common_vars.txdataF[0][aa],0,FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX*sizeof(mod_sym_t));						
		  }                                            
	      }                
	      //Init input buffer for interferer 
	      for(j=0;j<opts.nInterf;j++)
		{				
		  memset(interferer_input_buffer[j],0,input_buffer_length+4);
		  for (i=0; i<input_buffer_length; i++) {      
		    interferer_input_buffer[j][i]= (unsigned char)(taus()&0xff);				
		  }
					
		}
                
	      /*Lid: Simulate HARQ procedures!!!
	      if (round == 0) // First round, set Ndi to 1 and rv to floor(round/2)
                {
		  PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 1;
		  //Lid:PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round>>1;
		  PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round&3;
		  DLSCH_alloc_pdu2_1.ndi             = 1;			//New Data Indicator 1. 
		  DLSCH_alloc_pdu2_1.rv              = 0;			//Redundancy version 0. 
		  memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu2_1,sizeof(DCI1_20MHz_TDD_t));
                }
	      else 	// set Ndi to 0
               {
		 PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 0;
		 //Lid: PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round>>1;
		 PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round&3;
		  DLSCH_alloc_pdu2_1.ndi             = 0;				//New Data Indicator 0. 
		  //Lid:DLSCH_alloc_pdu2_1E.rv              = round>>1;			//Redundancy version 1. 
		  DLSCH_alloc_pdu2_1.rv              = round&3;
		  memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu2_1,sizeof(DCI1_20MHz_TDD_t));
                }*/
//*******************************************************
// TVT: This is the new stuff to change the BW and N_RB_DL
// Simulate HARQ procedures!!!
	    if (opts.common_flag == 0) {
	      
	      if (round == 0) {   // First round, set Ndi to 1 and rv to floor(round
		PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 1;
		PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round&3;		
		if (PHY_vars_eNB->lte_frame_parms.frame_type == TDD) {
		  
		  switch (opts.transmission_mode) {
		  case 1:
		  case 2:
		    switch (PHY_vars_eNB->lte_frame_parms.N_RB_DL) {
		    case 6:
		      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_1_5MHz_TDD_t));
		      break;
		    case 25:
		      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
		    //  ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc         = DLSCH_RB_ALLOC2[0];
		       ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc        = _allocRBs(&opts,0);
		      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->mcs			   = opts.mcs;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_5MHz_TDD_t));		      
		      break;
		    case 50:
		      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
		      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc         = DLSCH_RB_ALLOC2[0];
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_10MHz_TDD_t));
		      break;
		    case 100:
		      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
		      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc         = DLSCH_RB_ALLOC2[0];
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_20MHz_TDD_t));
		      break;
		    }
		    break;		
		  }
		   PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->first_Qm = get_Qm(opts.mcs);
		 
		}
		else { // FDD TVT:not our case
		  switch (opts.transmission_mode) {
		  case 1:
		  case 2:
		    switch (PHY_vars_eNB->lte_frame_parms.N_RB_DL) {
		    case 6:
		      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_1_5MHz_FDD_t));
		      break;
		    case 25:
		      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;		     
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_5MHz_FDD_t));
		      break;
		    case 50:
		      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_10MHz_FDD_t));
		      break;
		    case 100:
		      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 1;
		      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = 0;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_20MHz_FDD_t));
		      break;
		    }
		    break;
		  }
		  
		}
		
	      }
	      else { // set Ndi to 0 round>0
		PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 0;
		PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round&3;
		
		if (PHY_vars_eNB->lte_frame_parms.frame_type == TDD) {
		  
		  
		  switch (opts.transmission_mode) {
		  case 1:
		  case 2:
		    switch (PHY_vars_eNB->lte_frame_parms.N_RB_DL) {
		    case 6:
		      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_1_5MHz_TDD_t));
		      break;
		    case 25:
		      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;
		      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc        = _allocRBs(&opts,round);
		      ((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->mcs			   = opts.mcs2;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_5MHz_TDD_t));
		      //printf("round: %d\n",round);
		      break;
		    case 50:
		      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;
		      ((DCI1_10MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc			= DLSCH_RB_ALLOC2[round];
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_10MHz_TDD_t));
		      break;
		    case 100:
		      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;
		      ((DCI1_20MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc			= DLSCH_RB_ALLOC2[round];
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_20MHz_TDD_t));
		      break;
		    }
		    break;
		  }
		}
		else { //FDD TVT:not our case
		  switch (opts.transmission_mode) {
		  case 1:
		  case 2:
		    switch (PHY_vars_eNB->lte_frame_parms.N_RB_DL) {
		    case 6:
		      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_1_5MHz_FDD_t));
		      break;
		    case 25:
		      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_5MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_5MHz_FDD_t));
		      break;
		    case 50:
		      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_10MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_10MHz_FDD_t));
		      break;
		    case 100:
		      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->ndi             = 0;
		      ((DCI1_20MHz_FDD_t *)&DLSCH_alloc_pdu_1)->rv              = round&3;
		      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu_1,sizeof(DCI1_20MHz_FDD_t));
		      break;
		    }
		    break;
		  }
		}
	      }
	    }
	  
        //TVT: since we changed the dci_rballoc, we have to call this function again.
		generate_eNB_dlsch_params_from_dci(0,						
				     &DLSCH_alloc_pdu_1,	
				     opts.n_rnti,
				     format1,
				     PHY_vars_eNB->dlsch_eNB[0],
				     &PHY_vars_eNB->lte_frame_parms, PHY_vars_eNB->pdsch_config_dedicated,  
				     SI_RNTI,
				     0,
				     P_RNTI,
				     PHY_vars_eNB->eNB_UE_stats[0].DL_pmi_single);     
				  
//*******************************************************
//printf("PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->TBS: %d \n",PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->TBS);
	      num_pdcch_symbols_2 = _generate_dci_top(num_ue_spec_dci,num_common_dci,dci_alloc,opts,num_pdcch_symbols);

	      _writeTxData("1","dci", 0, 2,opts,0,0);
                
	      /*****Sending******/ //TVT:force it to use QPSK in the 2nd round
		 if (round==0){
	      i_mod=get_Qm(opts.mcs); 
	      i_mod1=i_mod;
	      coded_bits_per_codeword1 = get_G(&PHY_vars_eNB->lte_frame_parms,
					      PHY_vars_eNB->dlsch_eNB[idUser][0]->nb_rb,
					      PHY_vars_eNB->dlsch_eNB[idUser][0]->rb_alloc,
					      i_mod1,
					      num_pdcch_symbols,0,
					      opts.subframe);
					      tbs1 = (double)dlsch_tbs25[get_I_TBS(PHY_vars_eNB->dlsch_eNB[idUser][0]->harq_processes[0]->mcs)][PHY_vars_eNB->dlsch_eNB[idUser][0]->nb_rb-1];

	      }//Compute Q (modulation order) based on I_MCS.
		 else{
		  i_mod=get_Qm(opts.mcs2); 
		  }
		  
	      coded_bits_per_codeword = get_G(&PHY_vars_eNB->lte_frame_parms,
					      PHY_vars_eNB->dlsch_eNB[idUser][0]->nb_rb,
					      PHY_vars_eNB->dlsch_eNB[idUser][0]->rb_alloc,
					      i_mod,
					      num_pdcch_symbols,0,
					      opts.subframe);

                
	      tbs = (double)dlsch_tbs25[get_I_TBS(PHY_vars_eNB->dlsch_eNB[idUser][0]->harq_processes[0]->mcs)][PHY_vars_eNB->dlsch_eNB[idUser][0]->nb_rb-1];
		  //printf("\nround: %d dlsch_enB=->nb_rb: %d mcs: %d\n",round,PHY_vars_eNB->dlsch_eNB[idUser][0]->nb_rb,opts.mcs);
//printf("PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->MCS %d\n",PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->mcs);
	      
	      //printf("tbs= %d, G=%d \n",tbs,coded_bits_per_codeword);
	      rate = (double)tbs1/(double)coded_bits_per_codeword1;

	      uncoded_ber_bit = (short*) malloc(2*coded_bits_per_codeword);

	      if (cont_frames==0 && round==0){
		printf("\tRate = %f (%f bits/dim) (G %d, TBS %d, mod %d, pdcch_sym %d)\n",
		       rate,rate*i_mod1,coded_bits_per_codeword1,tbs1,i_mod1,num_pdcch_symbols);		      
		       rate1=rate*i_mod;
		       }
		       else{
				   if(round==1){				  
					rate2= (double)tbs1*((double)i_mod/((double)coded_bits_per_codeword1+(double)coded_bits_per_codeword));		        
					//printf("\t round= %d, Rate1=%f, rate2=%f\n",round,rate1,rate2);
				}
				}
				
	      //TVT: no tpmi in DCI format 1 --------------
	      // use the PMI from previous trial
	      //if (DLSCH_alloc_pdu2_1.tpmi == 5)
                //{
		  //PHY_vars_eNB->dlsch_eNB[0][0]->pmi_alloc = quantize_subband_pmi(&PHY_vars_UE->PHY_measurements,0);                  
		  //PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->pmi_alloc = quantize_subband_pmi(&PHY_vars_UE->PHY_measurements,0);
                //}
                //----------------------
	      //encoding dlsch for principal eNB and interferer
	      status= dlsch_encoding(input_buffer,
				     &PHY_vars_eNB->lte_frame_parms,num_pdcch_symbols,
				     PHY_vars_eNB->dlsch_eNB[idUser][0],0,opts.subframe,
				     &PHY_vars_eNB->dlsch_rate_matching_stats,
				     &PHY_vars_eNB->dlsch_turbo_encoding_stats,
				     &PHY_vars_eNB->dlsch_interleaving_stats);

	      if (status<0)	exit(-1);
	      for(i=0;i<opts.nInterf;i++)
		{
		  status= dlsch_encoding(interferer_input_buffer[i],
					 &(interf_PHY_vars_eNB[i]->lte_frame_parms),num_pdcch_symbols,
					 interf_PHY_vars_eNB[i]->dlsch_eNB[0][0],0,opts.subframe,
					 &interf_PHY_vars_eNB[i]->dlsch_rate_matching_stats,
					 &interf_PHY_vars_eNB[i]->dlsch_turbo_encoding_stats,
					 &interf_PHY_vars_eNB[i]->dlsch_interleaving_stats
					 );
		    
		  if (status<0)	exit(-1);
		}
                


	      PHY_vars_eNB->dlsch_eNB[idUser][0]->rnti = opts.n_rnti+idUser;

	      //scrambling
	      dlsch_scrambling(&PHY_vars_eNB->lte_frame_parms,
			       0,
			       PHY_vars_eNB->dlsch_eNB[idUser][0],
			       coded_bits_per_codeword, 0, opts.subframe<<1);
                                 
	      for(i=0;i<opts.nInterf;i++)
                {
		  dlsch_scrambling(&(interf_PHY_vars_eNB[i]->lte_frame_parms),
				   0,
				   interf_PHY_vars_eNB[i]->dlsch_eNB[0][0],
				   coded_bits_per_codeword, 0, opts.subframe<<1);
		}

	      if (opts.nframes==1)
                {
		  _dumpTransportBlockSegments(PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->C,
					      PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Cminus,
					      PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Kminus,
					      PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Kplus,
					      null,
					      PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->c);
                }

	      //Modulation
	      re_allocated = dlsch_modulation(PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell],
					      AMP,
					      opts.subframe,
					      &PHY_vars_eNB->lte_frame_parms,
					      num_pdcch_symbols,
					      PHY_vars_eNB->dlsch_eNB[idUser][0]);
                                                
	      for(i=0;i<opts.nInterf;i++)
                {
		  dlsch_modulation(interf_PHY_vars_eNB[i]->lte_eNB_common_vars.txdataF[0],
				   AMP,
				   opts.subframe,
				   &(interf_PHY_vars_eNB[i])->lte_frame_parms,
				   num_pdcch_symbols,
				   interf_PHY_vars_eNB[i]->dlsch_eNB[0][0]);
		}
	      _writeTxData("2","mod", 0, 2,opts,0,0);

	      /*if (cont_frames==0 && round==0) 
		printf("re_allocated:  %d\n",re_allocated);*/

	      //Generate pilots 
				
				 
				
				
	      generate_pilots(PHY_vars_eNB,PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell],
			      AMP,
			      LTE_NUMBER_OF_SUBFRAMES_PER_FRAME);
				
	      for(i=0;i<opts.nInterf;i++)
		{
		  generate_pilots(interf_PHY_vars_eNB[i],interf_PHY_vars_eNB[i]->lte_eNB_common_vars.txdataF[0],
				  AMP,
				  LTE_NUMBER_OF_SUBFRAMES_PER_FRAME);
		}
                
	      _writeTxData("3","pilots", 0, 2,opts,0,0);
				
	      //OFDM Modulation
	      for(i=0;i<3;i++)
		{
		  do_OFDM_mod(PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell],
			      PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.Nid_cell],
			      (opts.subframe*2)+i,
			      &PHY_vars_eNB->lte_frame_parms);
								
		  for(j=0;j<opts.nInterf;j++)
		    {
		      do_OFDM_mod(interf_PHY_vars_eNB[j]->lte_eNB_common_vars.txdataF[0],
				  interf_PHY_vars_eNB[j]->lte_eNB_common_vars.txdata[0],
				  (opts.subframe*2)+i,
				  &interf_PHY_vars_eNB[j]->lte_frame_parms);
		    }
		}				
                            
	      _writeTxData("6","ofdm3", 0, 3,opts,0,1);
				
	      for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx; aa++) {
		tx_lev += signal_energy(&PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.Nid_cell][aa]
					[opts.subframe*PHY_vars_eNB->lte_frame_parms.samples_per_tti],
					PHY_vars_eNB->lte_frame_parms.samples_per_tti);
		for(i=0;i<opts.nInterf;i++)
		  {
		    itx_lev[i] += signal_energy(&interf_PHY_vars_eNB[i]->lte_eNB_common_vars.txdata[0][aa]
						[opts.subframe*interf_PHY_vars_eNB[i]->lte_frame_parms.samples_per_tti],
						interf_PHY_vars_eNB[i]->lte_frame_parms.samples_per_tti);
		  }
                                            
	      }

	      tx_lev_dB = (unsigned int) dB_fixed(tx_lev);
	      for(i=0;i<opts.nInterf;i++)
                {
		  itxlev_dB[i] = (unsigned int) dB_fixed(itx_lev[i]);
		}

	      if (opts.nframes==1) {					
		printf("tx_lev = %d (%d dB)\n",tx_lev,tx_lev_dB);
		for(i=0;i<opts.nInterf;i++)
		  {	
		    printf("itx_lev[%d] = %d (%d dB)\n",i,itx_lev[i],itxlev_dB[i]);
		  }
                    
		write_output("txsigF0.m","txsF0", &PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell][0][0],opts.SIZE_TXDATAF ,1,1);                                     
		if(opts.nInterf>0)
		  {
		    write_output("txsigF1.m","txsF1", &interf_PHY_vars_eNB[0]->lte_eNB_common_vars.txdataF[opts.Nid_cell][0][0],opts.SIZE_TXDATAF ,1,1);                                                         
		    write_output("txsig1.m","txs1", &interf_PHY_vars_eNB[0]->lte_eNB_common_vars.txdata[opts.Nid_cell][0][0],opts.SIZE_TXDATA/20,1,1);					
		  }
		if (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1)
		  write_output("txsigF1.m","txsF1", &PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell][1][0],opts.SIZE_TXDATAF,1,1);                       
		write_output("txsig0.m","txs0", &PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.Nid_cell][0][0],opts.SIZE_TXDATA/20,1,1);					
																												
	      }
		
 

	      _fillData(opts,data,2);	   
	      NB_RB2[round]=conv_nprb(0,((DCI1_5MHz_TDD_t *)&DLSCH_alloc_pdu_1)->rballoc,opts.N_RB_DL);
	      numOFDMSymbSubcarrier=(double)PHY_vars_UE->lte_frame_parms.ofdm_symbol_size/(NB_RB2[round]*12.0);
		  
                
	      sigma2_dB = 10*log10((double)tx_lev) +10*log10(numOFDMSymbSubcarrier) - SNR- get_pa_dB(PHY_vars_eNB->pdsch_config_dedicated);
	      sigma2 = pow(10,sigma2_dB/10);
                //printf("\nround: %d sigma2_dB: %f\n",round,sigma2_dB);
	      //Noise and Interference
			//printf("before multipath\n")	;
	      _apply_Multipath_Noise_Interference(&opts,data,sigma2_dB,sigma2,2,round);
	   
				
	      _writeTxData("7","noise_ch_int", 0, 3,opts,1,1);	
				
	      /*****End Sending***/				
	      if (opts.nframes==1) {
		printf("Sigma2 %f (sigma2_dB %f)\n",sigma2,sigma2_dB);
		printf("RX level in null symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
		printf("RX level in data symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
		printf("rx_level Null symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
		printf("rx_level data symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
	      }
		  if (round==0)
			i_mod = get_Qm(opts.mcs);
		  else
			i_mod = get_Qm(opts.mcs2);

	      /*********Reciver **************/
	      //TODO: Optimize and clean code
	      // Inner receiver scheduling for 3 slots
	      for (Ns=(2*opts.subframe); Ns<((2*opts.subframe)+3); Ns++)
                {
		  for (l=0; l<opts.pilot2 ; l++)
                    {  		            
		      slot_fep(PHY_vars_UE,l,Ns%20,0,0);			
		      //TODO: Set NCell_id_i = syronger interferer
			
#ifdef PERFECT_CE						
		      _fillPerfectChannelDescription(opts,l);										
#endif
                       
						
					    
		      if ((Ns==((2*opts.subframe))) && (l==0)) {
			lte_ue_measurements(PHY_vars_UE,opts.subframe*PHY_vars_UE->lte_frame_parms.samples_per_tti,1,0);
		      }
						
		      if ((Ns==(2*opts.subframe)) && (l==opts.pilot1))
                        {// process symbols 0,1,2

			  if (opts.dci_flag == 1)
                            {
			      rx_pdcch(&PHY_vars_UE->lte_ue_common_vars,
				       PHY_vars_UE->lte_ue_pdcch_vars,
				       &PHY_vars_UE->lte_frame_parms,
				       opts.subframe,
				       0,
				       (PHY_vars_UE->lte_frame_parms.mode1_flag == 1) ? SISO : ALAMOUTI,
				       0);

			      PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols = num_pdcch_symbols;

			      dci_cnt = dci_decoding_procedure(PHY_vars_UE,dci_alloc_rx,1,0,opts.subframe);
			      //
			      if (dci_cnt==0)
                                {
				  dlsch_active = 0;
				  if (round==0) {
				    dci_errors++;
				    round=5;
				    errs[0]++;
				    round_trials[0]++;
				  }
                                }

			      for (i=0; i<dci_cnt; i++)
                                {                                    
				  status =generate_ue_dlsch_params_from_dci(0,dci_alloc_rx[i].dci_pdu,
									    dci_alloc_rx[i].rnti,dci_alloc_rx[i].format,
									    PHY_vars_UE->dlsch_ue[0],&PHY_vars_UE->lte_frame_parms,PHY_vars_UE->pdsch_config_dedicated,
									    SI_RNTI,0,P_RNTI);                                                                                                            
                                                                               
				  if ((dci_alloc_rx[i].rnti == opts.n_rnti) && (status==0)) {

				    coded_bits_per_codeword = get_G(&PHY_vars_eNB->lte_frame_parms,
								    PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->nb_rb,
								    PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->rb_alloc,
								    get_Qm(PHY_vars_UE->dlsch_ue[0][0]->harq_processes[PHY_vars_UE->dlsch_ue[0][0]->current_harq_pid]->mcs),
								    PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols,
								    0,opts.subframe);
				    dlsch_active = 1;
				  }
				  else {
				    dlsch_active = 0;
				    if (round==0) {
				      dci_errors++;
				      errs[0]++;
				      round_trials[0]++;

				      if (opts.nframes==1) {
					printf("DCI misdetection trial %d\n",cont_frames);
					round=5;
				      }
				    }
				  }
                                }
                            }  // if dci_flag==1
			  else { //dci_flag == 0

			    PHY_vars_UE->lte_ue_pdcch_vars[0]->crnti = opts.n_rnti;
			    PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols = num_pdcch_symbols;

			    generate_ue_dlsch_params_from_dci(0,&DLSCH_alloc_pdu_1,
							      C_RNTI,
							      format1,//TVT: E_2A_M10PRB,
							      PHY_vars_UE->dlsch_ue[0],
							      &PHY_vars_UE->lte_frame_parms,PHY_vars_UE->pdsch_config_dedicated,
							      SI_RNTI,
							      0,
							      P_RNTI);                            
			    dlsch_active = 1;
			  } // if dci_flag == 1
                        }

		      if (dlsch_active == 1)
                        {
			  if ((Ns==(1+(2*opts.subframe))) && (l==0))
                            {// process PDSCH symbols 1,2,3,4,5,(6 Normal Prefix)

			      for (m=PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols; m<opts.pilot2; m++)
                                {
#if defined ENABLE_FXP || ENABLE_FLP
				  //printf("fxp or flp release used\n");                                    		                                        
            					
				  if (rx_pdsch(PHY_vars_UE,
					       PDSCH,
					       opts.Nid_cell,
					       eNB_id_i,
					       opts.subframe,
					       m,
					       (m==PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols)?1:0,
					       opts.dual_stream_UE,
					       i_mod,
					       0))
                                    {
										
				      dlsch_active = 0;
				      break;
                                    }
#endif
#ifdef ENABLE_FULL_FLP
				  //    printf("Full flp release used\n");                                 
				  if (rx_pdsch_full_flp(PHY_vars_UE,
							PDSCH,
							opts.Nid_cell,
							eNB_id_i,
							opts.subframe,
							m,
							(m==PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols)?1:0,
							opts.dual_stream_UE,
							i_mod))
                                    {
				      dlsch_active = 0;
				      break;
                                    }
#endif
                                }
                            }

			  if ((Ns==(1+(2*opts.subframe))) && (l==opts.pilot1))
                            {// process symbols (6 Extended Prefix),7,8,9
			      for (m=opts.pilot2; m<opts.pilot3; m++)
                                {
#if defined ENABLE_FXP || ENABLE_FLP
				  //			printf("fxp or flp release used\n");

				  if (rx_pdsch(PHY_vars_UE,
					       PDSCH,
					       opts.Nid_cell,
					       eNB_id_i,
					       opts.subframe,
					       m,
					       0,
					       opts.dual_stream_UE,
					       i_mod,0)==-1)
                                    {										
				      dlsch_active=0;
				      break;
                                    }
#endif
#ifdef ENABLE_FULL_FLP
				  // printf("Full flp release used\n");                                    
				  if (rx_pdsch_full_flp(PHY_vars_UE,
							PDSCH,
							opts.Nid_cell,
							eNB_id_i,
							opts.subframe,
							m,
							0,
							opts.dual_stream_UE,
							i_mod)==-1)
                                    {
				      dlsch_active=0;
				      break;
                                    }
#endif
                                }
                            }

			  if ((Ns==(2+(2*opts.subframe))) && (l==0))  // process symbols 10,11,(12,13 Normal Prefix) do deinterleaving for TTI
                            {
			      for (m=opts.pilot3; m<PHY_vars_UE->lte_frame_parms.symbols_per_tti; m++)
                                {
#if defined ENABLE_FXP || ENABLE_FLP
				  //			printf("fxp or flp release used\n");                               
				  if (rx_pdsch(PHY_vars_UE,
					       PDSCH,
					       opts.Nid_cell,
					       eNB_id_i,
					       opts.subframe,
					       m,
					       0,
					       opts.dual_stream_UE,
					       i_mod,0)==-1)
                                    {										
				      dlsch_active=0;
				      break;
                                    }
#endif
#ifdef ENABLE_FULL_FLP
				  // printf("Full flp release used\n");        
				  if (rx_pdsch_full_flp(PHY_vars_UE,
							PDSCH,
							opts.Nid_cell,
							eNB_id_i,
							opts.subframe,
							m,
							0,
							opts.dual_stream_UE,
							i_mod)==-1)
                                    {
				      dlsch_active=0;
				      break;
                                    }
#endif
                                }
                            }                           
                        }
                        
                    }
                }

	      if(opts.nframes==1)
		{
		  printf("Dumping DLSCH output\n");
		  _writeOuputOneFrame(opts,coded_bits_per_codeword,uncoded_ber_bit,tbs);
		  write_output("fch0e.m","ch0e",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][0][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
		  write_output("fch1e.m","ch1e",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][0][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
		}


	      /*if(round==0)
		{
		raw_ber += compute_ber_soft(PHY_vars_eNB->dlsch_eNB[0][0]->e,
		PHY_vars_UE->lte_ue_pdsch_vars[0]->llr[0],
		coded_bits_per_codeword);
					
		}*/
				
	      //Compute BER
	      uncoded_ber=0;
	      for (i=0;i<coded_bits_per_codeword;i++) 
		{
		  if (PHY_vars_eNB->dlsch_eNB[0][0]->e[i] != (PHY_vars_UE->lte_ue_pdsch_vars[0]->llr[0][i]<0)) {
		    uncoded_ber_bit[i] = 1;
		    uncoded_ber++;
		  }
		  else
		    uncoded_ber_bit[i] = 0;
		}
	      // printf("uncoded_ber %f coded_bits_per_codeword %d \n ",uncoded_ber,coded_bits_per_codeword);				
	      uncoded_ber/=coded_bits_per_codeword;
	      avg_ber += uncoded_ber;
	      numresults++;
	      //  printf("avg_ber: %f\n",avg_ber);
		
	      //End compute BER
																	

	      PHY_vars_UE->dlsch_ue[0][0]->rnti = opts.n_rnti;
	      PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->G = coded_bits_per_codeword;
	      dlsch_unscrambling(&PHY_vars_UE->lte_frame_parms,
				 0,
				 PHY_vars_UE->dlsch_ue[0][0],
				 coded_bits_per_codeword,
				 PHY_vars_UE->lte_ue_pdsch_vars[opts.Nid_cell]->llr[0],
				 0,
				 opts.subframe<<1);
				
	      ret = dlsch_decoding(PHY_vars_UE,
				   PHY_vars_UE->lte_ue_pdsch_vars[opts.Nid_cell]->llr[0],
				   &PHY_vars_UE->lte_frame_parms,
				   PHY_vars_UE->dlsch_ue[0][0],
				   PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0],
				   opts.subframe,0,
				   1,0);
			
			 
#ifdef XFORMS
	      do_forms(form,
		       &PHY_vars_UE->lte_frame_parms,  
		       PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates_time[0],
		       PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0],
		       PHY_vars_UE->lte_ue_common_vars.rxdata,
		       PHY_vars_UE->lte_ue_common_vars.rxdataF,
		       PHY_vars_UE->lte_ue_pdsch_vars[0]->rxdataF_comp[0],
		       PHY_vars_UE->lte_ue_pdsch_vars[1]->rxdataF_comp[0],
		       PHY_vars_UE->lte_ue_pdsch_vars[0]->dl_ch_rho_ext[0],
		       PHY_vars_UE->lte_ue_pdsch_vars[0]->llr[0],coded_bits_per_codeword);
	      /* printf("Hit a key to continue\n");
		 char c = getchar();*/ 				 
#endif
			
			
	      _writeTxData("8","unsc_undec", 0, 2,opts,1,2);	
	      if (ret <= MAX_TURBO_ITERATIONS)  //No hay errores 4
                {
			 //round=5;		    
			 if (opts.fix_rounds==0)
				round=5;
			else
				round++;
		 
			 if (opts.nframes==1) {
			   printf("No DLSCH errors found\n");
			   _dumpTransportBlockSegments(PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->C,
						       PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Cminus,
						       PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Kminus,
						       PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Kplus,
						       PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->c,
						       PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->c);
			 }
                }
	      else
                {
		  errs[round]++;
		  //Lid: round++;
  
		  if (opts.nframes==1)
                    {
		      printf("DLSCH in error in round %d (ret %d)\n",round,ret);
		      printf("DLSCH errors found, uncoded ber %f\n",uncoded_ber);
		      _dumpTransportBlockSegments(PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->C,
						  PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Cminus,
						  PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Kminus,
						  PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Kplus,
						  PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->c,
						  PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->c);
		      exit(1);
                    }
		  round++;

		  if (opts.nframes==1) printf("DLSCH error in round %d\n",round);

                }
	      free(uncoded_ber_bit);
	      uncoded_ber_bit = NULL;
	      //printf("\t count_interf[%d]=%d\n",round, opts.interf_count[round]);
	      
			
            }  //round
            
		
	  if ((errs[0]>=opts.nframes/10) && (cont_frames>(opts.nframes/2)))
		break;        	

        }   //cont_frames
		
		
		
	  //printf("nprb1: %d, nprb2: %d, mcs: %d, mcs2: %d\n",opts.nprb1,opts.nprb2,opts.mcs, opts.mcs2);	    
      printf("\n---------------------------------------------------------------------\n");
      printf("SNR = %f dB (tx_lev %f, sigma2_dB %f)  BER (%f/%d=%f) BLER(%d/%d=%f)\n\t T (%d/%d = %f ) \n",
	     SNR,(double)tx_lev_dB+10*log10(numOFDMSymbSubcarrier),
	     sigma2_dB,avg_ber,numresults,(avg_ber/numresults),
	     errs[0],round_trials[0],((float)errs[0]/round_trials[0]),
	     0,0,0.0);
              
      fprintf(opts.outputTrougput,"%f %f;\n",SNR,  rate*((double)(round_trials[0]-dci_errors)/((double)round_trials[0] + round_trials[1] + round_trials[2] + round_trials[3])));
		printf("\t count_interf[0]=%d, count_interf[1]=%d\n",opts.interf_count[0], opts.interf_count[1]);
      _printResults(errs,round_trials,dci_errors,rate);
      //_printFileResults( SNR, rate1,  rate,errs,round_trials, dci_errors, opts,avg_ber/numresults);

		//if (((double)errs[0]/(round_trials[0]))<1e-2) break;//IF errors > 1%
		//TVT:if the outage is greater than some threshold stop, otherwise decrease the nprb2
        if (((double)errs[1]/(round_trials[0]+round_trials[1]))>1e-2){ //IF Pout2 > 1% 		
        printf("\t rate1:%f, rate2:%f \n",rate1,rate2);
         _printFileResults( SNR, rate1,rate2,  rate,errs,round_trials, dci_errors, opts,avg_ber/numresults);
		 break;
	    }
		opts.nprb2--; 
	 	}//nprb2
	 	if ((double)errs[0]/round_trials[0]<1e-2) //IF Pout1 > 1% 		 
		break;
    }// SNR
	//}//mcs


}



void do_OFDM_mod(mod_sym_t **txdataF, int32_t **txdata, uint16_t next_slot, LTE_DL_FRAME_PARMS *frame_parms) {

  int aa, slot_offset, slot_offset_F;


  slot_offset_F = (next_slot)*(frame_parms->ofdm_symbol_size)*((frame_parms->Ncp==1) ? 6 : 7);
  slot_offset = (next_slot)*(frame_parms->samples_per_tti>>1);

  for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
    if (frame_parms->Ncp == 1)
      PHY_ofdm_mod(&txdataF[aa][slot_offset_F],    // input
		   &txdata[aa][slot_offset],       // output
		   frame_parms->log2_symbol_size,  // log2_fft_size
		   6,                				 // number of symbols
		   frame_parms->nb_prefix_samples, // number of prefix samples
		   frame_parms->twiddle_ifft,  	 // IFFT twiddle factors
		   frame_parms->rev,           	 // bit-reversal permutation
		   CYCLIC_PREFIX);
    else {
      normal_prefix_mod(&txdataF[aa][slot_offset_F],
			&txdata[aa][slot_offset],
			7,
			frame_parms);
    }
  }


}
void _apply_Multipath_Noise_Interference(options_t *opts,data_t data,double sigma2_dB,double sigma2,int numSubFrames,int round)
{
  double iqim=0.0;
  int j;

  //Multipath channel
  //Generates and applys a random frequency selective random channel model.      
  multipath_channel(eNB2UE,data.s_re,data.s_im,data.r_re,data.r_im,numSubFrames*frame_parms->samples_per_tti,0);    
  for(j=0;j<opts->nInterf;j++)
    {
      multipath_channel(interf_eNB2UE[j],data.is_re[j],data.is_im[j],data.ir_re[j],data.ir_im[j],numSubFrames*frame_parms->samples_per_tti,0);					 
    }

  //Interference
  //printf("antes de applyInterf\n");
  _applyInterference(opts,data,sigma2,iqim,numSubFrames,round);
    
  //Noise
  _applyNoise(opts,data,sigma2,iqim,numSubFrames);

  if (opts->nframes==1) {
    printf("Sigma2 %f (sigma2_dB %f)\n",sigma2,sigma2_dB);
    printf("RX level in null symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
    printf("RX level in data symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
    printf("rx_level Null symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
    printf("rx_level data symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
  }	

}

void _writeOuputOneFrame(options_t opts,uint32_t coded_bits_per_codeword,short *uncoded_ber_bit,uint32_t tbs)
{

  printf("log2_maxh => %d\n",PHY_vars_UE->lte_ue_pdsch_vars[0]->log2_maxh);
			
  write_output("rho.m","rho_0",PHY_vars_UE->lte_ue_pdsch_vars[0]->dl_ch_rho_ext[0],300*((PHY_vars_UE->lte_frame_parms.Ncp == 0) ? 14 : 12),1,1);	

	
  if (PHY_vars_UE->lte_frame_parms.nb_antennas_rx>1) {    
    write_output("dlsch01_ch0.m","dl01_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][1][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
    write_output("dlsch10_ch0.m","dl10_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][2][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);        
    write_output("rxsigF1.m","rxsF1", &PHY_vars_UE->lte_ue_common_vars.rxdataF[1][0],opts.SIZE_RXDATAF,1,1);
    write_output("rxsig1.m","rxs1", &PHY_vars_UE->lte_ue_common_vars.rxdata[1][0],opts.SIZE_RXDATA,1,1);		
					 
    if (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1)        
      write_output("dlsch11_ch0.m","dl11_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][3][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);            
		
  }

  write_output("dlsch00_ch0.m","dl00_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][0][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
  write_output("dlsch00_ch1.m","dl00_ch1",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][0][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
  

  write_output("dlsch_e.m","e",PHY_vars_eNB->dlsch_eNB[0][0]->e,coded_bits_per_codeword,1,4);
  write_output("dlsch_ber_bit.m","ber_bit",uncoded_ber_bit,coded_bits_per_codeword,1,0);
  write_output("dlsch_eNB_w.m","w",PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->w[0],3*(tbs+64),1,4);
  write_output("dlsch_UE_w.m","w",PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->w[0],3*(tbs+64),1,0);

  write_output("rxsigF0.m","rxsF0", &PHY_vars_UE->lte_ue_common_vars.rxdataF[0][0],opts.SIZE_RXDATAF,1,1);
  write_output("rxsig0.m","rxs0", &PHY_vars_UE->lte_ue_common_vars.rxdata[0][0],opts.SIZE_RXDATA,1,1);	
  write_output("ch0.m","ch0",eNB2UE->ch[0],eNB2UE->channel_length,1,8);

  if (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1)
    {
      write_output("ch1.m","ch1",eNB2UE->ch[PHY_vars_eNB->lte_frame_parms.nb_antennas_rx],eNB2UE->channel_length,1,8);		
    }	
		

	
  write_output("pdcchF0_ext.m","pdcchF_ext", PHY_vars_UE->lte_ue_pdcch_vars[opts.Nid_cell]->rxdataF_ext[0],2*3*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size,1,1);
  write_output("pdcch00_ch0_ext.m","pdcch00_ch0_ext",PHY_vars_UE->lte_ue_pdcch_vars[opts.Nid_cell]->dl_ch_estimates_ext[0],300*3,1,1);
  write_output("pdcch_rxF_comp0.m","pdcch0_rxF_comp0",PHY_vars_UE->lte_ue_pdcch_vars[opts.Nid_cell]->rxdataF_comp[0],opts.nsymb*300,1,1);
  write_output("pdcch_rxF_llr.m","pdcch_llr",PHY_vars_UE->lte_ue_pdcch_vars[opts.Nid_cell]->llr,2400,1,4);
	
  dump_dlsch2(PHY_vars_UE,opts.Nid_cell,coded_bits_per_codeword);
  dump_dlsch2(PHY_vars_UE,1,coded_bits_per_codeword);
	
  char fname[32],vname[32];
  int i;	
  for(i=0;i<2;i++)
    {
      sprintf(fname,"dlsch%d_rxF_ext0_%d.m",i,opts.testNumber);
      sprintf(vname,"dl%d_rxF_ext0_%d",i,opts.testNumber);
      write_output(fname,vname,PHY_vars_UE->lte_ue_pdsch_vars[i]->rxdataF_ext[0],300*opts.nsymb,1,1);
	  
      sprintf(fname,"dlsch%d_rxF_comp0_%d.m",i,opts.testNumber);
      sprintf(vname,"dl%d_rxF_comp0_%d",i,opts.testNumber);
      write_output(fname,vname,PHY_vars_UE->lte_ue_pdsch_vars[i]->rxdataF_comp[0],300*opts.nsymb,1,1);
			
    } 
  
  sprintf(fname,"dlsch%d_rxF_llr_%d.m",i,opts.testNumber);
  sprintf(vname,"dl%d_llr_%d",i,opts.testNumber);
  write_output(fname,vname, PHY_vars_UE->lte_ue_pdsch_vars[0]->llr[0],coded_bits_per_codeword,1,0);

	

}

void _dumpTransportBlockSegments(uint32_t C,uint32_t Cminus,uint32_t Kminus,uint32_t Kplus,  uint8_t ** c_UE, uint8_t ** c_eNB)
{
  int i,s;
  int Kr,Kr_bytes;
  for (s=0; s<C; s++)
    {
      if (s<Cminus)
	Kr = Kminus;
      else
	Kr = Kplus;
      Kr_bytes = Kr>>3;

      //  printf("Decoded_output (Segment %d):\n",s);
      for (i=0; i<Kr_bytes; i++)
        {
	  if ( c_UE !=NULL)
	    printf("%d : %x (%x)\n",i,c_UE[s][i],c_UE[s][i]^c_eNB[s][i]);
	  else
	    printf("%d : (%x)\n",i,c_eNB[s][i]);
        }
    }
}


void _writeTxData(char *num,char *desc, int init, int numframes,options_t opts, int output,int senial)
{
  char fileName[80], vectorName[80];
  int i;
  if(WRITE_FILES && opts.nframes==1)
    {
		
      if(!output)
	{
	  if(senial==0 || senial==2)
	    {
	      sprintf(fileName,"log%s_txsigF_%s.m",num,desc);
	      sprintf(vectorName,"txF%s",num);

	      write_output(fileName,vectorName, &PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.Nid_cell][0][0],(opts.SIZE_TXDATAF/10)*numframes ,1,1);                    
	    }
	  if(senial==1 || senial==2)
	    {
				
	      sprintf(fileName,"log%s_txsig_%s.m",num,desc);
	      sprintf(vectorName,"tx%s",num);
	      write_output(fileName,vectorName, &PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.Nid_cell][0][0],opts.SIZE_TXDATA,1,1);					
	    }
				
	  for(i=0;i<opts.nInterf;i++)
	    {
	      if(senial==0 || senial==2)
		{
		  sprintf(fileName,"i%dlog%s_txsigF_%s.m",i,num,desc);
		  sprintf(vectorName,"i%dtxF%s",i,num);

		  write_output(fileName,vectorName, &(interf_PHY_vars_eNB[i]->lte_eNB_common_vars.txdataF[opts.Nid_cell][0][0]),(opts.SIZE_TXDATAF/10)*numframes ,1,1);                    
		}
	      if(senial==1 || senial==2)
		{
					
		  sprintf(fileName,"i%dog%s_txsig_%s.m",i,num,desc);
		  sprintf(vectorName,"i%tx%s",i,num);
		  write_output(fileName,vectorName, &interf_PHY_vars_eNB[i]->lte_eNB_common_vars.txdata[opts.Nid_cell][0][0],opts.SIZE_TXDATA,1,1);					
		}
	    }
	}
      else
	{
	  sprintf(fileName,"log%s_rxsigF_%s.m",num,desc);
	  sprintf(vectorName,"rxF%s",num);

	  write_output(fileName,vectorName,&PHY_vars_UE->lte_ue_common_vars.rxdataF[0][0],opts.SIZE_RXDATAF,1,1);
			
	  sprintf(fileName,"log%s_rxsig_%s.m",num,desc);
	  sprintf(vectorName,"rx%s",num);
	  write_output(fileName,vectorName, &PHY_vars_UE->lte_ue_common_vars.rxdata[0][0],opts.SIZE_RXDATA,1,1);	
	}
		
    }
}


double compute_ber_soft(uint8_t* ref, int16_t* rec, int n)
{
  int k;
  int e = 0;
 
  for(k = 0; k < n; k++) {
    if((ref[k]==1) != (rec[k]<0)) {
#ifdef SIG_DEBUG	   
      printf("error pos %d ( %d => %d)\n",k,ref[k],rec[k]);
#endif       
      e++;
    }
  }  
  //printf("ber:%d ,%d , %f\t\n",x++,e ,(double)n);
  
  return (double)e / (double)n;
}


void _fillPerfectChannelDescription(options_t opts,uint8_t l)
{
  //printf("Algo con la inter : %f",pow(10.0,.05*opts.dbInterf[0]));
  int aa, aarx,i,j;
	
  //init_freq_channel(eNB2UE,PHY_vars_UE->lte_frame_parms.N_RB_DL,12*PHY_vars_UE->lte_frame_parms.N_RB_DL + 1);  
		
  freq_channel(eNB2UE,PHY_vars_UE->lte_frame_parms.N_RB_DL,12*PHY_vars_UE->lte_frame_parms.N_RB_DL + 1);
			
  for(j=0;j<opts.nInterf;j++)
    {
      //init_freq_channel(interf_eNB2UE[j],PHY_vars_UE->lte_frame_parms.N_RB_DL,12*PHY_vars_UE->lte_frame_parms.N_RB_DL + 1);  
      freq_channel(interf_eNB2UE[j],PHY_vars_UE->lte_frame_parms.N_RB_DL,12*PHY_vars_UE->lte_frame_parms.N_RB_DL + 1);
		
		
    }	printf("PHY_vars_UE->dlsch_ue[0][0]->sqrt_rho_b %d",PHY_vars_UE->dlsch_ue[0][0]->sqrt_rho_b);
  for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
    {
      for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
	{
	  for (i=0; i<frame_parms->N_RB_DL*12; i++)
	    {
	      if (opts.awgn_flag==0) 
		{
		  /*((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].x*AMP/2);											
		    ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].y*AMP/2) ;					
		  */
					
		  ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].x*AMP/2);											
		  ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].y*AMP/2) ;					 
													
					
		  if(opts.nInterf>0) //Max num interferer 
		    {
			  if(opts.awgn_flagi==0){
				if(j==opts.Nid_cell) continue;						
				((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(interf_eNB2UE[0]->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].x*AMP/2)*pow(10.0,.05*opts.dbInterf[0]);											
				((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(interf_eNB2UE[0]->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].y*AMP/2)*pow(10.0,.05*opts.dbInterf[0]) ;
				}
				else{
					if(j==opts.Nid_cell) continue;					
					((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(short)((AMP/2)*(pow(10.0,.05*opts.dbInterf[0])));
					((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=0;													 
				}
			}
		}
	      else{						
					
		((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.Nid_cell][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=AMP/2;
		((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=0;

		if(opts.nInterf>0)//Max num interferer 
		  {
		    /*if(j==opts.Nid_cell) continue;					
		    ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(short)((AMP/2)*(pow(10.0,.05*opts.dbInterf[0])));
		    ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=0;											
		    */
		    if(opts.awgn_flagi==0){
				if(j==opts.Nid_cell) continue;						
				((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(interf_eNB2UE[0]->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].x*AMP/2)*pow(10.0,.05*opts.dbInterf[0]);											
				((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(interf_eNB2UE[0]->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].y*AMP/2)*pow(10.0,.05*opts.dbInterf[0]) ;
				}
				else{
					if(j==opts.Nid_cell) continue;					
					((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(short)((AMP/2)*(pow(10.0,.05*opts.dbInterf[0])));
					((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[1][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=0;													 
				}
		  }
	      }				

	    }
	}
    }								
			
}



#ifdef XFORMS
void do_forms(FD_lte_scope *form, LTE_DL_FRAME_PARMS *frame_parms, short **channel, short **channel_f, short **rx_sig, short **rx_sig_f, short *dlsch_comp, short* dlsch_comp_i, short* dlsch_rho, short *dlsch_llr, int coded_bits_per_codeword)
{

  int i,j,ind,k,s;

  float Re,Im;
  float mag_sig[NB_ANTENNAS_RX*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT],
    sig_time[NB_ANTENNAS_RX*4*NUMBER_OF_OFDM_CARRIERS*NUMBER_OF_OFDM_SYMBOLS_PER_SLOT],
    sig2[FRAME_LENGTH_COMPLEX_SAMPLES],
    time2[FRAME_LENGTH_COMPLEX_SAMPLES],
    I[25*12*11*4], Q[25*12*11*4],
    *llr,*llr_time;

  float avg, cum_avg;
  
  llr = malloc(coded_bits_per_codeword*sizeof(float));
  llr_time = malloc(coded_bits_per_codeword*sizeof(float));

  // Channel frequency response
  cum_avg = 0;
  ind = 0;
  for (j=0; j<4; j++) { 
    for (i=0;i<frame_parms->nb_antennas_rx;i++) {
      for (k=0;k<NUMBER_OF_OFDM_CARRIERS*7;k++){
	sig_time[ind] = (float)ind;
	Re = (float)(channel_f[(j<<1)+i][2*k]);
	Im = (float)(channel_f[(j<<1)+i][2*k+1]);
	//mag_sig[ind] = (short) rand(); 
	mag_sig[ind] = (short)10*log10(1.0+((double)Re*Re + (double)Im*Im)); 
	cum_avg += (short)sqrt((double)Re*Re + (double)Im*Im) ;
	ind++;
      }
      //      ind+=NUMBER_OF_OFDM_CARRIERS/4; // spacing for visualization
    }
  }

  avg = cum_avg/NUMBER_OF_USEFUL_CARRIERS;

  //fl_set_xyplot_ybounds(form->channel_f,30,70);
  fl_set_xyplot_data(form->channel_f,sig_time,mag_sig,ind,"","","");

  /*
  // channel time resonse
  cum_avg = 0;
  ind = 0;
  for (k=0;k<1;k++){
  for (j=0;j<1;j++) {
      
  for (i=0;i<frame_parms->ofdm_symbol_size;i++){
  sig_time[ind] = (float)ind;
  Re = (float)(channel[k+2*j][2*i]);
  Im = (float)(channel[k+2*j][2*i+1]);
  //mag_sig[ind] = (short) rand(); 
  mag_sig[ind] = (short)10*log10(1.0+((double)Re*Re + (double)Im*Im)); 
  cum_avg += (short)sqrt((double)Re*Re + (double)Im*Im) ;
  ind++;
  }
  }
  }
  
  //fl_set_xyplot_ybounds(form->channel_t_im,10,90);
  fl_set_xyplot_data(form->channel_t_im,sig_time,mag_sig,ind,"","","");
  */

  // channel_t_re = rx_sig_f[0]
  //for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX; i++)  {
  for (i=0; i<NUMBER_OF_OFDM_CARRIERS*frame_parms->symbols_per_tti/2; i++)  {
    sig2[i] = 10*log10(1.0+(double) ((rx_sig_f[0][4*i])*(rx_sig_f[0][4*i])+(rx_sig_f[0][4*i+1])*(rx_sig_f[0][4*i+1])));
    time2[i] = (float) i;
  } 

  //fl_set_xyplot_ybounds(form->channel_t_re,10,90);
  fl_set_xyplot_data(form->channel_t_re,time2,sig2,NUMBER_OF_OFDM_CARRIERS*frame_parms->symbols_per_tti,"","","");
  //fl_set_xyplot_data(form->channel_t_re,time2,sig2,FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX,"","","");
  

  // channel_t_im = rx_sig[0]
  //if (frame_parms->nb_antennas_rx>1) {
  for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++)  {
    //for (i=0; i<NUMBER_OF_OFDM_CARRIERS*frame_parms->symbols_per_tti/2; i++)  {
    sig2[i] = 10*log10(1.0+(double) ((rx_sig[0][2*i])*(rx_sig[0][2*i])+(rx_sig[0][2*i+1])*(rx_sig[0][2*i+1])));
    time2[i] = (float) i;
  }

  //fl_set_xyplot_ybounds(form->channel_t_im,0,100);
  //fl_set_xyplot_data(form->channel_t_im,&time2[640*12*6],&sig2[640*12*6],640*12,"","","");
  fl_set_xyplot_data(form->channel_t_im,time2,sig2,FRAME_LENGTH_COMPLEX_SAMPLES,"","","");
  //}

  /*
  // PBCH LLR
  j=0;
  for(i=0;i<1920;i++) {
  llr[j] = (float) pbch_llr[i];
  llr_time[j] = (float) j;
  //if (i==63)
  //  i=127;
  //else if (i==191)
  //  i=319;
  j++;
  }

  fl_set_xyplot_data(form->decoder_input,llr_time,llr,1920,"","","");
  //fl_set_xyplot_ybounds(form->decoder_input,-100,100);

  // PBCH I/Q
  j=0;
  for(i=0;i<12*12;i++) {
  I[j] = pbch_comp[2*i];
  Q[j] = pbch_comp[2*i+1];
  j++;
  //if (i==47)
  //  i=96;
  //else if (i==191)
  //  i=239;
  }

  fl_set_xyplot_data(form->scatter_plot,I,Q,12*12,"","","");
  //fl_set_xyplot_xbounds(form->scatter_plot,-100,100);
  //fl_set_xyplot_ybounds(form->scatter_plot,-100,100);

  // PDCCH I/Q
  j=0;
  for(i=0;i<12*25*3;i++) {
  I[j] = pdcch_comp[2*i];
  Q[j] = pdcch_comp[2*i+1];
  j++;
  //if (i==47)
  //  i=96;
  //else if (i==191)
  //  i=239;
  }

  fl_set_xyplot_data(form->scatter_plot1,I,Q,12*25*3,"","","");
  //fl_set_xyplot_xbounds(form->scatter_plot,-100,100);
  //fl_set_xyplot_ybounds(form->scatter_plot,-100,100);
  */

  // DLSCH LLR
  for(i=0;i<coded_bits_per_codeword;i++) {
    llr[i] = (float) dlsch_llr[i];
    llr_time[i] = (float) i;
  }

  fl_set_xyplot_data(form->demod_out,llr_time,llr,coded_bits_per_codeword,"","","");
  fl_set_xyplot_ybounds(form->demod_out,-256,256);

  // DLSCH I/Q
  j=0;
  for (s=0;s<frame_parms->symbols_per_tti;s++) {
    for(i=0;i<12*25;i++) {
      I[j] = dlsch_comp[(2*25*12*s)+2*i];
      Q[j] = dlsch_comp[(2*25*12*s)+2*i+1];
      j++;
    }
    //if (s==2)
    //  s=3;
    //else if (s==5)
    //  s=6;
    //else if (s==8)
    //  s=9;
  }

  fl_set_xyplot_data(form->scatter_plot,I,Q,j,"","","");
  fl_set_xyplot_xbounds(form->scatter_plot,-200,200);
  fl_set_xyplot_ybounds(form->scatter_plot,-200,200);

  // DLSCH I/Q
  j=0;
  for (s=0;s<frame_parms->symbols_per_tti;s++) {
    for(i=0;i<12*25;i++) {
      I[j] = dlsch_comp_i[(2*25*12*s)+2*i];
      Q[j] = dlsch_comp_i[(2*25*12*s)+2*i+1];
      j++;
    }
    //if (s==2)
    //  s=3;
    //else if (s==5)
    //  s=6;
    //else if (s==8)
    //  s=9;
  }

  fl_set_xyplot_data(form->scatter_plot1,I,Q,j,"","","");
  fl_set_xyplot_xbounds(form->scatter_plot1,-1000,1000);
  fl_set_xyplot_ybounds(form->scatter_plot1,-1000,1000);

  // DLSCH I/Q
  j=0;
  for (s=0;s<frame_parms->symbols_per_tti;s++) {
    for(i=0;i<12*25;i++) {
      I[j] = dlsch_rho[(2*25*12*s)+2*i];
      Q[j] = dlsch_rho[(2*25*12*s)+2*i+1];
      j++;
    }
    //if (s==2)
    //  s=3;
    //else if (s==5)
    //  s=6;
    //else if (s==8)
    //  s=9;
  }

  fl_set_xyplot_data(form->scatter_plot2,I,Q,j,"","","");
  fl_set_xyplot_xbounds(form->scatter_plot2,-1000,1000);
  fl_set_xyplot_ybounds(form->scatter_plot2,-1000,1000);


  free(llr);
  free(llr_time);

}
#endif
