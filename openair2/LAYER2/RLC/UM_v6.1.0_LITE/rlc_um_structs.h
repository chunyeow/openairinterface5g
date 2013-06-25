/*
                             rlc_um_structs.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#    ifndef __RLC_UM_STRUCTS_H__
#        define __RLC_UM_STRUCTS_H__

#        include "platform_types.h"
#        include "list.h"
#        include "rlc_am_constants.h"
#        include "mac_primitives.h"
#        include "rlc_primitives.h"
//#ifdef USER_MODE
#        include "mac_rlc_primitives.h"
//#endif //USER_MODE
//-----------------------
struct rlc_um_tx_sdu_management {
  u8_t             *first_byte;
  s32_t             sdu_creation_time;
  u16_t             sdu_remaining_size;
  u16_t             sdu_segmented_size;
  u16_t             sdu_size;
  u8_t              use_special_li;
};
//-----------------------
struct rlc_um_tx_pdu_management {
  u8_t             *first_byte;   // point to first byte of RLC header
  u8_t             *payload;
  u16_t             data_size;    // size of payload (not included byte1, LIs, and padding)
  u8_t              dummy[MAC_HEADER_MAX_SIZE];   // optimization for MAC layer : reserve bytes for MAC header
};
//-----------------------
struct rlc_um_rx_pdu_management {
  u8_t             *first_byte;
  u8_t              sn;
};
//-----------------------
struct rlc_um_tx_data_pdu_management {
  union {
    struct rlc_um_tx_pdu_management tx_pdu_mngmnt;
    struct mac_tb_req tb_req;
    struct mac_tx_tb_management tb_mngt;
#        ifdef BYPASS_L1
    struct rlc_um_rx_pdu_management dummy1;
    struct mac_tb_ind dummy2;
    struct mac_rx_tb_management dummy3;
    struct rlc_indication dummy4;
#        endif
  } dummy;
};
//-----------------------
struct rlc_um_rx_data_pdu_struct {
  u8_t              byte1;
  u8_t              li_data_7[1];
};
//-----------------------
struct rlc_um_tx_data_pdu_struct {
  u8_t              byte1;
  u8_t              li_data_7[1];
};
//-----------------------
struct rlc_um_data_req_alloc {  // alloc enought bytes for sdu mngt also
  union {
    struct rlc_um_data_req dummy1;
    struct rlc_um_tx_sdu_management dummy2;
  } dummy;
};
#    endif
