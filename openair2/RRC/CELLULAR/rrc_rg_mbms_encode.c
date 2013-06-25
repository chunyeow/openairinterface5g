/***************************************************************************
                          rrc_rg_mbms_encode.c - description
                          -------------------
    begin               : May 30, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by		: Huu-Nghia.Nguyen@eurecom.fr	
    modified by        	: Michelle.Wetterwald@eurecom.fr 
 **************************************************************************
    Encodes messages to be sent on MBMS
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

/**
 * @brief Prepare the buffer of p_rg_mbms->curr_Message for encoding,
 * the buffer's content is filled by zero
 * @return the pointer of the buffer's content.
 * @date Jun 28, 2005 11:14:16 AM
 */
//-----------------------------------------------------------------------------
char * rrc_rg_mbms_buffer_prepare(int msglen){
//-----------------------------------------------------------------------------
  p_rg_mbms->curr_Message.msg_length = msglen;
  memset (p_rg_mbms->curr_Message.msg_ptr, 0, msglen);		
  return p_rg_mbms->curr_Message.msg_ptr;
}

/**
 * @brief Validate the encoded content. 
 * @date Jun 28, 2005 11:46:42 AM
 */
//-----------------------------------------------------------------------------
mbms_bool rrc_rg_mbms_buffer_validate(int status){
//-----------------------------------------------------------------------------
  if (status != P_SUCCESS) 
    p_rg_mbms->curr_Message.msg_length = 0;
  return (status == P_SUCCESS);
}

/**
 * @brief create a MCCH message containing a MBMSAccessInformation  pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 3:26:37 PM
 * @see rrc_PEREnc_MBMSAccessInformation()
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_accessInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MCCH_MSG_HEAD_LGTH + sizeof(MBMSAccessInformation);	
  MCCH_Message * mcch_msg_ptr ;

  //Initialisation & Encode
  mcch_msg_ptr = (MCCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);
  mcch_msg_ptr->message.type = MCCH_mbmsAccessInformation;		
  status = rrc_PEREnc_MBMSAccessInformation((MBMSAccessInformation*) &mcch_msg_ptr->message.content);
  rrc_rg_mbms_buffer_validate(status);
}

/**
 * @brief create a MCCH message containing a MBMSCommonPTMRBInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_commonRBInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MCCH_MSG_HEAD_LGTH + sizeof(MBMSCommonPTMRBInformation);	
  MCCH_Message * mcch_msg_ptr;

  //Initialisation & Encode
  mcch_msg_ptr = (MCCH_Message *) rrc_rg_mbms_buffer_prepare(msglen);
  mcch_msg_ptr->message.type = MCCH_mbmsCommonPTMRBInformation;		
  status = rrc_PEREnc_MBMSCommonPTMRBInformation((MBMSCommonPTMRBInformation*) &mcch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}

/**
 * @brief create a MCCH message containing a MBMSCurrentCellPTMRBInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_currentCellRBInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MCCH_MSG_HEAD_LGTH + sizeof(MBMSCurrentCellPTMRBInformation);	
  MCCH_Message * mcch_msg_ptr;

  //Initialisation & Encode
  mcch_msg_ptr = (MCCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);
  mcch_msg_ptr->message.type = MCCH_mbmsCurrentCellPTMRBInformation;		
  status = rrc_PEREnc_MBMSCurrentCellPTMRBInformation((MBMSCurrentCellPTMRBInformation*) &mcch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}

/**
 * @brief create a MCCH message containing a MBMSModifiedServicesInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_modifservInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MCCH_MSG_HEAD_LGTH + sizeof(MBMSModifiedServicesInformation);	
  MCCH_Message * mcch_msg_ptr;

  //Initialisation & Encode
  mcch_msg_ptr = (MCCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);
  mcch_msg_ptr->message.type = MCCH_mbmsModifiedServicesInformation;
  status = rrc_PEREnc_MBMSModifiedServicesInformation((MBMSModifiedServicesInformation*) &mcch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}



/**
 * @brief create a DCCH message containing a MBMSModifiedServicesInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_DCCH_modifservInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MSG_HEAD_LGTH + sizeof(MBMSModifiedServicesInformation);
  DL_DCCH_Message * dl_dcch_msg_ptr;

  //Initialisation & Encode
  dl_dcch_msg_ptr = (DL_DCCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);	
  dl_dcch_msg_ptr->message.type = DL_DCCH_mbmsModifiedServicesInformation;
  dl_dcch_msg_ptr->integrityCheckInfo = 123;
  status = rrc_PEREnc_MBMSModifiedServicesInformation((MBMSModifiedServicesInformation*) &dl_dcch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}


/**
 * @brief create a MCCH message containing a MBMSGeneralInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_generalInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MSG_HEAD_LGTH + sizeof(MBMSGeneralInformation);
  MCCH_Message * mcch_msg_ptr;

  //Initialisation & Encode
  mcch_msg_ptr = (MCCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);
  mcch_msg_ptr->message.type = MCCH_mbmsGeneralInformation;		
  status = rrc_PEREnc_MBMSGeneralInformation((MBMSGeneralInformation*) &mcch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}


/**
 * @brief create a MCCH message containing a MBMSNeighbouringCellPTMRBInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_neighbouringCellRBInfo(int cellIndex){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MCCH_MSG_HEAD_LGTH + sizeof(MBMSNeighbouringCellPTMRBInformation);	
  MCCH_Message * mcch_msg_ptr;

  //Initialisation & Encode	
  mcch_msg_ptr = (MCCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);
  mcch_msg_ptr->message.type = MCCH_mbmsNeighbouringCellPTMRBInformation;
  status = rrc_PEREnc_MBMSNeighbouringCellPTMRBInformation(cellIndex, (MBMSNeighbouringCellPTMRBInformation*) &mcch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}

/**
 * @brief create a MSCH message containing a MBMSSchedulingInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MSCH_schedulingInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MSG_HEAD_LGTH + sizeof(MBMSSchedulingInformation);	
  MSCH_Message * msch_msg_ptr;

  //Initialisation & Encode
  msch_msg_ptr = (MSCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);
  msch_msg_ptr->message.type = MSCH_mbmsSchedulingInformation;	
  status = rrc_PEREnc_MBMSSchedulingInformation((MBMSSchedulingInformation*) &msch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}

/**
 * @brief create a MCCH message containing a MBMSUnmodifiedServicesInformation pdu. 
 * The result is stored in the p_rg_mbms->curr_Message
 * @date May 20, 2005 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_unmodifServInfo(void){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int msglen = MCCH_MSG_HEAD_LGTH + sizeof(MBMSUnmodifiedServicesInformation);	
  MCCH_Message * mcch_msg_ptr;

  //Initialisation & Encode
  mcch_msg_ptr = (MCCH_Message*) rrc_rg_mbms_buffer_prepare(msglen);
  mcch_msg_ptr->message.type = MCCH_mbmsUnmodifiedServicesInformation;		
  status = rrc_PEREnc_MBMSUnmodifiedServicesInformation((MBMSUnmodifiedServicesInformation*) &mcch_msg_ptr->message.content);	 	

  //Validate the result
  rrc_rg_mbms_buffer_validate(status);
}

/**
 * @brief used to create/encode the message MCCH whose type is specified. 
 * Result is stored in protocol_bs->rrc.mbms.curr_Message
 * @date May 19, 2005 5:18:20 PM
 * @param msgtype: determine the type of MBMS MCCH Message = MCCH_mbmsAccessInformation, MCCH_mbmsCommonPTMRBInformation, ...
 */
 //-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_encode(int msgtype){
//-----------------------------------------------------------------------------		
  switch (msgtype) {
    case MCCH_mbmsAccessInformation:
      #ifdef DEBUG_RRC_MBMS
      msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MCCH_encode is now creating the message AccessInformation\n");
      #endif	
      rrc_rg_mbms_MCCH_accessInfo();		
      break;
    case MCCH_mbmsCommonPTMRBInformation:
      #ifdef DEBUG_RRC_MBMS_BASIC
      msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MCCH_encode is now creating the message CommonPTMRBInformation\n");
      #endif		
      rrc_rg_mbms_MCCH_commonRBInfo();
      break;
    case MCCH_mbmsCurrentCellPTMRBInformation:
      #ifdef DEBUG_RRC_MBMS_BASIC
      msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MCCH_encode is now creating the message CurrentCellPTMRBInformation\n");
      #endif		
      rrc_rg_mbms_MCCH_currentCellRBInfo();
      break;
    case MCCH_mbmsGeneralInformation:
      #ifdef DEBUG_RRC_MBMS_BASIC
      msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MCCH_encode is now creating the message GeneralInformation\n");
      #endif		
      rrc_rg_mbms_MCCH_generalInfo();
      break;
    case MCCH_mbmsModifiedServicesInformation:
      #ifdef DEBUG_RRC_MBMS_BASIC
      msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MCCH_encode is now creating the message ModifiedServicesInformation\n");
      #endif		
      rrc_rg_mbms_MCCH_modifservInfo();
      break;
    case MCCH_mbmsNeighbouringCellPTMRBInformation:
      #ifdef DEBUG_RRC_MBMS
      msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MCCH_encode is now creating the message NeighbouringCellPTMRBInformation\n");
      #endif		
      rrc_rg_mbms_MCCH_neighbouringCellRBInfo(p_rg_mbms->neighb_index);
      break;
    case MCCH_mbmsUnmodifiedServicesInformation:
      #ifdef DEBUG_RRC_MBMS_BASIC
      msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MCCH_encode is now creating the message UnmodifiedServicesInformation\n");
      #endif		
      rrc_rg_mbms_MCCH_unmodifServInfo();
      break;
    default:
      //nothing to do
      msg("[RRC-RG][MBMS] rrc_rg_mbms_MCCH_encode : unrecognized message type \n");
      break;
  }
}

/**
 * 
 * @brief This function is used to create/encode the message 
 * MBMSModifiedServicesInformation on DCCH
 * @date May 30, 2005 12:00:14 AM
 * @param msgtype: the type must be  DL_DCCH_mbmsModifiedServicesInformation
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_DCCH_encode(int msgtype){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_MBMS_BASIC
  msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_DCCH_encode is now creating the message MBMSModifiedServicesInformation \n");
  #endif

  switch (msgtype) {
  case DL_DCCH_mbmsModifiedServicesInformation:
    rrc_rg_mbms_DCCH_modifservInfo();
    break;
  default:
    //nothing to do, should never happen
    msg("[RRC-RG][MBMS] rrc_rg_mbms_DCCH_encode : unrecognized message type \n");
    break;
  }
}

/**
 * @brief This function is used to create/encode the messages on MSCH
 * @date May 30, 2005 3:47:33 PM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MSCH_encode(int msgtype){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_MBMS
  msg("[RRC-RG][MBMS] Procedure rrc_rg_mbms_MSCH_encode is now creating the message MBMSSchedulingInformation\n");
  #endif

  switch (msgtype) {
    case MSCH_mbmsSchedulingInformation:
      rrc_rg_mbms_MSCH_schedulingInfo();
      break;
    default:
      //nothing to do, should never happen	
      msg("[RRC-RG][MBMS] rrc_rg_mbms_MSCH_encode : unrecognized message type \n");
      break;
  }
}

