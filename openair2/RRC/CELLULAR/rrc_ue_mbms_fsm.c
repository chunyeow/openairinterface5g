/***************************************************************************
                          rrc_ue_mbms_fsm.c - description
                          -------------------
    begin               : Jun 1, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by          : Huu-Nghia.Nguyen@eurecom.fr	
    modified by        	: Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  This file implements the state machine for UE side.
  The code is created manually in the same manner as Esterel.
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

//-----------------------------------------------------------------------------
int stateAcquisition = ACQUI_START;
//-----------------------------------------------------------------------------

//INPUT
//condition for acquisition
mbms_bool I_CONTROLING_CELL_CHANGED;
mbms_bool I_RETURN_FROM_LOSS_COVERAGE;
mbms_bool I_ACTIVATED_SERVICE_CHANGED;
mbms_bool I_SELECTING_CELL_MBMS;

//determine if message arrive ?
mbms_bool I_MODIF_SERV_INFO;
mbms_bool I_MCCH_MODIF_SERV_INFO; //more detailed
mbms_bool I_ALL_UNMODIF_PTM_SERVICES;
mbms_bool I_UNMODIF_SERV_INFO;

mbms_bool I_COMMON_CELL_RB_INFO;
mbms_bool I_CURRENT_CELL_RB_INFO;

mbms_bool I_NEIGHBOURING_CELL_RB_INFO;
mbms_bool I_MODIF_PERIOD_ENDED;

//local signal
mbms_bool modifServInfoReceived = FALSE;
//int ptmServCount = 0; //value is in protocol_ms->rrc.mbms.activatedServicesList.numService

#ifdef DEBUG_UE_MBMS_FSM_TEST
  mbms_bool O_NAS_MBMS_UE_NOTIFY_IND;
  mbms_bool O_ANALYSE_UNMODIF;
  mbms_bool O_CURRENT_CELL_RB_CONFIGURATION;
  mbms_bool O_NEIGHBOURING_CELL_RB_CONFIGURATION;
  mbms_bool O_MCCH_NOTIFICATION;
  mbms_bool O_DCCH_NOTIFICATION;
#endif

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_CONTROLING_CELL_CHANGED(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_CONTROLING_CELL_CHANGED\n");
  #endif
  I_CONTROLING_CELL_CHANGED = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_RETURN_FROM_LOSS_COVERAGE(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM	
    msg("[RRC-UE][MBMS][FSM] input: I_RETURN_FROM_LOSS_COVERAGE\n");
  #endif
  I_RETURN_FROM_LOSS_COVERAGE = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_ACTIVATED_SERVICE_CHANGED(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_ACTIVATED_SERVICE_CHANGED\n");	
  #endif		
  I_ACTIVATED_SERVICE_CHANGED = TRUE;		
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_SELECTING_CELL_MBMS(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_SELECTING_CELL_MBMS\n");
  #endif		
  I_SELECTING_CELL_MBMS = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_MODIF_SERV_INFO(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_MODIF_SERV_INFO\n");
  #endif		
  I_MODIF_SERV_INFO = TRUE;
  modifServInfoReceived = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_MCCH_MODIF_SERV_INFO(void){
//-----------------------------------------------------------------------------
  RRC_UE_MBMS_I_MODIF_SERV_INFO();
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_MCCH_MODIF_SERV_INFO\n");
  #endif		
  I_MCCH_MODIF_SERV_INFO = TRUE;
}


//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_UNMODIF_SERV_INFO(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_UNMODIF_SERV_INFO\n");
  #endif		
  I_UNMODIF_SERV_INFO = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_COMMON_CELL_RB_INFO(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_COMMON_CELL_RB_INFO\n");
  #endif		
  I_COMMON_CELL_RB_INFO = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_CURRENT_CELL_RB_INFO(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_CURRENT_CELL_RB_INFO\n");
  #endif	
  I_CURRENT_CELL_RB_INFO = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_NEIGHBOURING_CELL_RB_INFO(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_NEIGHBOURING_CELL_RB_INFO\n");
  #endif		
  I_NEIGHBOURING_CELL_RB_INFO = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_MODIF_PERIOD_ENDED(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_MODIF_PERIOD_ENDED\n");
  #endif		
  I_MODIF_PERIOD_ENDED = TRUE;
}

//-----------------------------------------------------------------------------
void RRC_UE_MBMS_I_ALL_UNMODIF_PTM_SERVICES(void){
//-----------------------------------------------------------------------------
  #ifdef DEBUG_UE_MBMS_FSM
    msg("[RRC-UE][MBMS][FSM] input: I_ALL_UNMODIF_PTM_SERVICES\n");
  #endif		
  I_ALL_UNMODIF_PTM_SERVICES = TRUE;	
}


/**
 * @brief FSM for the RRC UE MBMS
 * @date Jun 1, 2005 2:48:00 PM
 */
 //-----------------------------------------------------------------------------
void rrc_ue_mbms_fsm(void){
//-----------------------------------------------------------------------------
  // ptmServCount =  p_ue_mbms->act_numService;
  switch (stateAcquisition){
    case ACQUI_START:
      stateAcquisition = ACQUI_I_ACQUISITION;
      #ifdef DEBUG_UE_MBMS_FSM
      msg("[RRC-UE][MBMS][FSM] transition: START to I_ACQUISITION\n");
      #endif
      rrc_ue_mbms_fsm(); //Propagation
      break;
    case ACQUI_I_ACQUISITION:			
      if (I_CONTROLING_CELL_CHANGED || I_ACTIVATED_SERVICE_CHANGED ||
      I_RETURN_FROM_LOSS_COVERAGE || I_SELECTING_CELL_MBMS){
        #ifdef DEBUG_UE_MBMS_FSM
        msg("[RRC-UE][MBMS][FSM] transition: I_ACQUISITION to WAIT_MBMS_MSG\n");
        #endif
        stateAcquisition = ACQUI_WAIT_MBMS_MSG;			
        I_MODIF_PERIOD_ENDED = FALSE;
        rrc_ue_mbms_fsm(); //Propagation
      }
      break;
    case ACQUI_WAIT_MBMS_MSG:
      //Notification
      if (I_MODIF_SERV_INFO){
        if (I_MCCH_MODIF_SERV_INFO)
          RRC_UE_MBMS_O_MCCH_NOTIFICATION();
        else
          RRC_UE_MBMS_O_DCCH_NOTIFICATION();
        I_MCCH_MODIF_SERV_INFO = FALSE;
        I_MODIF_SERV_INFO = FALSE;
      }
      if (I_ALL_UNMODIF_PTM_SERVICES && I_UNMODIF_SERV_INFO){
        I_UNMODIF_SERV_INFO = FALSE;
        RRC_UE_MBMS_O_ANALYSE_UNMODIF();
      }
      //RB configuration
      // if (ptmServCount > 0 && I_COMMON_CELL_RB_INFO && I_CURRENT_CELL_RB_INFO)
      if (I_COMMON_CELL_RB_INFO && I_CURRENT_CELL_RB_INFO){
        RRC_UE_MBMS_O_CURRENT_CELL_RB_CONFIGURATION();
        I_COMMON_CELL_RB_INFO = FALSE;
        I_CURRENT_CELL_RB_INFO = FALSE;
      }			
      // if (ptmServCount > 0 && I_NEIGHBOURING_CELL_RB_INFO) 			
      if (I_NEIGHBOURING_CELL_RB_INFO){
        RRC_UE_MBMS_O_NEIGHBOURING_CELL_RB_CONFIGURATION();
        I_NEIGHBOURING_CELL_RB_INFO = FALSE;
      }			
      //End of modification period?
      if (I_MODIF_PERIOD_ENDED){
        //This is an adaptation for OAI, we don't return to the initial state as specififed in TS 25.331
        //Because, at this moment, the primitive NAS for UE is not specified yet
        #if 0
          if (modifServInfoReceived){
            #ifdef DEBUG_UE_MBMS_FSM
             msg("[RRC-UE][MBMS][FSM] transition: WAIT_MBMS_MSG to I_ACQUISITION\n");
            #endif
            stateAcquisition = ACQUI_I_ACQUISITION;
            modifServInfoReceived = FALSE;
            I_ACTIVATED_SERVICE_CHANGED = FALSE;
            I_CONTROLING_CELL_CHANGED = FALSE;
            I_RETURN_FROM_LOSS_COVERAGE = FALSE;
            I_SELECTING_CELL_MBMS = FALSE;					
          }
        #endif
        // else stay in this state!				
        I_MODIF_PERIOD_ENDED = FALSE;
        I_MODIF_SERV_INFO = FALSE;
        I_UNMODIF_SERV_INFO = FALSE;
        I_COMMON_CELL_RB_INFO = FALSE;
        I_CURRENT_CELL_RB_INFO = FALSE;
        I_ALL_UNMODIF_PTM_SERVICES = FALSE;
      }
      break;		
  }
}

/**
 * @brief Reset the FSM.
 * @date Jun 1, 2005 2:48:16 PM
 * @warning must be called some where in the rrc_ue_mbms_init function
 */
 //-----------------------------------------------------------------------------
void rrc_ue_mbms_fsm_reset(void){
//-----------------------------------------------------------------------------
  stateAcquisition = ACQUI_START;

  //condition for acquisition
  I_ACTIVATED_SERVICE_CHANGED = TRUE;
  I_CONTROLING_CELL_CHANGED = FALSE;
  I_RETURN_FROM_LOSS_COVERAGE = FALSE;
  I_SELECTING_CELL_MBMS = FALSE;

  //variables determining if messages arrive
  I_MODIF_SERV_INFO = FALSE;
  I_UNMODIF_SERV_INFO = FALSE;
  I_COMMON_CELL_RB_INFO = FALSE;
  I_CURRENT_CELL_RB_INFO = FALSE;
  I_NEIGHBOURING_CELL_RB_INFO = FALSE;
  I_MODIF_PERIOD_ENDED = FALSE;

  #ifdef DEBUG_UE_MBMS_FSM_TEST
  O_NAS_MBMS_UE_NOTIFY_IND = FALSE;
  O_ANALYSE_UNMODIF = FALSE;
  O_CURRENT_CELL_RB_CONFIGURATION = FALSE;
  O_NEIGHBOURING_CELL_RB_CONFIGURATION = FALSE;
  O_MCCH_NOTIFICATION = FALSE;
  O_DCCH_NOTIFICATION = FALSE;
  #endif
  modifServInfoReceived = FALSE;
}
