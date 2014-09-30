/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
    included in this distribution in the file called "COPYING". If not,
    see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

*******************************************************************************/


/*! \file defs.h
* \brief RRC struct definitions and function prototypes
* \author Navid Nikaein and Raymond Knopp
* \date 2010 - 2014
* \version 1.0
* \company Eurecom
* \email: navid.nikaein@eurecom.fr, raymond.knopp@eurecom.fr 
*/

#ifndef __OPENAIR_RRC_DEFS_H__
#define __OPENAIR_RRC_DEFS_H__

#ifdef USER_MODE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include "rrc_types.h"
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
#include "RRCConnectionReestablishmentRequest.h"
#include "BCCH-DL-SCH-Message.h"
#include "BCCH-BCH-Message.h"
#ifdef Rel10
#include "MCCH-Message.h"
#include "MBSFNAreaConfiguration-r9.h"
#include "SCellToAddMod-r10.h"
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

#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
#endif

#if defined(ENABLE_USE_MME)
# include "commonDef.h"
#endif

#if defined(ENABLE_RAL)
# include "collection/hashtable/obj_hashtable.h"
#endif

/** @defgroup _rrc_impl_ RRC Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

#if defined(ENABLE_RAL)
typedef struct rrc_ral_threshold_key_s {
    ral_link_param_type_t   link_param_type;
    ral_threshold_t         threshold;
}rrc_ral_threshold_key_t;
#endif

//#define NUM_PRECONFIGURED_LCHAN (NB_CH_CX*2)  //BCCH, CCCH

#define UE_MODULE_INVALID ((module_id_t) ~0)
#define UE_INDEX_INVALID  ((module_id_t) ~0)

typedef enum UE_STATE_e {
  RRC_INACTIVE=0,
  RRC_IDLE,
  RRC_SI_RECEIVED,
  RRC_CONNECTED,
  RRC_RECONFIGURED,
  RRC_HO_EXECUTION
} UE_STATE_t;

typedef enum HO_STATE_e {
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
#define RRC_BUF_SIZE 255
#define UNDEF_SECURITY_MODE 0xff
#define NO_SECURITY_MODE 0x33

#define CBA_OFFSET        0xfff4
// #define NUM_MAX_CBA_GROUP 4 // in the platform_constants

typedef struct UE_RRC_INFO_s {
  UE_STATE_t State;
  uint8_t SIB1Status;
  uint8_t SIB1systemInfoValueTag;
  uint8_t SIStatus;
#ifdef Rel10
  uint8_t MCCHStatus[8]; // MAX_MBSFN_AREA
#endif
  uint8_t SIwindowsize;
  uint8_t handoverTarget;
  HO_STATE_t ho_state; 
  uint16_t SIperiod;
  unsigned short UE_index;
  uint32_t T300_active;
  uint32_t T300_cnt;
  uint32_t T304_active;
  uint32_t T304_cnt;
  uint32_t T310_active;
  uint32_t T310_cnt;
  uint32_t N310_cnt;
  uint32_t N311_cnt;
} __attribute__ ((__packed__)) UE_RRC_INFO;

typedef struct UE_S_TMSI_s {
    uint8_t  presence;
    uint8_t  mme_code;
    uint32_t m_tmsi;
} __attribute__ ((__packed__)) UE_S_TMSI;

#if defined(ENABLE_ITTI)
typedef enum e_rab_satus_e {
    E_RAB_STATUS_NEW,
    E_RAB_STATUS_DONE,
    E_RAB_STATUS_FAILED,
} e_rab_status_t;

typedef struct e_rab_param_s {
    e_rab_t param;
    uint8_t status;
} __attribute__ ((__packed__)) e_rab_param_t;
#endif

typedef struct eNB_RRC_UE_INFO_s {
    uint8_t Status;

#if defined(ENABLE_ITTI)
    /* Information from UE RRC ConnectionRequest */
    UE_S_TMSI Initialue_identity_s_TMSI;
    EstablishmentCause_t establishment_cause;

  /* Information from UE RRC ConnectionReestablishmentRequest */
    
    ReestablishmentCause_t reestablishment_cause;


    /* UE id for initial connection to S1AP */
    uint16_t ue_initial_id;

    /* Information from S1AP initial_context_setup_req */
    uint32_t eNB_ue_s1ap_id :24;

    security_capabilities_t security_capabilities;

    /* Number of e_rab to be setup in the list */
    uint8_t nb_of_e_rabs;
    /* list of e_rab to be setup by RRC layers */
    e_rab_param_t e_rab[S1AP_MAX_E_RAB];

    // LG: For GTPV1 TUNNELS
    uint32_t                enb_gtp_teid[S1AP_MAX_E_RAB];
    transport_layer_addr_t  enb_gtp_addrs[S1AP_MAX_E_RAB];
    rb_id_t                 enb_gtp_ebi[S1AP_MAX_E_RAB];

#endif
} __attribute__ ((__packed__)) eNB_RRC_UE_INFO;

typedef struct eNB_RRC_INFO_s {
  /* Number of UE handle by the eNB */
  uint8_t Nb_ue;

  /* UE list for UE index allocation */
  uint64_t UE_list[NUMBER_OF_UE_MAX];

  /* Information on UE */
  eNB_RRC_UE_INFO UE[NUMBER_OF_UE_MAX];
} __attribute__ ((__packed__)) eNB_RRC_INFO;

typedef struct RRC_INFO_s {
  int Status;
union{
	UE_RRC_INFO UE_info;
	eNB_RRC_INFO CH_info;
 }Info;
} RRC_INFO;

/* Intermediate structure for Hanodver management. Associated per-UE in eNB_RRC_INST */
typedef struct HANDOVER_INFO_s {
  uint8_t ho_prepare;
  uint8_t ho_complete;
  uint8_t modid_s; //module_idP of serving cell
  uint8_t modid_t; //module_idP of target cell
  uint8_t ueid_s; //UE index in serving cell
  uint8_t ueid_t; //UE index in target cell
  AS_Config_t as_config; /* these two parameters are taken from 36.331 section 10.2.2: HandoverPreparationInformation-r8-IEs */
  AS_Context_t as_context; /* They are mandatory for HO */
  uint8_t buf[RRC_BUF_SIZE];	/* ASN.1 encoded handoverCommandMessage */
  int size;		/* size of above message in bytes */
} HANDOVER_INFO;

#define RRC_HEADER_SIZE_MAX 64
#define RRC_BUFFER_SIZE_MAX 1024
typedef struct{
  char Payload[RRC_BUFFER_SIZE_MAX];
  char Header[RRC_HEADER_SIZE_MAX];  
  char payload_size;
}RRC_BUFFER; 
#define RRC_BUFFER_SIZE sizeof(RRC_BUFFER)

typedef struct RB_INFO_s {
  uint16_t Rb_id;  //=Lchan_id
  LCHAN_DESC Lchan_desc[2];
  MAC_MEAS_REQ_ENTRY *Meas_entry;
} RB_INFO;

typedef struct SRB_INFO_s {
  uint16_t Srb_id;  //=Lchan_id
  RRC_BUFFER Rx_buffer; 
  RRC_BUFFER Tx_buffer; 
  LCHAN_DESC Lchan_desc[2];
  unsigned int Trans_id;
  uint8_t Active;
} SRB_INFO;

typedef struct RB_INFO_TABLE_ENTRY_s {
  RB_INFO Rb_info;
  uint8_t Active;
  uint32_t Next_check_frame;
  uint8_t Status;
} RB_INFO_TABLE_ENTRY;

typedef struct SRB_INFO_TABLE_ENTRY_s {
  SRB_INFO Srb_info;
  uint8_t Active;
  uint8_t Status;
  uint32_t Next_check_frame;
}SRB_INFO_TABLE_ENTRY;

typedef struct MEAS_REPORT_LIST_s {
  MeasId_t measId;
  //CellsTriggeredList	cellsTriggeredList;//OPTIONAL
  uint32_t numberOfReportsSent;
} MEAS_REPORT_LIST;

typedef struct HANDOVER_INFO_UE_s {
  PhysCellId_t targetCellId;
  uint8_t measFlag;
} HANDOVER_INFO_UE;

typedef struct eNB_RRC_INST_s {
  uint8_t                           *SIB1;
  uint8_t                           sizeof_SIB1;
  uint8_t                           *SIB23;
  uint8_t                           sizeof_SIB23;
  uint16_t                          physCellId;
#ifdef Rel10
  SCellToAddMod_r10_t              sCell_config[NUMBER_OF_UE_MAX][2];
#endif
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
  MeasConfig_t                      *measConfig[NUMBER_OF_UE_MAX];
  HANDOVER_INFO                     *handover_info[NUMBER_OF_UE_MAX];
  uint8_t                           HO_flag;
#if defined(ENABLE_RAL)
  obj_hash_table_t                  *ral_meas_thresholds;
#endif
#if defined(ENABLE_SECURITY)
  /* KeNB as derived from KASME received from EPC */
  uint8_t kenb[NUMBER_OF_UE_MAX][32];
#endif
  /* Used integrity/ciphering algorithms */
  e_SecurityAlgorithmConfig__cipheringAlgorithm     ciphering_algorithm[NUMBER_OF_UE_MAX];
  e_SecurityAlgorithmConfig__integrityProtAlgorithm integrity_algorithm[NUMBER_OF_UE_MAX];
} eNB_RRC_INST;

#define MAX_UE_CAPABILITY_SIZE 255
typedef struct OAI_UECapability_s {
  uint8_t sdu[MAX_UE_CAPABILITY_SIZE];
  uint8_t sdu_size;
  UE_EUTRA_Capability_t *UE_EUTRA_Capability;
} OAI_UECapability_t;

typedef struct UE_RRC_INST_s {
  Rrc_State_t     RrcState;
  Rrc_Sub_State_t RrcSubState;
# if defined(ENABLE_USE_MME)
  plmn_t          plmnID;
  Byte_t          rat;
  as_nas_info_t   initialNasMsg;
# endif
  uint8_t *UECapability;
  uint8_t UECapability_size;
  UE_RRC_INFO Info[NB_SIG_CNX_UE];
  SRB_INFO Srb0[NB_SIG_CNX_UE];
  SRB_INFO_TABLE_ENTRY Srb1[NB_CNX_UE];
  SRB_INFO_TABLE_ENTRY Srb2[NB_CNX_UE];
  HANDOVER_INFO_UE HandoverInfoUe;
  uint8_t *SIB1[NB_CNX_UE];
  uint8_t sizeof_SIB1[NB_CNX_UE];
  uint8_t *SI[NB_CNX_UE];
  uint8_t sizeof_SI[NB_CNX_UE];
  uint8_t SIB1Status[NB_CNX_UE];
  uint8_t SIStatus[NB_CNX_UE];
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
  uint8_t *MCCH_MESSAGE[NB_CNX_UE];
  uint8_t sizeof_MCCH_MESSAGE[NB_CNX_UE];
  uint8_t MCCH_MESSAGEStatus[NB_CNX_UE];
  MBSFNAreaConfiguration_r9_t       *mcch_message[NB_CNX_UE];  
  SystemInformationBlockType12_r9_t *sib12[NB_CNX_UE];
  SystemInformationBlockType13_r9_t *sib13[NB_CNX_UE];
#endif 
#ifdef CBA
  uint8_t                         num_active_cba_groups;
  uint16_t                        cba_rnti[NUM_MAX_CBA_GROUP];
#endif
  uint8_t                         num_srb;
  struct SRB_ToAddMod             *SRB1_config[NB_CNX_UE];
  struct SRB_ToAddMod             *SRB2_config[NB_CNX_UE];
  struct DRB_ToAddMod             *DRB_config[NB_CNX_UE][8];
  MeasObjectToAddMod_t            *MeasObj[NB_CNX_UE][MAX_MEAS_OBJ];
  struct ReportConfigToAddMod     *ReportConfig[NB_CNX_UE][MAX_MEAS_CONFIG];
  struct QuantityConfig           *QuantityConfig[NB_CNX_UE];
  struct MeasIdToAddMod           *MeasId[NB_CNX_UE][MAX_MEAS_ID];
  MEAS_REPORT_LIST		  *measReportList[NB_CNX_UE][MAX_MEAS_ID];
  uint32_t				   measTimer[NB_CNX_UE][MAX_MEAS_ID][6]; // 6 neighboring cells
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
#if defined(ENABLE_RAL)
  obj_hash_table_t               *ral_meas_thresholds;
  ral_transaction_id_t            scan_transaction_id;
#endif
#if defined(ENABLE_SECURITY)
  /* KeNB as computed from parameters within USIM card */
  uint8_t kenb[32];
#endif

  /* Used integrity/ciphering algorithms */
  e_SecurityAlgorithmConfig__cipheringAlgorithm     ciphering_algorithm;
  e_SecurityAlgorithmConfig__integrityProtAlgorithm integrity_algorithm;
} UE_RRC_INST;

//main.c
int rrc_init_global_param(void);
int L3_xface_init(void);
void openair_rrc_top_init(int eMBMS_active, uint8_t cba_group_active,uint8_t HO_enabled);
char openair_rrc_lite_eNB_init(module_id_t module_idP);
char openair_rrc_lite_ue_init(module_id_t module_idP,uint8_t CH_IDX);
void rrc_config_buffer(SRB_INFO *srb_info, uint8_t Lchan_type, uint8_t Role);
void openair_rrc_on(module_id_t module_idP, eNB_flag_t eNB_flagP);
void rrc_top_cleanup(void);

/** \brief Function to update timers every subframe.  For UE it updates T300,T304 and T310.
@param module_idP Instance of UE/eNB
@param frame Frame index
@param eNB_flag Flag to indicate if this instance is and eNB or UE
@param index Index of corresponding eNB (for UE)
*/
RRC_status_t rrc_rx_tx(module_id_t module_idP,frame_t frameP, eNB_flag_t eNB_flagP,uint8_t index);

// UE RRC Procedures

/** \brief Decodes DL-CCCH message and invokes appropriate routine to handle the message
    \param module_idP Instance ID of UE
    \param Srb_info Pointer to SRB_INFO structure (SRB0)
    \param eNB_index Index of corresponding eNB/CH*/
int rrc_ue_decode_ccch(module_id_t module_idP, frame_t frameP, SRB_INFO *Srb_info,uint8_t eNB_index);

/** \brief Decodes a DL-DCCH message and invokes appropriate routine to handle the message
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param Srb_id Index of Srb (1,2)
    \param buffer_pP Pointer to received SDU
    \param eNB_index Index of corresponding CH/eNB*/
void rrc_ue_decode_dcch(module_id_t module_idP, frame_t frameP, uint8_t Srb_id, uint8_t* buffer_pP,uint8_t eNB_index);

/** \brief Generate/Encodes RRCConnnectionRequest message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param Srb_id Index of Srb (1,2)
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionRequest(module_id_t module_idP, frame_t frameP, uint8_t eNB_index);

/** \brief Generates/Encodes RRCConnnectionSetupComplete message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionSetupComplete(module_id_t module_idP,frame_t frameP,uint8_t eNB_index, uint8_t Transaction_id);

/** \brief process the received rrcConnectionReconfiguration message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param *rrcConnectionReconfiguration pointer to the sturcture
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_process_rrcConnectionReconfiguration(module_id_t module_idP, frame_t frameP,RRCConnectionReconfiguration_t *rrcConnectionReconfiguration,uint8_t eNB_index);

/** \brief Generates/Encodes RRCConnectionReconfigurationComplete  message at UE 
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH*/
void rrc_ue_generate_RRCConnectionReconfigurationComplete(module_id_t module_idP, frame_t frameP, uint8_t eNB_index, uint8_t Transaction_id);

/** \brief Establish SRB1 based on configuration in SRB_ToAddMod structure.  Configures RLC/PDCP accordingly
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH
    \param SRB_config Pointer to SRB_ToAddMod IE from configuration
    @returns 0 on success*/
int32_t  rrc_ue_establish_srb1(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,struct SRB_ToAddMod *SRB_config);

/** \brief Establish SRB2 based on configuration in SRB_ToAddMod structure.  Configures RLC/PDCP accordingly
    \param module_idP Instance ID of UE
    \param frame Frame index
    \param eNB_index Index of corresponding eNB/CH
    \param SRB_config Pointer to SRB_ToAddMod IE from configuration
    @returns 0 on success*/
int32_t  rrc_ue_establish_srb2(module_id_t module_idP,frame_t frameP, uint8_t eNB_index,struct SRB_ToAddMod *SRB_config);

/** \brief Establish a DRB according to DRB_ToAddMod structure
    \param module_idP Instance ID of UE
    \param eNB_index Index of corresponding CH/eNB
    \param DRB_config Pointer to DRB_ToAddMod IE from configuration
    @returns 0 on success */
int32_t  rrc_ue_establish_drb(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,struct DRB_ToAddMod *DRB_config);

/** \brief Process MobilityControlInfo Message to proceed with handover and configure PHY/MAC
    \param module_idP Instance of UE on which to act
    \param frame frame time interval
    \param eNB_index Index of corresponding CH/eNB
    \param mobilityControlInfo Pointer to mobilityControlInfo
*/
void rrc_ue_process_mobilityControlInfo(module_id_t enb_module_idP, module_id_t ue_module_idP, frame_t frameP, struct MobilityControlInfo *mobilityControlInfo);

/** \brief Process a measConfig Message and configure PHY/MAC
    \param module_idP Instance of UE on which to act
    \param frame frame time interval
    \param eNB_index Index of corresponding CH/eNB
    \param  measConfig Pointer to MeasConfig  IE from configuration*/
void rrc_ue_process_measConfig(module_id_t module_idP,frame_t frameP, uint8_t eNB_index,MeasConfig_t *measConfig);

/** \brief Process a RadioResourceConfigDedicated Message and configure PHY/MAC
    \param module_idP Instance of UE on which to act
    \param eNB_index Index of corresponding CH/eNB
    \param radioResourceConfigDedicated Pointer to RadioResourceConfigDedicated IE from configuration*/
void rrc_ue_process_radioResourceConfigDedicated(module_id_t module_idP,frame_t frameP, uint8_t eNB_index,
						 RadioResourceConfigDedicated_t *radioResourceConfigDedicated);

// eNB/CH RRC Procedures

/**\brief Function to get the next transaction identifier.
   \param module_idP Instance ID for CH/eNB
   \return a transaction identifier*/
uint8_t rrc_eNB_get_next_transaction_identifier(module_id_t module_idP);

/**\brief Entry routine to decode a UL-CCCH-Message.  Invokes PER decoder and parses message.
   \param module_idP Instance ID for CH/eNB
   \param frame  Frame index
   \param Srb_info Pointer to SRB0 information structure (buffer, etc.)*/
int rrc_eNB_decode_ccch(module_id_t module_idP, frame_t frameP, SRB_INFO *Srb_info);

/**\brief Entry routine to decode a UL-DCCH-Message.  Invokes PER decoder and parses message.
   \param module_idP Instance ID for CH/eNB
   \param frame Frame index
   \param ue_module_idP Index of UE sending the message
   \param Rx_sdu Pointer Received Message
   \param sdu_size Size of incoming SDU*/
int rrc_eNB_decode_dcch(module_id_t module_idP, frame_t frameP, uint8_t Srb_id, module_id_t ue_module_idP, uint8_t *Rx_sdu, sdu_size_t sdu_size);

/**\brief Generate the RRCConnectionSetup based on information coming from RRM
   \param module_idP Instance ID for eNB/CH
   \param frame Frame index
   \param ue_module_idP Index of UE receiving the message*/
void rrc_eNB_generate_RRCConnectionSetup(module_id_t module_idP,frame_t frameP, module_id_t ue_module_idP);

/**\brief Process the RRCConnectionSetupComplete based on information coming from UE
   \param module_idP Instance ID for eNB/CH
   \param frame Frame index
   \param ue_module_idP Index of UE transmitting the message
   \param rrcConnectionSetupComplete Pointer to RRCConnectionSetupComplete message*/
void rrc_eNB_process_RRCConnectionSetupComplete(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP,
    RRCConnectionSetupComplete_r8_IEs_t *rrcConnectionSetupComplete);

/**\brief Process the RRCConnectionReconfigurationComplete based on information coming from UE
   \param module_idP Instance ID for eNB/CH
   \param ue_module_idP Index of UE transmitting the messages
   \param rrcConnectionReconfigurationComplete Pointer to RRCConnectionReconfigurationComplete message*/
void rrc_eNB_process_RRCConnectionReconfigurationComplete(module_id_t module_idP,frame_t frameP,module_id_t ue_module_idP,RRCConnectionReconfigurationComplete_r8_IEs_t *rrcConnectionReconfigurationComplete);

#if defined(ENABLE_ITTI)
/**\brief RRC eNB task.
   \param void *args_p Pointer on arguments to start the task. */
void *rrc_enb_task(void *args_p);

/**\brief RRC UE task.
   \param void *args_p Pointer on arguments to start the task. */
void *rrc_ue_task(void *args_p);
#endif

/**\brief Generate/decode the handover RRCConnectionReconfiguration at eNB
   \param module_idP Instance ID for eNB/CH
   \param frame Frame index
   \param ue_module_idP Index of UE transmitting the messages*/
void rrc_eNB_generate_RRCConnectionReconfiguration_handover(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP, uint8_t *nas_pdu, uint32_t nas_length);

//L2_interface.c
int8_t mac_rrc_lite_data_req( module_id_t module_idP, frame_t frameP, rb_id_t Srb_id, uint8_t Nb_tb, uint8_t *buffer_pP,eNB_flag_t eNB_flagP, uint8_t eNB_index, uint8_t mbsfn_sync_area);

int8_t mac_rrc_lite_data_ind( module_id_t module_idP,  frame_t frameP, rb_id_t Srb_id, uint8_t *Sdu, sdu_size_t Sdu_len,eNB_flag_t eNB_flagP,uint8_t eNB_index, uint8_t mbsfn_sync_area);

void mac_sync_ind( module_id_t Mod_instP, uint8_t status);

uint8_t rrc_lite_data_req(module_id_t enb_mod_idP,
                     module_id_t      ue_mod_idP,
                     frame_t          frameP,
                     eNB_flag_t       eNB_flagP,
                     rb_id_t          rb_idP,
                     mui_t            muiP,
                     confirm_t        confirmP,
                     sdu_size_t       sdu_sizeP,
                     uint8_t         *Buffer_pP,
                     pdcp_transmission_mode_t   modeP);

void rrc_lite_data_ind(module_id_t eNB_id, module_id_t UE_id, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t Rb_id, sdu_size_t sdu_size,uint8_t *buffer_pP);

void rrc_lite_in_sync_ind(module_id_t module_idP, frame_t frameP, uint16_t eNB_index);

void rrc_lite_out_of_sync_ind(module_id_t module_idP, frame_t frameP, unsigned short eNB_index);

int decode_MCCH_Message(module_id_t module_idP, frame_t frameP, uint8_t eNB_index, uint8_t *Sdu, uint8_t Sdu_len,uint8_t mbsfn_sync_area);

void decode_MBSFNAreaConfiguration(module_id_t module_idP, uint8_t eNB_index, frame_t frameP,uint8_t mbsfn_sync_area);

int decode_BCCH_DLSCH_Message(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,uint8_t *Sdu,uint8_t Sdu_len, uint8_t rsrq, uint8_t rsrp);

int decode_SIB1(module_id_t module_idP,uint8_t eNB_index, uint8_t rsrq, uint8_t rsrp);

int decode_SI(module_id_t module_idP,frame_t frameP,uint8_t eNB_index,uint8_t si_window);

void ue_meas_filtering(module_id_t module_idP,frame_t frameP,uint8_t eNB_index);

void ue_measurement_report_triggering(module_id_t module_idP, frame_t frameP,uint8_t eNB_index);

int mac_get_rrc_lite_status(module_id_t module_idP,eNB_flag_t eNB_flagP,uint8_t index);

void rrc_eNB_generate_UECapabilityEnquiry(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP);

void rrc_eNB_generate_SecurityModeCommand(module_id_t module_idP, frame_t frameP, module_id_t ue_module_idP);

void rrc_eNB_process_MeasurementReport(uint8_t module_idP,frame_t frameP, module_id_t ue_module_idP,MeasResults_t *measResults2) ;

void rrc_ue_generate_MeasurementReport(module_id_t eNB_id, module_id_t UE_id, frame_t frameP);

void rrc_eNB_generate_HandoverPreparationInformation (uint8_t module_idP, frame_t frameP, module_id_t ue_module_idP, PhysCellId_t targetPhyId) ;

void check_handovers(uint8_t module_idP, frame_t frameP);

uint8_t check_trigger_meas_event(uint8_t module_idP,frame_t frameP, uint8_t eNB_index, uint8_t ue_cnx_index, uint8_t meas_index,
			    Q_OffsetRange_t ofn, Q_OffsetRange_t ocn, Hysteresis_t hys, 
			    Q_OffsetRange_t ofs, Q_OffsetRange_t ocs, long a3_offset, TimeToTrigger_t ttt);

//void rrc_ue_process_ueCapabilityEnquiry(uint8_t module_idP,uint32_t frame,UECapabilityEnquiry_t *UECapabilityEnquiry,uint8_t eNB_index);
//void rrc_ue_process_securityModeCommand(uint8_t module_idP,uint32_t frame,SecurityModeCommand_t *securityModeCommand,uint8_t eNB_index);

void rrc_eNB_free_UE_index (module_id_t enb_mod_idP, module_id_t ue_mod_idP);

long binary_search_int(int elements[], long numElem, int value);

long binary_search_float(float elements[], long numElem, float value);
#endif
/** @ */
