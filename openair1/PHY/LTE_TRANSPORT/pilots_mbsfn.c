/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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

/*! \file PHY/LTE_TRANSPORT/pilots_mbsfn.c
* \brief Top-level routines for generating DL mbsfn reference signals 
* \authors S. Paranche, R. Knopp
* \date 2012
* \version 0.1
* \company Eurecom
* \email: knopp@eurecom.fr
* \note
* \warning
*/
//#include "defs.h"
#include "PHY/defs.h"

    
int generate_mbsfn_pilot(PHY_VARS_eNB *phy_vars_eNB,
			 mod_sym_t **txdataF,
			 s16 amp,
			 u16 subframe) {

  LTE_DL_FRAME_PARMS *frame_parms = &phy_vars_eNB->lte_frame_parms;  
  u32 subframe_offset,Nsymb,samples_per_symbol;

  

  if (subframe<0 || subframe>= 10) {
    msg("generate_mbsfn_pilots_subframe: subframe not in range (%d)\n",subframe);
    return(-1);
  }

  Nsymb = (frame_parms->Ncp==NORMAL) ? 7 : 6;
 
  subframe_offset = subframe*frame_parms->ofdm_symbol_size*Nsymb<<1;
  samples_per_symbol = frame_parms->ofdm_symbol_size;
    
    //    printf("tti %d : offset %d (slot %d)\n",tti,tti_offset,slot_offset);
    //Generate Pilots
    
    //antenna 4 symbol 2 Slot 0
  lte_dl_mbsfn(phy_vars_eNB,
	       &txdataF[0][subframe_offset+(2*samples_per_symbol)],
	       amp,
	       subframe,
	       0);

    
    
    //antenna 4 symbol 0 slot 1
  lte_dl_mbsfn(phy_vars_eNB,
	       &txdataF[0][subframe_offset+(6*samples_per_symbol)],
	       amp,
	       subframe,
	       1);
		   
    //antenna 4 symbol 4 slot 1
  lte_dl_mbsfn(phy_vars_eNB,
	       &txdataF[0][subframe_offset+(10*samples_per_symbol)],
	       amp,
	       subframe,
	       2);
     
  return(0);  
}

  
