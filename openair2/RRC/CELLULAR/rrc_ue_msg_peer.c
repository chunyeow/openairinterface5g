/***************************************************************************
                          rrc_ue_msg_peer.c  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform encoding/decoding of RRC messages in UE
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
#include "rrc_msg_class.h"
// A verifier
// #include "rrc_msg_pdus.h"
// #include "rrc_msg_constant.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_msg.h"
#include "rrc_proto_int.h"

/**************************************************************/
/*  Uplink CCCH messages - Encode                             */
/**************************************************************/
//-----------------------------------------------------------------------------
/*  RRC Connection Request                                    */
void rrc_ue_msg_connreq(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_CCCH_Message * ul_ccch_msg;
  mem_block_t *p;

  if (!protocol_ms->rrc.establishment_cause)
     protocol_ms->rrc.establishment_cause = registration;
  else
     protocol_ms->rrc.establishment_cause = callRe_establishment;
  protocol_ms->rrc.prot_error_indicator = noError;

  // prepare encoding
  *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
  protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RRCConnectionRequest);
   // Temp - Test/Messages
    //   ul_ccch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_ccch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_ccch_msg;
  p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
  ul_ccch_msg = (struct UL_CCCH_Message *)p->data;
   //
  protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
  protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_ccch_msg;
  // Encode
  ul_ccch_msg->integrityCheckInfo = 0; // No check with RRC Connection Request
  ul_ccch_msg->message.type = UL_CCCH_rrcConnectionRequest;
  status = rrc_PEREnc_RRCConnectionRequest((RRCConnectionRequest*) &(ul_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Connection Request - status : %d\n", status);
   msg("\n[RRC_MSG] Connection Request - length : %d\n", protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  Cell Update    */
void rrc_ue_msg_cellUpdate(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_CCCH_Message * ul_ccch_msg;
  mem_block_t *p;

  // Temp : Hard coded values
   protocol_ms->rrc.am_RLC_ErrorIndicationRb2_3or4 = FALSE;
   protocol_ms->rrc.am_RLC_ErrorIndicationRb5orAbove = FALSE;
   protocol_ms->rrc.cellUpdateCause = re_enteredServiceArea;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(CellUpdate);
   // Temp - Test/Messages
    //ul_ccch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //memset (ul_ccch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    // 
    //protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_ccch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_ccch_msg = (struct UL_CCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_ccch_msg;
  // Encode
   ul_ccch_msg->integrityCheckInfo = 123;
   ul_ccch_msg->message.type = UL_CCCH_cellUpdate;
   status = rrc_PEREnc_CellUpdate ((CellUpdate*) &(ul_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Cell Update - status : %d\n", status);
   msg("\n[RRC_MSG] Cell Update - length : %d\n", protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

/**************************************************************/
/*  Uplink DCCH messages - Encode                             */
/**************************************************************/
//-----------------------------------------------------------------------------
/*  RRC Connection Setup Complete      */
void rrc_ue_msg_connsucompl(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RRCConnectionSetupComplete);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
  // Encode
   ul_dcch_msg->integrityCheckInfo = 0; // No check with RRC Connection Setup Complete
   ul_dcch_msg->message.type = UL_DCCH_rrcConnectionSetupComplete;
   status = rrc_PEREnc_RRCConnectionSetupComplete((RRCConnectionSetupComplete*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Connection Setup Compl - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  RRC Connection Release MD      */
void rrc_ue_msg_connrelUL(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RRCConnectionRelease_UL);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_rrcConnectionReleaseUL;
   status = rrc_PEREnc_RRCConnectionRelease_UL((RRCConnectionRelease_UL*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Connection Release Uplink - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  InitialDirectTransfer      */
void rrc_ue_msg_initXfer(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(InitialDirectTransfer);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;

  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_initialDirectTransfer;
   status = rrc_PEREnc_InitialDirectTransfer((InitialDirectTransfer*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Initial Direct Transfer - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  Uplink Direct Transfer      */
void rrc_ue_msg_ulXfer(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(UplinkDirectTransfer);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;

  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_uplinkDirectTransfer;
   status = rrc_PEREnc_UplinkDirectTransfer((UplinkDirectTransfer*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Uplink Direct Transfer - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  Radio Bearer Setup Complete      */
void rrc_ue_msg_rbsetupcompl(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RadioBearerSetupComplete);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_radioBearerSetupComplete;
   status = rrc_PEREnc_RadioBearerSetupComplete((RadioBearerSetupComplete*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Setup Complete - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  Radio Bearer Setup Failure      */
void rrc_ue_msg_rbsetupfail(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RadioBearerSetupFailure);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_radioBearerSetupFailure;
   status = rrc_PEREnc_RadioBearerSetupFailure((RadioBearerSetupFailure*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Setup Failure - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  Radio Bearer Release Complete      */
void rrc_ue_msg_rbreleasecompl(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RadioBearerReleaseComplete);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_radioBearerReleaseComplete;
   status = rrc_PEREnc_RadioBearerReleaseComplete((RadioBearerReleaseComplete*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Release Complete - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

//-----------------------------------------------------------------------------
/*  Radio Bearer Release Failure      */
void rrc_ue_msg_rbreleasefail(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RadioBearerReleaseFailure);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //   //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_radioBearerReleaseFailure;
   status = rrc_PEREnc_RadioBearerReleaseFailure((RadioBearerReleaseFailure*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Release Failure - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

/*  Measurement Report   */
//-----------------------------------------------------------------------------
void rrc_ue_msg_measrep(int *Message_Id){
//-----------------------------------------------------------------------------
 int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(MeasurementReport);
   // Temp - Test/Messages
    //   ul_dcch_msg = malloc (protocol_ms->rrc.ue_msg_infos.msg_length);
    //   memset (ul_dcch_msg, 0, protocol_ms->rrc.ue_msg_infos.msg_length);
    //
    //   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;
   //
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;
  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_measurementReport;
   status = rrc_PEREnc_MeasurementReport((MeasurementReport*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Measurement Report - status : %d - length : %d\n", status, protocol_ms->rrc.ue_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

/**************************************************************/
/*  Downlink CCCH messages - Decode                           */
/**************************************************************/
/*  RRC Connection Setup  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_connsetup(DL_CCCH_Message * dl_ccch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

   status = rrc_PERDec_RRCConnectionSetup((RRCConnectionSetup*) &(dl_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] RRC Connection Setup Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d, Activation Time %d\n",
                    protocol_ms->rrc.rcved_trans.transaction_Id,
                    protocol_ms->rrc.nextActivationTime );
   msg("[RRC_MSG] U-RNTI %d, New_State %d\n",
                    protocol_ms->rrc.u_rnti,
                    protocol_ms->rrc.next_state );
  #endif
   return status;
}

/*  RRCConnectionReject  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_connreject(DL_CCCH_Message * dl_ccch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

   status = rrc_PERDec_RRCConnectionReject((RRCConnectionReject*) &(dl_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] RRC Connection Reject Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d, Wait Time %d\n",
                    protocol_ms->rrc.rcved_trans.transaction_Id,
                    protocol_ms->rrc.rejectWaitTime );
  #endif
   return status;
}

/*  CellUpdateConfirm_CCCH  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_cellupdatecnfccch(DL_CCCH_Message * dl_ccch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (dl_ccch_msg->integrityCheckInfo != 123)
    return P_INVINTEGRITY;

   status = rrc_PERDec_CellUpdateConfirm_CCCH((CellUpdateConfirm_CCCH*) &(dl_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] CellUpdateConfirm_CCCH Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d, Activation Time %d\n",
                    protocol_ms->rrc.rcved_trans.transaction_Id,
                    protocol_ms->rrc.nextActivationTime );
   msg("[RRC_MSG] U-RNTI %d, New_State %d\n",
                    protocol_ms->rrc.u_rnti,
                    protocol_ms->rrc.next_state );
   msg("[RRC_MSG] SRB2-3-4 %d, RB5+ %d\n",
                    protocol_ms->rrc.rlc_Re_establishIndicatorRb2_3or4,
                    protocol_ms->rrc.rlc_Re_establishIndicatorRb5orAbove );
  #endif
   return status;
}

/* UE Capability Information */
//-----------------------------------------------------------------------------
void  rrc_ue_msg_ueCapInfo(int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  UL_DCCH_Message * ul_dcch_msg;
  mem_block_t *p;

  // prepare encoding
   *Message_Id = ++ protocol_ms->rrc.ue_msg_infos.msg_Id;
   protocol_ms->rrc.ue_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(UECapabilityInformation);
   p = get_free_mem_block(protocol_ms->rrc.ue_msg_infos.msg_length);
   ul_dcch_msg = (struct UL_DCCH_Message *)p->data;

   //Reference to the memory block
   protocol_ms->rrc.ue_msg_infos.mem_block_ptr = p;
   protocol_ms->rrc.ue_msg_infos.msg_ptr = ul_dcch_msg;

  // Encode
   ul_dcch_msg->integrityCheckInfo = 123;
   ul_dcch_msg->message.type = UL_DCCH_ueCapabilityInformation;
   status = rrc_PEREnc_UECapabilityInformation ((UECapabilityInformation*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] UE Capability Information  - status : %d - length : %d\n",
                          status, protocol_ms->rrc.ue_msg_infos.msg_length);
   msg("[RRC_MSG] Kernel Release = %d, ", EURECOM_KERNEL_RELEASE_INDICATOR_DEFAULT);
   msg("Access Stratum Release = ");
   switch(ACCESS_STRATUM_RELEASE_INDICATOR_DEFAULT){
      case ACCESS_STRATUM_RELEASE_INDICATOR_REL_4: msg("REL-4\n"); break;
      case ACCESS_STRATUM_RELEASE_INDICATOR_REL_5: msg("REL-5\n"); break;
      case ACCESS_STRATUM_RELEASE_INDICATOR_REL_6: msg("REL-6\n"); break;
      default: msg("Unrecognized!\n");   	
   }

   rrc_print_buffer((char*)protocol_ms->rrc.ue_msg_infos.msg_ptr,protocol_ms->rrc.ue_msg_infos.msg_length);
  #endif
}

/**************************************************************/
/*  Downlink DCCH messages - Decode                           */
/**************************************************************/
/*  RadioBearerSetup  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_rbsetup(DL_DCCH_Message * dl_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (dl_dcch_msg->integrityCheckInfo != 123)
    return P_INVINTEGRITY;

   status = rrc_PERDec_RadioBearerSetup((RadioBearerSetup*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Setup Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d, Activation Time %d,",
                    protocol_ms->rrc.rcved_trans.transaction_Id,
                    protocol_ms->rrc.nextActivationTime );
   msg(" U-RNTI %d, New_State %d\n",
                    protocol_ms->rrc.u_rnti,
                    protocol_ms->rrc.next_state );
   msg("[RRC_MSG] RB information: ");
   msg("RB_Id: %d, Radio QoS: %d, IP DSCP: %d, SAP Id: %d. \n",
   protocol_ms->rrc.requested_rbId, protocol_ms->rrc.requested_QoSclass,
   protocol_ms->rrc.requested_dscp, protocol_ms->rrc.requested_sapid);
  #endif
   return status;
}

/*  RadioBearerRelease  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_rbrelease(DL_DCCH_Message * dl_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (dl_dcch_msg->integrityCheckInfo != 123)
    return P_INVINTEGRITY;

   status = rrc_PERDec_RadioBearerRelease((RadioBearerRelease*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Release Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d, Activation Time %d, U-RNTI %d, New_State %d\n",
                    protocol_ms->rrc.rcved_trans.transaction_Id,
                    protocol_ms->rrc.nextActivationTime,
                    protocol_ms->rrc.u_rnti,
                    protocol_ms->rrc.next_state);
   msg("[RRC_MSG] RB information : RB_Id : %d\n", protocol_ms->rrc.requested_rbId);
  #endif
   return status;
}

/*  DownlinkDirectTransfer  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_dlXfer(DL_DCCH_Message * dl_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (dl_dcch_msg->integrityCheckInfo != 123)
    return P_INVINTEGRITY;

   status = rrc_PERDec_DownlinkDirectTransfer((DownlinkDirectTransfer*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Downlink Direct Transfer Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d\n",
                    protocol_ms->rrc.rcved_trans.transaction_Id);
  #endif
   return status;
}

/*  PagingType2  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_pagingt2(DL_DCCH_Message * dl_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (dl_dcch_msg->integrityCheckInfo != 123)
    return P_INVINTEGRITY;

   status = rrc_PERDec_PagingType2((PagingType2*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Paging Type 2 Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d\n",
                    protocol_ms->rrc.rcved_trans.transaction_Id);
  #endif
   return status;
}

/*  MeasurementControl  */
//-----------------------------------------------------------------------------
int rrc_ue_msg_measctl(DL_DCCH_Message * dl_dcch_msg){
//-----------------------------------------------------------------------------

 int status = P_SUCCESS;

  if  (dl_dcch_msg->integrityCheckInfo != 123)
    return P_INVINTEGRITY;

   status = rrc_PERDec_MeasurementControl((MeasurementControl *) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Measurement Control message Received - status : %d\n", status);
   msg("\n[RRC_MSG] Transaction Identifier %d\n",
                    protocol_ms->rrc.rcved_trans.transaction_Id);
  #endif
   return (status);
}

// /*  Uplink Physical Channel Control  */
// //-----------------------------------------------------------------------------
// int rrc_ue_msg_ULPCHctl(DL_DCCH_Message * dl_dcch_msg){
// //-----------------------------------------------------------------------------
// 
//  int status = P_SUCCESS;
// 
//   if  (dl_dcch_msg->integrityCheckInfo != 123)
//     return P_INVINTEGRITY;
// 
//    status = rrc_PERDec_ULPCHControl((MeasurementControl *) &(dl_dcch_msg->message.content));
// 
//   #ifdef DEBUG_RRC_STATE
//    msg("\n[RRC_MSG] Uplink Physical Channel Control message Received - status : %d\n", status);
//    msg("\n[RRC_MSG] Transaction Identifier %d\n",
//                     protocol_ms->rrc.rcved_trans.transaction_Id);
//   #endif
//    return (status);
// }


/* UE Capability Information Confirm*/
//-----------------------------------------------------------------------------
int  rrc_ue_msg_ueCapInfoCnf(DL_DCCH_Message * dl_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (dl_dcch_msg->integrityCheckInfo != 123)
    return P_INVINTEGRITY;

   status = rrc_PERDec_UECapabilityInformationConfirm((UECapabilityInformationConfirm*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] UE Capability Information Confirm Received - status : %d\n", status);
  #endif
   return status;
}

