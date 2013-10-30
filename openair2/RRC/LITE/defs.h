/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/


/*! \file defs.h
* \brief struct definitions and func prototypes
* \author Raymond Knopp and Navid Nikaein
* \date 2011, 2012, 2013
* \version 1.0
* \company Eurecom
* \email: raymond.knopp@eurecom.fr and navid.nikaein@eurecom.fr
*/


#ifndef __OPENAIR_RRC_DEFS_H__
#define __OPENAIR_RRC_DEFS_H__

#ifdef USER_MODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "PHY/defs.h"
#include "COMMON/platform_constants.h"

#include "COMMON/mac_rrc_primitives.h"
#include "LAYER2/MAC/defs.h"

//#include "COMMON/openair_defs.h"
#ifndef USER_MODE
#include <rtai.h>
#endif

#include "SystemInformationBlockType1.h"
#include "SystemInformation.h"
#include "RRCConnectionReconfiguration.h"
#include "RRCConnectionReconfigurationComplete.h"
#include "RRCConnectionSetup.h"
#include "RRCConnectionSetupComplete.h"
#include "RRCConnectionRequest.h"
#include "BCCH-DL-SCH-Message.h"
#include "BCCH-BCH-Message.h"
#ifdef Rel10
#include "MCCH-Message.h"
#include "MBSFNAreaConfiguration-r9.h"
#endif
#include "AS-Config.h"
#include "AS-Context.h"
#include "UE-EUTRA-Capability.h"
#include "MeasResults.h"

//#include "L3_rrc_defs.h"
#ifndef NO_RRM
#include "L3_rrc_interface.h"
#include "rrc_rrm_msg.h"
#include "rrc_rrm_interface.h"
#endif

/** @defgroup _rrc_impl_ RRC Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

//#define NUM_PRECONFIGURED_LCHAN (NB_CH_CX*2)  //BCCH, CCCH

typedef enum  {
  RRC_IDLE=0,
  RRC_SI_RECEIVED,
  RRC_CONNECTED,
  RRC_RECONFIGURED,
  RRC_HO_EXECUTION
} UE_STATE_t;


typedef enum  {
  HO_IDLE=0,
  HO_MEASURMENT,
  HO_PREPARE,
  HO_CMD, // initiated by the src eNB
  HO_COMPLETE // initiated by the target eNB
} HO_STATE_t;


//#define NUMBER_OF_UE_MAX MAX_MOBILES_PER_RG
#define RRM_FREE(p)       if ( (p) != NULL) { free(p) ; p=NULL ; }
#define RRM_MALLOC(t,n)   (t *) malloc16( sizeof(t) * n ) 
#define RRM_CALLOC(t,n)   (t *) malloc16( sizeof(t) * n) 
#define RRM_CALLOC2(t,s)  (t *) malloc16( s ) 

#define MAX_MEAS_OBJ 6
#define MAX_MEAS_CONFIG 6
#define MAX_MEAS_ID 6

#define PAYLOAD_SIZE_MAX 1024
#define RRC_BUF_SIZE 140
#define UNDEF_SECURITY_MODE 0xff
#define NO_SECURITY_MODE 0x33


#define CBA_OFFSET        0xfff4
// #define NUM_MAX_CBA_GROUP 4 // in the platform_constants

typedef struct{
  UE_STATE_t State;
  u8 SIB1Status;
  u8 SIStatus;
#ifdef Rel10
  u8 MCCHStatus[8]; // MAX_MBSFN_AREA
#endif
  u8 SIwindowsize;
  u8 handoverTarget;
  HO_STATE_t ho_state; 
  u16 SIperiod;
  unsigned short UE_index;
  u32 T300_active;
  u32 T300_cnt;
  u32 T304_active;
  u32 T304_cnt;
  u32 T310_active;
  u32 T310_cnt;
  u32 N310_cnt;
  u32 N311_cnt;
}UE_RRC_INFO;

typedef struct{
  u8 Status[NUMBER_OF_UE_MAX];
  u8 Nb_ue;
  //unsigned short UE_index_list[NUMBER_OF_UE_MAX];
  //L2_ID UE_list[NUMBER_OF_UE_MAX];
  u8 UE_list[NUMBER_OF_UE_MAX][5];
}__attribute__ ((__packed__)) eNB_RRC_INFO;

typedef struct{
  int Status;
union{
	UE_RRC_INFO UE_info;
	eNB_RRC_INFO CH_info;
 }Info;
}RRC_INFO;

/* Intermediate structure for Hanodver management. Associated per-UE in eNB_RRC_INST */
typedef struct{
  u8 ho_prepare;
  u8 ho_complete;
  u8 modid_s; //Mod_id of serving cell
  u8 modid_t; //Mod_id of target cell
  u8 ueid_s; //UE index in serving cell
  u8 ueid_t; //UE index in target cell
  AS_Config_t as_config; /* these two parameters are taken from 36.331 section 10.2.2: HandoverPreparationInformation-r8-IEs */
  AS_Context_t as_context; /* They are mandatory for HO */
  uint8_t buf[RRC_BUF_SIZE];	/* ASN.1 encoded handoverCommandMessage */
  int size;		/* size of above message in bytes */
}HANDOVER_INFO;

#define RRC_HEADER_SIZE_MAX 64
#define RRC_BUFFER_SIZE_MAX 1024
typedef struct{
  char Payload[RRC_BUFFER_SIZE_MAX];
  char Header[RRC_HEADER_SIZE_MAX];  
  char payload_size;
}RRC_BUFFER; 
#define RRC_BUFFER_SIZE sizeof(RRC_BUFFER)

typedef struct{
  u16 Rb_id;  //=Lchan_id
  LCHAN_DESC Lchan_desc[2];
  MAC_MEAS_REQ_ENTRY *Meas_entry;
}RB_INFO;

typedef struct SRB_INFO{
  u16 Srb_id;  //=Lchan_id
  RRC_BUFFER Rx_buffer; 
  RRC_BUFFER Tx_buffer; 
  LCHAN_DESC Lchan_desc[2];
  unsigned int Trans_id;
  u8 Active;
}SRB_INFO;

typedef struct{
  RB_INFO Rb_info;
  u8 Active;
  u32 Next_check_frame;
  u8 Status;
}RB_INFO_TABLE_ENTRY;

typedef struct{
  SRB_INFO Srb_info;
  u8 Active;
  u8 Status;
  u32 Next_check_frame;
}SRB_INFO_TABLE_ENTRY;

typedef struct {
  MeasId_t measId;
  //CellsTriggeredList	cellsTriggeredList;//OPTIONAL
  u32 numberOfReportsSent;
} MEAS_REPORT_LIST;

typedef struct {
  PhysCellId_t targetCellId;
  u8 measFlag;
}HANDOVER_INFO_UE;


typedef struct{
  uint8_t                           *SIB1;
  uint8_t                           sizeof_SIB1;
  uint8_t                           *SIB23;
  uint8_t                           sizeof_SIB23;
  uint16_t                          physCellId;
  BCCH_BCH_Message_t                mib;
  BCCH_DL_SCH_Message_t             siblock1;
  BCCH_DL_SCH_Message_t             systemInformation;
  //  SystemInformation_t               systemInformation;
  SystemInformationBlockType1_t     *sib1;
  SystemInformationBlockType2_t     *sib2;
  SystemInformationBlockType3_t     *sib3;
#ifdef Rel10
  SystemInformationBlockType13_r9_t *sib13;
  uint8_t                           MBMS_flag;
  uint8_t                           num_mbsfn_sync_area;
  uint8_t                           **MCCH_MESSAGE; //  MAX_MBSFN_AREA
  uint8_t                           sizeof_MCCH_MESSAGE[8];// MAX_MBSFN_AREA
  MCCH_Message_t            mcch;
  MBSFNAreaConfiguration_r9_t       *mcch_message;  
  SRB_INFO                          MCCH_MESS[8];// MAX_MBSFN_AREA
#endif 
#ifdef CBA
  uint8_t                        num_active_cba_groups;
  uint16_t                       cba_rnti[NUM_MAX_CBA_GROUP];
#endif
  SRB_ToAddModList_t                *SRB_configList[NUMBER_OF_UE_MAX];
  DRB_ToAddModList_t                *DRB_configList[NUMBER_OF_UE_MAX];
  uint8_t                           DRB_active[NUMBER_OF_UE_MAX][8];
  struct PhysicalConfigDedicated    *physicalConfigDedicated[NUMBER_OF_UE_MAX];
  struct SPS_Config                 *sps_Config[NUMBER_OF_UE_MAX];
  MeasObjectToAddMod_t              *MeasObj[NUMBER_OF_UE_MAX][MAX_MEAS_OBJ];
  struct ReportConfigToAddMod       *ReportConfig[NUMBER_OF_UE_MAX][MAX_MEAS_CONFIG];
  struct QuantityConfig             *QuantityConfig[NUMBER_OF_UE_MAX];
  struct MeasIdToAddMod             *MeasId[NUMBER_OF_UE_MAX][MAX_MEAS_ID];
  MAC_MainConfig_t                  *mac_MainConfig[NUMBER_OF_UE_MAX];
  MeasGapConfig_t                   *measGapConfig[NUMBER_OF_UE_MAX];
  eNB_RRC_INFO                      Info;
  SRB_INFO                          SI;
  SRB_INFO                          Srb0;
  SRB_INFO_TABLE_ENTRY              Srb1[NUMBER_OF_UE_MAX+1];
  SRB_INFO_TABLE_ENTRY              Srb2[NUMBER_OF_UE_MAX+1];
  MeasConfig_t			    *measConfig[NUMBER_OF_UE_MAX];
  HANDOVER_INFO			    *handover_info[NUMBER_OF_UE_MAX];
  uint8_t                           HO_flag;
#if defined(ENABLE_SECURITY)
  /* KeNB as derived from KASME received from EPC */
  uint8_t kenb[NUMBER_OF_UE_MAX][32];
#endif
  /* Used integrity/ciphering algorithms */
  e_SecurityAlgorithmConfig__cipheringAlgorithm     ciphering_algorithm[NUMBER_OF_UE_MAX];
  e_SecurityAlgorithmConfig__integrityProtAlgorithm integrity_algorithm[NUMBER_OF_UE_MAX];
} eNB_RRC_INST;

#define MAX_UE_CAPABILITY_SIZE 255
typedef struct{
  uint8_t sdu[MAX_UE_CAPABILITY_SIZE];
  uint8_t sdu_size;
  UE_EUTRA_Capability_t *UE_EUTRA_Capability;
} OAI_UECapability_t;

typedef struct{
  uint8_t *UECapability;
  uint8_t UECapability_size;
  UE_RRC_INFO Info[NB_SIG_CNX_UE];
  SRB_INFO Srb0[NB_SIG_CNX_UE];
  SRB_INFO_TABLE_ENTRY Srb1[NB_CNX_UE];
  SRB_INFO_TABLE_ENTRY Srb2[NB_CNX_UE];
  HANDOVER_INFO_UE HandoverInfoUe;
  u8 *SIB1[NB_CNX_UE];
  u8 sizeof_SIB1[NB_CNX_UE];
  u8 *SI[NB_CNX_UE];
  u8 sizeof_SI[NB_CNX_UE];
  u8 SIB1Status[NB_CNX_UE];
  u8 SIStatus[NB_CNX_UE];
  SystemInformationBlockType1_t *sib1[NB_CNX_UE];
  SystemInformation_t *si[NB_CNX_UE][8];
  SystemInformationBlockType2_t *sib2[NB_CNX_UE];
  SystemInformationBlockType3_t *sib3[NB_CNX_UE];
  SystemInformationBlockType4_t *sib4[NB_CNX_UE];
  SystemInformationBlockType5_t *sib5[NB_CNX_UE];
  SystemInformationBlockType6_t *sib6[NB_CNX_UE];
  SystemInformationBlockType7_t *sib7[NB_CNX_UE];
  SystemInformationBlockType8_t *sib8[NB_CNX_UE];
  SystemInformationBlockType9_t *sib9[NB_CNX_UE];
  SystemInformationBlockType10_t *sib10[NB_CNX_UE];
  SystemInformationBlockType11_t *sib11[NB_CNX_UE];

#ifdef Rel10
  uint8_t                           MBMS_flag;
  u8 *MCCH_MESSAGE[NB_CNX_UE];
  u8 sizeof_MCCH_MESSAGE[NB_CNX_UE];
  u8 MCCH_MESSAGEStatus[NB_CNX_UE];
  MBSFNAreaConfiguration_r9_t       *mcch_message[NB_CNX_UE];  
  SystemInformationBlockType12_r9_t *sib12[NB_CNX_UE];
  SystemInformationBlockType13_r9_t *sib13[NB_CNX_UE];
#endif 
#ifdef CBA
  uint8_t                         num_active_cba_groups;
  uint16_t                        cba_rnti[NUM_MAX_CBA_GROUP];
#endif
  struct SRB_ToAddMod             *SRB1_config[NB_CNX_UE];
  struct SRB_ToAddMod             *SRB2_config[NB_CNX_UE];
  struct DRB_ToAddMod             *DRB_config[NB_CNX_UE][8];
  MeasObjectToAddMod_t            *MeasObj[NB_CNX_UE][MAX_MEAS_OBJ];
  struct ReportConfigToAddMod     *ReportConfig[NB_CNX_UE][MAX_MEAS_CONFIG];
  struct QuantityConfig           *QuantityConfig[NB_CNX_UE];
  struct MeasIdToAddMod           *MeasId[NB_CNX_UE][MAX_MEAS_ID];
  MEAS_REPORT_LIST		  *measReportList[NB_CNX_UE][MAX_MEAS_ID];
  u32				   measTimer[NB_CNX_UE][MAX_MEAS_ID][6]; // 6 neighboring cells
  RSRP_Range_t                    s_measure;
  struct MeasConfig__speedStatePars *speedStatePars;
  struct PhysicalConfigDedicated  *physicalConfigDedicated[NB_CNX_UE];
  struct SPS_Config               *sps_Config[NB_CNX_UE];
  MAC_MainConfig_t                *mac_MainConfig[NB_CNX_UE];
  MeasGapConfig_t                 *measGapConfig[NB_CNX_UE];
  double                          filter_coeff_rsrp; // [7] ???
  double                          filter_coeff_rsrq; // [7] ??? 
  float                           rsrp_db[7];
  float                           rsrq_db[7];
  float                           rsrp_db_filtered[7];
  float                           rsrq_db_filtered[7];
#if defined(ENABLE_SECURITY)
  /* KeNB as computed from parameters within USIM card */
  uint8_t kenb[32];
#endif

  /* Used integrity/ciphering algorithms */
  e_SecurityAlgorithmConfig__cipheringAlgorithm     ciphering_algorithm;
  e_SecurityAlgorithmConfig__integrityProtAlgorithm integrity_algorithm;
}UE_RRC_INST;

//main.c
int rrc_init_global_param(void);
int L3_xface_init(void);
void openair_rrc_top_init(int eMBMS_active, u8 cba_group_active,u8 HO_enabled);
char openair_rrc_lite_eNB_init(u8 Mod_id);
char openair_rrc_lite_ue_init(u8 Mod_id,u8 CH_IDX);
void rrc_config_buffer(SRB_INFO *srb_info, u8 Lchan_type, u8 Role);
void openair_rrc_on(u8 Mod_id,u8 eNB_flag);
void rrc_top_cleanup(void);

/** \brief Function to update timers every subframe.  For UE it updates T300,T304 and T310.
@param Mod_id Instance of UE/eNB
@param frame Frame index
@param eNB_flag Flag to indicate if this instance is and eNB or UE
@param index Index of corresponding eNB (for UE)
*/
RRC_status_t rrc_rx_tx(u8 Mod_id,u32 frame, u8 eNB_flag,u8 index);

// UE RRC Procedures

/** \brief Decodes DL-CCCH message and invokes appropriate routine to handle the message
    \param Mod_id Instance ID of UE
    \param Srb_info Pointer to SRB_INFO structure (SRB0)
    \param eNB_index Index of corresponding eNB/CH*/
int rrc_ue_decode_ccch(u8 Mod_id, u32 frame, SRB_INFO *Srb_info,u8 eNB_index);

/** \brief Decodes a DL-DCCH message and invokes appropriate routine to handle the message
    \param Mod_id Instance ID of UE
    \param frame Frame index
    \param Srb_id Index of Srb (1,2)
    \param Buffer Pointer to received SDU
    \param eNB_index Index of corresponding CH/eNB*/
void rrc_ue_decode_dcch(u8 Mod_id, u32 frame, u8 Srb_id, u8* Buffer,u8 eNB_index);

/** \brief Generate/Encodes RRCConnnectionRequest message at UE 
    \param Mod_id Instance ID of UE
    \param frame Frame index
    \param Srb_id Index of Srb (1,2)
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionRequest(u8 Mod_id, u32 frame, u8 eNB_index);

/** \brief Generates/Encodes RRCConnnectionSetupComplete message at UE 
    \param Mod_id Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionSetupComplete(u8 Mod_id,u32 frame,u8 eNB_index);

/** \brief process the received rrcConnectionReconfiguration message at UE 
    \param Mod_id Instance ID of UE
    \param frame Frame index
    \param *rrcConnectionReconfiguration pointer to the sturcture
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_process_rrcConnectionReconfiguration(u8 Mod_id, u32 frame,RRCConnectionReconfiguration_t *rrcConnectionReconfiguration,u8 eNB_index);

/** \brief Generates/Encodes RRCConnectionReconfigurationComplete  message at UE 
    \param Mod_id Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionReconfigurationComplete(u8 Mod_id, u32 frame, u8 eNB_index);

/** \brief Establish SRB1 based on configuration in SRB_ToAddMod structure.  Configures RLC/PDCP accordingly
    \param Mod_id Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH
    \param SRB_config Pointer to SRB_ToAddMod IE from configuration
    @returns 0 on success*/
s32  rrc_ue_establish_srb1(u8 Mod_id,u32 frame,u8 eNB_index,struct SRB_ToAddMod *SRB_config);

/** \brief Establish SRB2 based on configuration in SRB_ToAddMod structure.  Configures RLC/PDCP accordingly
    \param Mod_id Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH
    \param SRB_config Pointer to SRB_ToAddMod IE from configuration
    @returns 0 on success*/
s32  rrc_ue_establish_srb2(u8 Mod_id,u32 frame, u8 eNB_index,struct SRB_ToAddMod *SRB_config);

/** \brief Establish a DRB according to DRB_ToAddMod structure
    \param Mod_id Instance ID of UE
    \param eNB_index Index of corresponding CH/eNB
    \param DRB_config Pointer to DRB_ToAddMod IE from configuration
    @returns 0 on success */
s32  rrc_ue_establish_drb(u8 Mod_id,u32 frame,u8 eNB_index,struct DRB_ToAddMod *DRB_config);


/** \brief Process MobilityControlInfo Message to proceed with handover and configure PHY/MAC
    \param Mod_id Instance of UE on which to act
    \param frame frame time interval
    \param eNB_index Index of corresponding CH/eNB
    \param mobilityControlInfo Pointer to mobilityControlInfo
*/

void   rrc_ue_process_mobilityControlInfo(u8 Mod_id,u32 frame, u8 eNB_index,struct MobilityControlInfo *mobilityControlInfo);

/** \brief Process a measConfig Message and configure PHY/MAC
    \param Mod_id Instance of UE on which to act
    \param frame frame time interval
    \param eNB_index Index of corresponding CH/eNB
    \param  measConfig Pointer to MeasConfig  IE from configuration*/
void	rrc_ue_process_measConfig(u8 Mod_id,u32 frame, u8 eNB_index,MeasConfig_t *measConfig);

/** \brief Process a RadioResourceConfigDedicated Message and configure PHY/MAC
    \param Mod_id Instance of UE on which to act
    \param eNB_index Index of corresponding CH/eNB
    \param radioResourceConfigDedicated Pointer to RadioResourceConfigDedicated IE from configuration*/
void rrc_ue_process_radioResourceConfigDedicated(u8 Mod_id,u32 frame, u8 eNB_index,
						 RadioResourceConfigDedicated_t *radioResourceConfigDedicated);

// eNB/CH RRC Procedures

/**\brief Entry routine to decode a UL-CCCH-Message.  Invokes PER decoder and parses message.
   \param Mod_id Instance ID for CH/eNB
   \param frame  Frame index
   \param Srb_info Pointer to SRB0 information structure (buffer, etc.)*/
int rrc_eNB_decode_ccch(u8 Mod_id, u32 frame, SRB_INFO *Srb_info);

/**\brief Entry routine to decode a UL-DCCH-Message.  Invokes PER decoder and parses message.
   \param Mod_id Instance ID for CH/eNB
   \param frame Frame index
   \param UE_index Index of UE sending the message
   \param Rx_sdu Pointer Received Message
   \param sdu_size Size of incoming SDU*/
int rrc_eNB_decode_dcch(u8 Mod_id, u32 frame, u8 Srb_id, u8 UE_index, u8 *Rx_sdu, u8 sdu_size);  

/**\brief Generate the RRCConnectionSetup based on information coming from RRM
   \param Mod_id Instance ID for eNB/CH
   \param frame Frame index
   \param UE_index Index of UE receiving the message*/
void rrc_eNB_generate_RRCConnectionSetup(u8 Mod_id,u32 frame, u16 UE_index);

/**\brief Process the RRCConnectionSetupComplete based on information coming from UE
   \param Mod_id Instance ID for eNB/CH
   \param frame Frame index
   \param UE_index Index of UE transmitting the message
   \param rrcConnectionSetupComplete Pointer to RRCConnectionSetupComplete message*/
void rrc_eNB_process_RRCConnectionSetupComplete(u8 Mod_id, u32 frame, u8 UE_index, 
						RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete);

/**\brief Process the RRCConnectionReconfigurationComplete based on information coming from UE
   \param Mod_id Instance ID for eNB/CH
   \param UE_index Index of UE transmitting the messages
   \param rrcConnectionReconfigurationComplete Pointer to RRCConnectionReconfigurationComplete message*/
void rrc_eNB_process_RRCConnectionReconfigurationComplete(u8 Mod_id,u32 frame,u8 UE_index,RRCConnectionReconfigurationComplete_r8_IEs_t *rrcConnectionReconfigurationComplete);

/**\brief Generate/decode the Default (first) RRCConnectionReconfiguration at eNB
   \param Mod_id Instance ID for eNB/CH
   \param frame Frame index
   \param UE_index Index of UE transmitting the messages*/
void rrc_eNB_generate_defaultRRCConnectionReconfiguration(u8 Mod_id, u32 frame, u16 UE_index, u8 *nas_pdu, u32 nas_length, u8 ho_state);

#if defined(ENABLE_ITTI)
/**\brief RRC eNB task.
   \param void *args_p Pointer on arguments to start the task. */
void *rrc_enb_task(void *args_p);

/**\brief RRC UE task.
   \param void *args_p Pointer on arguments to start the task. */
void *rrc_ue_task(void *args_p);
#endif

/**\brief Generate/decode the handover RRCConnectionReconfiguration at eNB
   \param Mod_id Instance ID for eNB/CH
   \param frame Frame index
   \param UE_index Index of UE transmitting the messages*/
void rrc_eNB_generate_RRCConnectionReconfiguration_handover(u8 Mod_id, u32 frame, u16 UE_index, u8 *nas_pdu, u32 nas_length);


//L2_interface.c
s8 mac_rrc_lite_data_req( u8 Mod_id, u32 frame, unsigned short Srb_id, u8 Nb_tb, u8 *Buffer,u8 eNB_flag, u8 eNB_index, u8 mbsfn_sync_area);
s8 mac_rrc_lite_data_ind( u8 Mod_id,  u32 frame, unsigned short Srb_id, u8 *Sdu, unsigned short Sdu_len,u8 eNB_flag,u8 eNB_index, u8 mbsfn_sync_area);
void mac_sync_ind( u8 Mod_id, u8 status);
u8 rrc_lite_data_req(u8 Mod_id, u32 frame, u8 eNB_flag, unsigned int rb_id, u32 muiP, u32 confirmP,
                     unsigned int sdu_size, u8* Buffer, u8 mode);
void rrc_lite_data_ind( u8 Mod_id, u32 frame, u8 eNB_flag, u32 Rb_id, u32 sdu_size,u8 *Buffer);
void rrc_lite_in_sync_ind(u8 Mod_id, u32 frame, u16 eNB_index);
void rrc_lite_out_of_sync_ind(u8 Mod_id, u32 frame, unsigned short eNB_index);

int decode_MCCH_Message(u8 Mod_id, u32 frame, u8 eNB_index, u8 *Sdu, u8 Sdu_len,u8 mbsfn_sync_area);
void decode_MBSFNAreaConfiguration(u8 Mod_id, u8 eNB_index, u32 frame,u8 mbsfn_sync_area);

int decode_BCCH_DLSCH_Message(u8 Mod_id,u32 frame,u8 eNB_index,u8 *Sdu,u8 Sdu_len);

  int decode_SIB1(u8 Mod_id,u8 eNB_index);

int decode_SI(u8 Mod_id,u32 frame,u8 eNB_index,u8 si_window);

int mac_get_rrc_lite_status(u8 Mod_id,u8 eNB_flag,u8 index);

void rrc_eNB_generate_UECapabilityEnquiry(u8 Mod_id, u32 frame, u16 UE_index);
void rrc_eNB_generate_SecurityModeCommand(u8 Mod_id, u32 frame, u16 UE_index);

void rrc_eNB_process_MeasurementReport(u8 Mod_id,u32 frame, u16 UE_index,MeasResults_t *measResults2) ;
void rrc_eNB_generate_HandoverPreparationInformation (u8 Mod_id, u32 frame, u8 UE_index, PhysCellId_t targetPhyId) ;
u8 check_trigger_meas_event(u8 Mod_id,u32 frame, u8 eNB_index, u8 ue_cnx_index, u8 meas_index, 
			    Q_OffsetRange_t ofn, Q_OffsetRange_t ocn, Hysteresis_t hys, 
			    Q_OffsetRange_t ofs, Q_OffsetRange_t ocs, long a3_offset, TimeToTrigger_t ttt);

//void rrc_ue_process_ueCapabilityEnquiry(uint8_t Mod_id,uint32_t frame,UECapabilityEnquiry_t *UECapabilityEnquiry,uint8_t eNB_index);
//void rrc_ue_process_securityModeCommand(uint8_t Mod_id,uint32_t frame,SecurityModeCommand_t *securityModeCommand,uint8_t eNB_index);

void rrc_remove_UE (u8 Mod_id, u8 UE_id);

long binary_search_int(int elements[], long numElem, int value);
long binary_search_float(float elements[], long numElem, float value);
#endif


/** @ */
