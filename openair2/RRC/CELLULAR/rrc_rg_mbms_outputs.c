/***************************************************************************
                          rrc_rg_mbms_outputs.c - description
                          -------------------
    begin            : Jul 28, 2005
    copyright        : (C) 2001, 2010 by Eurecom
    created by       : Huu-Nghia.Nguyen@eurecom.fr	
    modified by      : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
    This file implements the code for outputs to RG other layers
 **************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_rg_vars.h"
#include "rrc_mbms_pdus.h"
#include "rrc_msg_class.h"
#include "rrc_rg_mbms_variables.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_mbms.h"
//-----------------------------------------------------------------------------
#ifdef MBMS_INTEGRATION_MODE
#include "rrc_messages.h"
#include "rrc_nas_primitives.h"
//#include "rrc_rg.h"
#include "rrc_proto_int.h"
#include "rrc_proto_intf.h"
//#include "rb_proto_extern.h"
//#include "rrc_srb_proto_extern.h"
#endif
//-----------------------------------------------------------------------------

/**
 * @brief Prepare RRM Request for MBMS RB configuration
 * @date Jul 28, 2005 10:59:05 AM
 */
//-------------------------------------------------------------------
void RRC_RG_MBMS_O_GET_RB_INFORMATION(int action){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
    msg("[RRC-RG][MBMS][IF] Get RB Information from RRM...\n");
  #endif

  #ifdef MBMS_INTEGRATION_MODE
  if (protocol_bs->rrc.rrc_currently_updating == FALSE) {
    //No pending request to RRM
    protocol_bs->rrc.rrc_multicast_bearer = TRUE;
    //Temp - Multicast bearer is attributed to UE_Id 0
    protocol_bs->rrc.Mobile_List[0].requested_rbId = p_rg_mbms->nas_rbId;    //List of RBs
    /* QoS class must be one of the following
         RRC_QOS_MBMS_STREAMING_128  20
         RRC_QOS_MBMS_STREAMING_256  21
         RRC_QOS_MBMS_STREAMING_384  22
     */
    protocol_bs->rrc.Mobile_List[0].requested_QoSclass = p_rg_mbms->nas_QoSclass;
    rrc_rg_compute_configuration (0, action);
  }else{
    msg("\n[RRC-RG][MBMS] Will not contact RRM, there is already a pending request\n\n");
  }
  #endif

  #ifdef MBMS_TEST_MODE
  p_rg_mbms->l12ConfigurationChanged = TRUE;
  #endif
}

/**
* @brief Request L12 to configure RBs
* @date Aug 24, 2005 11:50:48 AM
*/
//-------------------------------------------------------------------
void RRC_RG_MBMS_O_L12_CONFIGURE(void){
//-------------------------------------------------------------------
  int UE_Id=0; //default for MBMS
  #ifdef DEBUG_RRC_STATE
  msg("[RRC-RG][MBMS][IF] Request L12 to configure radio bearers\n");			
  #endif	
  // Should call RRC_RG_O_O_CPHY_RL_SETUP_Req ,
  // but not done here because FACH is used and PHY is not changed
  RRC_RG_O_O_CRLC_CONFIG_Req();
  RRC_RG_O_O_CMAC_CONFIG_Req();
  // clean commands in configuration w/ UE_Id=0 , msg =0
  rg_clear_transaction(UE_Id,0);
  // Confirm to RRM
  rrm_add_radio_access_bearer_confirm(UE_Id, 0); // 2nd parameter is RB_Id, to be included with real value
}

/**
 * @brief Send the message on the DCCH channel
 * @date Jun 27, 2005 9:26:16 AM
 * @param uegID used to locate the mobile terminal in the Mobile_List
 * @param pmsg pointer to MCCH message
 * @msglen length of the MCCH message
 */
//-------------------------------------------------------------------
void RRC_RG_MBMS_O_SEND_DCCH_UM(int ueID, char* pmsg, int msglen){
//-------------------------------------------------------------------
  #ifdef MBMS_INTEGRATION_MODE
  int retcode;
  // send message
  if (msglen > 0 && pmsg != NULL){	
    if ((protocol_bs->rrc.Mobile_List[0].state == CELL_FACH_Connected)
            || (protocol_bs->rrc.Mobile_List[0].state == CELL_DCH_Connected)){
      //retcode = rb_tx_data_srb_rg(RRC_SRB1_ID + (ueID*maxRB), pmsg, msglen * 8, 0, FALSE);
      retcode = rrc_rg_send_to_srb_rlc (ueID, RRC_SRB1_ID, pmsg, msglen);
      #ifdef DEBUG_RRC_STATE
      msg("\033[0;32m[RRC-RG][MBMS][TX] ModifiedServicesInformation, DCCH_UM, length %d\n\033[0m", msglen);			
      #ifdef DEBUG_RRC_MBMS_MSG_CONTENT
      msg("tx: ");	
      rrc_print_buffer((char*) pmsg, msglen);
      #endif
      #endif
    }
  }
  #endif

  #ifdef MBMS_TEST_MODE
  tx_simulate(DCCH_ID, pmsg, msglen);
  #ifdef DEBUG_RRC_STATE	
  if (pmsg != NULL && msglen > 0){
    msg("\033[0;32m[RRC-RG][MBMS][TX] ModifiedServicesInformation, DCCH, length %d, frame %d \n\033[0m", msglen, protocol_bs->rrc.current_SFN);
    #ifdef DEBUG_RRC_MBMS_MSG_CONTENT
    msg("tx: ");	
    rrc_print_buffer((char*) pmsg, msglen);			
    #endif								
  }
  #endif
  #endif
}

/**
 * @brief Send the message on the MCCH channel
 * @date Jun 27, 2005 9:26:16 AM
 * @param pmsg pointer to MCCH message
 * @msglen length of the MCCH message
 */
//-------------------------------------------------------------------
void RRC_RG_MBMS_O_SEND_MCCH(char* pmsg, int msglen){
//-------------------------------------------------------------------
  #ifdef MBMS_INTEGRATION_MODE
  //send message
  if (msglen > 0 && pmsg != NULL){
    rrc_rg_mcch_tx (pmsg, msglen);
    #ifdef DEBUG_RRC_MBMS
    //#ifdef DEBUG_RRC_MBMS_BASIC
      msg("\033[0;32m[RRC-RG][MBMS][TX] ");
      rrc_mbms_mcch_message_name_print(((MCCH_Message*) pmsg)->message.type);
      msg(", MCCH , length %d, frame %d \n\033[0m", msglen, protocol_bs->rrc.current_SFN);
      #ifdef DEBUG_RRC_MBMS_MSG_CONTENT
      msg("tx: ");	
      rrc_print_buffer((char*) pmsg, msglen);
      #endif
    #endif
  }
  #endif

  #ifdef MBMS_TEST_MODE
  tx_simulate(MCCH_ID, pmsg, msglen);
  #ifdef DEBUG_RRC_MBMS
  if (pmsg != NULL && msglen > 0){
    msg("\033[0;32m[RRC-RG][MBMS][TX] ");
    rrc_mbms_mcch_message_name_print(((MCCH_Message*) pmsg)->message.type);
    msg(", MCCH, length %d, frame %d \n\033[0m", msglen, protocol_bs->rrc.current_SFN);
    #ifdef DEBUG_RRC_MBMS_MSG_CONTENT
      msg("tx: ");
      rrc_print_buffer((char*) pmsg, msglen);
    #endif
  }
  #endif	
  #endif
}

/**
 * @brief Send the message on the MSCH channel
 * @date Jun 27, 2005 9:26:16 AM
 * @param pmsg pointer to MCCH message
 * @msglen length of the MCCH message
 */
//-------------------------------------------------------------------
void RRC_RG_MBMS_O_SEND_MSCH(char* pmsg, int msglen){
//-------------------------------------------------------------------
  #ifdef MBMS_INTEGRATION_MODE
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-RG][MBMS] MSCH Channel is not available, ignore the message\n");
    #endif
  #endif

  #ifdef MBMS_TEST_MODE
    tx_simulate(MSCH_ID, pmsg, msglen);
    #ifdef DEBUG_RRC_MBMS
    if (pmsg != NULL && msglen > 0){
      msg("\033[0;32m[RRC-RG][MBMS][TX] SchedulingInformation, MSCH, length %d, frame %d \n\033[0m", msglen, protocol_bs->rrc.current_SFN);
      #ifdef DEBUG_RRC_MBMS_MSG_CONTENT
        msg("tx: ");
        rrc_print_buffer((char*) pmsg, msglen);
      #endif
    }
    #endif
  #endif
}

//-------------------------------------------------------------------
void RRC_RG_MBMS_O_UE_NOTIFY_CNF(void){
//-------------------------------------------------------------------
  #ifdef MBMS_INTEGRATION_MODE
  struct nas_rg_if_element *msgToBuild;
  mem_block_t *p = get_free_mem_block(sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;  // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;
  msgToBuild->prim_length = NAS_TL_SIZE + sizeof(struct NASMBMSUENotifyCnf);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[p_rg_mbms->nas_ueID];  
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = MBMS_UE_NOTIFY_CNF;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.mbms_ue_notify_cnf.localConnectionRef = protocol_bs->rrc.Mobile_List[p_rg_mbms->nas_ueID].local_connection_ref;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.mbms_ue_notify_cnf.mbmsStatus = ACCEPTED; //Temp - hard coded	
  #ifdef DEBUG_RRC_STATE
  msg("[RRC-RG][MBMS] MBMS_UE_NOTIFY_CNF primitive sent to NAS, length %d.\n", msgToBuild->prim_length);
  #endif

  #endif

  #ifdef MBMS_TEST_MODE	
   msg("[RRC-RG][MBMS] MBMS_UE_NOTIFY_CNF primitive sent to NAS.\n");
  #endif
}

//-------------------------------------------------------------------
void RRC_RG_O_O_NAS_MBMS_RB_ESTAB_CNF (void){
//-------------------------------------------------------------------
#ifdef MBMS_INTEGRATION_MODE
//  int rb_id;
//  int qos_class;
  // This is hard-coded to go in DC-SAP of MT 0, to avoid introducing an uplink GC-SAP in RG
  int UE_Id = 0;
  struct nas_rg_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;
  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASMBMSBearerEstablishConf);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = MBMS_BEARER_ESTABLISH_CNF;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.mbms_establish_cnf.rbId = p_rg_mbms->nas_rbId;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.mbms_establish_cnf.sapId = p_rg_mbms->nas_sapId;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.mbms_establish_cnf.status = p_rg_mbms->nas_status;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC-RG][MBMS][FSM-OUT] MBMS_BEARER_ESTABLISH_CNF primitive sent to NAS, for mobile %d.\n", UE_Id);
  #endif
#endif
}


