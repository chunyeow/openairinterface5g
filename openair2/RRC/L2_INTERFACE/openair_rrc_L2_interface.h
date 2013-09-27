/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

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

/*! \file l2_interface.c
* \brief layer 2 interface 
* \author Raymond Knopp and Navid Nikaein 
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: raymond.knopp@eurecom.fr 
*/ 
#ifndef __OPENAIR_RRC_L2_INTERFACE_H__
#define __OPENAIR_RRC_L2_INTERFACE_H__

#include "COMMON/mac_rrc_primitives.h"

s8 mac_rrc_data_req( u8 Mod_id, u32 frame, u16 Srb_id, u8 Nb_tb,char *Buffer,u8 eNB_flag,u8 eNB_index, u8 mbsfn_sync_area);
s8 mac_rrc_data_ind( u8 Mod_id,  u32 frame, u16 Srb_id, char *Sdu, u16 Sdu_len,u8 eNB_flag, u8 eNB_index,u8 mbsfn_sync_area);
void mac_lite_sync_ind( u8 Mod_id, u8 status);
void mac_rrc_meas_ind(u8,MAC_MEAS_REQ_ENTRY*);
void rlcrrc_data_ind( u8 Mod_id, u32 frame, u8 eNB_flag,unsigned int Rb_id, u32 sdu_size,u8 *Buffer);
void pdcp_rrc_data_ind( u8 Mod_id, u32 frame, u8 eNB_flag, unsigned int Srb_id, unsigned int Sdu_size,u8 *Buffer);
void mac_out_of_sync_ind(u8 Mod_id,u32 frame,u16 CH_index);
char openair_rrc_eNB_init(u8 Mod_id);
char openair_rrc_ue_init(u8 Mod_id, unsigned char eNB_index);
int mac_get_rrc_status(u8 Mod_id,u8 eNB_flag,u8 index);
void mac_in_sync_ind(u8 Mod_id,u32 frame, u16 eNB_index);

#endif
