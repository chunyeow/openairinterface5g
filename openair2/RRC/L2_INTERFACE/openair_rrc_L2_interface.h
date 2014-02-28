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
#include "COMMON/platform_types.h"

int8_t mac_rrc_data_req(module_id_t module_idP, frame_t frameP, rb_id_t srb_idP, uint8_t nb_tbP, uint8_t *buffer_pP, eNB_flag_t eNB_flagP, uint8_t eNB_indexP,
                    uint8_t mbsfn_sync_areaP);

int8_t mac_rrc_data_ind(module_id_t module_idP, frame_t frameP, rb_id_t srb_idP, uint8_t *sduP, sdu_size_t sdu_lenP, eNB_flag_t eNB_flagP, uint8_t eNB_index,
                    uint8_t mbsfn_sync_areaP);

void mac_lite_sync_ind(module_id_t module_idP, uint8_t statusP);

void mac_rrc_meas_ind(module_id_t, MAC_MEAS_REQ_ENTRY*);

void rlcrrc_data_ind(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t rb_idP, sdu_size_t sdu_sizeP, uint8_t *buffer_pP);

uint8_t pdcp_rrc_data_req(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t rb_idP, mui_t muiP, boolean_t confirmP,
                     sdu_size_t sdu_buffer_size, uint8_t* sdu_buffer, uint8_t mode);

void pdcp_rrc_data_ind(module_id_t enb_mod_idP, module_id_t ue_mod_idP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t srb_idP, sdu_size_t sdu_sizeP, uint8_t *buffer_pP);

void mac_out_of_sync_ind(module_id_t module_idP, frame_t frameP, uint16_t CH_index);

char openair_rrc_eNB_init(module_id_t module_idP);

char openair_rrc_ue_init(module_id_t module_idP, unsigned char eNB_indexP);

int mac_get_rrc_status(module_id_t module_idP, eNB_flag_t eNB_flagP, uint8_t indexP);

void mac_in_sync_ind(module_id_t module_idP, frame_t frameP, uint16_t eNB_indexP);

#endif
