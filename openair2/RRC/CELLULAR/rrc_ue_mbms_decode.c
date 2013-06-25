/***************************************************************************
                          rrc_ue_mbms_decode.c - description
                          -------------------
    begin               : May 30, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by          : Huu-Nghia.Nguyen@eurecom.fr	
    modified by        	: Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Procedures to Decode received MBMS messages on MCCH, DCCH, MSCH
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
#include "rrc_mbms_pdus.h"
#include "rrc_msg_class.h"
#include "rrc_ue_mbms_variables.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_mbms.h"
#include "rrc_proto_int.h"

//-----------------------------------------------------------------------------
// Generic function to decode RRC messages received on MCCH
//void rrc_ue_mcch_decode (mem_block_t * sduP, u16 length){
void rrc_ue_mcch_decode (char * sduP, int offset){
//-----------------------------------------------------------------------------
  MCCH_Message * mcch_msg;
  int  status = SUCCESS;
  #ifdef DEBUG_RRC_MBMS_DETAIL
  msg ("\n[RRC_UE][MBMS][DECODE]Decoding from MCCH -- start of message: \n");
  rrc_print_buffer (&sduP[offset], 15);
  #endif
  mcch_msg = (MCCH_Message *)&sduP[offset];
  #ifdef DEBUG_RRC_MBMS
  msg("\033[0;32m[RRC-UE][MBMS][RX] ");
  switch (mcch_msg->message.type) {
    case MCCH_mbmsAccessInformation:
    	msg("AccessInformation"); break;
    case MCCH_mbmsCommonPTMRBInformation:
    	msg("CommonPTMRBInformation"); break;
    case MCCH_mbmsCurrentCellPTMRBInformation:
    	msg("CurrentCellPTMRBInformation"); break;
    case MCCH_mbmsGeneralInformation:
    	msg("GeneralInformation"); break;
    case MCCH_mbmsModifiedServicesInformation:
    	msg("ModifiedServicesInformation"); break;
    case MCCH_mbmsNeighbouringCellPTMRBInformation:
    	msg("NeighbouringCellPTMRBInformation"); break;
    case MCCH_mbmsUnmodifiedServicesInformation:
    	msg("UnmodifiedServicesInformation"); break;
    default:
        msg("ERROR : Unrecognized message type %d", mcch_msg->message.type);
  }
  msg(", MCCH , at frame %d\n\033[0m", protocol_ms->rrc.current_SFN);
  #endif
  status = rrc_ue_mbms_MCCH_decode(mcch_msg);

  if (status!=SUCCESS)
    msg("\n[RRC-UE][MBMS][DECODE]Message from MCCH could not be decoded. %d \n", mcch_msg->message.type);
}

/**
 * @brief Used to decode the MCCH Messages, the decoded content is stored in p_ue_mbms->...
 * @date May 20, 2005 11:18:55 AM
 * @param mcch_msg_ptr: pointer of message MCCH to be decoded
 * @param length: length of data read on MCCH (length of the messsage)
 */
//-----------------------------------------------------------------------------
int rrc_ue_mbms_MCCH_decode(MCCH_Message *mcch_msg_ptr){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int index = 0; // will return the index (in the list) where the neigbouring rb info is inserted

  if (!mcch_msg_ptr){
    msg("[RRC-UE][MBMS] rrc_ue_mbms_MCCH_decode : mcch_msg_ptr is NULL\n");
    status = FAILURE;
    return status;
  }
  //Decode message
  switch (mcch_msg_ptr->message.type){
    //----------------
    case MCCH_mbmsAccessInformation:
      status = rrc_PERDec_MBMSAccessInformation((MBMSAccessInformation*) &mcch_msg_ptr->message.content);
      break;
    //----------------
    case MCCH_mbmsCommonPTMRBInformation:
      if (p_ue_mbms->flags.commonPTMRBInformation){
        #ifdef DEBUG_RRC_MBMS_DETAIL
        msg("[RRC-UE][MBMS] Message %s has previously been decoded in the same current modification period\n", rrc_mbms_message_getName(MCCH_ID, 0, mcch_msg_ptr->message.type));
        #endif
      }else{
        if (p_ue_mbms->mustAcquireRBInfo == 0){
          #ifdef DEBUG_RRC_MBMS_DETAIL
          msg("[RRC-UE][MBMS] No need to acquire RB Information\n");
          #endif
        }else{
          #ifdef DEBUG_RRC_MBMS_DETAIL
          msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_MCCH_decode is now processing the message CommonPTMRBInformation \n");
          #endif
          status = rrc_PERDec_MBMSCommonPTMRBInformation((MBMSCommonPTMRBInformation*) &mcch_msg_ptr->message.content);
          if (status == P_SUCCESS){
            p_ue_mbms->flags.commonPTMRBInformation = TRUE;
            RRC_UE_MBMS_I_COMMON_CELL_RB_INFO();
            rrc_ue_mbms_fsm();
          }
        }
      }
      break;
    //----------------
    case MCCH_mbmsCurrentCellPTMRBInformation:
      if (p_ue_mbms->flags.currentCellPTMRBInformation){
        #ifdef DEBUG_RRC_MBMS_DETAIL
        msg("[RRC-UE][MBMS] Message %s has previously been decoded in the same current modification period\n", rrc_mbms_message_getName(MCCH_ID,0 , mcch_msg_ptr->message.type));
        #endif
      }else{
        if (p_ue_mbms->mustAcquireRBInfo == 0){
          #ifdef DEBUG_RRC_MBMS_DETAIL
          msg("[RRC-UE][MBMS] No need to acquire RB Information\n");
          #endif
        }else{
          #ifdef DEBUG_RRC_MBMS_DETAIL
          msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_MCCH_decode is now processing the message CurrentCellPTMRBInformation \n");	
          #endif
          status = rrc_PERDec_MBMSCurrentCellPTMRBInformation((MBMSCurrentCellPTMRBInformation*) &mcch_msg_ptr->message.content);
          if (status == P_SUCCESS){
            p_ue_mbms->flags.currentCellPTMRBInformation = TRUE;
            RRC_UE_MBMS_I_CURRENT_CELL_RB_INFO();
            rrc_ue_mbms_fsm();
          }
        }
      }
      break;
    //----------------
    case MCCH_mbmsGeneralInformation:
      if (p_ue_mbms->flags.generalInformation){
        #ifdef DEBUG_RRC_MBMS_DETAIL
        msg("[RRC-UE][MBMS] Message %s has previously been decoded in the same current modification period\n", rrc_mbms_message_getName(MCCH_ID,0 , mcch_msg_ptr->message.type));
        #endif
      }else{
        #ifdef DEBUG_RRC_MBMS_DETAIL
        msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_MCCH_decode is now processing the message GeneralInformation \n");
        #endif
        status = rrc_PERDec_MBMSGeneralInformation((MBMSGeneralInformation*) &mcch_msg_ptr->message.content);
        if (status == P_SUCCESS)
          p_ue_mbms->flags.generalInformation = 1;
      }
      break;
    //----------------
    case MCCH_mbmsModifiedServicesInformation:
      if (p_ue_mbms->flags.modifiedServicesInformation){
        #ifdef DEBUG_RRC_MBMS_DETAIL
        msg("[RRC-UE][MBMS] Message %s has previously been decoded in the same current modification period\n", rrc_mbms_message_getName(MCCH_ID,0 , mcch_msg_ptr->message.type));
        #endif
      }else{
        #ifdef DEBUG_RRC_MBMS_DETAIL
        msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_MCCH_decode is now processing the message ModifiedServicesInformation \n");	
        #endif
        status = rrc_PERDec_MBMSModifiedServicesInformation((MBMSModifiedServicesInformation*) &mcch_msg_ptr->message.content);
        if (status == P_SUCCESS){
          p_ue_mbms->flags.modifiedServicesInformation = TRUE;
          RRC_UE_MBMS_I_MCCH_MODIF_SERV_INFO();
          if (p_ue_mbms->mod_all_unmodified_ptm_services)
            RRC_UE_MBMS_I_ALL_UNMODIF_PTM_SERVICES();
          rrc_ue_mbms_fsm();
        }
      }
      break;
    //----------------
    case MCCH_mbmsNeighbouringCellPTMRBInformation:
      #ifdef DEBUG_RRC_MBMS_DETAIL
      msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_MCCH_decode is now processing the message NeighbouringCellPTMRBInformation \n");
      #endif
      status = rrc_PERDec_MBMSNeighbouringCellPTMRBInformation(&index, (MBMSNeighbouringCellPTMRBInformation*) &mcch_msg_ptr->message.content);
      if (status == P_SUCCESS){
        RRC_UE_MBMS_I_NEIGHBOURING_CELL_RB_INFO();
        rrc_ue_mbms_fsm();
      }
      break;
    //----------------			
    case MCCH_mbmsUnmodifiedServicesInformation:
      if (p_ue_mbms->flags.unmodifiedServicesInformation){
        #ifdef DEBUG_RRC_MBMS_DETAIL
        msg("[RRC-UE][MBMS] Message %s has previously been decoded in the same current modification period\n", rrc_mbms_message_getName(MCCH_ID, 0 , mcch_msg_ptr->message.type));
        #endif
      }else{ 
        if (!p_ue_mbms->flags.modifiedServicesInformation){
          #ifdef DEBUG_RRC_MBMS_DETAIL
          msg("[RRC-UE][MBMS] Do nothing because there has not been any ModifiedServiceInformation message.\n");
          #endif
        }else{
          #ifdef DEBUG_RRC_MBMS_DETAIL
          msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_MCCH_decode is now processing the message UnmodifiedServicesInformation \n");	
          #endif
          status = rrc_PERDec_MBMSUnmodifiedServicesInformation((MBMSUnmodifiedServicesInformation*) &mcch_msg_ptr->message.content);
          if (status == P_SUCCESS){
            p_ue_mbms->flags.unmodifiedServicesInformation = TRUE;
            RRC_UE_MBMS_I_UNMODIF_SERV_INFO();
            rrc_ue_mbms_fsm();
          }
        }
      }
      break;
    default:
      //nothing to do
      status = FAILURE;
  }
  return status;
}

/**
 * @brief Used to decode the MBMSModificationServicesInformation on DCCH
 * the decoded content is stored in p_ue_mbms->...
 * @date May 20, 2005 
 * @param dl_dcch_msg: message DCCH - MBMSModificationServicesInformation to be decoded
 * @param length: length of data read on DCCH (length of the messsage)
 */ 
//-----------------------------------------------------------------------------
int rrc_ue_mbms_DCCH_decode(DL_DCCH_Message *dl_dcch_msg_ptr){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  if (!dl_dcch_msg_ptr){
    msg("[RRC-UE][MBMS] rrc_ue_mbms_DCCH_decode : dl_dcch_msg_ptr is NULL\n");
    status = FAILURE;
    return status;
  }
  #ifdef DEBUG_RRC_MBMS_DETAIL
  msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_DCCH_decode is now processing the message MBMSModifiedServicesInformation\n");	
  #endif

  //Decode message
  switch (dl_dcch_msg_ptr->message.type) {
    case DL_DCCH_mbmsModifiedServicesInformation:
      //We always decode this message
      status = rrc_PERDec_MBMSModifiedServicesInformation((MBMSModifiedServicesInformation*) & dl_dcch_msg_ptr->message.content);
      if (status == P_SUCCESS){
        RRC_UE_MBMS_I_MODIF_SERV_INFO();
        if (p_ue_mbms->mod_all_unmodified_ptm_services){
          RRC_UE_MBMS_I_ALL_UNMODIF_PTM_SERVICES();
          //p_ue_mbms->flags.unmodifiedServicesInformation = FALSE; //should reacquire UnmodifiedServiceInfo on MCCH.								
        }
        rrc_ue_mbms_fsm();
        //p_ue_mbms->flags.modifiedServicesInformation = TRUE; //don't need anymore to acquire ModifiedServiceInfo on MCCH.
      }	
      break;
    default:
      status = FAILURE;
      //nothing to do
  }
  return status;
}

/**
 * @brief Used to decode the messages on MSCH
 * the decoded content is stored in p_ue_mbms->...
 * @date May 30, 2005 4:08:30 PM
 * @param msch_msg_ptr: message MSCH - MSCHSchedulingInformation to be decoded
 * @param length: length of data read on MSCH (length of the messsage)
 */ 
//-----------------------------------------------------------------------------
int rrc_ue_mbms_MSCH_decode(MSCH_Message *msch_msg_ptr){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;

  if (!msch_msg_ptr){
    msg("[RRC-UE][MBMS] rrc_ue_mbms_MSCH_decode : msch_msg_ptr is NULL\n");
    status = FAILURE;
    return status;
  }
  //decode msch message	
  #ifdef DEBUG_RRC_MBMS_DETAIL
  msg("[RRC-UE][MBMS] Procedure rrc_ue_mbms_MSCH_decode is now processing the message MSCHSchedulingInformation\n");	
  #endif

  //Decode message
  switch (msch_msg_ptr->message.type) {
    case MSCH_mbmsSchedulingInformation:
      status = rrc_PERDec_MBMSSchedulingInformation( (MBMSSchedulingInformation*) & msch_msg_ptr->message.content);
      break;
    default:
      //nothing to do
      status = FAILURE;
  }

return status;
}
