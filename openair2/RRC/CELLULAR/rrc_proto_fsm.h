/***************************************************************************
                          rrc_proto_fsm.h  -
                          -------------------
    copyright            : (C) 2001, 2010 by Eurecom
    created by           : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  prototypes for RRC FSM
 ***************************************************************************/

#ifndef __RRC_PROTO_FSM_H__
#define __RRC_PROTO_FSM_H__

// #include "rrc_constant.h"
// #include "rrc_msg_ies.h"

#ifdef NODE_RG
void rrc_rg_fsm_control (int UE_Id, int rrc_event);
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
//int rrc_ue_fsm_control (int eventP);
void rrc_ue_fsm_control (int rrc_event);
#endif

/***************************************************************************/
#ifdef NODE_RG

void  rrc_mt_list_print (int UE_Id);
void  rrc_mt_list_init (void);
int   get_new_UE_Id (IMEI *pIMEI);

/* Input Functions prototypes*/

extern void RRC_RG_I_I_RRC_CONNECTION_REQUEST (int UE_Id);
extern void RRC_RG_I_I_RRC_CONN_SETUP_COMPLETE (int UE_Id);
extern void RRC_RG_I_I_RRC_CONNECTION_RELEASE (int UE_Id);
extern void RRC_RG_I_I_RRC_CONN_REL_COMPLETE (int UE_Id);
extern void RRC_RG_I_I_RB_SETUP_COMPLETE (int UE_Id);
extern void RRC_RG_I_I_RB_SETUP_FAILURE (int UE_Id);
extern void RRC_RG_I_I_RB_REL_COMPLETE (int UE_Id);
extern void RRC_RG_I_I_RB_REL_FAILURE (int UE_Id);
extern void RRC_RG_I_I_CELL_UPDATE (int UE_Id);
extern void RRC_RG_I_I_NAS_CONN_ESTAB_CNF (int UE_Id);
extern void RRC_RG_I_I_NAS_CONN_RELEASE_REQ (int UE_Id);
extern void RRC_RG_I_I_NAS_RB_ESTAB_REQ (int UE_Id);
extern void RRC_RG_I_I_NAS_RB_RELEASE_REQ (int UE_Id);
extern void RRC_RG_I_I_CPHY_SYNCH_IND (int UE_Id);
extern void RRC_RG_I_I_CPHY_SYNCH_Failure (int UE_Id);
extern void RRC_RG_I_I_CPHY_OUT_OF_SYNCH (int UE_Id);
extern void RRC_RG_I_I_CPHY_CONNECTION_LOSS (int UE_Id);
extern void RRC_RG_I_I_CPHY_RL_SETUP_Cnf (int UE_Id);
extern void RRC_RG_I_I_CPHY_RL_MODIFY_Cnf (int UE_Id);
extern void RRC_RG_I_I_CRLC_STATUS (int UE_Id);
extern void RRC_RG_I_I_RLC_Success (int UE_Id);
extern void RRC_RG_I_I_RLC_Failure (int UE_Id);
extern void RRC_RG_I_I_Status (int UE_Id);
extern void RRC_RG_I_I_RB_List (int UE_Id);
extern void RRC_RG_I_I_Last_RB (void);
extern void RRC_RG_I_I_Configuration_Indication (void);
extern void RRC_RG_I_I_Configuration_Failure(void);
extern int  RRC_RG_reset (void);
extern int  RRC_RG (void);

/* Input trigger functions - Defined in rrc_rg_data.c
 */
void rrc_rg_fsm_init (void);

void rrc_NAS_Conn_Cnf_Rx (int UE_Id, int status);
void rrc_NAS_Conn_Cnf_RB_Rx (int UE_Id, int status, int RB_List);
void rrc_NAS_Conn_Rel_Req_Rx (int UE_Id);
void rrc_NAS_RB_Estab_Rx (int UE_Id, int RB_List);
void rrc_NAS_RB_Rel_Rx (int UE_Id, int RB_List);

void rrc_UE_RRC_Conn_Req_rx (int UE_Id);
void rrc_UE_RRC_Conn_Setup_Compl_rx (int UE_Id);
void rrc_UE_RRC_Conn_Release_rx (int UE_Id);
void rrc_UE_RRC_Conn_Rel_Complete_rx (int UE_Id);
void rrc_UE_RB_Setup_Complete_rx (int UE_Id);
void rrc_UE_RB_Setup_Failure_rx (int UE_Id);
void rrc_UE_RB_Release_Complete_rx (int UE_Id);
void rrc_UE_Last_RB_Release_Complete_rx (int UE_Id);
void rrc_UE_RB_Release_Failure_rx (int UE_Id);
void rrc_UE_Cell_Update_rx (int UE_Id);

void rrc_RG_PHY_Synch_rx (int UE_Id);
void rrc_RG_PHY_Synch_Failure_rx (int UE_Id);
void rrc_RG_PHY_Out_Synch_rx (int UE_Id);
void rrc_RG_PHY_Connection_Loss_rx (int UE_Id);
void rrc_RG_PHY_Setup_rx (int UE_Id);
void rrc_RG_PHY_Modify_rx (int UE_Id);
void rrc_RG_RLC_Status_rx (int UE_Id);
void rrc_RG_RLC_Data_Confirm_rx (int UE_Id);
void rrc_RG_RLC_Data_failure_rx (int UE_Id);
void rrc_RG_Configuration_rx (void);
void rrc_RG_Config_failure_rx (void);
#endif

//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_ue_get_initial_id (void);
void RRC_UE_O_O_SEND_DCCH_AM (int msgId);
void RRC_UE_O_O_SEND_DCCH_UM (int msgId);

/* External FUNCTIONS Definition*/
 // For Connection Model
extern void     RRC_UE_I_I_WaitTimerExpired (void);
extern void     RRC_UE_I_I_T308_TimeOut (void);
extern void     RRC_UE_I_I_T300_TimeOut (void);
extern void     RRC_UE_I_I_Go_CELL_FACH (void);
extern void     RRC_UE_I_I_Go_CELL_DCH (void);
extern void     RRC_UE_I_I_NAS_CONN_ESTABLISHMENT_REQ (void);
extern void     RRC_UE_I_I_NAS_CONN_RELEASE_REQ (void);
extern void     RRC_UE_I_I_RRC_CONNECTION_RELEASE (void);
extern void     RRC_UE_I_I_RRC_CONNECTION_REJECT (void);
extern void     RRC_UE_I_I_RRC_CONNECTION_SETUP (void);
extern void     RRC_UE_I_I_RLC_Success (void);
extern void     RRC_UE_I_I_RLC_Failure (void);
extern void     RRC_UE_I_I_CPHY_SYNCH_IND (void);
extern void     RRC_UE_I_I_CPHY_SYNCH_Failure (void);
extern void     RRC_UE_I_I_CPHY_CONNECTION_LOSS (void);
extern void     RRC_UE_I_I_rcved_on_CCCH (void);
extern void     RRC_UE_I_I_rcved_on_DCCH (void);
extern void     RRC_UE_I_I_Wait_Time (int);
extern void     RRC_UE_I_I_IE_Freq_Info (void);
extern void     RRC_UE_I_I_IE_RRC_State (int);
extern void     RRC_UE_I_UE_ID_INIT (int);
extern void     RRC_UE_I_UE_ID_MSG (int);
extern void     RRC_UE_reset (void);
extern void     RRC_UE (void);

 // For RB Control Model
extern void     RRC_RB_UE_I_I_RADIO_BEARER_SETUP (void);
extern void     RRC_RB_UE_I_I_RADIO_BEARER_RELEASE (void);
extern void     RRC_RB_UE_I_I_CELL_UPDATE_CONFIRM (void);
extern void     RRC_RB_UE_I_I_Prot_Error (int error);
extern void     RRC_RB_UE_I_I_CPHY_SYNCH_IND (void);
extern void     RRC_RB_UE_I_I_CPHY_SYNCH_Failure (void);
extern void     RRC_RB_UE_I_I_CPHY_OUT_OF_SYNCH (void);
extern void     RRC_RB_UE_I_I_CRLC_STATUS (void);
extern void     RRC_RB_UE_I_I_RLC_Success (void);
extern void     RRC_RB_UE_I_I_Go_Idle (void);
extern void     RRC_RB_UE_I_I_Go_CELL_DCH (void);
extern void     RRC_RB_UE_I_I_Go_CELL_FACH (void);
extern void     RRC_RB_UE_I_I_Go_CELL_PCH (void);
extern void     RRC_RB_UE_I_I_Radio_Bearer_ID (int rb_id);
extern void     RRC_RB_UE_I_I_IE_RRC_State (int next_state);
extern void     RRC_RB_UE_reset (void);
extern void     RRC_RB_UE (void);

/* Input trigger functions - Defined in rrc_ue_data.c */
void  rrc_ue_fsm_init (int Id);
void  rrc_NAS_Conn_Est_Req_Rx (void);
void  rrc_NAS_Conn_Release_Req_Rx (void);
void  rrc_RRC_Conn_Setup_rx (int Id, int State);
void  rrc_RRC_Conn_Reject_rx (int Id, int WaitTime);
void  rrc_RRC_Conn_Reject_Freq_rx (int Id, int WaitTime);
void  rrc_RRC_Conn_Release_DCCH_rx (void);
void  rrc_RRC_Conn_Release_CCCH_rx (void);
void  PHY_Setup_rx (void);
void  PHY_Synch_Failure_rx (void);
void  PHY_Connection_Loss_rx (void);
void  RLC_Data_Confirm_rx (void);
void  RLC_failure_rx (void);
void  TIMER_T300_Timeout (void);
void  TIMER_T308_Timeout (void);
void  Wait_Timer_Timeout (void);

/* Input trigger functions - Defined in rrc_rb_ue_data.c */
void  rrc_rb_ue_init (void);
void  rrc_rb_ue_new_state (int newState);
void  rrc_rb_ue_RB_Setup_rx (int rb_id, int next_state, int prot_error);
void  rrc_rb_ue_RB_Release_rx (int rb_id, int next_state, int prot_error);
void  rrc_rb_ue_Cell_Update_cnf_rx (void);
void  rrc_rb_ue_PHY_Setup_rx (void);
void  rrc_rb_ue_CPHY_Synch_Failure_rx (void);
void  rrc_rb_ue_CPHY_Out_of_Synch_rx (void);
void  rrc_rb_ue_CRLC_Status_rx (void);
void  rrc_rb_ue_RLC_Data_Confirm_rx (void);
#endif

#endif
