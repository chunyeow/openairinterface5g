/***************************************************************************
                          rrc_ue_esterfsm.c  -  description
                             -------------------
    begin                : June 21, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    author               : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  This file contains the function that prepares and controls the UE FSM
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
//#include "mac_ue_proto_extern.h"

// for FSM
int started = 0;

//-----------------------------------------------------------------------------
void rrc_ue_fsm_control (int rrc_event){
//-----------------------------------------------------------------------------
  //#ifdef USER_MODE
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM] RX EVENT %d\n", rrc_event);
  #endif
  //#endif
  switch (rrc_event) {
    case UE_PHY_SETUP:
      //PHY_Setup_rx();
      break;
    case NAS_CONN_REQ:
      rrc_NAS_Conn_Est_Req_Rx ();
      break;
    case RRC_CONNECT_SETUP:
      rrc_RRC_Conn_Setup_rx (protocol_ms->rrc.ue_initial_id, protocol_ms->rrc.next_state);
      PHY_Setup_rx ();
      break;
    case RRC_CONNECT_REJECT:
      rrc_RRC_Conn_Reject_rx (protocol_ms->rrc.ue_initial_id, protocol_ms->rrc.rejectWaitTime);
      break;
    case UE_RB_SETUP:
      ++protocol_ms->rrc.num_rb;  //Temp
      #ifdef DEBUG_RRC_STATE
       msg ("[RRC][FSM] UE_RB_SETUP, num_rb = %d\n", protocol_ms->rrc.num_rb);
      #endif
      rrc_rb_ue_RB_Setup_rx (protocol_ms->rrc.requested_rbId, protocol_ms->rrc.next_state, FALSE);
      rrc_rb_ue_PHY_Setup_rx ();
      break;
    case UE_RB_RELEASE:
      rrc_rb_ue_RB_Release_rx (protocol_ms->rrc.requested_rbId, protocol_ms->rrc.next_state, FALSE);
      break;
    case CELLU_CNF:
      rrc_rb_ue_Cell_Update_cnf_rx ();
      break;
    case T300_TO:
      TIMER_T300_Timeout ();
      break;
    case RRC_RLC_SUCCESS:
      switch (protocol_ms->rrc.last_message_sent) {
        case RRC_CONN_REQ:
        case RRC_CONN_SETUP_COMPLETE:
        case RRC_CONN_RELEASE:
          RLC_Data_Confirm_rx ();
          break;
        case RB_SETUP_COMPLETE:
        case RB_SETUP_FAILURE:
        case RB_RELEASE_COMPLETE:
        case RB_RELEASE_FAILURE:
        case CELL_UPDATE:
          rrc_rb_ue_RLC_Data_Confirm_rx ();
          break;
        default:
          //nothing to do
          break;
      }
      protocol_ms->rrc.last_message_sent = 0;
      break;
    case RRC_RLC_FAILURE:
      RLC_failure_rx ();
      break;
    case NAS_REL_REQ:
      rrc_NAS_Conn_Release_Req_Rx ();
      break;
    case UE_CRLC_STATUS:
      rrc_rb_ue_CRLC_Status_rx ();
      break;
    case UE_CONN_LOSS:
      PHY_Connection_Loss_rx ();
      break;
    case UE_CAP_INFO_CNF:
      //Do nothing.
      break;
    default:
      msg ("[RRC][FSM] Invalid call to RRC FSM control %d \n", rrc_event);
      break;
  }
  if (rrc_event == NAS_CONN_REQ) {
    started = 1;
  }
}
