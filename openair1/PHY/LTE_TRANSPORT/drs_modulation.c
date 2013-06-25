/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file PHY/LTE_TRANSPORT/drs_modulation.c
* \brief Top-level routines for generating the Demodulation Reference Signals from 36-211, V8.6 2009-03
* \author R. Knopp, F. Kaltenberger, A. Bhamri
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr,florian.kaltenberger@eurecom.fr,ankit.bhamri@eurecom.fr
* \note
* \warning
*/
#include "PHY/defs.h"
#include "PHY/extern.h"
#include <emmintrin.h>
#include <xmmintrin.h>
//#define DEBUG_DRS

int generate_drs_pusch(PHY_VARS_UE *phy_vars_ue,
		       u8 eNB_id,
		       short amp,
		       unsigned int subframe,
		       unsigned int first_rb,
		       unsigned int nb_rb,
		       u8 ant) {

  u16 k,l,Msc_RS,Msc_RS_idx,rb,drs_offset;
  u16 * Msc_idx_ptr;
  int subframe_offset,re_offset,symbol_offset;

  //u32 phase_shift; // phase shift for cyclic delay in DM RS
  //u8 alpha_ind;

  s16 alpha_re[12] = {32767, 28377, 16383,     0,-16384,  -28378,-32768,-28378,-16384,    -1, 16383, 28377};
  s16 alpha_im[12] = {0,     16383, 28377, 32767, 28377,   16383,     0,-16384,-28378,-32768,-28378,-16384};

  u8 cyclic_shift,cyclic_shift0,cyclic_shift1; 
  LTE_DL_FRAME_PARMS *frame_parms = &phy_vars_ue->lte_frame_parms;
  mod_sym_t *txdataF = phy_vars_ue->lte_ue_common_vars.txdataF[ant];
  u32 u,v,alpha_ind;
  u32 u0=frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.grouphop[subframe<<1];
  u32 u1=frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.grouphop[1+(subframe<<1)];
  u32 v0=frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.seqhop[subframe<<1];
  u32 v1=frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.seqhop[1+(subframe<<1)];
  s32 ref_re,ref_im;
  u8 harq_pid = subframe2harq_pid(frame_parms,phy_vars_ue->frame,subframe);

  cyclic_shift0 = (frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.cyclicShift +
		   phy_vars_ue->ulsch_ue[eNB_id]->harq_processes[harq_pid]->n_DMRS2 +
		   phy_vars_ue->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.nPRS[subframe<<1]+
		   ((phy_vars_ue->ulsch_ue[0]->cooperation_flag==2)?10:0)+
		   ant*6) % 12;
  //  printf("PUSCH.cyclicShift %d, n_DMRS2 %d, nPRS %d\n",frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.cyclicShift,phy_vars_ue->ulsch_ue[eNB_id]->n_DMRS2,phy_vars_ue->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.nPRS[subframe<<1]);
  cyclic_shift1 = (frame_parms->pusch_config_common.ul_ReferenceSignalsPUSCH.cyclicShift +
		   phy_vars_ue->ulsch_ue[eNB_id]->harq_processes[harq_pid]->n_DMRS2 +
		   phy_vars_ue->lte_frame_parms.pusch_config_common.ul_ReferenceSignalsPUSCH.nPRS[(subframe<<1)+1]+
		   ((phy_vars_ue->ulsch_ue[0]->cooperation_flag==2)?10:0)+
		   ant*6) % 12;

  //       cyclic_shift0 = 0;
  //        cyclic_shift1 = 0;
  Msc_RS = 12*nb_rb;    

#ifdef USER_MODE
  Msc_idx_ptr = (u16*) bsearch(&Msc_RS, dftsizes, 33, sizeof(u16), compareints);
  if (Msc_idx_ptr)
    Msc_RS_idx = Msc_idx_ptr - dftsizes;
  else {
    msg("generate_drs_pusch: index for Msc_RS=%d not found\n",Msc_RS);
    return(-1);
  }
#else
  u8 b;
  for (b=0;b<33;b++) 
    if (Msc_RS==dftsizes[b])
      Msc_RS_idx = b;
#endif
#ifdef DEBUG_DRS
  msg("[PHY] drs_modulation: Msc_RS = %d, Msc_RS_idx = %d,cyclic_shift %d, u0 %d, v0 %d, u1 %d, v1 %d,cshift0 %d,cshift1 %d\n",Msc_RS, Msc_RS_idx,cyclic_shift,u0,v0,u1,v1,cyclic_shift0,cyclic_shift1);

#endif


  for (l = (3 - frame_parms->Ncp),u=u0,v=v0,cyclic_shift=cyclic_shift0; 
       l<frame_parms->symbols_per_tti; 
       l += (7 - frame_parms->Ncp),u=u1,v=v1,cyclic_shift=cyclic_shift1) {

    drs_offset = 0;  //  msg("drs_modulation: Msc_RS = %d, Msc_RS_idx = %d\n",Msc_RS, Msc_RS_idx);


#ifdef IFFT_FPGA_UE
    re_offset = frame_parms->N_RB_DL*12/2;
    subframe_offset = subframe*frame_parms->symbols_per_tti*frame_parms->N_RB_UL*12;
    symbol_offset = subframe_offset + frame_parms->N_RB_UL*12*l;
#else
    re_offset = frame_parms->first_carrier_offset;
    subframe_offset = subframe*frame_parms->symbols_per_tti*frame_parms->ofdm_symbol_size;
    symbol_offset = subframe_offset + frame_parms->ofdm_symbol_size*l;
#endif
    
#ifdef DEBUG_DRS
    msg("generate_drs_pusch: symbol_offset %d, subframe offset %d, cyclic shift %d\n",symbol_offset,subframe_offset,cyclic_shift);
#endif
    alpha_ind = 0;
    for (rb=0;rb<frame_parms->N_RB_UL;rb++) {

      if ((rb >= first_rb) && (rb<(first_rb+nb_rb))) {

#ifdef DEBUG_DRS	
	msg("generate_drs_pusch: doing RB %d, re_offset=%d, drs_offset=%d,cyclic shift %d\n",rb,re_offset,drs_offset,cyclic_shift);
#endif

#ifdef IFFT_FPGA_UE
	if (cyclic_shift == 0) {
	  for (k=0;k<12;k++) {
	    if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] >= 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] >= 0)) 
	      txdataF[symbol_offset+re_offset] = (mod_sym_t) 1;
	    else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] >= 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] < 0)) 
	      txdataF[symbol_offset+re_offset] = (mod_sym_t) 2;
	    else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] < 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] >= 0)) 
	      txdataF[symbol_offset+re_offset] = (mod_sym_t) 3;
	    else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] < 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] < 0)) 
	      txdataF[symbol_offset+re_offset] = (mod_sym_t) 4;
	    re_offset++;
	    drs_offset++;
	    if (re_offset >= frame_parms->N_RB_UL*12)
	      re_offset=0;
	  }
	}
	else if(cyclic_shift == 6 ) {
	  for (k=0;k<12;k++) {
	    if(k%2 == 0) {
	      if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] >= 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] >= 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 4;
	      else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] >= 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] < 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 3;
	      else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] < 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] >= 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 2;
	      else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] < 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] < 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 1;
	    }
	    else {
	      if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] >= 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] >= 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 1;
	      else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] >= 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] < 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 2;
	      else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] < 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] >= 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 3;
	      else if ((ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1] < 0) && (ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1] < 0)) 
		txdataF[symbol_offset+re_offset] = (mod_sym_t) 4;
	    }
	    
	    re_offset++;
	    drs_offset++;
	    if (re_offset >= frame_parms->N_RB_UL*12)
	      re_offset=0;
	  }
	}
#else  //IFFT_FPGA_UE

	for (k=0;k<12;k++) {
	  ref_re = (s32) ul_ref_sigs[u][v][Msc_RS_idx][drs_offset<<1];
	  ref_im = (s32) ul_ref_sigs[u][v][Msc_RS_idx][(drs_offset<<1)+1];

	  ((s16*) txdataF)[2*(symbol_offset + re_offset)]   = (s16) (((ref_re*alpha_re[alpha_ind]) - 
								      (ref_im*alpha_im[alpha_ind]))>>15);
	  ((s16*) txdataF)[2*(symbol_offset + re_offset)+1] = (s16) (((ref_re*alpha_im[alpha_ind]) + 
								      (ref_im*alpha_re[alpha_ind]))>>15);
	  ((short*) txdataF)[2*(symbol_offset + re_offset)]   = (short) ((((short*) txdataF)[2*(symbol_offset + re_offset)]*(s32)amp)>>15);
	  ((short*) txdataF)[2*(symbol_offset + re_offset)+1] = (short) ((((short*) txdataF)[2*(symbol_offset + re_offset)+1]*(s32)amp)>>15);

	  
	  alpha_ind = (alpha_ind + cyclic_shift);
	  if (alpha_ind > 11)
	    alpha_ind-=12;
      
#ifdef DEBUG_DRS
	  msg("symbol_offset %d, alpha_ind %d , re_offset %d : (%d,%d)\n",
	      symbol_offset,
	      alpha_ind,
	      re_offset,
	      ((short*) txdataF)[2*(symbol_offset + re_offset)],
	      ((short*) txdataF)[2*(symbol_offset + re_offset)+1]);
	  
#endif  // DEBUG_DRS
	  re_offset++;
	  drs_offset++;
	  if (re_offset >= frame_parms->ofdm_symbol_size)
	    re_offset = 0;
	}
    
#endif // IFFT_FPGA_UE
      }
      else {
	re_offset+=12; // go to next RB
	
	// check if we crossed the symbol boundary and skip DC
#ifdef IFFT_FPGA_UE
	if (re_offset >= frame_parms->N_RB_DL*12) {
	  if (frame_parms->N_RB_DL&1)  // odd number of RBs 
	    re_offset=6;
	  else                         // even number of RBs (doesn't straddle DC)
	    re_offset=0;  
	}
#else
	if (re_offset >= frame_parms->ofdm_symbol_size) {
	  if (frame_parms->N_RB_DL&1)  // odd number of RBs 
	    re_offset=6;
	  else                         // even number of RBs (doesn't straddle DC)
	    re_offset=0;  
	}
#endif
      }
    }
  }
  return(0);
}

