/***************************************************************************
                          rlc_am_reset_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_RESET_PROTO_EXTERN_H__
#        define __RLC_AM_RESET_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
//-----------------------------------------------------------------------------
extern void     rlc_am_reset_time_out (struct rlc_am_entity *rlcP, mem_block_t * not_usedP);
extern void     send_reset_ack_pdu (u8_t rsnP, struct rlc_am_entity *rlcP);
extern void     send_reset_pdu (struct rlc_am_entity *rlcP);
extern void     reset_rlc_am (struct rlc_am_entity *rlcP);
extern void     process_reset_ack (mem_block_t * pduP, struct rlc_am_reset_header *controlP, struct rlc_am_entity *rlcP);
extern void     process_reset (mem_block_t * pduP, struct rlc_am_reset_header *controlP, struct rlc_am_entity *rlcP);
#    endif
