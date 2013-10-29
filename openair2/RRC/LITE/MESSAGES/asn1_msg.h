#ifdef USER_MODE
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>	/* for atoi(3) */
#include <unistd.h>	/* for getopt(3) */
#include <string.h>	/* for strerror(3) */
#include <sysexits.h>	/* for EX_* exit codes */
#include <errno.h>	/* for errno */
#else
#include <linux/module.h>  /* Needed by all modules */
#endif

#include <asn_application.h>
#include <asn_internal.h>	/* for _ASN_DEFAULT_STACK_MAX */

#include "RRC/LITE/defs.h"

/** 
\brief Generate a default configuration for SIB1 (eNB).
@param frame_parms Used to store some basic parameters from PHY configuration
@param buffer Pointer to PER-encoded ASN.1 description of SIB1
@param sib1 Pointer to asn1c C representation of SIB1
@return size of encoded bit stream in bytes*/

uint8_t do_SIB1(LTE_DL_FRAME_PARMS *frame_parms, uint8_t *buffer,
		BCCH_DL_SCH_Message_t *bcch_message,
		SystemInformationBlockType1_t **sib1);

/** 
\brief Generate a default configuration for SIB2/SIB3 in one System Information PDU (eNB).
@param Mod_id Index of eNB (used to derive some parameters)
@param buffer Pointer to PER-encoded ASN.1 description of SI PDU
@param systemInformation Pointer to asn1c C representation of SI PDU
@param sib2 Pointer (returned) to sib2 component withing SI PDU
@param sib3 Pointer (returned) to sib3 component withing SI PDU
@param sib13 Pointer (returned) to sib13 component withing SI PDU
@param MBMS_flag Indicates presence of MBMS system information (when 1)
@return size of encoded bit stream in bytes*/

uint8_t do_SIB23(uint8_t Mod_id,
		 LTE_DL_FRAME_PARMS *frame_parms,
		 uint8_t *buffer,
		 BCCH_DL_SCH_Message_t *systemInformation,
		 SystemInformationBlockType2_t **sib2,
		 SystemInformationBlockType3_t **sib3
#ifdef Rel10
		 ,
                 SystemInformationBlockType13_r9_t **sib13,
		 uint8_t MBMS_flag
#endif
);

/** 
\brief Generate an RRCConnectionRequest UL-CCCH-Message (UE) based on random string or S-TMSI.  This 
routine only generates an mo-data establishment cause.
@param buffer Pointer to PER-encoded ASN.1 description of UL-DCCH-Message PDU
@param rv 5 byte random string or S-TMSI
@returns Size of encoded bit stream in bytes*/

uint8_t do_RRCConnectionRequest(uint8_t *buffer,u8 *rv);

/** \brief Generate an RRCConnectionSetupComplete UL-DCCH-Message (UE)
@param buffer Pointer to PER-encoded ASN.1 description of UL-DCCH-Message PDU
@returns Size of encoded bit stream in bytes*/
uint8_t do_RRCConnectionSetupComplete(uint8_t *buffer);

/** \brief Generate an RRCConnectionReconfigurationComplete UL-DCCH-Message (UE)
@param buffer Pointer to PER-encoded ASN.1 description of UL-DCCH-Message PDU
@returns Size of encoded bit stream in bytes*/
uint8_t do_RRCConnectionReconfigurationComplete(uint8_t *buffer);

/** 
\brief Generate an RRCConnectionSetup DL-CCCH-Message (eNB).  This routine configures SRB_ToAddMod (SRB1/SRB2) and 
PhysicalConfigDedicated IEs.  The latter does not enable periodic CQI reporting (PUCCH format 2/2a/2b) or SRS.
@param buffer Pointer to PER-encoded ASN.1 description of DL-CCCH-Message PDU
@param transmission_mode Transmission mode for UE (1-9)
@param UE_id UE index for this message
@param Transaction_id Transaction_ID for this message
@param SRB_configList Pointer (returned) to SRB1_config/SRB2_config(later) IEs for this UE
@param physicalConfigDedicated Pointer (returned) to PhysicalConfigDedicated IE for this UE
@returns Size of encoded bit stream in bytes*/
uint8_t do_RRCConnectionSetup(uint8_t *buffer,
			      uint8_t transmission_mode,
			      uint8_t UE_id,
			      uint8_t Transaction_id,
			      LTE_DL_FRAME_PARMS *frame_parms,
			      SRB_ToAddModList_t **SRB1_configList,
			      struct PhysicalConfigDedicated  **physicalConfigDedicated);

/** 
\brief Generate an RRCConnectionReconfiguration DL-DCCH-Message (eNB).  This routine configures SRBToAddMod (SRB2) and one DRBToAddMod 
(DRB3).  PhysicalConfigDedicated is not updated.
@param Mod_id Module ID of this eNB Instance
@param buffer Pointer to PER-encoded ASN.1 description of DL-CCCH-Message PDU
@param UE_id UE index for this message
@param Transaction_id Transaction_ID for this message
@param SRB_list Pointer to SRB List to be added/modified (NULL if no additions/modifications)
@param DRB_list Pointer to DRB List to be added/modified (NULL if no additions/modifications)
@param DRB_list2 Pointer to DRB List to be released      (NULL if none to be released)
@param sps_Config Pointer to sps_Config to be modified (NULL if no modifications, or default if initial configuration)
@param physicalConfigDedicated Pointer to PhysicalConfigDedicated to be modified (NULL if no modifications)
@param MeasObj_list Pointer to MeasObj List to be added/modified (NULL if no additions/modifications)
@param ReportConfig_list Pointer to ReportConfig List (NULL if no additions/modifications)
@param QuantityConfig Pointer to QuantityConfig to be modified (NULL if no modifications)
@param MeasId_list Pointer to MeasID List (NULL if no additions/modifications)
@param mac_MainConfig Pointer to Mac_MainConfig(NULL if no modifications)
@param measGapConfig Pointer to MeasGapConfig (NULL if no modifications)
@param cba_rnti RNTI for the cba transmission 
@returns Size of encoded bit stream in bytes*/

uint8_t do_RRCConnectionReconfiguration(uint8_t                           Mod_id,
                                        uint8_t                          *buffer,
                                        uint8_t                           UE_id,
                                        uint8_t                           Transaction_id,
                                        SRB_ToAddModList_t                *SRB_list,
                                        DRB_ToAddModList_t                *DRB_list,
                                        DRB_ToReleaseList_t               *DRB_list2,
                                        struct SPS_Config                 *sps_Config,
                                        struct PhysicalConfigDedicated    *physicalConfigDedicated,
                                        MeasObjectToAddModList_t          *MeasObj_list,
                                        ReportConfigToAddModList_t        *ReportConfig_list,
                                        QuantityConfig_t                  *QuantityConfig,
                                        MeasIdToAddModList_t              *MeasId_list,
                                        MAC_MainConfig_t                  *mac_MainConfig,
                                        MeasGapConfig_t                   *measGapConfig,
                                        C_RNTI_t                          *cba_rnti, 
					uint8_t                           *nas_pdu,
                                        uint32_t                           nas_length);

/***
 * \brief Generate an MCCH-Message (eNB). This routine configures MBSFNAreaConfiguration (PMCH-InfoList and Subframe Allocation for MBMS data)
 * @param buffer Pointer to PER-encoded ASN.1 description of MCCH-Message PDU
 * @returns Size of encoded bit stream in bytes
*/
uint8_t do_MCCHMessage(uint8_t *buffer);
#ifdef Rel10
/***
 * \brief Generate an MCCH-Message (eNB). This routine configures MBSFNAreaConfiguration (PMCH-InfoList and Subframe Allocation for MBMS data)
 * @param buffer Pointer to PER-encoded ASN.1 description of MCCH-Message PDU
 * @returns Size of encoded bit stream in bytes
*/
uint8_t do_MBSFNAreaConfig(LTE_DL_FRAME_PARMS *frame_parms,
			   uint8_t sync_area,
			   uint8_t *buffer,
			   MCCH_Message_t *mcch_message,
			   MBSFNAreaConfiguration_r9_t **mbsfnAreaConfiguration);
#endif 

uint8_t do_MeasurementReport(uint8_t *buffer,int measid,int phy_id,int rsrp_s,int rsrq_s,long rsrp_t,long rsrq_t);

OAI_UECapability_t *fill_ue_capability();

uint8_t do_UECapabilityEnquiry(uint8_t Mod_id,
			       uint8_t *buffer,
			       uint8_t UE_id,
			       uint8_t Transaction_id);

uint8_t do_SecurityModeCommand(uint8_t Mod_id,
                               uint8_t *buffer,
                               uint8_t UE_id,
                               uint8_t Transaction_id,
                               uint8_t cipheringAlgorithm,
                               uint8_t integrityProtAlgorithm);
