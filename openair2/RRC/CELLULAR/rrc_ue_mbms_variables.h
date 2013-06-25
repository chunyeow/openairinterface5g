/***************************************************************************
                          rrc_ue_mbms_variables.h - description
                          -------------------
    begin               : May 30, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by		      : Huu-Nghia.Nguyen@eurecom.fr	
    modified by        	: 
    description		      :
 **************************************************************************/
#ifndef _RRC_UE_MBMS_VARIABLES_H_
#define _RRC_UE_MBMS_VARIABLES_H_

#include "rrc_mbms_constant.h"

/**
 * @brief Use this struct to determine if the msg has received in the period: 
 * value = Modication period, Access info period, scheduling period
 * if already, this message will not be decoded.
 * @date May 20, 2005 11:23:20 AM
 */
typedef struct rrc_ue_mbms_period_flags {
  int commonPTMRBInformation;
  int currentCellPTMRBInformation;
  int generalInformation;
  int modifiedServicesInformation;
  int unmodifiedServicesInformation;
} rrc_ue_mbms_period_flags;

typedef struct rrc_ue_mbms_variables {  	
  //Used for scheduling on MCCH
  rrc_ue_mbms_period_flags flags; //tags to determine which msg has been received in the period

  //Used to get MBMS message 
  #ifdef MBMS_TEST_MODE
          int	curr_msgLen;
          u8	curr_msgBuf[MBMS_MAX_BUFFER_SIZE];
  #endif	

  //Used to decode the MBMS message
  u8	acc_numAccessInfo; //0..maxMBMSservCount (=4), @see MBMS_ServiceAccessInfoList */
  u8	acc_shortTransmissionID[maxMBMSservCount]; /** Integer(1..32),  @see MBMS_ShortTransmissionID */
  u16	acc_accessprobabilityFactor_Idle[maxMBMSservCount]; /** Integer(0, 32, 64..960, 1000), @see MBMS_AccessProbabilityFactor */

  u16	gen_t_318; /** Default value ms1000, @see T_318 */
  u16	gen_cellGroupIdentity; /** Bit string (12), @see MBMS_CellGroupIdentity */

  u8 	umod_numService;	//0..maxMBMSservUnmodif
  o3	umod_serviceIdentity[maxMBMSservUnmodif];		/** OctetString(3), @see MBMS_ServiceIdentity */
  u8	umod_requiredUEAction[maxMBMSservUnmodif]; /** @see MBMS_RequiredUEAction_UMod */

 
  u8 	mod_numService;	/** 0..maxMBMSservModif  */
  o3	mod_serviceIdentity		[maxMBMSservModif];		/** OctetString(3), @see MBMS_ServiceIdentity */
  u8	mod_sessionIdentity		[maxMBMSservModif];
  u8	mod_requiredUEAction	[maxMBMSservModif]; /** @see MBMS_RequiredUEAction_Mod */
  mbms_bool contMCCHReading [maxMBMSservModif];
  mbms_bool mod_reacquireMCCH;
//	u8  mod_endOfModifiedMCCHInformation;		
  u8	mod_numberOfNeighbourCells;  //MCCH only
  u8  mod_ptm_activationTime;      //MCCH only
  mbms_bool	mod_all_unmodified_ptm_services;


  //Because we use mod_xxx for DCCH & MCCH Notification message
  //we need to store the value of MCCH Notification message
  u8 	saved_mod_numService;	/** 0..maxMBMSservModif  */
  o3	saved_mod_serviceIdentity[maxMBMSservModif];		/** OctetString(3), @see MBMS_ServiceIdentity */
  u8	saved_mod_sessionIdentity		[maxMBMSservModif];
  u8	saved_mod_requiredUEAction[maxMBMSservModif]; /** @see MBMS_RequiredUEAction_Mod */
  mbms_bool	saved_mod_reacquireMCCH;
  mbms_bool	saved_mod_all_unmodified_ptm_services;
//	u8	saved_mod_endOfModifiedMCCHInformation;		
	
//u8	comm_l12Configuration	[MBMS_L12_CONFIGURATION_SIZE]; /** OctetString, @see MBMS_L12Configuration */
  int comm_l12Config_lgth;
  u8  comm_l12Config_data[MTCH_CONFIGURATION_SIZE];
//u8	curr_l12Configuration	[MBMS_L12_CONFIGURATION_SIZE]; /** OctetString, @see MBMS_L12Configuration */
  int curr_l12Config_lgth;
  u8  curr_l12Config_data[MTCH_CONFIGURATION_SIZE];

  u8	neighb_cellCount; /** number of neighbouring cell in the list, 0..maxNeighbouringCellIdentity (=32) is supposed, @see maxNeighbouringCellIdentity */	
  u16	neighb_cellConf_lgth		[MBMS_MAX_NEIGBOURING_COUNT]; /** neigbouring cells' configuration length, @see MBMS_NeighbouringCellConfiguration*/
  u8	neighb_cellConf_data		[MBMS_MAX_NEIGBOURING_COUNT][NEIGHBOUR_CONFIGURATION_SIZE]; /** neigbouring cells' configuration, @see MBMS_NeighbouringCellConfiguration*/
  u8	neighb_cellIDList		[MBMS_MAX_NEIGBOURING_COUNT]; /** list of neighb identity (1..X), used as a pointer to SIB 11? */
  u8	neighb_index; /** current neighb index of the list - Use it to create the msg corresponding*/	

  u8	sched_numInfo; 	/** 0..maxMBMSservSched  */
  o3	sched_serviceIdentity		[maxMBMSservSched];		/** OctetString(3), @see MBMS_ServiceIdentity */	
  u8	sched_trans_numTransmis	[maxMBMSservSched];	/** 0..maxMBMSTransmis */	
  u16	sched_trans_start			[maxMBMSservSched][maxMBMSTransmis];	/** 0..1024 by step of 4 */
  u16	sched_trans_duration		[maxMBMSservSched][maxMBMSTransmis]; /** 4..1024 */
  u8	sched_nextSchedPeriod		[maxMBMSservSched];	/** 0..31 */

  //Usage is not the same as in specification of 3GPP. 
  //Because there isn't any NAS primitive of UE, this variable will contain all services	acquiring PTM RB Information.
  u8			act_numService;
  o3			act_serviceIdentity	[maxMBMSServices];		/** OctetString(3), @see MBMS_ServiceIdentity */	
  mbms_bool	act_activated		[maxMBMSServices];
  u8			act_serviceType	[maxMBMSServices];

  //Those variables are prepared for future compatibility
  u8	nas_joinedCount;
  u8	nas_leftCount;
  int	nas_joinedServices		[maxMBMSServices]; /** OctetString(3), @see MBMS_ServiceIdentity */	
  int	nas_leftServices		[maxMBMSServices]; /** OctetString(3), @see MBMS_ServiceIdentity */			
  //Other variables is stored here.	
  u8				modifPeriodCoef;
  mbms_bool		mustAcquireRBInfo; //To determine if we have to acquire Common, Current Messages.
  mbms_bool		initialised; //To know if we have passed the fist modification period.
//  mbms_bool		hasFirstRBInfo; //To determinde if we have the Common/Current configuration. If hasn't, --> must acquire RB Info
} rrc_ue_mbms_variables;

#endif
