/***************************************************************************
                          rrc_msg_class.h  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Define structures for RRC peer-to-peer messages
 ***************************************************************************/  
#ifndef __RRC_MSG_CLASS_H__
#define __RRC_MSG_CLASS_H__
#include "rrc_msg_pdus.h"
#include "rrc_mbms_pdus.h"

/**************************************************************/ 
/*  DL_DCCH_MessageType                                       */ 
/**************************************************************/ 
  typedef struct DL_DCCH_MessageType {
    unsigned char  type;
    union {
      /* type = 4 */
//      CellUpdateConfirm *cellUpdateConfirm;
      /* type = 6 */ 
      DownlinkDirectTransfer *downlinkDirectTransfer;
      /* type = 9 */
      MeasurementControl *measurementControl;
      /* type = 10 */
      PagingType2 *pagingType2;
      /* type = 14 */
      RadioBearerRelease *radioBearerRelease;
      /* type = 15 */
      RadioBearerSetup *radioBearerSetup;
      /* type = 16 */
//      RRCConnectionRelease *rrcConnectionRelease;
      /* type = 22 */
      UECapabilityInformationConfirm *ueCapabilityInformationConfirm;
      /* type = 23 */
      //ULPCHControl *ulPchControl;
      /* type = 27 */
      MBMSModifiedServicesInformation *mbmsModifiedServicesInformation;
    } content;
  } DL_DCCH_MessageType;


/**************************************************************/ 
/*  DL DCCH Message                                           */ 
/**************************************************************/ 
  typedef struct DL_DCCH_Message {
    IntegrityCheckInfo integrityCheckInfo;
    DL_DCCH_MessageType message;
  } DL_DCCH_Message;


/**************************************************************/ 
/*  UL_DCCH_MessageType                                       */ 
/**************************************************************/ 
  typedef struct UL_DCCH_MessageType {
    unsigned char  type;
    union {
      /* type = 6 */
      InitialDirectTransfer *initialDirectTransfer;
      /* type = 9 */
      MeasurementReport *measurementReport;
      /* type = 14 */
      RadioBearerReleaseComplete *radioBearerReleaseComplete;
      /* type = 15 */
      RadioBearerReleaseFailure *radioBearerReleaseFailure;
      /* type = 16 */
      RadioBearerSetupComplete *radioBearerSetupComplete;
      /* type = 17 */
      RadioBearerSetupFailure *radioBearerSetupFailure;
      /* type = 18 */
//      RRCConnectionReleaseComplete *rrcConnectionReleaseComplete;
      /* type = 19 */ 
      RRCConnectionSetupComplete *rrcConnectionSetupComplete;
      /*type = 27 */
      UECapabilityInformation	 *ueCapabilityInformation;
      /* type = 28 */
      UplinkDirectTransfer *uplinkDirectTransfer;
      /* type = 31 */
      RRCConnectionRelease_UL *rrcConnectionRelease_UL;
    } content;
  } UL_DCCH_MessageType;

/**************************************************************/ 
/*  UL DCCH Message                                           */ 
/**************************************************************/ 
  typedef struct UL_DCCH_Message {
    IntegrityCheckInfo integrityCheckInfo;
    UL_DCCH_MessageType message;
  } UL_DCCH_Message;


/**************************************************************/ 
/*  DL_CCCH_MessageType                                       */
/**************************************************************/
  typedef struct DL_CCCH_MessageType {
    unsigned char  type;
    union {
      /* type = 1 */
      CellUpdateConfirm_CCCH *cellUpdateConfirm;
      /* type = 2 */
      RRCConnectionReject *rrcConnectionReject;
      /* type = 3 */
//      RRCConnectionRelease_CCCH *rrcConnectionRelease;
      /* type = 4 */
      RRCConnectionSetup *rrcConnectionSetup;
    } content;
  } DL_CCCH_MessageType;

/**************************************************************/ 
/*  DL CCCH Message                                           */ 
/**************************************************************/ 
  typedef struct DL_CCCH_Message {
    IntegrityCheckInfo integrityCheckInfo;
    DL_CCCH_MessageType message;
  } DL_CCCH_Message;


/**************************************************************/ 
/*  UL CCCH MessageType                                       */ 
/**************************************************************/ 
  typedef struct UL_CCCH_MessageType {
    unsigned char  type;
    union {
      /* type = 1 */
      CellUpdate cellUpdate;
      /* type = 2 */
      RRCConnectionRequest rrcConnectionRequest;
    } content;
  } UL_CCCH_MessageType;

/**************************************************************/ 
/*  UL CCCH Message                                           */ 
/**************************************************************/ 
  typedef struct UL_CCCH_Message {
    IntegrityCheckInfo integrityCheckInfo;
    UL_CCCH_MessageType message;
  } UL_CCCH_Message;


/**************************************************************/
/*  DL MCCH MessageType                                       */
/**************************************************************/
  typedef struct MCCH_MessageType {
    unsigned char type;
    union {
      /* type = 1 */
      MBMSAccessInformation   *mbmsAccessInformation;
      /* type = 2 */
      MBMSCommonPTMRBInformation  *mbmsCommonPTMRBInformation;
      /* type = 3 */
      MBMSCurrentCellPTMRBInformation  *mbmsCurrentCellPTMRBInformation;
      /* type = 4 */
      MBMSGeneralInformation  *mbmsGeneralInformation;
      /* type = 5 */
      MBMSModifiedServicesInformation  *mbmsModifiedServicesInformation;
      /* type = 6 */
      MBMSNeighbouringCellPTMRBInformation  *mbmsNeighbouringCellPTMRBInformation;
      /* type = 7 */
      MBMSUnmodifiedServicesInformation  *mbmsUnmodifiedServicesInformation;
    } content;
  } MCCH_MessageType;

/**************************************************************/
/*  DL MCCH Message                                           */
/**************************************************************/
  typedef struct MCCH_Message {
    MCCH_MessageType  message;
  } MCCH_Message;


/**************************************************************/
/*  DL MSCH MessageType                                       */
/**************************************************************/
  typedef struct MSCH_MessageType {
    unsigned char type; //reserved for the future extension
    union {
      /* type = 1 */
      MBMSSchedulingInformation	*mbmsSchedulingInformation;
    } content;
  } MSCH_MessageType;

/**************************************************************/
/*  DL MSCH Message                                           */
/**************************************************************/
  typedef struct MSCH_Message {
    IntegrityCheckInfo  integrityCheckInfo;
    MSCH_MessageType  message;
  } MSCH_Message;

#endif
