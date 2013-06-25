/***************************************************************************
                          rrc_bch_mib.h  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define structures for Broadcast MIB and SIBs
 ***************************************************************************/
#ifndef __RRC_BCH_MIB_H__
#define __RRC_BCH_MIB_H__

#include "rrc_bch_ies.h"
#include "rrc_bch_constant.h"

/*********************************************************************
 System Information for BCH
*********************************************************************/
typedef struct FirstSegment {
  SIB_Type        sib_Type;
  SegCount        seg_Count;
  SIB_Data_fixed  sib_Data_fixed;
} FirstSegment;

typedef struct SubsequentSegment {
  SIB_Type        sib_Type;
  SegmentIndex    segmentIndex;
  SIB_Data_fixed  sib_Data_fixed;
} SubsequentSegment;

typedef struct LastSegmentShort {
  SIB_Type        sib_Type;
  SegmentIndex    segmentIndex;
  SIB_Data_variable sib_Data_variable;
} LastSegmentShort;

typedef struct CompleteSIBshort {
  SIB_Type        sib_Type;
  SIB_Data_variable sib_Data_variable;
} CompleteSIBshort;

typedef struct CompleteSIB_List {
  unsigned int    numSIB;
  CompleteSIBshort elem[maxSIBperMsg];
} CompleteSIB_List;

typedef struct CompleteSIB {
  SIB_Type        sib_Type;
  CompleteSIB_sib_Data_fixed sib_Data_fixed;
} CompleteSIB;

typedef struct LastSegment {
  SIB_Type        sib_Type;
  SegmentIndex    segmentIndex;
  SIB_Data_fixed  sib_Data_fixed;
} LastSegment;

typedef struct SystemInformation_BCH_payload {
  int             type;
  union {
    /* type = 1 */
    // noSegment
    /* type = 2 */
    FirstSegment   *firstSegment;
    /* type = 3 */
    SubsequentSegment *subsequentSegment;
    /* type = 4 */
    LastSegmentShort *lastSegmentShort;
    /* type = 8 */
    CompleteSIB_List *completeSIB_List;
    /* type = 10 */
    CompleteSIB    *completeSIB;
    /* type = 11 */
    LastSegment    *lastSegment;
  } segment;
} SystemInformation_BCH_payload;

typedef struct SystemInformation_BCH {
  SFN_Prime       sfn_Prime;
  SystemInformation_BCH_payload payload;
} SystemInformation_BCH;

/**************************************************************/
/*  MasterInformationBlock                                    */
/**************************************************************/

typedef struct MasterInformationBlock {
  MIB_ValueTag    mib_ValueTag;
  RCELL_PLMN_Identity   plmn_Identity;
  SIBSb_ReferenceList sibSb_ReferenceList;
} MasterInformationBlock;

/**************************************************************/
/*  SysInfoType1                                              */
/**************************************************************/
typedef struct SysInfoType1 {
  NAS_SystemInformation subnet_NAS_SysInfo;
  RCELL_UE_TimersAndConstants ue_TimersAndConstants;
} SysInfoType1;

/**************************************************************/
/*  SysInfoType2                                              */
/**************************************************************/
typedef struct SysInfoType2 {
  IP_Address      net_IP_addr;
} SysInfoType2;

/**************************************************************/
/*  SysInfoType5                                              */
/**************************************************************/
typedef struct SysInfoType5 {
  PRACH_SCCPCH_SIList prach_sCCPCH_SIList;
  OpenLoopPowerControl_TDD openLoopPowerControl_TDD;
} SysInfoType5;

/**************************************************************/
/*  SysInfoType11                                             */
/**************************************************************/
//typedef struct SysInfoType11 {
//   FACH_MeasurementOccasionInfo  fach_MeasurementOccasionInfo;
//   MeasurementControlSysInfo  measurementControlSysInfo;
//} SysInfoType11;

/**************************************************************/
/*  SysInfoType14                                             */
/**************************************************************/
typedef struct SysInfoType14 {
  IndividualTS_InterferenceList individualTS_InterferenceList;
  ExpirationTimeFactor expirationTimeFactor;
} SysInfoType14;

/**************************************************************/
/*  SysInfoType18                                             */
/**************************************************************/
typedef struct SysInfoType18 {
  IdentitiesOfNeighbourCells cellIdentities;
  CodeGroupsOfNeighbourCells cellCodegroups;
} SysInfoType18;

#endif
