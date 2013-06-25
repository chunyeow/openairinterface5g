/***************************************************************************
                          rlc_am_fsm_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_FSM_PROTO_EXTERN_H__
#        define __RLC_AM_FSM_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "platform_types.h"
#        include "rlc_am_entity.h"
//-----------------------------------------------------------------------------
extern int      rlc_am_fsm_notify_event (struct rlc_am_entity *rlcP, u8_t eventP);
#    endif
