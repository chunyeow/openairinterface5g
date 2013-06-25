/***************************************************************************
                        rrc_msg_pdus.c  -  description
                        -------------------
  begin                : Jan 6, 2003
  copyright            : (C) 2003, 2010 by Eurecom
  email                : Michelle.Wetterwald@eurecom.fr
  **************************************************************************
  Procedures to perform encoding of RRC peer-to-peer messages
  ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#ifdef NODE_MT
#include "rrc_ue_vars.h"
#endif
#ifdef NODE_RG
#include "rrc_rg_vars.h"
#endif
#include "rrc_msg_class.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_msg.h"
#include "rrc_proto_int.h"
//-----------------------------------------------------------------------------

//Temp
u8 new_conf[] = "666666666677777777770000000000888888888899999999991212121212121212121234343434343434343434";

/**************************************************************/
/*  RRCConnectionRequest                                      */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_RRCConnectionRequest (RRCConnectionRequest * pvalue){
  int status = P_SUCCESS;
  int i;

  /* encode initialUE_Identity = IMEI */
  pvalue->imei.numDigits = 14;  // limited version of IMEI is used
  for (i = 0; i < pvalue->imei.numDigits; i++)
    pvalue->imei.digit[i] = protocol_ms->rrc.IMEI[i];

  /* encode establishmentCause */
  pvalue->establishmentCause = protocol_ms->rrc.establishment_cause;

  /* encode protocolErrorIndicator */
  pvalue->protocolErrorIndicator = protocol_ms->rrc.prot_error_indicator;

  /* encode measuredResultsOnRACH */
  // status = rrc_PEREnc_MeasuredResultsOnRACH (&pvalue->measuredResultsOnRACH);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_RRCConnectionRequest (int UE_Id, RRCConnectionRequest * pvalue){
  int status = P_SUCCESS;

  /* decode initial UE_Identity = IMEI */
  if (pvalue->imei.numDigits > 15)
    return P_OUTOFBOUNDS;
  memcpy (protocol_bs->rrc.Mobile_List[UE_Id].IMEI, pvalue->imei.digit, 14);

  /* decode establishmentCause */
  protocol_bs->rrc.Mobile_List[UE_Id].establishment_cause = pvalue->establishmentCause;
  //    #ifdef DEBUG_RRC_STATE
  //    msg("\n[RRC_RG] Establishment Cause %d \n", pvalue->establishmentCause );
  //    #endif

  /* decode protocolErrorIndicator */
  protocol_bs->rrc.Mobile_List[UE_Id].prot_error_indicator = pvalue->protocolErrorIndicator;

  /* decode measuredResultsOnRACH */
  //  status = rrc_PERDec_MeasuredResultsOnRACH (&pvalue->measuredResultsOnRACH);

  return status;
}
#endif

/**************************************************************/
/*  RRCConnectionSetup                                        */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_RRCConnectionSetup (int UE_Id, RRCConnectionSetup * pvalue){
  int status = P_SUCCESS;
  int i;
  char *pwrite;
  int buff_length;
  //   char xor_result[sizeof(MT_CONFIG)];
  //   char compression_result[sizeof(MT_CONFIG)];

  /* encode initialUE_Identity = IMEI */
  pvalue->imei.numDigits = 14;  // limited version of IMEI is used
  for (i = 0; i < pvalue->imei.numDigits; i++)
    pvalue->imei.digit[i] = protocol_bs->rrc.Mobile_List[UE_Id].IMEI[i];

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode activationTime */
  pvalue->activationTime = 0;   //default value : now = absence

  /* encode new_U_RNTI */
  pvalue->new_U_RNTI = UE_Id;
  /* encode new_c_RNTI -- FFS */

  /* encode rrc_stateIndicator */
  pvalue->rrc_StateIndicator = cell_DCH;        // Temp hard coded

  /* encode qos class and dscp code */
  //   memcpy(pvalue->qos_Classes, protocol_bs->rrc.Mobile_List[UE_Id].qos_classes,MAXURAB) ;
  //   memcpy(pvalue->ip_dscp_Codes, protocol_bs->rrc.Mobile_List[UE_Id].dscp_codes,MAXURAB) ;

  // TODO: Remplacer buffers par config
  /* encode ue_Configuration */
  //   rrc_compress_buffer(new_conf,90, new_conf, &buff_length);
  //   rrc_xor_config((char*)&(rrm_config->mt_config[UE_Id]),
  //                  (char*)&(protocol_bs->rrc.saved_configuration.mt_config[UE_Id]), xor_result);
  //   rrc_compress_buffer(xor_result,sizeof(MT_CONFIG), compression_result, &buff_length);

  pwrite = (char *) &(pvalue->ue_Configuration.numoctets);
  rrc_compress_config ((MT_CONFIG *) &(rrm_config->mt_config[UE_Id]), &pwrite[2], &buff_length);

  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;

  *(u16 *) pwrite = buff_length;
  pwrite += 2;
  //   memcpy(pwrite,new_conf, buff_length);
  //   memcpy(pwrite,compression_result, buff_length);

#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] rrc_PEREnc_RRCConnectionSetup - Buffer length # 1 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length += buff_length + 2;
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] rrc_PEREnc_RRCConnectionSetup - Buffer length # 2 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_RRCConnectionSetup (RRCConnectionSetup * pvalue){
  int status = P_SUCCESS;
  int buff_length;
  char *buff_ptr;
  //   char compression_result[sizeof(MT_CONFIG)];

  /* decode initial UE_Identity = IMEI */
  if (pvalue->imei.numDigits > 15)
    return P_OUTOFBOUNDS;
  //   if (status = (strncmp((char *)protocol_ms->rrc.IMEI, (char *)pvalue->imei.digit,14)))
  status = memcmp ((char *) protocol_ms->rrc.IMEI, (char *) pvalue->imei.digit, 14);
  if (status) {
    msg ("\n[RRC_MSG] IMEI : --%s--, Received : --%s--, status %d\n", (char *) protocol_ms->rrc.IMEI, (char *) pvalue->imei.digit, status);
    return P_INVVALUE;          // ignore message
  }
  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_CCCH_rrcConnectionSetup;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;
  //Temp - Hard coded
  protocol_ms->rrc.accepted_trans[0].transaction_Id = pvalue->rrc_TransactionIdentifier;

  /* decode activationTime */
  protocol_ms->rrc.nextActivationTime = pvalue->activationTime;

  /* decode new_U_RNTI */
  protocol_ms->rrc.u_rnti = pvalue->new_U_RNTI;
  /* decode new_c_RNTI -- FFS */
  protocol_ms->rrc.c_rnti = pvalue->new_U_RNTI;

  /* decode rrc_stateIndicator */
  switch (pvalue->rrc_StateIndicator) {
      case cell_DCH:
        protocol_ms->rrc.next_state = CELL_DCH;
        break;
      case cell_FACH:
        protocol_ms->rrc.next_state = CELL_FACH;
        break;
      case cell_PCH:
        protocol_ms->rrc.next_state = CELL_PCH;
        break;
      default:                 //ura_PCH not supported
        return P_INVVALUE;
  }

  /* decode qos class and dscp code */
  //   memcpy( protocol_ms->rrc.qos_classes,pvalue->qos_Classes,MAXURAB) ;
  //   memcpy( protocol_ms->rrc.dscp_codes,pvalue->ip_dscp_Codes,MAXURAB) ;
  //#ifdef DEBUG_RRC_STATE
  //   msg("\n[RRC_MSG] QOS Classes and DSCP codes : \n");
  //   rrc_print_buffer((char*)&(protocol_ms->rrc.qos_classes), MAXURAB);
  //   rrc_print_buffer((char*)&(protocol_ms->rrc.dscp_codes), MAXURAB);
  //#endif

  /* decode ue_Configuration */
  buff_length = (u16) pvalue->ue_Configuration.numoctets;
  buff_ptr = (char *) (&(pvalue->ue_Configuration.numoctets));
  buff_ptr += 2;
  //   rrc_uncompress_buffer(buff_ptr,(u16)pvalue->ue_Configuration.numoctets, buff_ptr, &buff_length);
  //#ifdef DEBUG_RRC_STATE
  //   msg("\n[RRC_MSG] New Configuration : \n");
  //   rrc_print_buffer(buff_ptr, buff_length);
  //#endif

  //   rrc_uncompress_buffer(buff_ptr,(u16)pvalue->ue_Configuration.numoctets, compression_result, &buff_length);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Starting the uncompression of the configuration.- Conn Setup\n");
#endif
  rrc_uncompress_config ((MT_CONFIG *) &(rrm_config->mt_config), buff_ptr);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Length of New Configuration : %d \n", buff_length);
#endif

  //   rrc_xor_config(compression_result,(char*) &(protocol_ms->rrc.saved_configuration.mt_config),
  //                    (char*)&(rrm_config->mt_config));

  return status;
}
#endif

/**************************************************************/
/*  RRCConnectionSetupComplete                                */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_RRCConnectionSetupComplete (RRCConnectionSetupComplete * pvalue){
  int status = P_SUCCESS;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_ms->rrc.accepted_trans[0].transaction_Id;

  /* encode ue_RadioAccessCapability */
  pvalue->ue_RadioAccessCapability = eurecom_rrc;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_RRCConnectionSetupComplete (int UE_Id, RRCConnectionSetupComplete * pvalue){
  int status = P_SUCCESS;

  /* decode rrc_TransactionIdentifier */
  if (pvalue->rrc_TransactionIdentifier != protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id) {
    //      return P_INVVALUE;
    msg ("\n[RRC_MSG] Transaction Id : --%d--, Received : --%d--, status %d\n", protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id, pvalue->rrc_TransactionIdentifier, status);
    return P_INVVALUE;          // ignore message
  }

  /* decode ue_RadioAccessCapability */
  if (pvalue->ue_RadioAccessCapability != eurecom_rrc) {
    //      return P_INVVALUE;
    msg ("\n[RRC_MSG] Capability : --%d--, Received : --%d--, status %d\n", eurecom_rrc, pvalue->ue_RadioAccessCapability, status);
    return P_INVVALUE;          // ignore message
  }

  return status;
}
#endif

/**************************************************************/
/*  RRCConnectionReject                                       */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_RRCConnectionReject (int UE_Id, RRCConnectionReject * pvalue){
  int status = P_SUCCESS;
  int i;

  /* encode initialUE_Identity = IMEI */
  pvalue->imei.numDigits = 14;  // limited version of IMEI is used
  for (i = 0; i < pvalue->imei.numDigits; i++)
    pvalue->imei.digit[i] = protocol_bs->rrc.Mobile_List[UE_Id].IMEI[i];

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode waitTime */
  pvalue->waitTime = 0;         //Temp- Timer to be implemented

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_RRCConnectionReject (RRCConnectionReject * pvalue){
  int status = P_SUCCESS;

  /* decode initial UE_Identity = IMEI */
  if (pvalue->imei.numDigits > 15)
    return P_OUTOFBOUNDS;
  //   if (status = (strncmp((char *)protocol_ms->rrc.IMEI, (char *)pvalue->imei.digit,14)))
  status = memcmp ((char *) protocol_ms->rrc.IMEI, (char *) pvalue->imei.digit, 14);
  if (status) {
    msg ("\n[RRC_MSG] IMEI : --%s--, Received : --%s--, status %d\n", (char *) protocol_ms->rrc.IMEI, (char *) pvalue->imei.digit, status);
    return P_INVVALUE;          // ignore message
  }

  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_CCCH_rrcConnectionReject;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;

  /* decode wait Time */
  protocol_ms->rrc.rejectWaitTime = pvalue->waitTime;

  return status;
}
#endif

/**************************************************************/
/*  RRCConnectionRelease_UL                                   */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_RRCConnectionRelease_UL (RRCConnectionRelease_UL * pvalue){
  int   status = P_SUCCESS;

  /* encode u_RNTI */
  pvalue->u_RNTI = protocol_ms->rrc.u_rnti;

  /* encode releaseCause */// Temp hard coded
  pvalue->releaseCause = userInactivity;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_RRCConnectionRelease_UL (int UE_Id, RRCConnectionRelease_UL * pvalue){
  int   status = P_SUCCESS;



  /* decode and check u-RNTI */
  if (pvalue->u_RNTI != protocol_bs->rrc.Mobile_List[UE_Id].u_rnti) {
    msg ("\n[RRC_MSG] u-RNTI : --%d--, Received : --%d--, status: Invalid u-rnti \n", protocol_bs->rrc.Mobile_List[UE_Id].u_rnti, pvalue->u_RNTI);
    return P_INVVALUE;          // ignore message
  }

  /* decode releaseCause */
  protocol_bs->rrc.Mobile_List[UE_Id].release_cause = pvalue->releaseCause;

  return status;
}
#endif

/**************************************************************/
/*  InitialDirectTransfer                                     */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_InitialDirectTransfer (InitialDirectTransfer * pvalue){
  int status = P_SUCCESS;
  char *pwrite;
  int data_length;

  /* encode nas_Message */
  pwrite = (char *) &(pvalue->nas_Message.numoctets);
  protocol_ms->rrc.ue_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;
  //data_length = strlen(ul_data);
  data_length = protocol_ms->rrc.ul_nas_message_lgth;
  *(u16 *) pwrite = data_length;
  pwrite += 2;
  //memcpy(pwrite,ul_data, data_length);
  memcpy (pwrite, (protocol_ms->rrc.ul_nas_message_ptr)->data, data_length);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length #1 : %d\n", protocol_ms->rrc.ue_msg_infos.msg_length);
#endif
  protocol_ms->rrc.ue_msg_infos.msg_length += (data_length + 2);

  /* encode measuredResultsOnRACH */
  // status = rrc_PEREnc_MeasuredResultsOnRACH (&pvalue->measuredResultsOnRACH);

  return status;

}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_InitialDirectTransfer (int UE_Id, InitialDirectTransfer * pvalue){
  int status = P_SUCCESS;
//  char *buff_ptr;
  u8 *buff_ptr;

  // TODO: Remplacer buffers par nas_Message
  /* decode nas_Message */
  buff_ptr = (u8*) &(pvalue->nas_Message.numoctets);
  buff_ptr += 2;
  protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_ptr = buff_ptr;

  protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth = (u16) pvalue->nas_Message.numoctets;
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Initial Direct Transfer Received : \n");
  rrc_print_buffer ((char *)protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_ptr, protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth);
  msg ("\n[RRC_MSG] Data content --%s-- \n", protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_ptr);
#endif

  /* decode measuredResultsOnRACH */
  //  status = rrc_PERDec_MeasuredResultsOnRACH (&pvalue->measuredResultsOnRACH);

  return status;
}
#endif

/**************************************************************/
/*  UplinkDirectTransfer                                      */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_UplinkDirectTransfer (UplinkDirectTransfer * pvalue){
  int status = P_SUCCESS;
  char *pwrite;
  int data_length;

  /* encode nas_Message */
  pwrite = (char *) &(pvalue->nas_Message.numoctets);

  protocol_ms->rrc.ue_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;
  //data_length = strlen(ul_data);
  data_length = protocol_ms->rrc.ul_nas_message_lgth;
  *(u16 *) pwrite = data_length;
  pwrite += 2;
  //memcpy(pwrite,ul_data, data_length);
  memcpy (pwrite, (protocol_ms->rrc.ul_nas_message_ptr)->data, data_length);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length - 1 : %d\n", protocol_ms->rrc.ue_msg_infos.msg_length);
#endif
  protocol_ms->rrc.ue_msg_infos.msg_length += (data_length + 2);

  /* encode measuredResultsOnRACH */
  // status = rrc_PEREnc_MeasuredResultsOnRACH (&pvalue->measuredResultsOnRACH);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG

int rrc_PERDec_UplinkDirectTransfer (int UE_Id, UplinkDirectTransfer * pvalue){
  int status = P_SUCCESS;
  u8 *buff_ptr;

  // TODO: Remplacer buffers par nas_Message
  /* decode nas_Message */
  buff_ptr = (u8 *) (&(pvalue->nas_Message.numoctets));
  buff_ptr += 2;
  protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_ptr = buff_ptr;
  protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth = (u16) pvalue->nas_Message.numoctets;
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] New Uplink Direct Transfer Received: \n");
  rrc_print_buffer ((char *)protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_ptr, protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_lgth);
  msg ("\n[RRC_MSG] Data content --%s-- \n", protocol_bs->rrc.Mobile_List[UE_Id].ul_nas_message_ptr);
#endif

  /* decode measuredResultsOnRACH */
  //  status = rrc_PERDec_MeasuredResultsOnRACH (&pvalue->measuredResultsOnRACH);

  return status;
}
#endif

/**************************************************************/
/*  DownlinkDirectTransfer                                    */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_DownlinkDirectTransfer (int UE_Id, DownlinkDirectTransfer * pvalue){
  int status = P_SUCCESS;
  char *pwrite;
  int data_length;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode nas_Message */
  pwrite = (char *) &(pvalue->nas_Message.numoctets);
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;
  //data_length = strlen(dl_data);
  data_length = protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_lgth;
  *(u16 *) pwrite = data_length;

  pwrite += 2;
  //memcpy(pwrite,dl_data, data_length);
  memcpy (pwrite, (protocol_bs->rrc.Mobile_List[UE_Id].dl_nas_message_ptr)->data, data_length);

#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length - 1 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length += (data_length + 2);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_DownlinkDirectTransfer (DownlinkDirectTransfer * pvalue){
  int status = P_SUCCESS;
  char *buff_ptr;

  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_DCCH_downlinkDirectTransfer;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;

  // TODO: Remplacer buffers par nas_Message
  /* decode nas_Message */
  buff_ptr = (char *) (&(pvalue->nas_Message.numoctets));
  buff_ptr += 2;

  protocol_ms->rrc.dl_nas_message_ptr = buff_ptr;
  protocol_ms->rrc.dl_nas_message_lgth = (u16) pvalue->nas_Message.numoctets;
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] New Downlink Direct Transfer : \n");
  rrc_print_buffer (protocol_ms->rrc.dl_nas_message_ptr, protocol_ms->rrc.dl_nas_message_lgth);
  msg ("\n[RRC_MSG] Data content --%s-- \n", protocol_ms->rrc.dl_nas_message_ptr);
#endif

  return status;
}
#endif

/**************************************************************/
/*  RadioBearerSetup                                          */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_RadioBearerSetup (int UE_Id, RadioBearerSetup * pvalue){
  int status = P_SUCCESS;
  char *pwrite;
  int buff_length;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode activationTime */
  pvalue->activationTime = 0;   //default value : now = absence

  /* encode new_U_RNTI */
  pvalue->new_U_RNTI = UE_Id;
  /* encode new_c_RNTI -- FFS */

  /* encode rrc_stateIndicator */
  pvalue->rrc_StateIndicator = cell_DCH;        // Temp hard coded

  /* encode RB information */
  pvalue->rab_InformationSetupList.numrabs = 1; // Temp hard coded
  pvalue->rab_InformationSetupList.rbinfo[0].rb_identity = protocol_bs->rrc.Mobile_List[UE_Id].requested_MTrbId;
  pvalue->rab_InformationSetupList.rbinfo[0].qos_class = protocol_bs->rrc.Mobile_List[UE_Id].requested_QoSclass;
  pvalue->rab_InformationSetupList.rbinfo[0].ip_dscp_code = protocol_bs->rrc.Mobile_List[UE_Id].requested_dscp;
  pvalue->rab_InformationSetupList.rbinfo[0].sap_id = protocol_bs->rrc.Mobile_List[UE_Id].requested_sapid;

  /* encode ue_Configuration */
  //   rrc_compress_buffer(new_conf,90, new_conf, &buff_length);
  //
  //   buff_length=strlen(new_conf);
  //   *(u16*)pwrite = buff_length ;
  //   pwrite +=2;
  //   memcpy(pwrite,new_conf, buff_length);
  pwrite = (char *) &(pvalue->ue_Configuration.numoctets);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Starting the compression of the configuration.- RB Setup\n");
#endif
  rrc_compress_config ((MT_CONFIG *) &(rrm_config->mt_config[UE_Id]), &pwrite[2], &buff_length);

  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;

  *(u16 *) pwrite = buff_length;
  pwrite += 2;

#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length # 1 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length += buff_length + 2;
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length # 2 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif

  return status;
}

#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int  rrc_PERDec_RadioBearerSetup (RadioBearerSetup * pvalue){
  int status = P_SUCCESS;
  int buff_length;
  int rb_id = 0;
  char *buff_ptr;

#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Radio Bearer Setup: ");
  rrc_print_buffer ((char *) pvalue, 510);
#endif

  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_DCCH_radioBearerSetup;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;
  //Temp - Hard coded
  protocol_ms->rrc.accepted_trans[0].transaction_Id = pvalue->rrc_TransactionIdentifier;

  /* decode activationTime */
  protocol_ms->rrc.nextActivationTime = pvalue->activationTime;

  /* decode new_U_RNTI */
  protocol_ms->rrc.u_rnti = pvalue->new_U_RNTI;

  /* decode new_c_RNTI -- FFS */
  protocol_ms->rrc.c_rnti = pvalue->new_U_RNTI;

  /* decode rrc_stateIndicator */
  switch (pvalue->rrc_StateIndicator) {
      case cell_DCH:
        protocol_ms->rrc.next_state = CELL_DCH;
        break;
      case cell_FACH:
        protocol_ms->rrc.next_state = CELL_FACH;
        break;
      case cell_PCH:
        protocol_ms->rrc.next_state = CELL_PCH;
        break;
      default:                 //ura_PCH not supported
        return P_INVVALUE;
  }

  /* decode qos class and dscp code */
  if (pvalue->rab_InformationSetupList.numrabs > 1)
    msg ("\n[RRC_MSG] Number of rabs to setup not supported\n");
  protocol_ms->rrc.requested_rbId = pvalue->rab_InformationSetupList.rbinfo[0].rb_identity;
  protocol_ms->rrc.requested_QoSclass = pvalue->rab_InformationSetupList.rbinfo[0].qos_class;
  protocol_ms->rrc.requested_dscp = pvalue->rab_InformationSetupList.rbinfo[0].ip_dscp_code;
  protocol_ms->rrc.requested_sapid = pvalue->rab_InformationSetupList.rbinfo[0].sap_id;
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Number of Radio Bearers : %d \n", pvalue->rab_InformationSetupList.numrabs);
#endif
  rb_id = protocol_ms->rrc.requested_rbId;

  protocol_ms->rrc.ue_established_rbs[rb_id].rb_identity = protocol_ms->rrc.requested_rbId;
  protocol_ms->rrc.ue_established_rbs[rb_id].qos_class = protocol_ms->rrc.requested_QoSclass;
  protocol_ms->rrc.ue_established_rbs[rb_id].dscp_code = protocol_ms->rrc.requested_dscp;
  protocol_ms->rrc.ue_established_rbs[rb_id].sap_id = protocol_ms->rrc.requested_sapid;
  protocol_ms->rrc.ue_established_rbs[rb_id].rb_started = RB_STOPPED;

  /* decode ue_Configuration */
  //
  buff_length = (u16) pvalue->ue_Configuration.numoctets;
  buff_ptr = (char *) (&(pvalue->ue_Configuration.numoctets));
  buff_ptr += 2;
  //#ifdef DEBUG_RRC_STATE
  //   msg("\n[RRC_MSG] New Configuration , length %d : \n", buff_length);
  //   rrc_print_buffer(buff_ptr, buff_length);
  //#endif

  rrc_uncompress_config ((MT_CONFIG *) &(rrm_config->mt_config), buff_ptr);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Length of New Configuration : %d \n", buff_length);
#endif

  return status;
}
#endif

/**************************************************************/
/*  RadioBearerRelease                                        */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_RadioBearerRelease (int UE_Id, RadioBearerRelease * pvalue){
  int status = P_SUCCESS;

  char *pwrite;
  int buff_length;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode activationTime */
  pvalue->activationTime = 0;   //default value : now = absence

  /* encode new_U_RNTI */
  pvalue->new_U_RNTI = UE_Id;
  /* encode new_c_RNTI -- FFS */

  /* encode rrc_stateIndicator */
  pvalue->rrc_StateIndicator = cell_DCH;        // Temp hard coded

  /* encode rb_InformationReleaseList  */
  pvalue->rb_InformationReleaseList.numrabs = 1;
  pvalue->rb_InformationReleaseList.rbid[0] = protocol_bs->rrc.Mobile_List[UE_Id].requested_MTrbId;

  /* encode ue_Configuration */
  //   rrc_compress_buffer(new_conf,90, new_conf, &buff_length);
  //
  //   pwrite = (char *)  &(pvalue->ue_Configuration.numoctets);
  //   protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *)pvalue + MSG_HEAD_LGTH;
  //
  //   *(u16*)pwrite = buff_length ;
  //   pwrite +=2;
  //   memcpy(pwrite,new_conf, buff_length);
  //
  pwrite = (char *) &(pvalue->ue_Configuration.numoctets);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Starting the compression of the configuration.- RB Release\n");
#endif
  rrc_compress_config ((MT_CONFIG *) &(rrm_config->mt_config[UE_Id]), &pwrite[2], &buff_length);

  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;

  *(u16 *) pwrite = buff_length;
  pwrite += 2;

#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length # 1 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length += buff_length + 2;
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length # 2 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_RadioBearerRelease (RadioBearerRelease * pvalue){
  int status = P_SUCCESS;
  int buff_length;
  char *buff_ptr;

  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_DCCH_radioBearerRelease;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;
  //Temp - Hard coded
  protocol_ms->rrc.accepted_trans[0].transaction_Id = pvalue->rrc_TransactionIdentifier;

  /* decode activationTime */
  //   protocol_ms->rrc.nextActivationTime = pvalue->activationTime;
  protocol_ms->rrc.nextActivationTime = 0;
  //temp: contains RB_ID
  protocol_ms->rrc.requested_rbId = pvalue->activationTime;

  /* decode new_U_RNTI */
  protocol_ms->rrc.u_rnti = pvalue->new_U_RNTI;
  /* decode new_c_RNTI -- FFS */
  protocol_ms->rrc.c_rnti = pvalue->new_U_RNTI;

  /* decode rrc_stateIndicator */
  switch (pvalue->rrc_StateIndicator) {
      case cell_DCH:
        protocol_ms->rrc.next_state = CELL_DCH;
        break;
      case cell_FACH:
        protocol_ms->rrc.next_state = CELL_FACH;
        break;
      case cell_PCH:
        protocol_ms->rrc.next_state = CELL_PCH;
        break;
      default:                 //ura_PCH not supported
        return P_INVVALUE;
  }

  /* decode rb_InformationReleaseList  */
  if (pvalue->rb_InformationReleaseList.numrabs > 1)
    msg ("\n[RRC_MSG] Number of rabs to release not supported.\n");
  protocol_ms->rrc.requested_rbId = pvalue->rb_InformationReleaseList.rbid[0];
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Number of Radio Bearers : %d \n", pvalue->rb_InformationReleaseList.numrabs);
#endif

  /* decode ue_Configuration */
  //
  buff_length = (u16) pvalue->ue_Configuration.numoctets;
  buff_ptr = (char *) (&(pvalue->ue_Configuration.numoctets));
  buff_ptr += 2;
  //   rrc_uncompress_buffer(buff_ptr,(u16)pvalue->ue_Configuration.numoctets, buff_ptr, &buff_length);
  //#ifdef DEBUG_RRC_STATE
  //   msg("\n[RRC_MSG] New Configuration , length %d : \n", buff_length);
  //   rrc_print_buffer(buff_ptr, buff_length);
  //#endif

  rrc_uncompress_config ((MT_CONFIG *) &(rrm_config->mt_config), buff_ptr);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Length of New Configuration : %d \n", buff_length);
#endif

  return status;
}
#endif

/**************************************************************/
/*  RadioBearerSetupComplete                                  */
/**************************************************************/

#ifdef NODE_MT
int rrc_PEREnc_RadioBearerSetupComplete (RadioBearerSetupComplete * pvalue){
  int status = P_SUCCESS;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_ms->rrc.accepted_trans[0].transaction_Id;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_RadioBearerSetupComplete (int UE_Id, RadioBearerSetupComplete * pvalue){
  int status = P_SUCCESS;

  /* decode rrc_TransactionIdentifier */
  if (pvalue->rrc_TransactionIdentifier != protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id)
    //      return P_INVVALUE;
  {
    msg ("\n[RRC_MSG] Transaction Id : --%d--, Received : --%d--, status %d\n", protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id, pvalue->rrc_TransactionIdentifier, status);
    //return P_INVVALUE; // TEMP ignore error until transactions completely implemented
  }


  return status;
}
#endif

/**************************************************************/
/*  RadioBearerSetupFailure                                   */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_RadioBearerSetupFailure (RadioBearerSetupFailure * pvalue){
  int status = P_SUCCESS;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_ms->rrc.accepted_trans[0].transaction_Id;

  /* encode failureCause */// Temp hard coded
  pvalue->failureCause.type = FCWPE_protocolError;
  pvalue->failureCause.protocolError = ie_ValueNotComprehended;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int  rrc_PERDec_RadioBearerSetupFailure (int UE_Id, RadioBearerSetupFailure * pvalue){
  int status = P_SUCCESS;

  /* decode rrc_TransactionIdentifier */
  if (pvalue->rrc_TransactionIdentifier != protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id)
    return P_INVVALUE;
  //     {
  //       msg("\n[RRC_MSG] Transaction Id : --%d--, Received : --%d--, status %d\n",

  //                 protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id,
  //                 pvalue->rrc_TransactionIdentifier,status);
  //       return P_INVVALUE; // ignore message
  //     }

  /* decode failureCause */
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] RB Setup Failure cause: type %d, protocol error %d\n", pvalue->failureCause.type, pvalue->failureCause.protocolError);
#endif

  return status;
}
#endif

/**************************************************************/
/*  RadioBearerReleaseComplete                                */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_RadioBearerReleaseComplete (RadioBearerReleaseComplete * pvalue){
  int status = P_SUCCESS;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_ms->rrc.accepted_trans[0].transaction_Id;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_RadioBearerReleaseComplete (int UE_Id, RadioBearerReleaseComplete * pvalue){
  int status = P_SUCCESS;

  /* decode rrc_TransactionIdentifier */
  if (pvalue->rrc_TransactionIdentifier != protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id)
    return P_INVVALUE;
  //     {
  //       msg("\n[RRC_MSG] Transaction Id : --%d--, Received : --%d--, status %d\n",
  //                 protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id,

  //                 pvalue->rrc_TransactionIdentifier,status);
  //       return P_INVVALUE; // ignore message
  //     }

  return status;
}
#endif

/**************************************************************/
/*  RadioBearerReleaseFailure                                 */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_RadioBearerReleaseFailure (RadioBearerReleaseFailure * pvalue){
  int status = P_SUCCESS;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_ms->rrc.accepted_trans[0].transaction_Id;

  /* encode failureCause */// Temp hard coded
  pvalue->failureCause.type = FCWPE_protocolError;
  pvalue->failureCause.protocolError = ie_ValueNotComprehended;

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_RadioBearerReleaseFailure (int UE_Id, RadioBearerReleaseFailure * pvalue){
  int status = P_SUCCESS;

  /* decode rrc_TransactionIdentifier */
  if (pvalue->rrc_TransactionIdentifier != protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id)
    return P_INVVALUE;
  //     {
  //       msg("\n[RRC_MSG] Transaction Id : --%d--, Received : --%d--, status %d\n",
  //                 protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id,
  //                 pvalue->rrc_TransactionIdentifier,status);
  //       return P_INVVALUE; // ignore message
  //     }

  /* decode failureCause */
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] RB Setup Failure cause: type %d, protocol error %d\n", pvalue->failureCause.type, pvalue->failureCause.protocolError);
#endif

  return status;
}
#endif

/**************************************************************/
/*  CellUpdate                                                */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_CellUpdate (CellUpdate * pvalue){
  int status = P_SUCCESS;


  /* encode u_RNTI */
  pvalue->u_RNTI = protocol_ms->rrc.u_rnti;

  /* encode am_RLC_ErrorIndicationRb2_3or4 */
  pvalue->am_RLC_ErrorIndicationRb2_3or4 = protocol_ms->rrc.am_RLC_ErrorIndicationRb2_3or4;
  /* encode am_RLC_ErrorIndicationRb5orAbove */
  pvalue->am_RLC_ErrorIndicationRb5orAbove = protocol_ms->rrc.am_RLC_ErrorIndicationRb5orAbove;
  /* encode cellUpdateCause */
  pvalue->cellUpdateCause = protocol_ms->rrc.cellUpdateCause;
  /* encode failureCause */
  // FFS - In case of invalid message  received
  //pvalue->failureCause = protocol_ms->rrc.establishment_cause;


  /* encode measuredResultsOnRACH */
  // status = rrc_PEREnc_MeasuredResultsOnRACH (pvalue->measuredResultsOnRACH);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_CellUpdate (int *pUE_Id, CellUpdate * pvalue){
  int status = P_SUCCESS;
  int i, UE_Id;

  /* decode u_RNTI and get UE_Id */
  i = 0;

  while ((i < maxUsers) && (pvalue->u_RNTI != protocol_bs->rrc.Mobile_List[i].mt_id))
    i++;

  if (i >= maxUsers) {
    *pUE_Id = 999;
    return P_OUTOFBOUNDS;
  } else {
    *pUE_Id = i;
    UE_Id = *pUE_Id;
  }

  /* decode am_RLC_ErrorIndicationRb2_3or4 */
  protocol_bs->rrc.Mobile_List[UE_Id].am_RLC_ErrorIndicationRb2_3or4 = pvalue->am_RLC_ErrorIndicationRb2_3or4;
  /* decode am_RLC_ErrorIndicationRb5orAbove */
  protocol_bs->rrc.Mobile_List[UE_Id].am_RLC_ErrorIndicationRb5orAbove = pvalue->am_RLC_ErrorIndicationRb5orAbove;

  /* decode cellUpdateCause */
  protocol_bs->rrc.Mobile_List[UE_Id].cellUpdateCause = pvalue->cellUpdateCause;
  /* decode failureCause */
  // FFS - In case of invalid message  received
  // protocol_bs->rrc.Mobile_List[UE_Id].failureCause = pvalue->failureCause = protocol_ms->rrc.establishment_cause;

  //    #ifdef DEBUG_RRC_STATE
  //    msg("\n[RRC_RG] Establishment Cause %d \n", pvalue->establishmentCause );
  //    #endif

  /* decode measuredResultsOnRACH */
  //  status = rrc_PERDec_MeasuredResultsOnRACH (&pvalue->measuredResultsOnRACH);


  return status;
}
#endif

/**************************************************************/
/*  CellUpdateConfirm_CCCH                                    */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_CellUpdateConfirm_CCCH (int UE_Id, CellUpdateConfirm_CCCH * pvalue){
  int status = P_SUCCESS;

  char *pwrite;
  int buff_length;

  /* encode u_RNTI */
  pvalue->u_RNTI = protocol_bs->rrc.Mobile_List[UE_Id].mt_id;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode activationTime */
  pvalue->activationTime = 0;   //default value : now = absence

  /* encode new_U_RNTI */
  pvalue->new_U_RNTI = UE_Id;

  /* encode new_c_RNTI -- FFS */

  /* encode rrc_stateIndicator */
  pvalue->rrc_StateIndicator = cell_DCH;        // Temp hard coded

  /* encode rlc_Re_establishIndicatorRb2_3or4 */
  pvalue->rlc_Re_establishIndicatorRb2_3or4 = protocol_bs->rrc.Mobile_List[UE_Id].am_RLC_ErrorIndicationRb2_3or4;
  /* encode rlc_Re_establishIndicatorRb5orAbove */
  pvalue->rlc_Re_establishIndicatorRb5orAbove = protocol_bs->rrc.Mobile_List[UE_Id].am_RLC_ErrorIndicationRb5orAbove;

  // TODO: Remplacer buffers par config
  /* encode ue_Configuration */
  //   rrc_compress_buffer(new_conf,90, new_conf, &buff_length);
  //  pwrite = (char *) &(pvalue->ue_Configuration.numoctets);
  //  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;
  //
  //  *(u16 *) pwrite = buff_length;
  //  pwrite += 2;
  //  memcpy (pwrite, new_conf, buff_length);

  pwrite = (char *) &(pvalue->ue_Configuration.numoctets);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Starting the compression of the configuration.- RB Release\n");
#endif
  rrc_compress_config ((MT_CONFIG *) &(rrm_config->mt_config[UE_Id]), &pwrite[2], &buff_length);

  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;

  *(u16 *) pwrite = buff_length;
  pwrite += 2;

#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length - 1 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif

  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length += buff_length + 2;

#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length - 2 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_CellUpdateConfirm_CCCH (CellUpdateConfirm_CCCH * pvalue){
  int status = P_SUCCESS;
  int buff_length;
  char *buff_ptr;

  /* decode and check u-RNTI */
  if (pvalue->u_RNTI != protocol_ms->rrc.u_rnti) {
    msg ("\n[RRC_MSG] u-RNTI : --%d--, Received : --%d--, status: Invalid u-rnti \n", protocol_ms->rrc.u_rnti, pvalue->u_RNTI);
    return P_INVVALUE;          // ignore message
  }
  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_CCCH_cellUpdateConfirm;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;

  /* decode activationTime */
  protocol_ms->rrc.nextActivationTime = pvalue->activationTime;

  /* decode new_U_RNTI */
  protocol_ms->rrc.u_rnti = pvalue->new_U_RNTI;
  /* decode new_c_RNTI -- FFS */
  protocol_ms->rrc.c_rnti = pvalue->new_U_RNTI;

  /* decode rrc_stateIndicator */
  switch (pvalue->rrc_StateIndicator) {
      case cell_DCH:
        protocol_ms->rrc.next_state = CELL_DCH;
        break;

      case cell_FACH:

        protocol_ms->rrc.next_state = CELL_FACH;
        break;
      case cell_PCH:
        protocol_ms->rrc.next_state = CELL_PCH;
        break;
      default:                 //ura_PCH not supported
        return P_INVVALUE;
  }

  /* decode rlc_Re_establishIndicatorRb2_3or4 */
  protocol_ms->rrc.rlc_Re_establishIndicatorRb2_3or4 = pvalue->rlc_Re_establishIndicatorRb2_3or4;
  /* decode rlc_Re_establishIndicatorRb5orAbove */
  protocol_ms->rrc.rlc_Re_establishIndicatorRb5orAbove = pvalue->rlc_Re_establishIndicatorRb5orAbove;

//  /* decode ue_Configuration */
//  buff_ptr = (char *) (&(pvalue->ue_Configuration.numoctets));
//
//  buff_ptr += 2;
//  //   rrc_uncompress_buffer(buff_ptr,(u16)pvalue->ue_Configuration.numoctets, buff_ptr, &buff_length);
//#ifdef DEBUG_RRC_STATE
//  msg ("\n[RRC_MSG] New Configuration : \n");
//  rrc_print_buffer (buff_ptr, buff_length);
//#endif

  /* decode ue_Configuration */
  //
  buff_length = (u16) pvalue->ue_Configuration.numoctets;
  buff_ptr = (char *) (&(pvalue->ue_Configuration.numoctets));
  buff_ptr += 2;
  //#ifdef DEBUG_RRC_STATE
  //   msg("\n[RRC_MSG] New Configuration , length %d : \n", buff_length);
  //   rrc_print_buffer(buff_ptr, buff_length);
  //#endif

  rrc_uncompress_config ((MT_CONFIG *) &(rrm_config->mt_config), buff_ptr);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Length of New Configuration : %d \n", buff_length);
#endif

  return status;
}
#endif

/**************************************************************/
/*  PagingType2                                               */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_PagingType2 (int UE_Id, PagingType2 * pvalue){
  int status = P_SUCCESS;
  char *pwrite;
  int i;
  int data_length;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode IMEI */
  pvalue->imei.numDigits = 14;  // limited version of IMEI is used

  for (i = 0; i < pvalue->imei.numDigits; i++)
    pvalue->imei.digit[i] = protocol_bs->rrc.Mobile_List[UE_Id].IMEI[i];

  /* encode paging_Message */
  pwrite = (char *) &(pvalue->paging_Message.numoctets);
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH;
  //data_length = strlen(paging_data);
  data_length = protocol_bs->rrc.Mobile_List[UE_Id].paging_message_lgth;
  *(u16 *) pwrite = data_length;
  pwrite += 2;
  //memcpy(pwrite,paging_data, data_length);
  memcpy (pwrite, (protocol_bs->rrc.Mobile_List[UE_Id].paging_message_ptr)->data, data_length);
#ifdef DEBUG_RRC_STATE
  msg ("\n[RRC_MSG] Buffer length - 1 : %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length += (data_length + 2);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_PagingType2 (PagingType2 * pvalue){
  int status = P_SUCCESS;
  char *buff_ptr;

  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_DCCH_pagingType2;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;


  /* decode IMEI */
  if (pvalue->imei.numDigits > 15)
    return P_OUTOFBOUNDS;
  //   if (status = (strncmp((char *)protocol_ms->rrc.IMEI, (char *)pvalue->imei.digit,14)))
  status = memcmp ((char *) protocol_ms->rrc.IMEI, (char *) pvalue->imei.digit, 14);
  if (status) {
    msg ("\n[RRC_MSG] IMEI : --%s--, Received : --%s--, status %d\n", (char *) protocol_ms->rrc.IMEI, (char *) pvalue->imei.digit, status);
    return P_INVVALUE;          // ignore message
  }
  // TODO: Remplacer buffers par paging_Message + verifier qu'ils sont vides
  /* decode paging_Message */
  buff_ptr = (char *) (&(pvalue->paging_Message.numoctets));
  buff_ptr += 2;
  protocol_ms->rrc.paging_message_ptr = buff_ptr;
  protocol_ms->rrc.paging_message_lgth = (u16) pvalue->paging_Message.numoctets;
#ifdef DEBUG_RRC_STATE

  msg ("\n[RRC_MSG] New Paging Type 2 : \n");
  rrc_print_buffer (protocol_ms->rrc.paging_message_ptr, protocol_ms->rrc.paging_message_lgth);
  msg ("\n[RRC_MSG] Data content --%s-- \n", protocol_ms->rrc.paging_message_ptr);
#endif


  return status;
}
#endif

/**************************************************************/
/*  MeasurementControl                                        */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_MeasurementControl (int UE_Id, MeasurementControl * pvalue){
  int status = P_SUCCESS;
  MeasurementType *pCommand = NULL;
  char *pwrite;
  int i;

  /* encode rrc_TransactionIdentifier */
  pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;

  /* encode  measurementIdentity */
  pvalue->measurementIdentity = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.identity;

  /* encode  measurementCommand */
  pvalue->measurementCommand.cmdType = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.command;

  switch (pvalue->measurementCommand.cmdType) {
      case MC_setup:
        pCommand = &(pvalue->measurementCommand.command.setup);
        break;
      case MC_modify:
        pCommand = &(pvalue->measurementCommand.command.modify);
        break;
      default:
        // MC_delete : nothing to encode
        pCommand = NULL;
  }

  if (pCommand != NULL) {
    pCommand->measType = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.type;
    switch (pCommand->measType) {
        case MT_intraFrequencyMeasurement:
          pCommand->type.intraFrequencyMeasurement.intraFreqMeasQuantity.filterCoefficient = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_coeff;
          pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.cellIdentity_reportingIndicator = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_cellIdentity_ri;
          pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.timeslotISCP_reportingIndicator = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_timeslotISCP_ri;
          pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.primaryCCPCH_RSCP_reportingIndicator = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_BCH_RSCP_ri;
          pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.pathloss_reportingIndicator = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_pathloss_ri;
          pCommand->type.intraFrequencyMeasurement.measurementValidity = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.if_validity;
          pCommand->type.intraFrequencyMeasurement.reportCriteria.criteriaType =        // must be periodical
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.criteria_type;
          pCommand->type.intraFrequencyMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_amount;
          pCommand->type.intraFrequencyMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_interval;
          break;
        case MT_trafficVolumeMeasurement:
          pCommand->type.trafficVolumeMeasurement.trafficVolumeReportingQuantity.rlc_RB_BufferPayload = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_payload_ri;
          pCommand->type.trafficVolumeMeasurement.trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_average_ri;
          pCommand->type.trafficVolumeMeasurement.trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_variance_ri;
          pCommand->type.trafficVolumeMeasurement.measurementValidity = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.tv_validity;
          pCommand->type.trafficVolumeMeasurement.reportCriteria.criteriaType = // must be periodical
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.criteria_type;
          pCommand->type.trafficVolumeMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_amount;
          pCommand->type.trafficVolumeMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_interval;
          break;
        case MT_qualityMeasurement:
          pCommand->type.qualityMeasurement.qualityReportingQuantity.dl_TransChBLER = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.q_dl_trch_bler_ri;
          for (i = 0; i < MAXMEASTFCS; i++)
            pCommand->type.qualityMeasurement.qualityReportingQuantity.sir_TFCS_List.tfcs[i] = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.q_sir_TFCSid[i];
          pCommand->type.qualityMeasurement.reportCriteria.criteriaType =       // must be periodical
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.criteria_type;
          pCommand->type.qualityMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_amount;
          pCommand->type.qualityMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_interval;
          break;
        case MT_ue_InternalMeasurement:
          pCommand->type.ue_InternalMeasurement.ue_InternalMeasQuantity.measurementQuantity = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_quantity;
          pCommand->type.ue_InternalMeasurement.ue_InternalMeasQuantity.filterCoefficient = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_coeff;
          pCommand->type.ue_InternalMeasurement.ue_InternalReportingQuantity.ue_TransmittedPower = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_rep_ue_TransmittedPower;
          pCommand->type.ue_InternalMeasurement.ue_InternalReportingQuantity.appliedTA = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.int_rep_appliedTA;
          pCommand->type.ue_InternalMeasurement.reportCriteria.criteriaType = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.criteria_type;
          pCommand->type.ue_InternalMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_amount;
          pCommand->type.ue_InternalMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.rep_interval;
          break;
        default:
          msg ("\n[RRC_MSG] invalid Measure Command Type selected\n");
    }
  }                             //endif


  /* encode measurementReportingMode */
  pvalue->measurementReportingMode.measurementReportTransferMode = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.xfer_mode;
  pvalue->measurementReportingMode.periodicalOrEventTrigger = protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_cmd.trigger;

  /* determine the exact length of the message (compiler error) */
  pwrite = (char *) &(pvalue->measurementReportingMode.periodicalOrEventTrigger);
  protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length = pwrite - (char *) pvalue + MSG_HEAD_LGTH + 4;
  msg ("\n[RRC_MSG] Measurement Control message length %d\n", protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);

  return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_MeasurementControl (MeasurementControl * pvalue){
  int status = P_SUCCESS;
  int m_id, i;
  MeasurementType *pCommand = NULL;

  /* decode rrc_TransactionIdentifier */
  protocol_ms->rrc.rcved_trans.msg_type = DL_DCCH_measurementControl;
  protocol_ms->rrc.rcved_trans.transaction_Id = pvalue->rrc_TransactionIdentifier;

  /* decode  measurementIdentity */
  m_id = pvalue->measurementIdentity;
  protocol_ms->rrc.ue_meas_cmd[m_id].identity = pvalue->measurementIdentity;
  protocol_ms->rrc.rrc_ue_meas_to_activate = m_id;
  /* decode  measurementCommand */
  protocol_ms->rrc.ue_meas_cmd[m_id].command = pvalue->measurementCommand.cmdType;
  switch (pvalue->measurementCommand.cmdType) {
      case MC_setup:
        pCommand = &(pvalue->measurementCommand.command.setup);
        break;
      case MC_modify:
        pCommand = &(pvalue->measurementCommand.command.modify);
        break;
      default:
        // MC_release : nothing to encode
        pCommand = NULL;
  }

  if (pCommand != NULL) {
    protocol_ms->rrc.ue_meas_cmd[m_id].type = pCommand->measType;
    switch (pCommand->measType) {
        case MT_intraFrequencyMeasurement:
          protocol_ms->rrc.ue_meas_cmd[m_id].if_coeff = pCommand->type.intraFrequencyMeasurement.intraFreqMeasQuantity.filterCoefficient;
          protocol_ms->rrc.ue_meas_cmd[m_id].if_cellIdentity_ri = pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.cellIdentity_reportingIndicator;
          protocol_ms->rrc.ue_meas_cmd[m_id].if_timeslotISCP_ri = pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.timeslotISCP_reportingIndicator;
          protocol_ms->rrc.ue_meas_cmd[m_id].if_BCH_RSCP_ri = pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.primaryCCPCH_RSCP_reportingIndicator;
          protocol_ms->rrc.ue_meas_cmd[m_id].if_pathloss_ri = pCommand->type.intraFrequencyMeasurement.intraFreqReportingQuantity.pathloss_reportingIndicator;
          protocol_ms->rrc.ue_meas_cmd[m_id].if_validity = pCommand->type.intraFrequencyMeasurement.measurementValidity;
          protocol_ms->rrc.ue_meas_cmd[m_id].criteria_type = pCommand->type.intraFrequencyMeasurement.reportCriteria.criteriaType;      // must be periodical
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_amount = pCommand->type.intraFrequencyMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount;
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_interval = pCommand->type.intraFrequencyMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval;
          break;
        case MT_trafficVolumeMeasurement:
          protocol_ms->rrc.ue_meas_cmd[m_id].tv_payload_ri = pCommand->type.trafficVolumeMeasurement.trafficVolumeReportingQuantity.rlc_RB_BufferPayload;
          protocol_ms->rrc.ue_meas_cmd[m_id].tv_average_ri = pCommand->type.trafficVolumeMeasurement.trafficVolumeReportingQuantity.rlc_RB_BufferPayloadAverage;
          protocol_ms->rrc.ue_meas_cmd[m_id].tv_variance_ri = pCommand->type.trafficVolumeMeasurement.trafficVolumeReportingQuantity.rlc_RB_BufferPayloadVariance;
          protocol_ms->rrc.ue_meas_cmd[m_id].tv_validity = pCommand->type.trafficVolumeMeasurement.measurementValidity;
          protocol_ms->rrc.ue_meas_cmd[m_id].criteria_type = pCommand->type.trafficVolumeMeasurement.reportCriteria.criteriaType;       // must be periodical
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_amount = pCommand->type.trafficVolumeMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount;
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_interval = pCommand->type.trafficVolumeMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval;
          break;
        case MT_qualityMeasurement:
          protocol_ms->rrc.ue_meas_cmd[m_id].q_dl_trch_bler_ri = pCommand->type.qualityMeasurement.qualityReportingQuantity.dl_TransChBLER;
          for (i = 0; i < MAXMEASTFCS; i++)
            protocol_ms->rrc.ue_meas_cmd[m_id].q_sir_TFCSid[i] = pCommand->type.qualityMeasurement.qualityReportingQuantity.sir_TFCS_List.tfcs[i];
          protocol_ms->rrc.ue_meas_cmd[m_id].criteria_type = pCommand->type.qualityMeasurement.reportCriteria.criteriaType;     // must be periodical
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_amount = pCommand->type.qualityMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount;
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_interval = pCommand->type.qualityMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval;
          break;
        case MT_ue_InternalMeasurement:
          protocol_ms->rrc.ue_meas_cmd[m_id].int_quantity = pCommand->type.ue_InternalMeasurement.ue_InternalMeasQuantity.measurementQuantity;
          protocol_ms->rrc.ue_meas_cmd[m_id].int_coeff = pCommand->type.ue_InternalMeasurement.ue_InternalMeasQuantity.filterCoefficient;
          protocol_ms->rrc.ue_meas_cmd[m_id].int_rep_ue_TransmittedPower = pCommand->type.ue_InternalMeasurement.ue_InternalReportingQuantity.ue_TransmittedPower;
          protocol_ms->rrc.ue_meas_cmd[m_id].int_rep_appliedTA = pCommand->type.ue_InternalMeasurement.ue_InternalReportingQuantity.appliedTA;
          protocol_ms->rrc.ue_meas_cmd[m_id].criteria_type = pCommand->type.ue_InternalMeasurement.reportCriteria.criteriaType;
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_amount = pCommand->type.ue_InternalMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingAmount;
          protocol_ms->rrc.ue_meas_cmd[m_id].rep_interval = pCommand->type.ue_InternalMeasurement.reportCriteria.criteriaDef.periodicalReportingCriteria.reportingInterval;
          break;
        default:
          msg ("\n[RRC_MSG] invalid Measure Command Type selected : %d\n", pCommand->measType);
    }
  }                             //endif

  /* decode measurementReportingMode */
  protocol_ms->rrc.ue_meas_cmd[m_id].xfer_mode = pvalue->measurementReportingMode.measurementReportTransferMode;
  protocol_ms->rrc.ue_meas_cmd[m_id].trigger = pvalue->measurementReportingMode.periodicalOrEventTrigger;

  return status;
}
#endif

/**************************************************************/
/*  MeasurementReport                                         */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_MeasurementReport (MeasurementReport * pvalue){
  int status = P_SUCCESS;
  int i, j, nCells = 0;
  int m_id;

  /* encode measurementIdentity */
  m_id = protocol_ms->rrc.rrc_ue_last_measurement;
  pvalue->measurementIdentity = protocol_ms->rrc.ue_meas_rep[m_id].identity;

  /* encode measuredResults */
  pvalue->measuredResults.measResult = protocol_ms->rrc.ue_meas_rep[m_id].meas_results_type;

  switch (pvalue->measuredResults.measResult) {
      case MR_intraFreqMeasuredResultsList:
        pvalue->measuredResults.result.intraFreqMeasuredResultsList.numCells = protocol_ms->rrc.ue_meas_rep[m_id].if_num_cells;
        nCells = protocol_ms->rrc.ue_meas_rep[m_id].if_num_cells;
        for (i = 0; i < nCells; i++) {
          pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].cellIdentity = protocol_ms->rrc.ue_meas_rep[m_id].if_cell_id[i];
          pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.cellParametersID = protocol_ms->rrc.ue_meas_rep[m_id].if_cell_parms_id[i];
          pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.primaryCCPCH_RSCP = protocol_ms->rrc.ue_meas_rep[m_id].if_BCH_RSCP[i];
          pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.pathloss = protocol_ms->rrc.ue_meas_rep[m_id].if_pathloss[i];
          pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.timeslotISCP_List.numSlots = 14;
          for (j = 0; j < 14; j++) {
            pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.timeslotISCP_List.iscp[j] = protocol_ms->rrc.ue_meas_rep[m_id].if_slot_iscp[i][j];
          }
        }
        break;
      case MR_trafficVolumeMeasuredResultsList:
        pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.numRB = protocol_ms->rrc.ue_meas_rep[m_id].tv_num_rbs;
        for (i = 0; i < protocol_ms->rrc.ue_meas_rep[m_id].tv_num_rbs; i++) {
          pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].rb_Identity = protocol_ms->rrc.ue_meas_rep[m_id].tv_rbid[i];
          pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].rlc_BuffersPayload = protocol_ms->rrc.ue_meas_rep[m_id].tv_rb_payload[i];
          pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].averageRLC_BufferPayload = protocol_ms->rrc.ue_meas_rep[m_id].tv_rb_average[i];
          pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].varianceOfRLC_BufferPayload = protocol_ms->rrc.ue_meas_rep[m_id].tv_rb_variance[i];
        }
        break;
      case MR_qualityMeasuredResults:
        pvalue->measuredResults.result.qualityMeasuredResults.blerMeasurementResultsList.numTrCH = protocol_ms->rrc.ue_meas_rep[m_id].q_num_TrCH;
        for (i = 0; i < protocol_ms->rrc.ue_meas_rep[m_id].q_num_TrCH; i++) {
          pvalue->measuredResults.result.qualityMeasuredResults.blerMeasurementResultsList.measTrCH[i].transportChannelIdentity = protocol_ms->rrc.ue_meas_rep[m_id].q_dl_TrCH_id[i];
          pvalue->measuredResults.result.qualityMeasuredResults.blerMeasurementResultsList.measTrCH[i].dl_TransportChannelBLER = protocol_ms->rrc.ue_meas_rep[m_id].q_dl_TrCH_BLER[i];
        }
        pvalue->measuredResults.result.qualityMeasuredResults.sir_MeasurementList.numTFCS = protocol_ms->rrc.ue_meas_rep[m_id].q_num_tfcs;
        for (i = 0; i < protocol_ms->rrc.ue_meas_rep[m_id].q_num_tfcs; i++) {
          pvalue->measuredResults.result.qualityMeasuredResults.sir_MeasurementList.sirMeas[i].tfcs_ID = protocol_ms->rrc.ue_meas_rep[m_id].q_tfcs_id[i];
          pvalue->measuredResults.result.qualityMeasuredResults.sir_MeasurementList.sirMeas[i].sir_TimeslotList.numSIR = 14;
          for (j = 0; j < 14; j++) {
            pvalue->measuredResults.result.qualityMeasuredResults.sir_MeasurementList.sirMeas[i].sir_TimeslotList.sir[j] = protocol_ms->rrc.ue_meas_rep[m_id].q_sir[i][j];
          }
        }
        break;
      case MR_ue_InternalMeasuredResults:
        pvalue->measuredResults.result.ue_InternalMeasuredResults.ue_TransmittedPowerTDD_List.numSlots = 14;
        for (j = 0; j < 14; j++) {
          pvalue->measuredResults.result.ue_InternalMeasuredResults.ue_TransmittedPowerTDD_List.xmitPower[j] = protocol_ms->rrc.ue_meas_rep[m_id].int_xmit_power[j];
        }
        pvalue->measuredResults.result.ue_InternalMeasuredResults.appliedTA = protocol_ms->rrc.ue_meas_rep[m_id].int_timing_advance;
        break;
      default:
        msg ("\n[RRC_MSG] invalid Measure Report Type selected %d\n", pvalue->measuredResults.measResult);
  }


  return status;
}

#endif


//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_MeasurementReport (int UE_Id, MeasurementReport * pvalue)
{
  int status = P_SUCCESS;
  int i, j, nCells = 0;
  int m_id;

  /* decode measurementIdentity */
  m_id = pvalue->measurementIdentity;
  protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].identity = pvalue->measurementIdentity;
  protocol_bs->rrc.Mobile_List[UE_Id].rrc_rg_last_measurement = m_id;

  /* decode measuredResults */
  protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].meas_results_type = pvalue->measuredResults.measResult;

  switch (pvalue->measuredResults.measResult) {
      case MR_intraFreqMeasuredResultsList:
        protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].if_num_cells = pvalue->measuredResults.result.intraFreqMeasuredResultsList.numCells;
        nCells = pvalue->measuredResults.result.intraFreqMeasuredResultsList.numCells;
        for (i = 0; i < nCells; i++) {
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].if_cell_id[i] = pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].cellIdentity;
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].if_cell_parms_id[i] = pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.cellParametersID;
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].if_BCH_RSCP[i] = pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.primaryCCPCH_RSCP;
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].if_pathloss[i] = pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.pathloss;
          for (j = 0; j < 14; j++) {
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].if_slot_iscp[i][j] =
              pvalue->measuredResults.result.intraFreqMeasuredResultsList.cellMeas[i].modeSpecificInfo.timeslotISCP_List.iscp[j];
          }
        }
        break;
      case MR_trafficVolumeMeasuredResultsList:
        protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].tv_num_rbs = pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.numRB;
        for (i = 0; i < protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].tv_num_rbs; i++) {
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].tv_rbid[i] = pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].rb_Identity;
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].tv_rb_payload[i] = pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].rlc_BuffersPayload;
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].tv_rb_average[i] = pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].averageRLC_BufferPayload;
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].tv_rb_variance[i] = pvalue->measuredResults.result.trafficVolumeMeasuredResultsList.RBMeas[i].varianceOfRLC_BufferPayload;
        }
        break;
      case MR_qualityMeasuredResults:
        protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_num_TrCH = pvalue->measuredResults.result.qualityMeasuredResults.blerMeasurementResultsList.numTrCH;
        for (i = 0; i < protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_num_TrCH; i++) {
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_dl_TrCH_id[i] = pvalue->measuredResults.result.qualityMeasuredResults.blerMeasurementResultsList.measTrCH[i].transportChannelIdentity;
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_dl_TrCH_BLER[i] =
            pvalue->measuredResults.result.qualityMeasuredResults.blerMeasurementResultsList.measTrCH[i].dl_TransportChannelBLER;
        }
        protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_num_tfcs = pvalue->measuredResults.result.qualityMeasuredResults.sir_MeasurementList.numTFCS;
        for (i = 0; i < protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_num_tfcs; i++) {
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_tfcs_id[i] = pvalue->measuredResults.result.qualityMeasuredResults.sir_MeasurementList.sirMeas[i].tfcs_ID;
          for (j = 0; j < 14; j++) {
            protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].q_sir[i][j] = pvalue->measuredResults.result.qualityMeasuredResults.sir_MeasurementList.sirMeas[i].sir_TimeslotList.sir[j];
          }
        }
        break;
      case MR_ue_InternalMeasuredResults:
        for (j = 0; j < 14; j++) {
          protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].int_xmit_power[j] = pvalue->measuredResults.result.ue_InternalMeasuredResults.ue_TransmittedPowerTDD_List.xmitPower[j];
        }
        protocol_bs->rrc.Mobile_List[UE_Id].rg_meas_rep[m_id].int_timing_advance = pvalue->measuredResults.result.ue_InternalMeasuredResults.appliedTA;
        break;
      default:
        msg ("\n[RRC_MSG] invalid Measure Report Type selected\n");
  }


  return status;
}
#endif

// /**************************************************************/
// /*  ULPCHControl                                              */
// /**************************************************************/
// #ifdef NODE_RG
// int rrc_PEREnc_ULPCHControl (int UE_Id, ULPCHControl * pvalue){
//   int status = P_SUCCESS;
//   pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;
//   pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.timingAdvance = 0;
//   pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.alpha = 1;
//   pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.prach_ConstantValue = 6;
//   pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.pusch_ConstantValue = 0;
//   pvalue->uplinkPhysicalChannelControl_IE.ccTrCH_PowerControlInfo.ul_DPCH_PowerControlInfo.ul_TargetSIR = 10;
// 
//   ////#ifdef DEBUG_RRC_STATE
//   msg ("\n\n\n\n\n\n\n\n\n\n[RRC_MSG] Creating ULPCH Outer-Loop Power Control Information  frame = %d\n", frame);
//   msg ("[RRC_MSG] timingAdvance=%d,alpha=%d,prach_Cnst=%d,pusch_cnst=%d\n",
//        pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.timingAdvance,
//        pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.alpha,
//        pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.prach_ConstantValue, pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.pusch_ConstantValue);
//   msg ("[RRC_MSG] CCTrCH Power Control Info\n");
//   msg ("[RRC_MSG] TFCS Identity (CCTrCH index) = %d, Target SIR = %d\n",
//        pvalue->uplinkPhysicalChannelControl_IE.ccTrCH_PowerControlInfo.tfcs_Identity, pvalue->uplinkPhysicalChannelControl_IE.ccTrCH_PowerControlInfo.ul_DPCH_PowerControlInfo.ul_TargetSIR);
//   // #endif DEBUG_RRC_STATE
// 
//   return status;
// }
// #endif
// //-----------------------------------------------------------------------------
// #ifdef NODE_MT
// int rrc_PERDec_ULPCHControl (ULPCHControl * pvalue){
//   int status = P_SUCCESS;
// 
// #ifdef DEBUG_RRC_STATE
//   msg ("[RRC_MSG] Received ULPCH Outer-Loop Power Control Information\n");
//   msg ("[RRC_MSG] timingAdvance=%d,alpha=%d,prach_Cnst=%d,pusch_cnst=%d    dpch_ConstantValue =%d \n",
//        pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.timingAdvance,
//        pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.alpha,
//        pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.prach_ConstantValue,
//        pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.pusch_ConstantValue, pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.dpch_ConstantValue);
//   msg ("[RRC_MSG] CCTrCH Power Control Info\n");
//   msg ("[RRC_MSG] TFCS Identity (CCTrCH index) = %d, Target SIR = %d\n",
//        pvalue->uplinkPhysicalChannelControl_IE.ccTrCH_PowerControlInfo.tfcs_Identity, pvalue->uplinkPhysicalChannelControl_IE.ccTrCH_PowerControlInfo.ul_DPCH_PowerControlInfo.ul_TargetSIR);
// #endif
//   /* DEBUG_RRC_STATE */
//   // rrm_config->outer_loop_vars.= pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.timingAdvance;
// 
//   rrm_config->outer_loop_vars.alpha = pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.alpha;
//   rrm_config->outer_loop_vars.PRACH_CNST = pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.prach_ConstantValue;
// 
//   rrm_config->outer_loop_vars.DPCH_CNST = pvalue->uplinkPhysicalChannelControl_IE.uplinkPhysicalChannelControl.dpch_ConstantValue;
//   rrm_config->outer_loop_vars.SIR_Target[0] = pvalue->uplinkPhysicalChannelControl_IE.ccTrCH_PowerControlInfo.ul_DPCH_PowerControlInfo.ul_TargetSIR;
//   rrc_ue_tick = 0;
//   rrm_config->power_control_ul_received = 1;
//   // Write to RRM Power Control Structures
//   return status;
// }
// #endif

/**************************************************************/
/* UECapabilityInformation                                    */
/**************************************************************/
#ifdef NODE_MT
int rrc_PEREnc_UECapabilityInformation (UECapabilityInformation* pvalue){
	int status = P_SUCCESS;
	/* encode rrc_TransactionIdentifier */
	pvalue->rrc_TransactionIdentifier = protocol_ms->rrc.accepted_trans[0].transaction_Id;

	/* encode Access Stratum Release Indicator */
	pvalue->accessStratumReleaseIndicator = ACCESS_STRATUM_RELEASE_INDICATOR_DEFAULT;

	/* encode Eurecom Kernel Release Indicator */
	pvalue->eurecomKernelReleaseIndicator = EURECOM_KERNEL_RELEASE_INDICATOR_DEFAULT;
	return status;
}
#endif
//-----------------------------------------------------------------------------
#ifdef NODE_RG
int rrc_PERDec_UECapabilityInformation (int UE_Id, UECapabilityInformation* pvalue){
	int status = P_SUCCESS;
	/* decode rrc_TransactionIdentifier */
	//In fact, this check is not needed in certain cases because the rrc_TransactionIdentifier IE is OPTIONAL
	if (pvalue->rrc_TransactionIdentifier != protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id)
	{
		msg("\n[RRC_MSG] Transaction Id : --%d--, Received : --%d--, status %d\n",
			protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id,
			pvalue->rrc_TransactionIdentifier,status);
		//return P_INVVALUE; // TEMP ignore error until transactions completely implemented
	}
	protocol_bs->rrc.Mobile_List[UE_Id].ind_accessStratumRelease = pvalue->accessStratumReleaseIndicator;
  protocol_bs->rrc.Mobile_List[UE_Id].ind_eurecomKernelRelease = pvalue->eurecomKernelReleaseIndicator;
	return status;
}
#endif

/**************************************************************/
/* UECapabilityInformationConfirm                             */
/**************************************************************/
#ifdef NODE_RG
int rrc_PEREnc_UECapabilityInformationConfirm(int UE_Id, UECapabilityInformationConfirm * pvalue){
	int status = P_SUCCESS;
	/* encode rrc_TransactionIdentifier */
	pvalue->rrc_TransactionIdentifier = protocol_bs->rrc.Mobile_List[UE_Id].xmit_trans[0].transaction_Id;
	/* encode the message */
	/* Nothing */
	return status;
}
#endif

//-----------------------------------------------------------------------------
#ifdef NODE_MT
int rrc_PERDec_UECapabilityInformationConfirm(UECapabilityInformationConfirm* pvalue){
	int status = P_SUCCESS;
	/* decode rrc_TransactionIdentifier */
	protocol_ms->rrc.rcved_trans.msg_type= DL_DCCH_ueCapabilityInformationConfirm;
	protocol_ms->rrc.rcved_trans.transaction_Id= pvalue->rrc_TransactionIdentifier;
	/* decode the message */
	/* Nothing */
	return status;
}
#endif

