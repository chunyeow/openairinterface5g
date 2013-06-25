/***************************************************************************
                          rrc_ue_data.c  -  description
                             -------------------
    begin                : Nov 10, 2001
    copyright            : (C) 2001, 2010 by Eurecom
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
#include "rrc_messages.h"
#include "as_configuration.h"
//-------------------------------------------------------------------
#include "rrc_proto_fsm.h"
#include "rrc_proto_int.h"

// #include "mac_ue_proto_extern.h"
// #ifndef USER_MODE
// #ifndef BYPASS_L1
// #include "daq.h"
// #include "L1L_extern.h"
//   #endif
//   #endif

/* Initialization
	input parm: User Equipment Id*/
//-------------------------------------------------------------------
void rrc_ue_fsm_init (int Id){
//-------------------------------------------------------------------
  // execute reset
  RRC_UE_reset ();
  RRC_UE ();
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM] UE_ID_INIT initialized to %d \n", Id);
  #endif
  // First init tick
  RRC_UE_I_UE_ID_INIT (Id);
  RRC_UE ();
}

/* Input trigger functions - to be called by rrc_ue_fsm_control() only */

//-------------------------------------------------------------------
void rrc_NAS_Conn_Est_Req_Rx (void){
//-------------------------------------------------------------------
  // send input signal NAS_CONN_ESTABLISHMENT_REQ
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Conn_establishment_req message from NAS \n");
  #endif
  RRC_UE_I_I_NAS_CONN_ESTABLISHMENT_REQ ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void rrc_NAS_Conn_Release_Req_Rx (void){
//-------------------------------------------------------------------
  // send input signal NAS_CONN_RELEASE_REQ
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Conn_release_req message from NAS \n");
  #endif
  RRC_UE_I_I_NAS_CONN_RELEASE_REQ ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void rrc_RRC_Conn_Setup_rx (int Id, int State){
//-------------------------------------------------------------------
  // send input signal RRC_Conn_Setup
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Connection Setup Received, Id %d, state %d.\n", Id, State);
  #endif
  RRC_UE_I_UE_ID_MSG (Id);
  RRC_UE_I_I_IE_RRC_State (State);
  RRC_UE_I_I_RRC_CONNECTION_SETUP ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void rrc_RRC_Conn_Reject_rx (int Id, int WaitTime){
//-------------------------------------------------------------------
  // send input signal RRC_Conn_Reject
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Connection Reject Received, Id %d, wait time %d.\n", Id, WaitTime);
  #endif
  RRC_UE_I_UE_ID_MSG (Id);
  RRC_UE_I_I_Wait_Time (WaitTime);
  RRC_UE_I_I_RRC_CONNECTION_REJECT ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void rrc_RRC_Conn_Reject_Freq_rx (int Id, int WaitTime){
//-------------------------------------------------------------------
  // send input signal RRC_Conn_Reject
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Connection Reject Received w/ IE_Freq_Info, Id %d, wait time %d.\n", Id, WaitTime);
  #endif
  RRC_UE_I_UE_ID_MSG (Id);
  RRC_UE_I_I_Wait_Time (WaitTime);
  RRC_UE_I_I_IE_Freq_Info ();
  RRC_UE_I_I_RRC_CONNECTION_REJECT ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void rrc_RRC_Conn_Release_DCCH_rx (void){
//-------------------------------------------------------------------
  // RRC_CONN_RELEASE has been received on DCCH
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Connection Release received on DCCH.\n");
  #endif
  RRC_UE_I_I_RRC_CONNECTION_RELEASE ();
  RRC_UE_I_I_rcved_on_DCCH ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void rrc_RRC_Conn_Release_CCCH_rx (void){
//-------------------------------------------------------------------
  // RRC_CONN_RELEASE has been received on CCCH
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Connection Release received on CCCH.\n");
  #endif
  RRC_UE_I_I_RRC_CONNECTION_RELEASE ();
  RRC_UE_I_I_rcved_on_CCCH ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void PHY_Setup_rx (void){
//-------------------------------------------------------------------
  // simulate PHY_Synch received (DCH established)
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] PHY_Synch_Success Received \n");
  #endif
  RRC_UE_I_I_CPHY_SYNCH_IND ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void PHY_Synch_Failure_rx (void){
//-------------------------------------------------------------------
  // simulate PHY_Failure received (DCH failed to established)
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] PHY_Synch_Failure Received \n");
  #endif
  RRC_UE_I_I_CPHY_SYNCH_Failure ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void PHY_Connection_Loss_rx (void){
//-------------------------------------------------------------------
  // simulate PHY_Connection_Loss received (DCH failed to established)
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] PHY_Connection_Loss Received \n");
  #endif
  RRC_UE_I_I_CPHY_CONNECTION_LOSS ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void RLC_Data_Confirm_rx (void){
//-------------------------------------------------------------------
  // RLC_Data_Confirm received
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] RLC_Data_Confirm Received\n");
  #endif
  RRC_UE_I_I_RLC_Success ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void RLC_failure_rx (void){
//-------------------------------------------------------------------
  // RLC_Data_Failure received
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] RLC_Data_Failure Received\n");
  #endif
  RRC_UE_I_I_RLC_Failure ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void TIMER_T300_Timeout (void){
//-------------------------------------------------------------------
  // Notify T300 Time-out
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Timer T300 has expired.\n");
  #endif
  RRC_UE_I_I_T300_TimeOut ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void TIMER_T308_Timeout (void){
//-------------------------------------------------------------------
  // Notify T308 Time-out
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] Timer T308 has expired.\n");
  #endif
  RRC_UE_I_I_T308_TimeOut ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void Wait_Timer_Timeout (void){
//-------------------------------------------------------------------
  // Notify T308 Time-out
  #ifdef DEBUG_RRC_STATE
   msg ("[RRC][FSM-IN] CONN REJECT wait timer has expired.\n");
  #endif
  RRC_UE_I_I_WaitTimerExpired ();
  RRC_UE ();
}
