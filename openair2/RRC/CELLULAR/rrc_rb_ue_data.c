/***************************************************************************
                          rrc_rb_ue_data.c  -  description
                             -------------------
    begin                : April 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    author               : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  This file contains the definition of the functions called by Esterel FSM
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
//-----------------------------------------------------------------------------
#include "rrc_proto_fsm.h"

//-----------------------------------------------------------------------------

/* Initialization
	input parm: User Equipment Id*/
void rrc_rb_ue_init (void){
  // execute reset
  RRC_RB_UE_reset ();
  RRC_RB_UE ();
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM] FSM initialized\n");
  msg ("[RRC_RB][FSM]\n States : IDLE = 1, CELL_FACH = 6, CELL_DCH = 10 \n");
  msg ("[RRC_RB][FSM]\n Boolean : true = 1, false = 0\n");
#endif
  // First tick
  RRC_RB_UE ();
}

/* Input trigger functions - to be called by RRC_RB_UE_fsm_control() only */

void rrc_rb_ue_new_state (int newState){
  // force state transition in the RRC_RB_UE fsm
  switch (newState) {
      case CELL_FACH:
        RRC_RB_UE_I_I_Go_CELL_FACH ();
        break;
      case CELL_DCH:
        RRC_RB_UE_I_I_Go_CELL_DCH ();
        break;
      case RRC_CELL_IDLE:
        RRC_RB_UE_I_I_Go_Idle ();
        break;
      default:
#ifdef DEBUG_RRC_STATE
        msg ("[RRC_RB][FSM] Invalid State for transition\n");
#endif
        break;
  }
  RRC_RB_UE ();
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM] Transition to state %d\n", newState);
#endif
}

void rrc_rb_ue_RB_Setup_rx (int rb_id, int next_state, int prot_error){
  // send input signal RRC_Conn_Setup
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] RB Setup Received, rb_id %d , next_state %d ,  protocol error %d.\n", rb_id, next_state, prot_error);
#endif
  RRC_RB_UE_I_I_Radio_Bearer_ID (rb_id);
  RRC_RB_UE_I_I_IE_RRC_State (next_state);
  RRC_RB_UE_I_I_Prot_Error (prot_error);
  RRC_RB_UE_I_I_RADIO_BEARER_SETUP ();
  RRC_RB_UE ();
}

void rrc_rb_ue_RB_Release_rx (int rb_id, int next_state, int prot_error){
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] RB Release Received, rb_id %d , next_state %d , protocol error %d.\n", rb_id, next_state, prot_error);
#endif
  RRC_RB_UE_I_I_Radio_Bearer_ID (rb_id);
  RRC_RB_UE_I_I_IE_RRC_State (next_state);
  RRC_RB_UE_I_I_Prot_Error (prot_error);
  RRC_RB_UE_I_I_RADIO_BEARER_RELEASE ();
  RRC_RB_UE ();
}

void rrc_rb_ue_Cell_Update_cnf_rx (void){
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] Cell Update Confirm Received.\n");
#endif
  RRC_RB_UE_I_I_CELL_UPDATE_CONFIRM ();
  RRC_RB_UE ();
}

void rrc_rb_ue_PHY_Setup_rx (void){
  // simulate PHY_Synch received (DCH established)
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] PHY_Synch_Success Received \n");
#endif
  RRC_RB_UE_I_I_CPHY_SYNCH_IND ();
  RRC_RB_UE ();
}

void rrc_rb_ue_CPHY_Synch_Failure_rx (void){
  // simulate PHY_Failure received (DCH failed to established)
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] PHY_Synch_Failure Received \n");
#endif
  RRC_RB_UE_I_I_CPHY_SYNCH_Failure ();
  RRC_RB_UE ();
}

void rrc_rb_ue_CPHY_Out_of_Synch_rx (void){
  // simulate Out of Synch received (DCH failure)
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] PHY_Out_Of_Synch Received \n");
#endif
  RRC_RB_UE_I_I_CPHY_OUT_OF_SYNCH ();
  RRC_RB_UE ();
}

void rrc_rb_ue_CRLC_Status_rx (void){
  // simulate RLC_Status_Ind received (unrecoverable errors on RLC)
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] RLC_Status_Ind Received \n");
#endif
  RRC_RB_UE_I_I_CRLC_STATUS ();
  RRC_RB_UE ();
}

void rrc_rb_ue_RLC_Data_Confirm_rx (void){
  // simulate (to be removed?) RLC_Data_Confirm received
#ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-IN] RLC_Data_Confirm Received\n");
#endif
  RRC_RB_UE_I_I_RLC_Success ();
  RRC_RB_UE ();
}

/*void rrc_rb_ue_RLC_failure_rx (void)
{
     // simulate (to be removed?) RLC_Data_Confirm received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RB][FSM-IN] RLC_Data_Failure Received\n");
#endif
     RRC_RB_UE_I_I_RLC_Failure ();
     RRC_RB_UE();
}*/
