/***************************************************************************
                          rrc_mbms_pdus.h - description
                          -------------------
    begin               : May 9, 2005
    copyright           : (C) 2001, 2010 by Eurecom
    created by		      : Michelle.Wetterwald@eurecom.fr, Huu-Nghia.Nguyen@eurecom.fr	
    modified by		      : This file contains definitions of PDUs concerned with MBMS
 **************************************************************************/
#ifndef _RRC_MBMS_PDUS_H_
#define _RRC_MBMS_PDUS_H_

#include "rrc_mbms_ies.h"

/**************************************************************/
/*  MBMSAccessInformation                                     */
/**************************************************************/
typedef struct MBMSAccessInformation {
   MBMS_ServiceAccessInfoList  mbms_ServiceAccessInfoList;
} MBMSAccessInformation;

/**************************************************************/
/* MBMSGeneralInformation                                     */
/**************************************************************/
typedef struct MBMSGeneralInformation {
   MBMS_TimersAndCounters  mbms_TimersAndCounters;
// MBMS_MICHConfigurationInfo     michConfigurationInfo;
   MBMS_CellGroupIdentity  cellGroupIdentity;
// MBMS_MSCHConfigurationInfo mschDefaultConfigurationInfo;
} MBMSGeneralInformation;

/**************************************************************/
/* MBMSCommonPTMRBInformation                                 */
/**************************************************************/
typedef struct MBMSCommonPTMRBInformation {
   MTCH_Configuration	l12Configuration;

//       The followings are specified in TS 25.331
// MBMS_CommonRBInformationList   mbms_CommonRBInformationList;
// MBMS_TranspChInfoForEachTrCh   mbms_TranspChInfoForEachTrCh;
// MBMS_TranspChInfoForEachCCTrCh mbms_TranspChInfoForEachCCTrCh;
// MBMS_PhyChInformationList      mbms_PhyChInformationList;
} MBMSCommonPTMRBInformation;

/**************************************************************/
/* MBMSCurrentCellPTMRBInformation                            */
/**************************************************************/
typedef struct MBMSCurrentCellPTMRBInformation {
   MTCH_Configuration l12Configuration;
// MBMS_CurrentCell_SCCPCHList	mbms_CurrentCell_SCCPCHList;
// MBMS_SIBType5_SCCPCHList	mbms_SIBType5_SCCPCHList;
} MBMSCurrentCellPTMRBInformation;

/**************************************************************/
/* MBMSNeighbouringCellPTMRBInformation                       */
/**************************************************************/
typedef struct MBMSNeighbouringCellPTMRBInformation {
    /** in the range 1..X where X is undefined */
   NeighbourCellIdentity    neighbouringCellIdentity;
   NeighbouringCellConfiguration  neighbouringCellConfig;
} MBMSNeighbouringCellPTMRBInformation;

/**************************************************************/
/* MBMSModifiedServicesInformation                            */
/**************************************************************/
typedef struct MBMSModifiedServicesInformation {
   MBMS_ModifiedServiceList  modifiedServiceList;
   mbms_bool                 mbms_ReacquireMCCH;
// DynamicPersistenceLevel   mbms_DynamicPersistenceLevel;
//   u16           endOfModifiedMCCHInformation;  //[1..16]
   MBMS_NumberOfNeighbourCells  mbmsNumberOfNeighbourCells;
//   mbms_bool                 mbms_AllUnmodifiedPTMServices;
   MBMS_PTMActivationTime    mbms_PTMActivationTime;
} MBMSModifiedServicesInformation;

/**************************************************************/
/* MBMSUnmodifiedServicesInformation                          */
/**************************************************************/
typedef struct MBMSUnmodifiedServicesInformation {
   MBMS_UnmodifiedServiceList unmodifiedServiceList;
} MBMSUnmodifiedServicesInformation;

/**************************************************************/
/* MBMSSchedulingInformation                                  */
/**************************************************************/
typedef struct MBMSSchedulingInformation {
   MBMS_ServiceSchedulingInfoList serviceSchedulingInfoList;
//   u8                             nextSchedulingPeriod;
} MBMSSchedulingInformation;

#endif

