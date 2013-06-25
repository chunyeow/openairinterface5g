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

/*! \file PHY/LTE_TRANSPORT/pss.c
* \brief Top-level routines for generating primary synchronization signal (PSS) V8.6 2009-03
* \author F. Kaltenberger, O. Tonelli, R. Knopp
* \date 2011
* \version 0.1
* \company Eurecom
* \email: florian.kaltenberger@eurecom.fr, oscar.tonelli@yahoo.it,knopp@eurecom.fr
* \note
* \warning
*/
/* file: pss.c
   purpose: generate the primary synchronization signals of LTE
   author: florian.kaltenberger@eurecom.fr, oscar.tonelli@yahoo.it
   date: 21.10.2009 
*/

//#include "defs.h"
#include "PHY/defs.h"
#include "PHY/extern.h"

int generate_pss(mod_sym_t **txdataF,
		 short amp,
		 LTE_DL_FRAME_PARMS *frame_parms,
		 unsigned short symbol,
		 unsigned short slot_offset) {

  unsigned int Nsymb;
  unsigned short k,m,aa;
  u8 Nid2;
#ifdef IFFT_FPGA
  unsigned char *primary_sync_tab;
#else
  short *primary_sync;
#endif

  Nid2 = frame_parms->Nid_cell % 3;

  switch (Nid2) {
  case 0:
#ifdef IFFT_FPGA
    primary_sync_tab = primary_synch0_tab;
#else
    primary_sync = primary_synch0;
#endif
    break;
  case 1:
#ifdef IFFT_FPGA
    primary_sync_tab = primary_synch1_tab;
#else
    primary_sync = primary_synch1;
#endif
    break;
  case 2:
#ifdef IFFT_FPGA
    primary_sync_tab = primary_synch2_tab;
#else
    primary_sync = primary_synch2;
#endif
    break;
  default:
    msg("[PSS] eNb_id has to be 0,1,2\n");
    return(-1);
  }

  //a = (amp*ONE_OVER_SQRT2_Q15)>>15;
  //printf("[PSS] amp=%d, a=%d\n",amp,a);

  Nsymb = (frame_parms->Ncp==0)?14:12;

  //for (aa=0;aa<frame_parms->nb_antennas_tx;aa++) {
  aa = 0;

  // The PSS occupies the inner 6 RBs, which start at
#ifdef IFFT_FPGA
  k = (frame_parms->N_RB_DL-3)*12+5;
#else
  k = frame_parms->ofdm_symbol_size-3*12+5;
#endif
  //printf("[PSS] k = %d\n",k);
  for (m=5;m<67;m++) {
#ifdef IFFT_FPGA
    txdataF[aa][slot_offset*Nsymb/2*frame_parms->N_RB_DL*12 + symbol*frame_parms->N_RB_DL*12 + k] = primary_sync_tab[m];
#else
    ((short*)txdataF[aa])[2*(slot_offset*Nsymb/2*frame_parms->ofdm_symbol_size +
			    symbol*frame_parms->ofdm_symbol_size + k)] = 
      (amp * primary_sync[2*m]) >> 15; 
    ((short*)txdataF[aa])[2*(slot_offset*Nsymb/2*frame_parms->ofdm_symbol_size +
			    symbol*frame_parms->ofdm_symbol_size + k) + 1] = 
      (amp * primary_sync[2*m+1]) >> 15;
#endif
    k+=1;
#ifdef IFFT_FPGA
    if (k >= frame_parms->N_RB_DL*12) 
      k-=frame_parms->N_RB_DL*12;
#else
    if (k >= frame_parms->ofdm_symbol_size) {
      k++; //skip DC
      k-=frame_parms->ofdm_symbol_size;
    }
#endif
  }
  //}
  return(0);
}

int generate_pss_emul(PHY_VARS_eNB *phy_vars_eNb,u8 sect_id) {
  
  msg("[PHY] EMUL eNB generate_pss_emul eNB %d, sect_id %d\n",phy_vars_eNb->Mod_id,sect_id);
  eNB_transport_info[phy_vars_eNb->Mod_id].cntl.pss=sect_id;
  return(0);
}
