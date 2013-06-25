/***************************************************************************
                      rrc_msg_ies.h  -  description
                         -------------------
begin                : Jan 6, 2003
copyright            : (C) 2003, 2010 by Eurecom
email                : Michelle.Wetterwald@eurecom.fr
**************************************************************************
Define structures for RRC peer-to-peer messages
***************************************************************************/
#ifndef __RRC_MSG_IES_H__
#define __RRC_MSG_IES_H__

#include "rrc_platform_types.h"
#include "rrc_msg_constant.h"
#include "rrc_bch_per_basic.h"

/**************************************************************/
/*  For all Messages                                          */
/**************************************************************/
/*  IntegrityCheckInfo  */
typedef u32     IntegrityCheckInfo;

/**************************************************************/
/*  RRCConnectionRequest                                      */
/**************************************************************/
   /*  IMEI_Digit   */
typedef u8      IMEI_Digit;

   /*  IMEI         */
typedef struct IMEI {
  u8              numDigits;
  IMEI_Digit      digit[15];
} IMEI;

   /*  EstablishmentCause  -  Full 25.331 list */
typedef enum {
  originatingConversationalCall = 0,
  originatingStreamingCall = 1,
  originatingInteractiveCall = 2,
  originatingBackgroundCall = 3,
  originatingSubscribedTrafficCall = 4,
  terminatingConversationalCall = 5,
  terminatingStreamingCall = 6,
  terminatingInteractiveCall = 7,
  terminatingBackgroundCall = 8,
  emergencyCall = 9,
  interRAT_CellReselection = 10,
  interRAT_CellChangeOrder = 11,
  registration = 12,
  detach = 13,
  originatingHighPrioritySignalling = 14,
  originatingLowPrioritySignalling = 15,
  callRe_establishment = 16,
  terminatingHighPrioritySignalling = 17,
  terminatingLowPrioritySignalling = 18,
  terminatingCauseUnknown = 19,
} EstablishmentCause;

   /*  ProtocolErrorIndicator */
typedef enum {
  noError = 0,
  errorOccurred = 1
} ProtocolErrorIndicator;

/**************************************************************/
/*  RRCConnectionSetup                                        */
/**************************************************************/
/*  IMEI -- cf  RRCConnectionRequest       */

/*  RRC_TransactionIdentifier   */
// value >= 0 && value <= 3
typedef u8      RRC_TransactionIdentifier;

/*  ActivationTime    */
// value >= 0 && value <= 255
typedef u16     ActivationTime;

/*  U_RNTI            */
//typedef struct U_RNTI {
//   SRNC_Identity  srnc_Identity;
//   S_RNTI  s_RNTI;
//} U_RNTI;
typedef u16     U_RNTI;

/*  RRC_StateIndicator */
typedef enum {
  cell_DCH = 0,
  cell_FACH = 1,
  cell_PCH = 2,
  ura_PCH = 3
} RRC_StateIndicator;

/*  NAS QOS Info - Moby Dick */
//typedef u8  IP_DSCP_Codes[MAXURAB];
//typedef u8  QOS_Classes[MAXURAB];

/*  UE_Configuration   */
typedef struct UE_Configuration {
  u16             numoctets;
  u8              data[MSG_UECONFIG_LGTH];
} UE_Configuration;

/**************************************************************/
/*  RRCConnectionSetupComplete                                */
/**************************************************************/
// RRC_TransactionIdentifier

/*  UE_RadioAccessCapability  */
typedef enum {
  rel_4 = 0,
  eurecom_rrc = 7               // values != 0 are spare values
} UE_RadioAccessCapability;

/**************************************************************/
/*  RRCConnectionReject                                       */
/**************************************************************/
// IMEI
// RRC_TransactionIdentifier

/*  WaitTime   */
// value >= 0 && value <= 15
typedef u8      WaitTime;

/**************************************************************/
/*  RRCConnectionRelease                                      */
/**************************************************************/
// U_RNTI

/*  ReleaseCause  */
typedef enum {
  normalEvent = 0,
  unspecified = 1,
  pre_emptiveRelease = 2,
  congestion = 3,
  re_establishmentReject = 4,
  directedsignallingconnectionre_establishment = 5,
  userInactivity = 6,
} ReleaseCause;

/**************************************************************/
/*  InitialDirectTransfer                                     */
/**************************************************************/
/*  NAS_Message  */
typedef struct NAS_Message {
  u16             numoctets;
  u8              data[MSG_NASDATA_LGTH];
} NAS_Message;

/**************************************************************/
/*  RadioBearerSetup                                          */
/**************************************************************/
/*  RB_Identity        */
// value >= 1 && value <= 32
typedef u16     RB_Identity;

/*  NAS QOS Info - Moby Dick */
typedef u16     QOS_Class;
typedef u16     IP_DSCP_Code;
typedef u16     SAP_Id;

/*  RB_InformationSetup  */
typedef struct RB_InformationSetup {
  RB_Identity     rb_identity;
//   PDCP_Info  pdcp_Info;
//   RLC_InfoChoice  rlc_InfoChoice;
//   RB_MappingInfo  rb_MappingInfo;
  QOS_Class       qos_class;
  IP_DSCP_Code    ip_dscp_code;
  SAP_Id          sap_id;
} RB_InformationSetup;

/*  RAB_InformationSetupList  */
typedef struct RAB_InformationSetupList {
  u16             numrabs;
  RB_InformationSetup rbinfo[MAXRBSETUP];
} RAB_InformationSetupList;

/**************************************************************/
/*  RadioBearerRelease                                        */
/**************************************************************/
/*  RB_InformationReleaseList  */
typedef struct RB_InformationReleaseList {
  u16             numrabs;
  RB_Identity     rbid[MAXRBSETUP];
} RB_InformationReleaseList;

/**************************************************************/
/*  RadioBearerSetupFailure                                   */
/**************************************************************/
// RRC_TransactionIdentifier  rrc_TransactionIdentifier;
/*  ProtocolErrorCause    */
typedef enum {
  asn1_ViolationOrEncodingError = 0,
  messageTypeNonexistent = 1,
  messageNotCompatibleWithReceiverState = 2,
  ie_ValueNotComprehended = 3,
  informationElementMissing = 4,
  messageExtensionNotComprehended = 5,
} ProtocolErrorCause;

/*  FailureCauseWithProtErr  */
typedef struct FailureCauseWithProtErr {
  u8              type;
  ProtocolErrorCause protocolError;
} FailureCauseWithProtErr;

/**************************************************************/
/*  CellUpdate                                                */
/**************************************************************/
// U_RNTI

/*  CellUpdateCause  */
typedef enum {
  cellReselection = 0,
  periodicalCellUpdate = 1,
  uplinkDataTransmission = 2,
  utran_pagingResponse = 3,
  re_enteredServiceArea = 4,
  radiolinkFailure = 5,
  rlc_unrecoverableError = 6,
} CellUpdateCause;

/*  FailureCauseWithProtErrTrId   */
typedef struct FailureCauseWithProtErrTrId {
  RRC_TransactionIdentifier rrc_TransactionIdentifier;
  FailureCauseWithProtErr failureCause;
} FailureCauseWithProtErrTrId;

/**************************************************************/
/*  PagingType2                                               */
/**************************************************************/
// RRC_TransactionIdentifier
// IMEI

/*  PAGING_Message  */
typedef struct PAGING_Message {
  u16             numoctets;
  u8              data[MSG_PAGING_LGTH];
} PAGING_Message;

/**************************************************************/
/*  MeasurementControl                                        */
/**************************************************************/
//   RRC_TransactionIdentifier

/*  MeasurementIdentity  */
// value >= 1 && value <= 16
typedef u16     MeasurementIdentity;

/*  ReportingIntervalLong  */
typedef enum {
  int250 = 250,
  int500 = 500,
  int1000 = 1000,
  int2000 = 2000,
  int3000 = 3000,
  int4000 = 4000,
  int6000 = 6000,
  int8000 = 8000,
  int12000 = 12000,
  int16000 = 16000,
  int20000 = 20000,
  int24000 = 24000,
  int28000 = 28000,
  int32000 = 32000,
  int64000 = 64000,
} ReportingIntervalLong;
/*  ReportingAmount */
typedef enum {
  amount1 = 1,
  amount2 = 2,
  amount4 = 4,
  amount8 = 8,
  amount16 = 16,
  amount32 = 32,
  amount64 = 64,
  amount_inf = 0
} ReportingAmount;
/*  PeriodicalReportingCriteria  */
typedef struct PeriodicalReportingCriteria {
  ReportingAmount reportingAmount;
  ReportingIntervalLong reportingInterval;
} PeriodicalReportingCriteria;

/*  FilterCoefficient */
typedef enum {
  coeff0 = 0,                   //default value
  coeff1 = 1,
  coeff2 = 2,
  coeff3 = 3,
  coeff4 = 4,
  coeff5 = 5,
  coeff6 = 6,
  coeff7 = 7,
  coeff8 = 8,
  coeff9 = 9,
  coeff11 = 11,
  coeff13 = 13,
  coeff15 = 15,
  coeff17 = 17,
  coeff19 = 19,
} FilterCoefficient;

/**************************************************************/
/*  IntraFreqMeasQuantityList */
typedef enum {
  primaryCCPCH_RSCP = 0,
  pathloss = 1,
  timeslotISCP = 2,
} IntraFreqMeasQuantityList;
/*  IntraFreqMeasQuantity  */
typedef struct IntraFreqMeasQuantity {
  FilterCoefficient filterCoefficient;
//   IntraFreqMeasQuantityList measureList[4]; //4 defined in 25.331 - not used
} IntraFreqMeasQuantity;

/*  CellReportingQuantities */
typedef struct CellReportingQuantities {
  u8              cellIdentity_reportingIndicator;
  u8              timeslotISCP_reportingIndicator;
  u8              primaryCCPCH_RSCP_reportingIndicator;
  u8              pathloss_reportingIndicator;
} CellReportingQuantities;

/*  MV_ue_State  */
typedef enum {
  mv_cell_DCH = 0,
  mv_all_But_Cell_DCH = 1,
  mv_all_States = 2
//} MV_ue_State;
///*  MeasurementValidity */
//typedef struct  MeasurementValidity {
//   MV_ue_State  ue_State;
} MeasurementValidity;

/*  IntraFreqReportingCriteria_r4  */
typedef struct IntraFreqReportingCriteria_r4 {
  int             value;        //FFS
} IntraFreqReportingCriteria_r4;
/*  IntraFreqReportCriteria_r4 */
typedef struct IntraFreqReportCriteria_r4 {
  u16             criteriaType;
  union {
    /* criteriaType = 1 */
    IntraFreqReportingCriteria_r4 intraFreqReportingCriteria;
    /* criteriaType = 2 */
    PeriodicalReportingCriteria periodicalReportingCriteria;
    /* criteriaType = 3 */
    //ReportingCellStatusOpt *noReporting;
  } criteriaDef;
} IntraFreqReportCriteria_r4;

/*  IntraFrequencyMeasurement_r4  */
typedef struct IntraFrequencyMeasurement_r4 {
  IntraFreqMeasQuantity intraFreqMeasQuantity;
  CellReportingQuantities intraFreqReportingQuantity;
  MeasurementValidity measurementValidity;
  IntraFreqReportCriteria_r4 reportCriteria;
} IntraFrequencyMeasurement_r4;

/**************************************************************/
/*  TransportChannelIdentity
    (value >= 1 && value <= 32)  */
typedef u8      TransportChannelIdentity;
/*  UL_TrCH_Identity  */
typedef struct UL_TrCH_Identity {
  u8              ch_type;
  TransportChannelIdentity ch_id;
} UL_TrCH_Identity;
/*  TrafficVolumeMeasurementObjectList  */
typedef struct TrafficVolumeMeasurementObjectList {
  u8              numChannels;
  UL_TrCH_Identity channel[MAXMEASTRCH];        // should be 32
} TrafficVolumeMeasurementObjectList;

/*  TimeInterval
    (value >= 1 && value <= 13)  */
typedef u8      TimeInterval;
/*  TrafficVolumeMeasQuantity   */
typedef struct TrafficVolumeMeasQuantity {
  u8              tf_type;
  union {
    /* tf_type = 1 */
    /* tf_type = 2 */
    TimeInterval    averageRLC_BufferPayload;
    /* tf_type = 3 */
    TimeInterval    varianceOfRLC_BufferPayload;
  } timeinterval;
} TrafficVolumeMeasQuantity;

/*  TrafficVolumeReportingQuantity */
typedef struct TrafficVolumeReportingQuantity {
  u8              rlc_RB_BufferPayload;
  u8              rlc_RB_BufferPayloadAverage;
  u8              rlc_RB_BufferPayloadVariance;
} TrafficVolumeReportingQuantity;

//   MeasurementValidity

/*  TrafficVolumeReportingCriteria  */
typedef struct TrafficVolumeReportingCriteria {
  u16             value;        //Temp
} TrafficVolumeReportingCriteria;
// PeriodicalReportingCriteria
/*  TrafficVolumeReportCriteria  */
typedef struct TrafficVolumeReportCriteria {
  u16             criteriaType;
  union {
    /* criteriaType = 1 */
    TrafficVolumeReportingCriteria trafficVolumeReportingCriteria;
    /* criteriaType = 2 */
    PeriodicalReportingCriteria periodicalReportingCriteria;
    /* criteriaType = 3 */
  } criteriaDef;
} TrafficVolumeReportCriteria;

/*  TrafficVolumeMeasurement  */
typedef struct TrafficVolumeMeasurement {
//   TrafficVolumeMeasurementObjectList  trafficVolumeMeasurementObjectList;  FFS
//   TrafficVolumeMeasQuantity  trafficVolumeMeasQuantity;   Ignored anyway
  TrafficVolumeReportingQuantity trafficVolumeReportingQuantity;
  MeasurementValidity measurementValidity;
  TrafficVolumeReportCriteria reportCriteria;
} TrafficVolumeMeasurement;

/**************************************************************/
/*  BLER_TransChIdList  */
typedef struct BLER_TransChIdList {
  u8              numTrCH;
  TransportChannelIdentity ch_id[MAXMEASTRCH];  //should be 32
} BLER_TransChIdList;

/*  TFCS_IdentityPlain
    (value >= 1 && value <= 8)  */
typedef u8      TFCS_IdentityPlain;
/*  SIR_TFCS  */
typedef TFCS_IdentityPlain SIR_TFCS;
/*  SIR_TFCS_List  */
typedef struct SIR_TFCS_List {
  u8              numTFCS;
  SIR_TFCS        tfcs[MAXMEASTFCS];
} SIR_TFCS_List;

/*  QualityReportingQuantity */
typedef struct QualityReportingQuantity {
  u8              dl_TransChBLER;
//   BLER_TransChIdList  bler_dl_TransChIdList;FFS
  SIR_TFCS_List   sir_TFCS_List;
} QualityReportingQuantity;

/*  QualityReportingCriteria  */
typedef struct QualityReportingCriteria {
  u16             value;        //Temp
} QualityReportingCriteria;
// PeriodicalReportingCriteria
/*  QualityReportCriteria  */
typedef struct QualityReportCriteria {
  u16             criteriaType;
  union {
    /* criteriaType = 1 */
    QualityReportingCriteria qualityReportingCriteria;
    /* criteriaType = 2 */
    PeriodicalReportingCriteria periodicalReportingCriteria;
    /* criteriaType = 3 */
  } criteriaDef;
} QualityReportCriteria;
/*  QualityMeasurement  */
typedef struct QualityMeasurement {
  QualityReportingQuantity qualityReportingQuantity;
  QualityReportCriteria reportCriteria;
} QualityMeasurement;

/**************************************************************/
/*  UE_MeasurementQuantity  */
typedef enum {
  ue_TransmittedPower = 0,
  utra_Carrier_RSSI = 1,
} UE_MeasurementQuantity;

/*  UE_InternalMeasQuantity  */
typedef struct UE_InternalMeasQuantity {
  UE_MeasurementQuantity measurementQuantity;
  FilterCoefficient filterCoefficient;
} UE_InternalMeasQuantity;

/*  UE_InternalReportingQuantity_r4  */
typedef struct UE_InternalReportingQuantity_r4 {
  u8              ue_TransmittedPower;
  u8              appliedTA;    //modeSpecificInfo
} UE_InternalReportingQuantity_r4;

/*  UE_InternalReportingCriteria  */
typedef struct UE_InternalReportingCriteria {
  u16             value;        //Temp
} UE_InternalReportingCriteria;
// PeriodicalReportingCriteria

/*  UE_InternalReportCriteria  */
typedef struct UE_InternalReportCriteria {
  u16             criteriaType;
  union {
    /* criteriaType = 1 */
    UE_InternalReportingCriteria ue_InternalReportingCriteria;
    /* criteriaType = 2 */
    PeriodicalReportingCriteria periodicalReportingCriteria;
    /* criteriaType = 3 */
  } criteriaDef;
} UE_InternalReportCriteria;

/*  UE_InternalMeasurement */
typedef struct UE_InternalMeasurement_r4 {
  UE_InternalMeasQuantity ue_InternalMeasQuantity;
  UE_InternalReportingQuantity_r4 ue_InternalReportingQuantity;
  UE_InternalReportCriteria reportCriteria;
} UE_InternalMeasurement_r4;

//****************************************************************
/*  MeasurementType  */
typedef struct MeasurementType {
  u16             measType;
  union {
    /* measType = 1 */
    IntraFrequencyMeasurement_r4 intraFrequencyMeasurement;
    /* measType = 2 */
    //InterFrequencyMeasurement_r4  interFrequencyMeasurement;
    /* measType = 5 */
    TrafficVolumeMeasurement trafficVolumeMeasurement;
    /* measType = 6 */
    QualityMeasurement qualityMeasurement;
    /* measType = 7 */
    UE_InternalMeasurement_r4 ue_InternalMeasurement;
  } type;
} MeasurementType;

/*  MeasurementCommand_r4  */
typedef struct MeasurementCommand_r4 {
  u16             cmdType;
  union {
    /* cmdType = 1 */
    MeasurementType setup;
    /* cmdType = 2 */
    MeasurementType modify;
    /* cmdType = 3 */
  } command;
} MeasurementCommand_r4;

/*  PeriodicalOrEventTrigger */
typedef enum {
  periodical = 0,
  eventTrigger = 1
} PeriodicalOrEventTrigger;
/*  TransferMode  */
typedef enum {
  acknowledgedModeRLC = 0,
  unacknowledgedModeRLC = 1
} TransferMode;
/*  MeasurementReportingMode */
typedef struct MeasurementReportingMode {
  TransferMode    measurementReportTransferMode;
  PeriodicalOrEventTrigger periodicalOrEventTrigger;
} MeasurementReportingMode;


/**************************************************************/
/*  MeasurementReport                                         */
/**************************************************************/

//   MeasurementIdentity

/**************************************************************/
/*  CellIdentity                                              */
typedef u8      CellIdentity;
/*  SFN_SFN_ObsTimeDifference
    value >= 0 && value <= 16777215  */
typedef int     SFN_SFN_ObsTimeDifference;

/*  CellParametersID
    value >= 0 && value <= 127  */
typedef u8      CellParametersID;
/*  PrimaryCCPCH_RSCP
    value >= 0 && value <= 127  */
typedef u8      PrimaryCCPCH_RSCP;
/*  Pathloss
    value >= 46 && value <= 173 */
typedef u16     Pathloss;
/*  TimeslotISCP
    value >= 0 && value <= 127  */
typedef u8      TimeslotISCP;
/*  TimeslotISCP_List */
typedef struct TimeslotISCP_List {
  u8              numSlots;
  TimeslotISCP    iscp[JRRM_SLOTS_PER_FRAME];
} TimeslotISCP_List;

/*  CellMeasuredResults_modeSpecificInfo_tdd  */
typedef struct CellMeasuredResults_modeSpecificInfo_tdd {
  CellParametersID cellParametersID;
  //TGSN  proposedTGSN;
  PrimaryCCPCH_RSCP primaryCCPCH_RSCP;
  Pathloss        pathloss;
  TimeslotISCP_List timeslotISCP_List;
} CellMeasuredResults_modeSpecificInfo_tdd;

/*  CellMeasuredResults  */
typedef struct CellMeasuredResults {
  CellIdentity    cellIdentity;
  //SFN_SFN_ObsTimeDifference  sfn_SFN_ObsTimeDifference;
  //CellSynchronisationInfo  cellSynchronisationInfo;
  CellMeasuredResults_modeSpecificInfo_tdd modeSpecificInfo;
} CellMeasuredResults;

/*  IntraFreqMeasuredResultsList */
typedef struct IntraFreqMeasuredResultsList {
  u8              numCells;
  CellMeasuredResults cellMeas[MAXMEASCELLS];
} IntraFreqMeasuredResultsList;

/**************************************************************/

/*  RLC_BuffersPayload  */
typedef enum {
  pl0 = 0,
  pl4 = 1,
  pl8 = 2,
  pl16 = 3,
  pl32 = 4,
  pl64 = 5,
  pl128 = 6,
  pl256 = 7,
  pl512 = 8,
  pl1024 = 9,
  pl2k = 10,
  pl4k = 11,
  pl8k = 12,
  pl16k = 13,
  pl32k = 14,
  pl64k = 15,
  pl128k = 16,
  pl256k = 17,
  pl512k = 18,
  pl1024k = 19,
} RLC_BuffersPayload;
/*  AverageRLC_BufferPayload  */
typedef enum {
  pla0 = 0,
  pla4 = 1,
  pla8 = 2,
  pla16 = 3,
  pla32 = 4,
  pla64 = 5,
  pla128 = 6,
  pla256 = 7,
  pla512 = 8,
  pla1024 = 9,
  pla2k = 10,
  pla4k = 11,
  pla8k = 12,
  pla16k = 13,
  pla32k = 14,
  pla64k = 15,
  pla128k = 16,
  pla256k = 17,
  pla512k = 18,
  pla1024k = 19,
} AverageRLC_BufferPayload;
/*  VarianceOfRLC_BufferPayload  */
typedef enum {
  plv0 = 0,
  plv4 = 1,
  plv8 = 2,
  plv16 = 3,
  plv32 = 4,
  plv64 = 5,
  plv128 = 6,
  plv256 = 7,
  plv512 = 8,
  plv1024 = 9,
  plv2k = 10,
  plv4k = 11,
  plv8k = 12,
  plv16k = 13,
} VarianceOfRLC_BufferPayload;
/*  TrafficVolumeMeasuredResults  */
typedef struct TrafficVolumeMeasuredResults {
  RB_Identity     rb_Identity;
  RLC_BuffersPayload rlc_BuffersPayload;
  AverageRLC_BufferPayload averageRLC_BufferPayload;
  VarianceOfRLC_BufferPayload varianceOfRLC_BufferPayload;
} TrafficVolumeMeasuredResults;

/*  TrafficVolumeMeasuredResultsList  */
typedef struct TrafficVolumeMeasuredResultsList {
  u8              numRB;
  TrafficVolumeMeasuredResults RBMeas[MAXMEASRBS];      // should be 32
} TrafficVolumeMeasuredResultsList;

/**************************************************************/

/*  DL_TransportChannelBLER
    (value >= 0 && value <= 63)  */
typedef u8      DL_TransportChannelBLER;
/*  BLER_MeasurementResults  */
typedef struct BLER_MeasurementResults {
  TransportChannelIdentity transportChannelIdentity;
  DL_TransportChannelBLER dl_TransportChannelBLER;
} BLER_MeasurementResults;
/*  BLER_MeasurementResultsList  */
typedef struct BLER_MeasurementResultsList {
  u8              numTrCH;
  BLER_MeasurementResults measTrCH[MAXMEASTRCH];        // should be 32
} BLER_MeasurementResultsList;


//  TFCS_IdentityPlain
/*  SIR
    (value >= 0 && value <= 63)  */
typedef u8      SIR;

/*  SIR_TimeslotList  */
typedef struct SIR_TimeslotList {
  u8              numSIR;
  SIR             sir[JRRM_SLOTS_PER_FRAME];
} SIR_TimeslotList;
/*  SIR_MeasurementResults  */
typedef struct SIR_MeasurementResults {
  TFCS_IdentityPlain tfcs_ID;
  SIR_TimeslotList sir_TimeslotList;
} SIR_MeasurementResults;
/*  SIR_MeasurementList  */
typedef struct SIR_MeasurementList {
  u8              numTFCS;
  SIR_MeasurementResults sirMeas[MAXMEASTFCS];
} SIR_MeasurementList;


/*  QualityMeasuredResults  */
typedef struct QualityMeasuredResults {
  BLER_MeasurementResultsList blerMeasurementResultsList;
  SIR_MeasurementList sir_MeasurementList;
} QualityMeasuredResults;

/**************************************************************/

/*  UE_TransmittedPower
    value >= 0 && value <= 104  */
typedef u8      UE_TransmittedPower;
/*  UE_TransmittedPowerTDD_List  */
typedef struct UE_TransmittedPowerTDD_List {
  u8              numSlots;
  UE_TransmittedPower xmitPower[JRRM_SLOTS_PER_FRAME];
} UE_TransmittedPowerTDD_List;

/*  UL_TimingAdvance
    value >= 0 && value <= 63  */
typedef u8      UL_TimingAdvance;

/*  UE_InternalMeasuredResults_tdd  */
typedef struct UE_InternalMeasuredResults_modeSpecificInfo_tdd {
  UE_TransmittedPowerTDD_List ue_TransmittedPowerTDD_List;
  UL_TimingAdvance appliedTA;
} UE_InternalMeasuredResults_tdd;

/**************************************************************/
/*  MeasuredResults  */
typedef struct MeasuredResults {
  u8              measResult;
  union {
    /* measResult = 1 */
    IntraFreqMeasuredResultsList intraFreqMeasuredResultsList;
    /* measResult = 2 */
    //InterFreqMeasuredResultsList interFreqMeasuredResultsList;
    /* measResult = 4 */
    TrafficVolumeMeasuredResultsList trafficVolumeMeasuredResultsList;
    /* measResult = 5 */
    QualityMeasuredResults qualityMeasuredResults;
    /* measResult = 6 */
    UE_InternalMeasuredResults_tdd ue_InternalMeasuredResults;
  } result;
} MeasuredResults;


// /**************************************************************/
// /*  OpenLoopPowerControl_IPDL_TDD_r4                          */
// /**************************************************************/
// typedef struct OpenLoopPowerControl {
//   s16 alpha __attribute__ ((packed));
//   u16 maxPowerIncrease __attribute__ ((packed));
// } OpenLoopPowerControl;
// 
// /**************************************************************/
// /*  UL_DPCH_PowerControlInfo_tdd                              */
// /**************************************************************/
// typedef struct UplinkPhysicalChannelControl {
//   s16 timingAdvance __attribute__ ((packed));
//   u16 alpha __attribute__ ((packed));
//   s16 prach_ConstantValue __attribute__ ((packed));
//   s16 pusch_ConstantValue __attribute__ ((packed));
//   s16 dpch_ConstantValue __attribute__ ((packed));
//   OpenLoopPowerControl openLoopPowerControl __attribute__ ((packed));
// } UplinkPhysicalChannelControl;
// 
// /**************************************************************/
// /*  UL_DPCH_PowerControlInfo_tdd                              */
// /**************************************************************/
// typedef struct UL_DPCH_PowerControlInfo {
//   s16 ul_TargetSIR __attribute__ ((packed));
//   //   UL_DPCH_PowerControlInfo_tdd_ul_OL_PC_Signalling  ul_OL_PC_Signalling;
// } UL_DPCH_PowerControlInfo;
// /**************************************************************/
// /*  CCTrCH_PowerControlInfo_r4                                */
// /**************************************************************/
// typedef struct CCTrCH_PowerControlInfo {
//   u16 tfcs_Identity __attribute__ ((packed));   // CCTrCH Index
//   UL_DPCH_PowerControlInfo ul_DPCH_PowerControlInfo __attribute__ ((packed));
// } CCTrCH_PowerControlInfo;
// /**************************************************************/
// /*  UplinkPhysicalChannelControl_r4_IEs                       */
// /**************************************************************/
// typedef struct UplinkPhysicalChannelControl_IE {
//   u16 ue_id __attribute__ ((packed));
//   CCTrCH_PowerControlInfo ccTrCH_PowerControlInfo __attribute__ ((packed));     //CCTrCH UL Power Control
//   UplinkPhysicalChannelControl uplinkPhysicalChannelControl __attribute__ ((packed));   //PRACH/PUSCH Parameters
// } UplinkPhysicalChannelControl_IE;

//**************************************************************/
/*  UE Capability Information                                 */
/**************************************************************/
  /* AccessStratumReleaseIndicator */
typedef u8 AccessStratumReleaseIndicator;
  /* EurecomKernelReleaseIndicator */
typedef u8 EurecomKernelReleaseIndicator;


#endif
