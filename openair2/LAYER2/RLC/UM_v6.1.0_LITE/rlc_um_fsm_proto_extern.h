/***************************************************************************
                          rlc_um_fsm_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_UM_FSM_PROTO_EXTERN_H__
#        define __RLC_UM_FSM_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "platform_types.h"
#        include "rlc_um_entity.h"
//-----------------------------------------------------------------------------
extern int      rlc_um_fsm_notify_event (struct rlc_um_entity *rlcP, u8_t eventP);
#    endif
