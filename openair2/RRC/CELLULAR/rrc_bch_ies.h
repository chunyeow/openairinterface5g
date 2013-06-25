/***************************************************************************
                          rrc_bch_ies.h  -  description
                             -------------------
    begin                : Aug 30, 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define structures for Broadcast MIB and SIBs
 ***************************************************************************/
#ifndef __RRC_BCH_IES_H__
#define __RRC_BCH_IES_H__

#include "rrc_bch_constant.h"

/*********************************************************************
 System Information for BCH - Information Elements
*********************************************************************/
//  SegCount
typedef int     SegCount;

//  SegmentIndex
typedef int     SegmentIndex;

//  SFN_Prime
typedef int     SFN_Prime;

//  SIB_Type
typedef enum {
  masterInformationBlock = 0,
  systemInformationBlockType1 = 1,
  systemInformationBlockType2 = 2,
  systemInformationBlockType3 = 3,
  systemInformationBlockType4 = 4,
  systemInformationBlockType5 = 5,
  systemInformationBlockType6 = 6,
  systemInformationBlockType7 = 7,
  systemInformationBlockType8 = 8,
  systemInformationBlockType9 = 9,
  systemInformationBlockType10 = 10,
  systemInformationBlockType11 = 11,
  systemInformationBlockType12 = 12,
  systemInformationBlockType13 = 13,
  systemInformationBlockType13_1 = 14,
  systemInformationBlockType13_2 = 15,
  systemInformationBlockType13_3 = 16,
  systemInformationBlockType13_4 = 17,
  systemInformationBlockType14 = 18,
  systemInformationBlockType15 = 19,
  systemInformationBlockType15_1 = 20,
  systemInformationBlockType15_2 = 21,
  systemInformationBlockType15_3 = 22,
  systemInformationBlockType16 = 23,
  systemInformationBlockType17 = 24,
  systemInformationBlockType15_4 = 25,
  systemInformationBlockType18 = 26,
  schedulingBlock1 = 27,
  schedulingBlock2 = 28,
  systemInformationBlockType15_5 = 29
} SIB_Type;

//   SIB_Data_fixed
typedef struct SIB_Data_fixed {
  unsigned int    numbits;
  unsigned char   data[LSIBfixed];
} SIB_Data_fixed;

//   SIB_Data_variable
typedef struct SIB_Data_variable {
  unsigned int    numbits;
  unsigned char   data[27];
} SIB_Data_variable;

//   CompleteSIB_sib_Data_fixed
typedef struct CompleteSIB_sib_Data_fixed {
  unsigned int    numbits;
  unsigned char   data[LSIBcompl];
} CompleteSIB_sib_Data_fixed;

/**************************************************************/
/*  MasterInformationBlock   Information elements             */
/**************************************************************/
//   MIB_ValueTag
typedef int     MIB_ValueTag;

/**************************************************************/
//   PLMN_Type
  /*  Digit   */
//typedef int  Digit;  already defined

  /*  MCC   */
typedef struct RCELL_MCC {
  unsigned int    numDigits;
  Digit           elem[3];
} RCELL_MCC;

  /*  MNC  */
//typedef struct MNC {
//   unsigned int numDigits;
//   Digit elem[3];
//} MNC;
typedef unsigned int MNC;

  /*  PLMN_Identity */
typedef struct RCELL_PLMN_Identity {
  RCELL_MCC       mcc;
  MNC             mnc;
} RCELL_PLMN_Identity;

/**************************************************************/
/*  PLMN_ValueTag     */
typedef int     PLMN_ValueTag;

/*  CellValueTag     */
typedef int     CellValueTag;

/*  SIBSb_TypeAndTag */
typedef struct SIBSb_TypeAndTag {
  int             type;
  union {
    /* t = 1 */
    PLMN_ValueTag   sysInfoType1;
    /* t = 2 */
    CellValueTag    sysInfoType2;
    /* t = 5 */
    CellValueTag    sysInfoType5;
    /* t = 11 */
    CellValueTag    sysInfoType11;
    /* t = 28 */
    CellValueTag    sysInfoType18;
  } type_tag;
} SIBSb_TypeAndTag;

/**************************************************************/
/*  SegCount      */
//typedef int  SegCount;  already defined

/*  scheduling_sib_Pos  */
typedef struct Scheduling_sib_Pos {
  unsigned int    sib_Rep;
  unsigned int    sib_Pos;
} Scheduling_sib_Pos;

/*  SibOFF  */
typedef enum {
  so2 = 0,
  so4 = 1,
  so6 = 2,
  so8 = 3,
  so10 = 4,
  so12 = 5,
  so14 = 6,
  so16 = 7,
  so18 = 8,
  so20 = 9,
  so22 = 10,
  so24 = 11,
  so26 = 12,
  so28 = 13,
  so30 = 14,
  so32 = 15
} SibOFF;

/*  SibOFF_List  */
typedef struct SibOFF_List {
  unsigned int    num;
  SibOFF          data[15];
} SibOFF_List;

/*  SchedulingInformation    */
typedef struct SchedulingInformation {
  SegCount        segCount;
  Scheduling_sib_Pos scheduling_sib_rep;
  SibOFF_List     sib_PosOffsetInfo;
} SchedulingInformation;

/**************************************************************/
/*  SchedulingInformationSIBSb  */
typedef struct SchedulingInformationSIBSb {
  SIBSb_TypeAndTag sibSb_Type;
  SchedulingInformation scheduling;
} SchedulingInformationSIBSb;

  /*  SIBSb_ReferenceList  */
typedef struct SIBSb_ReferenceList {
  unsigned int    numSIB;
  SchedulingInformationSIBSb sib_ref[supportedSIBs];
} SIBSb_ReferenceList;

/**************************************************************/
/*  SysInfoType1  Information elements                        */
/**************************************************************/
    /*  NAS_SystemInformation */
typedef struct NAS_SystemInformation {
  unsigned int    numocts;
  unsigned char   data[maxSIB1NAS];
} NAS_SystemInformation;

    /*  UE_TimersAndConstants */
typedef struct RCELL_UE_TimersAndConstants {
  unsigned int    numocts;
  unsigned char   data[8];
} RCELL_UE_TimersAndConstants;

/**************************************************************/
/*  SysInfoType2  Information elements                        */
/**************************************************************/
   /*  IP_Address   */
//typedef struct SRNC_Identity {
typedef struct IP_Address {
  unsigned char   data[16];
} IP_Address;

/**************************************************************/
/*  SysInfoType5  Information elements                        */
/**************************************************************/
   /*  PRACH_SCCPCH_SIList   */
typedef struct PRACH_SCCPCH_SIList {
  unsigned int    numocts;
  unsigned char   data[maxBlock - 8];
} PRACH_SCCPCH_SIList;

/*  PrimaryCCPCH_TX_Power  */
typedef int     PrimaryCCPCH_TX_Power;
/*  Alpha  */
typedef int     Alpha;
/*  ConstantValueTdd   */
typedef int     ConstantValueTdd;

/*  OpenLoopPowerControl_TDD  */
typedef struct OpenLoopPowerControl_TDD {
  PrimaryCCPCH_TX_Power primaryCCPCH_TX_Power;
  Alpha           alpha;
  ConstantValueTdd prach_ConstantValue;
  ConstantValueTdd dpch_ConstantValue;
//   ConstantValueTdd  pusch_ConstantValue;
} OpenLoopPowerControl_TDD;

/**************************************************************/
/*  SysInfoType14  Information elements                        */
/**************************************************************/
/*  TimeslotNumber  */
//  value >= 0 && value <= 14
typedef unsigned int TimeslotNumber;

/*  TDD_UL_Interference  */
//  value >= -110 && value <= -52
typedef int     TDD_UL_Interference;

/*  IndividualTS_Interference  */
typedef struct IndividualTS_Interference {
  TimeslotNumber  timeslot;
  TDD_UL_Interference ul_TimeslotInterference;
} IndividualTS_Interference;

/*  IndividualTS_InterferenceList */
typedef struct IndividualTS_InterferenceList {
  unsigned int    numSlots;
  IndividualTS_Interference data[15];
} IndividualTS_InterferenceList;

/*  ExpirationTimeFactor   */
//  value >= 1 && value <= 8 --> 2^(value)
typedef unsigned int ExpirationTimeFactor;

/**************************************************************/
/*  SysInfoType18  Information elements                       */
/**************************************************************/
    /*  IdentitiesOfNeighbourCells */
typedef struct IdentitiesOfNeighbourCells {
  unsigned int    numocts;
  unsigned char   data[maxSIBNAS];
} IdentitiesOfNeighbourCells;

    /*  CodeGroupsOfNeighbourCells */
typedef struct CodeGroupsOfNeighbourCells {
  unsigned char   data[maxCells];
} CodeGroupsOfNeighbourCells;

#endif
