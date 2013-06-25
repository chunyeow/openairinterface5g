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
/*! \file rlc_tm_entity.h
* \brief This file defines the RLC TM variables stored in a struct called rlc_tm_entity_t.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
#    ifndef __RLC_TM_ENTITY_H__
#        define __RLC_TM_ENTITY_H__
//-----------------------------------------------------------------------------
#        include "platform_types.h"
#        include "platform_constants.h"
#        include "rlc_tm_structs.h"
#        include "rlc_def.h"
//-----------------------------------------------------------------------------
struct rlc_tm_entity {

  module_id_t     module_id;
  u8_t              allocation;
  u8_t              protocol_state;
  // for stats and trace purpose :
  u16_t             data_plane;   // act as a boolean
  u16_t              rb_id;
  //-----------------------------
  // discard info
  //-----------------------------
  u8_t              sdu_discard_mode;
  //-----------------------------
  // time
  //-----------------------------
  u16_t             timer_discard_init;
  u32_t            *frame_tick_milliseconds;      // pointer on this tick variable handled by RRC : READ ONLY
  s32_t             last_tti;
  //-----------------------------
  // tranmission
  //-----------------------------
  // sdu communication;
  mem_block_t   **input_sdus;   // should be accessed as an array
  mem_block_t    *input_sdus_alloc;     // allocation of the array
  u16_t             size_input_sdus_buffer;
  u16_t             nb_sdu;
  void            (*segmentation) (struct rlc_tm_entity * rlcP);

  u16_t             next_sdu_index;       // next location of incoming sdu
  u16_t             current_sdu_index;

  list_t          pdus_to_mac_layer;

  u16_t             rlc_pdu_size;
  u16_t             nb_pdu_requested_by_mac;
  u8_t              segmentation_indication;
  u8_t              delivery_of_erroneous_sdu;
  u32_t             buffer_occupancy;
  //-----------------------------
  // receiver
  //-----------------------------
  unsigned int    output_sdu_size_to_write;     // for writing in sdu
  mem_block_t    *output_sdu_in_construction;
  void            (*rx) (void *argP, struct mac_data_ind data_indP);
  u8_t              last_bit_position_reassemblied;


  list_t          pdus_from_mac_layer;

  //-----------------------------
  // C-SAP
  //-----------------------------
  list_t          c_sap;
  //-----------------------------
  // Mapping info
  //-----------------------------
  u8_t              logical_channel_identity;
};
#    endif
