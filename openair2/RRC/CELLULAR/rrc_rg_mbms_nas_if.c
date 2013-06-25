/***************************************************************************
                          rrc_rg_mbms_nas_if.c - description
                          -------------------
    begin               : Jul 8, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by		: Huu-Nghia.Nguyen@eurecom.fr	
    modified by        	: Michelle.Wetterwald@eurecom.fr 
 **************************************************************************
      This file contains the MBMS functions used to interface the NAS
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
 * @brief Called to handle the NAS primitive MBMS_BEARER_ESTABLISH_REQ
 * @date Jul 8, 2005 10:27:33 AM
 */
 //-----------------------------------------------------------------------------
void rrc_rg_mbms_NAS_ServStart_rx(void){
//-----------------------------------------------------------------------------
#ifdef ALLOW_MBMS_PROTOCOL
  #ifdef DEBUG_RRC_STATE
    msg("\033[0;31m[RRC-RG][MBMS][IF] Processing MBMS_BEARER_ESTABLISH_REQ (service id = %d, ....)  at frame %d \033[0m\n", 
             p_rg_mbms->nas_serviceId, protocol_bs->rrc.current_SFN);
  #endif

  //We process this primitive if it doesn't started before
  if (rrc_mbms_service_id_find(p_rg_mbms->buff_mod_numService, &p_rg_mbms->buff_mod_serviceIdentity, p_rg_mbms->nas_serviceId) < 0 
        && rrc_mbms_service_id_find(p_rg_mbms->umod_numService, &p_rg_mbms->umod_serviceIdentity, p_rg_mbms->nas_serviceId) < 0){
    // MW : in integration mode, it is done on positive answer from RRM
    #ifdef MBMS_TEST_MODE
    msg("[RRC-RG][MBMS] Activate Service - MBMS_TEST_MODE\n");
    //Update the content for Notification and Unmodif message, set the flags if having changes
    p_rg_mbms->flags.modifiedServicesInformation |= rrc_rg_mbms_addModifService(-1, p_rg_mbms->nas_serviceId, Mod_acquirePTM_RBInfo);
    // increment the number of active services in the RG (TEMP Max =1)
    p_rg_mbms->mbms_num_active_service ++;
    #endif	
    //Get RB Information from RRM...
    p_rg_mbms->ptm_requested_action = E_ADD_RB;
    RRC_RG_MBMS_O_GET_RB_INFORMATION(E_ADD_RB);
  }
#endif
}

/**
 * @brief Called to handle the NAS primitive MBMS_BEARER_RELEASE_REQ
 * @date Jul 8, 2005 10:27:59 AM
 */
 //-----------------------------------------------------------------------------
void rrc_rg_mbms_NAS_ServStop_rx(void){
//-----------------------------------------------------------------------------
#ifdef ALLOW_MBMS_PROTOCOL
  int index;

  #ifdef DEBUG_RRC_STATE
   msg("\033[0;31m[RRC-RG][MBMS][IF] Processing MBMS_BEARER_RELEASE_REQ (service id = %d, ....) at frame %d \033[0m\n", 
        p_rg_mbms->nas_serviceId, protocol_bs->rrc.current_SFN);
  #endif

  //Update the content for Notification and Unmodif message, set the flags if having changes
  index = rrc_mbms_service_id_find(p_rg_mbms->buff_mod_numService, &p_rg_mbms->buff_mod_serviceIdentity, p_rg_mbms->nas_serviceId);
  #ifdef DEBUG_RRC_MBMS
  msg("[RRC-RG][MBMS][IF] index returned %d for Modif services\n", index);
  #endif
  p_rg_mbms->flags.modifiedServicesInformation |= rrc_rg_mbms_addModifService(index, p_rg_mbms->nas_serviceId, Mod_releasePTM_RB);

  if (index < 0){  //If the service didn't exist in modif --> we may have to delete from unmodif services
    index = rrc_mbms_service_id_find(p_rg_mbms->umod_numService, &p_rg_mbms->umod_serviceIdentity, p_rg_mbms->nas_serviceId);
    msg("[RRC-RG][MBMS][IF] index returned %d for Unmodif services\n", index);
    if (index >= 0) p_rg_mbms->flags.unmodifiedServicesInformation |= rrc_rg_mbms_deleteUnmodifService(index);
  }

  //If the message has been started before.
  if (index >= 0){
    // decrement the number of active services in the RG (TEMP Max =1)
    p_rg_mbms->mbms_num_active_service --;
    //Get RB Information from RRM...
    p_rg_mbms->ptm_requested_action = E_REL_RB;
    RRC_RG_MBMS_O_GET_RB_INFORMATION(E_REL_RB);
  }
#endif
}

/**
 * @brief Called to handle the NAS primitive MBMS_UE_NOTIFY_REQ
 * @author nguyenhn
 * @date Jul 29, 2005 10:25:36 AM
 */	
 //-----------------------------------------------------------------------------
void rrc_rg_mbms_NAS_Notification_rx(void){
 //-----------------------------------------------------------------------------
#ifdef ALLOW_MBMS_PROTOCOL
  int i;
  int ueID;

  ueID = p_rg_mbms->nas_ueID;	
  //DEBUG INFO
  #ifdef DEBUG_RRC_STATE
    msg("\033[0;31m[RRC-RG][MBMS][IF] Processing MBMS_UE_NOTIFY_REQ: (UE_id = %d, ", ueID);		
    msg("joined services = {");
    for (i = 0; i<maxMBMSServices && p_rg_mbms->nas_joinedServices[i] != -1; i++){
      if ( i != 0 ) 
        msg(", %d", p_rg_mbms->nas_joinedServices[i]);
      else 
        msg("%d", p_rg_mbms->nas_joinedServices[i]);
    }
    msg("}, left services = {");
    for (i = 0; i<maxMBMSServices && p_rg_mbms->nas_leftServices[i] != -1; i++){
      if ( i != 0 ) 
        msg(", %d", p_rg_mbms->nas_leftServices[i]);
      else 
        msg("%d", p_rg_mbms->nas_leftServices[i]);
    }
    msg("} ) at frame %d \033[0m \n", protocol_bs->rrc.current_SFN);
  #endif
  if (p_rg_mbms->nas_joinedServices[0] == -1 && p_rg_mbms->nas_leftServices[0] == -1){
    // No service was provided in the NAS primitive
    #ifdef DEBUG_RRC_MBMS
     msg("[RRC-RG][MBMS][IF] DCCH Modified Service Information Message without services! Ignore\n");
    #endif 		
  }else{	
    //Copy all params Joined/Left services to mod_xxx to create the message Notification
    p_rg_mbms->mod_numService = 0;	
    rrc_rg_mbms_processJoinedServices();
    rrc_rg_mbms_processLeftServices();
    //Create and send the notification on DCCH if there is a service.
    if (p_rg_mbms->mod_numService > 0) 
      rrc_rg_mbms_DCCH_tx(p_rg_mbms->nas_ueID);	
    //Send NAS a MBMS_UE_NOTIFY_CNF	
    RRC_RG_MBMS_O_UE_NOTIFY_CNF();		
  }	
#endif
}

