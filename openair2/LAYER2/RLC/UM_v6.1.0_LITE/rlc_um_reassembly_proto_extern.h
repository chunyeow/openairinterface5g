/***************************************************************************
                          rlc_um_reassembly_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_UM_REASSEMBLY_PROTO_EXTERN_H__
#        define __RLC_UM_REASSEMBLY_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_um_entity.h"
//-----------------------------------------------------------------------------
extern void     rlc_um_send_sdu_minus_1_byte (struct rlc_um_entity *rlcP);
extern void     rlc_um_clear_rx_sdu (struct rlc_um_entity *rlcP);
extern void     rlc_um_reassembly (u8_t * srcP, u16_t lengthP, struct rlc_um_entity *rlcP);
extern void     rlc_um_send_sdu (struct rlc_um_entity *rlcP);
#    endif
