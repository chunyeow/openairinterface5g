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

/*! \file PHY/LTE_TRANSPORT/pcfich.c
* \brief Top-level routines for generating and decoding  the PCFICH/CFI physical/transport channel V8.6 2009-03
* \author R. Knopp
* \date 2011
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr
* \note
* \warning
*/
#include "PHY/defs.h"
#include "MAC_INTERFACE/extern.h"

//u16 pcfich_reg[4];
//u8 pcfich_first_reg_idx = 0;

//#define DEBUG_PCFICH

void generate_pcfich_reg_mapping(LTE_DL_FRAME_PARMS *frame_parms) {

  u16 kbar = 6 * (frame_parms->Nid_cell %(2*frame_parms->N_RB_DL));
  u16 first_reg;
  u16 *pcfich_reg = frame_parms->pcfich_reg;
  
  pcfich_reg[0] = kbar/6;
  first_reg = pcfich_reg[0];

  frame_parms->pcfich_first_reg_idx=0;

  pcfich_reg[1] = ((kbar + (frame_parms->N_RB_DL>>1)*6)%(frame_parms->N_RB_DL*12))/6;
  if (pcfich_reg[1] < pcfich_reg[0]) {
    frame_parms->pcfich_first_reg_idx = 1;
    first_reg = pcfich_reg[1];
  }  
  pcfich_reg[2] = ((kbar + (frame_parms->N_RB_DL)*6)%(frame_parms->N_RB_DL*12))/6;
  if (pcfich_reg[2] < first_reg) {
    frame_parms->pcfich_first_reg_idx = 2;
    first_reg = pcfich_reg[2];
  }
  pcfich_reg[3] = ((kbar + ((3*frame_parms->N_RB_DL)>>1)*6)%(frame_parms->N_RB_DL*12))/6;
  if (pcfich_reg[3] < first_reg) {
    frame_parms->pcfich_first_reg_idx = 3;
    first_reg = pcfich_reg[3];
  }
  
#ifdef DEBUG_PCFICH
  msg("[PHY] pcfich_reg : %d,%d,%d,%d\n",pcfich_reg[0],pcfich_reg[1],pcfich_reg[2],pcfich_reg[3]);
#endif
}

void pcfich_scrambling(LTE_DL_FRAME_PARMS *frame_parms,
		       u8 subframe,
		       u8 *b,
		       u8 *bt) {
  u32 i;
  u8 reset;
  u32 x1, x2, s=0;

  reset = 1;
  // x1 is set in lte_gold_generic
  x2 = ((((2*frame_parms->Nid_cell)+1)*(1+subframe))<<9) + frame_parms->Nid_cell; //this is c_init in 36.211 Sec 6.7.1
  for (i=0; i<32; i++) {
    if ((i&0x1f)==0) {
      s = lte_gold_generic(&x1, &x2, reset);
      //printf("lte_gold[%d]=%x\n",i,s);
      reset = 0;
    }

    bt[i] = (b[i]&1) ^ ((s>>(i&0x1f))&1);
    //    printf("scrambling %d : b %d => bt %d, c %d\n",i,b[i],bt[i],((s>>(i&0x1f))&1));
  }
}

void pcfich_unscrambling(LTE_DL_FRAME_PARMS *frame_parms,
			 u8 subframe,
			 s16 *d) {

  u32 i;
  u8 reset;
  u32 x1, x2, s=0;

  reset = 1;
  // x1 is set in lte_gold_generic
  x2 = ((((2*frame_parms->Nid_cell)+1)*(1+subframe))<<9) + frame_parms->Nid_cell; //this is c_init in 36.211 Sec 6.7.1

  for (i=0; i<32; i++) {
    if ((i&0x1f)==0) {
      s = lte_gold_generic(&x1, &x2, reset);
      //printf("lte_gold[%d]=%x\n",i,s);
      reset = 0;
    }

    if (((s>>(i&0x1f))&1) == 1) 
      d[i]=-d[i];

    //    printf("scrambling %d : b %d => bt %d, c %d\n",i,b[i],bt[i],((s>>(i&0x1f))&1));
  }
}

u8 pcfich_b[4][32]={{0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1},
		    {1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0},
		    {1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1,0,1,1},
		    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}};

void generate_pcfich(u8 num_pdcch_symbols,
		     s16 amp,
		     LTE_DL_FRAME_PARMS *frame_parms,
		     mod_sym_t **txdataF,
		     u8 subframe) {

  u8 pcfich_bt[32],nsymb,pcfich_quad;
  mod_sym_t pcfich_d[2][16];
  u8 i;
  u32 symbol_offset,m,re_offset,reg_offset;
  s16 gain_lin_QPSK;
#ifdef IFFT_FPGA
  u8 qpsk_table_offset = 0; 
  u8 qpsk_table_offset2 = 0;
#endif
  u16 *pcfich_reg = frame_parms->pcfich_reg;

  int nushiftmod3 = frame_parms->nushift%3;
#ifdef DEBUG_PCFICH
  msg("[PHY] Generating PCFICH for %d PDCCH symbols, AMP %d\n",num_pdcch_symbols,amp);
#endif

  // scrambling
  if ((num_pdcch_symbols>0) && (num_pdcch_symbols<4)) 
    pcfich_scrambling(frame_parms,subframe,pcfich_b[num_pdcch_symbols-1],pcfich_bt);

  // modulation
  if (frame_parms->mode1_flag==1) 
    gain_lin_QPSK = (s16)((amp*ONE_OVER_SQRT2_Q15)>>15);  
  else
    gain_lin_QPSK = amp/2;  

  if (frame_parms->mode1_flag) { // SISO
#ifndef IFFT_FPGA
    for (i=0;i<16;i++) {
      ((s16*)(&(pcfich_d[0][i])))[0]   = ((pcfich_bt[2*i] == 1) ? -gain_lin_QPSK : gain_lin_QPSK);
      ((s16*)(&(pcfich_d[1][i])))[0]   = ((pcfich_bt[2*i] == 1) ? -gain_lin_QPSK : gain_lin_QPSK);
      ((s16*)(&(pcfich_d[0][i])))[1]   = ((pcfich_bt[2*i+1] == 1) ? -gain_lin_QPSK : gain_lin_QPSK);
      ((s16*)(&(pcfich_d[1][i])))[1]   = ((pcfich_bt[2*i+1] == 1) ? -gain_lin_QPSK : gain_lin_QPSK);
    }
#else
    for (i=0;i<16;i++) {
      qpsk_table_offset = MOD_TABLE_QPSK_OFFSET;
      if (pcfich_bt[2*i] == 1)
	qpsk_table_offset+=2;
      if (pcfich_bt[2*i+1] == 1) 
	qpsk_table_offset+=1;
      
      pcfich_d[0][i] = (mod_sym_t) qpsk_table_offset;
      pcfich_d[1][i] = (mod_sym_t) qpsk_table_offset;
    }
#endif
  }
  else { // ALAMOUTI
#ifndef IFFT_FPGA
    for (i=0;i<16;i+=2) {
      // first antenna position n -> x0
      ((s16*)(&(pcfich_d[0][i])))[0]   = ((pcfich_bt[2*i] == 1) ? -gain_lin_QPSK : gain_lin_QPSK);
      ((s16*)(&(pcfich_d[0][i])))[1]   = ((pcfich_bt[2*i+1] == 1) ? -gain_lin_QPSK : gain_lin_QPSK);
      // second antenna position n -> -x1*
      ((s16*)(&(pcfich_d[1][i])))[0]   = ((pcfich_bt[2*i+2] == 1) ? gain_lin_QPSK : -gain_lin_QPSK);
      ((s16*)(&(pcfich_d[1][i])))[1]   = ((pcfich_bt[2*i+3] == 1) ? -gain_lin_QPSK : gain_lin_QPSK);
      // fill in the rest of the ALAMOUTI precoding
      ((s16*)&pcfich_d[0][i+1])[0] = -((s16*)&pcfich_d[1][i])[0];
      ((s16*)&pcfich_d[0][i+1])[1] =  ((s16*)&pcfich_d[1][i])[1];
      ((s16*)&pcfich_d[1][i+1])[0] =  ((s16*)&pcfich_d[0][i])[0];
      ((s16*)&pcfich_d[1][i+1])[1] = -((s16*)&pcfich_d[0][i])[1];
    
      
    }  
#else
    for (i=0;i<16;i+=2) {
      qpsk_table_offset =  MOD_TABLE_QPSK_OFFSET;  //x0
      qpsk_table_offset2 =  MOD_TABLE_QPSK_OFFSET; //x0*
      
      // flipping bit for imag part of symbol means taking x0*
      if (pcfich_bt[2*i] == 1) { //real
	qpsk_table_offset+=2;
	qpsk_table_offset2+=2;
      }
      if (pcfich_bt[2*i+1] == 1) //imag
	qpsk_table_offset+=1;
      else
	qpsk_table_offset2+=1;
	
      pcfich_d[0][i]   = (mod_sym_t) qpsk_table_offset;      // x0
      pcfich_d[1][i+1] = (mod_sym_t) qpsk_table_offset2;   // x0*
	
	
      qpsk_table_offset = MOD_TABLE_QPSK_OFFSET; //-x1*
      qpsk_table_offset2 = MOD_TABLE_QPSK_OFFSET;//x1
	
      // flipping bit for real part of symbol means taking -x1*
      if (pcfich_bt[2*i+2] == 1) //real   
	qpsk_table_offset2+=2;
      else
	qpsk_table_offset+=2;
      if (pcfich_bt[2*i+3] == 1) { //imag
	qpsk_table_offset+=1;
	qpsk_table_offset2+=1;
      }
	
      pcfich_d[1][i] = (mod_sym_t) qpsk_table_offset;     // -x1*
      pcfich_d[0][i+1] = (mod_sym_t) qpsk_table_offset2;  // x1
    }
#endif
  }


  // mapping
  nsymb = (frame_parms->Ncp==0) ? 14:12;
  
#ifdef IFFT_FPGA      
  symbol_offset = (u32)frame_parms->N_RB_DL*12*((subframe*nsymb));
  re_offset = frame_parms->N_RB_DL*12/2;
  
#else
  symbol_offset = (u32)frame_parms->ofdm_symbol_size*((subframe*nsymb));
  re_offset = frame_parms->first_carrier_offset;

#endif

  // loop over 4 quadruplets and lookup REGs
  m=0;
  for (pcfich_quad=0;pcfich_quad<4;pcfich_quad++) {
    reg_offset = re_offset+((u16)pcfich_reg[pcfich_quad]*6);
#ifdef IFFT_FPGA
    if (reg_offset>=(frame_parms->N_RB_DL*12))
      reg_offset-=(frame_parms->N_RB_DL*12);
#else
    if (reg_offset>=frame_parms->ofdm_symbol_size)
      reg_offset=1 + reg_offset-frame_parms->ofdm_symbol_size;
#endif
    //    printf("mapping pcfich reg_offset %d\n",reg_offset);
    for (i=0;i<6;i++) {
      if ((i!=nushiftmod3)&&(i!=(nushiftmod3+3))) {
	txdataF[0][symbol_offset+reg_offset+i] = pcfich_d[0][m];
	/*
#ifndef IFFT_FPGA
	printf("pcfich: quad %d, i %d, offset %d => m%d (%d,%d)\n",pcfich_quad,i,reg_offset+i,m,
	       ((s16*)&txdataF[0][symbol_offset+reg_offset+i])[0],
	       ((s16*)&txdataF[0][symbol_offset+reg_offset+i])[1]);
#else
	printf("pcfich: quad %d, i %d, offset %d => m%d (%d)\n",pcfich_quad,i,reg_offset+i,m,
	       txdataF[0][symbol_offset+reg_offset+i]);
#endif
	*/
	if (frame_parms->nb_antennas_tx_eNB>1)  
	  txdataF[1][symbol_offset+reg_offset+i] = pcfich_d[1][m];
	m++;
      }
    }
  }

}


u8 rx_pcfich(LTE_DL_FRAME_PARMS *frame_parms,
	     u8 subframe,
	     LTE_UE_PDCCH *lte_ue_pdcch_vars,
	     MIMO_mode_t mimo_mode) {

  u8 pcfich_quad;
  u8 i,j;
  u16 reg_offset;

  s32 **rxdataF_comp = lte_ue_pdcch_vars->rxdataF_comp;
  s16 pcfich_d[32],*pcfich_d_ptr;
  s32 metric,old_metric=-16384;
  u8 num_pdcch_symbols=3;
  u16 *pcfich_reg = frame_parms->pcfich_reg;

  // demapping
  // loop over 4 quadruplets and lookup REGs
  //  m=0;
  pcfich_d_ptr = pcfich_d;

  for (pcfich_quad=0;pcfich_quad<4;pcfich_quad++) {
    reg_offset = (pcfich_reg[pcfich_quad]*4);

    //    if (frame_parms->mode1_flag==1) {  // SISO
      for (i=0;i<4;i++) {

	  pcfich_d_ptr[0] = ((s16*)&rxdataF_comp[0][reg_offset+i])[0]; // RE component
	  pcfich_d_ptr[1] = ((s16*)&rxdataF_comp[0][reg_offset+i])[1]; // IM component
	/*
			printf("rx_pcfich: quad %d, i %d, offset %d => m%d (%d,%d) => pcfich_d_ptr[0] %d \n",pcfich_quad,i,reg_offset+i,m,
	       ((s16*)&rxdataF_comp[0][reg_offset+i])[0],
	       ((s16*)&rxdataF_comp[0][reg_offset+i])[1],
	       pcfich_d_ptr[0]);
	*/
	pcfich_d_ptr+=2;
      }
      /*
    }
    else { // ALAMOUTI
      for (i=0;i<4;i+=2) {
	pcfich_d_ptr[0] = 0;
	pcfich_d_ptr[1] = 0;
	pcfich_d_ptr[2] = 0;
	pcfich_d_ptr[3] = 0;
	for (j=0;j<frame_parms->nb_antennas_rx;j++) {

	  pcfich_d_ptr[0] += (((s16*)&rxdataF_comp[j][reg_offset+i])[0]+
			     ((s16*)&rxdataF_comp[j+2][reg_offset+i+1])[0]); // RE component
	  pcfich_d_ptr[1] += (((s16*)&rxdataF_comp[j][reg_offset+i])[1] -
			     ((s16*)&rxdataF_comp[j+2][reg_offset+i+1])[1]);// IM component
	  
	  pcfich_d_ptr[2] += (((s16*)&rxdataF_comp[j][reg_offset+i+1])[0]-
			     ((s16*)&rxdataF_comp[j+2][reg_offset+i])[0]); // RE component
	  pcfich_d_ptr[3] += (((s16*)&rxdataF_comp[j][reg_offset+i+1])[1] +
			     ((s16*)&rxdataF_comp[j+2][reg_offset+i])[1]);// IM component


	}

	pcfich_d_ptr+=4;

      }
*/
  }

  // pcfhich unscrambling

  pcfich_unscrambling(frame_parms,subframe,pcfich_d);

  // pcfich detection

  for (i=0;i<3;i++) {
    metric = 0;
    for (j=0;j<32;j++) {
      //printf("pcfich_b[%d][%d] %d => pcfich_d[%d] %d\n",i,j,pcfich_b[i][j],j,pcfich_d[j]);
      metric += (s32)(((pcfich_b[i][j]==0) ? (pcfich_d[j]) : (-pcfich_d[j])));
    }
#ifdef DEBUG_PCFICH
    msg("metric %d : %d\n",i,metric);
#endif
    if (metric > old_metric) {
      num_pdcch_symbols = 1+i;
      old_metric = metric;
    }
  }

#ifdef DEBUG_PCFICH
  msg("[PHY] PCFICH detected for %d PDCCH symbols\n",num_pdcch_symbols);
#endif
  return(num_pdcch_symbols);
}
