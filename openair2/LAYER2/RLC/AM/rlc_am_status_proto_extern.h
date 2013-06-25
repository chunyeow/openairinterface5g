/***************************************************************************
                          rlc_am_status_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr



 ***************************************************************************/
#    ifndef __RLC_AM_STATUS_PROTO_EXTERN_H__
#        define __RLC_AM_STATUS_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "rlc_am_constants.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern int       rlc_am_send_status (struct rlc_am_entity *rlcP);
extern void     rlc_am_process_status_info (struct rlc_am_entity *rlcP, u8_t * statusP);
extern mem_block_t *rlc_am_create_status_pdu_mrw_ack (struct rlc_am_entity *rlcP, u8_t nP, u16_t sn_ackP);
#    endif
