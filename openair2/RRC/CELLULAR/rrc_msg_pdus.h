/***************************************************************************
                          rrc_msg_pdus.h  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define structures for RRC peer-to-peer messages
 ***************************************************************************/
#ifndef __RRC_MSG_PDUS_H__
#define __RRC_MSG_PDUS_H__

#include "rrc_msg_ies.h"

/**************************************************************/
/*  RRCConnectionRequest                                      */
/**************************************************************/
typedef struct RRCConnectionRequest {
  IMEI            imei;
  EstablishmentCause establishmentCause;
  ProtocolErrorIndicator protocolErrorIndicator;
  //MeasuredResultsOnRACH  measuredResultsOnRACH;
} RRCConnectionRequest;

/**************************************************************/
/*  RRCConnectionSetup                                        */
/**************************************************************/
typedef struct RRCConnectionSetup {
// InitialUE_Identity  initialUE_Identity;
  IMEI            imei;
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  ActivationTime  activationTime;
  U_RNTI          new_U_RNTI;
// C_RNTI  new_c_RNTI;
  RRC_StateIndicator rrc_StateIndicator;
// UTRAN_DRX_CycleLengthCoefficient  utran_DRX_CycleLengthCoeff;
// CapabilityUpdateRequirement_r4  capabilityUpdateRequirement;
// SRB_InformationSetupList2  srb_InformationSetupList;
// UL_CommonTransChInfo  ul_CommonTransChInfo;
// UL_AddReconfTransChInfoList  ul_AddReconfTransChInfoList;
// DL_CommonTransChInfo_r4  dl_CommonTransChInfo;
// DL_AddReconfTransChInfoList  dl_AddReconfTransChInfoList;
// FrequencyInfo  frequencyInfo;
// MaxAllowedUL_TX_Power  maxAllowedUL_TX_Power;
// UL_ChannelRequirement_r4  ul_ChannelRequirement;
// DL_CommonInformation_r4  dl_CommonInformation;
// DL_InformationPerRL_List_r4  dl_InformationPerRL_List;
//   QOS_Classes  qos_Classes;
//   IP_DSCP_Codes  ip_dscp_Codes;
  UE_Configuration ue_Configuration;
} RRCConnectionSetup;

/**************************************************************/
/*  RRCConnectionSetupComplete                                */
/**************************************************************/
typedef struct RRCConnectionSetupComplete {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  UE_RadioAccessCapability ue_RadioAccessCapability;
} RRCConnectionSetupComplete;

/**************************************************************/
/*  RRCConnectionReject                                       */
/**************************************************************/
typedef struct RRCConnectionReject {
//   InitialUE_Identity  initialUE_Identity;
  IMEI            imei;
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  WaitTime        waitTime;
} RRCConnectionReject;

/**************************************************************/
/*  RRCConnectionReleaseMD                                      */
/**************************************************************/
typedef struct RRCConnectionRelease_UL {
  U_RNTI          u_RNTI;
  ReleaseCause    releaseCause;
} RRCConnectionRelease_UL;

/**************************************************************/
/*  InitialDirectTransfer                                     */
/**************************************************************/
typedef struct InitialDirectTransfer {
  NAS_Message     nas_Message;
  //MeasuredResultsOnRACH  measuredResultsOnRACH;
} InitialDirectTransfer;

/**************************************************************/
/*  UplinkDirectTransfer                                      */
/**************************************************************/
typedef struct UplinkDirectTransfer {
  NAS_Message     nas_Message;
  //MeasuredResultsOnRACH  measuredResultsOnRACH;
} UplinkDirectTransfer;

/**************************************************************/
/*  DownlinkDirectTransfer                                    */
/**************************************************************/
typedef struct DownlinkDirectTransfer {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  NAS_Message     nas_Message;
} DownlinkDirectTransfer;

/**************************************************************/
/*  RadioBearerSetup                                          */
/**************************************************************/
typedef struct RadioBearerSetup {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  //IntegrityProtectionModeInfo  integrityProtectionModeInfo;
  //CipheringModeInfo  cipheringModeInfo;
  ActivationTime  activationTime;
  U_RNTI          new_U_RNTI;
//   C_RNTI  new_C_RNTI;
  RRC_StateIndicator rrc_StateIndicator;
//   CN_InformationInfo  cn_InformationInfo;
//   SRB_InformationSetupList  srb_InformationSetupList;
  RAB_InformationSetupList rab_InformationSetupList;
//   RB_InformationAffectedList  rb_InformationAffectedList;
//   DL_CounterSynchronisationInfo  dl_CounterSynchronisationInfo;
//   UL_CommonTransChInfo  ul_CommonTransChInfo;
//   UL_DeletedTransChInfoList  ul_deletedTransChInfoList;
//   UL_AddReconfTransChInfoList  ul_AddReconfTransChInfoList;
//   DL_CommonTransChInfo  dl_CommonTransChInfo;
//   DL_DeletedTransChInfoList  dl_DeletedTransChInfoList;
//   DL_AddReconfTransChInfoList  dl_AddReconfTransChInfoList;
//   FrequencyInfo  frequencyInfo;
//   MaxAllowedUL_TX_Power  maxAllowedUL_TX_Power;
//   UL_ChannelRequirement  ul_ChannelRequirement;
//   DL_CommonInformation  dl_CommonInformation;
//   DL_InformationPerRL_List  dl_InformationPerRL_List;
//   QOS_Classes  qos_Classes;
//   IP_DSCP_Codes  ip_dscp_Codes;
  UE_Configuration ue_Configuration;
} RadioBearerSetup;

/**************************************************************/
/*  RadioBearerRelease                                        */
/**************************************************************/
typedef struct RadioBearerRelease {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  //IntegrityProtectionModeInfo  integrityProtectionModeInfo;
  //CipheringModeInfo  cipheringModeInfo;
  ActivationTime  activationTime;
  U_RNTI          new_U_RNTI;
//   C_RNTI  new_C_RNTI;
  RRC_StateIndicator rrc_StateIndicator;
//   CN_InformationInfo  cn_InformationInfo;
//   CN_DomainIdentity  signallingConnectionRelIndication;
//   RAB_InformationReconfigList  rab_InformationReconfigList;
  RB_InformationReleaseList rb_InformationReleaseList;
//   RB_InformationAffectedList  rb_InformationAffectedList;
//   DL_CounterSynchronisationInfo  dl_CounterSynchronisationInfo;
//   UL_CommonTransChInfo  ul_CommonTransChInfo;
//   UL_DeletedTransChInfoList  ul_deletedTransChInfoList;
//   UL_AddReconfTransChInfoList  ul_AddReconfTransChInfoList;
//   DL_CommonTransChInfo  dl_CommonTransChInfo;
//   DL_DeletedTransChInfoList  dl_DeletedTransChInfoList;
//   DL_AddReconfTransChInfo2List  dl_AddReconfTransChInfoList;
//   FrequencyInfo  frequencyInfo;
//   MaxAllowedUL_TX_Power  maxAllowedUL_TX_Power;
//   UL_ChannelRequirement  ul_ChannelRequirement;
//   DL_CommonInformation  dl_CommonInformation;
//   DL_InformationPerRL_List  dl_InformationPerRL_List;
  UE_Configuration ue_Configuration;
} RadioBearerRelease;

/**************************************************************/
/*  RadioBearerSetupComplete                                  */
/**************************************************************/
typedef struct RadioBearerSetupComplete {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  //IntegrityProtActivationInfo  ul_IntegProtActivationInfo;
  //UL_TimingAdvance  ul_TimingAdvance;
  //RB_ActivationTimeInfoList  rb_UL_CiphActivationTimeInfo;
  //UL_CounterSynchronisationInfo  ul_CounterSynchronisationInfo;
} RadioBearerSetupComplete;

/**************************************************************/
/*  RadioBearerSetupFailure                                   */
/**************************************************************/
typedef struct RadioBearerSetupFailure {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  FailureCauseWithProtErr failureCause;
  //RB_IdentityList  potentiallySuccesfulBearerList;
} RadioBearerSetupFailure;

/**************************************************************/
/*  RadioBearerReleaseComplete                                */
/**************************************************************/
typedef struct RadioBearerReleaseComplete {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  //IntegrityProtActivationInfo  ul_IntegProtActivationInfo;
  //UL_TimingAdvance  ul_TimingAdvance;
  //RB_ActivationTimeInfoList  rb_UL_CiphActivationTimeInfo;
  //UL_CounterSynchronisationInfo  ul_CounterSynchronisationInfo;
} RadioBearerReleaseComplete;

/**************************************************************/
/*  RadioBearerReleaseFailure                                 */
/**************************************************************/
typedef struct RadioBearerReleaseFailure {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  FailureCauseWithProtErr failureCause;
  //RB_IdentityList  potentiallySuccesfulBearerList;
} RadioBearerReleaseFailure;

/**************************************************************/
/*  CellUpdate                                                */
/**************************************************************/
typedef struct CellUpdate {
  U_RNTI          u_RNTI;
  //STARTList  startList;
  u8              am_RLC_ErrorIndicationRb2_3or4;
  u8              am_RLC_ErrorIndicationRb5orAbove;
  CellUpdateCause cellUpdateCause;
  FailureCauseWithProtErrTrId failureCause;
  //Rb_timer_indicator  rb_timer_indicator;
  //MeasuredResultsOnRACH  measuredResultsOnRACH;
} CellUpdate;

/**************************************************************/
/*  CellUpdateConfirm_CCCH                                    */
/**************************************************************/
typedef struct CellUpdateConfirm_CCCH {
  U_RNTI          u_RNTI;
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  //IntegrityProtectionModeInfo  integrityProtectionModeInfo;
  //CipheringModeInfo  cipheringModeInfo;
  ActivationTime  activationTime;
  U_RNTI          new_U_RNTI;
//   C_RNTI  new_C_RNTI;
  RRC_StateIndicator rrc_StateIndicator;
  u8              rlc_Re_establishIndicatorRb2_3or4;
  u8              rlc_Re_establishIndicatorRb5orAbove;
//   CN_InformationInfo  cn_InformationInfo;
//   RB_InformationReleaseList  rb_InformationReleaseList;
//   RB_InformationReconfigList  rb_InformationReconfigList;
//   RB_InformationAffectedList  rb_InformationAffectedList;
//   DL_CounterSynchronisationInfo  dl_CounterSynchronisationInfo;
//   UL_CommonTransChInfo  ul_CommonTransChInfo;
//   UL_DeletedTransChInfoList  ul_deletedTransChInfoList;
//   UL_AddReconfTransChInfoList  ul_AddReconfTransChInfoList;
//   DL_CommonTransChInfo  dl_CommonTransChInfo;
//   DL_DeletedTransChInfoList  dl_DeletedTransChInfoList;
//   DL_AddReconfTransChInfoList  dl_AddReconfTransChInfoList;
//   FrequencyInfo  frequencyInfo;
//   MaxAllowedUL_TX_Power  maxAllowedUL_TX_Power;
//   UL_ChannelRequirement  ul_ChannelRequirement;
//   DL_CommonInformation  dl_CommonInformation;
//   DL_InformationPerRL_List  dl_InformationPerRL_List;
  UE_Configuration ue_Configuration;
} CellUpdateConfirm_CCCH;

/**************************************************************/
/*  PagingType2                                               */
/**************************************************************/
typedef struct PagingType2 {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  //PagingCause  pagingCause;
  IMEI            imei;
//   NAS_Message  nas_Message;  EXAMPLE
  PAGING_Message  paging_Message;
} PagingType2;

/**************************************************************/
/*  MeasurementControl                                        */
/**************************************************************/
typedef struct MeasurementControl {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  MeasurementIdentity measurementIdentity;
  MeasurementCommand_r4 measurementCommand;
  MeasurementReportingMode measurementReportingMode;
  //AdditionalMeasurementID_List  additionalMeasurementList; FFS
} MeasurementControl;

/**************************************************************/
/*  MeasurementReport                                         */
/**************************************************************/
typedef struct MeasurementReport {
  MeasurementIdentity measurementIdentity;
  MeasuredResults measuredResults;
//   MeasuredResultsOnRACH  measuredResultsOnRACH;  FFS
//   MeasuredResultsList  additionalMeasuredResults; FFS
//   EventResults  eventResults; FFS
} MeasurementReport;

// /**************************************************************/
// /*  ULPCHControl + KeepAlive                                  */
// /**************************************************************/
// typedef struct ULPCHControl {
//   RRC_TransactionIdentifier rrc_TransactionIdentifier;
//   UplinkPhysicalChannelControl_IE uplinkPhysicalChannelControl_IE;
// } ULPCHControl;
// 
// typedef struct KeepAlive {
//   int             dummy;
// } KeepAlive;

/**************************************************************/
/* UECapabilityInformation                                    */
/**************************************************************/
typedef struct UECapabilityInformation {
  RRC_TransactionIdentifier      rrc_TransactionIdentifier;
  AccessStratumReleaseIndicator  accessStratumReleaseIndicator;
//An adaptation for Eurecom TD-CDMA
  EurecomKernelReleaseIndicator  eurecomKernelReleaseIndicator;
} UECapabilityInformation;

/**************************************************************/
/* UECapabilityInformationConfirm                             */
/**************************************************************/
typedef struct UECapabilityInformationConfirm {
  u8  present;
  RRC_TransactionIdentifier  rrc_TransactionIdentifier;
} UECapabilityInformationConfirm;

#endif
