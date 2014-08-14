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
/***************************************************************************
                          rlc_am_structs.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#    ifndef __RLC_AM_STRUCTS_H__
#        define __RLC_AM_STRUCTS_H__

#        include "platform_types.h"
#        include "platform_constants.h"
#        include "list.h"
#        include "mem_block.h"
#        include "rlc_am_constants.h"
//#ifdef USER_MODE
#        include "mac_rlc_primitives.h" 
//#endif //USER_MODE
#        include "mac_primitives.h"
#        include "rlc_primitives.h"
//-----------------------------------------------------------------------------
// SDU MANAGEMENT
//-----------------------------------------------------------------------------
struct rlc_am_tx_sdu_management {
  uint8_t             *first_byte;   // payload
  int32_t             sdu_creation_time;
  uint32_t             mui;          // mui may be used only by radio access bearers
  uint16_t             sdu_remaining_size;
  uint16_t             sdu_segmented_size;
  uint16_t             sdu_size;
  int16_t             pdus_index[42];       // sn of pdus (is an array)
  uint16_t             last_pdu_sn;
  uint8_t              nb_pdus;      // number of pdus were this sdu was segmented
  uint8_t              nb_pdus_internal_use; // count the number of pdus transmitted to lower layers (used in mux procedure)
  uint8_t              nb_pdus_ack;  // counter used for confirm and discard MaxDAT
  uint8_t              nb_pdus_time; // counter used for timer based discard

  uint8_t              confirm;      // confirm may be used only by signalling radio bearers

  int8_t              li_index_for_discard; // indicates the li index in the last pdu of the sdu, marking the end of the sdu
  uint8_t              discarded;
  uint8_t              segmented;    // != 0 if segmentation running or achieved
  uint8_t              no_new_sdu_segmented_in_last_pdu;
};
//-----------------------------------------------------------------------------
// SDU DISCARD
//-----------------------------------------------------------------------------
struct rlc_am_discard_procedure {
  // list of sdu discarded involved in this procedure
  list_t          sdu_list;     // index of sdu (index in rlc_am_entity.input_sdus[])
  uint8_t              nb_sdu;
  mem_block_t    *control_pdu;
  uint16_t             last_pdu_sn;  // for update of vt_s
  uint8_t              length;
  uint8_t              nlength;
  uint8_t              running;
};
//-----------------------------------------------------------------------------
// DATA PDU
//-----------------------------------------------------------------------------
struct rlc_am_rx_pdu_management {
  uint8_t             *first_byte;
  uint16_t             sn;
  uint8_t              piggybacked_processed;        // the piggybacked info if any of the pdu has already been processed.
};

struct rlc_am_tx_data_pdu_management {
  mem_block_t    *copy;         //pointer on pdu queued in retransmission_buffer_to_send in order
  // not to insert the same pdu to retransmit twice, also to avoid the transmission of the pdu if acknowledged
  // but previously was queued for retransmission in retransmission_buffer_to_send but not sent because of
  // limited number of pdu delivered by TTI.
  uint8_t             *first_byte;   // pointer on the pdu including header, LIs;
  uint8_t             *payload;      // pointer on the data field of the pdu;
  int16_t             sdu[15];      // index of sdu having segments in this pdu (index in rlc_am_entity.input_sdus[])
  uint16_t             padding_size; // size of padding area if any in a data pdu(for piggybacking)
  uint16_t             data_size;
  uint16_t             sn;


  uint8_t              nb_sdu;       // number of sdu having segments in this pdu
  uint8_t              vt_dat;       // This state variable counts the number of times a PDU has been transmitted;
  // there is one VT(DAT) for each PDU and it is incremented each time the PDU is transmitted;
  int8_t              ack;          // used when pdu is in retransmission buffer;
  // values are RLC_PDU_ACK_NO_EVENT, RLC_PDU_ACK_EVENT, RLC_PDU_NACK_EVENT
  uint8_t              last_pdu_of_sdu;      // nb of sdus ending in this pdu (for polling trigger)

  uint16_t             rlc_tb_type;  // mac will return this value with the tx status
  uint8_t              dummy[MAC_HEADER_MAX_SIZE];   // reserve area for mac header !!! always the last declaration in this struct !!!
};

struct rlc_am_tx_control_pdu_management {
  uint16_t             rlc_tb_type;  // mac will return this value with the tx status
  uint8_t              dummy[MAC_HEADER_MAX_SIZE];   // reserve area for mac header !!! always the last declaration in this struct !!!
};

//-----------------------------------------------------------------------------
// HEADERS
//-----------------------------------------------------------------------------
struct rlc_am_pdu_header {
  uint8_t              byte1;
  uint8_t              byte2;
  uint8_t              li_data_7[1];
};

struct rlc_am_reset_header {
  uint8_t              byte1;
  uint8_t              hfni[3];      // is coded on 20 most significant bits of 24 bits
};

struct rlc_am_status_header {
  uint8_t              byte1;
  uint8_t              suffi[1];     // next suffi(s)
};
//-----------------------------------------------------------------------------
//  interlayers optimizations
//-----------------------------------------------------------------------------
struct rlc_am_tx_data_pdu_allocation {
  union {
    struct rlc_am_tx_data_pdu_management rlc_am_tx_pdu_mngmnt;
    struct mac_tb_req tb_req;
    struct mac_tx_tb_management tb_mngt;
#        ifdef BYPASS_L1
    struct rlc_am_rx_pdu_management dummy;
    struct mac_tb_ind dummy2;
    struct mac_rx_tb_management dummy3;
    struct rlc_indication dummy4;
#        endif
  } dummy;
};

struct rlc_am_tx_control_pdu_allocation {
  union {
    struct mac_tb_req tb_req;
    struct mac_tx_tb_management tb_mngt;
    struct rlc_am_tx_control_pdu_management rlc_am_tx_pdu_mngmnt;
#        ifdef BYPASS_L1
    struct mac_tb_ind dummy2;
    struct mac_rx_tb_management dummy3;
    struct rlc_indication dummy4;
#        endif
  } dummy;
};

struct rlc_am_data_req_alloc {  // alloc enought bytes for sdu mngt also
  union {
    struct rlc_am_data_req dummy1;
    struct rlc_am_tx_sdu_management dummy2;
  } dummy;
};
//-----------------------------------------------------------------------------
//  MISC
//-----------------------------------------------------------------------------
struct sufi_to_insert_in_status {
  uint16_t             sn;
  uint8_t              length;       // in quartets, sufi invalid if length is zero
  uint8_t              type;         // ACK, BITMAP, LIST, etc
  uint8_t              sufi[SUFI_MAX_SIZE];
};

struct sufi_bitmap {
  uint16_t             fsn;
  uint8_t              valid;
  uint8_t              length;
  uint8_t              mask;
  uint8_t              bitmap[16];
};

struct pair {
  uint16_t             sn;
  uint8_t              valid;
  uint8_t              length;
};

struct sufi_list {
  struct pair     pairs[15];
  uint8_t              valid;
  uint8_t              length;
};

struct sufi_ack {
  uint16_t             lsn;
  uint8_t              vr_r_modified;        // generate ack with vr_r value;
  uint8_t              ack_other_vr_r;       // generate ack with lsn  value;
};
//-----------------------------------------------------------------------------
//  STATUS GENERATION
//-----------------------------------------------------------------------------
struct rlc_am_hole {
  uint16_t             valid;
  uint16_t             fsn;
  uint16_t             length;
  uint16_t             dist_to_next; //count in sn
};
#    endif
