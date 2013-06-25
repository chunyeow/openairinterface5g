/***************************************************************************
                          rrc_mbms_ies.h - description
                          -------------------
    begin               : May 9, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by	        : Huu-Nghia.Nguyen@eurecom.fr	
    modified by        	: Michelle.Wetterwald@eurecom.fr
    goal		: This file contains definitions of IEs for MBMS messages
 **************************************************************************/

#ifndef _RRC_MBMS_IES_H_
#define _RRC_MBMS_IES_H_

#include "rrc_platform_types.h"
#include "rrc_mbms_constant.h"

/**************************************************************/
/*  MBMSAccessInformation                                     */
/**************************************************************/
/* MBMS-ShortTransmissionID */
typedef u16 MBMS_ShortTransmissionID; /* 1..32 */

/* MBMS_AccessProbabilityFactor */
typedef u16 MBMS_AccessProbabilityFactor;

/* MBMS_ServiceAccessInfo-r6 */
typedef struct MBMS_ServiceAccessInfo {
  MBMS_ShortTransmissionID      shortTransmissionID;
  MBMS_AccessProbabilityFactor  accessprobabilityFactor_Idle;
  //MBMS_AccessProbabilityFactor  accessprobabilityFactor_UraPCH /* OPTIONAL */;
} MBMS_ServiceAccessInfo;

/* MBMS_ServiceAccessInfoList */
typedef struct MBMS_ServiceAccessInfoList {
	u8 numAccessInfo; //0..maxMBMSservCount (=4)
	MBMS_ServiceAccessInfo accessInfo[maxMBMSservCount];
} MBMS_ServiceAccessInfoList;

/**************************************************************/
/* MBMSGeneralInformation                                     */
/**************************************************************/
typedef u16	T_318;

/* MBMS_TimersAndCounters */
typedef struct MBMS_TimersAndCounters {
   T_318   t_318  /* DEFAULT 3 <-- ms1000 */;
} MBMS_TimersAndCounters;

/* MBMS_CellGroupIdentity */
typedef  u16  MBMS_CellGroupIdentity; /** Bit string (12) coded in u16 */

/**************************************************************/
/* MBMSCommonPTMRBInformation                                 */
/**************************************************************/

/* L1L2Configuration */
//HGN typedef u8 MBMS_L12Configuration[MBMS_L12_CONFIGURATION_SIZE];
typedef struct MTCH_Configuration{
   u16 numoctets;
   u8 data[MTCH_CONFIGURATION_SIZE];
} MTCH_Configuration;

/**************************************************************/
/* MBMSCurrentCellPTMRBInformation                            */
/**************************************************************/
/* L1L2Configuration cf. MBMSCommonPTMRBInformation*/

/**************************************************************/
/* MBMSNeighbouringCellPTMRBInformation                       */
/**************************************************************/
/*  NeighbourCellIdentity  */
typedef u8 NeighbourCellIdentity;

/* NeighbouringCellConfiguration */
//typedef u8 MBMS_NeighbouringCellConfiguration[MBMS_NEIGHBOURING_CONFIGURATION_SIZE];
typedef struct NeighbouringCellConfiguration{
   u16 numoctets;
   u8 data[NEIGHBOUR_CONFIGURATION_SIZE];
} NeighbouringCellConfiguration;

/**************************************************************/
/* MBMSModifiedServicesInformation                            */
/**************************************************************/
/* MBMS-ServiceIdentity */
typedef struct MBMS_ServiceIdentity {
   u8  serviceIdentity[3];
// PLMN_Identity plmn_Identity; //Not needed, assumed that this is the same value in BCH
} MBMS_ServiceIdentity;

/* MBMS_SessionIdentity */
typedef u8 MBMS_SessionIdentity;

/* MBMS-TransmissionIdentity */
typedef struct MBMS_TransmissionIdentity{
   MBMS_ServiceIdentity mbms_ServiceIdentity;
   MBMS_SessionIdentity mbms_SessionIdentity;
} MBMS_TransmissionIdentity;

/* MBMS_RequiredUEAction_Mod */
/*  values defined in rrc_mbms_constant.h */
typedef u16 MBMS_RequiredUEAction_Mod;

/* MBMS-ModifiedService-r6 */
typedef struct MBMS_ModifiedService {
	MBMS_TransmissionIdentity  mbms_TransmissionIdentity;
	MBMS_RequiredUEAction_Mod  mbms_RequiredUEAction;
  mbms_bool                  continueMCCHReading;
} MBMS_ModifiedService;

/* MBMS_ModifiedServiceList */
typedef struct MBMS_ModifiedServiceList {
   u16 numService; //0..maxMBMSservModif
   MBMS_ModifiedService mbms_modifiedServices[maxMBMSservModif];
} MBMS_ModifiedServiceList;
/****/
// mbms_ReacquireMCCH;   //ENUMERATED { true }
// mbms_AllUnmodifiedPTMServices   //ENUMERATED { true }
// are defined directly in the PDU, same as done in ASN1

/* MBMS-NumberOfNeighbourCells */
// INTEGER (0..32)
typedef u16 MBMS_NumberOfNeighbourCells;

/* MBMS-PTMActivationTime */
// INTEGER (0..2047)
typedef u16 MBMS_PTMActivationTime;


/**************************************************************/
/* MBMSUnmodifiedServicesInformation                          */
/**************************************************************/
/* MBMS-TransmissionIdentity cf. MBMSModifiedServicesInformation*/

/* MBMS_RequiredUEAction_UMod */
/*  values defined in rrc_mbms_constant.h */
typedef u16 MBMS_RequiredUEAction_UMod;

/* MBMS_UnmodifiedService */
typedef struct MBMS_UnmodifiedService {
	MBMS_TransmissionIdentity  mbms_TransmissionIdentity;
	MBMS_RequiredUEAction_UMod mbms_RequiredUEAction;
} MBMS_UnmodifiedService;

/* MBMS_UnmodifiedServiceList  */
typedef struct MBMS_UnmodifiedServiceList {
  u16 numService;		//0..maxMBMSservUnmodif
  MBMS_UnmodifiedService unmodifiedServices[maxMBMSservUnmodif];
} MBMS_UnmodifiedServiceList;


/**************************************************************/
/* MBMSSchedulingInformation                                  */
/**************************************************************/
/* MBMS-ServiceTransmInfo */
// Spec : Actual values (start, duration) = IE values * 4
typedef struct MBMS_ServiceTransmInfo {
 	u16  start;     //0..1020 by step of 4
 	u16  duration;  //4..1024
} MBMS_ServiceTransmInfo;

/* MBMS_ServiceTransmInfoList */
typedef struct MBMS_ServiceTransmInfoList  {
   u16 numInfo;	//0..maxMBMSTransmis  ( = 4)
   MBMS_ServiceTransmInfo serviceTransmissionInfo[maxMBMSTransmis];
} MBMS_ServiceTransmInfoList;

/* MBMS_ServiceSchedulingInfo */
typedef struct MBMS_ServiceSchedulingInfo {
	MBMS_TransmissionIdentity  mbms_TransmissionIdentity;
	MBMS_ServiceTransmInfoList mbms_TransmissionInfoList;
	u16  nextSchedulingPeriod;  //0..31
} MBMS_ServiceSchedulingInfo;

typedef struct MBMS_ServiceSchedulingInfoList {
   u16 numInfo;		//0..maxMBMSservSched ( = 16)
   MBMS_ServiceSchedulingInfo schedulingInfo[maxMBMSservSched];
} MBMS_ServiceSchedulingInfoList;

#endif
