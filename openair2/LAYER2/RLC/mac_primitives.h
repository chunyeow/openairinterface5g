/***************************************************************************
                          mac_primitives.h  -  description
                             -------------------
 ***************************************************************************/
#ifndef __MAC_PRIMITIVES_H__
#    define __MAC_PRIMITIVES_H__

#warning including mac_primitives.h

#    include "platform_types.h"
#    include "platform_constants.h"
#    include "list.h"
#    include "mem_block.h"

#    define MAC_DCCH                              0xCC
#    define MAC_DTCH                              0xDC
//----------------------------------------------------------
// primitives
//----------------------------------------------------------
#    define MAC_DATA_REQ                          0x01
#    define MAC_DATA_IND                          0x02
#    define MAC_STATUS_IND                        0x04
#    define MAC_STATUS_RESP                       0x08
//----------------------------------------------------------
// primitives definition
//----------------------------------------------------------
#    define UE_ID_TYPE_U_RNTI 0xFF
                                // should not be used ?
#    define UE_ID_TYPE_C_RNTI 0xCC

#    define MAC_TX_STATUS_SUCCESSFUL              0x0F
#    define MAC_TX_STATUS_UNSUCCESSFUL            0xF0

#    define MAC_HEADER_MAX_SIZE                   6
// from 3GPP TS 25.321 V4.2.0
// MAC-DATA-Req/Ind:
// -    MAC-DATA-Req primitive is used to request that an upper layer PDU be sent using the
//    procedures for the information transfer service;
// -    MAC-DATA-Ind primitive indicates the arrival of upper layer PDUs received within one
//    transmission time interval by means of the information transfer service.
// MAC-STATUS-Ind/Resp:
// -    MAC-STATUS-Ind primitive indicates to RLC for each logical channel the rate at which
//    it may transfer data to MAC. Parameters are the number of PDUs that can be transferred
//    in each transmission time interval and the PDU size; it is possible that MAC would use
//    this primitive to indicate that it expects the current buffer occupancy of the addressed
//    logical channel in order to provide for optimised TFC selection on transport channels
//    with long transmission time interval. At the UE, MAC-STATUS-Ind primitive is also used
//    to indicate from MAC to RLC that MAC has requested data transmission by PHY
//    (i.e. PHY-DATA-REQ has been submitted, see Fig. 11.2.2.1), or that transmission of an
//    RLC PDU on RACH or CPCH has failed due to exceeded preamble ramping cycle counter.
// -    MAC-STATUS-Resp primitive enables RLC to acknowledge a MAC-STATUS-Ind. It is possible
//    that RLC would use this primitive to indicate that it has nothing to send or that it is
//    in a suspended state or to indicate the current buffer occupancy to MAC.

// Parameters:
// a)   data:
// -    it contains the RLC layer messages (RLC-PDU) to be transmitted, or the RLC layer messages
//    that have been received by the MAC sub-layer.
//
// b)   no_tb :
// -    indicates the number of transport blocks transmitted by the peer entity within the
//    transmission time interval, based on the TFI value.
//
// c)   buffer_occupancy :
// -    the parameter Buffer Occupancy (BO) indicates for each logical channel the amount of data
//    in number of bytes that is available for transmission and retransmission in RLC layer.
//    When MAC is connected to an AM RLC entity, control PDUs to be transmitted and RLC PDUs
//    outside the RLC Tx window shall also be included in the BO. RLC PDUs that have been
//    transmitted but not negatively acknowledged by the peer entity shall not be included in the BO.
//
// d)   rx_timing_deviation, TDD only:
// -    it contains the RX Timing Deviation as measured by the physical layer for the physical
//    resources carrying the data of the Message Unit. This parameter is optional and only for
//    Indication. It is needed for the transfer of the RX Timing Deviation measurement of RACH
//    transmissions carrying CCCH data to RRC.
//
// e)   no_pdu:
// -    specifies the number of PDUs that the RLC is permitted to transfer to MAC within a
//    transmission time interval.
//
// f)   pdu_size:
// -    specifies the size of PDU that can be transferred to MAC within a transmission time interval.
//
// g)   UE-ID Type Indicator:
// -    indicates the UE-ID type to be included in MAC for a DCCH when it is mapped onto a common
//    transport channel (i.e. FACH, RACH, DSCH in FDD or CPCH). On the UE side UE-ID Type Indicator
//    shall always be set to C-RNTI.
//
// h)   tx_status:
// -    when set to value "transmission unsuccessful" this parameter indicates to RLC that transmission
//    of an RLC PDU failed in the previous Transmission Time Interval, when set to value
//    "transmission successful" this parameter indicates to RLC that the requested RLC PDU(s)
//     has been submitted for transmission by the physical layer.
//
// i)   rlc_info
// -    indicates to MAC the configuration parameters that are critical to TFC selection depending
//    on its mode and the amount of data that could be transmitted at the next TTI. This primitive
//    is meant to insure that MAC can perform TFC selection.

#ifdef USER_MODE
struct rlc_entity_info {
  u8_t              rlc_protocol_state;
};

struct mac_tx_tb_management {
  // BE CAREFULL TO KEEP THE SAME MAPPING FOR THE 6 FIELDS BELLOW AS FOR  struct mac_tb_req
  u8_t             *data_ptr;
  u8_t              first_bit;    // 0 if data starts on byte boundary(b7), 1 if b6, 2 if b5, etc.
  // Previously designed for interlayers optimizations, (avoid aligning on byte boundary)
  // but not used by L1 !, so extra cost for alignement in MAC.
  u16_t             tb_size_in_bits;      // L1H does not care of the field first bit any more, so in order to byte
  // align the tb we have to know its size

  // for reporting tx status to upper layers
  void           *rlc;
  void            (*rlc_callback) (void *rlcP, u16_t rlc_tb_type);
  u16_t             rlc_tb_type;

  u16_t             log_ch_interface_src; // index of the logical channel interface from which the tb was submitted
  u8_t              mac_header[MAC_HEADER_MAX_SIZE];      // worst case : tctf(5bits)+UE-Id type(2bits)+UE-Id(16 or 32bits)+C/T(4bits)=43bits max
};

struct mac_rx_tb_management {
  u8_t             *data_ptr;
  u16_t             tb_size;      // in bits
  u8_t              valid_checksum;
  u8_t              first_bit;    // 0 if data starts on byte boundary(b7), 1 if b6, 2 if b5, etc
};

struct mac_tb_req {
  // BE CAREFULL TO KEEP THE SAME MAPPING FOR THE 6 FIELDS BELLOW AS FOR  struct mac_tx_tb_management
  u8_t             *data_ptr;
  u8_t              first_bit;
  u16_t             tb_size_in_bits;      // L1H does not care of the field first bit any more, so in order to byte
  // align the tb we have to know its size

  // for reporting tx status to upper layers
  void           *rlc;
  void            (*rlc_callback) (void *rlcP, u16_t rlc_tb_type);
  u16_t             rlc_tb_type;

  u8_t              mac_header[MAC_HEADER_MAX_SIZE];      // worst case : tctf(5bits)+UE-Id type(2bits)+UE-Id(16 or 32bits)+C/T(4bits)=43bits max
};

struct mac_status_ind {
  u16_t             no_pdu;
  u16_t             tx_status;    // successful, unsuccessful
};

struct mac_tb_ind {
  u8_t             *data_ptr;
  u16_t             size;
  u8_t              error_indication;
  //u8_t              first_bit;    // 0 if data starts on byte boundary(b7), 1 if b6, 2 if b5, etc
};
//---------------------
struct mac_data_req {
  list_t          data;
  u32_t             buffer_occupancy_in_bytes;
  u16_t             buffer_occupancy_in_pdus;
  struct rlc_entity_info rlc_info;
  u8_t              ue_id_type_indicator;
};
//---------------------
struct mac_data_ind {
  list_t          data;
  u16_t             no_tb;
  u16_t             tb_size;      // in bits
  //u8_t              error_indication;
  //u8_t              rx_timing_deviation;
};
//---------------------
struct mac_status_resp {
  u32_t             buffer_occupancy_in_bytes;
  u16_t             buffer_occupancy_in_pdus;
  struct rlc_entity_info rlc_info;
};
//---------------------
struct mac_primitive {
  u8_t              primitive_type;
  union {
    struct mac_data_req data_req;
    struct mac_status_resp status_resp;
    struct mac_data_ind data_ind;
    //struct mac_status_ind  status_ind;
  } primitive;
};

#endif //USER_MODE

typedef u8_t  transport_format_id_t;
typedef u32_t transport_block_size_bit_t;
typedef u8_t  flow_id_t;

#    ifdef BYPASS_L1

// in bytes
#define MAC_BYPASS_TB_MAX_SIZE                100


typedef struct mac_bypass_flow_info_t {
  u8_t                              flow_id              __attribute__ ((packed));
  transport_format_id_t           transport_format     __attribute__ ((packed));
  u8_t                              num_transport_blocks __attribute__ ((packed));
  transport_block_size_bit_t      transport_block_size __attribute__ ((packed));
}mac_bypass_flow_info_t;


/*
typedef struct mac_bypass_cell_info_t {
  u8_t              num_cells;
  rg_id_t       cells_id_list[MAC_BYPASS_MAX_CELLS];
}mac_bypass_cell_info_t;

typedef struct mac_bypass_mobile_info_t {
  mobile_id_t        mobile_id;
}mac_bypass_mobile_info_t;

typedef struct mac_bypass_flows_info_t {
  u8_t              num_flows;
}mac_bypass_flows_info_t;

typedef struct mac_bypass_flow_info_t {
  u8_t                              flow_id              __attribute__ ((packed));
  transport_format_id_t           transport_format     __attribute__ ((packed));
  u8_t                              num_transport_blocks __attribute__ ((packed));
  transport_block_size_bit_t      transport_block_size __attribute__ ((packed));
}mac_bypass_flow_info_t;

typedef struct mac_bypass_rg_msg_data_header_t {
  mac_bypass_cell_info_t              cell_info        __attribute__ ((packed));
  mac_bypass_flows_info_t             flows_info       __attribute__ ((packed));
} mac_bypass_rg_msg_data_header_t;

typedef struct mac_bypass_rg_msg_cx_resp_header_t {
  mac_bypass_random_ue_id_t           random_ue_id     __attribute__ ((packed));
  mac_bypass_ue_id_t                  ue_id            __attribute__ ((packed));
} mac_bypass_rg_msg_cx_resp_header_t;

typedef struct mac_bypass_rg_msg_header_t {
  mac_bypass_message_type_t           message_type     __attribute__ ((packed));
  union  {
    mac_bypass_rg_msg_data_header_t    data_header;
    mac_bypass_rg_msg_cx_resp_header_t cx_resp_header;
  } select;

} mac_bypass_rg_msg_header_t;

typedef struct mac_bypass_mt_msg_data_header_t {
  mac_bypass_mobile_info_t            mobile_info      __attribute__ ((packed));
  mac_bypass_flows_info_t             flows_info       __attribute__ ((packed));
} mac_bypass_mt_msg_data_header_t;

typedef struct mac_bypass_mt_msg_cx_req_header_t {
  mac_bypass_random_ue_id_t           random_ue_id     __attribute__ ((packed));
} mac_bypass_mt_msg_cx_req_header_t;

typedef struct mac_bypass_mt_msg_header_t {
  mac_bypass_message_type_t           message_type     __attribute__ ((packed));
  union  {
    mac_bypass_mt_msg_data_header_t    data_header;
    mac_bypass_mt_msg_cx_req_header_t  cx_req_header;
  } select;
} mac_bypass_mt_msg_header_t;
*/



/*
// List element of TrCh Blocks data
struct Bypass_TrChBlk_MAC_Interface {
  u8_t              first_bit;    // First valid Bit in first word. 0 if word is full
  u8_t              valid_checksum;       // 1 if valid checksum (receive only)
  u8_t              peer_trch_id;
  u32_t             data_start_index;
  u8_t              data[100];
};

struct Bypass_TrChData_MAC_Interface {
  u16_t             tf;
  u8_t              nb_blocks;
  u8_t              updated;
  struct Bypass_TrChBlk_MAC_Interface tb[32];
};

struct Bypass_L1 {
  struct Bypass_TrChData_MAC_Interface ul_trch[JRRM_MAX_TRCH_RG];       // MT write data here. RG read here, index are rg trch ids
  struct Bypass_TrChData_MAC_Interface dl_trch[JRRM_MAX_TRCH_RG];       // RG write data here. MT read here,
  u8_t              mt_ack[JRRM_MAX_MANAGED_MOBILES_PER_RG];
  u8_t              rg_ack[JRRM_MAX_MANAGED_MOBILES_PER_RG];
  u8_t              mt_wrote[JRRM_MAX_MANAGED_MOBILES_PER_RG];
  u8_t              rg_wrote[JRRM_MAX_MANAGED_MOBILES_PER_RG];
  u8_t              num_mobiles;

  u8_t              join_request; // act as boolean
  u8_t              detach_request;       // act as boolean
  u8_t              join_requests[JRRM_MAX_MANAGED_MOBILES_PER_RG];
  u8_t              join_acks[JRRM_MAX_MANAGED_MOBILES_PER_RG];
  u8_t              detach_requests[JRRM_MAX_MANAGED_MOBILES_PER_RG];
  u8_t              detach_acks[JRRM_MAX_MANAGED_MOBILES_PER_RG];
};
*/

#    endif
#endif
