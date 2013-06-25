/***************************************************************************
                          rrc_rg_msg_peer.c  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to perform encoding/decoding of RRC messages in RG
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
#include "rrc_msg_class.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_msg.h"
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"

/**************************************************************/
/*  Uplink CCCH messages - Decode                             */
/**************************************************************/
/*  RRCConnectionRequest  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_connreq(int * pUE ,UL_CCCH_Message * ul_ccch_msg){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int UE_Id;

  if ((UE_Id = get_new_UE_Id(&(ul_ccch_msg->message.content.rrcConnectionRequest.imei))) == 999)
   return FAILURE;
  *pUE = UE_Id;

  status = rrc_PERDec_RRCConnectionRequest(UE_Id,(RRCConnectionRequest*) &(ul_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Connection Request Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}

/*  CellUpdate  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_cellUpdate(int* pUE ,UL_CCCH_Message * ul_ccch_msg){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int UE_Id;

  if  (ul_ccch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_CellUpdate(pUE,(CellUpdate*) &(ul_ccch_msg->message.content));
  if (status == SUCCESS)
     UE_Id = *pUE ;

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Cell Update Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}

/**************************************************************/
/*  Uplink DCCH messages - Decode                             */
/**************************************************************/
/*  RRCConnectionSetupComplete  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_connsucompl(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

   status = rrc_PERDec_RRCConnectionSetupComplete(UE_Id,(RRCConnectionSetupComplete*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Connection Setup Complete Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}

/*  RRCConnectionRelease_UL  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_connrelUL(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_RRCConnectionRelease_UL(UE_Id,(RRCConnectionRelease_UL*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] RRC Connection Release UL Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}

/*  InitialDirectTransfer  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_initXfer(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_InitialDirectTransfer(UE_Id,(InitialDirectTransfer*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Initial Direct Transfer Received - status : %d\n", status);
   //rrc_mt_list_print(UE_Id);
  #endif
  return status;
}
/*  UplinkDirectTransfer  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_ulXfer(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_UplinkDirectTransfer(UE_Id,(UplinkDirectTransfer*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Uplink Direct Transfer Received - status : %d\n", status);
   //rrc_mt_list_print(UE_Id);
  #endif
  return status;
}

/*  RadioBearerSetupComplete  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_rbsetupcompl(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_RadioBearerSetupComplete(UE_Id,(RadioBearerSetupComplete*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Setup Complete Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}
/*  RadioBearerSetupFailure  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_rbsetupfail(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_RadioBearerSetupFailure(UE_Id,(RadioBearerSetupFailure*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Setup Failure Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}
/*  RadioBearerReleaseComplete  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_rbreleasecompl(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_RadioBearerReleaseComplete(UE_Id,(RadioBearerReleaseComplete*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Release Complete Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}
/*  RadioBearerReleaseFailure  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_rbreleasefail(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_RadioBearerReleaseFailure(UE_Id,(RadioBearerReleaseFailure*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Release Failure Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}

/*  Measurement Report   */
//-----------------------------------------------------------------------------
int rrc_rg_msg_measrepr(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
    int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_MeasurementReport(UE_Id,(MeasurementReport*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Measurement Report Received - status : %d\n", status);
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}

/*  UE Capability Information  */
//-----------------------------------------------------------------------------
int rrc_rg_msg_ueCapInfo(int UE_Id ,UL_DCCH_Message * ul_dcch_msg){
//-----------------------------------------------------------------------------
   int status = P_SUCCESS;

  if  (ul_dcch_msg->integrityCheckInfo != 123)
   return P_INVINTEGRITY;

  status = rrc_PERDec_UECapabilityInformation(UE_Id,(UECapabilityInformation*) &(ul_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] UE Capability Information Received - status : %d\n", status);
   msg("[RRC_MSG] UE_Id = %d, Kernel Release = %d, ", UE_Id, protocol_bs->rrc.Mobile_List[UE_Id].ind_eurecomKernelRelease);
   msg("Access Stratum Release = ");
   switch(protocol_bs->rrc.Mobile_List[UE_Id].ind_accessStratumRelease){
     case ACCESS_STRATUM_RELEASE_INDICATOR_REL_4: msg("REL-4\n"); break;
     case ACCESS_STRATUM_RELEASE_INDICATOR_REL_5: msg("REL-5\n"); break;
     case ACCESS_STRATUM_RELEASE_INDICATOR_REL_6: msg("REL-6\n"); break;
     default: msg("Unrecoginized!\n");   	
   }
   rrc_mt_list_print(UE_Id);
  #endif
  return status;
}


/**************************************************************/
/*  Downlink CCCH messages - Encode                           */
/**************************************************************/
/*  RRCConnectionSetup  */
//-----------------------------------------------------------------------------
void rrc_rg_msg_connsetup(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_CCCH_Message * dl_ccch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_CCCH_rrcConnectionSetup;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
            (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  // *Message_Id = ++ protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_Id;
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length =
                MSG_HEAD_LGTH + sizeof(RRCConnectionSetup);
   // Temp - Test/Messages
  //   dl_ccch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  //   memset (dl_ccch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_ccch_msg = (struct DL_CCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_ccch_msg;

  // Encode
  dl_ccch_msg->integrityCheckInfo = 0; // No check with RRC Connection Setup
  dl_ccch_msg->message.type = DL_CCCH_rrcConnectionSetup;
  status = rrc_PEREnc_RRCConnectionSetup(UE_Id,(RRCConnectionSetup*) &(dl_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] RRC Connection Setup - status : %d - length : %d\n",
          status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}

/*  RRCConnectionReject */
//-----------------------------------------------------------------------------
void rrc_rg_msg_connreject(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_CCCH_Message * dl_ccch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_CCCH_rrcConnectionReject;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
         (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  // *Message_Id = ++ protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_Id;
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(RRCConnectionReject);
   // Temp - Test/Messages
  //   dl_ccch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  //   memset (dl_ccch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_ccch_msg = (struct DL_CCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_ccch_msg;
 // Encode
  dl_ccch_msg->integrityCheckInfo = 0; // No check with RRC Connection Reject
  dl_ccch_msg->message.type = DL_CCCH_rrcConnectionReject;
  status = rrc_PEREnc_RRCConnectionReject(UE_Id,(RRCConnectionReject*) &(dl_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] RRC Connection Reject - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}

/*  CellUpdateConfirm_CCCH  */
//-----------------------------------------------------------------------------
void rrc_rg_msg_cellupdatecnfccch(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_CCCH_Message * dl_ccch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_CCCH_cellUpdateConfirm;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
            (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  //   *Message_Id = ++ protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_Id;
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(CellUpdateConfirm_CCCH);
   // Temp - Test/Messages
  //   dl_ccch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  //   memset (dl_ccch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_ccch_msg = (struct DL_CCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_ccch_msg;

  // Encode
  dl_ccch_msg->integrityCheckInfo = 123;
  dl_ccch_msg->message.type = DL_CCCH_cellUpdateConfirm;
  status = rrc_PEREnc_CellUpdateConfirm_CCCH(UE_Id,(CellUpdateConfirm_CCCH*) &(dl_ccch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Cell Update Confirm (CCCH) - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}


/**************************************************************/
/*  Downlink DCCH messages - Encode                           */
/**************************************************************/
/*  RadioBearerSetup  */
//-----------------------------------------------------------------------------
void rrc_rg_msg_rbsetup(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_DCCH_Message * dl_dcch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_DCCH_radioBearerSetup;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
         (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  // *Message_Id = ++ protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_Id;
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length =
                MSG_HEAD_LGTH + sizeof(RadioBearerSetup);
  // Temp - Test/Messages
  //   dl_dcch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  //   memset (dl_dcch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_dcch_msg = (struct DL_DCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_dcch_msg;

  // Encode
  dl_dcch_msg->integrityCheckInfo = 123;
  dl_dcch_msg->message.type = DL_DCCH_radioBearerSetup;
  status = rrc_PEREnc_RadioBearerSetup(UE_Id,(RadioBearerSetup*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Setup - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}

/*  RadioBearerRelease   */
//-----------------------------------------------------------------------------
void rrc_rg_msg_rbrelease(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_DCCH_Message * dl_dcch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_DCCH_radioBearerRelease;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
            (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  //   *Message_Id = ++ protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_Id;
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length =
                MSG_HEAD_LGTH + sizeof(RadioBearerRelease);
   // Temp - Test/Messages
  //   dl_dcch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  //   memset (dl_dcch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_dcch_msg = (struct DL_DCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_dcch_msg;

  // Encode
  dl_dcch_msg->integrityCheckInfo = 123;
  dl_dcch_msg->message.type = DL_DCCH_radioBearerRelease;
  status = rrc_PEREnc_RadioBearerRelease(UE_Id,(RadioBearerRelease*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Radio Bearer Release - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}

/*  DownlinkDirectTransfer */
//-----------------------------------------------------------------------------
void rrc_rg_msg_dlXfer(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_DCCH_Message * dl_dcch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_DCCH_downlinkDirectTransfer;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
           (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  // *Message_Id = ++ protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_Id;
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length =
               MSG_HEAD_LGTH + sizeof(DownlinkDirectTransfer);
  // Temp - Test/Messages
  //   dl_dcch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  //   memset (dl_dcch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_dcch_msg = (struct DL_DCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_dcch_msg;

  // Encode
  dl_dcch_msg->integrityCheckInfo = 123;
  dl_dcch_msg->message.type = DL_DCCH_downlinkDirectTransfer;
  status = rrc_PEREnc_DownlinkDirectTransfer(UE_Id,(DownlinkDirectTransfer*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Downlink Direct Transfer - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}

/*  PagingType2   */
//-----------------------------------------------------------------------------
void rrc_rg_msg_pagingt2(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_DCCH_Message * dl_dcch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_DCCH_pagingType2;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
            (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  //   *Message_Id = ++ protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_Id;
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length =
               MSG_HEAD_LGTH + sizeof(PagingType2);
  // Temp - Test/Messages
  // dl_dcch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  // memset (dl_dcch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_dcch_msg = (struct DL_DCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_dcch_msg;

  // Encode
  dl_dcch_msg->integrityCheckInfo = 123;
  dl_dcch_msg->message.type = DL_DCCH_pagingType2;
  status = rrc_PEREnc_PagingType2(UE_Id,(PagingType2*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Paging Type 2 - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}

/*  Measurement Control   */
//-----------------------------------------------------------------------------
void rrc_rg_msg_measctl(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_DCCH_Message * dl_dcch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_DCCH_measurementControl;
  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
            (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;
  //
  // prepare encoding
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(MeasurementControl);
   // Temp - Test/Messages
  //   dl_dcch_msg = malloc (protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  //   memset (dl_dcch_msg, 0, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_dcch_msg = (struct DL_DCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_dcch_msg;

  // Encode
  dl_dcch_msg->integrityCheckInfo = 123;
  dl_dcch_msg->message.type = DL_DCCH_measurementControl;
  status = rrc_PEREnc_MeasurementControl(UE_Id,(MeasurementControl*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Measurement Control - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}


/*  UE Capability Information Confirm */
//-----------------------------------------------------------------------------
void rrc_rg_msg_ueCapInfoCnf(int UE_Id, int *Message_Id){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  DL_DCCH_Message * dl_dcch_msg;
  mem_block_t *p;

  protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].msg_type = DL_DCCH_measurementControl;
  // TODO
  //protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id =
  //         (protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id +1) %4;

  // prepare encoding
  *Message_Id = UE_Id;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = MSG_HEAD_LGTH + sizeof(UECapabilityInformationConfirm);
  //Allocation
  p = get_free_mem_block(protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  dl_dcch_msg = (struct DL_DCCH_Message *)p->data;
  //
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.mem_block_ptr = p;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr = dl_dcch_msg;

  // Encode
  dl_dcch_msg->integrityCheckInfo = 123;
  dl_dcch_msg->message.type =  DL_DCCH_ueCapabilityInformationConfirm;
  status = rrc_PEREnc_UECapabilityInformationConfirm(UE_Id, (UECapabilityInformationConfirm*) &(dl_dcch_msg->message.content));

  #ifdef DEBUG_RRC_STATE
   msg("\n[RRC_MSG] Measurement Control - status : %d - length : %d\n",
                    status, protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
   rrc_print_buffer((char*)protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_ptr,protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
  #endif
}

