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

/******************************************************************************
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*******************************************************************************/
#    ifndef __RLC_UM_ENTITY_H__
#        define __RLC_UM_ENTITY_H__

#        include "platform_types.h"
#        include "platform_constants.h"
#        include "list.h"
#        include "rlc_primitives.h"
#        include "rlc_def.h"
struct rlc_um_entity {
  module_id_t    module_id;

  uint8_t              allocation;
  uint8_t              protocol_state;
  // for stats and trace purpose :
  uint16_t             data_plane;   // act as a boolean
  uint16_t             crc_on_header;   // act as a boolean
  //*****************************************************************************
  // TRANSMITER
  //*****************************************************************************
  //-----------------------------
  // protocol variables
  //-----------------------------
  uint16_t             first_li_in_next_pdu; // indicates :
  // value = 000000000000000 that the previous PDU was exactly
  // with the last segment of an RLC SDU and there is no LI that
  // indicates the end of the SDU in the previous RLC PDU.
  // value = 111111111111011 The last segment of an RLC SDU was one octet
  // short of exactly filling the previous RLC PDU and there is no LI that
  // indicates the end of the SDU in the previous RLC PDU. The remaining one
  // octet in the previous RLC PDU is ignored.
  // value = 111111111111110 AMD PDU: The rest of the RLC PDU includes a
  // piggybacked STATUS PDU.
  // value = 111111111111111 The rest of the RLC PDU is padding. The padding
  // length can be zero.
  uint8_t              vt_us;
  //-----------------------------
  // discard info
  //-----------------------------
  uint8_t              sdu_discard_mode;
  //-----------------------------
  // timers
  //-----------------------------
  uint16_t             timer_discard_init;
  uint32_t            *frame_tick_milliseconds;      // pointer on this tick variable handled by RRC : READ ONLY
  //-----------------------------
  // tranmission
  //-----------------------------
  // sdu communication;
  mem_block_t     **input_sdus;   // should be accessed as an array
  mem_block_t     *input_sdus_alloc;     // allocation of the array
  uint16_t             size_input_sdus_buffer;
  uint16_t             nb_sdu;

  uint16_t             next_sdu_index;       // next location of incoming sdu
  uint16_t             current_sdu_index;

  uint32_t             buffer_occupancy;
  uint16_t             data_pdu_size;
  uint16_t             data_pdu_size_in_bits;
  uint16_t             nb_pdu_requested_by_mac;

  uint8_t              li_one_byte_short_to_add_in_next_pdu;
  uint8_t              li_exactly_filled_to_add_in_next_pdu;
  uint8_t              li_length_15_was_used_for_previous_pdu;

  list_t          pdus_to_mac_layer;
  //-----------------------------
  // C-SAP
  //-----------------------------
  list_t          c_sap;
  //-----------------------------
  // Mapping info
  //-----------------------------
  uint8_t              logical_channel_identity;

  //*****************************************************************************
  // RECEIVER
  //*****************************************************************************
  //-----------------------------
  // protocol variables
  //-----------------------------
  uint8_t              vr_us;        // Receiver Send Sequence state variable
  // The sequence number of the next PDU to be received. It shall be
  // set equal to SN+1 upon reception of a PDU. The initial value of
  // this state variable is 0;
  //-----------------------------
  // receiver
  //-----------------------------
  // the current output sdu is the first in the list
  list_t          output_sdu_list;
  mem_block_t    *output_sdu_in_construction;
  int32_t             output_sdu_size_to_write;     // for writing in sdu

  //struct rlc_um_data_ind   output_rlc_primitive;// for writing header in rt_fifo

  list_t          pdus_from_mac_layer;

  uint8_t              last_reassemblied_sn:7;
  uint16_t              rb_id;

  uint32_t             tx_sdus;
  uint32_t             rx_sdus;
  uint32_t             tx_pdus;
  uint32_t             rx_pdus;
  uint32_t             rx_pdus_in_error;
  uint8_t              first_pdu;
  
  unsigned int tx_pdcp_sdu;
  unsigned int tx_pdcp_sdu_discarded;
  unsigned int tx_data_pdu;
  unsigned int rx_sdu;
  unsigned int rx_error_pdu;  
  unsigned int rx_data_pdu;
  unsigned int rx_data_pdu_out_of_window;
};
#    endif
