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
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "MAC_INTERFACE/defs.h"
#include "MAC_INTERFACE/extern.h"

#define DEBUG_PHY

// Adjust location synchronization point to account for drift
// The adjustment is performed once per frame based on the
// last channel estimate of the receiver

void lte_adjust_synch(LTE_DL_FRAME_PARMS *frame_parms,
		      PHY_VARS_UE *phy_vars_ue,
		      unsigned char eNB_id,
		      unsigned char clear,
		      short coef)
{

  static int max_pos_fil = 0;
  int temp = 0, i, aa, max_val = 0, max_pos = 0;
  int diff;
  short Re,Im,ncoef;

  ncoef = 32767 - coef;

#ifdef DEBUG_PHY
  LOG_D(PHY,"frame %d: rx_offset (before) = %d\n",phy_vars_ue->frame_rx,phy_vars_ue->rx_offset);
#endif //DEBUG_PHY


  // we only use channel estimates from tx antenna 0 here
  for (i = 0; i < frame_parms->nb_prefix_samples; i++) {
    temp = 0;
    for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
      Re = ((int16_t*)phy_vars_ue->lte_ue_common_vars.dl_ch_estimates_time[eNB_id][aa])[(i<<2)];
      Im = ((int16_t*)phy_vars_ue->lte_ue_common_vars.dl_ch_estimates_time[eNB_id][aa])[1+(i<<2)];
      temp += (Re*Re/2) + (Im*Im/2);
    }
    if (temp > max_val) {
      max_pos = i;
      max_val = temp;
    }
  }

  // filter position to reduce jitter
  if (clear == 1)
    max_pos_fil = max_pos;
  else
    max_pos_fil = ((max_pos_fil * coef) + (max_pos * ncoef)) >> 15;


  diff = max_pos_fil - 2; //frame_parms->nb_prefix_samples/8;

  if ( diff > SYNCH_HYST )
    phy_vars_ue->rx_offset++;
  else if (diff < -SYNCH_HYST)
    phy_vars_ue->rx_offset--;
    
  if ( phy_vars_ue->rx_offset < 0 )
    phy_vars_ue->rx_offset += FRAME_LENGTH_COMPLEX_SAMPLES;

  if ( phy_vars_ue->rx_offset >= FRAME_LENGTH_COMPLEX_SAMPLES )
    phy_vars_ue->rx_offset -= FRAME_LENGTH_COMPLEX_SAMPLES;



#ifdef DEBUG_PHY
  LOG_D(PHY,"frame %d: rx_offset (after) = %d : max_pos = %d,max_pos_fil = %d (peak %d)\n",
        phy_vars_ue->frame_rx,phy_vars_ue->rx_offset,max_pos,max_pos_fil,temp);
#endif //DEBUG_PHY


}


int lte_est_timing_advance(LTE_DL_FRAME_PARMS *frame_parms,
			   LTE_eNB_SRS *lte_eNb_srs,
			   unsigned int  *eNB_id,
			   unsigned char clear,
			   unsigned char number_of_cards,
			   short coef)

{

  static int max_pos_fil2 = 0;
  int temp, i, aa, max_pos = 0,ind;
  int max_val=0;
  short Re,Im,ncoef;
#ifdef USER_MODE
#ifdef DEBUG_PHY
  char fname[100],vname[100];
#endif
#endif

  ncoef = 32768 - coef;

  for (ind=0;ind<number_of_cards;ind++) {

    if (ind==0)
      max_val=0;


    for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
      // do ifft of channel estimate
      fft((short*) &lte_eNb_srs->srs_ch_estimates[ind][aa][0],
	  (short*) lte_eNb_srs->srs_ch_estimates_time[ind][aa],
	  frame_parms->twiddle_ifft,
	  frame_parms->rev,
	  frame_parms->log2_symbol_size,
	  frame_parms->log2_symbol_size/2,
	  0);

#ifdef USER_MODE
#ifdef DEBUG_PHY
      sprintf(fname,"srs_ch_estimates_time_%d%d.m",ind,aa);
      sprintf(vname,"srs_time_%d%d",ind,aa);
      write_output(fname,vname,lte_eNb_srs->srs_ch_estimates_time[ind][aa],frame_parms->ofdm_symbol_size*2,2,1);
#endif
#endif
    }

    // we only use channel estimates from tx antenna 0 here
    // remember we fixed the SRS to use only every second subcarriers
    for (i = 0; i < frame_parms->ofdm_symbol_size/2; i++) {
      temp = 0;
      for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
	Re = ((int16_t*)lte_eNb_srs->srs_ch_estimates_time[ind][aa])[(i<<2)];
	Im = ((int16_t*)lte_eNb_srs->srs_ch_estimates_time[ind][aa])[1+(i<<2)];
	temp += (Re*Re/2) + (Im*Im/2);
      }
      if (temp > max_val) {
	max_pos = i; 
	max_val = temp;
	*eNB_id = ind;
      }
    }
  }

  // filter position to reduce jitter
  if (clear == 1)
    max_pos_fil2 = max_pos;
  else
    max_pos_fil2 = ((max_pos_fil2 * coef) + (max_pos * ncoef)) >> 15;
  
#ifdef DEBUG_PHY
  //LOG_D(PHY,"frame %d: max_pos = %d, max_pos_fil = %d\n",mac_xface->frame,max_pos,max_pos_fil2);
#endif //DEBUG_PHY
  
  return(max_pos_fil2);
}


int lte_est_timing_advance_pusch(PHY_VARS_eNB* phy_vars_eNB,uint8_t UE_id,uint8_t sched_subframe)
{
  static int first_run=1;
  static int max_pos_fil2=0;
  int temp, i, aa, max_pos=0, max_val=0;
  short Re,Im,coef=24576;
  short ncoef = 32768 - coef;

  LTE_DL_FRAME_PARMS *frame_parms = &phy_vars_eNB->lte_frame_parms;
  LTE_eNB_PUSCH *eNB_pusch_vars = phy_vars_eNB->lte_eNB_pusch_vars[UE_id];
  int32_t **ul_ch_estimates_time=  eNB_pusch_vars->drs_ch_estimates_time[0];
  int subframe = phy_vars_eNB->proc[sched_subframe].subframe_rx;
  uint8_t harq_pid;
  uint8_t Ns = 1; //we take the estimate from the second slot
  uint8_t cyclic_shift = 0;//(frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.cyclicShift +
    //phy_vars_eNB->ulsch_eNB[UE_id]->harq_processes[harq_pid]->n_DMRS2 +
    //frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.nPRS[(subframe<<1)+Ns]) % 12;

  harq_pid = subframe2harq_pid(frame_parms,phy_vars_eNB->proc[sched_subframe].frame_rx,subframe);

  int sync_pos = (frame_parms->ofdm_symbol_size-cyclic_shift*frame_parms->ofdm_symbol_size/12)%(frame_parms->ofdm_symbol_size);


  for (i = 0; i < frame_parms->ofdm_symbol_size; i++) {
      temp = 0;
      for (aa=0;aa<frame_parms->nb_antennas_rx;aa++) {
	Re = ((int16_t*)ul_ch_estimates_time[aa])[(i<<2)];
	Im = ((int16_t*)ul_ch_estimates_time[aa])[1+(i<<2)];
	temp += (Re*Re/2) + (Im*Im/2);
      }
      if (temp > max_val) {
	max_pos = i; 
	max_val = temp;
      }
  }


  // filter position to reduce jitter
  if (first_run == 1){
    first_run=0;
    max_pos_fil2 = max_pos;
  }
  else
    max_pos_fil2 = ((max_pos_fil2 * coef) + (max_pos * ncoef)) >> 15;
  
#ifdef DEBUG_PHY
  LOG_I(PHY,"frame %d: max_pos = %d, max_pos_fil = %d, sync_pos=%d\n",phy_vars_eNB->proc[sched_subframe].frame_rx,max_pos,max_pos_fil2,sync_pos);
#endif //DEBUG_PHY

  return(max_pos_fil2-sync_pos);
}
