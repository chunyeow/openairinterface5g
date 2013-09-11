/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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
/*! \file vars.h
* \brief mac vars
* \author Raymond Knopp, Navid Nikaein
* \date 2012
* \version 1.0
* @ingroup _mac

*/


#ifndef __MAC_VARS_H__
#define __MAC_VARS_H__
#ifdef USER_MODE
//#include "stdio.h"
#endif //USER_MODE
#include "PHY/defs.h"
#include "defs.h"
#include "PHY_INTERFACE/defs.h"
#include "COMMON/mac_rrc_primitives.h"

const u32 BSR_TABLE[BSR_TABLE_SIZE]={0,10,12,14,17,19,22,26,31,36,42,49,57,67,78,91,
			       105,125,146,171,200,234,274,321,376,440,515,603,706,826,967,1132,
			       1326,1552,1817,2127,2490,2915,3413,3995,4677,5467,6411,7505,8787,10287,12043,14099,
			       16507,19325,22624,26487,31009,36304,42502,49759,58255,68201,79846,93479,109439, 128125,150000, 300000};

//u32 EBSR_Level[63]={0,10,13,16,19,23,29,35,43,53,65,80,98,120,147,181};


u32 RRC_CONNECTION_FLAG;

UE_MAC_INST *UE_mac_inst; //[NB_MODULE_MAX]; 
eNB_MAC_INST *eNB_mac_inst; //[NB_MODULE_MAX]; 
MAC_RLC_XFACE *Mac_rlc_xface;

eNB_ULSCH_INFO eNB_ulsch_info[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX]; // eNBxUE = 8x8 
eNB_DLSCH_INFO eNB_dlsch_info[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX]; // eNBxUE = 8x8 

/*
#ifndef USER_MODE
RRC_XFACE *Rrc_xface;
MAC_xface *mac_xface;
#else
#include "PHY_INTERFACE/extern.h"
#include "RRC/LITE/extern.h"
#endif
*/

u8 Is_rrc_registered;

#ifdef OPENAIR2
unsigned char NB_eNB_INST=0;
unsigned char NB_UE_INST=0;
unsigned char NB_RN_INST=0;
unsigned char NB_INST=0;
#endif


DCI0_5MHz_TDD_1_6_t       UL_alloc_pdu;

DCI1A_5MHz_TDD_1_6_t      DLSCH_alloc_pdu1A;
DCI1A_5MHz_TDD_1_6_t      RA_alloc_pdu;
DCI1A_5MHz_TDD_1_6_t      BCCH_alloc_pdu;

DCI1A_5MHz_TDD_1_6_t      CCCH_alloc_pdu;
DCI1_5MHz_TDD_t           DLSCH_alloc_pdu;
#ifdef Rel10
DCI1C_5MHz_t                 MCCH_alloc_pdu;
#endif

DCI0_5MHz_FDD_t       UL_alloc_pdu_fdd;

DCI1A_5MHz_FDD_t      DLSCH_alloc_pdu1A_fdd;
DCI1A_5MHz_FDD_t      RA_alloc_pdu_fdd;
DCI1A_5MHz_FDD_t      BCCH_alloc_pdu_fdd;

DCI1A_5MHz_FDD_t      CCCH_alloc_pdu_fdd;
DCI1_5MHz_FDD_t       DLSCH_alloc_pdu_fdd;

DCI2_5MHz_2A_L10PRB_TDD_t DLSCH_alloc_pdu1;
DCI2_5MHz_2A_M10PRB_TDD_t DLSCH_alloc_pdu2;

DCI1E_5MHz_2A_M10PRB_TDD_t DLSCH_alloc_pdu1E;

#endif


