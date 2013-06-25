/***************************************************************************
                          rlc_am_fsm.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_entity.h"
#include "rlc_am_constants.h"
#include "rlc_def.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
int
rlc_am_fsm_notify_event (struct rlc_am_entity *rlcP, u8_t eventP)
{
//-----------------------------------------------------------------------------

  switch (rlcP->protocol_state) {
        //-------------------------------
        // RLC_NULL_STATE
        //-------------------------------
      case RLC_NULL_STATE:
        switch (eventP) {
            case RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_NULL_STATE -> RLC_DATA_TRANSFER_READY_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
              return 1;
              break;

            default:
              msg ("[RLC_AM][RB %d][FSM] WARNING PROTOCOL ERROR - EVENT %02X hex NOT EXPECTED FROM NULL_STATE frame %d\n", rlcP->rb_id, eventP, Mac_rlc_xface->frame);
              return 0;
        }
        break;
        //-------------------------------
        // RLC_DATA_TRANSFER_READY_STATE
        //-------------------------------
      case RLC_DATA_TRANSFER_READY_STATE:
        switch (eventP) {
            case RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_DATA_TRANSFER_READY_STATE -> RLC_NULL_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_NULL_STATE;
              return 1;
              break;

            case RLC_AM_RECEIVE_RESET_EVENT:
            case RLC_AM_TRANSMIT_RESET_ACK_EVENT:
              return 1;
              break;

            case RLC_AM_TRANSMIT_RESET_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_DATA_TRANSFER_READY_STATE -> RLC_RESET_PENDING_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_RESET_PENDING_STATE;
              return 1;
              break;

            case RLC_AM_RECEIVE_CRLC_SUSPEND_REQ_EVENT:
            case RLC_AM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_DATA_TRANSFER_READY_STATE -> RLC_LOCAL_SUSPEND_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_LOCAL_SUSPEND_STATE;
              return 1;
              break;

            default:
              msg ("[RLC_AM][RB %d][FSM] WARNING PROTOCOL ERROR - EVENT 0x%02X NOT EXPECTED FROM DATA_TRANSFER_READY_STATE frame %d\n", rlcP->rb_id, eventP, Mac_rlc_xface->frame);
              return 0;
        }
        break;
        //-------------------------------
        // RLC_RESET_PENDING_STATE
        //-------------------------------
      case RLC_RESET_PENDING_STATE:
        switch (eventP) {
            case RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_RESET_PENDING_STATE -> RLC_NULL_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_NULL_STATE;
              return 1;
              break;

            case RLC_AM_RECEIVE_RESET_EVENT:
            case RLC_AM_TRANSMIT_RESET_ACK_EVENT:
            case RLC_AM_TRANSMIT_RESET_EVENT:  // WARNING: THIS EVENT IS NOT IN SPECS BUT MAY BE AN OMISSION ????
              return 1;
              break;

            case RLC_AM_RECEIVE_RESET_ACK_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_RESET_PENDING_STATE -> RLC_DATA_TRANSFER_READY_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
              return 1;
              break;

            case RLC_AM_RECEIVE_CRLC_SUSPEND_REQ_EVENT:
            case RLC_AM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_RESET_PENDING_STATE -> RLC_RESET_AND_SUSPEND_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_RESET_AND_SUSPEND_STATE;
              return 1;
              break;

            default:
              msg ("[RLC_AM][RB %d][FSM] WARNING PROTOCOL ERROR - EVENT 0x%02X NOT EXPECTED FROM RLC_RESET_PENDING_STATE frame %d\n", rlcP->rb_id, eventP, Mac_rlc_xface->frame);
              return 0;
        }
        break;
        //-------------------------------
        // RLC_RESET_AND_SUSPEND_STATE
        //-------------------------------
      case RLC_RESET_AND_SUSPEND_STATE:
        switch (eventP) {
            case RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_RESET_AND_SUSPEND_STATE -> RLC_NULL_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_NULL_STATE;
              return 1;
              break;

            case RLC_AM_RECEIVE_RESET_ACK_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_RESET_AND_SUSPEND_STATE -> RLC_LOCAL_SUSPEND_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_LOCAL_SUSPEND_STATE;
              return 1;
              break;

            case RLC_AM_RECEIVE_CRLC_RESUME_REQ_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_RESET_AND_SUSPEND_STATE -> RLC_RESET_PENDING_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_RESET_PENDING_STATE;
              return 1;
              break;

            default:
              msg ("[RLC_AM][RB %d][FSM] WARNING PROTOCOL ERROR - EVENT 0x%02X NOT EXPECTED FROM RLC_RESET_AND_SUSPEND_STATE frame %d\n", rlcP->rb_id, eventP, Mac_rlc_xface->frame);
              return 0;
        }
        break;
        //-------------------------------
        // RLC_LOCAL_SUSPEND_STATE
        //-------------------------------
      case RLC_LOCAL_SUSPEND_STATE:
        switch (eventP) {
            case RLC_AM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_LOCAL_SUSPEND_STATE -> RLC_NULL_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_NULL_STATE;
              return 1;
              break;

            case RLC_AM_RECEIVE_CRLC_RESUME_REQ_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_LOCAL_SUSPEND_STATE -> RLC_DATA_TRANSFER_READY_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_DATA_TRANSFER_READY_STATE;
              return 1;
              break;

            case RLC_AM_TRANSMIT_RESET_EVENT:
#ifdef DEBUG_RLC_AM_FSM
              msg ("[RLC_AM][RB %d][FSM] RLC_LOCAL_SUSPEND_STATE -> RLC_RESET_AND_SUSPEND_STATE frame %d\n", rlcP->rb_id, Mac_rlc_xface->frame);
#endif
              rlcP->protocol_state = RLC_RESET_AND_SUSPEND_STATE;
              return 1;
              break;

            default:
              msg ("[RLC_AM][RB %d][FSM] WARNING PROTOCOL ERROR - EVENT 0x%02X NOT EXPECTED FROM RLC_LOCAL_SUSPEND_STATE frame %d\n", rlcP->rb_id, eventP, Mac_rlc_xface->frame);
              return 0;
        }
        break;

      default:
        msg ("[RLC_AM][RB %d][FSM] ERROR UNKNOWN STATE %d\n", rlcP->rb_id, rlcP->protocol_state);
        return 0;
  }
}
