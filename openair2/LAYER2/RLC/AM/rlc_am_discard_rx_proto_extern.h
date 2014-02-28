/***************************************************************************
                          rlc_am_discard_rx_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_DISCARD_RX_PROTO_EXTERN_H__
#        define __RLC_AM_DISCARD_RX_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     rlc_am_received_sufi_ack_check_discard_procedures (struct rlc_am_entity *rlcP);
extern void     rlc_am_free_discard_procedure (mem_block_t * mb_current_procedureP);
extern inline void rlc_am_discard_free_receiver_buffer (struct rlc_am_entity *rlcP, uint16_t sn_mrw_iP, uint8_t nlengthP);
extern uint8_t      *retransmission_buffer_management_mrw (struct rlc_am_entity *rlcP, uint8_t * byte1P, uint8_t * byte_alignedP);
extern uint8_t      *retransmission_buffer_management_mrw_ack (struct rlc_am_entity *rlcP, uint8_t * byte1P, uint8_t * byte_alignedP);
#    endif
