/***************************************************************************
                          rlc_am_discard_tx_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_DISCARD_TX_PROTO_EXTERN_H__
#        define __RLC_AM_DISCARD_TX_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "rlc_am_entity.h"
#        include "rlc_am_structs.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     rlc_am_schedule_procedure (struct rlc_am_entity *rlcP);
extern void     rlc_am_process_sdu_discarded (struct rlc_am_entity *rlcP);
extern void     rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_configured (struct rlc_am_entity *rlcP);
extern void     rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_not_configured (struct rlc_am_entity *rlcP);
#    endif
