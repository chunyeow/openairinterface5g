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
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "SIMULATION/TOOLS/defs.h"
#include "SIMULATION/RF/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"

#ifdef OPENAIR2
#include "LAYER2/MAC/defs.h"
#include "LAYER2/MAC/vars.h"
#include "UTIL/LOG/log_if.h"
#include "UTIL/LOG/log_extern.h"
#include "RRC/MESH/vars.h"
#include "PHY_INTERFACE/vars.h"
#ifdef OCG
#include "UTIL/OCG/OCG.h"
#include "OCG_extern.h"
#endif 
#endif

#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"

#ifdef IFFT_FPGA
#include "PHY/LTE_REFSIG/mod_table.h"
#endif

#include "SCHED/defs.h"
#include "SCHED/vars.h"

#ifdef XFORMS
#include "forms.h"
#include "phy_procedures_sim_form.h"
#endif

#define DEBUG_PHY
#define RF

//#define DEBUG_SIM 

#define BW 7.68

#define N_TRIALS 1

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


uint16_t NODE_ID[1];
uint8_t NB_INST=2;

void init_bypass() {

  msg("[PHYSIM] INIT BYPASS\n");      
  pthread_mutex_init(&Tx_mutex,NULL);
  pthread_cond_init(&Tx_cond,NULL);
  Tx_mutex_var=1; 
  pthread_mutex_init(&emul_low_mutex,NULL);
  pthread_cond_init(&emul_low_cond,NULL);
  emul_low_mutex_var=1; 
  bypass_init(emul_tx_handler,emul_rx_handler);
}

log_mapping level_names[] =
{
    {"emerg", LOG_EMERG},
    {"alert", LOG_ALERT},
    {"crit", LOG_CRIT},
    {"err", LOG_ERR},
    {"warn", LOG_WARNING},
    {"notice", LOG_NOTICE},
    {"info", LOG_INFO},
    {"debug", LOG_DEBUG},
    {"trace", LOG_TRACE},
    {NULL, -1}
};



void help(void) {
  printf("Usage: physim -h -a -e -x transmission_mode -m target_dl_mcs -r(ate_adaptation) -n n_frames -s snr_dB -k ricean_factor -t max_delay -f forgetting factor\n");
  printf("-h provides this help message!\n");
  printf("-a Activates PHY abstraction mode\n");
  printf("-e Activates extended prefix mode\n");
  printf("-m Gives a fixed DL mcs\n");
  printf("-r Activates rate adaptation (DL for now)\n");
  printf("-n Set the number of frames for the simulation\n");
  printf("-s snr_dB set a fixed (average) SNR\n");
  printf("-k Set the Ricean factor (linear)\n");
  printf("-t Set the delay spread (microseconds)\n");
  printf("-f Set the forgetting factor for time-variation\n"); 
  printf("-b Set the number of local eNB\n");
  printf("-u Set the number of local UE\n");
  printf("-M Set the machine ID for Ethernet-based emullation\n");
  printf("-p Set the total number of machine in emulation - valid if M is set\n");
  printf("-g Set multicast group ID (0,1,2,3) - valid if M is set\n");
  printf("-l Set the log level (trace, debug, info, warn, err) only valid for MAC layer\n");
  printf("-c Activate the config generator (OCG) - used in conjunction with openair emu web portal\n");
  printf("-x Set the transmission mode (1,2,6 supported for now)\n");
}

#ifdef XFORMS
void do_forms(FD_phy_procedures_sim *form, LTE_UE_DLSCH **lte_ue_dlsch_vars,LTE_eNB_ULSCH **lte_eNB_ulsch_vars, struct complex **ch,uint32_t ch_len) {

  int32_t j,s,i;
  float I[3600],Q[3600],I2[3600],Q2[3600],I3[300],Q3[300];

  j=0;
  //  printf("rxdataF_comp %p, lte_ue_dlsch_vars[0] %p\n",lte_ue_dlsch_vars[0]->rxdataF_comp[0],lte_ue_dlsch_vars[0]);
  for (s=4;s<12;s++) {
    for(i=0;i<12*12;i++) {
      I[j] = (float)((short*)lte_ue_dlsch_vars[0]->rxdataF_comp[0])[(2*25*12*s)+2*i];
      Q[j] = (float)((short*)lte_ue_dlsch_vars[0]->rxdataF_comp[0])[(2*25*12*s)+2*i+1];
      //      printf("%d (%d): %f,%f : %d,%d\n",j,(25*12*s)+i,I[j],Q[j],lte_ue_dlsch_vars[0]->rxdataF_comp[0][(2*25*12*s)+2*i],lte_ue_dlsch_vars[0]->rxdataF_comp[0][(2*25*12*s)+2*i+1]);
      j++;
    }
    if (s==5)
      s=6;
    else if (s==8)
      s=9;
  }
  if (j>0)
    fl_set_xyplot_data(form->pdsch_constellation,I,Q,j,"","","");


  //fl_set_xyplot_xbounds(form->pdsch_constellation,-800,800);
  //fl_set_xyplot_ybounds(form->pdsch_constellation,-800,800);


  j=0;
  //  printf("rxdataF_comp %p, lte_ue_dlsch_vars[0] %p\n",lte_ue_dlsch_vars[0]->rxdataF_comp[0],lte_ue_dlsch_vars[0]);
  for (s=0;s<12;s++) {
    for(i=0;i<6*12;i++) {
      I2[j] = (float)((short*)lte_eNB_ulsch_vars[0]->rxdataF_comp[0][0])[(2*25*12*s)+2*i];
      Q2[j] = (float)((short*)lte_eNB_ulsch_vars[0]->rxdataF_comp[0][0])[(2*25*12*s)+2*i+1];
      //      printf("%d (%d): %f,%f : %d,%d\n",j,(25*12*s)+i,I[j],Q[j],lte_ue_dlsch_vars[0]->rxdataF_comp[0][(2*25*12*s)+2*i],lte_ue_dlsch_vars[0]->rxdataF_comp[0][(2*25*12*s)+2*i+1]);
      j++;
    }
    if (s==1)
      s=2;
    else if (s==7)
      s=8;
  }
  if (j>0)
    fl_set_xyplot_data(form->pusch_constellation,I2,Q2,j,"","","");

  fl_set_xyplot_xbounds(form->pusch_constellation,-800,800);
  fl_set_xyplot_ybounds(form->pusch_constellation,-800,800);

  for (j=0;j<ch_len;j++) {

    I3[j] = j;
    Q3[j] = 10*log10(ch[0][j].r*ch[0][j].r + ch[0][j].i*ch[0][j].i);
  }

  fl_set_xyplot_data(form->ch00,I3,Q3,ch_len,"","","");
  //fl_set_xyplot_ybounds(form->ch00,-20,20);
}
#endif


void do_DL_sig(double **r_re0,double **r_im0,double **r_re,double **r_im,double **s_re,double **s_im,channel_desc_t *eNB2UE[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX],uint16_t next_slot,double *nf,double snr_dB,uint8_t abstraction_flag,LTE_DL_FRAME_PARMS *frame_parms) {

  mod_sym_t **txdataF;
#ifdef IFFT_FPGA
  int32_t **txdataF2;
  int l;
#endif
  int32_t **txdata,**rxdata;
  
  uint8_t eNB_id=0,UE_id=0,aa;
  double tx_pwr, rx_pwr;
  int32_t rx_pwr2;
  uint32_t i;
  uint32_t slot_offset;
    
  if (abstraction_flag == 0) {
#ifdef IFFT_FPGA
    txdata    = (int32_t **)malloc(2*sizeof(int32_t*));
    txdata[0] = (int32_t *)malloc(OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    txdata[1] = (int32_t *)malloc(OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));

    bzero(txdata[0],OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    bzero(txdata[1],OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    
    txdataF2    = (int32_t **)malloc(2*sizeof(int32_t*));
    txdataF2[0] = (int32_t *)malloc(NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    txdataF2[1] = (int32_t *)malloc(NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    
    bzero(txdataF2[0],NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    bzero(txdataF2[1],NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
#endif
    
    
    for (eNB_id=0;eNB_id<NB_CH_INST;eNB_id++) {
      
      frame_parms = &PHY_vars_eNB_g[eNB_id]->lte_frame_parms;
      
      txdataF = PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars.txdataF[0];
#ifndef IFFT_FPGA
      txdata = PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars.txdata[0];
#endif
      
#ifdef IFFT_FPGA
      
      slot_offset = (next_slot)*(PHY_vars_eNB_g[eNB_id]->lte_frame_parms.N_RB_DL*12)*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7);
      
      //write_output("eNB_txsigF0.m","eNB_txsF0", lte_eNB_common_vars->txdataF[eNB_id][0],300*120,1,4);
      //write_output("eNB_txsigF1.m","eNB_txsF1", lte_eNB_common_vars->txdataF[eNB_id][1],300*120,1,4);
      
      
      // do talbe lookup and write results to txdataF2
      for (aa=0;aa<frame_parms->nb_antennas_tx;aa++) {
	
	l = slot_offset;	
	for (i=0;i<NUMBER_OF_OFDM_CARRIERS*((frame_parms->Ncp==1) ? 6 : 7);i++) 
	  if ((i%512>=1) && (i%512<=150))
	    txdataF2[aa][i] = ((int32_t*)mod_table)[txdataF[aa][l++]];
	  else if (i%512>=362)
	    txdataF2[aa][i] = ((int32_t*)mod_table)[txdataF[aa][l++]];
	  else 
	    txdataF2[aa][i] = 0;
	
      }
      
      for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	if (frame_parms->Ncp == 1)
	  PHY_ofdm_mod(txdataF2[aa],        // input
		       txdata[aa],         // output
		       frame_parms->log2_symbol_size,                // log2_fft_size
		       6,                 // number of symbols
		       frame_parms->nb_prefix_samples,               // number of prefix samples
		       frame_parms->twiddle_ifft,  // IFFT twiddle factors
		       frame_parms->rev,           // bit-reversal permutation
		       CYCLIC_PREFIX);
	else {
	  normal_prefix_mod(txdataF2[aa],txdata[aa],7,frame_parms);
	}
      }
	
#else //IFFT_FPGA
      
	slot_offset = (next_slot)*(frame_parms->ofdm_symbol_size)*((frame_parms->Ncp==1) ? 6 : 7);
      
      
	for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	  if (frame_parms->Ncp == 1)
	    PHY_ofdm_mod(&txdataF[aa][slot_offset],        // input
			 txdata[aa],         // output
			 frame_parms->log2_symbol_size,                // log2_fft_size
			 6,                 // number of symbols
			 frame_parms->nb_prefix_samples,               // number of prefix samples
			 frame_parms->twiddle_ifft,  // IFFT twiddle factors
			 frame_parms->rev,           // bit-reversal permutation
			 CYCLIC_PREFIX);
	  else {
	    normal_prefix_mod(&txdataF[aa][slot_offset],
			      txdata[aa],
			      7,
			      frame_parms);
	  }
	}  
#endif //IFFT_FPGA
    }
  }

  for (UE_id=0;UE_id<NB_UE_INST;UE_id++) {
    
    if (abstraction_flag!=0) {
      for (eNB_id=0;eNB_id<NB_CH_INST;eNB_id++)
	random_channel(eNB2UE[eNB_id][UE_id]);
    }
    else {
      // Compute RX signal for UE = UE_id
      for (i=0;i<(frame_parms->samples_per_tti>>1);i++) {
	for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
	  r_re[aa][i]=0.0;
	  r_im[aa][i]=0.0;
	  
	}
      }
      // Compute RX signal for UE = UE_id
      for (eNB_id=0;eNB_id<NB_CH_INST;eNB_id++) {
	frame_parms = &PHY_vars_eNB_g[eNB_id]->lte_frame_parms;
	tx_pwr = dac_fixed_gain(s_re,
				s_im,
				txdata,
				0, //slot_offset,
				frame_parms->nb_antennas_tx,
				frame_parms->samples_per_tti>>1,
				14,
				
				18); 
#ifdef DEBUG_SIM
	printf("[SIM][DL] tx_pwr eNB %d %f dB for slot %d (subframe %d)\n",eNB_id,10*log10(tx_pwr),next_slot,next_slot>>1);
#endif
	//     printf("channel for slot %d (subframe %d)\n",next_slot,next_slot>>1);
	
	
	
	
	multipath_channel(eNB2UE[eNB_id][UE_id],s_re,s_im,r_re0,r_im0,
			  frame_parms->samples_per_tti>>1,0);
	
	rx_pwr = signal_energy_fp2(eNB2UE[eNB_id][UE_id]->ch[0],eNB2UE[eNB_id][UE_id]->channel_length);
#ifdef DEBUG_SIM
	printf("[SIM][DL] Channel eNB %d => UE %d : gain %f dB\n",eNB_id,UE_id,10*log10(rx_pwr));  
	printf("[SIM][DL] Channel eNB %d => UE %d : path_loss %f dB\n",eNB_id,UE_id,eNB2UE[eNB_id][UE_id]->path_loss_dB);
#endif
	rx_pwr = signal_energy_fp(r_re0,r_im0,frame_parms->nb_antennas_rx,frame_parms->samples_per_tti>>1,0);
#ifdef DEBUG_SIM      
	printf("[SIM][DL] UE %d : CH out %f dB for slot %d (subframe %d)\n",UE_id,10*log10(rx_pwr),next_slot,next_slot>>1);  
#endif      
	
	if (eNB2UE[eNB_id][UE_id]->first_run == 1)
	  eNB2UE[eNB_id][UE_id]->first_run = 0;
	
	
	// RF model
#ifdef DEBUG_SIM
	printf("[SIM][DL] UE_id %d: rx_gain %d dB for slot %d (subframe %d)\n",UE_id,PHY_vars_UE_g[UE_id]->rx_total_gain_dB,next_slot,next_slot>>1);      
#endif
	rf_rx(r_re0,
	      r_im0,
	      NULL,
	      NULL,
	      0,
	      frame_parms->nb_antennas_rx,
	      frame_parms->samples_per_tti>>1,
	      (eNB_id==0) ? (1.0/7.68e6 * 1e9) : 1e9,  // sampling time (ns)
	      0.0,               // freq offset (Hz) (-20kHz..20kHz)
	      0.0,               // drift (Hz) NOT YET IMPLEMENTED
	      nf,                // noise_figure NOT YET IMPLEMENTED
	      (double)PHY_vars_UE_g[UE_id]->rx_total_gain_dB - 66.227,   // rx_gain (dB) (66.227 = 20*log10(pow2(11)) = gain from the adc that will be applied later)
	      200,               // IP3_dBm (dBm)
	      &eNB2UE[eNB_id][UE_id]->ip,               // initial phase
	      30.0e3,            // pn_cutoff (kHz)
	      -500.0,            // pn_amp (dBc) default: 50
	      0.0,               // IQ imbalance (dB),
	      0.0);              // IQ phase imbalance (rad)
	
	for (i=0;i<(frame_parms->samples_per_tti>>1);i++) {
	  for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
	    r_re[aa][i]+=r_re0[aa][i]; 
	    r_im[aa][i]+=r_im0[aa][i]; 
	    
	  }
	}
	
      }
      
      rx_pwr = signal_energy_fp(r_re,r_im,frame_parms->nb_antennas_rx,frame_parms->samples_per_tti>>1,0);
#ifdef DEBUG_SIM    
      printf("[SIM][DL] UE %d : ADC in %f dB for slot %d (subframe %d)\n",UE_id,10*log10(rx_pwr),next_slot,next_slot>>1);  
#endif    
      rxdata = PHY_vars_UE_g[UE_id]->lte_ue_common_vars.rxdata;
      
      slot_offset = (next_slot)*(frame_parms->samples_per_tti>>1);
      
      
      
      adc(r_re,
	  r_im,
	  0,
	  slot_offset,
	  rxdata,
	  frame_parms->nb_antennas_rx,
	  frame_parms->samples_per_tti>>1,
	  12);
      
      rx_pwr2 = signal_energy(rxdata[0]+slot_offset,frame_parms->samples_per_tti>>1);
#ifdef DEBUG_SIM    
      printf("[SIM][DL] rx_pwr (ADC out) %f dB (%d) for slot %d (subframe %d)\n",10*log10((double)rx_pwr2),rx_pwr2,next_slot,next_slot>>1);  
#endif
    } // abstraction_flag==0    
  } // UE_index loop
 
  
#ifdef IFFT_FPGA
  free(txdataF2[0]);
  free(txdataF2[1]);
  free(txdataF2);
  free(txdata[0]);
  free(txdata[1]);
  free(txdata);
#endif 
  
}

void do_UL_sig(double **r_re0,double **r_im0,double **r_re,double **r_im,double **s_re,double **s_im,channel_desc_t *UE2eNB[NUMBER_OF_UE_MAX][NUMBER_OF_eNB_MAX],uint16_t next_slot,double *nf,double snr_dB,uint8_t abstraction_flag,LTE_DL_FRAME_PARMS *frame_parms) {

  mod_sym_t **txdataF;
#ifdef IFFT_FPGA
  int32_t **txdataF2;
  int l;
#endif
  int32_t **txdata,**rxdata;

  uint8_t UE_id=0,eNB_id=0,aa;
  double tx_pwr, rx_pwr;
  int32_t rx_pwr2;
  uint32_t i;
  uint32_t slot_offset;

  if (abstraction_flag==0) {
#ifdef IFFT_FPGA
    txdata    = (int32_t **)malloc(2*sizeof(int32_t*));
    txdata[0] = (int32_t *)malloc(OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    txdata[1] = (int32_t *)malloc(OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    
    bzero(txdata[0],OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    bzero(txdata[1],OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[0]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    
    txdataF2    = (int32_t **)malloc(2*sizeof(int32_t*));
    txdataF2[0] = (int32_t *)malloc(NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    txdataF2[1] = (int32_t *)malloc(NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    
    bzero(txdataF2[0],NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
    bzero(txdataF2[1],NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7)*sizeof(int32_t));
#endif
  }  

  if (abstraction_flag==0) {
    for (UE_id=0;UE_id<NB_UE_INST;UE_id++){
      frame_parms = &PHY_vars_UE_g[UE_id]->lte_frame_parms;
      
      
      txdataF = PHY_vars_UE_g[UE_id]->lte_ue_common_vars.txdataF;
      //    printf("UE %d txdataF[0] %p\n",UE_id,txdataF[0]);
#ifndef IFFT_FPGA
      txdata = PHY_vars_UE_g[UE_id]->lte_ue_common_vars.txdata;
#endif
      
#ifdef IFFT_FPGA
      
      slot_offset = (next_slot)*(PHY_vars_UE_g[UE_id]->lte_frame_parms.N_RB_DL*12)*((PHY_vars_eNB_g[UE_id]->lte_frame_parms.Ncp==1) ? 6 : 7);
      
      //write_output("eNB_txsigF0.m","eNB_txsF0", lte_eNB_common_vars->txdataF[eNB_id][0],300*120,1,4);
      //write_output("eNB_txsigF1.m","eNB_txsF1", lte_eNB_common_vars->txdataF[eNB_id][1],300*120,1,4);
      
      
      // do talbe lookup and write results to txdataF2
      for (aa=0;aa<frame_parms->nb_antennas_tx;aa++) {
	
	l = slot_offset;	
	for (i=0;i<NUMBER_OF_OFDM_CARRIERS*((frame_parms->Ncp==1) ? 6 : 7);i++) 
	  if ((i%512>=1) && (i%512<=150))
	    txdataF2[aa][i] = ((int32_t*)mod_table)[txdataF[aa][l++]];
	  else if (i%512>=362)
	    txdataF2[aa][i] = ((int32_t*)mod_table)[txdataF[aa][l++]];
	  else 
	    txdataF2[aa][i] = 0;
	
      }
      
      for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) 
	if (frame_parms->Ncp==1)
	  PHY_ofdm_mod(txdataF2[aa],        // input
		       txdata[aa],         // output
		       frame_parms->log2_symbol_size,                // log2_fft_size
		       6,                 // number of symbols
		       frame_parms->nb_prefix_samples,               // number of prefix samples
		       frame_parms->twiddle_ifft,  // IFFT twiddle factors
		       frame_parms->rev,           // bit-reversal permutation
		       CYCLIC_PREFIX);
	else {
	  normal_prefix_mod(txdataF2[aa],txdata[aa],7,frame_parms);
	}
      if ((next_slot==8) && (mac_xface->frame==4)) {
	write_output("UEtxsigF20.m","txsF20", txdataF2[0],NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7),2,1);
      }
#else //IFFT_FPGA
      
      slot_offset = (next_slot)*(frame_parms->ofdm_symbol_size)*((frame_parms->Ncp==1) ? 6 : 7);
      
      
      for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
	if (frame_parms->Ncp==1) {
	  PHY_ofdm_mod(&txdataF[aa][slot_offset],        // input
		       txdata[aa],         // output
		       frame_parms->log2_symbol_size,                // log2_fft_size
		       6,                 // number of symbols
		       frame_parms->nb_prefix_samples,               // number of prefix samples
		       frame_parms->twiddle_ifft,  // IFFT twiddle factors
		       frame_parms->rev,           // bit-reversal permutation
		       CYCLIC_PREFIX);
	}
	else {
	  normal_prefix_mod(&txdataF[aa][slot_offset],
			    txdata[aa],
			    7,
			    frame_parms);

	}
      }
      if ((next_slot==8) && (mac_xface->frame==4)) {
	write_output("UEtxsigF0.m","txsF0", &txdataF[0][slot_offset],NUMBER_OF_OFDM_CARRIERS*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7),2,1);
      }
  
#endif //IFFT_FPGA
      if ((next_slot==8) && (mac_xface->frame==4)) {
	write_output("UEtxsig0.m","txs0", txdata[0],OFDM_SYMBOL_SIZE_SAMPLES*((PHY_vars_eNB_g[eNB_id]->lte_frame_parms.Ncp==1) ? 6 : 7),1,1);
      }

    }  // UE_id TX loop
  } // abstraction_flag

 
  for (eNB_id=0;eNB_id<NB_CH_INST;eNB_id++) {

    if (abstraction_flag!=0) {
      for (UE_id=0;UE_id<NB_UE_INST;UE_id++)
	random_channel(UE2eNB[UE_id][eNB_id]);
    }
    else {
    // Clear RX signal for eNB = eNB_id
      for (i=0;i<(frame_parms->samples_per_tti>>1);i++) {
	for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
	  r_re[aa][i]=0.0;
	  r_im[aa][i]=0.0;
	  
	}
      }
      
      // Compute RX signal for eNB = eNB_id
      for (UE_id=0;UE_id<NB_UE_INST;UE_id++){
	
	txdataF = PHY_vars_UE_g[UE_id]->lte_ue_common_vars.txdataF;
	//      printf("UE %d txdataF[0] %p\n",UE_id,txdataF[0]);
#ifndef IFFT_FPGA
	txdata = PHY_vars_UE_g[UE_id]->lte_ue_common_vars.txdata;
#endif      
	frame_parms = &PHY_vars_UE_g[UE_id]->lte_frame_parms;
	tx_pwr = dac_fixed_gain(s_re,
				s_im,
				txdata,
				0, //slot_offset,
				frame_parms->nb_antennas_tx,
				frame_parms->samples_per_tti>>1,
				14,
				18); 
#ifdef DEBUG_SIM
	printf("[SIM][UL] UE %d txdataF %p tx_pwr %f dB for slot %d (subframe %d)\n",UE_id,&txdataF[0][slot_offset],10*log10(tx_pwr),next_slot,next_slot>>1);
#endif
	
	
	rx_pwr = signal_energy_fp(s_re,s_im,frame_parms->nb_antennas_rx,frame_parms->samples_per_tti>>1,0);
#ifdef DEBUG_SIM    
	printf("[SIM][UL] CH %d : CH in %f dB for slot %d (subframe %d), sptti %d\n",eNB_id,10*log10(rx_pwr),next_slot,next_slot>>1,frame_parms->samples_per_tti);  
#endif
	/*
	uint8_t aarx,aatx,k;
	for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	printf("BMP(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	*/ 
      
	multipath_channel(UE2eNB[UE_id][eNB_id],s_re,s_im,r_re0,r_im0,
			  frame_parms->samples_per_tti>>1,0);
	/*
	for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	printf("AMP(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	*/

	rx_pwr = signal_energy_fp2(UE2eNB[UE_id][eNB_id]->ch[0],UE2eNB[UE_id][eNB_id]->channel_length);
#ifdef DEBUG_SIM
	printf("[SIM][UL] Channel UE %d => eNB %d : %f dB\n",UE_id,eNB_id,10*log10(rx_pwr));  
#endif
	rx_pwr = signal_energy_fp(r_re0,r_im0,frame_parms->nb_antennas_rx,frame_parms->samples_per_tti>>1,0);
#ifdef DEBUG_SIM    
	printf("[SIM][UL] CH %d : CH out %f dB for slot %d (subframe %d), sptti %d\n",eNB_id,10*log10(rx_pwr),next_slot,next_slot>>1,frame_parms->samples_per_tti);  
#endif
	if (UE2eNB[UE_id][eNB_id]->first_run == 1)
	  UE2eNB[UE_id][eNB_id]->first_run = 0;
      
      
	// RF model
	
	
	rf_rx(r_re0,
	      r_im0,
	      NULL,
	      NULL,
	      0,
	      frame_parms->nb_antennas_rx,
	      frame_parms->samples_per_tti>>1,
	      (UE_id==0) ? (1.0/7.68e6 * 1e9) : 1e9,  // sampling time (ns) + set noise bandwidth to 0 for UE>0 (i.e. no noise except for first UE)
	      0.0,               // freq offset (Hz) (-20kHz..20kHz)
	      0.0,               // drift (Hz) NOT YET IMPLEMENTED
	      nf,                // noise_figure NOT YET IMPLEMENTED
	      (double)PHY_vars_eNB_g[eNB_id]->rx_total_gain_eNB_dB - 66.227,   // rx_gain (dB) (66.227 = 20*log10(pow2(11)) = gain from the adc that will be applied later)
	      200,               // IP3_dBm (dBm)
	      &UE2eNB[UE_id][eNB_id]->ip,               // initial phase
	      30.0e3,            // pn_cutoff (kHz)
	      -500.0,            // pn_amp (dBc) default: 50
	      0.0,               // IQ imbalance (dB),
	      0.0);              // IQ phase imbalance (rad)
	
	for (i=0;i<(frame_parms->samples_per_tti>>1);i++) {
	  for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
	    r_re[aa][i]+=r_re0[aa][i]; 
	    r_im[aa][i]+=r_im0[aa][i]; 
	    
	  }
	}
	rx_pwr = signal_energy_fp(r_re,r_im,frame_parms->nb_antennas_rx,frame_parms->samples_per_tti>>1,0);
#ifdef DEBUG_SIM    
	printf("[SIM][UL] rx_pwr (ADC in) %f dB for slot %d (subframe %d)\n",10*log10(rx_pwr),next_slot,next_slot>>1);  
#endif
      }
      

    
    
      rxdata = PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars.rxdata[0];
    
      slot_offset = (next_slot)*(frame_parms->samples_per_tti>>1);
      
      
      
      adc(r_re,
	  r_im,
	  0,
	  slot_offset,
	  rxdata,
	  frame_parms->nb_antennas_rx,
	  frame_parms->samples_per_tti>>1,
	  12);
      
      rx_pwr2 = signal_energy(rxdata[0]+slot_offset,frame_parms->samples_per_tti>>1);
#ifdef DEBUG_SIM    
      printf("[SIM][UL] eNB %d rx_pwr (ADC out) %f dB (%d) for slot %d (subframe %d)\n",eNB_id,10*log10((double)rx_pwr2),rx_pwr2,next_slot,next_slot>>1);  
#endif    
      
    } // abstraction_flag==0
  }
  
#ifdef IFFT_FPGA
  if (abstraction_flag==0){
    free(txdataF2[0]);
    free(txdataF2[1]);
    free(txdataF2);
    free(txdata[0]);
    free(txdata[1]);
    free(txdata);
  }
#endif 
}


int main(int argc, char **argv) {

 
  char c;
  int32_t i,j;
  double **s_re,**s_im,**r_re,**r_im,**r_re0,**r_im0;
  double amps[8] = {0.3868472 , 0.3094778 , 0.1547389 , 0.0773694 , 0.0386847 , 0.0193424 , 0.0096712 , 0.0038685};
  double aoa=.03,ricean_factor=.001,Td=1.0,forgetting_factor=.5,maxDoppler=0;
  uint8_t channel_length,nb_taps=8;


  int32_t n_frames,n_errors;

  int32_t slot,last_slot, next_slot;

  double nf[2] = {3.0,3.0}; //currently unused
  double snr_dB;



  uint8_t target_dl_mcs=4;
  uint8_t target_ul_mcs=2;
  uint8_t rate_adaptation_flag;
  uint8_t transmission_mode;
  uint8_t abstraction_flag=0,ethernet_flag=0;
  uint16_t ethernet_id=0;
  uint8_t extended_prefix_flag=0;
  int32_t UE_id,eNB_id,ret; 
#ifdef EMOS
  fifo_dump_emos emos_dump;
#endif
  //uint8_t nb_ue_local=1,nb_ue_remote=0;
  //uint8_t nb_eNB_local=1,nb_eNB_remote=0;
  //uint8_t first_eNB_local=0,first_UE_local=0, nb_machine=0;
 
  char * g_log_level="trace"; // by default global log level is set to trace 
  lte_subframe_t direction;

  int OCG_flag =0;
  OAI_Emulation * emulation_scen;

  channel_desc_t *eNB2UE[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX];
  channel_desc_t *UE2eNB[NUMBER_OF_UE_MAX][NUMBER_OF_eNB_MAX];

#ifdef XFORMS
  FD_phy_procedures_sim *form[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX];
  char title[255];
#endif
  LTE_DL_FRAME_PARMS *frame_parms;
  
  RRC_CONNECTION_FLAG = 1;
  
  //default parameters
  oai_emulation.info.is_primary_master=0;
  oai_emulation.info.master_list=0;
  oai_emulation.info.nb_ue_remote=0;
  oai_emulation.info.nb_enb_remote=0;
  oai_emulation.info.first_ue_local=0;
  oai_emulation.info.first_enb_local=0;
  oai_emulation.info.master_id=0;
  oai_emulation.info.nb_master =0;
  oai_emulation.info.nb_ue_local= 1;
  oai_emulation.info.nb_enb_local= 1;
  oai_emulation.info.ethernet_flag=0;
  oai_emulation.info.multicast_group=0;
  
  transmission_mode = 1;
  target_dl_mcs = 0;
  rate_adaptation_flag = 1;
  n_frames = 100;
  snr_dB = 30;

  while ((c = getopt (argc, argv, "haect:k:x:m:rn:s:f:u:b:M:p:g:l")) != -1)

    {
       switch (c)
	{
	case 'h':
	  help();
	  exit(1);
	case 'x':
	  transmission_mode = atoi(optarg);
	  break;
	case 'm':
	  target_dl_mcs = atoi(optarg);
	  break;
	case 'r':
	  rate_adaptation_flag = 1;
	  break;
	case 'n':
	  n_frames = atoi(optarg);
	  break;
	case 's':
	  snr_dB = atoi(optarg);
	  break;
	case 'k': 
	  ricean_factor = atof(optarg);
	  break;
	case 't':
    	  Td = atof(optarg);
	  break;
	case 'f':
	  forgetting_factor = atof(optarg);
	  break;
	case 'u':
	  oai_emulation.info.nb_ue_local = atoi(optarg);
	  break;
	  //	case 'U':
	  //nb_ue_remote = atoi(optarg);
	  //break;
	case 'b':
	  oai_emulation.info.nb_enb_local = atoi(optarg);
	  break;
	  //	case 'B':
	  // nb_eNB_remote = atoi(optarg);
	  //break;
	case 'a':
	  abstraction_flag=1;
	  break;
	case 'p':
	  oai_emulation.info.nb_master = atoi(optarg);
	  break;
	case 'M':
	  abstraction_flag=1;
	  ethernet_flag=1;
	  ethernet_id = atoi(optarg);
	  oai_emulation.info.master_id=ethernet_id;
	  oai_emulation.info.ethernet_flag=1;
	  break;
	case 'e':
	  extended_prefix_flag=1;
	  break;
	case 'l':
	  g_log_level=optarg;
	  break;
	case 'c':
	  OCG_flag=1;
	  break;
	case 'g':
	  oai_emulation.info.multicast_group=atoi(optarg);
	  break;	
	default:
	  help ();
	  exit (-1);
	  break;
	}
    }
 //initialize the log generator 
  logInit(map_str_to_int(level_names, g_log_level));
  LOG_T(LOG,"global log level is set to %s \n",g_log_level );
  
  //set_comp_log(EMU,  LOG_INFO, LOG_MED);
#ifdef OCG  
  if (OCG_flag==1){ // activate OCG
    printf("start\n");
    emulation_scen= OCG_main();
    LOG_I(MAC,"the area is x %f y %f option %s\n",
	  emulation_scen->envi_config.area.x,
		  emulation_scen->envi_config.area.y, emulation_scen->topo_config.eNB_topology.selected_option);
      abstraction_flag=1;
      extended_prefix_flag=1;

      oai_emulation.info.nb_ue_local  = emulation_scen->topo_config.number_of_UE;

      if (!strcmp(emulation_scen->topo_config.eNB_topology.selected_option, "random")) {
         oai_emulation.info.nb_enb_local = emulation_scen->topo_config.eNB_topology.totally_random.number_of_eNB;
      } else if (!strcmp(emulation_scen->topo_config.eNB_topology.selected_option, "hexagonal")) {
	oai_emulation.info.nb_enb_local = emulation_scen->topo_config.eNB_topology.hexagonal.number_of_cells;
      } else if (!strcmp(emulation_scen->topo_config.eNB_topology.selected_option, "grid")) {
	oai_emulation.info.nb_enb_local = emulation_scen->topo_config.eNB_topology.grid.x * emulation_scen->topo_config.eNB_topology.grid.y;
      } 
      n_frames  =  (int) emulation_scen->emu_config.emu_time * 60 * 100;
      transmission_mode = 1;
      //set_comp_log(EMU,  LOG_INFO, LOG_MED);
      //set_comp_log(MAC,  LOG_INFO, LOG_MED);
      //set_comp_log(RLC,  LOG_INFO, LOG_MED);
      
      LOG_I(OCG," ue local %d enb local %d frame %d\n",   nb_ue_local,   nb_eNB_local, n_frames );
   }
#endif    
  //if ( (ethernet_flag==1) || (abstraction_flag==1) )
  ret=netlink_init();
  
  if (ethernet_flag==1){
    oai_emulation.info.master[oai_emulation.info.master_id].nb_ue=oai_emulation.info.nb_ue_local;
    oai_emulation.info.master[oai_emulation.info.master_id].nb_enb=oai_emulation.info.nb_enb_local;

    if(!oai_emulation.info.master_id) 
      oai_emulation.info.is_primary_master=1;
    j=1;
    for(i=0;i<oai_emulation.info.nb_master;i++){
      if(i!=oai_emulation.info.master_id)
	oai_emulation.info.master_list=oai_emulation.info.master_list+j;
      LOG_T(EMU, "Index of master id i=%d  MASTER_LIST %d\n",i,oai_emulation.info.master_list);
      j*=2;
    }
    LOG_T(EMU,"nb_ue_local %d nb_enb_local %d nb_master %d master id %d\n", 
	  oai_emulation.info.nb_ue_local, 
	  oai_emulation.info.nb_enb_local,
	  oai_emulation.info.nb_master,
	  oai_emulation.info.master_id);
    
    //ret=netlink_init();
    init_bypass();
    
    while (emu_tx_status != SYNCED_TRANSPORT ) {
      LOG_T(EMU, " Waiting for EMU Transport to be synced\n"); 
      emu_transport_sync();//emulation_tx_rx();
    }
  }// ethernet flag

  NB_UE_INST = oai_emulation.info.nb_ue_local + oai_emulation.info.nb_ue_remote;
  NB_CH_INST = oai_emulation.info.nb_enb_local + oai_emulation.info.nb_enb_remote;
   
   
  printf("Running with mode %d, target dl_mcs %d, rate adaptation %d, nframes %d\n",
  	 transmission_mode,target_dl_mcs,rate_adaptation_flag,n_frames);

  channel_length = (uint8_t) (11+2*BW*Td);

  //PHY_vars = malloc(sizeof(PHY_VARS));
  //  PHY_VARS_eNB *PHY_vars_eNB; 

  PHY_vars_eNB_g = malloc(NB_CH_INST*sizeof(PHY_VARS_eNB*));
  for (eNB_id=0; eNB_id<NB_CH_INST;eNB_id++){ 
    PHY_vars_eNB_g[eNB_id] = malloc(sizeof(PHY_VARS_eNB));
    PHY_vars_eNB_g[eNB_id]->Mod_id=eNB_id;
  }
  //  PHY_VARS_UE *PHY_vars_UE; 
  PHY_vars_UE_g = malloc(NB_UE_INST*sizeof(PHY_VARS_UE*));
  for (UE_id=0; UE_id<NB_UE_INST;UE_id++){ // begin navid
    PHY_vars_UE_g[UE_id] = malloc(sizeof(PHY_VARS_UE));
    PHY_vars_UE_g[UE_id]->Mod_id=UE_id; 
  }// end navid

  //PHY_config = malloc(sizeof(PHY_CONFIG));
  mac_xface = malloc(sizeof(MAC_xface));

  frame_parms = malloc(sizeof(LTE_DL_FRAME_PARMS));
  frame_parms->frame_type         = 1;
  frame_parms->tdd_config         = 3;
  frame_parms->N_RB_DL            = 25;
  frame_parms->N_RB_UL            = 25;
  frame_parms->phich_config_common.phich_resource = oneSixth;
  frame_parms->Ncp                = extended_prefix_flag;
  frame_parms->Nid_cell           = 0;
  frame_parms->nushift            = 0;
  frame_parms->nb_antennas_tx     = 2;
  frame_parms->nb_antennas_rx     = 2;
  frame_parms->mode1_flag = (transmission_mode == 1) ? 1 : 0;

  init_frame_parms(frame_parms,1);
  //copy_lte_parms_to_phy_framing(frame_parms, &(PHY_config->PHY_framing));
  phy_init_top(frame_parms);

  frame_parms->twiddle_fft      = twiddle_fft;
  frame_parms->twiddle_ifft     = twiddle_ifft;
  frame_parms->rev              = rev;
  // navid 
  frame_parms->tdd_config = 3;

  phy_init_lte_top(frame_parms);

  // init all eNB vars

  for (eNB_id=0;eNB_id<NB_CH_INST;eNB_id++) {
    memcpy(&(PHY_vars_eNB_g[eNB_id]->lte_frame_parms), frame_parms, sizeof(LTE_DL_FRAME_PARMS));
    phy_init_lte_eNB(&PHY_vars_eNB_g[eNB_id]->lte_frame_parms,
		     &PHY_vars_eNB_g[eNB_id]->lte_eNB_common_vars,
		     PHY_vars_eNB_g[eNB_id]->lte_eNB_ulsch_vars,
		     0,
		     PHY_vars_eNB_g[eNB_id],
		     0,
		     0,
		     abstraction_flag);
    
    /*
      PHY_vars_eNB_g[eNB_id]->dlsch_eNB[0] = (LTE_eNB_DLSCH_t**) malloc16(NUMBER_OF_UE_MAX*sizeof(LTE_eNB_DLSCH_t*));
      PHY_vars_eNB_g[eNB_id]->dlsch_eNB[1] = (LTE_eNB_DLSCH_t**) malloc16(NUMBER_OF_UE_MAX*sizeof(LTE_eNB_DLSCH_t*));
      PHY_vars_eNB_g[eNB_id]->ulsch_eNB = (LTE_eNB_ULSCH_t**) malloc16((1+NUMBER_OF_UE_MAX)*sizeof(LTE_eNB_ULSCH_t*));
    */

    for (i=0;i<NUMBER_OF_UE_MAX;i++) {
      for (j=0;j<2;j++) {
	PHY_vars_eNB_g[eNB_id]->dlsch_eNB[i][j] = new_eNB_dlsch(1,8,abstraction_flag);
	if (!PHY_vars_eNB_g[eNB_id]->dlsch_eNB[i][j]) {
	  msg("Can't get eNB dlsch structures\n");
	  exit(-1);
	}
	else {
	  msg("dlsch_eNB[%d][%d] => %p\n",i,j,PHY_vars_eNB_g[eNB_id]->dlsch_eNB[i][j]);
	  PHY_vars_eNB_g[eNB_id]->dlsch_eNB[i][j]->rnti=0;
	}
      }
      PHY_vars_eNB_g[eNB_id]->ulsch_eNB[1+i] = new_eNB_ulsch(3,abstraction_flag);
      if (!PHY_vars_eNB_g[eNB_id]->ulsch_eNB[1+i]) {
	msg("Can't get eNB ulsch structures\n");
	exit(-1);
      }

    }

    // ULSCH for RA
    PHY_vars_eNB_g[eNB_id]->ulsch_eNB[0] = new_eNB_ulsch(3,abstraction_flag);
    if (!PHY_vars_eNB_g[eNB_id]->ulsch_eNB[0]) {
      msg("Can't get eNB ulsch structures\n");
      exit(-1);
    }

    PHY_vars_eNB_g[eNB_id]->dlsch_eNB_SI  = new_eNB_dlsch(1,1,abstraction_flag);
    printf("eNB %d : SI %p\n",eNB_id,PHY_vars_eNB_g[eNB_id]->dlsch_eNB_SI);
    PHY_vars_eNB_g[eNB_id]->dlsch_eNB_ra  = new_eNB_dlsch(1,1,abstraction_flag);
    printf("eNB %d : RA %p\n",eNB_id,PHY_vars_eNB_g[eNB_id]->dlsch_eNB_ra);

    PHY_vars_eNB_g[eNB_id]->rx_total_gain_eNB_dB=150;
  }

  // init all UE vars

  for (UE_id=0; UE_id<NB_UE_INST;UE_id++){ 
    memcpy(&(PHY_vars_UE_g[UE_id]->lte_frame_parms), frame_parms, sizeof(LTE_DL_FRAME_PARMS));
    
    phy_init_lte_ue(&PHY_vars_UE_g[UE_id]->lte_frame_parms,
		    &PHY_vars_UE_g[UE_id]->lte_ue_common_vars,
		    PHY_vars_UE_g[UE_id]->lte_ue_dlsch_vars,
		    PHY_vars_UE_g[UE_id]->lte_ue_dlsch_vars_SI,
		    PHY_vars_UE_g[UE_id]->lte_ue_dlsch_vars_ra,
		    PHY_vars_UE_g[UE_id]->lte_ue_pbch_vars,
		    PHY_vars_UE_g[UE_id]->lte_ue_pdcch_vars,
		    PHY_vars_UE_g[UE_id],
		    abstraction_flag);

    /*
      PHY_vars_UE_g[UE_id]->dlsch_ue[0] = (LTE_UE_DLSCH_t**) malloc16(NUMBER_OF_eNB_MAX*sizeof(LTE_UE_DLSCH_t*));
      PHY_vars_UE_g[UE_id]->dlsch_ue[1] = (LTE_UE_DLSCH_t**) malloc16(NUMBER_OF_eNB_MAX*sizeof(LTE_UE_DLSCH_t*));
    
      PHY_vars_UE_g[UE_id]->ulsch_ue = (LTE_UE_ULSCH_t**) malloc16(NUMBER_OF_eNB_MAX*sizeof(LTE_UE_ULSCH_t*));
    
      PHY_vars_UE_g[UE_id]->dlsch_ue_SI = (LTE_UE_DLSCH_t**) malloc16(NUMBER_OF_eNB_MAX*sizeof(LTE_UE_DLSCH_t*));
      PHY_vars_UE_g[UE_id]->dlsch_ue_ra = (LTE_UE_DLSCH_t**) malloc16(NUMBER_OF_eNB_MAX*sizeof(LTE_UE_DLSCH_t*));
    */

    for (i=0;i<NUMBER_OF_eNB_MAX;i++) {
      for (j=0;j<2;j++) {
	PHY_vars_UE_g[UE_id]->dlsch_ue[i][j]  = new_ue_dlsch(1,8,abstraction_flag);
	if (!PHY_vars_UE_g[UE_id]->dlsch_ue[i][j]) {
	  msg("Can't get ue dlsch structures\n");
	  exit(-1);
	}
	else
	  msg("dlsch_ue[%d][%d] => %p\n",UE_id,i,PHY_vars_UE_g[UE_id]->dlsch_ue[i][j]);//navid
      }
      
      
      PHY_vars_UE_g[UE_id]->ulsch_ue[i]  = new_ue_ulsch(3,abstraction_flag);
      if (!PHY_vars_UE_g[UE_id]->ulsch_ue[i]) {
	msg("Can't get ue ulsch structures\n");
	exit(-1);
      }
      
      PHY_vars_UE_g[UE_id]->dlsch_ue_SI[i]  = new_ue_dlsch(1,1,abstraction_flag);
      PHY_vars_UE_g[UE_id]->dlsch_ue_ra[i]  = new_ue_dlsch(1,1,abstraction_flag);
    }
  }

  // do the srs init
  for (UE_id=0;UE_id<NB_UE_INST;UE_id++) {

    PHY_vars_UE_g[UE_id]->lte_frame_parms.soundingrs_ul_config_common.srs_BandwidthConfig = 2;

    PHY_vars_UE_g[UE_id]->soundingrs_ul_config_dedicated.srs_Bandwidth = 0;
    PHY_vars_UE_g[UE_id]->soundingrs_ul_config_dedicated.transmissionComb = 0;
    PHY_vars_UE_g[UE_id]->soundingrs_ul_config_dedicated.freqDomainPosition = 0;

    // choose configurations 1 (UE0 SF 1), 3 (UE1 SF 2), 5 (UE2 SF 3)
    PHY_vars_UE_g[UE_id]->soundingrs_ul_config_dedicated.srs_ConfigIndex = 1+(UE_id<<1);
    if (UE_id>=3) {
      printf("This SRS config will only work for 3 users! \n");
     
    }

    /// This is for 3 users (SRS can be transmitted in SF 1,2,3,4
    PHY_vars_UE_g[UE_id]->lte_frame_parms.soundingrs_ul_config_common.srs_SubframeConfig = 7;
    
    for (eNB_id=0;eNB_id<NB_CH_INST;eNB_id++) {

      PHY_vars_eNB_g[eNB_id]->lte_frame_parms.soundingrs_ul_config_common.srs_BandwidthConfig = 2;
      PHY_vars_eNB_g[eNB_id]->lte_frame_parms.soundingrs_ul_config_common.srs_SubframeConfig = 7;
      PHY_vars_eNB_g[eNB_id]->soundingrs_ul_config_dedicated[UE_id].srs_Bandwidth = 0;
      PHY_vars_eNB_g[eNB_id]->soundingrs_ul_config_dedicated[UE_id].transmissionComb = 0;
      PHY_vars_eNB_g[eNB_id]->soundingrs_ul_config_dedicated[UE_id].freqDomainPosition = 0;
      // choose configurations 1 (UE0 SF 1), 3 (UE1 SF 2), 5 (UE2 SF 3)
      PHY_vars_eNB_g[eNB_id]->soundingrs_ul_config_dedicated[UE_id].srs_ConfigIndex = 1+(UE_id<<1);
    }
  }


  //  init_transport_channels(transmission_mode);

  if (abstraction_flag==0) {
    /*
#ifdef IFFT_FPGA
    txdata    = (int32_t **)malloc16(2*sizeof(int32_t*));
    txdata[0] = (int32_t *)malloc16(FRAME_LENGTH_BYTES);
    txdata[1] = (int32_t *)malloc16(FRAME_LENGTH_BYTES);
    
    bzero(txdata[0],FRAME_LENGTH_BYTES);
    bzero(txdata[1],FRAME_LENGTH_BYTES);
    
    txdataF2    = (int32_t **)malloc16(2*sizeof(int32_t*));
    txdataF2[0] = (int32_t *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
    txdataF2[1] = (int32_t *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
    
    bzero(txdataF2[0],FRAME_LENGTH_BYTES_NO_PREFIX);
    bzero(txdataF2[1],FRAME_LENGTH_BYTES_NO_PREFIX);
#endif
    */

    s_re = malloc(2*sizeof(double*));
    s_im = malloc(2*sizeof(double*)); 
    r_re = malloc(2*sizeof(double*));
    r_im = malloc(2*sizeof(double*));
    r_re0 = malloc(2*sizeof(double*));
    r_im0 = malloc(2*sizeof(double*));
    
    
    for (i=0;i<2;i++) {
      
      s_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_re[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      s_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(s_im[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_re[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_re0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_re0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      r_im0[i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
      bzero(r_im0[i],FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    }
    
  }
   // initialized channel descriptors
  for (eNB_id=0;eNB_id<NB_CH_INST;eNB_id++) {
    for (UE_id=0;UE_id<NB_UE_INST;UE_id++) {
#ifdef DEBUG_SIM
      printf("[SIM] Initializing channel from eNB %d to UE %d\n",eNB_id,UE_id);
#endif
      eNB2UE[eNB_id][UE_id] = new_channel_desc(PHY_vars_eNB_g[eNB_id]->lte_frame_parms.nb_antennas_tx,
					       PHY_vars_UE_g[UE_id]->lte_frame_parms.nb_antennas_rx,
					       nb_taps,
					       channel_length,
					       amps,
					       NULL,
					       NULL,
					       Td,
					       BW,
					       ricean_factor,
					       aoa,
					       forgetting_factor,
					       maxDoppler,
					       0,
					       0);

      eNB2UE[eNB_id][UE_id]->path_loss_dB = -105 + snr_dB;

      UE2eNB[UE_id][eNB_id] = new_channel_desc(PHY_vars_UE_g[UE_id]->lte_frame_parms.nb_antennas_tx,
					       PHY_vars_eNB_g[eNB_id]->lte_frame_parms.nb_antennas_rx,
					       nb_taps,
					       channel_length,
					       amps,
					       NULL,
					       NULL,
					       Td,
					       BW,
					       ricean_factor,
					       aoa,
					       forgetting_factor,
					       maxDoppler,
					       0,
					       0);

      UE2eNB[UE_id][eNB_id]->path_loss_dB = -105 + snr_dB + 20;
#ifdef DEBUG_SIM
      printf("[SIM] Path loss from eNB %d to UE %d => %f dB\n",eNB_id,UE_id,eNB2UE[eNB_id][UE_id]->path_loss_dB);
      printf("[SIM] Path loss from UE %d to eNB %d => %f dB\n",UE_id,eNB_id,UE2eNB[UE_id][eNB_id]->path_loss_dB);
#endif
    }
  }
    
  randominit(0);
  set_taus_seed(0);

  number_of_cards = 1;

  openair_daq_vars.rx_rf_mode = 1;
  openair_daq_vars.tdd = 1;
  openair_daq_vars.rx_gain_mode = DAQ_AGC_ON;
  if ((transmission_mode != 1) && (transmission_mode != 6))
    openair_daq_vars.dlsch_transmission_mode = 2;
  else
    openair_daq_vars.dlsch_transmission_mode = transmission_mode;
  openair_daq_vars.target_ue_dl_mcs = target_dl_mcs;
  openair_daq_vars.target_ue_ul_mcs = target_ul_mcs;
  openair_daq_vars.dlsch_rate_adaptation = rate_adaptation_flag;
  openair_daq_vars.ue_ul_nb_rb = 2;

  for (UE_id=0; UE_id<NB_UE_INST;UE_id++){ // begin navid
    PHY_vars_UE_g[UE_id]->rx_total_gain_dB=140;
    PHY_vars_UE_g[UE_id]->UE_mode[0] = PRACH;
    PHY_vars_UE_g[UE_id]->lte_ue_pdcch_vars[0]->crnti = 0xBEEF;
    PHY_vars_UE_g[UE_id]->current_dlsch_cqi[0]=4;
  }// end navid 
 

#ifdef XFORMS
  fl_initialize(&argc, argv, NULL, 0, 0);
  for (UE_id=0; UE_id<NB_UE_INST;UE_id++)
    for (eNB_id=0; eNB_id<NB_CH_INST;eNB_id++) {
      form[eNB_id][UE_id] = create_form_phy_procedures_sim();                 
      sprintf(title,"LTE SIM UE %d eNB %d",UE_id,eNB_id);   
      fl_show_form(form[eNB_id][UE_id]->phy_procedures_sim,FL_PLACE_HOTSPOT,FL_FULLBORDER,title);
    }
#endif

#ifdef OPENAIR2
  l2_init(&PHY_vars_eNB_g[0]->lte_frame_parms);



  if (NB_CH_INST>0)
    mac_xface->mrbch_phy_sync_failure(0,0);
#ifdef DEBUG_SIM
  printf("[SIM] Synching to eNB\n");
#endif
  for (UE_id=0;UE_id<NB_UE_INST;UE_id++)
    mac_xface->chbch_phy_sync_success(1+UE_id,0);
#endif 
 
  for (mac_xface->frame=0; mac_xface->frame<n_frames; mac_xface->frame++) {
    
    for (slot=0 ; slot<20 ; slot++) {
      last_slot = (slot - 1)%20;
      if (last_slot <0)
	last_slot+=20;
      next_slot = (slot + 1)%20;

      if(next_slot %2 ==0)
	clear_eNB_transport_info(oai_emulation.info.nb_enb_local);
      for (eNB_id=oai_emulation.info.first_enb_local;eNB_id<(oai_emulation.info.first_enb_local+oai_emulation.info.nb_enb_local);eNB_id++) {
	//#ifdef DEBUG_SIM
	printf("[SIM] EMU PHY procedures eNB %d for frame %d, slot %d (subframe %d)\n",eNB_id,mac_xface->frame,slot,next_slot>>1);
	//#endif
	phy_procedures_eNB_lte(last_slot,next_slot,PHY_vars_eNB_g[eNB_id],abstraction_flag);
      }
      direction = subframe_select(frame_parms,next_slot>>1);
      
      if (ethernet_flag ==1) {
	if (( (direction == SF_DL) || (direction == SF_S) ) && ((next_slot%2)== 0)){
	  emu_transport_DL(last_slot,next_slot);
	  LOG_I(EMU, "DL frame %d subframe %d slot %d \n", mac_xface->frame, next_slot>>1, slot)
	}
      }
      // Call ETHERNET emulation here
      if((next_slot %2) == 0) 
	clear_UE_transport_info(oai_emulation.info.nb_ue_local);
      for (UE_id=oai_emulation.info.first_ue_local; UE_id<(oai_emulation.info.first_ue_local+oai_emulation.info.nb_ue_local);UE_id++)
	if (mac_xface->frame >= (UE_id*10)) { // activate UE only after 10*UE_id frames so that different UEs turn on separately
	  //#ifdef DEBUG_SIM
	  printf("[SIM] EMU PHY procedures UE %d for frame %d, slot %d (subframe %d)\n", UE_id,mac_xface->frame,slot, next_slot>>1);
	  //printf("[SIM] txdataF[0] %p\n",PHY_vars_UE_g[UE_id]->lte_ue_common_vars.txdataF[0]);
	  //#endif
	  phy_procedures_ue_lte(last_slot,next_slot,PHY_vars_UE_g[UE_id],0,abstraction_flag);
	}
      if (ethernet_flag == 1){
	if (((direction == SF_UL) && ((next_slot%2)==0)) || ((direction == SF_S) && ((last_slot%2)==1))){
	  emu_transport_UL(last_slot , next_slot);
	  LOG_I(EMU, "UL frame %d subframe %d slot %d \n", mac_xface->frame, next_slot>>1, slot)
	}
      }
    
      if (direction  == SF_DL) {
	/*
	  uint8_t aarx,aatx,k;	  for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	  for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	  for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	  printf("DL A(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	*/
	do_DL_sig(r_re0,r_im0,r_re,r_im,s_re,s_im,eNB2UE,next_slot,nf,snr_dB,abstraction_flag,frame_parms);
	/*
	  for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	  for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	  for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	  printf("DL B(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	*/
      }
      else if (direction  == SF_UL) {
	/*
	  for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	  for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	  for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	  printf("UL A(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	*/
	do_UL_sig(r_re0,r_im0,r_re,r_im,s_re,s_im,UE2eNB,next_slot,nf,snr_dB,abstraction_flag,frame_parms);
	/*
	  for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	  for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	  for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	  printf("UL B(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	*/
      }
      else {//it must be a special subframe
	if (next_slot%2==0) {//DL part
	  /*
	    for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	    for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	    for (k=0;k<UE2eNB[1][0]->channel_length;k++)
		
	    printf("SA(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	  */
	  do_DL_sig(r_re0,r_im0,r_re,r_im,s_re,s_im,eNB2UE,next_slot,nf,snr_dB,abstraction_flag,frame_parms);
	  /*
	    for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	    for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	    for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	    printf("SB(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	  */
	}
	else {// UL part
	  /*
	    for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	    for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	    for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	    printf("SC(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	  */
	  do_UL_sig(r_re0,r_im0,r_re,r_im,s_re,s_im,UE2eNB,next_slot,nf,snr_dB,abstraction_flag,frame_parms);
	  /*
	    for (aarx=0;aarx<UE2eNB[1][0]->nb_rx;aarx++)
	    for (aatx=0;aatx<UE2eNB[1][0]->nb_tx;aatx++)
	    for (k=0;k<UE2eNB[1][0]->channel_length;k++)
	    printf("SD(%d,%d,%d)->(%f,%f)\n",k,aarx,aatx,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].r,UE2eNB[1][0]->ch[aarx+(aatx*UE2eNB[1][0]->nb_rx)][k].i);
	  */
	}
      }
      if ((last_slot==1) && (mac_xface->frame==1) && (abstraction_flag==0)) {
	write_output("UErxsigF0.m","rxsF0", PHY_vars_UE_g[0]->lte_ue_common_vars.rxdataF[0],frame_parms->ofdm_symbol_size*frame_parms->symbols_per_tti,2,1);
	write_output("eNBrxsigF0.m","rxsF0", PHY_vars_eNB_g[0]->lte_eNB_common_vars.rxdataF[0][0],frame_parms->ofdm_symbol_size*frame_parms->symbols_per_tti,2,1);
	write_output("dlchan0.m","dlch0",&(PHY_vars_UE_g[0]->lte_ue_common_vars.dl_ch_estimates[0][0][0]),(6*(PHY_vars_UE_g[0]->lte_frame_parms.ofdm_symbol_size)),1,1);
	write_output("pbch_rxF_comp0.m","pbch_comp0",PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->rxdataF_comp[0],6*12*4,1,1);
	write_output("pbch_rxF_llr.m","pbch_llr",PHY_vars_UE_g[0]->lte_ue_pbch_vars[0]->llr,(frame_parms->Ncp==0) ? 1920 : 1728,1,4);

      }
      /*
      if ((last_slot==1) && (mac_xface->frame==1)) {
	write_output("dlsch_rxF_comp0.m","dlsch0_rxF_comp0",PHY_vars_UE->lte_ue_dlsch_vars[eNB_id]->rxdataF_comp[0],300*(-(PHY_vars_UE->lte_frame_parms.Ncp*2)+14),1,1);
	write_output("pdcch_rxF_comp0.m","pdcch0_rxF_comp0",PHY_vars_UE->lte_ue_pdcch_vars[eNB_id]->rxdataF_comp[0],4*300,1,1);
      }
      */ 
    }

    if ((mac_xface->frame==1)&&(abstraction_flag==0)) {
      write_output("UErxsig0.m","rxs0", PHY_vars_UE_g[0]->lte_ue_common_vars.rxdata[0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
      write_output("eNBrxsig0.m","rxs0", PHY_vars_eNB_g[0]->lte_eNB_common_vars.rxdata[0][0],FRAME_LENGTH_COMPLEX_SAMPLES,1,1);
    }

#ifdef XFORMS
    for (UE_id=0; UE_id<NB_UE_INST;UE_id++)
      for (eNB_id=0; eNB_id<NB_CH_INST;eNB_id++) {
	do_forms(form[eNB_id][UE_id],PHY_vars_UE_g[UE_id]->lte_ue_dlsch_vars,PHY_vars_eNB_g[eNB_id]->lte_eNB_ulsch_vars,eNB2UE[eNB_id][UE_id]->ch,eNB2UE[eNB_id][UE_id]->channel_length);
      }
#endif
      
  }
  // relase all rx state
  emu_transport_release();
  
  if (abstraction_flag==0) {
    /*
#ifdef IFFT_FPGA
    free(txdataF2[0]);
    free(txdataF2[1]);
    free(txdataF2);
    free(txdata[0]);
    free(txdata[1]);
    free(txdata);
#endif 
    */

    for (i=0;i<2;i++) {
      free(s_re[i]);
      free(s_im[i]);
      free(r_re[i]);
      free(r_im[i]);
    }
    free(s_re);
    free(s_im);
    free(r_re);
    free(r_im);
    
    lte_sync_time_free();
  }
  return(n_errors);
}
   

