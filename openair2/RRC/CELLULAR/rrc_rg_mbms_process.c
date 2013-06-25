/***************************************************************************
                          rrc_rg_mbms_process.c - description
                          -------------------
    begin               : May 30, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by		: Huu-Nghia.Nguyen@eurecom.fr
    modified by        	: Michelle.Wetterwald@eurecom.fr
 **************************************************************************
      This file contains the MBMS procedures for RG
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

extern rrc_rg_mbms_variables * volatile p_rg_mbms; /** pointer reference to protocol_bs->rrc.mbms */

/**
 * @brief Initialize the RG side mbms variables, fixed value in this phase
 * @date May 27, 2005 6:44:53 PM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_init(void){
//-----------------------------------------------------------------------------
  #ifdef ALLOW_MBMS_PROTOCOL
  int i;

  #ifdef DEBUG_RRC_STATE
   msg("[RRC-RG][MBMS] START OF MBMS INITIALISATION.......................\n");
  #endif

  //Reference:
  p_rg_mbms = &protocol_bs->rrc.mbms;

  //Empty all the control buffer
  memset(p_rg_mbms, 0, sizeof(rrc_rg_mbms_variables));	

  //Init variables:
  p_rg_mbms->ptm_config_transaction_id = MBMS_MIN_TRANSACTION_ID;

  /***************************************/	
  //Init the configuration for MBMS
  //Access Information message - None at initialization
  p_rg_mbms->acc_numAccessInfo = 0;
  //General Information message
  p_rg_mbms->gen_t_318 = ms1000;  //default = 1000
  //p_rg_mbms->gen_cellGroupIdentity = 	1<<11; //should be 12 bits only
  p_rg_mbms->gen_cellGroupIdentity = MBMS_DEFAULT_CGID;

  //RB information - MW RAZ already done above
  #ifdef MBMS_TEST_MODE  // only for test mode
  strcpy((char*)&(p_rg_mbms->comm_l12Config_data), "ZZZ - This is Common L1L2 PTM Configuration\0");
  p_rg_mbms->comm_l12Config_lgth = sizeof ("ZZZ - This is Common L1L2 PTM Configuration\0");
  strcpy((char*)&(p_rg_mbms->curr_l12Config_data), "AAA - This is Current L1L2 PTM Configuration\0");
  p_rg_mbms->curr_l12Config_lgth = sizeof ("AAA - This is Current L1L2 PTM Configuration\0");
  #endif

  //sched
  p_rg_mbms->accessInfoPeriodCoef = ACCESS_PERIOD_COEF_DEFAULT;
  p_rg_mbms->repetitionPeriodCoef = REPETITION_PERIOD_COEF_DEFAULT;
  p_rg_mbms->modifPeriodCoef = MODIF_PERIOD_COEF_DEFAULT;
  p_rg_mbms->schedPeriodCoef = SCHED_PERIOD_COEF_DEFAULT;

  // MW : these variables are FFS in Modified Services message - temp initialisation here
  p_rg_mbms->mod_reacquireMCCH = TRUE;
  p_rg_mbms->mod_numberOfNeighbourCells = 0;
  p_rg_mbms->mod_ptm_activationTime = 0;
  // p_rg_mbms->mod_endOfModifiedMCCHInformation = 5;		

  // Currently, no neighbouring cell is supported
  p_rg_mbms->neighb_cellCount = 0;

  /*
  memset(p_rg_mbms->neighb_cellConfList[0],0, MBMS_NEIGHBOURING_CONFIGURATION_SIZE);
  strcpy(p_rg_mbms->neighb_cellConfList[0], "NeighbConf123456789012345678901\0");
  p_rg_mbms->neighb_cellIDList[0] = 6;

  memcpy(p_rg_mbms->neighb_cellConfList[1], data3, MBMS_NEIGHBOURING_CONFIGURATION_SIZE);
  p_rg_mbms->neighb_cellIDList[1] = 5;

  memcpy(p_rg_mbms->neighb_cellConfList[2], data3, MBMS_NEIGHBOURING_CONFIGURATION_SIZE);
  p_rg_mbms->neighb_cellIDList[2] = 4;
  */
 
  rrc_rg_mbms_MCCH_encode(MCCH_mbmsModifiedServicesInformation);	
  rrc_rg_mbms_message_update(&p_rg_mbms->messageModif);

  rrc_rg_mbms_MCCH_encode(MCCH_mbmsUnmodifiedServicesInformation);	
  rrc_rg_mbms_message_update(&p_rg_mbms->messageUnmodif);

  rrc_rg_mbms_MCCH_encode(MCCH_mbmsGeneralInformation);
  rrc_rg_mbms_message_update(&p_rg_mbms->messageGeneral);

  /*
  rrc_rg_mbms_MCCH_encode(MCCH_mbmsCommonPTMRBInformation);
  rrc_rg_mbms_message_update(&p_rg_mbms->messageCommon);

  rrc_rg_mbms_MCCH_encode(MCCH_mbmsCurrentCellPTMRBInformation);
  rrc_rg_mbms_message_update(&p_rg_mbms->messageCurrent);
  */

  for (i = 0; i<p_rg_mbms->neighb_cellCount; i++){
    p_rg_mbms->neighb_index = i; //Create the message for the ith neighbour
    rrc_rg_mbms_MCCH_encode(MCCH_mbmsNeighbouringCellPTMRBInformation);
    rrc_rg_mbms_message_update(&p_rg_mbms->messageNeighbouring[i]);
  }	

  p_rg_mbms->msgToSend_ptr = NULL;
  p_rg_mbms->msgToSend_type = 0;

  #ifdef DEBUG_RRC_STATE
  msg("[RRC-RG][MBMS] END OF MBMS INITIALISATION\n\n");
  #endif	
#endif
}

/**
 * @brief Used to schedule the order of MBMS messages as specified in TS 25.331#8.7.1
 * @date May 20, 2005 11:32:58 AM
 * return the index of the message in the message info list
 */
//-----------------------------------------------------------------------------
int rrc_rg_mbms_MCCH_next_message(void){
//-----------------------------------------------------------------------------
  switch(p_rg_mbms->msgToSend_type){
    case 0: 
      p_rg_mbms->msgToSend_type = MCCH_mbmsModifiedServicesInformation;
      p_rg_mbms->msgToSend_ptr = & p_rg_mbms->messageModif;		
      break;
    case MCCH_mbmsModifiedServicesInformation:
      p_rg_mbms->msgToSend_type = MCCH_mbmsUnmodifiedServicesInformation;
      p_rg_mbms->msgToSend_ptr = & p_rg_mbms->messageUnmodif;
      break;
    case MCCH_mbmsUnmodifiedServicesInformation:
      p_rg_mbms->msgToSend_type = MCCH_mbmsGeneralInformation;
      p_rg_mbms->msgToSend_ptr = & p_rg_mbms->messageGeneral;
      break;
    case MCCH_mbmsGeneralInformation:
      if (p_rg_mbms->allowCellRBInfo){
        p_rg_mbms->msgToSend_type = MCCH_mbmsCommonPTMRBInformation;
        p_rg_mbms->msgToSend_ptr = & p_rg_mbms->messageCommon;
        break;
      } 
      //else, we ignore the common/current RB info, and send neigbouring cell RB Info. No break
    case MCCH_mbmsCommonPTMRBInformation:
      if (p_rg_mbms->allowCellRBInfo){
        p_rg_mbms->msgToSend_type = MCCH_mbmsCurrentCellPTMRBInformation;
        p_rg_mbms->msgToSend_ptr = & p_rg_mbms->messageCurrent;
        break;
      }
      //else, we ignore the current RB info, and send neigbouring cell RB Info. No break
    case MCCH_mbmsCurrentCellPTMRBInformation:
      p_rg_mbms->msgToSend_type = MCCH_mbmsNeighbouringCellPTMRBInformation;
      p_rg_mbms->msgToSend_neigbIndex = 0;
      //Don't break, we will transfer neighbouring cell info 
    case MCCH_mbmsNeighbouringCellPTMRBInformation:		
      if (p_rg_mbms->msgToSend_neigbIndex < p_rg_mbms->neighb_cellCount)
        p_rg_mbms->msgToSend_ptr = & p_rg_mbms->messageNeighbouring[p_rg_mbms->msgToSend_neigbIndex++];
      else{
        p_rg_mbms->msgToSend_type = -1;
        p_rg_mbms->msgToSend_ptr = NULL; //All messages have been sent
      }
      break;
    default:
      p_rg_mbms->msgToSend_type = -1;
  }
  return p_rg_mbms->msgToSend_type;
}


/**
 * @brief This function get the next mcch message, of which the order is specified 
 * by TS 25.331, and send it on the MCCH. Temporary use of tx_simulate to send messages. 
 * @warning In the future, must be coded for using MCCH chanel.
 * @date May 27, 2005 6:43:36 PM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MCCH_tx(void) {
//-----------------------------------------------------------------------------
  #ifdef ALLOW_MBMS_PROTOCOL
  int msgType;

  //Determine the message to transmit, Access info will be sent on its proper period
  if (p_rg_mbms->messageAccess.msg_length > 0){
    //Temporary, we send the message on SRB3
    RRC_RG_MBMS_O_SEND_MCCH(p_rg_mbms->messageAccess.msg_ptr, p_rg_mbms->messageAccess.msg_length); 	
    rrc_rg_mbms_message_reset(&p_rg_mbms->messageAccess);
  }else{
  //not a MBMSAccessInformation message.
    msgType = rrc_rg_mbms_MCCH_next_message();
    if (msgType >= 0 && p_rg_mbms->msgToSend_ptr != NULL && p_rg_mbms->msgToSend_ptr->msg_length > 0){
      //transmission on MCCH - Temporary, we use SRB3
      RRC_RG_MBMS_O_SEND_MCCH(p_rg_mbms->msgToSend_ptr->msg_ptr, p_rg_mbms->msgToSend_ptr->msg_length);
    }
  }
  #endif
}

/**
 * @brief Send the notification on DCCH if possible (having a notification message) 
 * Temporary use tx_simulate, will use UM DCCH (sRB 1) to send message 
 * @date May 31, 2005 9:28:14 AM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_DCCH_tx(int ueID){
//-----------------------------------------------------------------------------
  #ifdef ALLOW_MBMS_PROTOCOL
  rrc_rg_mbms_message_reset(&p_rg_mbms->messageNotification);
  rrc_rg_mbms_DCCH_encode(DL_DCCH_mbmsModifiedServicesInformation);
  rrc_rg_mbms_message_update(&p_rg_mbms->messageNotification);

  if (p_rg_mbms->messageNotification.msg_length > 0){
    //Use DCCH-UM.
    RRC_RG_MBMS_O_SEND_DCCH_UM(ueID, p_rg_mbms->messageNotification.msg_ptr, p_rg_mbms->messageNotification.msg_length);
    p_rg_mbms->messageNotification.sent = TRUE;
  }
  #endif
}

/**
 * @brief Send the scheduling info message if possible
 * @date May 31, 2005 9:28:47 AM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_MSCH_tx(void){
//-----------------------------------------------------------------------------
  #ifdef ALLOW_MBMS_PROTOCOL
  if (!p_rg_mbms->messageScheduling.sent && p_rg_mbms->messageScheduling.msg_length > 0){
    RRC_RG_MBMS_O_SEND_MSCH(p_rg_mbms->messageScheduling.msg_ptr, p_rg_mbms->messageScheduling.msg_length);
    p_rg_mbms->messageScheduling.sent = TRUE;		
  }	
  #endif
}

//-----------------------------------------------------------------------------
void rrc_rg_mbms_transfer_stable_services(void){
//-----------------------------------------------------------------------------
  int i;
  u8 umod_requiredAction;
  int count;

  //Debug Information
  #ifdef DEBUG_RRC_MBMS
    msg("[RRC-RG][MBMS] Transferring mbms services (in steady state) from ModifiedServiceInformation");
    msg(" to UnmodifiedServiceInformation...\n");
  #endif

  //Transfer code
  count = 0;	
  for (i = 0; i<p_rg_mbms->buff_mod_numService; i++){
    //if stable: --> transfer to umod
    //how is stable??? here, i means there is no more actions/changes on this service for a mdification period
    if ( p_rg_mbms->buff_mod_changed[i] == FALSE ){  //i<p_rg_mbms->mod_numService && p_rg_mbms->buff_mod_requiredUEAction[i] == p_rg_mbms->mod_requiredUEAction[i])
      //mapping the action: convert mod_requiredUEAction --> umod_requiredUEAction: We can use a map table for optimisation.
      switch (p_rg_mbms->buff_mod_requiredUEAction[i]){
        case Mod_acquirePTM_RBInfo: 
          umod_requiredAction = UMod_acquirePTM_RBInfo; 
          //append Unmodified service, mark the flag to recreate the Unmodified message if neccessary
          p_rg_mbms->flags.unmodifiedServicesInformation |= rrc_rg_mbms_addUnmodifService(-1, p_rg_mbms->buff_mod_serviceIdentity[i], umod_requiredAction);
          #ifdef DEBUG_RRC_MBMS_BASIC
           msg("[RRC-RG][MBMS] Service %d was transferred from Modified service to Unmodified service\n", p_rg_mbms->buff_mod_serviceIdentity[i]);
          #endif					
          break;				
        case Mod_releasePTM_RB: 
          #ifdef DEBUG_RRC_MBMS_BASIC	
           msg("[RRC-RG][MBMS] Service %d with action = 'release PTM RB' was removed from Modified service\n", p_rg_mbms->buff_mod_serviceIdentity[i]);
          #endif
          //Do nothing == don't copy to mod_xxx <--> remove action
        case Mod_none: 
        default:
          //Do not store the service with action != acquire...
          break;
      }	
    }else{ //not stable --> update to mod / will affect the Modified Service Information message.
      p_rg_mbms->mod_requiredUEAction[count] = p_rg_mbms->buff_mod_requiredUEAction[i];
      memcpy(&p_rg_mbms->mod_serviceIdentity[count ++ ], &p_rg_mbms->buff_mod_serviceIdentity[i], sizeof(o3));
    }
  }

  #ifdef DEBUG_RRC_MBMS
  if (count == p_rg_mbms->buff_mod_numService)	
    msg("[RRC-RG][MBMS] There's no modified service in steady state, nothing is transferred\n");
  #endif

  //Finalisation 	
  p_rg_mbms->mod_numService = count;
  if (count < p_rg_mbms->buff_mod_numService){ //<-- something was transferred
    //Indentique the buffer and the current content:		
    p_rg_mbms->buff_mod_numService = count;
    memcpy(&p_rg_mbms->buff_mod_requiredUEAction, &p_rg_mbms->mod_requiredUEAction, count * sizeof(u8));
    memcpy(&p_rg_mbms->buff_mod_serviceIdentity, &p_rg_mbms->mod_serviceIdentity, count * sizeof(o3));
    //Set the flag to recreate the Modified Service Info message.
    p_rg_mbms->flags.modifiedServicesInformation = TRUE; //Added opn 21/08/2005		
  }	

  //All services in the list may be stable/transferred on the next modification period.
  memset(p_rg_mbms->buff_mod_changed, 0, count * sizeof(mbms_bool));

  //Debug information
  #ifdef DEBUG_RRC_MBMS
   msg("[RRC-RG][MBMS] Transfer completed!\n");		
  #endif			
}

/**
 * @brief We must call this function before increasing the variable frame.
 * @date Aug 25, 2005 2:31:34 PM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_end_modification_period_check(void){
//-----------------------------------------------------------------------------
  #ifdef ALLOW_MBMS_PROTOCOL
    if ((protocol_bs->rrc.current_SFN+1) % (1 << p_rg_mbms->modifPeriodCoef) == 0)  //End of modification period.
      if (p_rg_mbms->l12ConfigurationChanged)
        RRC_RG_MBMS_O_L12_CONFIGURE();
  #endif
}

/**
 * @brief Called by rrc_rg_mbms_advance_time whenever it determines a new modification period.
 * Used to create a list of messages for sending during this new modification period.
 * @date May 31, 2005 4:52:48 PM
 * @see rrc_rg_mbms_advance_time()
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_activate_modification_period(void){
//-----------------------------------------------------------------------------
  int i;
  #ifdef DEBUG_RRC_MBMS
  msg("\033[31m\n**************************************************\033[0m\n");
  msg("[RRC-RG][MBMS] START A NEW MODIFICATION PERIOD\n");
  #endif

  p_rg_mbms->msgToSend_type = 0;  //Restart the counter on MCCH
  p_rg_mbms->msgToSend_ptr = NULL;
  p_rg_mbms->msgToSend_neigbIndex = 0;
  p_rg_mbms->messageNotification.sent = FALSE; //restart the flag to enable the notification
  //Allow to recreate the MBMS PTM RB Information messages if the configuration has been changed.
  p_rg_mbms->flags.commonPTMRBInformation = p_rg_mbms->l12ConfigurationChanged; 
  p_rg_mbms->flags.currentCellPTMRBInformation = p_rg_mbms->l12ConfigurationChanged;	
  p_rg_mbms->l12ConfigurationChanged = FALSE;

  //1. Move last modified services --> unmodified services
  rrc_rg_mbms_transfer_stable_services();
  #ifdef DEBUG_RRC_MBMS_DETAIL
   msg("[RRC-RG][MBMS] Current Modified Services:\n");
   rrc_rg_mbms_modif_services_print();
   msg("[RRC-RG][MBMS] Current Unmodified Services:\n");
   rrc_rg_mbms_unmodif_services_print();
  #endif

  //Test if there is any service which needs to acquire RB information:
  p_rg_mbms->allowCellRBInfo = (p_rg_mbms->mod_numService + p_rg_mbms->umod_numService) > 0;

  //2. Update messages if neccessary (but message Modified and Notified will be created on sending)
  if (p_rg_mbms->flags.modifiedServicesInformation){
    rrc_rg_mbms_MCCH_encode(MCCH_mbmsModifiedServicesInformation);	
    rrc_rg_mbms_message_update(&p_rg_mbms->messageModif);
  }

  if (p_rg_mbms->flags.unmodifiedServicesInformation){
    rrc_rg_mbms_MCCH_encode(MCCH_mbmsUnmodifiedServicesInformation);	
    rrc_rg_mbms_message_update(&p_rg_mbms->messageUnmodif);
  }

  if (p_rg_mbms->flags.generalInformation){
    rrc_rg_mbms_MCCH_encode(MCCH_mbmsGeneralInformation);
    rrc_rg_mbms_message_update(&p_rg_mbms->messageGeneral);
  }

  if (p_rg_mbms->flags.commonPTMRBInformation){
    rrc_rg_mbms_MCCH_encode(MCCH_mbmsCommonPTMRBInformation);
    rrc_rg_mbms_message_update(&p_rg_mbms->messageCommon);
  }

  if (p_rg_mbms->flags.currentCellPTMRBInformation){		
          rrc_rg_mbms_MCCH_encode(MCCH_mbmsCurrentCellPTMRBInformation);
          rrc_rg_mbms_message_update(&p_rg_mbms->messageCurrent);
  }

  //3. Recreate the Neighbouring messages if neccessary
  if (p_rg_mbms->flags.neighbouringCellPTMRBInformation)
    for (i = 0; i<p_rg_mbms->neighb_cellCount; i++){
      p_rg_mbms->neighb_index = i; //Create the message for the ith neighbour
      rrc_rg_mbms_MCCH_encode(MCCH_mbmsNeighbouringCellPTMRBInformation);		
      rrc_rg_mbms_message_update(&p_rg_mbms->messageNeighbouring[i]);
    }	

  //4. Prepare for the next modification
  //Reset all the message flags
  memset(&p_rg_mbms->flags, 0, sizeof(rrc_rg_mbms_msg_flags));
}

/**
 * @brief Called by rrc_rg_mbms_advance_time whenever it determines a new repetition period.
 * Used to start a new process of sending on MCCH
 * @date May 31, 2005 4:54:16 PM
 * @see rrc_rg_mbms_advance_time()
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_activate_repetition_period(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_MBMS
  msg("-------------------------------------------------\n");
  msg("[RRC-RG][MBMS] START A NEW REPETITION PERIOD\n");
  #endif	
  // p_rg_mbms->curr_repetition_counter = 1;
  p_rg_mbms->msgToSend_type = 0;  //Restart the counter on MCCH
  p_rg_mbms->messageModif.sent = FALSE;	
}

/**
 * @brief Called by rrc_rg_mbms_advance_time whenever it determines a new access period.
 * used to create the message MBMSAccessInfomation for sending on MCCH.
 * @date May 31, 2005 4:52:31 PM
 * @see rrc_rg_mbms_advance_time()
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_activate_access_period(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_MBMS_DETAIL
   msg("++++++++++++++++++++++++++++++++++++++++++++++++\n");
   msg("[RRC-RG][MBMS] START A NEW ACCESS PERIOD\n");
   msg("\n[MW] flags.accessInformation %d\n\n", p_rg_mbms->flags.accessInformation);
  #endif
  //Release old, and prepare a new access message
  if (p_rg_mbms->flags.accessInformation){
    rrc_rg_mbms_message_reset(&p_rg_mbms->messageAccess);
    rrc_rg_mbms_MCCH_encode(MCCH_mbmsAccessInformation);
    rrc_rg_mbms_message_update(&p_rg_mbms->messageAccess); //Only updated when allowed
  }
}

/**
 * @brief Called by rrc_rg_mbms_advance_time whenever it determines a new scheduling period.
 * @date May 31, 2005 4:52:38 PM
 * @see rrc_rg_mbms_advance_time()
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_activate_scheduling_period(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_MBMS_DETAIL
   msg(".................................................\n");
   msg("[RRC-RG][MBMS] START A NEW SCHEDULING PERIOD\n");
  #endif

  //Release old, and prepare a new scheduling message
  if (p_rg_mbms->flags.schedulingInformation){
    rrc_rg_mbms_message_reset(&p_rg_mbms->messageScheduling);
    rrc_rg_mbms_MSCH_encode(MSCH_mbmsSchedulingInformation);
    rrc_rg_mbms_message_update(&p_rg_mbms->messageScheduling);
  }
}

/**
 * @brief advance the timing counter to determine which time event will occur 
 * This will be the main interface for the RG side
 * @date May 31, 2005 11:24:19 AM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_scheduling_check(void){
//-----------------------------------------------------------------------------
  #ifdef ALLOW_MBMS_PROTOCOL
  if (protocol_bs->rrc.current_SFN % (1 << p_rg_mbms->modifPeriodCoef) == 0)	
    rrc_rg_mbms_activate_modification_period();

  if (protocol_bs->rrc.current_SFN % (1 << (p_rg_mbms->modifPeriodCoef - p_rg_mbms->accessInfoPeriodCoef)) == 0)
    rrc_rg_mbms_activate_access_period();

  if (protocol_bs->rrc.current_SFN % (1 << (p_rg_mbms->modifPeriodCoef - p_rg_mbms->repetitionPeriodCoef)) == 0)
    rrc_rg_mbms_activate_repetition_period();

  if (protocol_bs->rrc.current_SFN % (1 << p_rg_mbms->schedPeriodCoef) == 0 )
    rrc_rg_mbms_activate_scheduling_period();
  #endif
}

/**
 * @brief Delete the index th element in the list of Unmodified Services p_rg_mbms->umod_xxx
 * @date Jul 8, 2005 10:41:40 AM
 * @return TRUE if deleted, FALSE if not.
 */
 //-----------------------------------------------------------------------------
mbms_bool rrc_rg_mbms_deleteUnmodifService(int index){
//-----------------------------------------------------------------------------
  int rest;
  mbms_bool result = FALSE;
  //Operation delete is defined as moving 1 to the left all the element: index + 1, index + 2....
  if ((result = index >= 0 && index < p_rg_mbms->umod_numService)){
    if ((rest = p_rg_mbms->umod_numService - (index + 1))){
      memmove(&p_rg_mbms->umod_serviceIdentity[index], &p_rg_mbms->umod_serviceIdentity[index+1], rest*sizeof(o3));
      memmove(&p_rg_mbms->umod_requiredUEAction[index], &p_rg_mbms->umod_requiredUEAction[index+1], rest*sizeof(u8));
    }
    p_rg_mbms->umod_numService--;
  }
  return result;	
}

/**
 * @brief Add the service and its action to the list p_rg_mbms->umod_serviceIdentity[].
 * @date Jul 8, 2005 11:26:44 AM
 * @param serviceID service to be added
 * @param umod_requiredAction required action corresponds to the serviceID. 
 * @param index defaultIndex = -1 --> append and increase the number of services,  >= 0 --> edit the element.
 * @return TRUE if inserted or modified, FALSE if not.
 */
//----------------------------------------------------------------------------- 
mbms_bool rrc_rg_mbms_addUnmodifService(int index, o3 serviceID, u8 umod_requiredAction){
//-----------------------------------------------------------------------------
  mbms_bool result = FALSE;

  if ((result = (index < 0) && p_rg_mbms->umod_numService<maxMBMSservUnmodif)){
          memcpy(&p_rg_mbms->umod_serviceIdentity[p_rg_mbms->umod_numService], &serviceID, sizeof(o3));
          p_rg_mbms->umod_requiredUEAction[p_rg_mbms->umod_numService++] = umod_requiredAction;
  }else{
    //Otherwise, we update
    if ((result = (index >= 0) && index<maxMBMSservUnmodif)) //exist --> modified the value of required ue action.
          p_rg_mbms->umod_requiredUEAction[index] = umod_requiredAction;
  }
  return result;		
}

/**
 * @brief Delete the index th element in the list of Modified Services p_rg_mbms->buff_mod_xxx
 * if found --> delete this element from the list.
 * @date Jul 8, 2005 10:41:40 AM
 * @return TRUE if deleted, FALSE if not.
 */
 //-----------------------------------------------------------------------------
mbms_bool rrc_rg_mbms_deleteModifService(int index){
//-----------------------------------------------------------------------------
  int rest;
  mbms_bool result = FALSE;

  //Operation delete is defined as moving 1 to the left all the element: index + 1, index + 2....
  if ((result = (index >= 0 && index < p_rg_mbms->buff_mod_numService))){
    if ((rest = p_rg_mbms->buff_mod_numService - (index + 1))){  //number of elements
      memmove(&p_rg_mbms->buff_mod_changed[index], &p_rg_mbms->buff_mod_changed[index+1], rest*sizeof(mbms_bool));		
      memmove(&p_rg_mbms->buff_mod_serviceIdentity[index], &p_rg_mbms->buff_mod_serviceIdentity[index+1], rest*sizeof(o3));							
      memmove(&p_rg_mbms->buff_mod_requiredUEAction[index], &p_rg_mbms->buff_mod_requiredUEAction[index+1], rest*sizeof(u8));
    }
    p_rg_mbms->buff_mod_numService--;
  }
  return result;	
}

/**
 * @brief Add the service and its action to the list p_rg_mbms->buff_mod_serviceIdentity[].
  * @date Jul 8, 2005 11:26:44 AM
 * @param serviceID service to be added
 * @param mod_requiredAction required action corresponds to the serviceID. 
 * @param index defaultIndex = -1 --> append and increase the number of services,  >= 0 --> edit the element.
 * @return TRUE if inserted or modified, FALSE if not.

 */
//----------------------------------------------------------------------------- 
mbms_bool rrc_rg_mbms_addModifService(int index, o3 serviceID, u8 requiredAction){
//-----------------------------------------------------------------------------
  mbms_bool result = FALSE;
  if ( (result = (index < 0) && (p_rg_mbms->buff_mod_numService< maxMBMSservModif))){
    #ifdef DEBUG_RRC_MBMS_BASIC
     msg("[RRC-RG][MBMS] Add the service %d to the list of Modified Services. (index %d)\n", serviceID, index);
    #endif				
    //Service ID
    memcpy(&p_rg_mbms->buff_mod_serviceIdentity[p_rg_mbms->buff_mod_numService], &serviceID, sizeof(o3));
    //Required Action & Others
    p_rg_mbms->buff_mod_requiredUEAction[p_rg_mbms->buff_mod_numService] = requiredAction;
    p_rg_mbms->buff_mod_changed[p_rg_mbms->buff_mod_numService] = TRUE;
    p_rg_mbms->buff_mod_numService++;		
  }else
    if ((result = (index >= 0))){ //exist --> modified the value of required ue action.
    #ifdef DEBUG_RRC_MBMS_BASIC
     msg("[RRC-RG][MBMS] Mark the service %d to the list of Modified Services-- removed. (index %d)\n", serviceID, index);
    #endif				
      p_rg_mbms->buff_mod_requiredUEAction[index] = requiredAction;	
      p_rg_mbms->buff_mod_changed[index] = TRUE;
    }	
  return result;
}

/**
 * @brief Process the received joined services from NAS. copy them to mod_xxx to create the DCCH Notification
 * @date Aug 24, 2005 4:30:52 PM
 */
 //-----------------------------------------------------------------------------
void rrc_rg_mbms_processJoinedServices(void){
 //-----------------------------------------------------------------------------
  int i, index, ueID;
  u8 act_numService;

  ueID = p_rg_mbms->nas_ueID;
  for (i = 0; i<maxMBMSServices && p_rg_mbms->nas_joinedServices[i] != -1; i++){
    act_numService = protocol_bs->rrc.Mobile_List[ueID].act_numService;
    //Check if this service exist in the list of services/UE of the user p_rg_mbms->nas_ueID
    index = rrc_mbms_service_id_find(act_numService, &(protocol_bs->rrc.Mobile_List[ueID].act_serviceIdentity), p_rg_mbms->nas_joinedServices[i]);
    //We add this service in the notification only if it hasn't been activated before
    if (index < 0){
      p_rg_mbms->mod_requiredUEAction[p_rg_mbms->mod_numService] = Mod_acquirePTM_RBInfo;
      p_rg_mbms->mod_sessionIdentity[p_rg_mbms->mod_numService] = 0;   //temp
      memcpy(&p_rg_mbms->mod_serviceIdentity[p_rg_mbms->mod_numService++], &p_rg_mbms->nas_joinedServices[i], sizeof(o3));

      //add in the managed services/ue list
      memcpy(&(protocol_bs->rrc.Mobile_List[ueID].act_serviceIdentity[act_numService]), &(p_rg_mbms->nas_joinedServices[i]), sizeof(o3));
      protocol_bs->rrc.Mobile_List[ueID].act_numService++;
    }else{
      #ifdef DEBUG_RRC_MBMS
       msg("[RRC-RG][MBMS] Joined service %d has previously been notified to UE %d. Ignore this request\n", p_rg_mbms->nas_joinedServices[i], p_rg_mbms->nas_ueID);		
      #endif
    }
  }	
}

/**
 * @brief Process the received left services from NAS. copy them to mod_xxx to create the DCCH Notification
 * @date Aug 24, 2005 4:29:57 PM
 */
 //-----------------------------------------------------------------------------
void rrc_rg_mbms_processLeftServices(void){
 //-----------------------------------------------------------------------------
  int i, index, ueID;
  u8 act_numService;
  int remaining;

  ueID = p_rg_mbms->nas_ueID;
  for (i = 0; i<maxMBMSServices && p_rg_mbms->nas_leftServices[i] != -1; i++){
    act_numService = protocol_bs->rrc.Mobile_List[ueID].act_numService;
    index = rrc_mbms_service_id_find(act_numService, &(protocol_bs->rrc.Mobile_List[ueID].act_serviceIdentity), p_rg_mbms->nas_leftServices[i]);
    //We add this service in the notification only if it has previously been activated
    if (index >= 0){
      p_rg_mbms->mod_requiredUEAction[p_rg_mbms->mod_numService] = Mod_releasePTM_RB;
      memcpy(& p_rg_mbms->mod_serviceIdentity[p_rg_mbms->mod_numService++], &p_rg_mbms->nas_leftServices[i], 3);

      //delete from the services/ue list
      remaining = act_numService - (index + 1);
      if (remaining > 0)
        memmove(&(protocol_bs->rrc.Mobile_List[ueID].act_serviceIdentity[index]), &(protocol_bs->rrc.Mobile_List[ueID].act_serviceIdentity[index+1]), remaining * sizeof(o3));
      protocol_bs->rrc.Mobile_List[ueID].act_numService --;
    }else{
      #ifdef DEBUG_RRC_MBMS
       msg("[RRC-RG][MBMS] Service %d was not joined. Ignore this request\n", p_rg_mbms->nas_leftServices[i]);		
      #endif				
    }
  }
}

/**
 * @brief If the length of p_rg_mbms->curr_Message > 0, 
 * copy the current content of p_rg_mbms->curr_Message in to the struct pointed by ptrMsgInfo.
 * @param ptrMsgInfo pointer pointing to a static buffer in p_rg_mbms
 * 
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_message_update(rrc_rg_mbms_msg_buf * ptrMsgInfo){
//-----------------------------------------------------------------------------
  if (p_rg_mbms->curr_Message.msg_length >0)
    memcpy(ptrMsgInfo, &p_rg_mbms->curr_Message, sizeof(rrc_rg_mbms_msg_buf));
}

/**
 * @brief Reset the content of message info.
 * @param ptrMsgInfo pointer pointing to the message information
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_message_reset(rrc_rg_mbms_msg_buf * ptrMsgInfo){
//-----------------------------------------------------------------------------
  memset(ptrMsgInfo, 0, sizeof(rrc_rg_mbms_msg_buf));
}

/**
 * @brief Used to print out the list of modified services
 * @date Jul 29, 2005 10:24:11 AM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_modif_services_print(void){
//-----------------------------------------------------------------------------
  int i;
  for (i = 0; i< p_rg_mbms->mod_numService; i++){
    msg("\t\t[RRC-RG][MBMS] service id = %d", p_rg_mbms->mod_serviceIdentity[i]);
    msg(", required action = ");			
    switch (p_rg_mbms->mod_requiredUEAction[i]){
      case Mod_none: msg("None\n"); break;
      case Mod_acquirePTM_RBInfo: msg("Acquire PTM RB Info\n"); break;
      case Mod_releasePTM_RB: msg("Release PTM RB Info\n"); break;
    }
  }
}

/**
 * @brief Used to print out the list of unmodified services
 * @date Jul 29, 2005 10:24:11 AM
 */
//-----------------------------------------------------------------------------
void rrc_rg_mbms_unmodif_services_print(void){
//-----------------------------------------------------------------------------
  int i;
  for (i = 0; i< p_rg_mbms->umod_numService; i++){
    msg("\t\t[RRC-RG][MBMS] service id = %d", p_rg_mbms->umod_serviceIdentity[i]);
    msg(", required action = ");			
    switch (p_rg_mbms->umod_requiredUEAction[i]){
      case UMod_none: msg("None\n"); break;
      case UMod_acquirePTM_RBInfo: msg("Acquire PTM RB Info\n"); break;
      case UMod_releasePTM_RB: msg("Release PTM RB Info\n"); break;
    }		
  }	
}
