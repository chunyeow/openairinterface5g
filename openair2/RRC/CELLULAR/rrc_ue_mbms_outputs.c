/***************************************************************************
                          rrc_ue_mbms_outputs.c - description
                          -------------------
    begin               : Jun 1, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by          : Huu-Nghia.Nguyen@eurecom.fr	
    modified by         : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
    This file implements the UE's output signals
 **************************************************************************/
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

#ifdef MBMS_INTEGRATION_MODE
#include "rrc_messages.h"
#include "rrc_nas_primitives.h"
#include "rrc_proto_int.h"
#endif

#ifdef DEBUG_UE_MBMS_FSM_TEST
extern mbms_bool O_NAS_MBMS_UE_NOTIFY_IND;
extern mbms_bool O_ANALYSE_UNMODIF;
extern mbms_bool O_CURRENT_CELL_RB_CONFIGURATION;
extern mbms_bool O_NEIGHBOURING_CELL_RB_CONFIGURATION;
extern mbms_bool O_MCCH_NOTIFICATION;
extern mbms_bool O_DCCH_NOTIFICATION;
#endif
//-----------------------------------------------------------------------------

/**
 * @brief briefdescription
 * @date Aug 24, 2005 10:45:47 AM
 */
//-----------------------------------------------------------------------------
void rrc_ue_mbms_saveMCCHModifiedServiceInformation(void){
//-----------------------------------------------------------------------------
  p_ue_mbms->saved_mod_all_unmodified_ptm_services = p_ue_mbms->mod_all_unmodified_ptm_services;
  //p_ue_mbms->saved_mod_endOfModifiedMCCHInformation = p_ue_mbms->mod_endOfModifiedMCCHInformation;
  p_ue_mbms->saved_mod_numService = p_ue_mbms->mod_numService;
  p_ue_mbms->saved_mod_reacquireMCCH = p_ue_mbms->mod_reacquireMCCH;
  memcpy(&p_ue_mbms->saved_mod_requiredUEAction, &p_ue_mbms->mod_requiredUEAction, p_ue_mbms->mod_numService * sizeof(u8));
  memcpy(&p_ue_mbms->saved_mod_serviceIdentity, &p_ue_mbms->mod_serviceIdentity, p_ue_mbms->mod_numService * sizeof(o3));
}

 /**
 * @brief Search for the element in the list p_ue_mbms->activatedServiceList.
 * if not found --> insert this element to the list.
 * @return the position in the list where the service is inserted: POSITIVE if inserted or modified, NEGATIVE if not.
 * @author nguyenhn
 * @date Jul 28, 2005 4:26:40 PM
 */
//----------------------------------------------------------------------------- 
int rrc_ue_mbms_addActivatedService(o3 serviceID){
//-----------------------------------------------------------------------------
  int index;
  index  = rrc_mbms_service_id_find(p_ue_mbms->act_numService, &p_ue_mbms->act_serviceIdentity, serviceID);
  if (index < 0){ // we will add
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] Add the service %d to the MBMS Activated Services List\n", serviceID);
    #endif
    p_ue_mbms->act_serviceIdentity[p_ue_mbms->act_numService] = serviceID;
    p_ue_mbms->act_serviceType[p_ue_mbms->act_numService] = MBMS_MULTICAST; //Impl default	
    p_ue_mbms->act_activated[p_ue_mbms->act_numService] = FALSE;
    index = p_ue_mbms->act_numService++;
  }else{
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] The service %d exists in the MBMS Activated Services List (position = %d)\n", serviceID, index);
    #endif
  }
  return index;
}

 /**
 * @brief Search for the element in the list p_ue_mbms->activatedServiceList
 * if found --> delete this element from the list.
 * @return the deleted position: POSITIVE if deleted, NEGATIVE if not.
 * @author nguyenhn
 * @date Jul 28, 2005 4:27:01 PM
 */
 //-----------------------------------------------------------------------------
int rrc_ue_mbms_deleteActivatedService(o3 serviceID){
//-----------------------------------------------------------------------------
  int index, rest;

  index  = rrc_mbms_service_id_find(p_ue_mbms->act_numService, &p_ue_mbms->act_serviceIdentity, serviceID);
  if (index >= 0){ // we will delete
  //Operation delete is defined as moving 1 to the left all the element: index + 1, index + 2....
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] Delete the service %d from the MBMS Activated Services List\n", serviceID);
    #endif

    rest = p_ue_mbms->act_numService - (index + 1);
    if (rest>0){
      memmove(&p_ue_mbms->act_serviceIdentity[index], &p_ue_mbms->act_serviceIdentity[index+1], rest * sizeof(o3));		
      memmove(&p_ue_mbms->act_activated[index], &p_ue_mbms->act_activated[index+1], rest*sizeof(mbms_bool));
      memmove(&p_ue_mbms->act_serviceType[index], &p_ue_mbms->act_serviceType[index+1], rest*sizeof(u8));
    }
    p_ue_mbms->act_numService--;
  }
  return index;	
}

/**
 * @brief Add the service pointed by pserviceID into the variable p_ue_mbms->nas_joinedServices.
 * No need to check for the existance of this service in the variable
 * @author nguyenhn
 * @date Aug 23, 2005 4:22:20 PM
 */
//----------------------------------------------------------------------------- 
void rrc_ue_mbms_addJoinedService(o3 serviceID){
//-----------------------------------------------------------------------------
  if (p_ue_mbms->nas_joinedCount < maxMBMSServices){
    p_ue_mbms->nas_joinedServices[p_ue_mbms->nas_joinedCount++] = serviceID;
    p_ue_mbms->nas_joinedServices[p_ue_mbms->nas_joinedCount] = -1;
  }else{
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS][ERROR] Joined Services Overflow !\n");
    #endif
  }
}

/**
 * @brief add the service pointed by pserviceID into the variable p_ue_mbms->nas_leftServices
 * No need to check for the existance of this service in the variable
 * @author nguyenhn
 * @date Aug 23, 2005 4:22:54 PM
 */
//----------------------------------------------------------------------------- 
void rrc_ue_mbms_addLeftService(o3 serviceID){
//-----------------------------------------------------------------------------
  if (p_ue_mbms->nas_leftCount < maxMBMSServices){
    p_ue_mbms->nas_leftServices[p_ue_mbms->nas_leftCount++] = serviceID;
    p_ue_mbms->nas_leftServices[p_ue_mbms->nas_leftCount] = -1;
  }else{
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS][ERROR] Left Services Overflow !\n");
    #endif
  }
}

/**
 * @brief Activate the service index in the list of MBMS Activated Services (joined services)
 * @author nguyenhn
 * @date Aug 24, 2005 9:57:05 AM
 */
//----------------------------------------------------------------------------- 
void rrc_ue_mbms_activateService(int index){
//----------------------------------------------------------------------------- 
  if (index < p_ue_mbms->act_numService){
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] Activate the service %d in L1-L2\n", p_ue_mbms->act_serviceIdentity[index]);
    #endif
    // Uncompress the stored configuration
    rrc_uncompress_config ((char *) &(rrm_config->mt_config), (char *) (&(protocol_ms->rrc.mbms.curr_l12Config_data)));
    // TEMP :
    //   Until further checking, this prevents the start of a unicast resource
    //   between activation in RG and activation in MT.
    //
    //activate the config with Unicast procedures
    //FACH is used, no need for PHY Config
    RRC_RB_UE_O_O_CMAC_CONFIG_Req();
    RRC_RB_UE_O_O_CRLC_CONFIG_Req ();
    set_RRC_Transaction_ID();
    //
    p_ue_mbms->act_activated[index] = TRUE;
  }else{
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] Activate: MBMS Activated Service out of range - %d is out of 0..%d", index, p_ue_mbms->act_numService);
    #endif
  }
}

/**
 * @brief Deactivate the service index in the list of MBMS Activated Services (joined services)
 * @author nguyenhn
 * @date Aug 24, 2005 10:01:12 AM
 */
//----------------------------------------------------------------------------- 
void rrc_ue_mbms_deactivateService(int index){
//----------------------------------------------------------------------------- 
  if (index < p_ue_mbms->act_numService){
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] Deactivate the service %d in L1-L2\n", p_ue_mbms->act_serviceIdentity[index]);
    #endif				
    p_ue_mbms->act_activated[index] = FALSE;
  }else{
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] Deactivate: MBMS Activated Service out of range - %d is out of 0..%d", index, p_ue_mbms->act_numService);
    #endif			
  }
}

/**
 * @brief Process the joined services list from NAS (3GPP) / Network Notification (Daidalos)
 * @author nguyenhn
 * @date Aug 24, 2005 10:07:31 AM
 */
//----------------------------------------------------------------------------- 
void rrc_ue_mbms_processJoinedServices(void){
//----------------------------------------------------------------------------- 
  int i, index;

  for (i = 0; i<p_ue_mbms->nas_joinedCount; i++){
    index = rrc_ue_mbms_addActivatedService(p_ue_mbms->nas_joinedServices[i]);
    //If This service has been joined before --> this may be a handover
/*    Adapted to DAIDALOS
    this is a joined service, so we check if this service is currently being transferred & the configuration is available
    If "yes", we activate immediately the service if we could find it in the list of Modified Services or Unmodified Services.
    If the service started in last modification periods --> configuration is available
    If in this modification period --> must wait until the configuration info is available.
*/
    if ((p_ue_mbms->flags.currentCellPTMRBInformation
          && rrc_mbms_service_id_find(p_ue_mbms->saved_mod_numService,&p_ue_mbms->saved_mod_serviceIdentity,p_ue_mbms->nas_joinedServices[i]) >= 0)
          ||
          rrc_mbms_service_id_find(p_ue_mbms->umod_numService, &p_ue_mbms->umod_serviceIdentity,p_ue_mbms->nas_joinedServices[i]) >= 0){
      rrc_ue_mbms_activateService(index);
    }
  }
}

/**
 * @brief Process the left services list from NAS (3GPP) / Network Notification (Daidalos)
 * @author nguyenhn
 * @date Aug 24, 2005 10:23:12 AM
 */
//----------------------------------------------------------------------------- 
void rrc_ue_mbms_processLeftServices(void){
//----------------------------------------------------------------------------- 
  int i, index;
  for (i = 0; i<p_ue_mbms->nas_leftCount; i++){
  //This service is being transferred on the network --> deactivate now
    if (rrc_mbms_service_id_find(p_ue_mbms->saved_mod_numService, &p_ue_mbms->saved_mod_serviceIdentity,p_ue_mbms->nas_leftServices[i])
         || rrc_mbms_service_id_find(p_ue_mbms->umod_numService, &p_ue_mbms->umod_serviceIdentity,p_ue_mbms->nas_leftServices[i])){
      index = rrc_mbms_service_id_find(p_ue_mbms->act_numService, &p_ue_mbms->act_serviceIdentity, p_ue_mbms->nas_leftServices[i]) ;
      if (index >= 0 && p_ue_mbms->act_activated[index])
        rrc_ue_mbms_deactivateService(index);
    }
    rrc_ue_mbms_deleteActivatedService(p_ue_mbms->nas_leftServices[i]);
  }
}


/**
 * @brief inform NAS about services left/join by sending MBMS_UE_NOTIFY_IND
 * @date Jun 3, 2005 9:45:15 AM
 * @param serviceID id of the service to be stopped
 */
//-----------------------------------------------------------------------------
void RRC_UE_MBMS_O_NAS_MBMS_UE_NOTIFY_IND(void){
//-----------------------------------------------------------------------------
  #ifdef MBMS_INTEGRATION_MODE
  struct nas_ue_if_element *msgToBuild;
  int i;
  mem_block_t *p = get_free_mem_block(sizeof (struct nas_ue_if_element));

  protocol_ms->rrc.NASMessageToXmit = p;  // Temp - will later enqueue at bottom of list		
  //Set pointer to newly allocated structure and fills it
  msgToBuild = (struct nas_ue_if_element *) p->data;
  msgToBuild->prim_length = NAS_TL_SIZE + sizeof(struct NASMBMSUENotifyInd);
  msgToBuild->xmit_fifo = protocol_ms->rrc.rrc_ue_DCOut_fifo;
  msgToBuild->nasUePrimitive.dc_sap_prim.type = MBMS_UE_NOTIFY_IND;
  msgToBuild->nasUePrimitive.dc_sap_prim.length = msgToBuild->prim_length;

  //Fill the list		
  for (i = 0; i<MAX_MBMS_SERVICES; i++){
    msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.mbms_ue_notify_ind.joined_services[i].mbms_serviceId = (u32) -1;
    msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.mbms_ue_notify_ind.left_services[i].mbms_serviceId = (u32) -1;
  }
  for (i = 0; i<p_ue_mbms->nas_joinedCount; i++)
    msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.mbms_ue_notify_ind.joined_services[i].mbms_serviceId = (u32) p_ue_mbms->nas_joinedServices[i];
  for (i = 0; i<p_ue_mbms->nas_leftCount; i++)
    msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.mbms_ue_notify_ind.left_services[i].mbms_serviceId = (u32) p_ue_mbms->nas_leftServices[i];
  //msgToBuild->nasUePrimitive.dc_sap_prim.nasUEDCPrimitive.mbms_ue_notify_ind.localConnectionRef	

  #ifdef DEBUG_RRC_STATE	
    msg("[RRC-UE][MBMS] MBMS_UE_NOTIFY_IND primitive sent to NAS, length %d\n",  msgToBuild->prim_length);
  #endif
  #endif

  #ifdef MBMS_TEST_MODE
  #ifdef DEBUG_RRC_STATE	
  msg("[RRC-UE][MBMS] MBMS_UE_NOTIFY_IND primitive sent to NAS\n");
  #endif
  #endif

  #ifdef DEBUG_UE_MBMS_FSM_TEST	
  O_NAS_MBMS_UE_NOTIFY_IND = TRUE;
  #endif
}

/**
 * @brief Analyse the message Unmodified Service Information if required in Modified Service Information
 * @author nguyenhn
 * @date Jul 29, 2005 10:47:27 AM
 */
 //-----------------------------------------------------------------------------
void RRC_UE_MBMS_O_ANALYSE_UNMODIF(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg("\033[0;31m[RRC-UE][MBMS][FSM] Analysing the required actions in Unmodified Service Information (as required in Modified Service Information) ...\033[0m\n");
  #endif

  #ifdef DEBUG_UE_MBMS_FSM_TEST
  O_ANALYSE_UNMODIF = TRUE;
  #endif


  //TODO Code here
}

/**
 * @brief activate the PTM RB Configuration procedure for the current cell
 * @date Jun 9, 2005 4:40:57 PM
 */
//-----------------------------------------------------------------------------
void RRC_UE_MBMS_O_CURRENT_CELL_RB_CONFIGURATION(void){
//-----------------------------------------------------------------------------
  int i, index;

  #ifdef DEBUG_RRC_STATE
  msg("\033[0;31m[RRC-UE][MBMS][FSM] Processing the cell configuration  at frame %d \033[0m\n", protocol_ms->rrc.current_SFN);
  #endif

  #ifdef DEBUG_UE_MBMS_FSM_TEST
  O_CURRENT_CELL_RB_CONFIGURATION = TRUE;	
  #endif

  // Complete the initialisation process.
  if (!p_ue_mbms->initialised)
    p_ue_mbms->initialised = TRUE;
    // Do as specified in 8.7.5.3. -- MBMS Modified service was received
    // Start : continue receiving the indicated p-t-m radio bearers
    // depending on its UE capabilities
  for (i = 0; i< p_ue_mbms->saved_mod_numService; i++){
    if (p_ue_mbms->saved_mod_requiredUEAction[i] == Mod_acquirePTM_RBInfo){
      //Check if the service has been joined before. If yes --> activate the service.
      index = rrc_mbms_service_id_find(p_ue_mbms->act_numService, &p_ue_mbms->act_serviceIdentity, p_ue_mbms->saved_mod_serviceIdentity[i]) ;
      if (index >= 0 && !p_ue_mbms->act_activated[index])
        rrc_ue_mbms_activateService(index);	
    }
  }
}

/**
 * @brief Activate the PTM RB Configuration procedure for the 
 * neighbouring cell. 
 * @date Jun 9, 2005 4:41:11 PM
 */
//-----------------------------------------------------------------------------
void RRC_UE_MBMS_O_NEIGHBOURING_CELL_RB_CONFIGURATION(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg("\033[0m[RRC-UE][MBMS][FSM] Processing the received neighbouring Cell Configuration at frame %d \033[0m\n", protocol_ms->rrc.current_SFN);	
  #endif

  #ifdef DEBUG_UE_MBMS_FSM_TEST
  O_NEIGHBOURING_CELL_RB_CONFIGURATION = TRUE;
  #endif
}

/**
 * @brief Process the information in the Modified Service Information message as specified in 8.7.3
 * @date Sep 6, 2005 10:30:33 AM
 */
//-----------------------------------------------------------------------------
void RRC_UE_MBMS_O_MCCH_NOTIFICATION(void){
//-----------------------------------------------------------------------------
  int i;
  int index;
  int acquireRBInfo;
  #ifdef DEBUG_RRC_MBMS
  msg("\033[0;31m[RRC-UE][MBMS][FSM] Processing the received MCCH Modified Service Information at frame %d \033[0m \n", protocol_ms->rrc.current_SFN);	
  #ifdef DEBUG_RRC_MBMS_DETAIL
  rrc_ue_mbms_modif_services_print();					
  #endif
  if (p_ue_mbms->mod_numService == 0)
    msg("[RRC-UE][MBMS] Modified Service Information doesn't contain any service, ignore!\n");
  #endif	

  #ifdef DEBUG_UE_MBMS_FSM_TEST
  O_MCCH_NOTIFICATION = TRUE;
  #endif	

  //For each of the services included in the message MODIFIED SERVICES INFORMATION
  //provided that the service is included in variable MBMS_ACTIVATED_SERVICES and upper layer
  //indicate that the session has not yet been received correctly (refered as 'applicable services'):

  // Implementation : to speed up service activation, the configuration is
  // read and stored every time it is changed (Modified Services +
  // required action = acquirePTM_RBInfo, releasePTM_RB )
  acquireRBInfo = FALSE;
  for (i = 0; i< p_ue_mbms->mod_numService; i++){
    //Do as 8.6.9		
    switch (p_ue_mbms->mod_requiredUEAction[i]){
      case Mod_acquirePTM_RBInfo:
        //do 8.7.5 if received...PTM RB Information.
        acquireRBInfo = TRUE;
        break;
      case Mod_releasePTM_RB:
        //Check if the service is activated or not. If activated, deactivate the service.
        index = rrc_mbms_service_id_find(p_ue_mbms->act_numService, &p_ue_mbms->act_serviceIdentity, p_ue_mbms->mod_serviceIdentity[i]) ;
        if (index >= 0 && p_ue_mbms->act_activated[index])
          rrc_ue_mbms_deactivateService(index);
        //Adaptation for Daidalos, we need to reacquire the RB Info
        acquireRBInfo = TRUE;
        break;
      case Mod_none:				
        //Do nothing
        break;
      default:
        msg("[RRC-UE][MBMS]MCCH Notification - requiredAction[%d] = %d not recognized\n", i, p_ue_mbms->mod_requiredUEAction[i]);
        break;
    }
  }

  if (acquireRBInfo){
    #ifdef DEBUG_RRC_STATE
    msg("[RRC-UE][MBMS] Allow the procedure MBMS PTM RB Configuration\n");	
    #endif				
    p_ue_mbms->mustAcquireRBInfo = TRUE; //We will acquire Common and Current RB Info msg.								
  }

  //The procedure end, we store all values of mod_ into saved_mod_
  rrc_ue_mbms_saveMCCHModifiedServiceInformation();	

  #ifdef DEBUG_RRC_MBMS_STATUS
  rrc_ue_mbms_status_services_print();
  #endif
}

/**
 * @brief activate the notification procedure, process the information in the DCCH MBMSModifiedServicesInfo message . 
 * The procedure is adapted to DAIDALOS. Used to inform services left/join
 * @date Jun 9, 2005 4:39:10 PM
 */
//-----------------------------------------------------------------------------
void RRC_UE_MBMS_O_DCCH_NOTIFICATION(void){
//-----------------------------------------------------------------------------
  int i;

  #ifdef DEBUG_RRC_STATE
    msg("\033[0;31m[RRC-UE][MBMS][FSM] Processing the received DCCH Notification at frame %d \033[0m\n", protocol_ms->rrc.current_SFN);		
    #ifdef DEBUG_RRC_MBMS_DETAIL
    rrc_ue_mbms_modif_services_print();
    #endif
  #endif	

  #ifdef DEBUG_UE_MBMS_FSM_TEST
  O_DCCH_NOTIFICATION = TRUE;
  #endif		
  
  //Copy all services in notification message to nas_xxx. Used for the future compability
  p_ue_mbms->nas_joinedCount = 0;
  p_ue_mbms->nas_leftCount = 0;
  for (i = 0; i< p_ue_mbms->mod_numService; i++){
    switch (p_ue_mbms->mod_requiredUEAction[i]){
      case Mod_acquirePTM_RBInfo:
        rrc_ue_mbms_addJoinedService(p_ue_mbms->mod_serviceIdentity[i]);
        break;
      case Mod_releasePTM_RB:
        rrc_ue_mbms_addLeftService(p_ue_mbms->mod_serviceIdentity[i]); 
        break;
      default:
        #ifdef DEBUG_RRC_MBMS
        msg("[RRC-UE][MBMS][WARNING] requiredAction[%d] = %d not recognized\n", i, p_ue_mbms->mod_requiredUEAction[i]);
        #endif
        break;
    }
  }

  //Process the received joined/left services
  if (p_ue_mbms->nas_joinedCount > 0) 
    rrc_ue_mbms_processJoinedServices();
  if (p_ue_mbms->nas_leftCount > 0 ) 
    rrc_ue_mbms_processLeftServices();

  //Should we send UE_NOTIFY_IND to NAS ?
  RRC_UE_MBMS_O_NAS_MBMS_UE_NOTIFY_IND(); 
}
