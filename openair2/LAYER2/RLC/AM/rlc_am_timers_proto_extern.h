/***************************************************************************
                          rlc_am_timers_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#    ifndef __RLC_AM_TIMERS_H__
#        define __RLC_AM_RIMERS_H__
#        include "rlc_am_entity.h"
#        include "platform_types.h"

extern void     rlc_am_timer_epc_notify_time_out (struct rlc_am_entity *rlcP, void *arg_not_usedP);
extern void     rlc_am_timer_epc_fsm (struct rlc_am_entity *rlcP, u8_t eventP);
#    endif
