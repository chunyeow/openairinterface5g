/***************************************************************************
                          rrc_rg_outputs.c  -  description
                             -------------------
    begin                : May 29, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Contains the output functions for the RRC_RG model
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
#include "rrc_nas_primitives.h"
#include "rrc_messages.h"
//-------------------------------------------------------------------
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
#include "rrc_proto_int.h"

//-------------------------------------------------------------------
// #ifndef USER_MODE
// #ifndef BYPASS_L1
// #include "daq.h"
// #include "L1L_extern.h"
// #endif
// #endif
// 
// #ifndef BYPASS_L1
// #include "L1L_proto.h"
// #include "L1H_proto.h"
// #include "L1C.h"
// #include "L1H.h"
// #include "L1L.h"
// #endif
// 
// #include "umts_sched_struct.h"
// #include "umts_sched_proto_extern.h"
// #include "rlc.h"
// #include "mac_log_interface_flow_proto_extern.h"
// #include "mac_traffic_measurement_proto_extern.h"
// #include "mac_control_primitives_proto_extern.h"
// #include "rb_proto_extern.h"

/* Output Trigger functions - Radio Gateway Model (protos in rrc_rg_data.h) */
//-------------------------------------------------------------------
void RRC_RG_O_O_SEND_CCCH (int msgId){
//-------------------------------------------------------------------
  // Temp 01/03 - msgId contains UE_Id
  char *tx_ccch_info;
  int  data_length = 0;
  int  retcode;

  tx_ccch_info = (char *) protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr;
  data_length = protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length;

  if (data_length > 0) {
    //retcode = Mac_rlc_xface->rrc_rlc_data_req (RRC_MODULE_INST_ID, RRC_LCHAN_SRB0_ID, 0, RRC_RLC_CONFIRM_NO, data_length, tx_ccch_info);
    retcode = rrc_rg_send_to_srb_rlc (msgId, RRC_SRB0_ID, tx_ccch_info, data_length);
    #ifdef DEBUG_RRC_STATE
      msg ("[RRC_RG][FSM-OUT] Message sent on CCCH. SRB0 -- retcode = %d -- data length %d\n", retcode, data_length); //RC = 1 ==> OK
      //msg ("[RRC_RG][FSM-OUT] Message %s sent on CCCH. SRB0\n", tx_ccch_info);
    #endif
  }
  // clear resources
  protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length = 0;
  //free(protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr);
  free_mem_block (protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.mem_block_ptr);
}

//-------------------------------------------------------------------
void RRC_RG_O_O_SEND_DCCH_UM (int msgId){
//-------------------------------------------------------------------
  // Temp 01/03 - msgId contains UE_Id
  char tx_dcch_info;
  int  data_length = 0 ;
  int  retcode;

  // prepare message
  tx_dcch_info = (char *) protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr;
  data_length = protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length;
  if (data_length > 0) {
    //retcode = rb_tx_data_srb_rg (RRC_SRB1_ID + (msgId * maxRB), tx_dcch_info, data_length * 8, 0, FALSE);
    //retcode = Mac_rlc_xface->rrc_rlc_data_req (RRC_MODULE_INST_ID, RRC_LCHAN_SRB1_ID, 0, RRC_RLC_CONFIRM_NO, data_length, tx_dcch_info);
    retcode = rrc_rg_send_to_srb_rlc (msgId, RRC_SRB1_ID, tx_dcch_info, data_length);
    #ifdef DEBUG_RRC_STATE
      msg ("[RRC_RG][FSM-OUT] Message %s sent on DCCH-UM, ret code= %d.\n", tx_dcch_info, retcode);
    #endif
  }
  // clear resources
  protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length = 0;
  //free(protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr);
  free_mem_block (protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.mem_block_ptr);
}

//-------------------------------------------------------------------
void RRC_RG_O_O_SEND_DCCH_AM (int msgId){
//-------------------------------------------------------------------
  // Temp 01/03 - msgId contains UE_Id
  char *tx_dcch_info;
  int data_length = 0;
  int retcode;

  // send message
  tx_dcch_info = (char *) protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr;
  data_length = protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length;
  if (data_length > 0) {
    //retcode = rb_tx_data_srb_rg (RRC_SRB2_ID + (msgId * maxRB), tx_dcch_info, data_length * 8, protocol_bs->rrc.next_MUI++, TRUE);
    //retcode = Mac_rlc_xface->rrc_rlc_data_req (RRC_MODULE_INST_ID, RRC_LCHAN_SRB2_ID, protocol_bs->rrc.next_MUI++, 0, data_length, tx_dcch_info);
    retcode = rrc_rg_send_to_srb_rlc (msgId, RRC_SRB2_ID, tx_dcch_info, data_length);
    #ifdef DEBUG_RRC_STATE
      msg ("[RRC_RG][FSM-OUT] Message %s sent on DCCH-AM, MUI %d, ret code= %d.\n", tx_dcch_info, (protocol_bs->rrc.next_MUI)-1,retcode);
      rrc_print_buffer (tx_dcch_info, 15);
    #endif
  }
  // clear resources
  protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length = 0;
  //  free(protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr);
  free_mem_block (protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.mem_block_ptr);
}

//-------------------------------------------------------------------
void RRC_RG_SEND_AM_SRB3 (int msgId){
//-------------------------------------------------------------------
  // Temp 01/03 - msgId contains UE_Id
  char *tx_dcch_info;
  int data_length = 0;
  int retcode;

  // send message
  tx_dcch_info = (char *) protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr;
  data_length = protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length;
  if (data_length > 0) {
    //retcode = rb_tx_data_srb_rg (RRC_SRB3_ID + (msgId * maxRB), tx_dcch_info, data_length * 8, protocol_bs->rrc.next_MUI++, TRUE);
    retcode = rrc_rg_send_to_srb_rlc (msgId, RRC_SRB3_ID, tx_dcch_info, data_length);
    #ifdef DEBUG_RRC_STATE
      msg ("[RRC_RG][DATA-OUT] Message %s sent on DCCH-AM-SRB3, ret code= %d.\n", tx_dcch_info, retcode);
    #endif
  }
  // clear resources
  protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_length = 0;
  //  free(protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.msg_ptr);
  free_mem_block (protocol_bs->rrc.Mobile_List[msgId].rg_msg_infos.mem_block_ptr);
}

//Output of primitives to NAS
//-------------------------------------------------------------------
void RRC_RG_O_O_NAS_CONN_ESTAB_IND (int UE_Id){
//-------------------------------------------------------------------
  struct nas_rg_if_element *msgToBuild;
  int j;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;

  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASConnEstablishInd);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = CONN_ESTABLISH_IND;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  // Initial setting of local connection reference - still TBD
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.conn_establish_ind.localConnectionRef = protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref;
  for (j = 0; j < 14; j++)
    msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.conn_establish_ind.InterfaceIMEI[j]
      = protocol_bs->rrc.Mobile_List[UE_Id].IMEI[j];

  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] CONN Indication Message sent to NAS, for mobile %d.\n", UE_Id);
    msg ("[RRC] RG NAS PRIMITIVE ENCODE, length Id %d.\n", msgToBuild->prim_length);
  //  msg("[RRC_DEBUG] Pointer p %p , Control Block %p.\n",p, protocol_bs->rrc.NASMessageToXmit);
  #endif
}

//-------------------------------------------------------------------
void RRC_RG_O_O_NAS_CONN_RELEASE_IND (int UE_Id){
//-------------------------------------------------------------------
  struct nas_rg_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASConnReleaseInd);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = CONN_RELEASE_IND;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.conn_release_ind.localConnectionRef = protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.conn_release_ind.releaseCause = protocol_bs->rrc.Mobile_List[UE_Id].release_cause;
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC][FSM-OUT] CONN_RELEASE_IND primitive sent to NAS, length %d, with cause code %d.\n", msgToBuild->prim_length, protocol_bs->rrc.Mobile_List[UE_Id].release_cause);
  #endif
}

//-------------------------------------------------------------------
void RRC_RG_O_O_NAS_CONN_LOSS_IND (int UE_Id){
//-------------------------------------------------------------------
  struct nas_rg_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASConnLossInd);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = CONN_LOSS_IND;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.conn_loss_ind.localConnectionRef = protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref;
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC][FSM-OUT] CONN LOSS IND primitive sent to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
}

//-------------------------------------------------------------------
void RRC_RG_O_O_NAS_RB_ESTAB_CNF (int UE_Id){
//-------------------------------------------------------------------
  struct nas_rg_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASrbEstablishConf);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = RB_ESTABLISH_CNF;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.localConnectionRef = protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.rbId = protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.sapId = protocol_bs->rrc.Mobile_List[UE_Id].requested_sapid;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.status = ACCEPTED;
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] RB_ESTABLISH_Cnf primitive sent to NAS, for mobile %d, RB %u.\n", UE_Id, protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId);
  #endif
}

//-------------------------------------------------------------------
void RRC_RG_O_O_NAS_RB_Failure (int UE_Id){
//-------------------------------------------------------------------
  struct nas_rg_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASrbEstablishConf);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = RB_ESTABLISH_CNF;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.localConnectionRef = protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.rbId = protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.sapId = 3;  //Temp
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.status = FAILURE;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.rb_establish_conf.fail_code = protocol_bs->rrc.curr_rb_failure_code;
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] RB_Establish_Failure primitive sent to NAS, for mobile %d, RB %u.\n", UE_Id, protocol_bs->rrc.Mobile_List[UE_Id].requested_rbId);
  #endif
}

// Control of other Access Stratum entities
//-------------------------------------------------------------------
void RRC_RG_O_O_CPHY_RL_SETUP_Req (void){
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] Sending RL_SETUP_Req primitive to PHY.\n");
  #endif
  CPHY_config_req (rrm_config, 0, 0);
}

//-------------------------------------------------------------------
void RRC_RG_O_O_CPHY_RL_RELEASE_Req (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] Sending RL_RELEASE_Req primitive to PHY.\n");
  #endif
  CPHY_config_req (rrm_config, 0, 0);
}

//-------------------------------------------------------------------
void RRC_RG_O_O_CRLC_CONFIG_Req (void){
// WARNING : RLC Config must be done before MAC config
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] Sending CRLC_CONFIG_Req primitive to RLC (crb).\n");
  #endif
  crb_config_req (0);
}

//-------------------------------------------------------------------
void RRC_RG_O_O_CMAC_CONFIG_Req (void){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] Sending CMAC_CONFIG_Req primitive to MAC.\n");
   msg ("TEMP OPENAIR : COMMENTED\n");
  #endif
  cmac_config_req (protocol_bs->rrc.rrc_UE_updating, 0);
}

//-------------------------------------------------------------------
void RRC_RG_O_O_Action (int action){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] New action will be requested from RRM : %d.\n", action);
  #endif
  protocol_bs->rrc.curr_config_action = (u8) action;
}

//-------------------------------------------------------------------
void RRC_RG_O_O_Compute_Configuration (int UE_Id){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] Request to compute configuration.\n");
  #endif
  protocol_bs->rrc.rrc_multicast_bearer = FALSE;
  rrc_rg_compute_configuration (UE_Id, protocol_bs->rrc.curr_config_action);
}

//-------------------------------------------------------------------
void RRC_RG_O_O_RB_failure_code(int error_cause){
//-------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC_RG][FSM-OUT] RB Establishment failure cause %d.\n", error_cause);
  #endif
  protocol_bs->rrc.curr_rb_failure_code = error_cause;
}

//Other functions, not related with Esterel
//-------------------------------------------------------------------
void RRC_RG_O_NAS_DATA_IND (int UE_Id){
//-------------------------------------------------------------------
  struct nas_rg_if_element *msgToBuild;
  char *pdata;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element) + RRC_NAS_MAX_SIZE);
  protocol_bs->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASDataInd);
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC][DATA-OUT] DATA_TRANSFER_IND, length #1 %d.\n", msgToBuild->prim_length);
  #endif
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = DATA_TRANSFER_IND;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.data_transfer_ind.localConnectionRef = protocol_bs->rrc.Mobile_List[UE_Id].local_connection_ref;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.data_transfer_ind.priority = protocol_bs->rrc.current_SFN;
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.data_transfer_ind.nasDataLength = protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth;
  pdata = (char *) &(msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.data_transfer_ind.nasDataLength);
  pdata += 2;
  memcpy (pdata, protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_ptr, protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth);
  #ifdef DEBUG_RRC_DETAILS
    rrc_print_buffer (pdata - 2, 20);
    //msg ("[RRC][DATA-OUT] DATA_TRANSFER_IND, length #1.0 %d.\n", msgToBuild->prim_length);
  #endif
  msgToBuild->prim_length += (int) protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth;
  #ifdef DEBUG_RRC_DETAILS
    //msg ("[RRC][DATA-OUT] DATA_TRANSFER_IND, length #1.1 %d.\n", protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth);
    msg ("[RRC][DATA-OUT] DATA_TRANSFER_IND, length #2 %d.\n", msgToBuild->prim_length);
  #endif
  #ifdef DEBUG_RRC_STATE
    msg ("[RRC][DATA-OUT] DATA_TRANSFER_IND primitive ready to send to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
}

//-------------------------------------------------------------------
void RRC_RG_O_NAS_ENB_MEASUREMENT_IND (void){
//-------------------------------------------------------------------

  int UE_Id = 0;
  int num_connected_UEs;
  int ix;
  struct nas_rg_if_element *msgToBuild;

  mem_block_t *p = get_free_mem_block (sizeof (struct nas_rg_if_element));
  protocol_bs->rrc.NASMessageToXmit = p;        // Temp - will later enqueue at bottom of list
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_rg_if_element *) p->data;

  msgToBuild->prim_length = NAS_TL_SIZE + sizeof (struct NASEnbMeasureInd);
  msgToBuild->xmit_fifo = protocol_bs->rrc.rrc_rg_DCOut_fifo[UE_Id];
  msgToBuild->nasRgPrimitive.dc_sap_prim.type = ENB_MEASUREMENT_IND;
  msgToBuild->nasRgPrimitive.dc_sap_prim.length = msgToBuild->prim_length;

  // comment or uncomment next lines according to test
  #ifdef RRC_ENABLE_REAL_ENB_MESURES
  num_connected_UEs = protocol_bs->rrc.num_connected_UEs;
  #else
  num_connected_UEs =2;
  #endif
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.eNBmeasurement_ind.cell_id = protocol_bs->rrc.rg_cell_id;
  // next values are temp hard coded, to be replaced by real values
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.eNBmeasurement_ind.num_UEs = num_connected_UEs;
  for (ix=0; ix<num_connected_UEs; ix++){
    msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.eNBmeasurement_ind.measures[ix].rlcBufferOccupancy = rrc_rg_ENbMeas_get_rlcBufferOccupancy(ix);
    msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.eNBmeasurement_ind.measures[ix].scheduledPRB = rrc_rg_ENbMeas_get_scheduledPRB(ix);
    msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.eNBmeasurement_ind.measures[ix].totalDataVolume = rrc_rg_ENbMeas_get_totalDataVolume(ix);
  }
  msgToBuild->nasRgPrimitive.dc_sap_prim.nasRGDCPrimitive.eNBmeasurement_ind.totalNumPRBs = rrc_rg_ENbMeas_get_totalNumPRBs();

  #ifdef DEBUG_RRC_STATE
  msg ("[RRC][DATA-OUT] ENB_MEASUREMENT_IND primitive ready to send to NAS, length %d.\n", msgToBuild->prim_length);
  #endif
}