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
  u8_t             *first_byte;   // payload
  s32_t             sdu_creation_time;
  u32_t             mui;          // mui may be used only by radio access bearers
  u16_t             sdu_remaining_size;
  u16_t             sdu_segmented_size;
  u16_t             sdu_size;
  s16_t             pdus_index[42];       // sn of pdus (is an array)
  u16_t             last_pdu_sn;
  u8_t              nb_pdus;      // number of pdus were this sdu was segmented
  u8_t              nb_pdus_internal_use; // count the number of pdus transmitted to lower layers (used in mux procedure)
  u8_t              nb_pdus_ack;  // counter used for confirm and discard MaxDAT
  u8_t              nb_pdus_time; // counter used for timer based discard

  u8_t              confirm;      // confirm may be used only by signalling radio bearers

  s8_t              li_index_for_discard; // indicates the li index in the last pdu of the sdu, marking the end of the sdu
  u8_t              discarded;
  u8_t              segmented;    // != 0 if segmentation running or achieved
  u8_t              no_new_sdu_segmented_in_last_pdu;
};
//-----------------------------------------------------------------------------
// SDU DISCARD
//-----------------------------------------------------------------------------
struct rlc_am_discard_procedure {
  // list of sdu discarded involved in this procedure
  list_t          sdu_list;     // index of sdu (index in rlc_am_entity.input_sdus[])
  u8_t              nb_sdu;
  mem_block_t    *control_pdu;
  u16_t             last_pdu_sn;  // for update of vt_s
  u8_t              length;
  u8_t              nlength;
  u8_t              running;
};
//-----------------------------------------------------------------------------
// DATA PDU
//-----------------------------------------------------------------------------
struct rlc_am_rx_pdu_management {
  u8_t             *first_byte;
  u16_t             sn;
  u8_t              piggybacked_processed;        // the piggybacked info if any of the pdu has already been processed.
};

struct rlc_am_tx_data_pdu_management {
  mem_block_t    *copy;         //pointer on pdu queued in retransmission_buffer_to_send in order
  // not to insert the same pdu to retransmit twice, also to avoid the transmission of the pdu if acknowledged
  // but previously was queued for retransmission in retransmission_buffer_to_send but not sent because of
  // limited number of pdu delivered by TTI.
  u8_t             *first_byte;   // pointer on the pdu including header, LIs;
  u8_t             *payload;      // pointer on the data field of the pdu;
  s16_t             sdu[15];      // index of sdu having segments in this pdu (index in rlc_am_entity.input_sdus[])
  u16_t             padding_size; // size of padding area if any in a data pdu(for piggybacking)
  u16_t             data_size;
  u16_t             sn;


  u8_t              nb_sdu;       // number of sdu having segments in this pdu
  u8_t              vt_dat;       // This state variable counts the number of times a PDU has been transmitted;
  // there is one VT(DAT) for each PDU and it is incremented each time the PDU is transmitted;
  s8_t              ack;          // used when pdu is in retransmission buffer;
  // values are RLC_PDU_ACK_NO_EVENT, RLC_PDU_ACK_EVENT, RLC_PDU_NACK_EVENT
  u8_t              last_pdu_of_sdu;      // nb of sdus ending in this pdu (for polling trigger)

  u16_t             rlc_tb_type;  // mac will return this value with the tx status
  u8_t              dummy[MAC_HEADER_MAX_SIZE];   // reserve area for mac header !!! always the last declaration in this struct !!!
};

struct rlc_am_tx_control_pdu_management {
  u16_t             rlc_tb_type;  // mac will return this value with the tx status
  u8_t              dummy[MAC_HEADER_MAX_SIZE];   // reserve area for mac header !!! always the last declaration in this struct !!!
};

//-----------------------------------------------------------------------------
// HEADERS
//-----------------------------------------------------------------------------
struct rlc_am_pdu_header {
  u8_t              byte1;
  u8_t              byte2;
  u8_t              li_data_7[1];
};

struct rlc_am_reset_header {
  u8_t              byte1;
  u8_t              hfni[3];      // is coded on 20 most significant bits of 24 bits
};

struct rlc_am_status_header {
  u8_t              byte1;
  u8_t              suffi[1];     // next suffi(s)
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
  u16_t             sn;
  u8_t              length;       // in quartets, sufi invalid if length is zero
  u8_t              type;         // ACK, BITMAP, LIST, etc
  u8_t              sufi[SUFI_MAX_SIZE];
};

struct sufi_bitmap {
  u16_t             fsn;
  u8_t              valid;
  u8_t              length;
  u8_t              mask;
  u8_t              bitmap[16];
};

struct pair {
  u16_t             sn;
  u8_t              valid;
  u8_t              length;
};

struct sufi_list {
  struct pair     pairs[15];
  u8_t              valid;
  u8_t              length;
};

struct sufi_ack {
  u16_t             lsn;
  u8_t              vr_r_modified;        // generate ack with vr_r value;
  u8_t              ack_other_vr_r;       // generate ack with lsn  value;
};
//-----------------------------------------------------------------------------
//  STATUS GENERATION
//-----------------------------------------------------------------------------
struct rlc_am_hole {
  u16_t             valid;
  u16_t             fsn;
  u16_t             length;
  u16_t             dist_to_next; //count in sn
};
#    endif
