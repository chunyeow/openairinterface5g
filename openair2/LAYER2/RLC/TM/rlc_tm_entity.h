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
  uint8_t              allocation;
  uint8_t              protocol_state;
  // for stats and trace purpose :
  uint16_t             data_plane;   // act as a boolean
  uint16_t              rb_id;
  //-----------------------------
  // discard info
  //-----------------------------
  uint8_t              sdu_discard_mode;
  //-----------------------------
  // time
  //-----------------------------
  uint16_t             timer_discard_init;
  uint32_t            *frame_tick_milliseconds;      // pointer on this tick variable handled by RRC : READ ONLY
  int32_t             last_tti;
  //-----------------------------
  // tranmission
  //-----------------------------
  // sdu communication;
  mem_block_t   **input_sdus;   // should be accessed as an array
  mem_block_t    *input_sdus_alloc;     // allocation of the array
  uint16_t             size_input_sdus_buffer;
  uint16_t             nb_sdu;
  void            (*segmentation) (struct rlc_tm_entity * rlcP);

  uint16_t             next_sdu_index;       // next location of incoming sdu
  uint16_t             current_sdu_index;

  list_t          pdus_to_mac_layer;

  uint16_t             rlc_pdu_size;
  uint16_t             nb_pdu_requested_by_mac;
  uint8_t              segmentation_indication;
  uint8_t              delivery_of_erroneous_sdu;
  uint32_t             buffer_occupancy;
  //-----------------------------
  // receiver
  //-----------------------------
  unsigned int    output_sdu_size_to_write;     // for writing in sdu
  mem_block_t    *output_sdu_in_construction;
  void            (*rx) (void *argP, struct mac_data_ind data_indP);
  uint8_t              last_bit_position_reassemblied;


  list_t          pdus_from_mac_layer;

  //-----------------------------
  // C-SAP
  //-----------------------------
  list_t          c_sap;
  //-----------------------------
  // Mapping info
  //-----------------------------
  uint8_t              logical_channel_identity;
};
#    endif
