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

//#        include "platform.h"
#        include "list.h"
#        include "rlc_primitives.h"
#        include "rlc_def.h"

#        ifndef NO_THREAD_SAFE
#            if !defined(USER_MODE) && defined(RTAI_ENABLED)
#                include <rtai.h>
#                include <rtai_posix.h>
#            else
#                include <pthread.h>
#            endif
#        endif
struct rlc_um_entity {
#        ifndef NO_THREAD_SAFE
  pthread_mutex_t mutex_input_buffer;
#        endif
  uint8_t              allocation;
  uint8_t              protocol_state;
  // for stats and trace purpose :
  uint16_t             data_plane;   // act as a boolean
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
  uint8_t              vt_us;        // This state variable contains the "Sequence Number" 
  // of the next UMD PDU to be transmitted. It shall be 
  // incremented by 1 each time a UMD PDU is transmitted.
  // The initial value of this variable is 0.
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
  mem_block     **input_sdus;   // should be accessed as an array
  mem_block      *input_sdus_alloc;     // allocation of the array
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

  cnt_list_up     pdus_to_mac_layer;
  //-----------------------------
  // C-SAP
  //-----------------------------
  list            c_sap;
  //-----------------------------
  // Mapping info
  //-----------------------------
  uint8_t              logical_channel_identity;

  void           *upper_layer;  // may be PDCP or (RRC)
  //*****************************************************************************
  // RECEIVER
  //*****************************************************************************
  //-----------------------------
  // protocol variables
  //-----------------------------
  uint8_t              vr_us;        // This state variable contains the "Sequence Number" 
  // following that of the last UMD PDU received. When a UMD PDU 
  // with "Sequence Number" equal to x is received, the state
  // variable shall set equal to x + 1.
  // The initial value of this variable is 0
  //-----------------------------
  // receiver
  //-----------------------------
  // the current output sdu is the first in the list
  list            output_sdu_list;
  mem_block      *output_sdu_in_construction;
  unsigned int    output_sdu_size_to_write;     // for writing in sdu

  //struct rlc_um_data_ind   output_rlc_primitive;// for writing header in rt_fifo

  list_up         pdus_from_mac_layer;

  uint8_t              last_reassemblied_sn:7;
  uint8_t              rb_id;
  void           *(*rlc_data_ind) (void *, mem_block * sduP, uint8_t);

  uint32_t             tx_sdus;
  uint32_t             rx_sdus;
  uint32_t             tx_pdus;
  uint32_t             rx_pdus;
  uint32_t             rx_pdus_in_error;
  uint8_t              first_pdu;
};
#    endif
