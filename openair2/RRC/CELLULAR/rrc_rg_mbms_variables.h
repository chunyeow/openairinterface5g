/***************************************************************************
                          rrc_rg_mbms_variables.h - description
                          -------------------
    begin               : May 30, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by	        : Huu-Nghia.Nguyen@eurecom.fr	
    modified by        	: 
    description	        : Definitions of variables in control block for RRC MBMS
 **************************************************************************/
#ifndef _RRC_RG_MBMS_VARIABLES_H_
#define _RRC_RG_MBMS_VARIABLES_H_

#include "rrc_mbms_constant.h"
#include "rrc_constant.h"


/** this struct store flags to identify which messages
 * need to be reconstructed at the begining of modification period */
typedef struct rrc_rg_mbms_msg_flags {
  int accessInformation;
  int commonPTMRBInformation;
  int currentCellPTMRBInformation;
  int generalInformation;
  int modifiedServicesInformation;
  int neighbouringCellPTMRBInformation;
  int unmodifiedServicesInformation;
  int schedulingInformation;
} rrc_rg_mbms_msg_flags;

/** this struct is used to avoid creating repeatly the mbms in a modification period.
 * Once the message is created, we will store it in this buffer, and use the flag 'sent'
 * to determine if this message has been sent on its channel*/
typedef struct rrc_rg_mbms_msg_buf {
  int 	msg_length;
  char	msg_ptr[MBMS_MAX_BUFFER_SIZE];
  mbms_bool sent;
} rrc_rg_mbms_msg_buf;


/**
 *  **control block for RRC MBMS **
 * For more information, read the design document
 **/
typedef struct rrc_rg_mbms_variables {
  //Used to encode the MBMS message
  u8	acc_numAccessInfo; //0..maxMBMSservCount (=4), @see MBMS_ServiceAccessInfoList */
  u8	acc_shortTransmissionID			[maxMBMSservCount]; /** Integer(1..32),  @see MBMS_ShortTransmissionID */
  u16	acc_accessprobabilityFactor_Idle	[maxMBMSservCount]; /** Integer(0, 32, 64..960, 1000), @see MBMS_AccessProbabilityFactor */

  u16	gen_t_318; /** Default value ms1000, @see T_318 */
  u16	gen_cellGroupIdentity; /** Bit string (12), @see MBMS_CellGroupIdentity */

  u8 	umod_numService;	//0..maxMBMSservUnmodif
  o3	umod_serviceIdentity	[maxMBMSservUnmodif];		/** OctetString(3), @see MBMS_ServiceIdentity @see also rrc_mbms_constant*/
  u8	umod_requiredUEAction	[maxMBMSservUnmodif]; /** @see MBMS_RequiredUEAction_UMod */

  u8 	mod_numService;	/** 0..maxMBMSservModif  */
  o3	mod_serviceIdentity		[maxMBMSservModif];		/** OctetString(3), @see MBMS_ServiceIdentity */
  u8	mod_sessionIdentity		[maxMBMSservModif];
  u8	mod_requiredUEAction	[maxMBMSservModif]; /** @see MBMS_RequiredUEAction_Mod */
  mbms_bool contMCCHReading [maxMBMSservModif];
  mbms_bool mod_reacquireMCCH;
//	u8	mod_endOfModifiedMCCHInformation;		
  u8	mod_numberOfNeighbourCells;
  u8  mod_ptm_activationTime;
  mbms_bool	mod_all_unmodified_ptm_services;

  //BUFFER to interact with NAS. Only needed for notification message on MCCH
  u8 	buff_mod_numService;	/** 0..maxMBMSservModif  */
  o3	buff_mod_serviceIdentity		[maxMBMSservModif];		/** OctetString(3), @see MBMS_ServiceIdentity */
  u8	buff_mod_requiredUEAction	[maxMBMSservModif]; /** @see MBMS_RequiredUEAction_Mod */
  mbms_bool	buff_mod_changed	[maxMBMSservModif]; /** Used  to determine if we need to transfer this service to unmodified message */

//u8 comm_l12Configuration	[MBMS_L12_CONFIGURATION_SIZE]; /** OctetString, @see MBMS_L12Configuration */
  int comm_l12Config_lgth;
  u8  comm_l12Config_data[MTCH_CONFIGURATION_SIZE];
//u8 curr_l12Configuration	[MBMS_L12_CONFIGURATION_SIZE]; /** OctetString, @see MBMS_L12Configuration */
  int curr_l12Config_lgth;
  u8  curr_l12Config_data[MTCH_CONFIGURATION_SIZE];

  u8	neighb_cellCount; /** number of neighbouring cell in the list, 0..maxNeighbouringCellIdentity (=32) is supposed, @see maxNeighbouringCellIdentity */	
  u16	neighb_cellConf_lgth	[MBMS_MAX_NEIGBOURING_COUNT]; /** neigbouring cells' configuration length, @see MBMS_NeighbouringCellConfiguration*/
  u8	neighb_cellConf_data	[MBMS_MAX_NEIGBOURING_COUNT][NEIGHBOUR_CONFIGURATION_SIZE]; /** neigbouring cells' configuration, @see MBMS_NeighbouringCellConfiguration*/
  u8	neighb_cellIDList	[MBMS_MAX_NEIGBOURING_COUNT]; /** list of neighb identity (1..X), used as a pointer to SIB 11? */
  u8	neighb_index; /** current neighb index of the list - Use it to create the msg corresponding*/

  u8	sched_numInfo; 	/** 0..maxMBMSservSched  */
  o3	sched_serviceIdentity		[maxMBMSservSched];		/** OctetString(3), @see MBMS_ServiceIdentity */	
  u8	sched_trans_numTransmis	[maxMBMSservSched];	/** 0..maxMBMSTransmis */	
  u16	sched_trans_start	[maxMBMSservSched][maxMBMSTransmis];	/** 0..1024 by step of 4 */
  u16	sched_trans_duration	[maxMBMSservSched][maxMBMSTransmis]; /** 4..1024 */
  u8	sched_nextSchedPeriod	[maxMBMSservSched];	/** 0..31 */
  
//	//Used for scheduling on MCCH
  mbms_bool	l12ConfigurationChanged;	/** Used to signal if we have to configure L12 at the end of modification period*/
//	u32	rg_mbms_frame;
//	u32	curr_repetition_counter;	/** counter for modification period */
//	u32	curr_modif_counter;		/** counter for modification period */
//	u32	curr_access_counter;		/** counter for access period */
//	u32	curr_sched_counter;		/** counter for scheduling period */

  u8	accessInfoPeriodCoef;
  u8	repetitionPeriodCoef;
  u8	modifPeriodCoef;
  u8	schedPeriodCoef;
// mbms_bool 	allowNotification; //allow the message Notification to be sent on DCCH at the begining of the repetion period.
  mbms_bool	allowCellRBInfo; //Turned on when we start at least one mbms service.
  rrc_rg_mbms_msg_flags flags; /** Used to see if the message need to be reconstructed ? */

  //Used to send MCCH message in the order specified by TS25.331	
  rrc_rg_mbms_msg_buf*	msgToSend_ptr;
  int			msgToSend_type; //the current message type to be sent
  int			msgToSend_neigbIndex; //Used only when neighbouring rb info msg is available

  //A list of DCCH, MCCH, MSCH stored messages during periods
  rrc_rg_mbms_msg_buf 	curr_Message;	/** Buffer for encoding message */
  rrc_rg_mbms_msg_buf 	messageAccess;
  rrc_rg_mbms_msg_buf 	messageNotification; //Modification Services Information on DCCH
  rrc_rg_mbms_msg_buf 	messageModif; //Modification Services Information on MCCH
  rrc_rg_mbms_msg_buf 	messageUnmodif;
  rrc_rg_mbms_msg_buf 	messageGeneral;
  rrc_rg_mbms_msg_buf 	messageCommon;
  rrc_rg_mbms_msg_buf 	messageCurrent;
  rrc_rg_mbms_msg_buf 	messageNeighbouring[MBMS_MAX_NEIGBOURING_COUNT]	;
  rrc_rg_mbms_msg_buf 	messageScheduling; 	


  //Other variables are stored here.
  int mbms_num_active_service;
  int nas_ueID;	
  o3  nas_serviceId;
  u16 nas_sessionId; //1 byte
  u16 nas_rbId; //
  u16 nas_QoSclass;
  u32 nas_sapId;
  u16 nas_duration;	
  int nas_status;
  o3	nas_joinedServices[maxMBMSServices]; /** OctetString(3), @see MBMS_ServiceIdentity */	
  o3	nas_leftServices[maxMBMSServices]; /** OctetString(3), @see MBMS_ServiceIdentity */	

  //mbms rrm config. flag =  TRUE --> contacted RRM; FALSE --> otherwise
  mbms_bool  ptm_config_requested;
  mbms_bool  ptm_config_updating;
  u32        ptm_config_transaction_id;
  int ptm_requested_action;

} rrc_rg_mbms_variables;

#endif
