/***************************************************************************
                          rrc_rg_esterfsm.c  -  description
                             -------------------
    begin                : June 17, 2002
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
#include "rrc_rg_vars.h"
#include "rrc_messages.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_fsm.h"
//-----------------------------------------------------------------------------
// #include "mac_rg_proto_extern.h"
// #ifndef USER_MODE
// #ifndef BYPASS_L1
// #include "daq.h"
// #include "L1L_extern.h"
// #endif
// #endif

/* This function prepares and controls the FSM  thru the functions defined above */
//-----------------------------------------------------------------------------
void rrc_rg_fsm_control(int UE_Id, int rrc_event){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
   msg("[RRC_RG][FSM] RX EVENT: %d\n",rrc_event);
  #endif
  protocol_bs->rrc.rrc_UE_updating = UE_Id ;
  switch (rrc_event){
    case RRC_CONNECT_REQ :
      rrc_UE_RRC_Conn_Req_rx (UE_Id);
      break;
    case RRC_CONNECT_LOSS :
      rrc_RG_PHY_Connection_Loss_rx (UE_Id);
      break;
    case UE_PHY_SYNCH :
      //rrc_RG_PHY_Synch_rx(UE_Id);
      break;
    case RRC_CONN_SETUP_COMP :
      rrc_UE_RRC_Conn_Setup_Compl_rx (UE_Id);
      rrc_mt_list_print(UE_Id);
      break;
    // case NAS_REL_REQ :
    //   rrc_NAS_Conn_Rel_Req_Rx (UE_Id);
    //   rrc_UE_RRC_Conn_Rel_Complete_rx (UE_Id);
    //   rrc_mt_list_print(UE_Id);
    //   break;
    case RRC_CONNECT_RELUL :
      rrc_UE_RRC_Conn_Release_rx(UE_Id);
      rrc_mt_list_print(UE_Id);
      break;
    case UE_RB_SU_CMP :
      // ++protocol_bs->rrc.Mobile_List[UE_Id].num_rb; //Temp
      rrc_RG_PHY_Synch_rx (UE_Id);  //Temp
      rrc_UE_RB_Setup_Complete_rx(UE_Id);
      rrc_mt_list_print(UE_Id);
      break;
    case UE_RB_SU_FAIL :
      rrc_RG_PHY_Synch_rx (UE_Id);  //Temp
      rrc_UE_RB_Setup_Failure_rx(UE_Id);
      rrc_mt_list_print(UE_Id);
      break;
    case UE_RB_REL_CMP :
      rrc_UE_RB_Release_Complete_rx(UE_Id);
      // or FFS: rrc_UE_Last_RB_Release_Complete_rx (UE_Id);
      // --protocol_bs->rrc.Mobile_List[UE_Id].num_rb; //Temp
      rrc_RG_PHY_Modify_rx (UE_Id);   //Temp
      rrc_mt_list_print(UE_Id);
      break;
    case UE_RB_REL_FAIL :
      rrc_UE_RB_Release_Failure_rx(UE_Id);
      rrc_mt_list_print(UE_Id);
      break;
    case UE_CELLU :
      rrc_UE_Cell_Update_rx (UE_Id);
      rrc_mt_list_print(UE_Id);
      break;
    case NAS_CONN_CNF :
      //Temp - No RB list provided at connection -
      rrc_NAS_Conn_Cnf_RB_Rx (UE_Id,(int)(protocol_bs->rrc.establishment_cause), 0);
      break;
    case NAS_RB_ESTAB :
      rrc_NAS_RB_Estab_Rx (UE_Id, protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId);
      rrc_RG_PHY_Setup_rx (UE_Id);
    // rrc_RG_RLC_Data_Confirm_rx (UE_Id); Not needed
    // rrc_RG_PHY_Synch_rx (UE_Id);  associated with config
    // rrc_UE_RB_Setup_Complete_rx (UE_Id);
      break;
    case NAS_RB_RELEASE :
      rrc_NAS_RB_Rel_Rx (UE_Id, protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId);
      // rrc_RG_RLC_Data_Confirm_rx (UE_Id); Not needed
      // rrc_UE_RB_Release_Complete_rx (UE_Id);
      // rrc_RG_PHY_Modify_rx (UE_Id);   associated with previous one
      break;
    case RG_RLC_SUCCESS :
      rrc_RG_RLC_Data_Confirm_rx (UE_Id);
      rrc_RG_PHY_Synch_rx (UE_Id);  //associated with config
      break;
    case RG_RLC_FAILURE :
      //rrc_RG_RLC_Status_rx (UE_Id); Temp
      rrc_RG_PHY_Connection_Loss_rx (UE_Id);
      break;
    case RG_CRLC_STATUS :
      //rrc_RG_RLC_Status_rx (UE_Id);
      rrc_RG_PHY_Connection_Loss_rx (UE_Id);
      break;
    case RRM_CFG :
      rrc_RG_Configuration_rx ();
      rrc_RG_PHY_Synch_rx (UE_Id);
      rrc_RG_PHY_Setup_rx (UE_Id);
      break;
    case RRM_FAILURE :
      rrc_RG_Config_failure_rx();
      break;
    case UE_CAP_INFO:
      rrc_RG_UE_Cap_Info_rx(UE_Id);
      break;
    default :
      msg("[RRC][FSM] Invalid call to RRC FSM control\n");
      break;
  }
}











