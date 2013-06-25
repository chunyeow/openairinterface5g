/***************************************************************************
                          rrc_rg_data.c  -  description
                             -------------------
    begin                : May 29, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    author               : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  This file contains the definition of the functions triggering the Esterel FSM
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_rg_vars.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_fsm.h"
#include "rrc_proto_int.h"
#include "rrc_proto_intf.h"
//-----------------------------------------------------------------------------
// #include "mac_rg_proto_extern.h"
// #ifndef USER_MODE
// #ifndef BYPASS_L1
// #include "daq.h"
// #include "L1L_extern.h"
// #endif
// #endif

//-----------------------------------------------------------------------------
void rrc_mt_list_init(void){
//-----------------------------------------------------------------------------
  int i,j;

  for (i=0; i<maxUsers ; i++){
    protocol_bs->rrc.Mobile_List[i].mt_id= 999;
    protocol_bs->rrc.Mobile_List[i].state= RRC_CELL_IDLE;
    strcpy (protocol_bs->rrc.Mobile_List[i].IMEI, "01231234564569");
    protocol_bs->rrc.Mobile_List[i].establishment_cause = 0;
    protocol_bs->rrc.Mobile_List[i].release_cause = 0;
    protocol_bs->rrc.Mobile_List[i].prot_error_indicator = 0;
//    strcpy (protocol_bs->rrc.Mobile_List[i].qos_classes, "987987654654987654987654321");
//    strcpy (protocol_bs->rrc.Mobile_List[i].dscp_codes, "321321987987321321987987654");
    for (j=4;j<maxRB;j++)
        protocol_bs->rrc.Mobile_List[i].rg_established_rbs[j].rb_identity = 99;
  }
  // Initialize other stuff in control block
  protocol_bs->rrc.rrc_currently_updating = FALSE;
// Test - Second MT
//  protocol_bs->rrc.Mobile_List[0].mt_id= 0;
//
#ifdef DEBUG_RRC_STATE
  msg("[RRC_RG][FSM-IN] Mobile List initialized \n\n");
#endif
}


//-----------------------------------------------------------------------------
void rrc_mt_list_print(int UE_Id){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_STATE
   int j;
   msg("\n\n[RRC_RG][FSM-IN] Mobile List print for mobile %d at time %d\n", UE_Id, protocol_bs->rrc.current_SFN);
   msg("[RRC_RG][FSM-IN] Id %d, \t State %d, \t IMEI ,\n", //\t IMEI %s,
   			protocol_bs->rrc.Mobile_List[UE_Id].mt_id,
    		protocol_bs->rrc.Mobile_List[UE_Id].state);
//        protocol_bs->rrc.Mobile_List[UE_Id].IMEI);
   rrc_print_buffer(protocol_bs->rrc.Mobile_List[UE_Id].IMEI,14);
   msg("[RRC_RG][FSM-IN] conn_ref %d ,\t est_cause %d,\t rel_cause %d,\t prot_error_indicator %d.\n",
   			protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref,
   			protocol_bs->rrc.Mobile_List[UE_Id].establishment_cause,
   			protocol_bs->rrc.Mobile_List[UE_Id].release_cause,
        protocol_bs->rrc.Mobile_List[UE_Id].prot_error_indicator);
   msg("[RRC_RG][FSM-IN] %d Established Radio Bearers, Ids: ",protocol_bs->rrc.Mobile_List[UE_Id].num_rb);
   for (j=4;j<maxRB;j++){
     if (protocol_bs->rrc.Mobile_List[UE_Id].rg_established_rbs[j].rb_started == RB_STARTED)
        msg(" %d ", j );
   }
   msg("\n");
#endif
}

//-----------------------------------------------------------------------------
int get_new_UE_Id(IMEI* pIMEI){
//-----------------------------------------------------------------------------
  // Must check IMEI as well
  int UE_Id;
  int i, found=FAILURE;
  i=0;
//  while ((strcmp(pIMEI->digit,protocol_bs->rrc.Mobile_List[i].IMEI))&&(i<maxUsers))
  while ((memcmp((char *)pIMEI->digit,(char *)protocol_bs->rrc.Mobile_List[i].IMEI,14))&&(i<maxUsers))
    i++;
  if (i>=maxUsers){
      i=0;
      while ((protocol_bs->rrc.Mobile_List[i].mt_id!= 999)&&(found==FAILURE)&&(i<maxUsers))
        i++;
  }
  if (i<maxUsers){
    UE_Id = i;
    protocol_bs->rrc.Mobile_List[i].mt_id=i;
    protocol_bs->rrc.Mobile_List[i].u_rnti=i;
    #ifdef DEBUG_RRC_STATE
    msg("\n[RRC_RG] New mobile is accepted, with Id: %d \n",UE_Id);
    #endif
  }else{
    UE_Id = 999;
    #ifdef DEBUG_RRC_STATE
    msg("\n[RRC_RG] Radio Gateway overload, no more mobile can be accepted. \n");
    #endif
  }
// End test
  return UE_Id;
}


/* Initialization
 */
//-----------------------------------------------------------------------------
void rrc_rg_fsm_init(void){
//-----------------------------------------------------------------------------
     // execute reset
     RRC_RG_reset();
     RRC_RG();
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Radio Gateway initialized \n\n");
#endif
}

/* Input trigger functions - to be called by RRC_RG_fsm_control() only */
//NAS Events
//-----------------------------------------------------------------------------
void rrc_NAS_Conn_Cnf_Rx (int UE_Id, int status){
//-----------------------------------------------------------------------------
     // input signal NAS_CONN_ESTABLISHMENT_CNF
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Conn_Confirm message from NAS for mobile %d, \n \t\t with status %d \n",
          UE_Id, status);
#endif
     RRC_RG_I_I_NAS_CONN_ESTAB_CNF(UE_Id);
     RRC_RG_I_I_Status (status);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_NAS_Conn_Cnf_RB_Rx (int UE_Id, int status, int RB_List){
//-----------------------------------------------------------------------------
     // input signal NAS_CONN_CNF
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Conn_Confirm message from NAS for mobile %d, \n \t\t with status %d \t\t RB_list %d \n",
          UE_Id, status, RB_List);
#endif
     RRC_RG_I_I_NAS_CONN_ESTAB_CNF(UE_Id);
     RRC_RG_I_I_Status (status);
     RRC_RG_I_I_RB_List (RB_List);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_NAS_Conn_Rel_Req_Rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal NAS_CONN_RELEASE_REQ
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Conn_Release message from NAS for mobile %d \n", UE_Id);
#endif
     RRC_RG_I_I_NAS_CONN_RELEASE_REQ(UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_NAS_RB_Estab_Rx (int UE_Id, int RB_List){
//-----------------------------------------------------------------------------
     // input signal NAS_RB_ESTABLISHMENT_REQ
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Rb_establishment message from NAS for mobile %d, \n \t\t RB_list %d \n",
                 UE_Id, RB_List);
#endif
     RRC_RG_I_I_NAS_RB_ESTAB_REQ (UE_Id);
     RRC_RG_I_I_RB_List (RB_List);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_NAS_RB_Rel_Rx (int UE_Id, int RB_List){
//-----------------------------------------------------------------------------
     // send input signal NAS_RB_RELEASE_REQ
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Rb_Release message from NAS for mobile %d, \n \t\t RB_list %d \n",
                 UE_Id, RB_List);
#endif
     RRC_RG_I_I_NAS_RB_RELEASE_REQ (UE_Id);
     RRC_RG_I_I_RB_List (RB_List);
     RRC_RG();
}
// Messages received from UE
//-----------------------------------------------------------------------------
void rrc_UE_RRC_Conn_Req_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RRC_Conn_Request
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Connection Request Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RRC_CONNECTION_REQUEST(UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_UE_RRC_Conn_Setup_Compl_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RRC_Conn_Setup_Complete
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Connection Setup Complete Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RRC_CONN_SETUP_COMPLETE (UE_Id);
     RRC_RG();
     rrm_add_user_confirm(UE_Id);
}
//-----------------------------------------------------------------------------
void rrc_UE_RRC_Conn_Release_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RRC_Conn_Release
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Connection Release Request Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RRC_CONNECTION_RELEASE(UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_UE_RRC_Conn_Rel_Complete_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RRC_Conn_Rel_Complete
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Connection Release Complete Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RRC_CONN_REL_COMPLETE (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_UE_RB_Setup_Complete_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RB_SETUP_COMPLETE
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] RB Setup Complete Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RB_SETUP_COMPLETE (UE_Id);
     RRC_RG();
//      rrm_add_radio_access_bearer_confirm(UE_Id);
}
//-----------------------------------------------------------------------------
void rrc_UE_RB_Setup_Failure_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RB_SETUP_FAILURE
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] RB Setup Failure Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RB_SETUP_FAILURE (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_UE_RB_Release_Complete_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RB_RELEASE_COMPLETE
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] RB Release Complete Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RB_REL_COMPLETE (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_UE_Last_RB_Release_Complete_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RB_RELEASE_COMPLETE
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] RB Release Complete Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RB_REL_COMPLETE (UE_Id);
     RRC_RG_I_I_Last_RB();
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_UE_RB_Release_Failure_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal RB_RELEASE_FAILURE
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] RB Release Failure Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RB_REL_FAILURE (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_UE_Cell_Update_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // input signal CELL_UPDATE
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Cell Update Received from UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CELL_UPDATE (UE_Id);
     RRC_RG();
}


//-----------------------------------------------------------------------------
void rrc_RG_PHY_Synch_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate PHY_Synch received (DCH established)
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] PHY_Synch_Success Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CPHY_SYNCH_IND (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_RG_PHY_Synch_Failure_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate PHY_Failure received (DCH failed to established)
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] PHY_Synch_Failure Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CPHY_SYNCH_Failure (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_RG_PHY_Out_Synch_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate PHY_Synch_Failure received on a DCH
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] PHY_Out_Synch Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CPHY_OUT_OF_SYNCH (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_RG_PHY_Connection_Loss_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate PHY_Connection_Loss received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] PHY_Connection_Loss Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CPHY_CONNECTION_LOSS (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_RG_PHY_Setup_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate PHY_Setup_cnf received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] PHY_Setup_cnf Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CPHY_RL_SETUP_Cnf (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_RG_PHY_Modify_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate PHY_Modify_cnf received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] PHY_Modify_cnf Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CPHY_RL_MODIFY_Cnf (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_RG_RLC_Status_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate CRLC_Status_ind received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] CRLC_Status_ind Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_CRLC_STATUS (UE_Id);
     RRC_RG();
}

//-----------------------------------------------------------------------------
void rrc_RG_RLC_Data_Confirm_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate RLC_Data_Confirm received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] RLC_Data_Confirm Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RLC_Success (UE_Id);
     RRC_RG();
}
//-----------------------------------------------------------------------------
void rrc_RG_RLC_Data_failure_rx (int UE_Id){
//-----------------------------------------------------------------------------
     // simulate RLC_Data_Failure received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] RLC_Data_Failure Received for UE with Id %d.\n",UE_Id);
#endif
     RRC_RG_I_I_RLC_Failure (UE_Id);
     RRC_RG();
}

//-----------------------------------------------------------------------------
void rrc_RG_Configuration_rx (void){
//-----------------------------------------------------------------------------
     // Configuration Indication received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Configuration Indication Received from RRM.\n");
#endif
     RRC_RG_I_I_Configuration_Indication();
     RRC_RG();
}

//-----------------------------------------------------------------------------
void rrc_RG_Config_failure_rx (void){
//-----------------------------------------------------------------------------
     // Configuration Failure received
#ifdef DEBUG_RRC_STATE
     msg("[RRC_RG][FSM-IN] Configuration Failure Received from RRM.\n");
#endif
     RRC_RG_I_I_Configuration_Failure();
     RRC_RG();
}




