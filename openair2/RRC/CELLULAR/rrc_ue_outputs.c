/***************************************************************************
                          rrc_ue_outputs.c  -  description
                             -------------------
    begin                : Jan 11, 2002
    copyright            : (C) 2001, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Output functions for RRC UE FSM
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
//-------------------------------------------------------------------
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
#include "rrc_proto_int.h"
#include "rrc_proto_bch.h"

/* Output Trigger functions - Connection Model(protos in rrc_ue.c) */
//-------------------------------------------------------------------
void RRC_UE_O_O_PHY_SYNCH(void){
//-------------------------------------------------------------------
  int i;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] PHY Synchronization requested.\n");
  #endif
  CPHY_config_req (rrm_config, 0, protocol_ms->rrc.u_rnti);
  crb_config_req (0);
  cmac_config_req (protocol_ms->rrc.u_rnti, 0);
  /* TEMP - Waiting for new RRM
  // clean up commands in config
  for (i = 0; i < JRRM_MAX_COMMANDS_PER_TRANSACTION; i++) {
    (&rrm_config->mt_config)->rrm_commands[i].rrm_action = 0;
  }
  */
}

//-------------------------------------------------------------------
void RRC_UE_O_O_SEND_CCCH (int msgId){
//-------------------------------------------------------------------
  char *tx_ccch_info;
  int data_length = 0;
  int  retcode;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] Sending Message on CCCH.");
  #endif

  tx_ccch_info = (char *) protocol_ms->rrc.ue_msg_infos.msg_ptr;
  data_length = protocol_ms->rrc.ue_msg_infos.msg_length;
  msg (" data length %d\n", data_length);
  if (data_length > 0) {
    //retcode = Mac_rlc_xface->rrc_rlc_data_req (RRC_MODULE_INST_ID, RRC_LCHAN_SRB0_ID, 0, RRC_RLC_CONFIRM_NO, data_length, tx_ccch_info);
    retcode = rrc_ue_send_to_srb_rlc (RRC_SRB0_ID, tx_ccch_info, data_length);
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC][FSM-OUT] Message sent on CCCH. SRB0 -- retcode = %d (1=OK) -- data length %d\n", retcode, data_length); //RC = 1 ==> OK
    #endif
  }
  msg ("[RRC][FSM-OUT] Debug T300 target %d, ", protocol_ms->rrc.rrc_ue_t300_target);
  // clear resources if T300 not running, otherwise keep them available
  if (protocol_ms->rrc.rrc_ue_t300_target == 0){
    protocol_ms->rrc.ue_msg_infos.msg_length = 0;
    //  free(protocol_ms->rrc.ue_msg_infos.msg_ptr);
    free_mem_block (protocol_ms->rrc.ue_msg_infos.mem_block_ptr);
  }
  msg ("Message length %d\n", protocol_ms->rrc.ue_msg_infos.msg_length);
}

//-------------------------------------------------------------------
void RRC_UE_O_O_SEND_DCCH_UM (int msgId){
//-------------------------------------------------------------------
  char *tx_dcch_info;
  int data_length = 0;
  int retcode;

  // send message
  tx_dcch_info = (char *) protocol_ms->rrc.ue_msg_infos.msg_ptr;
  data_length = protocol_ms->rrc.ue_msg_infos.msg_length;

  if (data_length > 0) {
    //retcode = Mac_rlc_xface->rrc_rlc_data_req (RRC_MODULE_INST_ID, RRC_LCHAN_SRB1_ID, 0, RRC_RLC_CONFIRM_NO, data_length, tx_dcch_info);
    retcode = rrc_ue_send_to_srb_rlc (RRC_SRB1_ID, tx_dcch_info, data_length);
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC][FSM-OUT] Message %s sent on DCCH-UM, ret code= %d.\n", tx_dcch_info, retcode);
    #endif
  }
  // clear resources
  protocol_ms->rrc.ue_msg_infos.msg_length = 0;
  //  free(protocol_ms->rrc.ue_msg_infos.msg_ptr);
  free_mem_block (protocol_ms->rrc.ue_msg_infos.mem_block_ptr);
}


// This is the real function, when all srbs are available -
//-------------------------------------------------------------------
void RRC_UE_O_O_SEND_DCCH_AM (int msgId){
//-------------------------------------------------------------------
  char *tx_dcch_info;
  int data_length = 0;
  int retcode;

  // send message
  tx_dcch_info = (char *) protocol_ms->rrc.ue_msg_infos.msg_ptr;
  data_length = protocol_ms->rrc.ue_msg_infos.msg_length;

  if (data_length > 0) {
    //retcode = rb_tx_data_srb_mt (RRC_SRB2_ID, tx_dcch_info, data_length * 8, protocol_ms->rrc.next_MUI++, TRUE);
    retcode = rrc_ue_send_to_srb_rlc (RRC_SRB2_ID, tx_dcch_info, data_length);

  #ifdef DEBUG_RRC_STATE
    msg ("[RRC][FSM-OUT] Message %s sent on DCCH-AM, ret code= %d.\n", tx_dcch_info, retcode);
  #endif
  }
  // clear resources
  protocol_ms->rrc.ue_msg_infos.msg_length = 0;
  //free(protocol_ms->rrc.ue_msg_infos.msg_ptr);
  free_mem_block (protocol_ms->rrc.ue_msg_infos.mem_block_ptr);
}


//-----------------------------------------------------------------------------
void RRC_UE_SEND_AM_SRB3 (int msgId){
//-----------------------------------------------------------------------------
  char *tx_dcch_info;
  int data_length = 0;
  int retcode;

  // send message
  tx_dcch_info = (char *) protocol_ms->rrc.ue_msg_infos.msg_ptr;
  data_length = protocol_ms->rrc.ue_msg_infos.msg_length;

  if (data_length > 0) {
    //retcode = rb_tx_data_srb_mt (RRC_SRB3_ID, tx_dcch_info, data_length * 8, protocol_ms->rrc.next_MUI++, TRUE);
    retcode = rrc_ue_send_to_srb_rlc (RRC_SRB3_ID, tx_dcch_info, data_length);
    #ifdef DEBUG_RRC_STATE
    msg ("[RRC][DATA-OUT] Message %s sent on DCCH-AM-SRB3, ret code= %d.\n", tx_dcch_info, retcode);
    #endif
  }
  // clear resources
  protocol_ms->rrc.ue_msg_infos.msg_length = 0;
  // free(protocol_ms->rrc.ue_msg_infos.msg_ptr);
  free_mem_block (protocol_ms->rrc.ue_msg_infos.mem_block_ptr);
}


//-------------------------------------------------------------------
void RRC_UE_O_O_NAS_CONN_ESTAB_RESP (int retCode){
//-------------------------------------------------------------------
  struct nas_ue_if_element *msgToBuild;
  //Added for UE Capability Information
  int msgId;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_ue_if_element));
  protocol_ms->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASConnEstablishResp);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
  msgToBuild->nasUePrimitive.dc_sap_prim.type = CONN_ESTABLISH_RESP;
  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.conn_establish_resp.localConnectionRef = protocol_ms->rrc.local_connection_ref;
  strcpy (((char *) msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.conn_establish_resp.InterfaceIMEI), protocol_ms->rrc.IMEI);
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.conn_establish_resp.status = retCode;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] CONN Response Message sent to NAS, length %d, with status %d.\n", msgToBuild->prim_length, retCode);
  msg ("TEMP OPENAIR : COMMENTED UE Capability Information\n");
  #endif
  //Added for UE Capability Information - should be in FSM
      // TEMPComment - OPENAIR
  //encode_message(&msgId, UE_CAPABILITY_INFO);
  RRC_UE_O_O_SEND_DCCH_AM(msgId);
}

////-------------------------------------------------------------------
void RRC_UE_O_O_NAS_CONN_RELEASE_IND(void){
////-------------------------------------------------------------------
//  struct nas_ue_if_element *msgToBuild;
//      
//  mem_block_t *p =  get_free_mem_block(sizeof (struct nas_ue_if_element));
//  protocol_ms->rrc.NASMessageToXmit = p;  // Temp - will later enqueue at bottom of list
//  //Set pointer to newly allocated structure and fills it
//  msgToBuild = (struct nas_ue_if_element *) p->data;
//
//  msgToBuild->prim_length = NAS_TL_SIZE + sizeof(struct NASConnReleaseInd);
//  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
//  msgToBuild->nasUePrimitive.dc_sap_prim.type = CONN_RELEASE_IND;
//  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;
//  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.conn_release_ind.localConnectionRef
//                                                                                                                                                                                                                      = protocol_ms->rrc.local_connection_ref;
//  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.conn_release_ind.releaseCause=protocol_ms->rrc.failure_cause;
//  #ifdef DEBUG_RRC_STATE
//      msg("[RRC][FSM-OUT] CONN_RELEASE_IND primitive sent to NAS, length %d, with cause code %d.\n",
//                                  msgToBuild->prim_length,  protocol_ms->rrc.failure_cause);
//  #endif
}

//-------------------------------------------------------------------
void RRC_UE_O_O_NAS_CONN_LOSS_IND(void){
//-------------------------------------------------------------------
  struct nas_ue_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_ue_if_element));
  protocol_ms->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASConnLossInd);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
  msgToBuild->nasUePrimitive.dc_sap_prim.type = CONN_LOSS_IND;
  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.conn_loss_ind.localConnectionRef = protocol_ms->rrc.local_connection_ref;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] CONN LOSS IND primitive sent to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
}

//-------------------------------------------------------------------
void RRC_UE_O_O_UpdateSI_852(void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("\n\n[RRC][FSM-OUT] Clean System Information as described in section 8.5.2.\n");
  #endif
  memset ((char *) &(protocol_ms->rrc.ue_bch_blocks), 0, sizeof (struct rrc_ue_bch_blocks));
  rrc_ue_bch_init ();

}

//-------------------------------------------------------------------
void RRC_UE_O_O_stopT300(void){
//-------------------------------------------------------------------
  // stop timer 
  //umts_timer_delete_timer (&protocol_ms->rrc.rrc_timers, RRC_T300);
  protocol_ms->rrc.rrc_ue_t300_target =0;
  protocol_ms->rrc.rrc_ue_t300_retry = 0;
  // clear resources - Temp
  protocol_ms->rrc.ue_msg_infos.msg_length = 0;
  //free(protocol_ms->rrc.ue_msg_infos.msg_ptr);
  if (protocol_ms->rrc.ue_msg_infos.mem_block_ptr != NULL) {
    free_mem_block (protocol_ms->rrc.ue_msg_infos.mem_block_ptr);
  }
  protocol_ms->rrc.ue_msg_infos.mem_block_ptr = NULL;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] Stop Physical Timer T300.\n");
  #endif
}

//-------------------------------------------------------------------
void RRC_UE_O_O_startT300(void){
//-------------------------------------------------------------------

  int targetFrameNumber = protocol_ms->rrc.current_SFN*RRC_FRAME_DURATION;
  //umts_add_timer_list_up (&protocol_ms->rrc.rrc_timers, rrc_ue_t300_timeout, NULL, RRC_T300, T300_DURATION, protocol_ms->rrc.current_SFN/RRC_FRAME_DURATION);
  // Nest line is for real-time operation while second one is for emulation mode debug
  //protocol_ms->rrc.rrc_ue_t300_target = protocol_ms->rrc.current_SFN + (T300_DURATION/RRC_FRAME_DURATION);
  if (protocol_ms->rrc.rrc_ue_t300_retry == 0)
     protocol_ms->rrc.rrc_ue_t300_target = protocol_ms->rrc.current_SFN + 10;
  else 
     protocol_ms->rrc.rrc_ue_t300_target = protocol_ms->rrc.current_SFN + 200;

  protocol_ms->rrc.rrc_ue_t300_retry++;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] Start Physical Timer T300. frame %d, target %d\n", protocol_ms->rrc.current_SFN, protocol_ms->rrc.rrc_ue_t300_target);
  #endif
}

//-------------------------------------------------------------------
void RRC_UE_O_O_Setup_FACHRACH(void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] Configure FACH-RACH channels.\n");
  #endif
  rrc_ue_L2_setupFachRach();

}

/*****************************************************************/
//-------------------------------------------------------------------
void RRC_UE_O_O_wait (int duration){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] Request timer on RRC_CONNECTION_REJECT: duration %d.\n", duration);
  #endif
}

//-------------------------------------------------------------------
void RRC_UE_O_O_startT308 (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][FSM-OUT] Start Physical Timer T308.\n");
  #endif
}

/*****************************************************************/
/* Output Trigger functions - RB Control Model (protos in rrc_rb_ue.c) */

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_NAS_RB_ESTABLISH_Ind (int rb_id){
//-------------------------------------------------------------------
  struct nas_ue_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_ue_if_element));
  protocol_ms->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASrbEstablishInd);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
  msgToBuild->nasUePrimitive.dc_sap_prim.type = RB_ESTABLISH_IND;
  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_establish_ind.localConnectionRef = protocol_ms->rrc.local_connection_ref;
  // Temp MW 19/9/13 - Next parameter to be hard coded and should be revised for support of multiple DRBs (transferred at arrival in NAS)
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_establish_ind.rbId = protocol_ms->rrc.requested_rbId;
  //msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_establish_ind.rbId = RRC_OAI_DRB0_ID;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_establish_ind.QoSclass = protocol_ms->rrc.requested_QoSclass;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_establish_ind.dscp = protocol_ms->rrc.requested_dscp;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_establish_ind.sapId = protocol_ms->rrc.requested_sapid;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] RB_ESTABLISH_Ind primitive ready to send to NAS, length %d.\n", msgToBuild->prim_length, msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_establish_ind.rbId);
  #endif
  //Temp - to be put back in clear_transaction
  protocol_ms->rrc.requested_rbId = 0;
  protocol_ms->rrc.requested_QoSclass = 0;
  protocol_ms->rrc.requested_dscp = 0;
  protocol_ms->rrc.requested_sapid = 0;
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_NAS_RB_RELEASE_Ind (int rb_id){
//-------------------------------------------------------------------
  struct nas_ue_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_ue_if_element));
  protocol_ms->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASrbReleaseInd);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
  msgToBuild->nasUePrimitive.dc_sap_prim.type = RB_RELEASE_IND;
  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_release_ind.localConnectionRef = protocol_ms->rrc.local_connection_ref;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.rb_release_ind.rbId = protocol_ms->rrc.requested_rbId;
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] RB_RELEASE_Ind primitive sent to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
  protocol_ms->rrc.requested_rbId = 0;
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_CPHY_RL_SETUP_Req (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Sending RL_SETUP_Req primitive to PHY.\n");
  #endif
  CPHY_config_req (rrm_config, 0, protocol_ms->rrc.u_rnti);
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_CPHY_RL_RELEASE_Req (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Sending RL_RELEASE_Req primitive to PHY.\n");
   msg ("TEMP OPENAIR : COMMENTED\n");
  #endif
  CPHY_config_req (rrm_config, 0, protocol_ms->rrc.u_rnti);
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_CMAC_CONFIG_Req (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Sending CMAC_CONFIG_Req primitive to RLC + MAC.\n");
  #endif
  crb_config_req (0);
  cmac_config_req (protocol_ms->rrc.u_rnti, 0);
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_CRLC_CONFIG_Req (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Sending CRLC_CONFIG_Req primitive to RLC.\n");
  #endif
  crb_config_req(0);
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_SEND_DCCH_AM (int msgId){
//-------------------------------------------------------------------
  // call function from RRC_UE FSM to avoid duplication of errors
  RRC_UE_O_O_SEND_DCCH_AM (msgId);
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] RRC_RB_UE_O_O_SEND_DCCH_AM complete.\n");
  #endif
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_SEND_CCCH (int msgId){
//-------------------------------------------------------------------
  // call function from RRC_UE FSM to avoid duplication of errors
   RRC_UE_O_O_SEND_CCCH (msgId);
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] RRC_RB_UE_O_O_SEND_CCCH complete.\n");
  #endif
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_New_State_CELL_FACH (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Transition to CELL_FACH state.\n");
  msg ("[RRC][FSM-IN] Go_CELL_FACH Received\n");
  #endif
  RRC_UE_I_I_Go_CELL_FACH ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_New_State_CELL_DCH (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Transition to CELL_DCH state.\n");
  msg ("[RRC][FSM-IN] Go_CELL_DCH Received\n");
  #endif
  RRC_UE_I_I_Go_CELL_DCH ();
  RRC_UE ();
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_startT305 (void){
//-------------------------------------------------------------------
// Not implemented yet. Will be in final CELL_UPDATE procedure
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Start Physical Timer T305.\n");
  #endif
}

//-------------------------------------------------------------------
void RRC_RB_UE_O_O_CELL_Update_Proc (void){
//-------------------------------------------------------------------
// Not implemented yet. Will be in final CELL_UPDATE procedure
  #ifdef DEBUG_RRC_STATE
  msg ("[RRC_RB][FSM-OUT] Execute Cell Update procedure as requested by RG.\n");
  #endif
}

/*    Other functions, not related with Esterel   */
//-----------------------------------------------------------------------------
// build NAS broadcast indication message
void rrc_ue_broadcast_encode_nas_sib1 (void){
//-----------------------------------------------------------------------------
//      char *msgData =  "INFO_BROADCAST_IND\0";
  struct nas_ue_if_element *msgToBuild;
  char *pdata;
  int data_length;
  mem_block_t *p = NULL;

  data_length = protocol_ms->rrc.ue_bch_blocks.currSIB1.subnet_NAS_SysInfo.numocts;

  if ((data_length > 0) && (protocol_ms->rrc.NASMessageToXmit == NULL)) {

    p = get_free_mem_block (sizeof (struct nas_ue_if_element) + RRC_NAS_MAX_SIZE);
    protocol_ms->rrc.NASMessageToXmit = p;

    //Set pointer to newly allocated structure and fills it
    msgToBuild = (struct nas_ue_if_element *) p->data;

    msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASInfoBroadcastInd);
    msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_GC_fifo;
    msgToBuild->nasUePrimitive.gc_sap_prim.type = INFO_BROADCAST_IND;
    msgToBuild->nasUePrimitive.gc_sap_prim.length = msgToBuild->prim_length;
    //  msgToBuild->nasUePrimitive.gc_sap_prim.nasUEGCPrimitive.broadcast_ind.nasDataLength = strlen (msgData);
    //  memcpy (((char*) msgToBuild->nasData), msgData , );
    // (char*)&protocol_ms->rrc.ue_bch_blocks.currSIB1.subnet_NAS_SysInfo.data
    // (char*)&protocol_ms->rrc.ue_bch_blocks.currSIB1.subnet_NAS_SysInfo.numocts
    msgToBuild->nasUePrimitive.gc_sap_prim.nasUEGCPrimitive.broadcast_ind.nasDataLength = (u16) data_length;
    pdata = (char *) &(msgToBuild->nasUePrimitive.gc_sap_prim.nasUEGCPrimitive.broadcast_ind.nasDataLength);
    pdata += 2;
    memcpy (pdata, (char *) &protocol_ms->rrc.ue_bch_blocks.currSIB1.subnet_NAS_SysInfo.data, data_length);
    msgToBuild->prim_length += data_length;
    #ifdef DEBUG_RRC_BROADCAST_NAS
    msg ("[RRC] Building BROADCAST IND for NAS - SIB1, length:  %d.\n", msgToBuild->prim_length);
    #endif
  } else {
    #ifdef DEBUG_RRC_BROADCAST_NAS
    msg ("[RRC] BROADCAST IND for NAS, No SIB1 data.\n");
    #endif
  }

}

//-----------------------------------------------------------------------------
// build NAS broadcast indication message
void rrc_ue_broadcast_encode_nas_sib18 (void){
//-----------------------------------------------------------------------------
//      char *msgData =  "INFO_BROADCAST_IND\0";
  struct nas_ue_if_element *msgToBuild;
  char *pdata;
  int data_length;

  mem_block_t *p;

  data_length = protocol_ms->rrc.ue_bch_blocks.currSIB18.cellIdentities.numocts;

  if ((data_length > 0) && (protocol_ms->rrc.NASMessageToXmit == NULL)) {
    p = get_free_mem_block (sizeof (struct nas_ue_if_element) + RRC_NAS_MAX_SIZE);
    protocol_ms->rrc.NASMessageToXmit = p;

    //Set pointer to newly allocated structure and fills it
    msgToBuild = (struct nas_ue_if_element *) p->data;

    msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASInfoBroadcastInd);
    msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_GC_fifo;
    msgToBuild->nasUePrimitive.gc_sap_prim.type = INFO_BROADCAST_IND;
    msgToBuild->nasUePrimitive.gc_sap_prim.length = msgToBuild->prim_length;
    msgToBuild->nasUePrimitive.gc_sap_prim.nasUEGCPrimitive.broadcast_ind.nasDataLength = (u16) data_length;
    pdata = (char *) &(msgToBuild->nasUePrimitive.gc_sap_prim.nasUEGCPrimitive.broadcast_ind.nasDataLength);
    pdata += 2;
    memcpy (pdata, (char *) &protocol_ms->rrc.ue_bch_blocks.currSIB18.cellIdentities.data, data_length);
    msgToBuild->prim_length += data_length;
    #ifdef DEBUG_RRC_BROADCAST_NAS
    msg ("[RRC] Building BROADCAST IND - SIB 18 - for NAS, length:  %d.\n", msgToBuild->prim_length);
    #endif
  } else {
    #ifdef DEBUG_RRC_BROADCAST_NAS
    msg ("[RRC] BROADCAST IND for NAS, No SIB18 data.\n");
    #endif
  }
}

//-------------------------------------------------------------------
void RRC_UE_O_NAS_DATA_IND (void){
//-------------------------------------------------------------------
  struct nas_ue_if_element *msgToBuild;
  char *pdata;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_ue_if_element) + RRC_NAS_MAX_SIZE);
  protocol_ms->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASDataInd);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
  msgToBuild->nasUePrimitive.dc_sap_prim.type = DATA_TRANSFER_IND;
  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.data_transfer_ind.localConnectionRef = protocol_ms->rrc.local_connection_ref;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.data_transfer_ind.priority = 5;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.data_transfer_ind.nasDataLength = protocol_ms->rrc.dl_nas_message_lgth;
  pdata = (char *) &(msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.data_transfer_ind.nasDataLength);
  pdata += 2;
  memcpy (pdata, protocol_ms->rrc.dl_nas_message_ptr, protocol_ms->rrc.dl_nas_message_lgth);
  msgToBuild->prim_length += protocol_ms->rrc.dl_nas_message_lgth;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][DATA-OUT] DATA_TRANSFER_IND primitive ready to send to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
}

//-------------------------------------------------------------------
void RRC_UE_O_NAS_NOTIFICATION_IND (void){
//-------------------------------------------------------------------
  struct nas_ue_if_element *msgToBuild;
  char *pdata;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_ue_if_element) + RRC_NAS_MAX_SIZE);
  protocol_ms->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASNotificationInd);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_NT_fifo;
  msgToBuild->nasUePrimitive.nt_sap_prim.type = NOTIFICATION_IND;
  msgToBuild->nasUePrimitive.nt_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasUePrimitive.nt_sap_prim.nasUENTPrimitive.notification_ind.nasDataLength = protocol_ms->rrc.paging_message_lgth;
  pdata = (char *) &(msgToBuild->nasUePrimitive.nt_sap_prim.nasUENTPrimitive.notification_ind.nasDataLength);
  pdata += 2;
  memcpy (pdata, protocol_ms->rrc.paging_message_ptr, protocol_ms->rrc.paging_message_lgth);
  msgToBuild->prim_length += protocol_ms->rrc.paging_message_lgth;

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][DATA-OUT] NOTIFICATION_IND primitive ready to send to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
}

//-------------------------------------------------------------------
void RRC_UE_O_NAS_MEASUREMENT_IND (void){
//-------------------------------------------------------------------
  struct nas_ue_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_ue_if_element));
  protocol_ms->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASMeasureInd);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
  msgToBuild->nasUePrimitive.dc_sap_prim.type = MEASUREMENT_IND;
  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.measurement_ind.localConnectionRef = protocol_ms->rrc.local_connection_ref;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.measurement_ind.nb_rg = 2;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.measurement_ind.measures[0].cell_id = protocol_ms->rrc.cell_id;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.measurement_ind.measures[0].level =
    // TEMPComment - OPENAIR
    100;  //hard coded value - also translation to get a positive value [0-90]
    //(rrm_config->outer_loop_vars.RSSI0 / RATIO_RSSI0) + SHIFT_RSSI0;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.measurement_ind.measures[1].cell_id = 2;
  msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.measurement_ind.measures[1].level = 50;

  #ifdef DEBUG_RRC_STATE
      // TEMPComment - OPENAIR
  //msg ("[RRC][DATA-OUT] MEASUREMENT_IND primitive: RSSI0 = %d .\n", rrm_config->outer_loop_vars.RSSI0);
  msg ("[RRC][DATA-OUT] MEASUREMENT_IND primitive ready to send to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
}
