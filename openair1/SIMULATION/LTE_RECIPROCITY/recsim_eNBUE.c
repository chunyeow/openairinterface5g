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
//cd Desktop/openair4G/trunk/openair1/SIMULATION/LTE_RECIPROCITY/
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>

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
#include "LAYER2/MAC/vars.h"

#include "OCG_vars.h"

#include "coeffs.h"

#ifdef XFORMS
#include "forms.h"
#include "../../USERSPACE_TOOLS/SCOPE/lte_scope.h"
#endif

//#define AWGN
//#define NO_DCI

#define BW 7.68

extern uint16_t beta_ack[16],beta_ri[16],beta_cqi[16];
extern unsigned short dftsizes[33];
extern short *ul_ref_sigs[30][2][33];

PHY_VARS_eNB *PHY_vars_eNB[2];
PHY_VARS_UE *PHY_vars_UE[2];

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, 2);
  exit(1);
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
  
    for (i=0; i<FRAME_LENGTH_COMPLEX_SAMPLES; i++)  {
      sig2[i] = 10*log10(1.0+(double) ((rx_sig[0][2*i])*(rx_sig[0][2*i])+(rx_sig[0][2*i+1])*(rx_sig[0][2*i+1])));
      time2[i] = (float) i;
    }

    //fl_set_xyplot_ybounds(form->channel_t_im,0,100);
    //fl_set_xyplot_data(form->channel_t_im,&time2[640*12*6],&sig2[640*12*6],640*12,"","","");
    fl_set_xyplot_data(form->channel_t_im,time2,sig2,FRAME_LENGTH_COMPLEX_SAMPLES,"","","");
    //}

  
  // DLSCH LLR
  for(i=0;i<coded_bits_per_codeword;i++) {
    llr[i] = (float) dlsch_llr[i];
    llr_time[i] = (float) i;
  }

  fl_set_xyplot_data(form->demod_out,llr_time,llr,coded_bits_per_codeword,"","","");
  fl_set_xyplot_ybounds(form->demod_out,-1000,1000);

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
  fl_set_xyplot_xbounds(form->scatter_plot,-2000,2000);
  fl_set_xyplot_ybounds(form->scatter_plot,-2000,2000);

  // DLSCH I/Q
  j=0;
  for (s=0;s<frame_parms->symbols_per_tti;s++) {
    for(i=0;i<12*25;i++) {
      I[j] = dlsch_comp_i[(2*25*12*s)+2*i];
      Q[j] = dlsch_comp_i[(2*25*12*s)+2*i+1];
      j++;
    }
  
  }

  fl_set_xyplot_data(form->scatter_plot1,I,Q,j,"","","");
  fl_set_xyplot_xbounds(form->scatter_plot1,-2000,2000);
  fl_set_xyplot_ybounds(form->scatter_plot1,-2000,2000);

  // DLSCH I/Q
  j=0;
  for (s=0;s<frame_parms->symbols_per_tti;s++) {
    for(i=0;i<12*25;i++) {
      I[j] = dlsch_rho[(2*25*12*s)+2*i];
      Q[j] = dlsch_rho[(2*25*12*s)+2*i+1];
      j++;
    }
    
  }

  fl_set_xyplot_data(form->scatter_plot2,I,Q,j,"","","");

  free(llr);
  free(llr_time);

}
#endif

void lte_param_init(unsigned char N_tx, unsigned char N_rx,unsigned char transmission_mode,uint8_t extended_prefix_flag,uint16_t Nid_cell,uint8_t tdd_config,uint8_t N_RB_DL,uint8_t osf) {

  LTE_DL_FRAME_PARMS *lte_frame_parms;
  int i, kk;

  printf("Start lte_param_init\n");
  PHY_vars_eNB[0] = malloc(sizeof(PHY_VARS_eNB));
  PHY_vars_eNB[1] = malloc(sizeof(PHY_VARS_eNB));
  PHY_vars_UE[0] = malloc(sizeof(PHY_VARS_UE));
  PHY_vars_UE[1] = malloc(sizeof(PHY_VARS_UE));
  //PHY_config = malloc(sizeof(PHY_CONFIG));
  mac_xface = malloc(sizeof(MAC_xface));

  randominit(0);
  set_taus_seed(0);
  
  lte_frame_parms = &(PHY_vars_eNB[0]->lte_frame_parms);

  lte_frame_parms->N_RB_DL            = N_RB_DL;   //50 for 10MHz and 25 for 5 MHz
  lte_frame_parms->N_RB_UL            = N_RB_DL;   
  lte_frame_parms->Ncp                = extended_prefix_flag;
  lte_frame_parms->Nid_cell           = Nid_cell;
  lte_frame_parms->nushift            = 0;
  lte_frame_parms->nb_antennas_tx     = N_tx;
  lte_frame_parms->nb_antennas_rx     = N_rx;
  lte_frame_parms->phich_config_common.phich_resource         = oneSixth;
  lte_frame_parms->tdd_config         = tdd_config;
  lte_frame_parms->frame_type         = 1;
  //  lte_frame_parms->Csrs = 2;
  //  lte_frame_parms->Bsrs = 0;
  //  lte_frame_parms->kTC = 0;44
  //  lte_frame_parms->n_RRC = 0;
  lte_frame_parms->mode1_flag = (transmission_mode == 1)? 1 : 0;

  init_frame_parms(lte_frame_parms,osf);
  
  //copy_lte_parms_to_phy_framing(lte_frame_parms, &(PHY_config->PHY_framing));
  
  phy_init_top(lte_frame_parms); //allocation
  
  lte_frame_parms->twiddle_fft      = twiddle_fft;
  lte_frame_parms->twiddle_ifft     = twiddle_ifft;
  lte_frame_parms->rev              = rev;

  for (kk=0; kk<2; kk++) {

	PHY_vars_UE[kk]->is_secondary_ue = 0;
	PHY_vars_UE[kk]->lte_frame_parms = *lte_frame_parms;

	for (i=0;i<3;i++)
    		lte_gold(lte_frame_parms,PHY_vars_UE[kk]->lte_gold_table[i],i);

	phy_init_lte_ue(&PHY_vars_UE[kk]->lte_frame_parms,
		  	&PHY_vars_UE[kk]->lte_ue_common_vars,
		  	PHY_vars_UE[kk]->lte_ue_dlsch_vars,
		  	PHY_vars_UE[kk]->lte_ue_dlsch_vars_SI,
		  	PHY_vars_UE[kk]->lte_ue_dlsch_vars_ra,
		  	PHY_vars_UE[kk]->lte_ue_pbch_vars,
		  	PHY_vars_UE[kk]->lte_ue_pdcch_vars,
		  	PHY_vars_UE[kk]
			,0);
  }
  
  PHY_vars_eNB[0]->lte_frame_parms = *lte_frame_parms;
  

  phy_init_lte_top(lte_frame_parms);
  dump_frame_parms(lte_frame_parms);

  phy_init_lte_eNB(&PHY_vars_eNB[0]->lte_frame_parms,
		   &PHY_vars_eNB[0]->lte_eNB_common_vars,
		   PHY_vars_eNB[0]->lte_eNB_ulsch_vars,
		   0,
		   PHY_vars_eNB[0],
		   1,
		   0);

  PHY_vars_eNB[1]->lte_frame_parms = PHY_vars_eNB[0]->lte_frame_parms;

  phy_init_lte_eNB(&PHY_vars_eNB[1]->lte_frame_parms,
		   &PHY_vars_eNB[1]->lte_eNB_common_vars,
		   PHY_vars_eNB[1]->lte_eNB_ulsch_vars,
		   0,
		   PHY_vars_eNB[1],
		   1,
		   0);

  PHY_vars_eNB[0]->lte_frame_parms.nb_antennas_tx = 1;
  PHY_vars_eNB[0]->lte_frame_parms.nb_antennas_rx = 1;
  PHY_vars_eNB[1]->lte_frame_parms.nb_antennas_tx = 1;
  PHY_vars_eNB[1]->lte_frame_parms.nb_antennas_rx = 1;

  PHY_vars_UE[0]->lte_frame_parms.nb_antennas_tx = 1;
  PHY_vars_UE[0]->lte_frame_parms.nb_antennas_rx = 1;
  PHY_vars_UE[1]->lte_frame_parms.nb_antennas_tx = 1;
  PHY_vars_UE[1]->lte_frame_parms.nb_antennas_rx = 1;

  printf("Done lte_param_init\n");
}

// Apply phase offsets
void phase_offsets(double *re_in, double *im_in, double *re_out, double *im_out, int length_sig, double *phase_in, double phase_inc, int pos_neg) {

	int k;
	double tmp_re,tmp_im;
	double phase;

	for (k=0; k<length_sig; k++) {
	        re_out[k] = 0;
		im_out[k] = 0;
	}

	phase = *phase_in;

	for (k=0; k<length_sig; k++) {
		tmp_re = re_in[k]*cos(phase) - pos_neg*im_in[k]*sin(phase);
		tmp_im = pos_neg*re_in[k]*sin(phase) + im_in[k]*cos(phase);

		re_out[k] = tmp_re;
      		im_out[k] = tmp_im;

		phase += phase_inc;
	}

	*phase_in = phase;
}


void real_fir(double *re_in, double *im_in, double *re_out, double *im_out, double *coeffs, int ord_fir, int length_sig) {
	int k, l;
	double temp1, temp2;

	for (k=0; k<length_sig; k++) {
	        re_out[k] = 0;
		im_out[k] = 0;
	}

	for (k=ord_fir; k<length_sig; k++) {
		temp1 = 0;
		temp2 = 0;
		for (l=0; l<ord_fir; l++) {
			temp1 += coeffs[l]*re_in[k-l-1];
			temp2 += coeffs[l]*im_in[k-l-1];
		}
		re_out[k] = temp1;
		im_out[k] = temp2;
	}
}

// Modif Channel quantization at UE
void do_quantization_UE(PHY_VARS_UE *PHY_vars_UE, unsigned int nsymb, uint8_t pilot0, int quant_v, short *dl_ch_estimates, int dec_f)  {
        int k;
        short tx_energy;
	short dl_ch_estimates_norm[PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb];
        
	tx_energy = 8; 

	for (k=0;k<PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb;k++)
 		dl_ch_estimates_norm[k] = ((short *)PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][0])[k]/tx_energy;
			
	for (k=pilot0*2*512; k<pilot0*2*512+2*300-1; k+=(2*dec_f)) {
		if (dl_ch_estimates_norm[k] > (quant_v-1))
		        dl_ch_estimates[k-pilot0*2*512] = quant_v-1;

		else if (dl_ch_estimates_norm[k] < (-quant_v))
			dl_ch_estimates[k-pilot0*2*512] = -quant_v;
		else 
			dl_ch_estimates[k-pilot0*2*512] = dl_ch_estimates_norm[k];

		if (dl_ch_estimates_norm[k+1]>(quant_v-1))
		        dl_ch_estimates[k+1-pilot0*2*512] = quant_v-1;
		else if (dl_ch_estimates_norm[k+1] < (-quant_v))
			dl_ch_estimates[k+1-pilot0*2*512] = -quant_v;
		else
			dl_ch_estimates[k+1-pilot0*2*512] = dl_ch_estimates_norm[k+1];
	}
/*
        for(aa=0;aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx;aa++) { 
		for (aarx=0;aarx<PHY_vars_UE->lte_frame_parms.nb_antennas_rx;aarx++) {
			for (k=0;k<PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb;k++) { 
 			    dl_ch_estimates_norm[k+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb] = ((short *)PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][(aa<<1)+aarx])[k]/tx_energy;
			}
	        }
	}

	for(aa=0;aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx;aa++) { 
		for (k=pilot[aa]*2*512; k<pilot[aa]*2*512+2*300-1; k+=(2*dec_f)) {
		        if (dl_ch_estimates_norm[k+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb] > (quant_v-1))
		                dl_ch_estimates[k-pilot[aa]*2*512+aa*2*300] = quant_v-1;

			else if (dl_ch_estimates_norm[k+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb] < (-quant_v))
				dl_ch_estimates[k-pilot[aa]*2*512+aa*2*300] = -quant_v;
			else 
				dl_ch_estimates[k-pilot[aa]*2*512+aa*2*300] = dl_ch_estimates_norm[k+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb];

			if (dl_ch_estimates_norm[k+1+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb]>(quant_v-1))
		                dl_ch_estimates[k+1-pilot[aa]*2*512+aa*2*300] = quant_v-1;
			else if (dl_ch_estimates_norm[k+1+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb] < (-quant_v))
				dl_ch_estimates[k+1-pilot[aa]*2*512+aa*2*300] = -quant_v;
			else
				dl_ch_estimates[k+1-pilot[aa]*2*512+aa*2*300] = dl_ch_estimates_norm[k+1+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb];
		}
	}
*/
}

void do_quantization_eNB(PHY_VARS_eNB *PHY_vars_eNB, PHY_VARS_UE *PHY_vars_UE, unsigned int nsymb, uint8_t pilot0, uint8_t pilot1, int quant_v, short *drs_ch_estimates, int UE_id)  {
        int k, aa, aarx;
        short tx_energy;
	short drs_ch_estimates_norm[2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size*nsymb];

        uint8_t pilot[2];
        pilot[0] = pilot0;
        pilot[1] = pilot1;
	
	//for(k=0;k<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx;k++)
        	tx_energy = 8; 

        for(aa=0;aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx;aa++) { 
		for (aarx=0;aarx<PHY_vars_UE->lte_frame_parms.nb_antennas_rx;aarx++) {
			for (k=0;k<PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb;k++) { 
			    drs_ch_estimates_norm[k+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb] = ((short *)PHY_vars_eNB->lte_eNB_ulsch_vars[0]->drs_ch_estimates[UE_id][aa+aarx])[k]/tx_energy;
			}
	        }
	}

        for(aa=0;aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx;aa++) { 		
		for (k=pilot[aa]*2*300; k<pilot[aa]*2*300+2*300-1; k+=2) {
	      		if (drs_ch_estimates_norm[k+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb]>(quant_v-1))
		        	drs_ch_estimates[k-pilot[aa]*2*300+aa*2*300] = quant_v-1;
			else if ((drs_ch_estimates_norm[k+aa*2*300*nsymb]) < (-quant_v))
				drs_ch_estimates[k-pilot[aa]*2*300+aa*2*300] = -quant_v;
			else 
			 	drs_ch_estimates[k-pilot[aa]*2*300+aa*2*300] = drs_ch_estimates_norm[k+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb];

			if (drs_ch_estimates_norm[k+1+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb]>(quant_v-1))
		                drs_ch_estimates[k+1-pilot[aa]*2*300+aa*2*300] = quant_v-1;
			else if ((drs_ch_estimates_norm[k+1+aa*2*300*nsymb])< (-quant_v))
				drs_ch_estimates[k+1-pilot[aa]*2*300+aa*2*300] = -quant_v;
			else
			 	drs_ch_estimates[k+1-pilot[aa]*2*300+aa*2*300] = drs_ch_estimates_norm[k+1+aa*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb];
		}
	}
}

void do_precoding(PHY_VARS_eNB *PHY_vars_eNB, PHY_VARS_UE *PHY_vars_UE, double PeNb_factor[2][600], short *prec, double *Norm, int nsymb, int UE_id, int aa) {

  int l, k;
  double temp[nsymb][600];
  short drs_ch_estimates[600*nsymb];
  short dl_ch_estimates[600*nsymb];
 
  for (k=0; k<600*nsymb; k++) {
    drs_ch_estimates[k] = ((short *)PHY_vars_eNB->lte_eNB_ulsch_vars[0]->drs_ch_estimates[UE_id][(aa<<1)])[k];
    //drs_ch_estimates[aa][2*k+1] = ((short *)PHY_vars_eNB->lte_eNB_ulsch_vars[0]->drs_ch_estimates[UE_id][0])[2*k+1];
    prec[2*k] = 0;
    prec[2*k+1] = 0;
  }

  for (k=0; k<600*nsymb; k++) {
      dl_ch_estimates[k] = ((short *)PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][(aa<<1)])[k];
    }

  //printf("PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size = %d\n", PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size);
  //printf("nsymb = %d\n", nsymb);

  //write_output("drs1.m","drs", drs_ch_estimates[0],nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size,1,1);

  //exit(-1);

    for (k=0; k<nsymb; k++) {
      for (l=0; l<600; l+=2) {
        temp[k][l] = drs_ch_estimates[k*600+l]*PeNb_factor[aa][l] - drs_ch_estimates[k*600+l+1]*PeNb_factor[aa][l+1];
        temp[k][l+1] = drs_ch_estimates[k*600+l+1]*PeNb_factor[aa][l] + drs_ch_estimates[k*600+l]*PeNb_factor[aa][l+1];
      
        prec[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l] = (short)(temp[k][l]);///(temp[k][l]*temp[k][l]+temp[k][l+1]*temp[k][l+1]));
        prec[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l+1] = (short)(temp[k][l+1]);///(temp[k][l]*temp[k][l]+temp[k][l+1]*temp[k][l+1]));
	Norm[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l] = sqrt(temp[k][l]*temp[k][l] + temp[k][l+1]*temp[k][l+1]);
        Norm[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l+1] = sqrt(temp[k][l]*temp[k][l] + temp[k][l+1]*temp[k][l+1]);
      }
    }
  //write_output("drsch.m","drschF", drs_ch_estimates[0],nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size,1,1);
  //write_output("prec.m","precF", prec,nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size,1,1);  
  //write_output("dlch.m","dlchF", dl_ch_estimates,nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size,1,1);
//exit(-1);

}
/*
void do_precoding_perfect(PHY_VARS_UE *PHY_vars_UE, double PeNb_factor[2][600], short *prec, double *Norm, int nsymb, int UE_id) {

  int l, k, aa;
  double temp[nsymb][600];
  short dl_ch_estimates[2][2*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb];

  for (aa = 0; aa<1; aa++) {
    for (k=0; k<PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size*nsymb; k++) {
      dl_ch_estimates[aa][2*k] = ((short *)PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][0])[2*k];
      dl_ch_estimates[aa][2*k+1] = ((short *)PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][0])[2*k+1];
      prec[2*k] = 1;
      prec[2*k+1] = 0;
    }
  }

  //write_output("drs1.m","drs", drs_ch_estimates[0],2*nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size,1,1);

  for (aa=0; aa<1; aa++) {
    for (k=0; k<nsymb; k++) {
      for (l=0; l<600; l+=2) {
        temp[k][l] = dl_ch_estimates[aa][2*k*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size+l];
        temp[k][l+1] = dl_ch_estimates[aa][2*k*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size+l+1];
      }
    }
    for (k=0; k<nsymb; k++) {
      for (l=0; l<600; l+=2) {
        prec[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l] = (short)(temp[k][l]);///(temp[k][l]*temp[k][l]+temp[k][l+1]*temp[k][l+1])
        prec[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l+1] = (short)(temp[k][l+1]);//(short)(-temp[k][l+1]);
	//Norm[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l] = sqrt(temp[k][l]*temp[k][l] + temp[k][l+1]*temp[k][l+1]);
        //Norm[k*2*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size+l+1] = sqrt(temp[k][l]*temp[k][l] + temp[k][l+1]*temp[k][l+1]);
      }
    }
  }
}*/

//end Modif channel quantization at UE 

//calibration algo
void do_calibration(short K_dl_ch_estimates[25][2][600], short K_drs_ch_estimates[25][2][600], double PeNb_factor[2][600], int ofdm_syn, int n_K) {
       
  //Calib Algor in eNb
  int i=0, s_c=0;
  double ar=0,ai=0,br=0,bi=0,cr=0,ci=0,dr=0,di=0;
  int aa;
  int length_H_G = n_K*4;

  //double phase_inc = 2*M_PI*(4*512-4*300)*(5-1)*1/7.68e6;


  short H[length_H_G];
  short G[length_H_G];  	
  bzero(H,length_H_G);
  bzero(G,length_H_G);
 
 for(s_c=0; s_c<600; s_c+=2)
  {
  for(aa=0; aa<1; aa++)
    {
    //system for 1 ant at primary, change to perform onother prim ant
	for(i=0;i<n_K;i++)
	{ 
	//printf("i = %d\n",i);
	G[(i<<2)+0] = K_dl_ch_estimates[i][aa][s_c+0];	
	G[(i<<2)+1] = K_dl_ch_estimates[i][aa][s_c+1];	
	H[(i<<2)+0] = K_drs_ch_estimates[i][aa][s_c+0];
	H[(i<<2)+1] = K_drs_ch_estimates[i][aa][s_c+1];
	}
       
      for(i=0;i<n_K;i++)
	{ 
	  ar +=  H[(i<<2)+0]*H[(i<<2)+0] + H[(i<<2)+1]*H[(i<<2)+1];
	  br +=  H[(i<<2)+0]*G[(i<<2)+0] + H[(i<<2)+1]*G[(i<<2)+1];
	  bi += -H[(i<<2)+0]*G[(i<<2)+1] + H[(i<<2)+1]*G[(i<<2)+0];
	  dr +=  G[(i<<2)+0]*G[(i<<2)+0] + G[(i<<2)+1]*G[(i<<2)+1];
	}
	  ar = (double)(ar/100); 
	  br = (double)(br/100);
	  bi = (double)(bi/100);
	  dr = (double)(dr/100);
	  

      if( (ar-dr+iSqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4)))==0)
      {
      PeNb_factor[aa][s_c] = 0;
      PeNb_factor[aa][s_c+1] = 0;     
      } 
      else{
      PeNb_factor[aa][s_c]   = (2*br/(ar-dr+iSqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4))));
      PeNb_factor[aa][s_c+1] = (-2*bi/(ar-dr+iSqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4))));  
      }
      
      ar=0; ai=0; br=0; bi=0; cr=0; ci=0; dr=0; di=0;
      //if ((s_c>>1) > 4) exit(-1);      
   }
  }
  msg("P_eNb DETERMINED.. \n");
}


//DCI2_5MHz_2A_M10PRB_TDD_t DLSCH_alloc_pdu2_2A[2];
DCI2_5MHz_2D_M10PRB_TDD_t DLSCH_alloc_pdu2_2D[2];


#define UL_RB_ALLOC 0x1ff;
//#define CCCH_RB_ALLOC computeRIV(PHY_vars_eNB[0]->lte_frame_parms.N_RB_UL,0,2)
//#define DLSCH_RB_ALLOC 0x1fbf // igore DC component,RB13
#define DLSCH_RB_ALLOC 0x1fff // all 25 RBs
//#define DLSCH_RB_ALLOC 0x0001

int main(int argc, char **argv) {

  char c;
  int k,i,j,b,aa,aarx,Msc_RS_idx=0;

  double sigma2_eNB[2], sigma2_eNB_dB[2]={10,10},SNR,snr0=-2.0,snr1,rate_eNB=0;
//modif start UL
  unsigned int coded_bits_per_codeword_UE;
  double sigma2_UE[2], sigma2_UE_dB[2]={10,10}, SNRmeas[2], rate_UE=0;
  uint8_t control_only_flag = 0;
  uint8_t cooperation_flag = 0; 
  int **txdata_UE[2];
  uint8_t cyclic_shift = 0;
  uint8_t beta_ACK=0,beta_RI=0,beta_CQI=2;
  uint8_t srs_flag = 0;
  char fname[20],vname[20];
//modif end UL
  double snr_step=1, snr_int=30;
  //int **txdataF, **txdata;
  int **txdata_eNB[2];
#ifdef IFFT_FPGA
  int **txdataF2_eNB[2];
//modif start UL
  int **txdataF2_UE[2];
//modif end UL
  int ind;
#endif
  LTE_DL_FRAME_PARMS *frame_parms;
  double **s_re_eNB[2], **s_im_eNB[2], **r_re_eNB[2][2], **r_im_eNB[2][2], **r_re_2eNB[2], **r_im_2eNB[2];
//modif start UL
int llb;
  double **s_re_UE[2], **s_im_UE[2], **r_re_UE[2][2], **r_im_UE[2][2], **r_re_2UE[2], **r_im_2UE[2];
//modif end UL
  double forgetting_factor=1.0; //in [0,1] 0 means a new channel every time, 1 means keep the same channel
  //double hold_channel=0; //use hold_channel=1 instead of forgetting_factor=1 (more efficient)
  double iqim=0.0;

  uint8_t extended_prefix_flag=0,transmission_mode=1,n_tx=1,n_rx=1;
  uint16_t Nid_cell=0;

  int eNB_id = 0, eNB_id_i = NUMBER_OF_eNB_MAX;
//modif start UL
  int UE_id = 0;
  unsigned char mcs_UE;
  int dec_f=1;
  short quant=8, quant_v;
//modif end UL
  unsigned char mcs_eNB,dual_stream_UE = 0,awgn_flag=0,round_eNB[2],round_UE[2],dci_flag=0;
  unsigned char i_mod = 2;
  unsigned short NB_RB=conv_nprb(0,DLSCH_RB_ALLOC);
  unsigned char Ns,l,m;
  uint16_t tdd_config=3;
  uint16_t n_rnti=0x1234;

  int decalibration = 0, phase_offset = 0;

  double s_time = 1/7.68e6;
  double delta_offset = 100;
  double phase_inc = 2*M_PI*delta_offset*s_time;

  double phase_in_UL = phase_inc; 
  double phase_in_DL = phase_inc;

  SCM_t channel_model=Rayleigh1_corr;

  unsigned char *input_buffer[2];
  unsigned short input_buffer_length[2];
  unsigned int ret_eNB[2],ret_UE[2];
  unsigned int coded_bits_per_codeword=0,nsymb,dci_cnt,tbs;
 
  unsigned int tx_lev_eNB[2],tx_lev_eNB_dB[2],trials,errs_eNB[2][4],round_trials_UE[2][4],round_trials_eNB[2][4],dci_errors[2]={0,0},dlsch_active=0,num_layers;
//modif start UL
  unsigned int tx_lev_UE[2],tx_lev_UE_dB[2],errs_UE[2][4];
  //unsigned char *input_buffer_UE; //b
  char *input_buffer_UE[2]; //b
  unsigned short input_buffer_length_UE[2];
//modif end UL
  int re_allocated;
  FILE *bler_fd=NULL;
  char bler_fname[256];
  FILE *tikz_fd=NULL;
  char tikz_fname[256];

  FILE *input_trch_fd=NULL;
  unsigned char input_trch_file=0;
  FILE *input_fd=NULL;
  unsigned char input_file=0;
  char input_val_str[50],input_val_str2[50];

  char input_trch_val[16];
  double pilot_sinr, abs_channel;

  //  unsigned char pbch_pdu[6];

  DCI_ALLOC_t dci_alloc[8],dci_alloc_rx[8];
  int num_common_dci=0,num_ue_spec_dci=0,num_dci=0;

  //  FILE *rx_frame_file;

  int kk, ll;

  int n_frames;
  int n_ch_rlz = 1;
  channel_desc_t *eNB2UE[2][2];
  channel_desc_t *UE2eNB[2][2];
  double snr;
  uint8_t num_pdcch_symbols=3,num_pdcch_symbols_2=0;
  uint8_t pilot1,pilot2,pilot3;
  uint8_t rx_sample_offset = 0;
  //char stats_buffer[4096];
  //int len;
  uint8_t num_rounds = 1,fix_rounds=0;
  uint8_t subframe_DL=6;
//modif start UL
  int subframe_UL=2;
//modif end UL
  int u;
  int abstx=0;
  int iii;
  FILE *csv_fd=NULL;
  char csv_fname[20];
  int ch_realization;
  int pmi_feedback=0;
  // void *data;
  // int ii;
  // int bler;
  double blerr,uncoded_ber[2],avg_ber[2];
  short *uncoded_ber_bit;
  uint8_t N_RB_DL=25,osf=1;
  int16_t amp;
//modif start UL
  unsigned char harq_pid[2];
  FILE *trch_out_fd=NULL;
  unsigned char nb_rb_UE=25, first_rb=0, bundling_flag=1; 
//modif end UL
#ifdef XFORMS
  FD_lte_scope *form;
  char title[255];
#endif

  // Calibration parameters
  int P_eNb_active=0;
  double PeNb_factor[2][600];

  signal(SIGSEGV, handler); 

  // default parameters
  mcs_eNB = 0;
//modif start UL
  mcs_UE = 4;
//modif end UL
  n_frames = 100;
  snr0 = 20;
  num_layers = 1;

  while ((c = getopt (argc, argv, "hadpm:n:o:s:f:t:c:g:r:F:x:y:z:M:N:I:i:R:S:C:T:b:u:w:X:q:D:")) != -1) {
    switch (c)
      {
      case 'a':
	awgn_flag = 1;
	break;
      case 'b':
	tdd_config=atoi(optarg);
	break;
      case 'd':
	dci_flag = 1;
	break;
      case 'm':
	mcs_eNB = atoi(optarg);
	break;
      /*case 'C':
      beta_CQI = atoi(optarg);
      if ((beta_CQI>15)||(beta_CQI<2)) {
	printf("beta_cqi must be in (2..15)\n");
	exit(-1);
      }
       break;
      
      case 'R':
      beta_RI = atoi(optarg);
      if ((beta_RI>15)||(beta_RI<2)) {
	printf("beta_ri must be in (0..13)\n");
	exit(-1);
      }
       break;*/
	//modif start UL
      case 'w':	
	mcs_UE = atoi(optarg);	
	break;
      case 'r':
        nb_rb_UE = atoi(optarg);
        break;
      case 'f':
        first_rb = atoi(optarg);
        break;
      case 'q':
        quant = atoi(optarg);	
        break;
      case 'D':
        dec_f = atoi(optarg);	
        break;
	//modif end UL
      case 'n':
	n_frames = atoi(optarg);
	break;
      case 'C':
	Nid_cell = atoi(optarg);
	break;
      case 'o':
	rx_sample_offset = atoi(optarg);
	break;
      case 'F':
	forgetting_factor = atof(optarg);
	break;
      case 's':
	snr0 = atoi(optarg);
	break;
      case 't':
	//Td= atof(optarg);
	printf("Please use the -G option to select the channel model\n");
	exit(-1);
	break;
      case 'X':
	snr_step= atof(optarg);
	break;
      case 'M':
	abstx= atof(optarg);
	break;
      case 'N':
	n_ch_rlz= atof(optarg);
	break;
      case 'p':
	extended_prefix_flag=1;
	break;
      case 'c':
	num_pdcch_symbols=atoi(optarg);
	break;
      case 'g':
	switch((char)*optarg) {
	case 'A': 
	  channel_model=SCM_A;
	  break;
	case 'B': 
	  channel_model=SCM_B;
	  break;
	case 'C': 
	  channel_model=SCM_C;
	  break;
	case 'D': 
	  channel_model=SCM_D;
	  break;
	case 'E': 
	  channel_model=EPA;
	  break;
	case 'F': 
	  channel_model=EVA;
	  break;
	case 'G': 
	  channel_model=ETU;
	  break;
	case 'H':
	  channel_model=Rayleigh8;
	  break;
	case 'I':
	  channel_model=Rayleigh1;
	  break;
	case 'J':
	  channel_model=Rayleigh1_corr;
	  break;
	case 'K':
	  channel_model=Rayleigh1_anticorr;
	  break;
	case 'L':
	  channel_model=Rice8;
	  break;
	case 'M':
	  channel_model=Rice1;
	  break;
	default:
	  msg("Unsupported channel model!\n");
	  exit(-1);
	}
	break;
      case 'x':
	transmission_mode=atoi(optarg);
	if ((transmission_mode!=1) &&
	    (transmission_mode!=2) &&
	    (transmission_mode!=5) &&
	    (transmission_mode!=6)) {
	  msg("Unsupported transmission mode %d\n",transmission_mode);
	  exit(-1);
	}
	break;
      case 'y':
	n_tx=atoi(optarg);
	if ((n_tx==0) || (n_tx>2)) {
	  msg("Unsupported number of tx antennas %d\n",n_tx);
	  exit(-1);
	}
	break;
      case 'z':
	n_rx=atoi(optarg);
	if ((n_rx==0) || (n_rx>2)) {
	  msg("Unsupported number of rx antennas %d\n",n_rx);
	  exit(-1);
	}
	break;
      case 'I':
	input_trch_fd = fopen(optarg,"r");
	input_trch_file=1;
	break;
      case 'i':
	input_fd = fopen(optarg,"r");
	input_file=1;
	dci_flag = 1;	
	break;
      case 'R':
	num_rounds=atoi(optarg);
	fix_rounds=1;
	break;
      case 'S':
	subframe_DL=atoi(optarg);
	break;
      case 'T':
	n_rnti=atoi(optarg);
	break;	
      case 'u':
	dual_stream_UE=atoi(optarg);
	if ((n_tx!=2) || (transmission_mode!=5)) {
	  msg("Unsupported nb of decoded users: %d user(s), %d user(s) to decode\n", n_tx, dual_stream_UE);
	  exit(-1);
	}
	break;
      case 'h':
      default:
	printf("%s -h(elp) -a(wgn on) -d(ci decoding on) -p(extended prefix on) -m mcs_eNB -n n_frames -s snr0 -t Delayspread -x transmission mode (1,2,5,6) -y TXant -z RXant -I trch_file\n",argv[0]);
      printf("-h This message\n");
      printf("-a Use AWGN channel and not multipath\n");
      printf("-c Number of PDCCH symbols\n");
      printf("-m MCS_eNB\n");
      printf("-w MCS_UE\n");
      printf("-q quantization parameters\n");
      printf("-D DL decimacion factor at UE\n");
      printf("-r nb_rb_UE Number of ressource blocs in the UL\n");
      printf("-f First ressource bloc in the UL\n");
      printf("-d Transmit the DCI and compute its error statistics and the overall throughput\n");
      printf("-p Use extended prefix mode\n");
      printf("-n Number of frames to simulate\n");
      printf("-o Sample offset for receiver\n");
      printf("-s Starting SNR, runs from SNR to SNR+%.1fdB in steps of %.1fdB. If n_frames is 1 then just SNR is simulated and MATLAB/OCTAVE output is generated\n", snr_int, snr_step);
      printf("-X step size of SNR, default value is 1.\n");
      printf("-t Delay spread for multipath channel\n");
      //printf("-r Ricean factor (dB, 0 dB = Rayleigh, 100 dB = almost AWGN)\n");
      printf("-g [A:M] Use 3GPP 25.814 SCM-A/B/C/D('A','B','C','D') or 36-101 EPA('E'), EVA ('F'),ETU('G') models (ignores delay spread and Ricean factor), Rayghleigh8 ('H'), Rayleigh1('I'), Rayleigh1_corr('J'), Rayleigh1_anticorr ('K'), Rice8('L'), Rice1('M')\n");
      printf("-F forgetting factor (0 new channel every trial, 1 channel constant\n");
      printf("-x Transmission mode (1,2,6 for the moment)\n");
      printf("-y Number of TX antennas used in eNB\n");
      printf("-z Number of RX antennas used in UE\n");
      printf("-R Number of HARQ rounds (fixed)\n");
      printf("-M Determines whether the Absraction flag is on or Off. 1-->On and 0-->Off. Default status is Off. \n");
      printf("-N Determines the number of Channel Realizations in Absraction mode. Default value is 1. \n");
      printf("-I Input filename for TrCH data (binary)\n");
      printf("-u Determines if the 2 streams at the UE are decoded or not. 0-->U2 is interference only and 1-->U2 is detected\n");
      exit(1);
      break;
      }
  }

#ifdef XFORMS
  fl_initialize (&argc, argv, NULL, 0, 0);
  form = create_form_lte_scope();
  sprintf (title, "LTE DLSIM SCOPE");
  fl_show_form (form->lte_scope, FL_PLACE_HOTSPOT, FL_FULLBORDER, title);
#endif

  lte_param_init(n_tx,n_rx,transmission_mode,extended_prefix_flag,Nid_cell,tdd_config,N_RB_DL,osf);  
  
  printf("Setting mcs_eNB = %d\n",mcs_eNB);
//modif start UL
  printf("Setting mcs_UE = %d\n",mcs_UE);
  quant_v = (2<<(quant-1))/2; //b quantization bit
  //printf("quant %d\n",quant_v);
//exit(-1);
//modif end UL
  printf("NPRB = %d\n",NB_RB);
  printf("n_frames = %d\n",n_frames);
  printf("Transmission mode %d with %dx%d antenna configuration, Extended Prefix %d\n",transmission_mode,n_tx,n_rx,extended_prefix_flag);

  snr1 = snr0+snr_int;
  printf("SNR0 %f, SNR1 %f\n",snr0,snr1);

  frame_parms = &PHY_vars_eNB[0]->lte_frame_parms;

#ifdef IFFT_FPGA
  for (kk=0; kk<2; kk++) {
        txdata_eNB[kk]    = (int **)malloc16(2*sizeof(int*));
	txdata_eNB[kk][0] = (int *)malloc16(FRAME_LENGTH_BYTES);
	txdata_eNB[kk][1] = (int *)malloc16(FRAME_LENGTH_BYTES);

	bzero(txdata_eNB[kk][0],FRAME_LENGTH_BYTES);
	bzero(txdata_eNB[kk][1],FRAME_LENGTH_BYTES);

	txdataF2_eNB[kk]    = (int **)malloc16(2*sizeof(int*));
	txdataF2_eNB[kk][0] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
	txdataF2_eNB[kk][1] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);

	bzero(txdataF2_eNB[kk][0],FRAME_LENGTH_BYTES_NO_PREFIX);
	bzero(txdataF2_eNB[kk][1],FRAME_LENGTH_BYTES_NO_PREFIX);

  	txdata_UE[kk]    = (int **)malloc16(2*sizeof(int*));
  	txdata_UE[kk][0] = (int *)malloc16(FRAME_LENGTH_BYTES);
  	txdata_UE[kk][1] = (int *)malloc16(FRAME_LENGTH_BYTES);

  	bzero(txdata_UE[kk][0],FRAME_LENGTH_BYTES);
  	bzero(txdata_UE[kk][1],FRAME_LENGTH_BYTES);

  	txdataF2_UE[kk] = (int **)malloc16(2*sizeof(int*));
  	txdataF2_UE[kk][0] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);
  	txdataF2_UE[kk][1] = (int *)malloc16(FRAME_LENGTH_BYTES_NO_PREFIX);

  	bzero(txdataF2_UE[kk][0],FRAME_LENGTH_BYTES_NO_PREFIX);
  	bzero(txdataF2_UE[kk][1],FRAME_LENGTH_BYTES_NO_PREFIX);
  }
//modif end UL
#else
  txdata_eNB[0] = PHY_vars_eNB[0]->lte_eNB_common_vars.txdata[eNB_id];
  txdata_eNB[1] = PHY_vars_eNB[1]->lte_eNB_common_vars.txdata[eNB_id];
//modif start UL
  txdata_UE[0] = PHY_vars_UE[0]->lte_ue_common_vars.txdata;
  txdata_UE[1] = PHY_vars_UE[1]->lte_ue_common_vars.txdata;
//modif end UL
#endif

printf("PHY_vars_eNB->lte_frame_parms.Ncp = %d\n", PHY_vars_eNB[0]->lte_frame_parms.Ncp);

  for (kk=0; kk<2; kk++) {
  	s_re_eNB[kk] = malloc(2*sizeof(double*));
  	s_im_eNB[kk] = malloc(2*sizeof(double*));
	s_re_UE[kk] = malloc(2*sizeof(double*));
	s_im_UE[kk] = malloc(2*sizeof(double*));
	r_re_2eNB[kk] = malloc(2*sizeof(double*));
	r_im_2eNB[kk] = malloc(2*sizeof(double*));
	r_re_2UE[kk] = malloc(2*sizeof(double*));
	r_im_2UE[kk] = malloc(2*sizeof(double*));
        for (ll=0; ll<2; ll++) {
  		r_re_eNB[kk][ll] = malloc(2*sizeof(double*));
  		r_im_eNB[kk][ll] = malloc(2*sizeof(double*));
		r_re_UE[ll][kk] = malloc(2*sizeof(double*));
	  	r_im_UE[ll][kk] = malloc(2*sizeof(double*));
	}
  }
  nsymb = (PHY_vars_eNB[0]->lte_frame_parms.Ncp == 0) ? 14 : 12;
//modif start UL
  //int dl_ch_estimates_norm[4][PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*nsymb/2];
  int n_K=15;
  int   dl_ch_estimates_length=(2*300*4)/dec_f;
  short dl_ch_estimates[2][dl_ch_estimates_length];
  bzero(dl_ch_estimates[0],(dl_ch_estimates_length));
  bzero(dl_ch_estimates[1],(dl_ch_estimates_length));

  short K_dl_ch_estimates[n_K][2][600];
  short K_drs_ch_estimates[n_K][2][600];

  double s_re_out[2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES], s_im_out[2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES], r_re_out[2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES], r_im_out[2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES];

for(aa=0;aa<2;aa++)
{
   for(k=0;k<n_K;k++)
    {
    bzero(K_dl_ch_estimates[k][aa],600);
    bzero(K_drs_ch_estimates[k][aa],600);
   }
}

  int drs_ch_estimates_length=(2*300*4)/dec_f;
  short drs_ch_estimates[2][drs_ch_estimates_length];
  bzero(drs_ch_estimates[0],(drs_ch_estimates_length));
  bzero(drs_ch_estimates[1],(drs_ch_estimates_length));

  coded_bits_per_codeword_UE = nb_rb_UE * (12 * get_Qm(mcs_UE)) * nsymb;
  rate_UE = (double)dlsch_tbs25[get_I_TBS(mcs_UE)][nb_rb_UE-1]/(coded_bits_per_codeword_UE);
//modif end UL

  printf("Channel Model=%d\n",channel_model);
  printf("SCM-A=%d, SCM-B=%d, SCM-C=%d, SCM-D=%d, EPA=%d, EVA=%d, ETU=%d, Rayleigh8=%d, Rayleigh1=%d, Rayleigh1_corr=%d, Rayleigh1_anticorr=%d, Rice1=%d, Rice8=%d\n",
	 SCM_A, SCM_B, SCM_C, SCM_D, EPA, EVA, ETU, Rayleigh8, Rayleigh1, Rayleigh1_corr, Rayleigh1_anticorr, Rice1, Rice8);
  sprintf(bler_fname,"second_bler_tx%d_mcs%d_chan%d.csv",transmission_mode,mcs_eNB,channel_model);
  bler_fd = fopen(bler_fname,"w");
  fprintf(bler_fd,"SNR; MCS; TBS; rate; err0; trials0; err1; trials1; err2; trials2; err3; trials3; dci_err\n");

  if(abstx){
    // CSV file 
    sprintf(csv_fname,"data_out%d.m",mcs_eNB);
    csv_fd = fopen(csv_fname,"w");
    fprintf(csv_fd,"data_all%d=[",mcs_eNB);
  }

  sprintf(tikz_fname, "second_bler_tx%d_u2=%d_mcs%d_chan%d_nsimus%d",transmission_mode,dual_stream_UE,mcs_eNB,channel_model,n_frames);
  tikz_fd = fopen(tikz_fname,"w");

  switch (mcs_eNB)
    {
    case 0:
      fprintf(tikz_fd,"\\addplot[color=blue, mark=star] plot coordinates {");
      break;
    case 1:
      fprintf(tikz_fd,"\\addplot[color=red, mark=star] plot coordinates {");
      break;
    case 2:
      fprintf(tikz_fd,"\\addplot[color=green, mark=star] plot coordinates {");
      break;
    case 3:
      fprintf(tikz_fd,"\\addplot[color=yellow, mark=star] plot coordinates {");
      break;
    case 4:
      fprintf(tikz_fd,"\\addplot[color=black, mark=star] plot coordinates {");
      break;
    case 5:
      fprintf(tikz_fd,"\\addplot[color=blue, mark=o] plot coordinates {");
      break;
    case 6:
      fprintf(tikz_fd,"\\addplot[color=red, mark=o] plot coordinates {");
      break;
    case 7:
      fprintf(tikz_fd,"\\addplot[color=green, mark=o] plot coordinates {");
      break;
    case 8:
      fprintf(tikz_fd,"\\addplot[color=yellow, mark=o] plot coordinates {");
      break;
    case 9:
      fprintf(tikz_fd,"\\addplot[color=black, mark=o] plot coordinates {");
      break;
    }
  for (i=0;i<2;i++) {
    for (kk=0; kk<2; kk++) {
	    s_re_eNB[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    	    s_im_eNB[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    s_re_UE[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    s_im_UE[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    r_re_2eNB[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
    	    r_im_2eNB[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    r_re_2UE[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    r_im_2UE[kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    for (ll=0; ll<2; ll++) {
		r_re_eNB[kk][ll][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    	r_im_eNB[kk][ll][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    	r_re_UE[ll][kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));
	    	r_im_UE[ll][kk][i] = malloc(FRAME_LENGTH_COMPLEX_SAMPLES*sizeof(double));  
	    }
    } 
  }

  UL_alloc_pdu.type    = 0;
  UL_alloc_pdu.rballoc = computeRIV(PHY_vars_eNB[0]->lte_frame_parms.N_RB_UL,first_rb,nb_rb_UE);// 12 RBs from position 8
  printf("rballoc %d (dci %x)\n",UL_alloc_pdu.rballoc,*(uint32_t *)&UL_alloc_pdu);
  UL_alloc_pdu.mcs     = mcs_UE;
  UL_alloc_pdu.ndi     = 1;
  UL_alloc_pdu.TPC     = 0;
  UL_alloc_pdu.cqi_req = 0;
  UL_alloc_pdu.cshift  = 0;
  UL_alloc_pdu.dai     = 1;

  //PHY_vars_UE->lte_ue_pdcch_vars[0]->crnti = n_rnti;
  
  for (kk=0; kk<2; kk++) {
	PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->crnti = 14;
	PHY_vars_UE[kk]->PHY_measurements.rank[0] = 0;
  	PHY_vars_UE[kk]->transmission_mode[0] = transmission_mode;
  	PHY_vars_UE[kk]->pucch_config_dedicated[0].tdd_AckNackFeedbackMode = bundling_flag == 1 ? bundling : multiplexing;
	PHY_vars_UE[kk]->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.groupHoppingEnabled = 1;
	PHY_vars_UE[kk]->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled = 0;
	PHY_vars_UE[kk]->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH = 0;
	init_ul_hopping(&PHY_vars_UE[kk]->lte_frame_parms);
	msg("Init UL hopping UE %d\n", kk);
  
  PHY_vars_eNB[kk]->transmission_mode[0] = transmission_mode;
  PHY_vars_eNB[kk]->pucch_config_dedicated[0].tdd_AckNackFeedbackMode = bundling_flag == 1 ? bundling : multiplexing;
  PHY_vars_eNB[kk]->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.groupHoppingEnabled = 1;
  PHY_vars_eNB[kk]->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.sequenceHoppingEnabled = 0;
  PHY_vars_eNB[kk]->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.groupAssignmentPUSCH = 0;
  msg("Init UL hopping eNB\n");

  init_ul_hopping(&PHY_vars_eNB[kk]->lte_frame_parms);

  }//end kk

// Fill in UL_alloc
  CCCH_alloc_pdu.type               = 0;
  CCCH_alloc_pdu.vrb_type           = 0;
  //CCCH_alloc_pdu.rballoc            = CCCH_RB_ALLOC;
  CCCH_alloc_pdu.ndi      = 1;
  CCCH_alloc_pdu.mcs      = 1;
  CCCH_alloc_pdu.harq_pid = 0;
//modif start UL
  DLSCH_alloc_pdu2.rah              = 0;
  DLSCH_alloc_pdu2.rballoc          = DLSCH_RB_ALLOC;
  DLSCH_alloc_pdu2.TPC              = 0;
  DLSCH_alloc_pdu2.dai              = 0;
  DLSCH_alloc_pdu2.harq_pid         = 0;
  DLSCH_alloc_pdu2.tb_swap          = 0;
  DLSCH_alloc_pdu2.mcs1             = mcs_UE;//to check
  DLSCH_alloc_pdu2.ndi1             = 1;
  DLSCH_alloc_pdu2.rv1              = 0;
  // Forget second codeword
  DLSCH_alloc_pdu2.tpmi             = 5 ;  // precoding
//modif end UL

  DLSCH_alloc_pdu2_2D[0].rah              = 0;
  DLSCH_alloc_pdu2_2D[0].rballoc          = DLSCH_RB_ALLOC;
  DLSCH_alloc_pdu2_2D[0].TPC              = 0;
  DLSCH_alloc_pdu2_2D[0].dai              = 0;
  DLSCH_alloc_pdu2_2D[0].harq_pid         = 0;
  DLSCH_alloc_pdu2_2D[0].tb_swap          = 0;
  DLSCH_alloc_pdu2_2D[0].mcs1             = mcs_eNB;  
  DLSCH_alloc_pdu2_2D[0].ndi1             = 1;
  DLSCH_alloc_pdu2_2D[0].rv1              = 0;
  // Forget second codeword
  DLSCH_alloc_pdu2_2D[0].tpmi             = (transmission_mode>=5 ? 5 : 0);  // precoding
  DLSCH_alloc_pdu2_2D[0].dl_power_off     = (transmission_mode==5 ? 0 : 1);

  DLSCH_alloc_pdu2_2D[1].rah              = 0;
  DLSCH_alloc_pdu2_2D[1].rballoc          = DLSCH_RB_ALLOC;
  DLSCH_alloc_pdu2_2D[1].TPC              = 0;
  DLSCH_alloc_pdu2_2D[1].dai              = 0;
  DLSCH_alloc_pdu2_2D[1].harq_pid         = 0;
  DLSCH_alloc_pdu2_2D[1].tb_swap          = 0;
  DLSCH_alloc_pdu2_2D[1].mcs1             = mcs_eNB;  
  DLSCH_alloc_pdu2_2D[1].ndi1             = 1;
  DLSCH_alloc_pdu2_2D[1].rv1              = 0;
  // Forget second codeword
  DLSCH_alloc_pdu2_2D[1].tpmi             = (transmission_mode>=5 ? 5 : 0);  // precoding
  DLSCH_alloc_pdu2_2D[1].dl_power_off     = (transmission_mode==5 ? 0 : 1);

  for (kk=0; kk<2; kk++) {
	  PHY_vars_eNB[kk]->dlsch_eNB_SI        = new_eNB_dlsch(1,1,0);
	  PHY_vars_eNB[kk]->dlsch_eNB_SI->rnti  = SI_RNTI;

	  PHY_vars_eNB[kk]->lte_frame_parms.soundingrs_ul_config_common.srs_BandwidthConfig = 2;
	  PHY_vars_eNB[kk]->lte_frame_parms.soundingrs_ul_config_common.srs_SubframeConfig = 7;

	  PHY_vars_eNB[kk]->soundingrs_ul_config_dedicated[UE_id].srs_ConfigIndex = 1;
	  PHY_vars_eNB[kk]->soundingrs_ul_config_dedicated[UE_id].srs_Bandwidth = 0;
	  PHY_vars_eNB[kk]->soundingrs_ul_config_dedicated[UE_id].transmissionComb = 0;
	  PHY_vars_eNB[kk]->soundingrs_ul_config_dedicated[UE_id].freqDomainPosition = 0;
	  PHY_vars_eNB[kk]->cooperation_flag = cooperation_flag;
	  
	  PHY_vars_eNB[kk]->pusch_config_dedicated[UE_id].betaOffset_ACK_Index = beta_ACK;
	  PHY_vars_eNB[kk]->pusch_config_dedicated[UE_id].betaOffset_RI_Index  = beta_RI;
	  PHY_vars_eNB[kk]->pusch_config_dedicated[UE_id].betaOffset_CQI_Index = beta_CQI;

	  PHY_vars_eNB[kk]->ulsch_eNB[0] = new_eNB_ulsch(3,0);
	  // Create transport channel structures for SI pdus
	  PHY_vars_UE[kk]->dlsch_ue_SI[0]  = new_ue_dlsch(1,1,0);
	  PHY_vars_UE[kk]->dlsch_ue_SI[0]->rnti = SI_RNTI;
          PHY_vars_UE[kk]->lte_frame_parms.soundingrs_ul_config_common.srs_BandwidthConfig = 2;
  	  PHY_vars_UE[kk]->lte_frame_parms.soundingrs_ul_config_common.srs_SubframeConfig = 7;
  	  PHY_vars_UE[kk]->soundingrs_ul_config_dedicated[eNB_id].srs_Bandwidth = 0;
  	  PHY_vars_UE[kk]->soundingrs_ul_config_dedicated[eNB_id].transmissionComb = 0;
  	  PHY_vars_UE[kk]->soundingrs_ul_config_dedicated[eNB_id].freqDomainPosition = 0;

          PHY_vars_UE[kk]->pusch_config_dedicated[eNB_id].betaOffset_ACK_Index = beta_ACK;
  	  PHY_vars_UE[kk]->pusch_config_dedicated[eNB_id].betaOffset_RI_Index  = beta_RI;
  	  PHY_vars_UE[kk]->pusch_config_dedicated[eNB_id].betaOffset_CQI_Index = beta_CQI;
          PHY_vars_UE[kk]->ulsch_ue[0]   = new_ue_ulsch(3,0);

	  for (ll=0; ll<2; ll++) {
  		eNB2UE[kk][ll] = new_channel_desc_scm(PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx,
						PHY_vars_UE[ll]->lte_frame_parms.nb_antennas_rx,
						channel_model,
						BW,
						forgetting_factor,
						rx_sample_offset,
						(kk==ll ? 0 : -100));

  	       UE2eNB[ll][kk] = new_channel_desc_scm(PHY_vars_UE[ll]->lte_frame_parms.nb_antennas_tx,//b
			        	PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_rx,//b
                                	channel_model,
                                	BW,
                                	forgetting_factor,
					0, //rx_sample_offset
			        	(kk==ll ? 0 : -100));

	  	if (eNB2UE[kk][ll]==NULL) {
    			msg("Problem generating channel model. Exiting.\n");
    			exit(-1);
  	  	}
          }
  
          printf("PUSCH Beta : ACK %f, RI %f, CQI %f\n",(double)beta_ack[beta_ACK]/8,(double)beta_ri[beta_RI]/8,(double)beta_cqi[beta_CQI]/8);
  
          // Create transport channel structures for 2 transport blocks (MIMO)
          for (i=0;i<2;i++) {
      		PHY_vars_eNB[kk]->dlsch_eNB[0][i] = new_eNB_dlsch(1,8,0);
      
     	 if (!PHY_vars_eNB[kk]->dlsch_eNB[0][i]) {
		printf("Can't get eNB dlsch structures\n");
		exit(-1);
      	}
      
      PHY_vars_eNB[kk]->dlsch_eNB[0][i]->rnti = n_rnti+0;
    }

  	for (i=0;i<2;i++) {
    		PHY_vars_UE[kk]->dlsch_ue[0][i]  = new_ue_dlsch(1,8,0);
    		if (!PHY_vars_UE[kk]->dlsch_ue[0][i]) {
      			printf("Can't get ue dlsch structures for ant %d\n", kk);
      			exit(-1);
    		}    
    	PHY_vars_UE[kk]->dlsch_ue[0][i]->rnti = n_rnti; //b Check rnti numb
  	}
  
  	generate_ue_ulsch_params_from_dci((void *)&UL_alloc_pdu,
				    	14,
				    	(subframe_UL<4)?(subframe_UL+6):(subframe_UL-4),
				    	format0,
				    	PHY_vars_UE[kk],
				    	SI_RNTI,
				    	RA_RNTI,
				   	P_RNTI,
				    	0,
				    	srs_flag);

  	PHY_vars_UE[kk]->ulsch_ue[0]->o_ACK[0] = 1;
  

        generate_eNB_ulsch_params_from_dci((DCI0_5MHz_TDD_1_6_t *)&UL_alloc_pdu,
				       14,
				       (subframe_UL<4)?(subframe_UL+6):(subframe_UL-4),
				       format0,
				       0,
				       PHY_vars_eNB[kk],
				       SI_RNTI,
				       RA_RNTI,
				       P_RNTI,
				       srs_flag);
  
  if (DLSCH_alloc_pdu2_2D[0].tpmi == 5)
    PHY_vars_eNB[kk]->eNB_UE_stats[0].DL_pmi_single = (unsigned short)(taus()&0xffff);
  else 
    PHY_vars_eNB[kk]->eNB_UE_stats[0].DL_pmi_single = 0;



  if (input_fd==NULL) {

      printf("Generating dlsch params for user\n");
      generate_eNB_dlsch_params_from_dci(0,
					 &DLSCH_alloc_pdu2_2D[0],
					 n_rnti+0,
					 format2_2D_M10PRB,
					 PHY_vars_eNB[kk]->dlsch_eNB[0],
					 &PHY_vars_eNB[kk]->lte_frame_parms,
					 SI_RNTI,
					 RA_RNTI,
					 P_RNTI,
					 PHY_vars_eNB[kk]->eNB_UE_stats[0].DL_pmi_single);

    num_dci = 0;
    num_ue_spec_dci = 0;
    num_common_dci = 0;
   

    // UE specific DCI
      memcpy(&dci_alloc[num_dci].dci_pdu[0],&DLSCH_alloc_pdu2_2D[0],sizeof(DCI2_5MHz_2D_M10PRB_TDD_t));
      dci_alloc[num_dci].dci_length = sizeof_DCI2_5MHz_2D_M10PRB_TDD_t;
      dci_alloc[num_dci].L          = 2;
      dci_alloc[num_dci].rnti       = n_rnti+0;
      dci_alloc[num_dci].format     = format2_2D_M10PRB;

      dump_dci(&PHY_vars_eNB[kk]->lte_frame_parms,&dci_alloc[num_dci]);

      num_dci++;
      num_ue_spec_dci++;

      input_buffer_length[kk] = PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->TBS/8;
      input_buffer[kk] = (unsigned char *)malloc(input_buffer_length[kk]+4);
      memset(input_buffer[kk],0,input_buffer_length[kk]+4);
    
      if (input_trch_file==0) {
	for (i=0;i<input_buffer_length[kk];i++) {
	  input_buffer[kk][i]= (unsigned char)(taus()&0xff);
	}
      }
      
      else {
	i=0;
	while ((!feof(input_trch_fd)) && (i<input_buffer_length[kk]<<3)) {
	  fscanf(input_trch_fd,"%s",input_trch_val);
	  if (input_trch_val[0] == '1')
	    input_buffer[kk][i>>3]+=(1<<(7-(i&7)));
	  if (i<16)
	    printf("input_trch_val %d : %c\n",i,input_trch_val[0]);
	  i++;
	  if (((i%8) == 0) && (i<17))
	    printf("%x\n",input_buffer[kk][(i-1)>>3]);
	}
	printf("Read in %d bits\n",i);
      }
  }



  if (PHY_vars_eNB[kk]->lte_frame_parms.Ncp == 0) {  // normal prefix
	    pilot1 = 4;
	    pilot2 = 7;
	    pilot3 = 11;
	  }
	  else {  // extended prefix
	    pilot1 = 3;
	    pilot2 = 6;
	    pilot3 = 9;
	  }
  }// end kk

  int prec_length = 2*nsymb*PHY_vars_eNB[0]->lte_frame_parms.ofdm_symbol_size;
  short prec[prec_length];
  double Norm[prec_length];

  for (ch_realization=0;ch_realization<n_ch_rlz;ch_realization++){
    if(abstx){
      printf("**********************Channel Realization Index = %d **************************\n", ch_realization);
    }
    for (SNR=snr0;SNR<snr1;SNR+=snr_step) {

      P_eNb_active = 0;
      for (kk=0; kk<2; kk++) {
	for (aa=0; aa<4; aa++) {
	      errs_eNB[kk][aa]=0;
	      round_trials_eNB[kk][aa] = 0;
              errs_UE[kk][aa]=0;
	      round_trials_UE[kk][aa] = 0;
      	}
	for (ll=0; ll<2; ll++) {
	      random_channel(eNB2UE[kk][ll]);
      	      UE2eNB[ll][kk] = eNB2UE[kk][ll];
	}//end ll
	      round_UE[kk] = 0;
	      round_eNB[kk] = 0; 
	      dci_errors[kk] = 0;
              avg_ber[kk] = 0;
      }//end kk

     
      llb=0;

    randominit(0);

    for (kk=0; kk<2; kk++) {//kk for UE
      
    	harq_pid[kk] = subframe2harq_pid(&PHY_vars_UE[kk]->lte_frame_parms,subframe_UL);

    	if (input_fd == NULL) {
      		input_buffer_length_UE[kk] = PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[harq_pid[kk]]->TBS/8;
  	
      		//input_buffer_UE = (unsigned char *)malloc(input_buffer_length_UE+4);//b
      		input_buffer_UE[kk] = (char *)malloc(input_buffer_length_UE[kk]+4);//b
      		mac_xface->frame=1;
      		if (n_frames == 1) {
			trch_out_fd = fopen("ulsch_trch.txt","w");
			for (i=0;i<input_buffer_length_UE[kk];i++) {
	  			input_buffer_UE[kk][i] = taus()&0xff;
	  			for (j=0;j<8;j++)
	    				fprintf(trch_out_fd,"%d\n",(input_buffer_UE[kk][i]>>(7-j))&1);
			} //exit(-1);
    
		fclose(trch_out_fd);
      		}
    	}
    	else {
      	n_frames=1;
      	i=0;
      	while (!feof(input_fd)) {
		fscanf(input_fd,"%s %s",input_val_str,input_val_str2);//&input_val1,&input_val2);
	
		if ((i%4)==0) {
	  		((short*)txdata_UE[kk][0])[i/2] = (short)((1<<15)*strtod(input_val_str,NULL));
	 		((short*)txdata_UE[kk][0])[(i/2)+1] = (short)((1<<15)*strtod(input_val_str2,NULL));
	  		if ((i/4)<100)
	    			printf("sample %d => %e + j%e (%d +j%d)\n",i/4,strtod(input_val_str,NULL),strtod(input_val_str2,NULL),((short*)txdata_UE[kk][0])[i/4],((short*)txdata_UE[kk][0])[(i/4)+1]);//1,input_val2,);
		}
		i++;
		if (i>(FRAME_LENGTH_SAMPLES))
	  		break;
      	}
      	printf("Read in %d samples\n",i/4);
      	write_output("txsig0_UE.m","txs0_UE", txdata_UE[kk][0],2*frame_parms->samples_per_tti,1,1);

      	tx_lev_UE[kk] = signal_energy(&txdata_UE[kk][0][0],
			     	OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
      	tx_lev_UE_dB[kk] = (unsigned int) dB_fixed(tx_lev_UE[kk]); 
  	}
   }// end kk
      
      for (trials = 0;trials<n_frames;trials++) {
	//  printf("Trial %d\n",trials);
	fflush(stdout);

	for (kk=0; kk<2; kk++) {    
	round_eNB[kk]=0;  
	round_UE[kk]=0; 
	for (ll=0; ll<2; ll++)
		eNB2UE[kk][ll]->first_run = 1;
	}

	while ((round_eNB[0] < num_rounds)||(round_eNB[1] < num_rounds)) {
	  

	  if(transmission_mode>=5)
	    pmi_feedback=1;
	  else 
	    pmi_feedback=0;

	PMI_FEEDBACK:
	
for (kk=0; kk<2; kk++) {
        round_trials_eNB[kk][round_eNB[kk]]++;	
	round_trials_UE[kk][round_UE[kk]]++;
    for (aa=0; aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx;aa++) {
#ifdef IFFT_FPGA
	    memset(&PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa][0],0,NUMBER_OF_USEFUL_CARRIERS*NUMBER_OF_SYMBOLS_PER_FRAME*sizeof(mod_sym_t));
#else
	    memset(&PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa][0],0,FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX*sizeof(mod_sym_t));
#endif
	  }
	  if (input_fd==NULL) {

	    if (round_UE[kk] == 0) {
	      PHY_vars_eNB[kk]->ulsch_eNB[0]->harq_processes[0]->Ndi = 1;
	      PHY_vars_eNB[kk]->ulsch_eNB[0]->harq_processes[0]->rvidx = round_UE[kk]>>1;
	      
	      	PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[0]->Ndi = 1;
	      	PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[0]->rvidx = round_UE[kk]>>1;
	    }
	    else {
              PHY_vars_eNB[kk]->ulsch_eNB[0]->harq_processes[0]->Ndi = 0;
	      PHY_vars_eNB[kk]->ulsch_eNB[0]->harq_processes[0]->rvidx = round_UE[kk]>>1;
	      		PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[0]->Ndi = 0;
       	      		PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[0]->rvidx = round_UE[kk]>>1;
	    }

	    if (round_eNB[kk] == 0) {
	      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 1;
	      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round_eNB[kk]>>1;
	      DLSCH_alloc_pdu2_2D[0].ndi1             = 1;
	      DLSCH_alloc_pdu2_2D[0].rv1              = 0;
	      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu2_2D[0],sizeof(DCI2_5MHz_2D_M10PRB_TDD_t));
	    }
	    else {

	      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 0;
	      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round_eNB[kk]>>1;
	      DLSCH_alloc_pdu2_2D[0].ndi1             = 0;
	      DLSCH_alloc_pdu2_2D[0].rv1              = round_eNB[kk]>>1;
	      memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu2_2D[0],sizeof(DCI2_5MHz_2D_M10PRB_TDD_t));
	    }

//********************** DL part ****************************************************************************
	    num_pdcch_symbols_2 = generate_dci_top(num_ue_spec_dci,
						   num_common_dci,
						   dci_alloc,
						   0,
						   1024,
						   &PHY_vars_eNB[kk]->lte_frame_parms,
						   PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id],
						   subframe_DL);

	    if (num_pdcch_symbols_2 > num_pdcch_symbols) {
	      msg("Error: given num_pdcch_symbols not big enough\n");
	      exit(-1);
	    }

	      coded_bits_per_codeword = get_G(&PHY_vars_eNB[kk]->lte_frame_parms,
					      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->nb_rb,
					      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->rb_alloc,
					      get_Qm(PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->mcs),
					      num_pdcch_symbols,
					      subframe_DL);
	      
#ifdef TBS_FIX
	      tbs = (double)3*dlsch_tbs25[get_I_TBS(PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->mcs)][PHY_vars_eNB[kk]->dlsch_eNB[0][0]->nb_rb-1]/4;
#else
	      tbs = (double)dlsch_tbs25[get_I_TBS(PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->mcs)][PHY_vars_eNB[kk]->dlsch_eNB[0][0]->nb_rb-1];
#endif
	      
	      rate_eNB = (double)tbs/(double)coded_bits_per_codeword;

	      uncoded_ber_bit = (short*) malloc(2*coded_bits_per_codeword);
	      
	      if (trials==0 && round_eNB[0]==0 && kk==0) 
		printf("\nRate = %f (G %d, TBS %d, TBS_UE[%d] %d, mod %d, pdcch_sym %d)\n",
		       rate_eNB,
		       coded_bits_per_codeword,
		       tbs,
		       kk,
		       PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[harq_pid[0]]->TBS,
		       get_Qm(PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->mcs),
		       num_pdcch_symbols);	   
	      
		
	      // use the PMI from previous trial
	      if (DLSCH_alloc_pdu2_2D[0].tpmi == 5) {
			PHY_vars_eNB[kk]->dlsch_eNB[0][0]->pmi_alloc = quantize_subband_pmi(&PHY_vars_UE[kk]->PHY_measurements,0);
			PHY_vars_UE[kk]->dlsch_ue[0][0]->pmi_alloc = quantize_subband_pmi(&PHY_vars_UE[kk]->PHY_measurements,0);
	      }
	      
	      if (dlsch_encoding(input_buffer[kk],
				 &PHY_vars_eNB[kk]->lte_frame_parms,
				 num_pdcch_symbols,
				 PHY_vars_eNB[kk]->dlsch_eNB[0][0],
				 subframe_DL)<0)
		exit(-1);
	      
	      // printf("Did not Crash here 1\n");
	      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->rnti = n_rnti+0;	  
	      dlsch_scrambling(&PHY_vars_eNB[kk]->lte_frame_parms,
			       num_pdcch_symbols,
			       PHY_vars_eNB[kk]->dlsch_eNB[0][0],
			       coded_bits_per_codeword,
			       0,
			       subframe_DL<<1);
	      if (transmission_mode == 5) {
		amp = (int16_t)(((int32_t)1024*ONE_OVER_SQRT2_Q15)>>15);
	      }
	      else
		amp = 1024;

	      re_allocated = dlsch_modulation(PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id],
					      amp,
					      subframe_DL,
					      &PHY_vars_eNB[kk]->lte_frame_parms,
					      num_pdcch_symbols,
					      PHY_vars_eNB[kk]->dlsch_eNB[0][0]);

	      if (num_layers>1)
		re_allocated = dlsch_modulation(PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id],
						1024,
						subframe_DL,
						&PHY_vars_eNB[kk]->lte_frame_parms,
						num_pdcch_symbols,
						PHY_vars_eNB[kk]->dlsch_eNB[0][1]);
	    
	       generate_pilots(PHY_vars_eNB[kk],
			    PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id],
			    1024,
			    LTE_NUMBER_OF_SUBFRAMES_PER_FRAME);

#ifdef IFFT_FPGA 
	    // do table lookup and write results to txdataF2
	    for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx;aa++) {
	      ind = 0;
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++) 
		if (((i%512)>=1) && ((i%512)<=150)) {
		  txdataF2_eNB[kk][aa][i] = ((int*)mod_table)[PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa][ind++]];
		}
		else if ((i%512)>=362) {
		  txdataF2_eNB[kk][aa][i] = ((int*)mod_table)[PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa][ind++]];
		}
		else {
		  txdataF2_eNB[kk][aa][i] = 0;
		}
	    }

            for (aa=0;aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_tx;aa++) {
	      ind = 0;
	      for (i=0;i<FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX;i++) 
		if (((i%512)>=1) && ((i%512)<=150)) {
		  txdataF2_UE[kk][aa][i] = ((int*)mod_table)[PHY_vars_UE[kk]->lte_ue_common_vars.txdataF[aa][ind++]];
		}
		else if ((i%512)>=362) {
                  txdataF2_UE[kk][aa][i] = ((int*)mod_table)[PHY_vars_UE[kk]->lte_ue_common_vars.txdataF[aa][ind++]];	
		}
		else {
		  txdataF2_UE[kk][aa][i] = 0;
		}
	    }

	    tx_lev_eNB[kk] = 0;
	    for (aa=0; aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx; aa++) {
	      if (frame_parms->Ncp == 1)
		PHY_ofdm_mod(&txdataF2_eNB[kk][aa][subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size],        // input
			     &txdata_eNB[kk][aa][subframe_DL*PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti],         // output
			     PHY_vars_eNB[kk]->lte_frame_parms.log2_symbol_size,                // log2_fft_size
			     2*nsymb,//NUMBER_OF_SYMBOLS_PER_FRAME,                 // number of symbols
			     PHY_vars_eNB[kk]->lte_frame_parms.nb_prefix_samples,               // number of prefix samples
			     PHY_vars_eNB[kk]->lte_frame_parms.twiddle_ifft,  // IFFT twiddle factors
			     PHY_vars_eNB[kk]->lte_frame_parms.rev,           // bit-reversal permutation
			     CYCLIC_PREFIX);
	      else {
		normal_prefix_mod(&txdataF2_eNB[kk][aa][subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size],
				  &txdata_eNB[kk][aa][subframe_DL*PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti],
				  2*nsymb,
				  frame_parms);
	      }
	      tx_lev_eNB[kk] += signal_energy(&txdata_eNB[kk][aa][(PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size+PHY_vars_eNB[kk]->lte_frame_parms.nb_prefix_samples0)],
				      OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    }	 
#else //IFFT_FPGA

//Precoding
	    if ((P_eNb_active == 1) && (kk == 1)) {

            for (aa=0; aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx; aa++) {

               // write_output("txaF.m","txaF11", &PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0][0],(subframe_DL+1)*nsymb*PHY_vars_eNB[0]->lte_frame_parms.ofdm_symbol_size,1,1);

/*
for (i=0; i<10; i++) {
  printf("PHY_vars_eNB[%d]=%d\n", i, PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0][i+subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]);
  printf("PHY_vars_eNB[%d]=%d\n", i, ((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0])[2*i+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]);
  printf("PHY_vars_eNB[%d]=%d\n", i, ((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0])[2*i+1+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]);
}*/
		do_precoding(PHY_vars_eNB[kk], PHY_vars_UE[kk], PeNb_factor, prec, Norm, nsymb, UE_id, aa);

//for (i=0; i<10; i++)
//printf("Norm[%d] = %d\n", i, prec[2*i]);

//printf("subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size = %d\n", subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size);

//for (i=0; i<10; i++)
//printf("enb[%d] = %d\n", i, ((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0])[2*i+subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]);


for (i=0; i<nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size; i++) {

	if ((prec[2*i]*prec[2*i] + prec[2*i+1]*prec[2*i+1]) != 0) {
				((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa])[2*i+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size] = (short)(((((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa])[2*i+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]*prec[2*i] + ((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa])[2*i+1+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]*prec[2*i+1]))/(prec[2*i]*prec[2*i] + prec[2*i+1]*prec[2*i+1]));
				((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa])[2*i+1+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size] = (short)(((((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa])[2*i+1+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]*prec[2*i] - ((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa])[2*i+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]*prec[2*i+1]))/(prec[2*i]*prec[2*i] + prec[2*i+1]*prec[2*i+1]));
}
			}
/*for (i=100; i<110; i++) {
  printf("PHY_vars_eNB[%d]=%d\n", i, PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0][i+subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]);
  printf("PHY_vars_eNB[%d]=%d\n", i, ((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0])[2*i+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]);
  printf("PHY_vars_eNB[%d]=%d\n", i, ((short *)PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0])[2*i+1+2*subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size]);
  printf("PREC[%d]=%d\n", i, prec[2*i]);
  printf("PREC[%d]=%d\n", i, prec[2*i+1]);
  //printf("Norm[%d] = %d\n", i, (short)Norm[2*i]);
  //printf("Norm[%d] = %f\n", i, Norm[2*i]);
	    }

 exit(-1);*/
}//aa
}//if
	    //write_output("txaF.m","txaF", PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][0],nsymb*PHY_vars_eNB[0]->lte_frame_parms.ofdm_symbol_size,1,1);

	    tx_lev_eNB[kk] = 0;
	    for (aa=0; aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx; aa++) {
	      if (frame_parms->Ncp == 1)
		PHY_ofdm_mod(&PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa][subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size],        // input
			     &txdata_eNB[kk][aa][subframe_DL*PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti],         // output
			     PHY_vars_eNB[kk]->lte_frame_parms.log2_symbol_size,                // log2_fft_size
			     2*nsymb,//NUMBER_OF_SYMBOLS_PER_FRAME,                 // number of symbols
			     PHY_vars_eNB[kk]->lte_frame_parms.nb_prefix_samples,               // number of prefix samples
			     PHY_vars_eNB[kk]->lte_frame_parms.twiddle_ifft,  // IFFT twiddle factors
			     PHY_vars_eNB[kk]->lte_frame_parms.rev,           // bit-reversal permutation
			     CYCLIC_PREFIX);
	      else {
		normal_prefix_mod(&PHY_vars_eNB[kk]->lte_eNB_common_vars.txdataF[eNB_id][aa][subframe_DL*nsymb*PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size],
				  &txdata_eNB[kk][aa][subframe_DL*PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti],
				  2*nsymb,
				  frame_parms);
	      }
	    
	      tx_lev_eNB[kk] += signal_energy(&txdata_eNB[kk][aa][subframe_DL*PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti],
				      PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti);
	   }
#endif //IFFT_FPGA
           tx_lev_eNB_dB[kk] = (unsigned int) dB_fixed(tx_lev_eNB[kk]);

}// input_fd
else {  // Read signal from file
	    i=0;
	    while (!feof(input_fd)) {
	      fscanf(input_fd,"%s %s",input_val_str,input_val_str2);
	    
	      if ((i%4)==0) {
		((short*)txdata_eNB[kk][0])[i/2] = (short)((1<<15)*strtod(input_val_str,NULL));
		((short*)txdata_eNB[kk][0])[(i/2)+1] = (short)((1<<15)*strtod(input_val_str2,NULL));
		if ((i/4)<100)
		  printf("sample %d => %e + j%e (%d +j%d)\n",i/4,strtod(input_val_str,NULL),strtod(input_val_str2,NULL),((short*)txdata_eNB[kk][0])[i/4],((short*)txdata_eNB[kk][0])[(i/4)+1]);//1,input_val2,);
	      }
	      i++;
	      if (i>(FRAME_LENGTH_SAMPLES))
		break;
	    }
	    printf("Read in %d samples\n",i/4);

	    tx_lev_eNB[kk] = signal_energy(&txdata_eNB[kk][0][0],
				   OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);
	    tx_lev_eNB_dB[kk] = (unsigned int) dB_fixed(tx_lev_eNB[kk]);
	  }// else read from file

	  for (i=0;i<2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES;i++) {
	    for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx;aa++) {
	      if (awgn_flag == 0) {
		s_re_eNB[kk][aa][i] = ((double)(((short *)txdata_eNB[kk][aa]))[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti) + (i<<1)]);
		s_im_eNB[kk][aa][i] = ((double)(((short *)txdata_eNB[kk][aa]))[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti) +(i<<1)+1]);
	      }
	      else {
		for (aarx=0;aarx<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aarx++) {
                  for (ll=0; ll<2; ll++) {
                  if (ll==kk) {
		  if (aa==0) {
		    r_re_eNB[kk][ll][aarx][i] = ((double)(((short *)txdata_eNB[kk][aa]))[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti) +(i<<1)]);
		    r_im_eNB[kk][ll][aarx][i] = ((double)(((short *)txdata_eNB[kk][aa]))[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti) +(i<<1)+1]);
		  }
		  else {
		    r_re_eNB[kk][ll][aarx][i] += ((double)(((short *)txdata_eNB[kk][aa]))[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti) +(i<<1)]);
		    r_im_eNB[kk][ll][aarx][i] += ((double)(((short *)txdata_eNB[kk][aa]))[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti) +(i<<1)+1]);
		  }
		  } else
			{
			r_re_eNB[kk][ll][aarx][i] = 0;
		    	r_im_eNB[kk][ll][aarx][i] = 0;
			}
 		 }
		}
	      }
          	  r_re_2eNB[kk][aa][i] = 0;
		  r_im_2eNB[kk][aa][i] = 0;
	    }
	  }

	  // filtre RF tx -> s_re
	  if ((decalibration == 1) && (kk == 1)) {
		  for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx;aa++) {
			  real_fir(s_re_eNB[kk][aa], s_im_eNB[kk][aa], s_re_out, s_im_out, s_coeffs_eNB, s_ord_fir_eNB, 2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);

		  for (i=0;i<2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES;i++) {
			  s_re_eNB[kk][aa][i] = s_re_out[i];
			  s_im_eNB[kk][aa][i] = s_im_out[i];
		  }
		  }
	  }

	  //	n0_pow_dB = tx_lev_dB + 10*log10(512/(NB_RB*12)) + SNR;
	  // generate new channel if pmi_feedback==0, otherwise hold channel
        for (ll=0; ll<2; ll++) {
	  if(abstx){
	    if (trials==0 && round_eNB[kk]==0){
	      if (awgn_flag == 0) {
			
		if(SNR==snr0){
		  if(pmi_feedback==0)
		    multipath_channel(eNB2UE[kk][ll],s_re_eNB[kk],s_im_eNB[kk],r_re_eNB[kk][ll],r_im_eNB[kk][ll],
				      2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1);
		  else 
		    multipath_channel(eNB2UE[kk][ll],s_re_eNB[kk],s_im_eNB[kk],r_re_eNB[kk][ll],r_im_eNB[kk][ll],
				      2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1);//b
		}else{
		    multipath_channel(eNB2UE[kk][ll],s_re_eNB[kk],s_im_eNB[kk],r_re_eNB[kk][ll],r_im_eNB[kk][ll],
				    2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1);
		}
		
		freq_channel(eNB2UE[kk][ll], 25,51);
		snr=pow(10.0,.1*SNR);
		fprintf(csv_fd,"%f,",SNR);
		
		for (u=0;u<50;u++){
		  abs_channel = (eNB2UE[kk][ll]->chF[0][u].x*eNB2UE[kk][ll]->chF[0][u].x + eNB2UE[kk][ll]->chF[0][u].y*eNB2UE[kk][ll]->chF[0][u].y);
		  if(transmission_mode==5){
		    fprintf(csv_fd,"%e,",abs_channel);
		  }
		  else{
		    pilot_sinr = 10*log10(snr*abs_channel);
		    fprintf(csv_fd,"%e,",pilot_sinr);
		  } 
		}
	      }
	    }
	    
	    else{
	      if (awgn_flag == 0) {	
		multipath_channel(eNB2UE[kk][ll],s_re_eNB[kk],s_im_eNB[kk],r_re_eNB[kk][ll],r_im_eNB[kk][ll],
				  2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1);
	      }
	    }
	  }
	  
	  else{ //ABStraction
	    if (awgn_flag == 0) {	

	      if (pmi_feedback==0) {
			if (trials<n_K-1)
		multipath_channel(eNB2UE[kk][ll],s_re_eNB[kk],s_im_eNB[kk],r_re_eNB[kk][ll],r_im_eNB[kk][ll],
				  2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1);//b
		else 
		multipath_channel(eNB2UE[kk][ll],s_re_eNB[kk],s_im_eNB[kk],r_re_eNB[kk][ll],r_im_eNB[kk][ll],
				  2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1);//b
 			}
	      else 
		multipath_channel(eNB2UE[kk][ll],s_re_eNB[kk],s_im_eNB[kk],r_re_eNB[kk][ll],r_im_eNB[kk][ll],
				  2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,1);//b
	    }
	  }//ABStraction

	if ((phase_offset == 1) && (kk == 1) && (ll == 1)) {
		  for (aa=0;aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
			  phase_offsets(r_re_eNB[kk][ll][aa], r_im_eNB[kk][ll][aa], r_re_out, r_im_out, 2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES, &phase_in_DL, phase_inc, 1);

		  for (i=0;i<2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES;i++) {
			  r_re_eNB[kk][ll][aa][i] = r_re_out[i];
			  r_im_eNB[kk][ll][aa][i] = r_im_out[i];
		  }
		  }
	  }
	  
	  // filtre RF rx -> r_re_eNB
	 if ((decalibration == 1) && (kk == 1) && (ll == 1)) {
		  for (aa=0;aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
			  real_fir(r_re_eNB[kk][ll][aa], r_im_eNB[kk][ll][aa], r_re_out, r_im_out, r_coeffs_eNB, r_ord_fir_eNB, 2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);

		  for (i=0;i<2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES;i++) {
			  r_re_eNB[kk][ll][aa][i] = r_re_out[i];
			  r_im_eNB[kk][ll][aa][i] = r_im_out[i];
		  }
		  }
	  }
	}//end ll
}//end kk
for (kk=0; kk<2; kk++) {
	  for (ll=0; ll<2; ll++) {
	    for (i=0; i<2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++) {
	      for (aarx=0;aarx<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aarx++) {
          	r_re_2eNB[kk][aarx][i] += r_re_eNB[ll][kk][aarx][i];
		r_im_2eNB[kk][aarx][i] += r_im_eNB[ll][kk][aarx][i];
	      }
	    }
	  }

	

	  sigma2_eNB_dB[kk] = 10*log10((double)tx_lev_eNB[kk]) +10*log10(PHY_vars_eNB[kk]->lte_frame_parms.ofdm_symbol_size/(NB_RB*12)) - SNR;
	  
	  //AWGN
	  sigma2_eNB[kk] = pow(10,sigma2_eNB_dB[kk]/10);
	  	
	  if (pmi_feedback==0) {  
	    for (i=0; i<2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++) {
	      for (aa=0;aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
		((short*) PHY_vars_UE[kk]->lte_ue_common_vars.rxdata[aa])[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti)+2*i] = 
		  (short) (r_re_2eNB[kk][aa][i] + sqrt(sigma2_eNB[kk]/2)*gaussdouble(0.0,1.0));
		((short*) PHY_vars_UE[kk]->lte_ue_common_vars.rxdata[aa])[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti)+2*i+1] = 
		  (short) (r_im_2eNB[kk][aa][i] + (iqim*r_re_2eNB[kk][aa][i]) + sqrt(sigma2_eNB[kk]/2)*gaussdouble(0.0,1.0));
	      }
	    }   
	  }
	  else {
	    for (i=0; i<2*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++) {
	      for (aa=0;aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
		((short*) PHY_vars_UE[kk]->lte_ue_common_vars.rxdata[aa])[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti)+2*i] = (short) (r_re_2eNB[kk][aa][i]);
		((short*) PHY_vars_UE[kk]->lte_ue_common_vars.rxdata[aa])[(2*subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti)+2*i+1] = (short) (r_im_2eNB[kk][aa][i]);
	      }
	    }
	  }

// Inner receiver scheduling for 3 slots
	  for (Ns=(2*subframe_DL);Ns<((2*subframe_DL)+3);Ns++) {
	    for (l=0;l<pilot2;l++) {	     
	      slot_fep(PHY_vars_UE[kk],
		       l,
		       Ns%20,
		       0,
		       0);

#ifdef PERFECT_CE
	      if (awgn_flag==0) {
		// fill in perfect channel estimates
		freq_channel(eNB2UE[kk][kk],PHY_vars_UE[kk]->lte_frame_parms.N_RB_DL,301);
		//write_output("channel.m","ch",desc1->ch[0],desc1->channel_length,1,8);
		//write_output("channelF.m","chF",desc1->chF[0],nb_samples,1,8);
		for(k=0;k<NUMBER_OF_eNB_MAX;k++) {
		  for(aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx;aa++) 
		    { 
		      for (aarx=0;aarx<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aarx++)
			{
			  for (i=0;i<frame_parms->N_RB_DL*12;i++)
			    { 
			      ((int16_t *) PHY_vars_UE[kk]->lte_ue_common_vars.dl_ch_estimates[k][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE[kk][kk]->chF[aarx+(aa*PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx)][i].x*AMP/2);
			      ((int16_t *) PHY_vars_UE[kk]->lte_ue_common_vars.dl_ch_estimates[k][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE[kk][kk]->chF[aarx+(aa*PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx)][i].y*AMP/2) ;
			    }
			}
		    }
		  }
	      }
	      else {
		for(aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_tx;aa++) 
		  { 
		    for (aarx=0;aarx<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aarx++)
		      {
			for (i=0;i<frame_parms->N_RB_DL*12;i++)
			  { 
			    ((int16_t *) PHY_vars_UE[kk]->lte_ue_common_vars.dl_ch_estimates[0][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=AMP/2;
			    ((int16_t *) PHY_vars_UE[kk]->lte_ue_common_vars.dl_ch_estimates[0][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=0/2;
			  }
		      }
		  }
	      }
#endif
	    if (trials<=n_K) {

                do_quantization_UE(PHY_vars_UE[kk],
			nsymb, 
			pilot1-1, 
			quant_v, 
			dl_ch_estimates[kk],
			dec_f);
		}

}//Ns
}//l
}//end kk


//**************************** UL Part ********************************************************
for (kk=0; kk<2; kk++) {
	for (i=0;i<input_buffer_length_UE[kk];i++)
			input_buffer_UE[kk][i] = taus()&0xff;
			//input_buffer_UE[kk][i]=(char)(dl_ch_estimates[kk][i]);

	if (input_fd==NULL) {
#ifdef OFDMA_ULSCH
	  if (srs_flag)
	  	generate_srs_tx(PHY_vars_UE[kk],0,AMP,subframe_UL);

	  generate_drs_pusch(PHY_vars_UE[kk],0,AMP,subframe_UL,first_rb,nb_rb_UE);
	  
#else
	  if (srs_flag)
	    	generate_srs_tx(PHY_vars_UE[kk],0,scfdma_amps[nb_rb_UE],subframe_UL);
	  
	  generate_drs_pusch(PHY_vars_UE[kk],0,
			     scfdma_amps[nb_rb_UE],
			     subframe_UL,
			     PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[0]->first_rb,
			     PHY_vars_UE[kk]->ulsch_ue[0]->harq_processes[0]->nb_rb);
#endif	
	  //printf("harq_pid = %d\n\n",harq_pid);
	  if (ulsch_encoding(input_buffer_UE[kk], //prob
			     &PHY_vars_UE[kk]->lte_frame_parms,
			     PHY_vars_UE[kk]->ulsch_ue[0],
			     harq_pid[kk],
			     transmission_mode, // transmission mode
			     control_only_flag,
			     1// Nbundled
			     )==-1) {
	    printf("ulsim.c Problem with ulsch_encoding\n");
	    exit(-1);
	  }
	  
#ifdef OFDMA_ULSCH
	  ulsch_modulation(PHY_vars_UE[kk]->lte_ue_common_vars.txdataF,AMP,subframe_UL,&PHY_vars_UE[kk]->lte_frame_parms,PHY_vars_UE[kk]->ulsch_ue[0],cooperation_flag);
#else  
	  //	printf("Generating PUSCH in subframe %d with amp %d, nb_rb %d\n",subframe,scfdma_amps[nb_rb_UE],nb_rb_UE);
	  ulsch_modulation(PHY_vars_UE[kk]->lte_ue_common_vars.txdataF,scfdma_amps[nb_rb_UE],
			   subframe_UL,&PHY_vars_UE[kk]->lte_frame_parms,
			   PHY_vars_UE[kk]->ulsch_ue[0],cooperation_flag);
#endif	     

#ifdef IFFT_FPGA
	    for (aa=0; aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx; aa++)  {
		for (kk=0; kk<2; kk++) {// UE 0 and UE 1
	    		if (frame_parms->Ncp == 1)
	      			PHY_ofdm_mod(txdataF2_UE[kk][aa],        // input
			   			txdata_UE[kk][aa],         // output
			   			PHY_vars_UE[kk]->lte_frame_parms.log2_symbol_size,                // log2_fft_size
			   			nsymb,                 // number of symbols
			   			PHY_vars_UE[kk]->lte_frame_parms.nb_prefix_samples,               // number of prefix samples
			   			PHY_vars_UE[kk]->lte_frame_parms.twiddle_ifft,  // IFFT twiddle factors
			   			PHY_vars_UE[kk]->lte_frame_parms.rev,           // bit-reversal permutation
			   			CYCLIC_PREFIX);
	    		else 
	      			normal_prefix_mod(txdataF2_UE[kk][aa],txdata_UE[kk][aa],nsymb,frame_parms);
		} 
	     } 
#else //IFFT_FPGA 
	  tx_lev_UE[kk]=0;
	  for (aa=0; aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx; aa++) {
		    	if (frame_parms->Ncp == 1) 
		     		PHY_ofdm_mod(&PHY_vars_UE[kk]->lte_ue_common_vars.txdataF[aa][subframe_UL*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES_NO_PREFIX],        // input
						   &txdata_UE[kk][aa][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL],         // output
						   PHY_vars_UE[kk]->lte_frame_parms.log2_symbol_size,                // log2_fft_size
						   nsymb,                 // number of symbols
						   PHY_vars_UE[kk]->lte_frame_parms.nb_prefix_samples,               // number of prefix samples
						   PHY_vars_UE[kk]->lte_frame_parms.twiddle_ifft,  // IFFT twiddle factors
						   PHY_vars_UE[kk]->lte_frame_parms.rev,           // bit-reversal permutation
						   CYCLIC_PREFIX);
		    	else
		      		normal_prefix_mod(&PHY_vars_UE[kk]->lte_ue_common_vars.txdataF[aa][subframe_UL*nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES_NO_PREFIX],
						&txdata_UE[kk][aa][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL],
						nsymb,
						frame_parms);
		    
#ifndef OFDMA_ULSCH
		    apply_7_5_kHz(PHY_vars_UE[kk],subframe_UL<<1);
		    apply_7_5_kHz(PHY_vars_UE[kk],1+(subframe_UL<<1));
#endif
		    
		    tx_lev_UE[kk] += signal_energy(&txdata_UE[kk][aa][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL],
					  OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES);

	}
#endif //IFFT_FPGA
} //input_fd

        // multipath channel
	tx_lev_UE_dB[kk] = (unsigned int) dB_fixed(tx_lev_UE[kk]);
 	    	
	for (i=0;i<PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti;i++) {
		    if (awgn_flag == 0) {
                      for (aarx=0;aarx<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_tx;aarx++) {
		        s_re_UE[kk][aarx][i] = ((double)(((short *)&txdata_UE[kk][aarx][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL]))[(i<<1)]);
		        s_im_UE[kk][aarx][i] = ((double)(((short *)&txdata_UE[kk][aarx][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL]))[(i<<1)+1]);
                      }
		    }
		    else {
                    for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
		      for (ll=0; ll<2; ll++) {
                        if (kk == ll) {
		      		r_re_UE[kk][ll][aa][i] = ((double)(((short *)&txdata_UE[kk][aa][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL]))[(i<<1)]);
		      		r_im_UE[kk][ll][aa][i] = ((double)(((short *)&txdata_UE[kk][aa][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL]))[(i<<1)+1]);
			} else
			{
				r_re_UE[kk][ll][aa][i] = 0;
		      		r_im_UE[kk][ll][aa][i] = 0;
			}		    
		      }
                    }
		  }
		for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
          	  r_re_2UE[kk][aa][i] = 0;
		  r_im_2UE[kk][aa][i] = 0;
		}
	  }

	  // filtre RF tx -> s_re_UE
	  if ((decalibration == 1) && (kk == 1)) {
		  for (aa=0;aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_tx;aa++) {
			  real_fir(s_re_UE[kk][aa], s_im_UE[kk][aa], s_re_out, s_im_out, s_coeffs_UE, s_ord_fir_UE, PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti);

		  for (i=0;i<PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti;i++) {
			  s_re_UE[kk][aa][i] = s_re_out[i];
			  s_im_UE[kk][aa][i] = s_im_out[i];
		  }
		  }
	  }

	  if ((phase_offset == 1) && (kk == 1)) {
		  for (aa=0;aa<PHY_vars_UE[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
			  phase_in_DL = phase_in_UL;
			  phase_offsets(s_re_UE[kk][aa], s_im_UE[kk][aa], s_re_out, s_im_out, PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti, &phase_in_UL, phase_inc, -1);

		  for (i=0;i<PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti;i++) {
			  s_re_UE[kk][aa][i] = s_re_out[i];
			  s_im_UE[kk][aa][i] = s_im_out[i];
		  }
		  }
	  }

	  for (ll=0; ll<2; ll++) {

          if (awgn_flag == 0) {
		
		if (trials<n_K-1)
			multipath_channel(UE2eNB[ll][kk],s_re_UE[ll],s_im_UE[ll],r_re_UE[ll][kk],r_im_UE[ll][kk],
				  PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti,1);
		else 
			multipath_channel(UE2eNB[ll][kk],s_re_UE[ll],s_im_UE[ll],r_re_UE[ll][kk],r_im_UE[ll][kk],
				  PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti,1);
	 }

		
	// filtre RF rx -> r_re
	if ((decalibration == 1) && (kk == 1) && (ll == 1)) {
		  for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
			  real_fir(r_re_UE[ll][kk][aa], r_im_UE[ll][kk][aa], r_re_out, r_im_out, r_coeffs_UE, r_ord_fir_UE, PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti);

		  for (i=0;i<PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti;i++) {
			  r_re_UE[ll][kk][aa][i] = r_re_out[i];
			  r_im_UE[ll][kk][aa][i] = r_im_out[i];
		  }
		  }
        }
	}// end ll
   }//end kk

//**************************************************************************************


         for (kk=0; kk<2; kk++) {
	  for (ll=0; ll<2; ll++) {
	    for (i=0; i<PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti; i++) {
	      for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
          	r_re_2UE[kk][aa][i] += r_re_UE[ll][kk][aa][i];
		r_im_2UE[kk][aa][i] += r_im_UE[ll][kk][aa][i];
	      }
	    }
	  }

	sigma2_UE_dB[kk] = tx_lev_UE_dB[kk] +10*log10(PHY_vars_UE[kk]->lte_frame_parms.ofdm_symbol_size/(PHY_vars_UE[kk]->lte_frame_parms.N_RB_DL*12)) - SNR;
	//AWGN
	sigma2_UE[kk] = pow(10,sigma2_UE_dB[kk]/10);

        for (i=0; i<PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti; i++) {
	  for (aa=0;aa<PHY_vars_eNB[kk]->lte_frame_parms.nb_antennas_rx;aa++) {
	    ((short*) &PHY_vars_eNB[kk]->lte_eNB_common_vars.rxdata[0][aa][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL])[2*i] = (short) (r_re_2UE[kk][aa][i] + sqrt(sigma2_UE[kk]/2)*gaussdouble(0.0,1.0));
	    ((short*) &PHY_vars_eNB[kk]->lte_eNB_common_vars.rxdata[0][aa][PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL])[2*i+1] = (short) (r_im_2UE[kk][aa][i] + (iqim*r_re_2UE[kk][aa][i]) + sqrt(sigma2_UE[kk]/2)*gaussdouble(0.0,1.0));
	  }
	} 	  
	  i_mod = get_Qm(mcs_eNB);
	  
	  // Inner receiver scheduling for 3 slots
	  for (Ns=(2*subframe_DL);Ns<((2*subframe_DL)+3);Ns++) {
	    for (l=0;l<pilot2;l++) {	     
	      if ((Ns==(2+(2*subframe_DL))) && (l==0)) {
		lte_ue_measurements(PHY_vars_UE[kk],
				    subframe_DL*PHY_vars_UE[kk]->lte_frame_parms.samples_per_tti,
				    1,
				    0);
		    
		if (transmission_mode==5 || transmission_mode==6) {
		  if (pmi_feedback==1) {
		    pmi_feedback= 0;
		    //		    printf("measured PMI %x\n",pmi2hex_2Ar1(quantize_subband_pmi(&PHY_vars_UE->PHY_measurements,0)));
		    goto PMI_FEEDBACK;
		  }
		}

	      }


	      if ((Ns==(2*subframe_DL)) && (l==pilot1)) {// process symbols 0,1,2

		if (dci_flag == 1) {
		  rx_pdcch(&PHY_vars_UE[kk]->lte_ue_common_vars,
			   PHY_vars_UE[kk]->lte_ue_pdcch_vars,
			   &PHY_vars_UE[kk]->lte_frame_parms,
			   subframe_DL,
			   0,
			   (PHY_vars_UE[kk]->lte_frame_parms.mode1_flag == 1) ? SISO : ALAMOUTI,
			   0);

		  // overwrite number of pdcch symbols
		  PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->num_pdcch_symbols = num_pdcch_symbols;

		  dci_cnt = dci_decoding_procedure(PHY_vars_UE[kk],
						   dci_alloc_rx,
						   eNB_id,
						   subframe_DL,
						   SI_RNTI,
						   RA_RNTI);
		  //printf("dci_cnt %d\n",dci_cnt);
		
		  if (dci_cnt==0) {
		    dlsch_active = 0;
		    if (round_eNB[kk]==0) {
		      dci_errors[kk]++;
		      round_eNB[kk]=5;
		      errs_eNB[kk][0]++;
		      round_trials_eNB[kk][0]++;
		    }
		  }
		
		  for (i=0;i<dci_cnt;i++) {
		    //printf("Generating dlsch parameters for RNTI %x\n",dci_alloc_rx[i].rnti);
		    if ((dci_alloc_rx[i].rnti == n_rnti) && 
			(generate_ue_dlsch_params_from_dci(0,
							   dci_alloc_rx[i].dci_pdu,
							   dci_alloc_rx[i].rnti,
							   dci_alloc_rx[i].format,
							   PHY_vars_UE[kk]->dlsch_ue[0],
							   &PHY_vars_UE[kk]->lte_frame_parms,
							   SI_RNTI,
							   RA_RNTI,
							   P_RNTI)==0)) {
		      //dump_dci(&PHY_vars_UE->lte_frame_parms,&dci_alloc_rx[i]);
		      coded_bits_per_codeword = get_G(&PHY_vars_eNB[kk]->lte_frame_parms,
						      PHY_vars_UE[kk]->dlsch_ue[0][0]->nb_rb,
						      PHY_vars_UE[kk]->dlsch_ue[0][0]->rb_alloc,
						      get_Qm(PHY_vars_UE[kk]->dlsch_ue[0][0]->harq_processes[PHY_vars_UE[kk]->dlsch_ue[0][0]->current_harq_pid]->mcs),
						      PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->num_pdcch_symbols,
						      subframe_DL);
		      dlsch_active = 1;
		    }
		    else {
		      dlsch_active = 0;
		      if (round_eNB[kk]==0) {
			dci_errors[kk]++;
			errs_eNB[kk][0]++;
			round_trials_eNB[kk][0]++;

			if (n_frames==1) {
			  printf("DCI misdetection trial %d\n",trials);
			  round_eNB[kk]=5;
			}
		      }
		    }
		  }
		}  // if dci_flag==1
		else { //dci_flag == 0

		  PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->crnti = n_rnti;
		  PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->num_pdcch_symbols = num_pdcch_symbols;

		  generate_ue_dlsch_params_from_dci(0,
						    &DLSCH_alloc_pdu2_2D[0],
						    C_RNTI,
						    format2_2D_M10PRB,
						    PHY_vars_UE[kk]->dlsch_ue[0],
						    &PHY_vars_UE[kk]->lte_frame_parms,
						    SI_RNTI,
						    RA_RNTI,
						    P_RNTI);
		  dlsch_active = 1;
		} // if dci_flag == 1
	      }

	      if (dlsch_active == 1) {
		if ((Ns==(1+(2*subframe_DL))) && (l==0)) {// process symbols 3,4,5

		  for (m=PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->num_pdcch_symbols;
		       m<pilot2;
		       m++) {
		    if (rx_dlsch(&PHY_vars_UE[kk]->lte_ue_common_vars,
				 PHY_vars_UE[kk]->lte_ue_dlsch_vars,
				 &PHY_vars_UE[kk]->lte_frame_parms,
				 eNB_id,
				 eNB_id_i,
				 PHY_vars_UE[kk]->dlsch_ue[0],
				 subframe_DL,
				 m,
				 (m==PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->num_pdcch_symbols)?1:0,
				 dual_stream_UE,
				 &PHY_vars_UE[kk]->PHY_measurements,
				 i_mod)==-1) {

		      dlsch_active = 0;
		      break;
		    }
		  }
	       
		}
		  
		if ((Ns==(1+(2*subframe_DL))) && (l==pilot1)) {// process symbols 6,7,8
		  
		  for (m=pilot2;
		       m<pilot3;
		       m++)
		    if (rx_dlsch(&PHY_vars_UE[kk]->lte_ue_common_vars,
				 PHY_vars_UE[kk]->lte_ue_dlsch_vars,
				 &PHY_vars_UE[kk]->lte_frame_parms,
				 eNB_id,
				 eNB_id_i,
				 PHY_vars_UE[kk]->dlsch_ue[0],
				 subframe_DL,
				 m,
				 0,
				 dual_stream_UE,
				 &PHY_vars_UE[kk]->PHY_measurements,
				 i_mod)==-1) {
		      dlsch_active=0;
		      break;
		    }
		}
	      
		if ((Ns==(2+(2*subframe_DL))) && (l==0))  // process symbols 10,11, do deinterleaving for TTI
		  for (m=pilot3;
		       m<PHY_vars_UE[kk]->lte_frame_parms.symbols_per_tti;
		       m++)
		    if (rx_dlsch(&PHY_vars_UE[kk]->lte_ue_common_vars,
				 PHY_vars_UE[kk]->lte_ue_dlsch_vars,
				 &PHY_vars_UE[kk]->lte_frame_parms,
				 eNB_id,
				 eNB_id_i,
				 PHY_vars_UE[kk]->dlsch_ue[0],
				 subframe_DL,
				 m,
				 0,
				 dual_stream_UE,
				 &PHY_vars_UE[kk]->PHY_measurements,
				 i_mod)==-1) {
		      dlsch_active=0;
		      break;
		    }
                if ((SNR==snr0) && (llb==0))  {
			llb=1;

		 
		  dump_dlsch2(PHY_vars_UE[kk],eNB_id,coded_bits_per_codeword);
		  dump_dlsch2(PHY_vars_UE[kk],eNB_id_i,coded_bits_per_codeword);
		  write_output("dlsch_e.m","e",PHY_vars_eNB[kk]->dlsch_eNB[0][0]->e,coded_bits_per_codeword,1,4);
		}
	      
	      }
	    }
	  }

	  // calculate uncoded BLER
	  uncoded_ber[kk]=0;
	  for (i=0;i<coded_bits_per_codeword;i++) 
	    if (PHY_vars_eNB[kk]->dlsch_eNB[0][0]->e[i] != (PHY_vars_UE[kk]->lte_ue_dlsch_vars[0]->llr[0][i]<0)) {
	      uncoded_ber_bit[i] = 1;
	      uncoded_ber[kk]++;
	    }
	    else
	      uncoded_ber_bit[i] = 0;

	  uncoded_ber[kk]/=coded_bits_per_codeword;
	  avg_ber[kk] += uncoded_ber[kk];
	 
	  //imran
	  if(abstx){
	    if (trials<10 && round_eNB[kk]==0 && transmission_mode==5){
	      for (iii=0; iii<NB_RB; iii++){
		//fprintf(csv_fd, "%d, %d", (PHY_vars_UE->lte_ue_dlsch_vars[eNB_id]->pmi_ext[iii]),(PHY_vars_UE->lte_ue_dlsch_vars[eNB_id_i]->pmi_ext[iii]));
		msg(" %x",(PHY_vars_UE[kk]->lte_ue_dlsch_vars[eNB_id]->pmi_ext[iii]));
		// msg("Opposite Extracted pmi %x\n",(PHY_vars_UE->lte_ue_dlsch_vars[eNB_id_i]->pmi_ext[iii]));
				    
	      }
	    }
	  }
	 

	  PHY_vars_UE[kk]->dlsch_ue[0][0]->rnti = n_rnti;
	  dlsch_unscrambling(&PHY_vars_UE[kk]->lte_frame_parms,
			     PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->num_pdcch_symbols,
			     PHY_vars_UE[kk]->dlsch_ue[0][0],
			     coded_bits_per_codeword,
			     PHY_vars_UE[kk]->lte_ue_dlsch_vars[eNB_id]->llr[0],
			     0,
			     subframe_DL<<1);
	  

	  ret_eNB[kk] = dlsch_decoding(PHY_vars_UE[kk]->lte_ue_dlsch_vars[eNB_id]->llr[0],		 
			       &PHY_vars_UE[kk]->lte_frame_parms,
			       PHY_vars_UE[kk]->dlsch_ue[0][0],
			       subframe_DL,
			       PHY_vars_UE[kk]->lte_ue_pdcch_vars[0]->num_pdcch_symbols);

#ifdef XFORMS
	  do_forms(form,
		   &PHY_vars_UE[kk]->lte_frame_parms,  
		   PHY_vars_UE[kk]->lte_ue_common_vars.dl_ch_estimates_time,
		   PHY_vars_UE[kk]->lte_ue_common_vars.dl_ch_estimates[eNB_id],
		   PHY_vars_UE[kk]->lte_ue_common_vars.rxdata,
		   PHY_vars_UE[kk]->lte_ue_common_vars.rxdataF,
		   PHY_vars_UE[kk]->lte_ue_dlsch_vars[0]->rxdataF_comp[0],
		   PHY_vars_UE[kk]->lte_ue_dlsch_vars[3]->rxdataF_comp[0],
		   PHY_vars_UE[kk]->lte_ue_dlsch_vars[0]->dl_ch_rho_ext[0],
		   PHY_vars_UE[kk]->lte_ue_dlsch_vars[0]->llr[0],coded_bits_per_codeword); 
#endif
	  if (ret_eNB[kk] <= MAX_TURBO_ITERATIONS) {
	    if (fix_rounds==0)
	      round_eNB[kk]=5;
	    else
	      round_eNB[kk]++;
	  }	
	  else {
	    errs_eNB[kk][round_eNB[kk]]++;
	    round_eNB[kk]++;
	  }
	//********************** DL part end
        
	//********************** DL Channel Feedback

        //****************************** UL Decoding Proc

//modif start UL
SNRmeas[kk] = 10*log10(((double)signal_energy((int*)&PHY_vars_eNB[kk]->lte_eNB_common_vars.rxdata[0][0][160+(PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*subframe_UL)],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2))/((double)signal_energy((int*)&PHY_vars_eNB[kk]->lte_eNB_common_vars.rxdata[0][0][160+(PHY_vars_eNB[kk]->lte_frame_parms.samples_per_tti*(1+subframe_UL))],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)) - 1);
     
#ifndef OFDMA_ULSCH
	remove_7_5_kHz(PHY_vars_eNB[kk],subframe_UL<<1);
	remove_7_5_kHz(PHY_vars_eNB[kk],1+(subframe_UL<<1));
#endif  
	for (l=subframe_UL*PHY_vars_UE[kk]->lte_frame_parms.symbols_per_tti;l<((1+subframe_UL)*PHY_vars_UE[kk]->lte_frame_parms.symbols_per_tti);l++) {
	
	  slot_fep_ul(&PHY_vars_eNB[kk]->lte_frame_parms,
		      &PHY_vars_eNB[kk]->lte_eNB_common_vars,
		      l%(PHY_vars_eNB[kk]->lte_frame_parms.symbols_per_tti/2),
		      l/(PHY_vars_eNB[kk]->lte_frame_parms.symbols_per_tti/2),
		      0,
		      0);
	}



	PHY_vars_eNB[kk]->ulsch_eNB[0]->cyclicShift = cyclic_shift;// cyclic shift for DMRS
	rx_ulsch(PHY_vars_eNB[kk],
		 subframe_UL,
		 0,  // this is the effective sector id
		 0,  // this is the UE_id
		 PHY_vars_eNB[kk]->ulsch_eNB,
		 cooperation_flag);

	ret_UE[kk]= ulsch_decoding(PHY_vars_eNB[kk],
			    	0, // UE_id
			    	subframe_UL,
			    	control_only_flag,
			    	1  // Nbundled 
			    	);

	if (ret_UE[kk] <= MAX_TURBO_ITERATIONS)
	  round_UE[kk]=5;	
	else {
	  errs_UE[kk][round_UE[kk]]++;
	  round_UE[kk]++;
	}  // ulsch error
	
	if (trials<=n_K) {
	do_quantization_eNB(PHY_vars_eNB[kk], 
		PHY_vars_UE[kk],
		nsymb, 
		pilot1-1, //pilot ant 0
		pilot1, //pilot ant 1 
		quant_v, 
		drs_ch_estimates[kk], 
		UE_id);
	}
}//end kk

	// Calibration
	if (trials <= n_K) {

		for (aa=0; aa<PHY_vars_eNB[1]->lte_frame_parms.nb_antennas_rx; aa++)		
			for (k=0; k<2*300; k++) {
				K_dl_ch_estimates[trials][aa][k] = dl_ch_estimates[1][k+aa*2*300];
				K_drs_ch_estimates[trials][aa][k] = drs_ch_estimates[1][k+aa*2*300];
			}

	} else if ( (trials>n_K) && (P_eNb_active==0)) {
	
		do_calibration (K_dl_ch_estimates,
				K_drs_ch_estimates, 
				PeNb_factor,
				PHY_vars_eNB[1]->lte_frame_parms.ofdm_symbol_size,
				n_K);

		P_eNb_active=1;
		
        
        //write_output("cal1.m","cal", PeNb_factor[1],600,1,8);

	//write_output("aue1.m","aue", drs_ch_estimates[0],1200,1,1);
	//write_output("aenb1.m","aenb", dl_ch_estimates[0],1200,1,1);
        //write_output("vulb0.m","vul0", PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0]->drs_ch_estimates[UE_id][0],5000,1,1);
        //write_output("vulb.m","vul", PHY_vars_eNB[1]->lte_eNB_ulsch_vars[0]->drs_ch_estimates[UE_id][1],5000,1,1);
	//write_output("vdlb.m","vdl", PHY_vars_UE[1]->lte_ue_common_vars.dl_ch_estimates[eNB_id][0], 5000,1,1);

//exit(-1);
	
	}
	
//modif end UL
	    
	}  //round

	//if ((errs_eNB[0]>=100) && (trials>(n_frames/2)) && (errs_UE[0]>=100) )
	//  break;  //b 
      
      }   //trials

      for (kk=0; kk<2; kk++) {
      printf("\n*******DL %d *************SNR = %f dB (tx_lev_eNB %f, sigma2_eNB_dB %f)************DL************\n",
	     kk,
	     SNR,
	     (double)tx_lev_eNB_dB[kk]+10*log10(PHY_vars_UE[kk]->lte_frame_parms.ofdm_symbol_size/(NB_RB*12)),
	     sigma2_eNB_dB[kk]);
    
      printf("Errors (%d/%d %d/%d %d/%d %d/%d), Pe = (%e,%e,%e,%e), dci_errors %d/%d, Pe = %e => effective rate %f (%f), normalized delay %f (%f), uncoded_ber %f\n",
	     errs_eNB[kk][0],
	     round_trials_eNB[kk][0],
	     errs_eNB[kk][1],
	     round_trials_eNB[kk][1],
	     errs_eNB[kk][2],
	     round_trials_eNB[kk][2],
	     errs_eNB[kk][3],
	     round_trials_eNB[kk][3],
	     (double)errs_eNB[kk][0]/(round_trials_eNB[kk][0]),
	     (double)errs_eNB[kk][1]/(round_trials_eNB[kk][1]),
	     (double)errs_eNB[kk][2]/(round_trials_eNB[kk][2]),
	     (double)errs_eNB[kk][3]/(round_trials_eNB[kk][3]),
	     dci_errors[kk],
	     round_trials_eNB[kk][0],
	     (double)dci_errors[kk]/(round_trials_eNB[kk][0]),
	     rate_eNB*((double)(round_trials_eNB[kk][0]-dci_errors[kk])/((double)round_trials_eNB[kk][0] + round_trials_eNB[kk][1] + round_trials_eNB[kk][2] + round_trials_eNB[kk][3])),
	     rate_eNB,
	     (1.0*(round_trials_eNB[kk][0]-errs_eNB[kk][0])+2.0*(round_trials_eNB[kk][1]-errs_eNB[kk][1])+3.0*(round_trials_eNB[kk][2]-errs_eNB[kk][2])+4.0*(round_trials_eNB[kk][3]-errs_eNB[kk][3]))/((double)round_trials_eNB[kk][0])/(double)PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->TBS,
	     (1.0*(round_trials_eNB[kk][0]-errs_eNB[kk][0])+2.0*(round_trials_eNB[kk][1]-errs_eNB[kk][1])+3.0*(round_trials_eNB[kk][2]-errs_eNB[kk][2])+4.0*(round_trials_eNB[kk][3]-errs_eNB[kk][3]))/((double)round_trials_eNB[kk][0]),
	     avg_ber[kk]/round_trials_eNB[kk][0]);
    
      fprintf(bler_fd,"%f;%d;%d;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d;%f\n",
	      SNR,
	      mcs_eNB,
	      PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->TBS,
	      rate_eNB,
	      errs_eNB[kk][0],
	      round_trials_eNB[kk][0],
	      errs_eNB[kk][1],
	      round_trials_eNB[kk][1],
	      errs_eNB[kk][2],
	      round_trials_eNB[kk][2],
	      errs_eNB[kk][3],
	      round_trials_eNB[kk][3],
	      dci_errors[kk],
	      avg_ber[kk]/round_trials_eNB[kk][0]);

      fprintf(tikz_fd,"(%f,%f)", SNR, (float)errs_eNB[kk][0]/round_trials_eNB[kk][0]);
    
      if(abstx){ //ABSTRACTION         
	blerr= (double)errs_eNB[kk][0]/(round_trials_eNB[kk][0]);
	fprintf(csv_fd,"%e;\n",blerr);
      } //ABStraction

      printf("\n++++++UL %d +++++++++++++SNR = %f dB (tx_UE_lev %f, sigma2_UE_dB %f)++++++++++++UL+++++++++++\n",
           kk,
	   SNR,
	   (double)tx_lev_UE_dB[kk]+10*log10(PHY_vars_UE[kk]->lte_frame_parms.ofdm_symbol_size/(nb_rb_UE*12)),
	   sigma2_UE_dB[kk]);
    
    printf("Errors (%d/%d %d/%d %d/%d %d/%d), Pe = (%e,%e,%e,%e) => effective rate_UL %f (%f), normalized delay %f (%f)\n",
	   errs_UE[kk][0],
	   round_trials_UE[kk][0],
	   errs_UE[kk][1],
	   round_trials_UE[kk][1],
	   errs_UE[kk][2],
	   round_trials_UE[kk][2],
	   errs_UE[kk][3],
	   round_trials_UE[kk][3],
	   (double)errs_UE[kk][0]/(round_trials_UE[kk][0]),
	   (double)errs_UE[kk][1]/(round_trials_UE[kk][1]),
	   (double)errs_UE[kk][2]/(round_trials_UE[kk][2]),
	   (double)errs_UE[kk][3]/(round_trials_UE[kk][3]),
	   rate_UE*((double)(round_trials_UE[kk][0])/((double)round_trials_UE[kk][0] + round_trials_UE[kk][1] + round_trials_UE[kk][2] + round_trials_UE[kk][3])),
	   rate_UE,
	   (1.0*(round_trials_UE[kk][0]-errs_UE[kk][0])+2.0*(round_trials_UE[kk][1]-errs_UE[kk][1])+3.0*(round_trials_UE[kk][2]-errs_UE[kk][2])+4.0*(round_trials_UE[kk][3]-errs_UE[kk][3]))/((double)round_trials_UE[kk][0])/(double)PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->TBS,
	   (1.0*(round_trials_UE[kk][0]-errs_UE[kk][0])+2.0*(round_trials_UE[kk][1]-errs_UE[kk][1])+3.0*(round_trials_UE[kk][2]-errs_UE[kk][2])+4.0*(round_trials_UE[kk][3]-errs_UE[kk][3]))/((double)round_trials_UE[kk][0]));
    
    fprintf(bler_fd,"%f;%d;%d;%f;%d;%d;%d;%d;%d;%d;%d;%d\n",
	    SNR,
	    mcs_UE,
	    PHY_vars_eNB[kk]->dlsch_eNB[0][0]->harq_processes[0]->TBS,
	    rate_UE,
	    errs_UE[kk][0],
	    round_trials_UE[kk][0],
	    errs_UE[kk][1],
	    round_trials_UE[kk][1],
	    errs_UE[kk][2],
	    round_trials_UE[kk][2],
	    errs_UE[kk][3],
	    round_trials_UE[kk][3]);
    }//end kk

    }// SNR 
  
  } //ch_realization
  
  
  fclose(bler_fd);
  fprintf(tikz_fd,"};\n");
  fclose(tikz_fd);

  if (input_trch_file==1)
    fclose(input_trch_fd);
  if (input_file==1)
    fclose(input_fd);
  if(abstx){// ABSTRACTION
    fprintf(csv_fd,"];");
    fclose(csv_fd);
  }
 
  printf("Freeing dlsch structures\n");
  for (i=0;i<2;i++) {
    printf("eNB 0 %d\n",i);
    free_eNB_dlsch(PHY_vars_eNB[0]->dlsch_eNB[0][i]);
    printf("eNB 1 %d\n",i);
    free_eNB_dlsch(PHY_vars_eNB[1]->dlsch_eNB[0][i]);
    printf("UE 0 %d\n",i);
    free_ue_dlsch(PHY_vars_UE[0]->dlsch_ue[0][i]);
    printf("UE 1 %d\n",i);
    free_ue_dlsch(PHY_vars_UE[1]->dlsch_ue[0][i]);
  }
  
  
#ifdef IFFT_FPGA
  printf("Freeing transmit signals\n");
  for (kk=0; kk<2; kk++) {
        free(txdataF2_eNB[kk][0]);
	  free(txdataF2_eNB[kk][1]);
	  free(txdataF2_eNB[kk]);
	  free(txdata_eNB[kk][0]);
	  free(txdata_eNB[kk][1]);
	  free(txdata_eNB[kk]);

  	free(txdataF2_UE[kk][0]);
  	free(txdataF2_UE[kk][1]);
  	free(txdataF2_UE[kk]);
  	free(txdata_UE[kk][0]);
  	free(txdata_UE[kk][1]);
  	free(txdata_UE[kk]);
  }
//modif end UL
#endif
  
  printf("Freeing channel I/O\n");
  for (i=0;i<2;i++) {
    for (kk=0; kk<2; kk++) {
        free(s_re_eNB[kk][i]);
    	free(s_im_eNB[kk][i]);
    	free(s_re_UE[kk][i]);
    	free(s_im_UE[kk][i]);
	free(r_re_2eNB[kk][i]);
    	free(r_im_2eNB[kk][i]);
    	free(r_re_2UE[kk][i]);
    	free(r_im_2UE[kk][i]);
	for (ll=0; ll<2; ll++) {
		free(r_re_eNB[kk][ll][i]);
    		free(r_im_eNB[kk][ll][i]);
    		free(r_re_UE[ll][kk][i]);
    		free(r_im_UE[ll][kk][i]);
	}
    }
  }

  for (kk=0; kk<2; kk++) {
   	for (ll=0; ll<2; ll++) {
  		free(r_re_eNB[kk][ll]);
		free(r_im_eNB[kk][ll]);
        	free(r_re_UE[ll][kk]);
		free(r_im_UE[ll][kk]);
	}
	free(s_re_eNB[kk]);
  	free(s_im_eNB[kk]); 
	free(s_re_UE[kk]);
	free(s_im_UE[kk]);
	free(r_re_2eNB[kk]);
  	free(r_im_2eNB[kk]); 
	free(r_re_2UE[kk]);
	free(r_im_2UE[kk]);
	
  }
//modif end UL
  //  lte_sync_time_free();
  
  return(0);
}
  
