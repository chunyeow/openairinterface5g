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

******************************************n*************************************/

/*! \file PHY/LTE_TRANSPORT/srs_modulation.c
* \brief Top-level routines for generating sounding reference signal (SRS) V8.6 2009-03
* \author R. Knopp, F. Kaltenberger
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr, florian.kaltenberger@eurecom.fr
* \note
* \warning
*/
#include "PHY/defs.h"
#include "extern.h"
#include "UTIL/LOG/log.h"

unsigned short msrsb_6_40[8][4] = {{36,12,4,4},
				   {32,16,8,4},
				   {24,4,4,4},
				   {20,4,4,4},
				   {16,4,4,4},
				   {12,4,4,4},
				   {8,4,4,4},
				   {4,4,4,4}};

unsigned short msrsb_41_60[8][4] = {{48,24,12,4},
				    {48,16,8,4},
				    {40,20,4,4},
				    {36,12,4,4},
				    {32,16,8,4},
				    {24,4,4,4},
				    {20,4,4,4},
				    {16,4,4,4}};

unsigned short msrsb_61_80[8][4] = {{72,24,12,4},
				    {64,32,16,4},
				    {60,20,4,4},
				    {48,24,12,4},
				    {48,16,8,4},
				    {40,20,4,4},
				    {36,12,4,4},
				    {32,16,8,4}};

unsigned short msrsb_81_110[8][4] = {{96,48,24,4},
				     {96,32,16,4},
				     {80,40,20,4},
				     {72,24,12,4},
				     {64,32,16,4},
				     {60,20,4,4},
				     {48,24,12,4},
				     {48,16,8,4}};

unsigned short Nb_6_40[8][4] = {{36,12,4,4},
				{32,16,8,4},
				{24,4,4,4},
				{20,4,4,4},
				{16,4,4,4},
				{12,4,4,4},
				{8,4,4,4},
				{4,4,4,4}};

unsigned short Nb_41_60[8][4] = {{48,24,12,4},
				 {48,16,8,4},
				 {40,20,4,4},
				 {36,12,4,4},
				 {32,16,8,4},
				 {24,4,4,4},
				 {20,4,4,4},
				 {16,4,4,4}};

unsigned short Nb_61_80[8][4] = {{72,24,12,4},
				 {64,32,16,4},
				 {60,20,4,4},
				 {48,24,12,4},
				 {48,16,8,4},
				 {40,20,4,4},
				 {36,12,4,4},
				 {32,16,8,4}};

unsigned short Nb_81_110[8][4] = {{96,48,24,4},
				  {96,32,16,4},
				  {80,40,20,4},
				  {72,24,12,4},
				  {64,32,16,4},
				  {60,20,4,4},
				  {48,24,12,4},
				  {48,16,8,4}};

unsigned short transmission_offset_tdd[16] = {2,6,10,18,14,22,26,30,70,74,194,326,586,210};

int compareints (const void * a, const void * b)
{
  return ( *(unsigned short*)a - *(unsigned short*)b );
}


s32 generate_srs_tx(PHY_VARS_UE *phy_vars_ue,
		    u8 eNB_id,
		    s16 amp,
		    u32 subframe) {

  LTE_DL_FRAME_PARMS *frame_parms=&phy_vars_ue->lte_frame_parms;
  SOUNDINGRS_UL_CONFIG_DEDICATED *soundingrs_ul_config_dedicated=&phy_vars_ue->soundingrs_ul_config_dedicated[eNB_id];
  mod_sym_t *txdataF = phy_vars_ue->lte_ue_common_vars.txdataF[0];
  u16 msrsb=0,Nb=0,nb,b,msrs0=0,k,Msc_RS,Msc_RS_idx,carrier_pos,symbol_offset;
  u16 *Msc_idx_ptr;
  u32 k0,T_SFC;
  u32 subframe_offset;
  u8 Bsrs  = soundingrs_ul_config_dedicated->srs_Bandwidth;
  u8 Csrs  = frame_parms->soundingrs_ul_config_common.srs_BandwidthConfig;
  u8 Ssrs  = frame_parms->soundingrs_ul_config_common.srs_SubframeConfig;
  u8 n_RRC = soundingrs_ul_config_dedicated->freqDomainPosition;
  u8 kTC   = soundingrs_ul_config_dedicated->transmissionComb;


  if (frame_parms->N_RB_UL < 41) {
    msrs0 = msrsb_6_40[Csrs][0];
    msrsb = msrsb_6_40[Csrs][Bsrs];
    Nb    = Nb_6_40[Csrs][Bsrs];
  }
  else if (frame_parms->N_RB_UL < 61) {
    msrs0 = msrsb_41_60[Csrs][0];
    msrsb = msrsb_41_60[Csrs][Bsrs];
    Nb    = Nb_41_60[Csrs][Bsrs];
  }
  else if (frame_parms->N_RB_UL < 81) {
    msrs0 = msrsb_61_80[Csrs][0];
    msrsb = msrsb_61_80[Csrs][Bsrs];
    Nb    = Nb_61_80[Csrs][Bsrs];
  }
  else if (frame_parms->N_RB_UL <111) {
    msrs0 = msrsb_81_110[Csrs][0];
    msrsb = msrsb_81_110[Csrs][Bsrs];
    Nb    = Nb_81_110[Csrs][Bsrs];
  }

  Msc_RS = msrsb * 6;
  k0 = (((frame_parms->N_RB_UL>>1)-(msrs0>>1))*12) + kTC;
  nb  = (4*n_RRC/msrsb)%Nb;

  for (b=0;b<=Bsrs;b++) {
    k0 += 2*nb*Msc_RS;
  }

  if (k0<0) {
    msg("generate_srs: invalid parameter set msrs0=%d, msrsb=%d, Nb=%d => nb=%d, k0=%d\n",msrs0,msrsb,Nb,nb,k0);
    return(-1);
  }

#ifdef USER_MODE
  Msc_idx_ptr = (u16*) bsearch((u16*) &Msc_RS, (u16*) dftsizes, 33, sizeof(u16), compareints);
  if (Msc_idx_ptr)
    Msc_RS_idx = Msc_idx_ptr - dftsizes;
  else {
    msg("generate_srs: index for Msc_RS=%d not found\n",Msc_RS);
    return(-1);
  }
#else //stdlib not availiable in RTAI
  if (Msc_RS==216)
    Msc_RS_idx = 12;
  else if (Msc_RS==144)
    Msc_RS_idx = 9;
  else {
    msg("generate_srs: index for Msc_RS=%d not implemented\n",Msc_RS);
    return(-1);
  }
#endif

#ifdef DEBUG_SRS
  msg("generate_srs_tx: Msc_RS = %d, Msc_RS_idx = %d\n",Msc_RS, Msc_RS_idx);
#endif

  T_SFC = (Ssrs<=7 ? 5 : 10); 
  if ((1<<(subframe%T_SFC))&transmission_offset_tdd[Ssrs]) {

#ifndef IFFT_FPGA_UE
  carrier_pos = (frame_parms->first_carrier_offset + k0) % frame_parms->ofdm_symbol_size;
  //msg("carrier_pos = %d\n",carrier_pos);
  
  subframe_offset = subframe*frame_parms->symbols_per_tti*frame_parms->ofdm_symbol_size;
  symbol_offset = subframe_offset+(frame_parms->symbols_per_tti-1)*frame_parms->ofdm_symbol_size;

  for (k=0;k<Msc_RS;k++) {
    ((short*) txdataF)[2*(symbol_offset + carrier_pos)]   = (short) (((s32) amp * (s32) ul_ref_sigs[0][0][Msc_RS_idx][k<<1])>>15);
    ((short*) txdataF)[2*(symbol_offset + carrier_pos)+1] = (short) (((s32) amp * (s32) ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1])>>15);
    carrier_pos+=2;
    if (carrier_pos >= frame_parms->ofdm_symbol_size)
      carrier_pos=1;
  }
#else
  carrier_pos = (frame_parms->N_RB_UL*12/2 + k0) % (frame_parms->N_RB_UL*12);
  //msg("carrier_pos = %d\n",carrier_pos);

  subframe_offset = subframe*frame_parms->symbols_per_tti*frame_parms->N_RB_UL*12;
  symbol_offset = subframe_offset+(frame_parms->symbols_per_tti-1)*frame_parms->N_RB_UL*12;

  for (k=0;k<Msc_RS;k++) {
    if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] >= 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] >= 0)) 
      txdataF[symbol_offset+carrier_pos] = (mod_sym_t) 4;
    else if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] >= 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] < 0)) 
      txdataF[symbol_offset+carrier_pos] = (mod_sym_t) 2;
    else if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] < 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] >= 0)) 
      txdataF[symbol_offset+carrier_pos] = (mod_sym_t) 3;
    else if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] < 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] < 0)) 
      txdataF[symbol_offset+carrier_pos] = (mod_sym_t) 1;

    carrier_pos+=2;
    if (carrier_pos >= frame_parms->N_RB_UL*12)
      carrier_pos=0;
  }
#endif
  //  write_output("srs_txF.m","srstxF",&txdataF[symbol_offset],512,1,1);
  }
  return(0);
}

int generate_srs_tx_emul(PHY_VARS_UE *phy_vars_ue,u8 subframe) {

  LOG_D(PHY,"[UE] generate_srs_tx_emul for subframe %d\n",subframe);
  return(0); 
}

int generate_srs_rx(LTE_DL_FRAME_PARMS *frame_parms,
		    SOUNDINGRS_UL_CONFIG_DEDICATED *soundingrs_ul_config_dedicated,		    
		    int *txdataF) {

  u16 msrsb=0,Nb=0,nb,b,msrs0=0,k,Msc_RS,Msc_RS_idx,carrier_pos;
  u16 *Msc_idx_ptr;
  int k0;
  u8 Bsrs  = soundingrs_ul_config_dedicated->srs_Bandwidth;
  u8 Csrs  = frame_parms->soundingrs_ul_config_common.srs_BandwidthConfig;
  u8 n_RRC = soundingrs_ul_config_dedicated->freqDomainPosition;
  u8 kTC   = soundingrs_ul_config_dedicated->transmissionComb;

  if (frame_parms->N_RB_UL < 41) {
    msrs0 = msrsb_6_40[Csrs][0];
    msrsb = msrsb_6_40[Csrs][Bsrs];
    Nb    = Nb_6_40[Csrs][Bsrs];
  }
  else if (frame_parms->N_RB_UL < 61) {
    msrs0 = msrsb_41_60[Csrs][0];
    msrsb = msrsb_41_60[Csrs][Bsrs];
    Nb    = Nb_41_60[Csrs][Bsrs];
  }
  else if (frame_parms->N_RB_UL < 81) {
    msrs0 = msrsb_61_80[Csrs][0];
    msrsb = msrsb_61_80[Csrs][Bsrs];
    Nb    = Nb_61_80[Csrs][Bsrs];
  }
  else if (frame_parms->N_RB_UL <111) {
    msrs0 = msrsb_81_110[Csrs][0];
    msrsb = msrsb_81_110[Csrs][Bsrs];
    Nb    = Nb_81_110[Csrs][Bsrs];
  }

  Msc_RS = msrsb * 6;
  k0 = (((frame_parms->N_RB_UL>>1)-(msrs0>>1))*12) + kTC;
  nb  = (4*n_RRC/msrsb)%Nb;

  for (b=0;b<=Bsrs;b++) {
    k0 += 2*nb*Msc_RS;
  }

  if (k0<0) {
    msg("Invalid parameter set msrs0=%d, msrsb=%d, Nb=%d => nb=%d, k0=%d\n",msrs0,msrsb,Nb,nb,k0);
    return(-1);
  }

#ifdef USER_MODE
  Msc_idx_ptr = (u16*) bsearch((u16*) &Msc_RS, (u16*) dftsizes, 33, sizeof(u16), compareints);
  if (Msc_idx_ptr)
    Msc_RS_idx = Msc_idx_ptr - dftsizes;
  else {
    msg("generate_srs: index for Msc_RS=%d not found\n",Msc_RS);
    return(-1);
  }
#else //stdlib not availiable in RTAI
  if (Msc_RS==216)
    Msc_RS_idx = 12;
  else if (Msc_RS==144)
    Msc_RS_idx = 9;
  else {
    msg("generate_srs: index for Msc_RS=%d not implemented\n",Msc_RS);
    return(-1);
  }
#endif

#ifdef DEBUG_SRS
  msg("generate_srs_rx: Msc_RS = %d, Msc_RS_idx = %d, k0=%d\n",Msc_RS, Msc_RS_idx,k0);
#endif

  carrier_pos = (frame_parms->first_carrier_offset + k0) % frame_parms->ofdm_symbol_size;

  for (k=0;k<Msc_RS;k++) {
    ((short*) txdataF)[carrier_pos<<2]   = ul_ref_sigs_rx[0][0][Msc_RS_idx][k<<2];
    ((short*) txdataF)[(carrier_pos<<2)+1] = ul_ref_sigs_rx[0][0][Msc_RS_idx][(k<<2)+1];
    ((short*) txdataF)[(carrier_pos<<2)+2] = ul_ref_sigs_rx[0][0][Msc_RS_idx][(k<<2)+2];
    ((short*) txdataF)[(carrier_pos<<2)+3] = ul_ref_sigs_rx[0][0][Msc_RS_idx][(k<<2)+3];
    carrier_pos+=2;
    if (carrier_pos >= frame_parms->ofdm_symbol_size) 
      carrier_pos=1;
  }
  /*
  for (k=0;k<Msc_RS;k++) {
    if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] >= 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] >= 0)) {
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)] = ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+1] = ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+2] = -ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+3] = ONE_OVER_SQRT2_Q15;
    }
    else if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] >= 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] < 0)) {
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)] = ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+1] = -ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+2] = ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+3] = ONE_OVER_SQRT2_Q15;
    }
    else if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] < 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] >= 0)) {
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)] = -ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+1] = ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+2] = -ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+3] = -ONE_OVER_SQRT2_Q15;
    }
    else if ((ul_ref_sigs[0][0][Msc_RS_idx][k<<1] < 0) && (ul_ref_sigs[0][0][Msc_RS_idx][(k<<1)+1] < 0)) {
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)] = -ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+1] = -ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+2] = ONE_OVER_SQRT2_Q15;
      ((short*) txdataF)[4*(symbol_offset + carrier_pos)+3] = -ONE_OVER_SQRT2_Q15;
    }

    carrier_pos+=2;
    if (carrier_pos >= frame_parms->ofdm_symbol_size)
      carrier_pos=0;
  }
  */

  //  write_output("srs_rx.m","srsrx",txdataF,1024,2,1);
  return(0);
}


#ifdef MAIN
main() {


}
#endif
