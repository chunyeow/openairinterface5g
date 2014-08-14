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

/*! \file l2_interface.c
* \brief layer 2 interface 
* \author Navid Nikaein and Raymond Knopp
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: navid.nikaein@eurecom.fr,raymond.knopp@eurecom.fr 
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
