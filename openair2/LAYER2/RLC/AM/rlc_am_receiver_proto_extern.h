/***************************************************************************
                          rlc_am_receiver_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_AM_RECEIVER_PROTO_EXTERN_H__
#        define __RLC_AM_RECEIVER_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     receiver_retransmission_management (struct rlc_am_entity *rlcP, mem_block_t * pduP, struct rlc_am_pdu_header *rlc_headerP);
extern void     free_receiver_buffer (struct rlc_am_entity *rlcP, u16_t indexP);
extern void     insert_into_receiver_buffer (struct rlc_am_entity *rlcP, u16_t indexP, mem_block_t * pduP);
extern void     process_receiver_buffer_15 (struct rlc_am_entity *rlcP);
extern void     process_receiver_buffer_7 (struct rlc_am_entity *rlcP);
#    endif
