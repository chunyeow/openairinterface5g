/***************************************************************************
                          rrc_msg_constant.h  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define structures for RRC peer-to-peer messages
 ***************************************************************************/
#ifndef __RRC_MSG_CONSTANT_H__
#define __RRC_MSG_CONSTANT_H__

/* Size of message Header = integrity + type */
#define MSG_HEAD_LGTH 8
#define MSG_PAGING_LGTH 1023
#define MSG_NASDATA_LGTH 4095
#define MSG_UECONFIG_LGTH 2048

/* Message types for classes - from 25.331 ASN1 definitions */
/* DL DCCH Message Type constants */
#define DL_DCCH_cellUpdateConfirm             4
#define DL_DCCH_downlinkDirectTransfer        6
#define DL_DCCH_measurementControl            9
#define DL_DCCH_pagingType2                  10
#define DL_DCCH_radioBearerRelease           14
#define DL_DCCH_radioBearerSetup             15
#define DL_DCCH_rrcConnectionRelease         16
#define DL_DCCH_ueCapabilityInformationConfirm 22
#define DL_DCCH_uplinkPhysicalChannelControl 23
//TEMP Valeur a verifier vs ASN1
#define DL_DCCH_keepAlive                    25
#define DL_DCCH_mbmsModifiedServicesInformation 27

/* UL DCCH Message Type constants */
#define UL_DCCH_initialDirectTransfer         6
#define UL_DCCH_measurementReport             9
#define UL_DCCH_radioBearerReleaseComplete   14
#define UL_DCCH_radioBearerReleaseFailure    15
#define UL_DCCH_radioBearerSetupComplete     16
#define UL_DCCH_radioBearerSetupFailure      17
#define UL_DCCH_rrcConnectionReleaseComplete 18
#define UL_DCCH_rrcConnectionSetupComplete   19
#define UL_DCCH_ueCapabilityInformation      27
#define UL_DCCH_uplinkDirectTransfer         28
#define UL_DCCH_rrcConnectionReleaseUL       31  // was spare value

/* DL CCCH Message Type constants */
#define DL_CCCH_cellUpdateConfirm    1
#define DL_CCCH_rrcConnectionReject  2
#define DL_CCCH_rrcConnectionRelease 3
#define DL_CCCH_rrcConnectionSetup   4

/* UL CCCH Message Type constants */
#define UL_CCCH_cellUpdate           1
#define UL_CCCH_rrcConnectionRequest 2

/* MCCH Message Type constants */
#define MCCH_mbmsAccessInformation            1
#define MCCH_mbmsCommonPTMRBInformation       2
#define MCCH_mbmsCurrentCellPTMRBInformation  3
#define MCCH_mbmsGeneralInformation           4
#define MCCH_mbmsModifiedServicesInformation  5
#define MCCH_mbmsNeighbouringCellPTMRBInformation 6
#define MCCH_mbmsUnmodifiedServicesInformation 7

/* MSCH Message Type constants */
#define MSCH_mbmsSchedulingInformation 1

/********************************/

/* Max number of transactions */
#define MAXTRANS 3
#define MAXURAB  27  //must be identical to MAX_RABS in rrc_nas_primitives.h
//#define MAXRBSETUP maxRABsetup  For further step
#define MAXRBSETUP 4

/*  FailureCauseWithProtErr  */
#define FCWPE_configurationUnsupported 1
#define FCWPE_physicalChannelFailure 2
#define FCWPE_incompatibleSimultaneousReconfiguration 3
#define FCWPE_compressedModeRuntimeError 4
#define FCWPE_protocolError 5
#define FCWPE_cellUpdateOccurred 6
#define FCWPE_invalidConfiguration 7
#define FCWPE_configurationIncomplete 8
#define FCWPE_unsupportedMeasurement 9


/* Measurement parameters */
#define MAXMEASTYPES 4
// indexes of measurement types for UE
#define IX_ifM  0
#define IX_tvM  1
#define IX_qM   2
#define IX_iueM 3

// indexes of measurement types for BS
#define IX_ibsM  0
#define IX_tvbM  1
#define IX_qbM   2


// max number of measured objects
#define MAXMEASCELLS  2
#define MAXMEASRBS    8
#define MAXMEASTRCH   16
#define MAXMEASTFCS   8
#define MAXCH	        16   //cf L1
#define NUMSPARE      1    // number of additional measures in BS

/*  MeasurementCommand_r4  */
#define MC_setup   1
#define MC_modify  2
#define MC_release 3

/*  MeasurementType  */
#define MT_intraFrequencyMeasurement 1
#define MT_interFrequencyMeasurement 2
#define MT_trafficVolumeMeasurement  5
#define MT_qualityMeasurement        6
#define MT_ue_InternalMeasurement    7

/*  ReportCriteria  */
#define RC_ThresholdReportingCriteria  1
#define RC_PeriodicalReportingCriteria 2
#define RC_NoReporting                 3

/*  UL_TrCH_Identity   */
#define  UL_TrCH_Id_dch   1
#define  UL_TrCH_Id_rach  2
#define  UL_TrCH_Id_usch  3

/*  TrafficVolumeMeasQuantity  */
#define TV_rlc_BufferPayload           1
#define TV_averageRLC_BufferPayload    2
#define TV_varianceOfRLC_BufferPayload 3

/*  MeasuredResults  */
#define MR_intraFreqMeasuredResultsList 1
#define MR_interFreqMeasuredResultsList 2
#define MR_trafficVolumeMeasuredResultsList 4
#define MR_qualityMeasuredResults 5
#define MR_ue_InternalMeasuredResults 6

/* Access Stratum Release Indicator  */
#define ACCESS_STRATUM_RELEASE_INDICATOR_REL_4 0
#define ACCESS_STRATUM_RELEASE_INDICATOR_REL_5 1
#define ACCESS_STRATUM_RELEASE_INDICATOR_REL_6 2

/* Eurecom Kernel Release Indicator */
#define EURECOM_KERNEL_RELEASE_INDICATOR_REL_24 24
#define EURECOM_KERNEL_RELEASE_INDICATOR_REL_26 26

/* Present ie of the message UECapabilityInformationConfirm */
#define	UE_CAPABILITY_INFORMATION_CONFIRM_PR_NOTHING 0
#define	UE_CAPABILITY_INFORMATION_CONFIRM_PR_R3 1
#define UE_CAPABILITY_INFORMATION_CONFIRM_PR_LATER_THAN_R3 2

#endif
