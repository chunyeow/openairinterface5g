/***************************************************************************
                         rrc_ue_mbms_process.c - description
                          -------------------
    begin               : May 30, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by          : Huu-Nghia.Nguyen@eurecom.fr  
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
    This file implements Interface procedures for UE 
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

rrc_ue_mbms_variables * volatile p_ue_mbms; /** pointer reference to protocol_ms->rrc.mbms */

/**
 * @brief Init the UE side mbms variables
 * @date May 27, 2005 6:45:24 PM
 */  
//-----------------------------------------------------------------------------
void rrc_ue_mbms_init(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_STATE
  msg("[RRC-UE][MBMS] START OF MBMS INITIALISATION.......................\n");
  #endif

  p_ue_mbms = &protocol_ms->rrc.mbms;
  memset(p_ue_mbms, 0, sizeof(rrc_ue_mbms_variables));

  #ifdef DEBUG_RRC_STATE
  msg("[RRC-UE][MBMS] Allow the procedure MBMS PTM RB Configuration\n");  
  #endif
  p_ue_mbms->initialised = FALSE;
  p_ue_mbms->mustAcquireRBInfo = TRUE;

  p_ue_mbms->modifPeriodCoef = MODIF_PERIOD_COEF_DEFAULT;
  rrc_ue_mbms_fsm_reset();
  rrc_ue_mbms_fsm(); //Enter the state machine.

  #ifdef DEBUG_RRC_STATE
  msg("[RRC-UE][MBMS] END OF MBMS INITIALISATION\n\n");
  #endif
}

/**
 * @brief Reset all flags and signals for a new modification period
 * @date May 31, 2005 4:52:48 PM
 * @see rrc_ue_mbms_advance_time()
 */
//-----------------------------------------------------------------------------
void rrc_ue_mbms_activate_modification_period(void){
//-----------------------------------------------------------------------------
  int i;
  #ifdef DEBUG_RRC_MBMS
    msg("\033[31m\n**************************************************\033[0m\n");
    msg("[RRC-UE][MBMS] START A NEW MODIFICATION PERIOD at frame %d \n", protocol_ms->rrc.current_SFN);
  #endif

  //If this is the first time, we didn't have the RB info, we must acquire it. Otherwise we reset it to FALSE
  p_ue_mbms->mustAcquireRBInfo = !p_ue_mbms->initialised;  

  memset(&p_ue_mbms->flags, 0, sizeof(p_ue_mbms->flags)); //Clear flags  
  RRC_UE_MBMS_I_MODIF_PERIOD_ENDED();
  rrc_ue_mbms_fsm();

  //Print a list of MBMS Activated Services:
  #ifdef DEBUG_RRC_MBMS
  msg("[RRC-UE][MBMS] MBMS Activated Services: { ");
  for (i = 0; i<p_ue_mbms->act_numService; i++){
    msg(" service%d/", p_ue_mbms->act_serviceIdentity[i]);
    if (p_ue_mbms->act_activated[i]) 
      msg("active");
    else 
      msg("standby");
  }
  msg(" }\n");
  #endif
}

/**
 * @brief check for the modification period 
 * @warning the counter p_ue_mbms->curr_modif_counter will be synchronize with RG when
 * decoding the MBMS Modified Services Information message.
 * @date May 31, 2005 11:24:19 AM
 */
//-----------------------------------------------------------------------------
void rrc_ue_mbms_scheduling_check(void){
//-----------------------------------------------------------------------------
  if (protocol_ms->rrc.current_SFN % (1 << p_ue_mbms->modifPeriodCoef) == 0)
    rrc_ue_mbms_activate_modification_period();
}

/**
 * @brief Local function, used to print out the list of modified services
 * @date Jul 29, 2005 10:24:11 AM
 */
//-----------------------------------------------------------------------------
void rrc_ue_mbms_modif_services_print(void){
//-----------------------------------------------------------------------------
  int i;
  msg("[RRC-UE][MBMS] MBMS Modified Services: \n");
  for (i = 0; i< p_ue_mbms->mod_numService; i++){
    msg("[RRC-UE][MBMS]\t\t service id = %d, required action = ", p_ue_mbms->mod_serviceIdentity[i]);
    switch (p_ue_mbms->mod_requiredUEAction[i]){
      case Mod_none: msg("None\n"); break;
      case Mod_acquirePTM_RBInfo: msg("Acquire PTM RB Info\n"); break;
      case Mod_releasePTM_RB: msg("Release PTM RB Info\n"); break;
    }
  }
}

/**
 * @brief Local function, used to print out the status of activated services
 * @date Sept 06, 2006
 */
//-----------------------------------------------------------------------------
void rrc_ue_mbms_status_services_print(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_RRC_MBMS_STATUS
  int i;
  //Print the list of MBMS broadcasted services
  msg("[RRC-UE][MBMS] MBMS Started Services: \n");
  for (i = 0; i< maxMBMSServices; i++){
    msg("[RRC-UE][MBMS]\t\t service id = %d, activated = ", i+1);
    switch (p_ue_mbms->act_activated[i]){
      case TRUE  : msg("YES\n"); break;
      case FALSE : msg("NO\n"); break;
      default: msg("UNDEFINED\n"); break;
    }    
}  

  //Print a list of MBMS Activated Services:
  msg("[RRC-UE][MBMS] MBMS Activated Services: { ");
  for (i = 0; i<p_ue_mbms->act_numService; i++){
    msg(" service%d/", p_ue_mbms->act_serviceIdentity[i]);
    if (p_ue_mbms->act_activated[i]) msg("active");
    else msg("standby");
  }
  msg(" }\n");
  #endif
}

//-----------------------------------------------------------------------------
const char * rrc_mbms_message_getName(int channelID, char* pmsg, int msgtype){
//-----------------------------------------------------------------------------
  const char * mcch_message_name[8] = {"", "AccessInfomation", "CommonPTMRBInformation", "CurrentCellPTMRBInformation", "GeneralInformation", "ModifiedServicesInformation", "NeighbouringCellPTMRBInformation", "UnmodifiedServicesInformation" };
  switch (channelID){
    case DCCH_ID:
      return "ModifiedServicesInformation";
    case MCCH_ID:
      if (pmsg != NULL) return mcch_message_name[((MCCH_Message*) pmsg)->message.type];
      else return mcch_message_name[msgtype];
    case MSCH_ID:
      return "SchedulingInformation";
    default:
      return "Unrecognized";
  }
}

