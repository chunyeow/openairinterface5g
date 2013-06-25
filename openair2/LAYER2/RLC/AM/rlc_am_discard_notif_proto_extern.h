/***************************************************************************
                          rlc_am_discard_notif_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_DISCARD_NOTIF_PROTO_EXTERN_H__
#        define __RLC_AM_DISCARD_NOTIF_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "rlc_am_constants.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     rlc_am_discard_notify_mrw_ack_time_out (struct rlc_am_entity *rlcP, mem_block_t * discard_procedureP);
extern void     rlc_am_discard_check_sdu_time_out (struct rlc_am_entity *rlcP);
extern void     rlc_am_discard_notify_max_dat_pdu (struct rlc_am_entity *rlcP, mem_block_t * pduP);
#    endif
