/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

/*******************************************************************************
 * This file had been created by asn1tostruct.py script v0.3
 * Please do not modify it directly.
 * Created on: 2012-11-13 11:44:11.921370
 * from /homes/roux/trunk-clean/openair2/S1AP/MESSAGES/ASN1/R9.8/S1AP-PDU-Contents.asn
 ******************************************************************************/
#include "s1ap_common.h"

#ifndef S1AP_IES_DEFS_H_
#define S1AP_IES_DEFS_H_

typedef struct E_RABToBeSetupListBearerSUReqIEs_s {
    A_SEQUENCE_OF(struct E_RABToBeSetupItemBearerSUReq_s) e_RABToBeSetupItemBearerSUReq;
} E_RABToBeSetupListBearerSUReqIEs_t;

typedef struct UE_associatedLogicalS1_ConnectionListResAckIEs_s {
    A_SEQUENCE_OF(struct UE_associatedLogicalS1_ConnectionItemResAck_s) uE_associatedLogicalS1_ConnectionItemResAck;
} UE_associatedLogicalS1_ConnectionListResAckIEs_t;

typedef struct TAIListIEs_s {
    A_SEQUENCE_OF(struct TAIItem_s) taiItem;
} TAIListIEs_t;

typedef struct E_RABAdmittedListIEs_s {
    A_SEQUENCE_OF(struct E_RABAdmittedItem_s) e_RABAdmittedItem;
} E_RABAdmittedListIEs_t;

typedef struct E_RABListIEs_s {
    A_SEQUENCE_OF(struct E_RABItem_s) e_RABItem;
} E_RABListIEs_t;

typedef struct E_RABToBeSetupListHOReqIEs_s {
    A_SEQUENCE_OF(struct E_RABToBeSetupItemHOReq_s) e_RABToBeSetupItemHOReq;
} E_RABToBeSetupListHOReqIEs_t;

typedef struct E_RABSetupListBearerSUResIEs_s {
    A_SEQUENCE_OF(struct E_RABSetupItemBearerSURes_s) e_RABSetupItemBearerSURes;
} E_RABSetupListBearerSUResIEs_t;

typedef struct E_RABToBeSetupListCtxtSUReqIEs_s {
    A_SEQUENCE_OF(struct E_RABToBeSetupItemCtxtSUReq_s) e_RABToBeSetupItemCtxtSUReq;
} E_RABToBeSetupListCtxtSUReqIEs_t;

typedef struct E_RABSetupListCtxtSUResIEs_s {
    A_SEQUENCE_OF(struct E_RABSetupItemCtxtSURes_s) e_RABSetupItemCtxtSURes;
} E_RABSetupListCtxtSUResIEs_t;

typedef struct E_RABInformationListIEs_s {
    A_SEQUENCE_OF(struct E_RABInformationItem_s) e_RABInformationItem;
} E_RABInformationListIEs_t;

typedef struct E_RABReleaseListBearerRelCompIEs_s {
    A_SEQUENCE_OF(struct E_RABReleaseItemBearerRelComp_s) e_RABReleaseItemBearerRelComp;
} E_RABReleaseListBearerRelCompIEs_t;

typedef struct E_RABToBeModifiedListBearerModReqIEs_s {
    A_SEQUENCE_OF(struct E_RABToBeModifiedItemBearerModReq_s) e_RABToBeModifiedItemBearerModReq;
} E_RABToBeModifiedListBearerModReqIEs_t;

typedef struct E_RABToBeSwitchedDLListIEs_s {
    A_SEQUENCE_OF(struct E_RABToBeSwitchedDLItem_s) e_RABToBeSwitchedDLItem;
} E_RABToBeSwitchedDLListIEs_t;

typedef struct Bearers_SubjectToStatusTransfer_ListIEs_s {
    A_SEQUENCE_OF(struct Bearers_SubjectToStatusTransfer_Item_s) bearers_SubjectToStatusTransfer_Item;
} Bearers_SubjectToStatusTransfer_ListIEs_t;

typedef struct E_RABToBeSwitchedULListIEs_s {
    A_SEQUENCE_OF(struct E_RABToBeSwitchedULItem_s) e_RABToBeSwitchedULItem;
} E_RABToBeSwitchedULListIEs_t;

typedef struct E_RABDataForwardingListIEs_s {
    A_SEQUENCE_OF(struct E_RABDataForwardingItem_s) e_RABDataForwardingItem;
} E_RABDataForwardingListIEs_t;

typedef struct E_RABModifyListBearerModResIEs_s {
    A_SEQUENCE_OF(struct E_RABModifyItemBearerModRes_s) e_RABModifyItemBearerModRes;
} E_RABModifyListBearerModResIEs_t;

typedef struct UE_associatedLogicalS1_ConnectionListResIEs_s {
    A_SEQUENCE_OF(struct UE_associatedLogicalS1_ConnectionItemRes_s) uE_associatedLogicalS1_ConnectionItemRes;
} UE_associatedLogicalS1_ConnectionListResIEs_t;

typedef struct E_RABFailedtoSetupListHOReqAckIEs_s {
    A_SEQUENCE_OF(struct E_RABFailedtoSetupItemHOReqAck_s) e_RABFailedtoSetupItemHOReqAck;
} E_RABFailedtoSetupListHOReqAckIEs_t;

#define MMECONFIGURATIONUPDATEIES_MMENAME_PRESENT             (1 << 0)
#define MMECONFIGURATIONUPDATEIES_SERVEDGUMMEIS_PRESENT       (1 << 1)
#define MMECONFIGURATIONUPDATEIES_RELATIVEMMECAPACITY_PRESENT (1 << 2)

typedef struct MMEConfigurationUpdateIEs_s {
    uint16_t              presenceMask;
    MMEname_t             mmEname; ///< Optional field
    ServedGUMMEIs_t       servedGUMMEIs; ///< Optional field
    RelativeMMECapacity_t relativeMMECapacity; ///< Optional field
} MMEConfigurationUpdateIEs_t;

#define UECONTEXTMODIFICATIONREQUESTIES_SECURITYKEY_PRESENT               (1 << 0)
#define UECONTEXTMODIFICATIONREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT (1 << 1)
#define UECONTEXTMODIFICATIONREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT (1 << 2)
#define UECONTEXTMODIFICATIONREQUESTIES_CSFALLBACKINDICATOR_PRESENT       (1 << 3)
#define UECONTEXTMODIFICATIONREQUESTIES_UESECURITYCAPABILITIES_PRESENT    (1 << 4)
#define UECONTEXTMODIFICATIONREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT       (1 << 5)
#define UECONTEXTMODIFICATIONREQUESTIES_LAI_PRESENT                       (1 << 6)

typedef struct UEContextModificationRequestIEs_s {
    uint16_t                    presenceMask;
    MME_UE_S1AP_ID_t            mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t            eNB_UE_S1AP_ID;
    SecurityKey_t               securityKey; ///< Optional field
    SubscriberProfileIDforRFP_t subscriberProfileIDforRFP; ///< Optional field
    UEAggregateMaximumBitrate_t uEaggregateMaximumBitrate; ///< Optional field
    CSFallbackIndicator_t       csFallbackIndicator; ///< Optional field
    UESecurityCapabilities_t    ueSecurityCapabilities; ///< Optional field
    CSGMembershipStatus_t       csgMembershipStatus; ///< Optional field
    LAI_t                       registeredLAI; ///< Optional field
} UEContextModificationRequestIEs_t;

typedef struct E_RABToBeSetupItemBearerSUReqIEs_s {
    E_RABToBeSetupItemBearerSUReq_t e_RABToBeSetupItemBearerSUReq;
} E_RABToBeSetupItemBearerSUReqIEs_t;

typedef struct ENBDirectInformationTransferIEs_s {
    Inter_SystemInformationTransferType_t inter_SystemInformationTransferTypeEDT;
} ENBDirectInformationTransferIEs_t;

typedef struct UE_associatedLogicalS1_ConnectionItemResAck_s {
    UE_associatedLogicalS1_ConnectionItem_t uE_associatedLogicalS1_ConnectionItem;
} UE_associatedLogicalS1_ConnectionItemResAck_t;

#define RESETACKNOWLEDGEIES_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_PRESENT (1 << 0)
#define RESETACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT                      (1 << 1)

typedef struct ResetAcknowledgeIEs_s {
    uint16_t                                      presenceMask;
    UE_associatedLogicalS1_ConnectionListResAckIEs_t uE_associatedLogicalS1_ConnectionListResAck; ///< Optional field
    CriticalityDiagnostics_t                      criticalityDiagnostics; ///< Optional field
} ResetAcknowledgeIEs_t;

#define PAGINGIES_PAGINGDRX_PRESENT            (1 << 0)
#define PAGINGIES_CSG_IDLIST_PRESENT           (1 << 1)

typedef struct PagingIEs_s {
    uint16_t               presenceMask;
    UEIdentityIndexValue_t ueIdentityIndexValue;
    UEPagingID_t           uePagingID;
    PagingDRX_t            pagingDRX; ///< Optional field
    CNDomain_t             cnDomain;
    TAIListIEs_t taiList;
    CSG_IdList_t           csG_IdList; ///< Optional field
} PagingIEs_t;

typedef struct PathSwitchRequestIEs_s {
    ENB_UE_S1AP_ID_t          eNB_UE_S1AP_ID;
    E_RABToBeSwitchedDLListIEs_t e_RABToBeSwitchedDLList;
    MME_UE_S1AP_ID_t          sourceMME_UE_S1AP_ID;
    EUTRAN_CGI_t              eutran_cgi;
    TAI_t                     tai;
    UESecurityCapabilities_t  ueSecurityCapabilities;
} PathSwitchRequestIEs_t;

typedef struct DeactivateTraceIEs_s {
    MME_UE_S1AP_ID_t   mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t   eNB_UE_S1AP_ID;
    E_UTRAN_Trace_ID_t e_UTRAN_Trace_ID;
} DeactivateTraceIEs_t;

typedef struct LocationReportingControlIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    RequestType_t    requestType;
} LocationReportingControlIEs_t;

#define ENBCONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct ENBConfigurationUpdateAcknowledgeIEs_s {
    uint16_t                 presenceMask;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} ENBConfigurationUpdateAcknowledgeIEs_t;

#define HANDOVERFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct HandoverFailureIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id;
    Cause_t                  cause;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} HandoverFailureIEs_t;

typedef struct LocationReportingFailureIndicationIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    Cause_t          cause;
} LocationReportingFailureIndicationIEs_t;

#define DOWNLINKNASTRANSPORTIES_HANDOVERRESTRICTIONLIST_PRESENT (1 << 0)

typedef struct DownlinkNASTransportIEs_s {
    uint16_t                  presenceMask;
    MME_UE_S1AP_ID_t          mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t          eNB_UE_S1AP_ID;
    NAS_PDU_t                 nas_pdu;
    HandoverRestrictionList_t handoverRestrictionList; ///< Optional field
} DownlinkNASTransportIEs_t;

#define MMECONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT             (1 << 0)
#define MMECONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 1)

typedef struct MMEConfigurationUpdateFailureIEs_s {
    uint16_t                 presenceMask;
    Cause_t                  cause;
    TimeToWait_t             timeToWait; ///< Optional field
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} MMEConfigurationUpdateFailureIEs_t;

typedef struct TAIItemIEs_s {
    TAIItem_t taiItem;
} TAIItemIEs_t;

#define UECONTEXTRELEASECOMPLETEIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct UEContextReleaseCompleteIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t         eNB_UE_S1AP_ID;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} UEContextReleaseCompleteIEs_t;

#define S1SETUPRESPONSEIES_MMENAME_PRESENT                (1 << 0)
#define S1SETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 1)

typedef struct S1SetupResponseIEs_s {
    uint16_t                 presenceMask;
    MMEname_t                mmEname; ///< Optional field
    ServedGUMMEIs_t          servedGUMMEIs;
    RelativeMMECapacity_t    relativeMMECapacity;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} S1SetupResponseIEs_t;

#define HANDOVERPREPARATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct HandoverPreparationFailureIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t         eNB_UE_S1AP_ID;
    Cause_t                  cause;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} HandoverPreparationFailureIEs_t;

typedef struct E_RABAdmittedItemIEs_s {
    E_RABAdmittedItem_t e_RABAdmittedItem;
} E_RABAdmittedItemIEs_t;

#define KILLRESPONSEIES_BROADCASTCANCELLEDAREALIST_PRESENT (1 << 0)
#define KILLRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT     (1 << 1)

typedef struct KillResponseIEs_s {
    uint16_t                     presenceMask;
    MessageIdentifier_t          messageIdentifier;
    SerialNumber_t               serialNumber;
    BroadcastCancelledAreaList_t broadcastCancelledAreaList; ///< Optional field
    CriticalityDiagnostics_t     criticalityDiagnostics; ///< Optional field
} KillResponseIEs_t;

typedef struct UplinkUEAssociatedLPPaTransportIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    Routing_ID_t     routing_ID;
    LPPa_PDU_t       lpPa_PDU;
} UplinkUEAssociatedLPPaTransportIEs_t;

#define ENBCONFIGURATIONUPDATEIES_ENBNAME_PRESENT      (1 << 0)
#define ENBCONFIGURATIONUPDATEIES_SUPPORTEDTAS_PRESENT (1 << 1)
#define ENBCONFIGURATIONUPDATEIES_CSG_IDLIST_PRESENT   (1 << 2)
#define ENBCONFIGURATIONUPDATEIES_PAGINGDRX_PRESENT    (1 << 3)

typedef struct ENBConfigurationUpdateIEs_s {
    uint16_t       presenceMask;
    ENBname_t      eNBname; ///< Optional field
    SupportedTAs_t supportedTAs; ///< Optional field
    CSG_IdList_t   csG_IdList; ///< Optional field
    PagingDRX_t    defaultPagingDRX; ///< Optional field
} ENBConfigurationUpdateIEs_t;

#define INITIALCONTEXTSETUPREQUESTIES_TRACEACTIVATION_PRESENT             (1 << 0)
#define INITIALCONTEXTSETUPREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT     (1 << 1)
#define INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT           (1 << 2)
#define INITIALCONTEXTSETUPREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT   (1 << 3)
#define INITIALCONTEXTSETUPREQUESTIES_CSFALLBACKINDICATOR_PRESENT         (1 << 4)
#define INITIALCONTEXTSETUPREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT      (1 << 5)
#define INITIALCONTEXTSETUPREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT         (1 << 6)
#define INITIALCONTEXTSETUPREQUESTIES_LAI_PRESENT                         (1 << 7)

typedef struct InitialContextSetupRequestIEs_s {
    uint16_t                      presenceMask;
    MME_UE_S1AP_ID_t              mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t              eNB_UE_S1AP_ID;
    UEAggregateMaximumBitrate_t   uEaggregateMaximumBitrate;
    E_RABToBeSetupListCtxtSUReqIEs_t e_RABToBeSetupListCtxtSUReq;
    UESecurityCapabilities_t      ueSecurityCapabilities;
    SecurityKey_t                 securityKey;
    TraceActivation_t             traceActivation; ///< Optional field
    HandoverRestrictionList_t     handoverRestrictionList; ///< Optional field
    UERadioCapability_t           ueRadioCapability; ///< Optional field
    SubscriberProfileIDforRFP_t   subscriberProfileIDforRFP; ///< Optional field
    CSFallbackIndicator_t         csFallbackIndicator; ///< Optional field
    SRVCCOperationPossible_t      srvccOperationPossible; ///< Optional field
    CSGMembershipStatus_t         csgMembershipStatus; ///< Optional field
    LAI_t                         registeredLAI; ///< Optional field
} InitialContextSetupRequestIEs_t;

typedef struct MMEDirectInformationTransferIEs_s {
    Inter_SystemInformationTransferType_t inter_SystemInformationTransferTypeMDT;
} MMEDirectInformationTransferIEs_t;

typedef struct E_RABItemIEs_s {
    E_RABItem_t e_RABItem;
} E_RABItemIEs_t;

typedef struct UplinkNonUEAssociatedLPPaTransportIEs_s {
    Routing_ID_t routing_ID;
    LPPa_PDU_t   lpPa_PDU;
} UplinkNonUEAssociatedLPPaTransportIEs_t;

typedef struct ResetIEs_s {
    Cause_t     cause;
    ResetType_t resetType;
} ResetIEs_t;

typedef struct UEContextReleaseCommandIEs_s {
    UE_S1AP_IDs_t uE_S1AP_IDs;
    Cause_t       cause;
} UEContextReleaseCommandIEs_t;

typedef struct UECapabilityInfoIndicationIEs_s {
    MME_UE_S1AP_ID_t    mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t    eNB_UE_S1AP_ID;
    UERadioCapability_t ueRadioCapability;
} UECapabilityInfoIndicationIEs_t;

#define UPLINKS1CDMA2000TUNNELINGIES_CDMA2000HOREQUIREDINDICATION_PRESENT       (1 << 0)
#define UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXSRVCCINFO_PRESENT              (1 << 1)
#define UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXRAND_PRESENT                   (1 << 2)
#define UPLINKS1CDMA2000TUNNELINGIES_EUTRANROUNDTRIPDELAYESTIMATIONINFO_PRESENT (1 << 3)

typedef struct UplinkS1cdma2000tunnelingIEs_s {
    uint16_t                             presenceMask;
    MME_UE_S1AP_ID_t                     mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                     eNB_UE_S1AP_ID;
    Cdma2000RATType_t                    cdma2000RATType;
    Cdma2000SectorID_t                   cdma2000SectorID;
    Cdma2000HORequiredIndication_t       cdma2000HORequiredIndication; ///< Optional field
    Cdma2000OneXSRVCCInfo_t              cdma2000OneXSRVCCInfo; ///< Optional field
    Cdma2000OneXRAND_t                   cdma2000OneXRAND; ///< Optional field
    Cdma2000PDU_t                        cdma2000PDU;
    EUTRANRoundTripDelayEstimationInfo_t eutranRoundTripDelayEstimationInfo; ///< Optional field
} UplinkS1cdma2000tunnelingIEs_t;

typedef struct E_RABToBeSetupItemHOReqIEs_s {
    E_RABToBeSetupItemHOReq_t e_RABToBeSetupItemHOReq;
} E_RABToBeSetupItemHOReqIEs_t;

#define WRITEREPLACEWARNINGRESPONSEIES_BROADCASTCOMPLETEDAREALIST_PRESENT (1 << 0)
#define WRITEREPLACEWARNINGRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT     (1 << 1)

typedef struct WriteReplaceWarningResponseIEs_s {
    uint16_t                     presenceMask;
    MessageIdentifier_t          messageIdentifier;
    SerialNumber_t               serialNumber;
    BroadcastCompletedAreaList_t broadcastCompletedAreaList; ///< Optional field
    CriticalityDiagnostics_t     criticalityDiagnostics; ///< Optional field
} WriteReplaceWarningResponseIEs_t;

#define E_RABRELEASECOMMANDIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT (1 << 0)
#define E_RABRELEASECOMMANDIES_NAS_PDU_PRESENT                   (1 << 1)

typedef struct E_RABReleaseCommandIEs_s {
    uint16_t                    presenceMask;
    MME_UE_S1AP_ID_t            mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t            eNB_UE_S1AP_ID;
    UEAggregateMaximumBitrate_t uEaggregateMaximumBitrate; ///< Optional field
    E_RABListIEs_t e_RABToBeReleasedList;
    NAS_PDU_t                   nas_pdu; ///< Optional field
} E_RABReleaseCommandIEs_t;

typedef struct CellTrafficTraceIEs_s {
    MME_UE_S1AP_ID_t        mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t        eNB_UE_S1AP_ID;
    E_UTRAN_Trace_ID_t      e_UTRAN_Trace_ID;
    EUTRAN_CGI_t            eutran_cgi;
    TransportLayerAddress_t traceCollectionEntityIPAddress;
} CellTrafficTraceIEs_t;

typedef struct E_RABSetupItemBearerSUResIEs_s {
    E_RABSetupItemBearerSURes_t e_RABSetupItemBearerSURes;
} E_RABSetupItemBearerSUResIEs_t;

typedef struct E_RABToBeSetupItemCtxtSUReqIEs_s {
    E_RABToBeSetupItemCtxtSUReq_t e_RABToBeSetupItemCtxtSUReq;
} E_RABToBeSetupItemCtxtSUReqIEs_t;

#define PATHSWITCHREQUESTFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct PathSwitchRequestFailureIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t         eNB_UE_S1AP_ID;
    Cause_t                  cause;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} PathSwitchRequestFailureIEs_t;

typedef struct E_RABSetupItemCtxtSUResIEs_s {
    E_RABSetupItemCtxtSURes_t e_RABSetupItemCtxtSURes;
} E_RABSetupItemCtxtSUResIEs_t;

typedef struct E_RABInformationItemIEs_s {
    E_RABInformationListItem_t e_RABInformationListItem;
} E_RABInformationItemIEs_t;

#define INITIALCONTEXTSETUPRESPONSEIES_E_RABLIST_PRESENT               (1 << 0)
#define INITIALCONTEXTSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT  (1 << 1)

typedef struct InitialContextSetupResponseIEs_s {
    uint16_t                  presenceMask;
    MME_UE_S1AP_ID_t          mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t          eNB_UE_S1AP_ID;
    E_RABSetupListCtxtSUResIEs_t e_RABSetupListCtxtSURes;
    E_RABListIEs_t e_RABFailedToSetupListCtxtSURes; ///< Optional field
    CriticalityDiagnostics_t  criticalityDiagnostics; ///< Optional field
} InitialContextSetupResponseIEs_t;

typedef struct MMEStatusTransferIEs_s {
    MME_UE_S1AP_ID_t                          mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                          eNB_UE_S1AP_ID;
    ENB_StatusTransfer_TransparentContainer_t eNB_StatusTransfer_TransparentContainer;
} MMEStatusTransferIEs_t;

#define S1SETUPREQUESTIES_ENBNAME_PRESENT       (1 << 0)
#define S1SETUPREQUESTIES_CSG_IDLIST_PRESENT    (1 << 1)

typedef struct S1SetupRequestIEs_s {
    uint16_t        presenceMask;
    Global_ENB_ID_t global_ENB_ID;
    ENBname_t       eNBname; ///< Optional field
    SupportedTAs_t  supportedTAs;
    PagingDRX_t     defaultPagingDRX;
    CSG_IdList_t    csG_IdList; ///< Optional field
} S1SetupRequestIEs_t;

#define HANDOVERREQUIREDIES_DIRECT_FORWARDING_PATH_AVAILABILITY_PRESENT  (1 << 0)
#define HANDOVERREQUIREDIES_SRVCCHOINDICATION_PRESENT                    (1 << 1)
#define HANDOVERREQUIREDIES_SOURCE_TOTARGET_TRANSPARENTCONTAINER_PRESENT (1 << 2)
#define HANDOVERREQUIREDIES_MSCLASSMARK2_PRESENT                         (1 << 3)
#define HANDOVERREQUIREDIES_MSCLASSMARK3_PRESENT                         (1 << 4)
#define HANDOVERREQUIREDIES_CSG_ID_PRESENT                               (1 << 5)
#define HANDOVERREQUIREDIES_CELLACCESSMODE_PRESENT                       (1 << 6)
#define HANDOVERREQUIREDIES_PS_SERVICENOTAVAILABLE_PRESENT               (1 << 7)

typedef struct HandoverRequiredIEs_s {
    uint16_t                               presenceMask;
    MME_UE_S1AP_ID_t                       mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                       eNB_UE_S1AP_ID;
    HandoverType_t                         handoverType;
    Cause_t                                cause;
    TargetID_t                             targetID;
    Direct_Forwarding_Path_Availability_t  direct_Forwarding_Path_Availability; ///< Optional field
    SRVCCHOIndication_t                    srvcchoIndication; ///< Optional field
    Source_ToTarget_TransparentContainer_t source_ToTarget_TransparentContainer;
    Source_ToTarget_TransparentContainer_t source_ToTarget_TransparentContainer_Secondary; ///< Optional field
    MSClassmark2_t                         msClassmark2; ///< Conditional field
    MSClassmark3_t                         msClassmark3; ///< Conditional field
    CSG_Id_t                               csG_Id; ///< Optional field
    CellAccessMode_t                       cellAccessMode; ///< Optional field
    PS_ServiceNotAvailable_t               pS_ServiceNotAvailable; ///< Optional field
} HandoverRequiredIEs_t;

typedef struct UplinkNASTransportIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    NAS_PDU_t        nas_pdu;
    EUTRAN_CGI_t     eutran_cgi;
    TAI_t            tai;
} UplinkNASTransportIEs_t;

typedef struct LocationReportIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    EUTRAN_CGI_t     eutran_cgi;
    TAI_t            tai;
    RequestType_t    requestType;
} LocationReportIEs_t;

#define UECONTEXTMODIFICATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct UEContextModificationFailureIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t         eNB_UE_S1AP_ID;
    Cause_t                  cause;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} UEContextModificationFailureIEs_t;

#define ENBCONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT (1 << 0)

typedef struct ENBConfigurationTransferIEs_s {
    uint16_t                   presenceMask;
    SONConfigurationTransfer_t sonConfigurationTransferECT; ///< Optional field
} ENBConfigurationTransferIEs_t;

typedef struct DownlinkNonUEAssociatedLPPaTransportIEs_s {
    Routing_ID_t routing_ID;
    LPPa_PDU_t   lpPa_PDU;
} DownlinkNonUEAssociatedLPPaTransportIEs_t;

#define INITIALUEMESSAGEIES_S_TMSI_PRESENT                  (1 << 0)
#define INITIALUEMESSAGEIES_CSG_ID_PRESENT                  (1 << 1)
#define INITIALUEMESSAGEIES_GUMMEI_PRESENT                  (1 << 2)
#define INITIALUEMESSAGEIES_CELLACCESSMODE_PRESENT          (1 << 3)

typedef struct InitialUEMessageIEs_s {
    uint16_t                  presenceMask;
    ENB_UE_S1AP_ID_t          eNB_UE_S1AP_ID;
    NAS_PDU_t                 nas_pdu;
    TAI_t                     tai;
    EUTRAN_CGI_t              eutran_cgi;
    RRC_Establishment_Cause_t rrC_Establishment_Cause;
    S_TMSI_t                  s_tmsi; ///< Optional field
    CSG_Id_t                  csG_Id; ///< Optional field
    GUMMEI_t                  gummei_id; ///< Optional field
    CellAccessMode_t          cellAccessMode; ///< Optional field
} InitialUEMessageIEs_t;

#define E_RABSETUPRESPONSEIES_E_RABSETUPLISTBEARERSURES_PRESENT (1 << 0)
#define E_RABSETUPRESPONSEIES_E_RABLIST_PRESENT                 (1 << 1)
#define E_RABSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT    (1 << 2)

typedef struct E_RABSetupResponseIEs_s {
    uint16_t                    presenceMask;
    MME_UE_S1AP_ID_t            mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t            eNB_UE_S1AP_ID;
    E_RABSetupListBearerSUResIEs_t e_RABSetupListBearerSURes; ///< Optional field
    E_RABListIEs_t e_RABFailedToSetupListBearerSURes; ///< Optional field
    CriticalityDiagnostics_t    criticalityDiagnostics; ///< Optional field
} E_RABSetupResponseIEs_t;

#define PATHSWITCHREQUESTACKNOWLEDGEIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT (1 << 0)
#define PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBESWITCHEDULLIST_PRESENT   (1 << 1)
#define PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABLIST_PRESENT                 (1 << 2)
#define PATHSWITCHREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT    (1 << 3)

typedef struct PathSwitchRequestAcknowledgeIEs_s {
    uint16_t                    presenceMask;
    MME_UE_S1AP_ID_t            mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t            eNB_UE_S1AP_ID;
    UEAggregateMaximumBitrate_t uEaggregateMaximumBitrate; ///< Optional field
    E_RABToBeSwitchedULListIEs_t e_RABToBeSwitchedULList; ///< Optional field
    E_RABListIEs_t e_RABToBeReleasedList; ///< Optional field
    SecurityContext_t           securityContext;
    CriticalityDiagnostics_t    criticalityDiagnostics; ///< Optional field
} PathSwitchRequestAcknowledgeIEs_t;

typedef struct E_RABReleaseItemBearerRelCompIEs_s {
    E_RABReleaseItemBearerRelComp_t e_RABReleaseItemBearerRelComp;
} E_RABReleaseItemBearerRelCompIEs_t;

typedef struct OverloadStartIEs_s {
    OverloadResponse_t overloadResponse;
} OverloadStartIEs_t;

typedef struct NASNonDeliveryIndicationIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    NAS_PDU_t        nas_pdu;
    Cause_t          cause;
} NASNonDeliveryIndicationIEs_t;

#define HANDOVERCANCELACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct HandoverCancelAcknowledgeIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t         eNB_UE_S1AP_ID;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} HandoverCancelAcknowledgeIEs_t;

typedef struct ENBStatusTransferIEs_s {
    MME_UE_S1AP_ID_t                          mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                          eNB_UE_S1AP_ID;
    ENB_StatusTransfer_TransparentContainer_t eNB_StatusTransfer_TransparentContainer;
} ENBStatusTransferIEs_t;

typedef struct HandoverCancelIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    Cause_t          cause;
} HandoverCancelIEs_t;

typedef struct E_RABToBeModifiedItemBearerModReqIEs_s {
    E_RABToBeModifiedItemBearerModReq_t e_RABToBeModifiedItemBearerModReq;
} E_RABToBeModifiedItemBearerModReqIEs_t;

#define E_RABRELEASERESPONSEIES_E_RABRELEASELISTBEARERRELCOMP_PRESENT (1 << 0)
#define E_RABRELEASERESPONSEIES_E_RABLIST_PRESENT                     (1 << 1)
#define E_RABRELEASERESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT        (1 << 2)

typedef struct E_RABReleaseResponseIEs_s {
    uint16_t                        presenceMask;
    MME_UE_S1AP_ID_t                mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                eNB_UE_S1AP_ID;
    E_RABReleaseListBearerRelCompIEs_t e_RABReleaseListBearerRelComp; ///< Optional field
    E_RABListIEs_t e_RABFailedToReleaseList; ///< Optional field
    CriticalityDiagnostics_t        criticalityDiagnostics; ///< Optional field
} E_RABReleaseResponseIEs_t;

typedef struct UEContextReleaseRequestIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    Cause_t          cause;
} UEContextReleaseRequestIEs_t;

typedef struct TraceStartIEs_s {
    MME_UE_S1AP_ID_t  mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t  eNB_UE_S1AP_ID;
    TraceActivation_t traceActivation;
} TraceStartIEs_t;

#define MMECONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT (1 << 0)

typedef struct MMEConfigurationTransferIEs_s {
    uint16_t                   presenceMask;
    SONConfigurationTransfer_t sonConfigurationTransferMCT; ///< Optional field
} MMEConfigurationTransferIEs_t;

typedef struct TraceFailureIndicationIEs_s {
    MME_UE_S1AP_ID_t   mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t   eNB_UE_S1AP_ID;
    E_UTRAN_Trace_ID_t e_UTRAN_Trace_ID;
    Cause_t            cause;
} TraceFailureIndicationIEs_t;

#define INITIALCONTEXTSETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct InitialContextSetupFailureIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t         eNB_UE_S1AP_ID;
    Cause_t                  cause;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} InitialContextSetupFailureIEs_t;

#define ERRORINDICATIONIES_MME_UE_S1AP_ID_PRESENT         (1 << 0)
#define ERRORINDICATIONIES_ENB_UE_S1AP_ID_PRESENT         (1 << 1)
#define ERRORINDICATIONIES_CAUSE_PRESENT                  (1 << 2)
#define ERRORINDICATIONIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 3)

typedef struct ErrorIndicationIEs_s {
    uint16_t                 presenceMask;
    MME_UE_S1AP_ID_t         mme_ue_s1ap_id; ///< Optional field
    ENB_UE_S1AP_ID_t         eNB_UE_S1AP_ID; ///< Optional field
    Cause_t                  cause; ///< Optional field
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} ErrorIndicationIEs_t;

typedef struct HandoverNotifyIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    EUTRAN_CGI_t     eutran_cgi;
    TAI_t            tai;
} HandoverNotifyIEs_t;

#define DOWNLINKS1CDMA2000TUNNELINGIES_E_RABDATAFORWARDINGLIST_PRESENT (1 << 0)
#define DOWNLINKS1CDMA2000TUNNELINGIES_CDMA2000HOSTATUS_PRESENT        (1 << 1)

typedef struct DownlinkS1cdma2000tunnelingIEs_s {
    uint16_t                  presenceMask;
    MME_UE_S1AP_ID_t          mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t          eNB_UE_S1AP_ID;
    E_RABDataForwardingListIEs_t e_RABSubjecttoDataForwardingList; ///< Optional field
    Cdma2000HOStatus_t        cdma2000HOStatus; ///< Optional field
    Cdma2000RATType_t         cdma2000RATType;
    Cdma2000PDU_t             cdma2000PDU;
} DownlinkS1cdma2000tunnelingIEs_t;

#define MMECONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 0)

typedef struct MMEConfigurationUpdateAcknowledgeIEs_s {
    uint16_t                 presenceMask;
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} MMEConfigurationUpdateAcknowledgeIEs_t;

#define HANDOVERREQUESTACKNOWLEDGEIES_E_RABFAILEDTOSETUPLISTHOREQACK_PRESENT       (1 << 0)
#define HANDOVERREQUESTACKNOWLEDGEIES_CSG_ID_PRESENT                               (1 << 1)
#define HANDOVERREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT               (1 << 2)

typedef struct HandoverRequestAcknowledgeIEs_s {
    uint16_t                               presenceMask;
    MME_UE_S1AP_ID_t                       mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                       eNB_UE_S1AP_ID;
    E_RABAdmittedListIEs_t e_RABAdmittedList;
    E_RABFailedtoSetupListHOReqAckIEs_t e_RABFailedToSetupListHOReqAck; ///< Optional field
    Target_ToSource_TransparentContainer_t target_ToSource_TransparentContainer;
    CSG_Id_t                               csG_Id; ///< Optional field
    CriticalityDiagnostics_t               criticalityDiagnostics; ///< Optional field
} HandoverRequestAcknowledgeIEs_t;

#define E_RABSETUPREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT     (1 << 0)

typedef struct E_RABSetupRequestIEs_s {
    uint16_t                        presenceMask;
    MME_UE_S1AP_ID_t                mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                eNB_UE_S1AP_ID;
    UEAggregateMaximumBitrate_t     uEaggregateMaximumBitrate; ///< Optional field
    E_RABToBeSetupListBearerSUReqIEs_t e_RABToBeSetupListBearerSUReq;
} E_RABSetupRequestIEs_t;

typedef struct E_RABToBeSwitchedDLItemIEs_s {
    E_RABToBeSwitchedDLItem_t e_RABToBeSwitchedDLItem;
} E_RABToBeSwitchedDLItemIEs_t;

typedef struct Bearers_SubjectToStatusTransfer_ItemIEs_s {
    Bearers_SubjectToStatusTransfer_Item_t bearers_SubjectToStatusTransfer_Item;
} Bearers_SubjectToStatusTransfer_ItemIEs_t;

typedef struct DownlinkUEAssociatedLPPaTransportIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    Routing_ID_t     routing_ID;
    LPPa_PDU_t       lpPa_PDU;
} DownlinkUEAssociatedLPPaTransportIEs_t;

#define KILLREQUESTIES_WARNINGAREALIST_PRESENT   (1 << 0)

typedef struct KillRequestIEs_s {
    uint16_t            presenceMask;
    MessageIdentifier_t messageIdentifier;
    SerialNumber_t      serialNumber;
    WarningAreaList_t   warningAreaList; ///< Optional field
} KillRequestIEs_t;

typedef struct E_RABToBeSwitchedULItemIEs_s {
    E_RABToBeSwitchedULItem_t e_RABToBeSwitchedULItem;
} E_RABToBeSwitchedULItemIEs_t;

#define E_RABMODIFYRESPONSEIES_E_RABMODIFYLISTBEARERMODRES_PRESENT (1 << 0)
#define E_RABMODIFYRESPONSEIES_E_RABLIST_PRESENT                   (1 << 1)
#define E_RABMODIFYRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT      (1 << 2)

typedef struct E_RABModifyResponseIEs_s {
    uint16_t                      presenceMask;
    MME_UE_S1AP_ID_t              mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t              eNB_UE_S1AP_ID;
    E_RABModifyListBearerModResIEs_t e_RABModifyListBearerModRes; ///< Optional field
    E_RABListIEs_t e_RABFailedToModifyList; ///< Optional field
    CriticalityDiagnostics_t      criticalityDiagnostics; ///< Optional field
} E_RABModifyResponseIEs_t;

typedef struct E_RABDataForwardingItemIEs_s {
    E_RABDataForwardingItem_t e_RABDataForwardingItem;
} E_RABDataForwardingItemIEs_t;

typedef struct E_RABReleaseIndicationIEs_s {
    MME_UE_S1AP_ID_t mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t eNB_UE_S1AP_ID;
    E_RABListIEs_t e_RABReleasedList;
} E_RABReleaseIndicationIEs_t;

#define ENBCONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT             (1 << 0)
#define ENBCONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 1)

typedef struct ENBConfigurationUpdateFailureIEs_s {
    uint16_t                 presenceMask;
    Cause_t                  cause;
    TimeToWait_t             timeToWait; ///< Optional field
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} ENBConfigurationUpdateFailureIEs_t;

#define WRITEREPLACEWARNINGREQUESTIES_WARNINGAREALIST_PRESENT                   (1 << 0)
#define WRITEREPLACEWARNINGREQUESTIES_EXTENDEDREPETITIONPERIOD_PRESENT          (1 << 1)
#define WRITEREPLACEWARNINGREQUESTIES_WARNINGTYPE_PRESENT                       (1 << 2)
#define WRITEREPLACEWARNINGREQUESTIES_WARNINGSECURITYINFO_PRESENT               (1 << 3)
#define WRITEREPLACEWARNINGREQUESTIES_DATACODINGSCHEME_PRESENT                  (1 << 4)
#define WRITEREPLACEWARNINGREQUESTIES_WARNINGMESSAGECONTENTS_PRESENT            (1 << 5)
#define WRITEREPLACEWARNINGREQUESTIES_CONCURRENTWARNINGMESSAGEINDICATOR_PRESENT (1 << 6)

typedef struct WriteReplaceWarningRequestIEs_s {
    uint16_t                            presenceMask;
    MessageIdentifier_t                 messageIdentifier;
    SerialNumber_t                      serialNumber;
    WarningAreaList_t                   warningAreaList; ///< Optional field
    RepetitionPeriod_t                  repetitionPeriod;
    ExtendedRepetitionPeriod_t          extendedRepetitionPeriod; ///< Optional field
    NumberofBroadcastRequest_t          numberofBroadcastRequest;
    WarningType_t                       warningType; ///< Optional field
    WarningSecurityInfo_t               warningSecurityInfo; ///< Optional field
    DataCodingScheme_t                  dataCodingScheme; ///< Optional field
    WarningMessageContents_t            warningMessageContents; ///< Optional field
    ConcurrentWarningMessageIndicator_t concurrentWarningMessageIndicator; ///< Optional field
} WriteReplaceWarningRequestIEs_t;

typedef struct E_RABModifyItemBearerModResIEs_s {
    E_RABModifyItemBearerModRes_t e_RABModifyItemBearerModRes;
} E_RABModifyItemBearerModResIEs_t;

typedef struct UE_associatedLogicalS1_ConnectionItemRes_s {
    UE_associatedLogicalS1_ConnectionItem_t uE_associatedLogicalS1_ConnectionItem;
} UE_associatedLogicalS1_ConnectionItemRes_t;

typedef struct E_RABFailedtoSetupItemHOReqAckIEs_s {
    E_RABFailedToSetupItemHOReqAck_t e_RABFailedtoSetupItemHOReqAck;
} E_RABFailedtoSetupItemHOReqAckIEs_t;

#define S1SETUPFAILUREIES_TIMETOWAIT_PRESENT             (1 << 0)
#define S1SETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT (1 << 1)

typedef struct S1SetupFailureIEs_s {
    uint16_t                 presenceMask;
    Cause_t                  cause;
    TimeToWait_t             timeToWait; ///< Optional field
    CriticalityDiagnostics_t criticalityDiagnostics; ///< Optional field
} S1SetupFailureIEs_t;

#define E_RABMODIFYREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT         (1 << 0)

typedef struct E_RABModifyRequestIEs_s {
    uint16_t                            presenceMask;
    MME_UE_S1AP_ID_t                    mme_ue_s1ap_id;
    ENB_UE_S1AP_ID_t                    eNB_UE_S1AP_ID;
    UEAggregateMaximumBitrate_t         uEaggregateMaximumBitrate; ///< Optional field
    E_RABToBeModifiedListBearerModReqIEs_t e_RABToBeModifiedListBearerModReq;
} E_RABModifyRequestIEs_t;

typedef struct s1ap_message_s {
    uint8_t procedureCode;
    uint8_t criticality;
    uint8_t direction;
    union {
        MMEConfigurationUpdateIEs_t mmeConfigurationUpdateIEs;
        UEContextModificationRequestIEs_t ueContextModificationRequestIEs;
        ENBDirectInformationTransferIEs_t enbDirectInformationTransferIEs;
        ResetAcknowledgeIEs_t resetAcknowledgeIEs;
        PagingIEs_t pagingIEs;
        PathSwitchRequestIEs_t pathSwitchRequestIEs;
        DeactivateTraceIEs_t deactivateTraceIEs;
        LocationReportingControlIEs_t locationReportingControlIEs;
        ENBConfigurationUpdateAcknowledgeIEs_t enbConfigurationUpdateAcknowledgeIEs;
        HandoverFailureIEs_t handoverFailureIEs;
        LocationReportingFailureIndicationIEs_t locationReportingFailureIndicationIEs;
        DownlinkNASTransportIEs_t downlinkNASTransportIEs;
        MMEConfigurationUpdateFailureIEs_t mmeConfigurationUpdateFailureIEs;
        UEContextReleaseCompleteIEs_t ueContextReleaseCompleteIEs;
        S1SetupResponseIEs_t s1SetupResponseIEs;
        HandoverPreparationFailureIEs_t handoverPreparationFailureIEs;
        KillResponseIEs_t killResponseIEs;
        UplinkUEAssociatedLPPaTransportIEs_t uplinkUEAssociatedLPPaTransportIEs;
        ENBConfigurationUpdateIEs_t enbConfigurationUpdateIEs;
        InitialContextSetupRequestIEs_t initialContextSetupRequestIEs;
        MMEDirectInformationTransferIEs_t mmeDirectInformationTransferIEs;
        UplinkNonUEAssociatedLPPaTransportIEs_t uplinkNonUEAssociatedLPPaTransportIEs;
        ResetIEs_t resetIEs;
        UEContextReleaseCommandIEs_t ueContextReleaseCommandIEs;
        UECapabilityInfoIndicationIEs_t ueCapabilityInfoIndicationIEs;
        UplinkS1cdma2000tunnelingIEs_t uplinkS1cdma2000tunnelingIEs;
        WriteReplaceWarningResponseIEs_t writeReplaceWarningResponseIEs;
        E_RABReleaseCommandIEs_t e_RABReleaseCommandIEs;
        CellTrafficTraceIEs_t cellTrafficTraceIEs;
        PathSwitchRequestFailureIEs_t pathSwitchRequestFailureIEs;
        InitialContextSetupResponseIEs_t initialContextSetupResponseIEs;
        MMEStatusTransferIEs_t mmeStatusTransferIEs;
        S1SetupRequestIEs_t s1SetupRequestIEs;
        HandoverRequiredIEs_t handoverRequiredIEs;
        UplinkNASTransportIEs_t uplinkNASTransportIEs;
        LocationReportIEs_t locationReportIEs;
        UEContextModificationFailureIEs_t ueContextModificationFailureIEs;
        ENBConfigurationTransferIEs_t enbConfigurationTransferIEs;
        DownlinkNonUEAssociatedLPPaTransportIEs_t downlinkNonUEAssociatedLPPaTransportIEs;
        InitialUEMessageIEs_t initialUEMessageIEs;
        E_RABSetupResponseIEs_t e_RABSetupResponseIEs;
        PathSwitchRequestAcknowledgeIEs_t pathSwitchRequestAcknowledgeIEs;
        OverloadStartIEs_t overloadStartIEs;
        NASNonDeliveryIndicationIEs_t nasNonDeliveryIndicationIEs;
        HandoverCancelAcknowledgeIEs_t handoverCancelAcknowledgeIEs;
        ENBStatusTransferIEs_t enbStatusTransferIEs;
        HandoverCancelIEs_t handoverCancelIEs;
        E_RABReleaseResponseIEs_t e_RABReleaseResponseIEs;
        UEContextReleaseRequestIEs_t ueContextReleaseRequestIEs;
        TraceStartIEs_t traceStartIEs;
        MMEConfigurationTransferIEs_t mmeConfigurationTransferIEs;
        TraceFailureIndicationIEs_t traceFailureIndicationIEs;
        InitialContextSetupFailureIEs_t initialContextSetupFailureIEs;
        ErrorIndicationIEs_t errorIndicationIEs;
        HandoverNotifyIEs_t handoverNotifyIEs;
        DownlinkS1cdma2000tunnelingIEs_t downlinkS1cdma2000tunnelingIEs;
        MMEConfigurationUpdateAcknowledgeIEs_t mmeConfigurationUpdateAcknowledgeIEs;
        HandoverRequestAcknowledgeIEs_t handoverRequestAcknowledgeIEs;
        E_RABSetupRequestIEs_t e_RABSetupRequestIEs;
        DownlinkUEAssociatedLPPaTransportIEs_t downlinkUEAssociatedLPPaTransportIEs;
        KillRequestIEs_t killRequestIEs;
        E_RABModifyResponseIEs_t e_RABModifyResponseIEs;
        E_RABReleaseIndicationIEs_t e_RABReleaseIndicationIEs;
        ENBConfigurationUpdateFailureIEs_t enbConfigurationUpdateFailureIEs;
        WriteReplaceWarningRequestIEs_t writeReplaceWarningRequestIEs;
        S1SetupFailureIEs_t s1SetupFailureIEs;
        E_RABModifyRequestIEs_t e_RABModifyRequestIEs;
    } msg;
} s1ap_message;

/** \brief Decode function for MMEConfigurationUpdateIEs ies.
 * \param mmeConfigurationUpdateIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_mmeconfigurationupdateies(
    MMEConfigurationUpdateIEs_t *mmeConfigurationUpdateIEs,
    ANY_t *any_p);

/** \brief Encode function for MMEConfigurationUpdateIEs ies.
 *  \param mmeConfigurationUpdate Pointer to the ASN1 structure.
 *  \param mmeConfigurationUpdateIEs Pointer to the IES structure.
 **/
int s1ap_encode_mmeconfigurationupdateies(
    MMEConfigurationUpdate_t *mmeConfigurationUpdate,
    MMEConfigurationUpdateIEs_t *mmeConfigurationUpdateIEs);

/** \brief Decode function for UEContextModificationRequestIEs ies.
 * \param ueContextModificationRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uecontextmodificationrequesties(
    UEContextModificationRequestIEs_t *ueContextModificationRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for UEContextModificationRequestIEs ies.
 *  \param ueContextModificationRequest Pointer to the ASN1 structure.
 *  \param ueContextModificationRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_uecontextmodificationrequesties(
    UEContextModificationRequest_t *ueContextModificationRequest,
    UEContextModificationRequestIEs_t *ueContextModificationRequestIEs);

/** \brief Decode function for ENBDirectInformationTransferIEs ies.
 * \param enbDirectInformationTransferIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_enbdirectinformationtransferies(
    ENBDirectInformationTransferIEs_t *enbDirectInformationTransferIEs,
    ANY_t *any_p);

/** \brief Encode function for ENBDirectInformationTransferIEs ies.
 *  \param eNBDirectInformationTransfer Pointer to the ASN1 structure.
 *  \param enbDirectInformationTransferIEs Pointer to the IES structure.
 **/
int s1ap_encode_enbdirectinformationtransferies(
    ENBDirectInformationTransfer_t *eNBDirectInformationTransfer,
    ENBDirectInformationTransferIEs_t *enbDirectInformationTransferIEs);

/** \brief Decode function for ResetAcknowledgeIEs ies.
 * \param resetAcknowledgeIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_resetacknowledgeies(
    ResetAcknowledgeIEs_t *resetAcknowledgeIEs,
    ANY_t *any_p);

/** \brief Encode function for ResetAcknowledgeIEs ies.
 *  \param resetAcknowledge Pointer to the ASN1 structure.
 *  \param resetAcknowledgeIEs Pointer to the IES structure.
 **/
int s1ap_encode_resetacknowledgeies(
    ResetAcknowledge_t *resetAcknowledge,
    ResetAcknowledgeIEs_t *resetAcknowledgeIEs);

/** \brief Decode function for PagingIEs ies.
 * \param pagingIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_pagingies(
    PagingIEs_t *pagingIEs,
    ANY_t *any_p);

/** \brief Encode function for PagingIEs ies.
 *  \param paging Pointer to the ASN1 structure.
 *  \param pagingIEs Pointer to the IES structure.
 **/
int s1ap_encode_pagingies(
    Paging_t *paging,
    PagingIEs_t *pagingIEs);

/** \brief Decode function for PathSwitchRequestIEs ies.
 * \param pathSwitchRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_pathswitchrequesties(
    PathSwitchRequestIEs_t *pathSwitchRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for PathSwitchRequestIEs ies.
 *  \param pathSwitchRequest Pointer to the ASN1 structure.
 *  \param pathSwitchRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_pathswitchrequesties(
    PathSwitchRequest_t *pathSwitchRequest,
    PathSwitchRequestIEs_t *pathSwitchRequestIEs);

/** \brief Decode function for DeactivateTraceIEs ies.
 * \param deactivateTraceIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_deactivatetraceies(
    DeactivateTraceIEs_t *deactivateTraceIEs,
    ANY_t *any_p);

/** \brief Encode function for DeactivateTraceIEs ies.
 *  \param deactivateTrace Pointer to the ASN1 structure.
 *  \param deactivateTraceIEs Pointer to the IES structure.
 **/
int s1ap_encode_deactivatetraceies(
    DeactivateTrace_t *deactivateTrace,
    DeactivateTraceIEs_t *deactivateTraceIEs);

/** \brief Decode function for LocationReportingControlIEs ies.
 * \param locationReportingControlIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_locationreportingcontrolies(
    LocationReportingControlIEs_t *locationReportingControlIEs,
    ANY_t *any_p);

/** \brief Encode function for LocationReportingControlIEs ies.
 *  \param locationReportingControl Pointer to the ASN1 structure.
 *  \param locationReportingControlIEs Pointer to the IES structure.
 **/
int s1ap_encode_locationreportingcontrolies(
    LocationReportingControl_t *locationReportingControl,
    LocationReportingControlIEs_t *locationReportingControlIEs);

/** \brief Decode function for ENBConfigurationUpdateAcknowledgeIEs ies.
 * \param enbConfigurationUpdateAcknowledgeIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_enbconfigurationupdateacknowledgeies(
    ENBConfigurationUpdateAcknowledgeIEs_t *enbConfigurationUpdateAcknowledgeIEs,
    ANY_t *any_p);

/** \brief Encode function for ENBConfigurationUpdateAcknowledgeIEs ies.
 *  \param eNBConfigurationUpdateAcknowledge Pointer to the ASN1 structure.
 *  \param enbConfigurationUpdateAcknowledgeIEs Pointer to the IES structure.
 **/
int s1ap_encode_enbconfigurationupdateacknowledgeies(
    ENBConfigurationUpdateAcknowledge_t *eNBConfigurationUpdateAcknowledge,
    ENBConfigurationUpdateAcknowledgeIEs_t *enbConfigurationUpdateAcknowledgeIEs);

/** \brief Decode function for HandoverFailureIEs ies.
 * \param handoverFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_handoverfailureies(
    HandoverFailureIEs_t *handoverFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for HandoverFailureIEs ies.
 *  \param handoverFailure Pointer to the ASN1 structure.
 *  \param handoverFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_handoverfailureies(
    HandoverFailure_t *handoverFailure,
    HandoverFailureIEs_t *handoverFailureIEs);

/** \brief Decode function for LocationReportingFailureIndicationIEs ies.
 * \param locationReportingFailureIndicationIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_locationreportingfailureindicationies(
    LocationReportingFailureIndicationIEs_t *locationReportingFailureIndicationIEs,
    ANY_t *any_p);

/** \brief Encode function for LocationReportingFailureIndicationIEs ies.
 *  \param locationReportingFailureIndication Pointer to the ASN1 structure.
 *  \param locationReportingFailureIndicationIEs Pointer to the IES structure.
 **/
int s1ap_encode_locationreportingfailureindicationies(
    LocationReportingFailureIndication_t *locationReportingFailureIndication,
    LocationReportingFailureIndicationIEs_t *locationReportingFailureIndicationIEs);

/** \brief Decode function for DownlinkNASTransportIEs ies.
 * \param downlinkNASTransportIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_downlinknastransporties(
    DownlinkNASTransportIEs_t *downlinkNASTransportIEs,
    ANY_t *any_p);

/** \brief Encode function for DownlinkNASTransportIEs ies.
 *  \param downlinkNASTransport Pointer to the ASN1 structure.
 *  \param downlinkNASTransportIEs Pointer to the IES structure.
 **/
int s1ap_encode_downlinknastransporties(
    DownlinkNASTransport_t *downlinkNASTransport,
    DownlinkNASTransportIEs_t *downlinkNASTransportIEs);

/** \brief Decode function for MMEConfigurationUpdateFailureIEs ies.
 * \param mmeConfigurationUpdateFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_mmeconfigurationupdatefailureies(
    MMEConfigurationUpdateFailureIEs_t *mmeConfigurationUpdateFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for MMEConfigurationUpdateFailureIEs ies.
 *  \param mmeConfigurationUpdateFailure Pointer to the ASN1 structure.
 *  \param mmeConfigurationUpdateFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_mmeconfigurationupdatefailureies(
    MMEConfigurationUpdateFailure_t *mmeConfigurationUpdateFailure,
    MMEConfigurationUpdateFailureIEs_t *mmeConfigurationUpdateFailureIEs);

/** \brief Decode function for UEContextReleaseCompleteIEs ies.
 * \param ueContextReleaseCompleteIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uecontextreleasecompleteies(
    UEContextReleaseCompleteIEs_t *ueContextReleaseCompleteIEs,
    ANY_t *any_p);

/** \brief Encode function for UEContextReleaseCompleteIEs ies.
 *  \param ueContextReleaseComplete Pointer to the ASN1 structure.
 *  \param ueContextReleaseCompleteIEs Pointer to the IES structure.
 **/
int s1ap_encode_uecontextreleasecompleteies(
    UEContextReleaseComplete_t *ueContextReleaseComplete,
    UEContextReleaseCompleteIEs_t *ueContextReleaseCompleteIEs);

/** \brief Decode function for S1SetupResponseIEs ies.
 * \param s1SetupResponseIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_s1setupresponseies(
    S1SetupResponseIEs_t *s1SetupResponseIEs,
    ANY_t *any_p);

/** \brief Encode function for S1SetupResponseIEs ies.
 *  \param s1SetupResponse Pointer to the ASN1 structure.
 *  \param s1SetupResponseIEs Pointer to the IES structure.
 **/
int s1ap_encode_s1setupresponseies(
    S1SetupResponse_t *s1SetupResponse,
    S1SetupResponseIEs_t *s1SetupResponseIEs);

/** \brief Decode function for HandoverPreparationFailureIEs ies.
 * \param handoverPreparationFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_handoverpreparationfailureies(
    HandoverPreparationFailureIEs_t *handoverPreparationFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for HandoverPreparationFailureIEs ies.
 *  \param handoverPreparationFailure Pointer to the ASN1 structure.
 *  \param handoverPreparationFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_handoverpreparationfailureies(
    HandoverPreparationFailure_t *handoverPreparationFailure,
    HandoverPreparationFailureIEs_t *handoverPreparationFailureIEs);

/** \brief Decode function for KillResponseIEs ies.
 * \param killResponseIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_killresponseies(
    KillResponseIEs_t *killResponseIEs,
    ANY_t *any_p);

/** \brief Encode function for KillResponseIEs ies.
 *  \param killResponse Pointer to the ASN1 structure.
 *  \param killResponseIEs Pointer to the IES structure.
 **/
int s1ap_encode_killresponseies(
    KillResponse_t *killResponse,
    KillResponseIEs_t *killResponseIEs);

/** \brief Decode function for UplinkUEAssociatedLPPaTransportIEs ies.
 * \param uplinkUEAssociatedLPPaTransportIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uplinkueassociatedlppatransporties(
    UplinkUEAssociatedLPPaTransportIEs_t *uplinkUEAssociatedLPPaTransportIEs,
    ANY_t *any_p);

/** \brief Encode function for UplinkUEAssociatedLPPaTransportIEs ies.
 *  \param uplinkUEAssociatedLPPaTransport Pointer to the ASN1 structure.
 *  \param uplinkUEAssociatedLPPaTransportIEs Pointer to the IES structure.
 **/
int s1ap_encode_uplinkueassociatedlppatransporties(
    UplinkUEAssociatedLPPaTransport_t *uplinkUEAssociatedLPPaTransport,
    UplinkUEAssociatedLPPaTransportIEs_t *uplinkUEAssociatedLPPaTransportIEs);

/** \brief Decode function for ENBConfigurationUpdateIEs ies.
 * \param enbConfigurationUpdateIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_enbconfigurationupdateies(
    ENBConfigurationUpdateIEs_t *enbConfigurationUpdateIEs,
    ANY_t *any_p);

/** \brief Encode function for ENBConfigurationUpdateIEs ies.
 *  \param eNBConfigurationUpdate Pointer to the ASN1 structure.
 *  \param enbConfigurationUpdateIEs Pointer to the IES structure.
 **/
int s1ap_encode_enbconfigurationupdateies(
    ENBConfigurationUpdate_t *eNBConfigurationUpdate,
    ENBConfigurationUpdateIEs_t *enbConfigurationUpdateIEs);

/** \brief Decode function for InitialContextSetupRequestIEs ies.
 * \param initialContextSetupRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_initialcontextsetuprequesties(
    InitialContextSetupRequestIEs_t *initialContextSetupRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for InitialContextSetupRequestIEs ies.
 *  \param initialContextSetupRequest Pointer to the ASN1 structure.
 *  \param initialContextSetupRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_initialcontextsetuprequesties(
    InitialContextSetupRequest_t *initialContextSetupRequest,
    InitialContextSetupRequestIEs_t *initialContextSetupRequestIEs);

/** \brief Decode function for MMEDirectInformationTransferIEs ies.
 * \param mmeDirectInformationTransferIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_mmedirectinformationtransferies(
    MMEDirectInformationTransferIEs_t *mmeDirectInformationTransferIEs,
    ANY_t *any_p);

/** \brief Encode function for MMEDirectInformationTransferIEs ies.
 *  \param mmeDirectInformationTransfer Pointer to the ASN1 structure.
 *  \param mmeDirectInformationTransferIEs Pointer to the IES structure.
 **/
int s1ap_encode_mmedirectinformationtransferies(
    MMEDirectInformationTransfer_t *mmeDirectInformationTransfer,
    MMEDirectInformationTransferIEs_t *mmeDirectInformationTransferIEs);

/** \brief Decode function for UplinkNonUEAssociatedLPPaTransportIEs ies.
 * \param uplinkNonUEAssociatedLPPaTransportIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uplinknonueassociatedlppatransporties(
    UplinkNonUEAssociatedLPPaTransportIEs_t *uplinkNonUEAssociatedLPPaTransportIEs,
    ANY_t *any_p);

/** \brief Encode function for UplinkNonUEAssociatedLPPaTransportIEs ies.
 *  \param uplinkNonUEAssociatedLPPaTransport Pointer to the ASN1 structure.
 *  \param uplinkNonUEAssociatedLPPaTransportIEs Pointer to the IES structure.
 **/
int s1ap_encode_uplinknonueassociatedlppatransporties(
    UplinkNonUEAssociatedLPPaTransport_t *uplinkNonUEAssociatedLPPaTransport,
    UplinkNonUEAssociatedLPPaTransportIEs_t *uplinkNonUEAssociatedLPPaTransportIEs);

/** \brief Decode function for ResetIEs ies.
 * \param resetIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_reseties(
    ResetIEs_t *resetIEs,
    ANY_t *any_p);

/** \brief Encode function for ResetIEs ies.
 *  \param reset Pointer to the ASN1 structure.
 *  \param resetIEs Pointer to the IES structure.
 **/
int s1ap_encode_reseties(
    Reset_t *reset,
    ResetIEs_t *resetIEs);

/** \brief Decode function for UEContextReleaseCommandIEs ies.
 * \param ueContextReleaseCommandIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uecontextreleasecommandies(
    UEContextReleaseCommandIEs_t *ueContextReleaseCommandIEs,
    ANY_t *any_p);

/** \brief Encode function for UEContextReleaseCommandIEs ies.
 *  \param ueContextReleaseCommand Pointer to the ASN1 structure.
 *  \param ueContextReleaseCommandIEs Pointer to the IES structure.
 **/
int s1ap_encode_uecontextreleasecommandies(
    UEContextReleaseCommand_t *ueContextReleaseCommand,
    UEContextReleaseCommandIEs_t *ueContextReleaseCommandIEs);

/** \brief Decode function for UECapabilityInfoIndicationIEs ies.
 * \param ueCapabilityInfoIndicationIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uecapabilityinfoindicationies(
    UECapabilityInfoIndicationIEs_t *ueCapabilityInfoIndicationIEs,
    ANY_t *any_p);

/** \brief Encode function for UECapabilityInfoIndicationIEs ies.
 *  \param ueCapabilityInfoIndication Pointer to the ASN1 structure.
 *  \param ueCapabilityInfoIndicationIEs Pointer to the IES structure.
 **/
int s1ap_encode_uecapabilityinfoindicationies(
    UECapabilityInfoIndication_t *ueCapabilityInfoIndication,
    UECapabilityInfoIndicationIEs_t *ueCapabilityInfoIndicationIEs);

/** \brief Decode function for UplinkS1cdma2000tunnelingIEs ies.
 * \param uplinkS1cdma2000tunnelingIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uplinks1cdma2000tunnelingies(
    UplinkS1cdma2000tunnelingIEs_t *uplinkS1cdma2000tunnelingIEs,
    ANY_t *any_p);

/** \brief Encode function for UplinkS1cdma2000tunnelingIEs ies.
 *  \param uplinkS1cdma2000tunneling Pointer to the ASN1 structure.
 *  \param uplinkS1cdma2000tunnelingIEs Pointer to the IES structure.
 **/
int s1ap_encode_uplinks1cdma2000tunnelingies(
    UplinkS1cdma2000tunneling_t *uplinkS1cdma2000tunneling,
    UplinkS1cdma2000tunnelingIEs_t *uplinkS1cdma2000tunnelingIEs);

/** \brief Decode function for OverloadStopIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_overloadstopies(
    ANY_t *any_p);

/** \brief Decode function for WriteReplaceWarningResponseIEs ies.
 * \param writeReplaceWarningResponseIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_writereplacewarningresponseies(
    WriteReplaceWarningResponseIEs_t *writeReplaceWarningResponseIEs,
    ANY_t *any_p);

/** \brief Encode function for WriteReplaceWarningResponseIEs ies.
 *  \param writeReplaceWarningResponse Pointer to the ASN1 structure.
 *  \param writeReplaceWarningResponseIEs Pointer to the IES structure.
 **/
int s1ap_encode_writereplacewarningresponseies(
    WriteReplaceWarningResponse_t *writeReplaceWarningResponse,
    WriteReplaceWarningResponseIEs_t *writeReplaceWarningResponseIEs);

/** \brief Decode function for E-RABReleaseCommandIEs ies.
 * \param e_RABReleaseCommandIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_e_rabreleasecommandies(
    E_RABReleaseCommandIEs_t *e_RABReleaseCommandIEs,
    ANY_t *any_p);

/** \brief Encode function for E-RABReleaseCommandIEs ies.
 *  \param e_RABReleaseCommand Pointer to the ASN1 structure.
 *  \param e_RABReleaseCommandIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabreleasecommandies(
    E_RABReleaseCommand_t *e_RABReleaseCommand,
    E_RABReleaseCommandIEs_t *e_RABReleaseCommandIEs);

/** \brief Decode function for CellTrafficTraceIEs ies.
 * \param cellTrafficTraceIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_celltraffictraceies(
    CellTrafficTraceIEs_t *cellTrafficTraceIEs,
    ANY_t *any_p);

/** \brief Encode function for CellTrafficTraceIEs ies.
 *  \param cellTrafficTrace Pointer to the ASN1 structure.
 *  \param cellTrafficTraceIEs Pointer to the IES structure.
 **/
int s1ap_encode_celltraffictraceies(
    CellTrafficTrace_t *cellTrafficTrace,
    CellTrafficTraceIEs_t *cellTrafficTraceIEs);

/** \brief Decode function for PathSwitchRequestFailureIEs ies.
 * \param pathSwitchRequestFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_pathswitchrequestfailureies(
    PathSwitchRequestFailureIEs_t *pathSwitchRequestFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for PathSwitchRequestFailureIEs ies.
 *  \param pathSwitchRequestFailure Pointer to the ASN1 structure.
 *  \param pathSwitchRequestFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_pathswitchrequestfailureies(
    PathSwitchRequestFailure_t *pathSwitchRequestFailure,
    PathSwitchRequestFailureIEs_t *pathSwitchRequestFailureIEs);

/** \brief Decode function for InitialContextSetupResponseIEs ies.
 * \param initialContextSetupResponseIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_initialcontextsetupresponseies(
    InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs,
    ANY_t *any_p);

/** \brief Encode function for InitialContextSetupResponseIEs ies.
 *  \param initialContextSetupResponse Pointer to the ASN1 structure.
 *  \param initialContextSetupResponseIEs Pointer to the IES structure.
 **/
int s1ap_encode_initialcontextsetupresponseies(
    InitialContextSetupResponse_t *initialContextSetupResponse,
    InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs);

/** \brief Decode function for MMEStatusTransferIEs ies.
 * \param mmeStatusTransferIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_mmestatustransferies(
    MMEStatusTransferIEs_t *mmeStatusTransferIEs,
    ANY_t *any_p);

/** \brief Encode function for MMEStatusTransferIEs ies.
 *  \param mmeStatusTransfer Pointer to the ASN1 structure.
 *  \param mmeStatusTransferIEs Pointer to the IES structure.
 **/
int s1ap_encode_mmestatustransferies(
    MMEStatusTransfer_t *mmeStatusTransfer,
    MMEStatusTransferIEs_t *mmeStatusTransferIEs);

/** \brief Decode function for S1SetupRequestIEs ies.
 * \param s1SetupRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_s1setuprequesties(
    S1SetupRequestIEs_t *s1SetupRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for S1SetupRequestIEs ies.
 *  \param s1SetupRequest Pointer to the ASN1 structure.
 *  \param s1SetupRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_s1setuprequesties(
    S1SetupRequest_t *s1SetupRequest,
    S1SetupRequestIEs_t *s1SetupRequestIEs);

/** \brief Decode function for HandoverRequiredIEs ies.
 * \param handoverRequiredIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_handoverrequiredies(
    HandoverRequiredIEs_t *handoverRequiredIEs,
    ANY_t *any_p);

/** \brief Encode function for HandoverRequiredIEs ies.
 *  \param handoverRequired Pointer to the ASN1 structure.
 *  \param handoverRequiredIEs Pointer to the IES structure.
 **/
int s1ap_encode_handoverrequiredies(
    HandoverRequired_t *handoverRequired,
    HandoverRequiredIEs_t *handoverRequiredIEs);

/** \brief Decode function for UplinkNASTransportIEs ies.
 * \param uplinkNASTransportIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uplinknastransporties(
    UplinkNASTransportIEs_t *uplinkNASTransportIEs,
    ANY_t *any_p);

/** \brief Encode function for UplinkNASTransportIEs ies.
 *  \param uplinkNASTransport Pointer to the ASN1 structure.
 *  \param uplinkNASTransportIEs Pointer to the IES structure.
 **/
int s1ap_encode_uplinknastransporties(
    UplinkNASTransport_t *uplinkNASTransport,
    UplinkNASTransportIEs_t *uplinkNASTransportIEs);

/** \brief Decode function for LocationReportIEs ies.
 * \param locationReportIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_locationreporties(
    LocationReportIEs_t *locationReportIEs,
    ANY_t *any_p);

/** \brief Encode function for LocationReportIEs ies.
 *  \param locationReport Pointer to the ASN1 structure.
 *  \param locationReportIEs Pointer to the IES structure.
 **/
int s1ap_encode_locationreporties(
    LocationReport_t *locationReport,
    LocationReportIEs_t *locationReportIEs);

/** \brief Decode function for UEContextModificationFailureIEs ies.
 * \param ueContextModificationFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uecontextmodificationfailureies(
    UEContextModificationFailureIEs_t *ueContextModificationFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for UEContextModificationFailureIEs ies.
 *  \param ueContextModificationFailure Pointer to the ASN1 structure.
 *  \param ueContextModificationFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_uecontextmodificationfailureies(
    UEContextModificationFailure_t *ueContextModificationFailure,
    UEContextModificationFailureIEs_t *ueContextModificationFailureIEs);

/** \brief Decode function for ENBConfigurationTransferIEs ies.
 * \param enbConfigurationTransferIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_enbconfigurationtransferies(
    ENBConfigurationTransferIEs_t *enbConfigurationTransferIEs,
    ANY_t *any_p);

/** \brief Encode function for ENBConfigurationTransferIEs ies.
 *  \param eNBConfigurationTransfer Pointer to the ASN1 structure.
 *  \param enbConfigurationTransferIEs Pointer to the IES structure.
 **/
int s1ap_encode_enbconfigurationtransferies(
    ENBConfigurationTransfer_t *eNBConfigurationTransfer,
    ENBConfigurationTransferIEs_t *enbConfigurationTransferIEs);

/** \brief Decode function for DownlinkNonUEAssociatedLPPaTransportIEs ies.
 * \param downlinkNonUEAssociatedLPPaTransportIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_downlinknonueassociatedlppatransporties(
    DownlinkNonUEAssociatedLPPaTransportIEs_t *downlinkNonUEAssociatedLPPaTransportIEs,
    ANY_t *any_p);

/** \brief Encode function for DownlinkNonUEAssociatedLPPaTransportIEs ies.
 *  \param downlinkNonUEAssociatedLPPaTransport Pointer to the ASN1 structure.
 *  \param downlinkNonUEAssociatedLPPaTransportIEs Pointer to the IES structure.
 **/
int s1ap_encode_downlinknonueassociatedlppatransporties(
    DownlinkNonUEAssociatedLPPaTransport_t *downlinkNonUEAssociatedLPPaTransport,
    DownlinkNonUEAssociatedLPPaTransportIEs_t *downlinkNonUEAssociatedLPPaTransportIEs);

/** \brief Decode function for InitialUEMessageIEs ies.
 * \param initialUEMessageIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_initialuemessageies(
    InitialUEMessageIEs_t *initialUEMessageIEs,
    ANY_t *any_p);

/** \brief Encode function for InitialUEMessageIEs ies.
 *  \param initialUEMessage Pointer to the ASN1 structure.
 *  \param initialUEMessageIEs Pointer to the IES structure.
 **/
int s1ap_encode_initialuemessageies(
    InitialUEMessage_t *initialUEMessage,
    InitialUEMessageIEs_t *initialUEMessageIEs);

/** \brief Decode function for E-RABSetupResponseIEs ies.
 * \param e_RABSetupResponseIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_e_rabsetupresponseies(
    E_RABSetupResponseIEs_t *e_RABSetupResponseIEs,
    ANY_t *any_p);

/** \brief Encode function for E-RABSetupResponseIEs ies.
 *  \param e_RABSetupResponse Pointer to the ASN1 structure.
 *  \param e_RABSetupResponseIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabsetupresponseies(
    E_RABSetupResponse_t *e_RABSetupResponse,
    E_RABSetupResponseIEs_t *e_RABSetupResponseIEs);

/** \brief Decode function for PathSwitchRequestAcknowledgeIEs ies.
 * \param pathSwitchRequestAcknowledgeIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_pathswitchrequestacknowledgeies(
    PathSwitchRequestAcknowledgeIEs_t *pathSwitchRequestAcknowledgeIEs,
    ANY_t *any_p);

/** \brief Encode function for PathSwitchRequestAcknowledgeIEs ies.
 *  \param pathSwitchRequestAcknowledge Pointer to the ASN1 structure.
 *  \param pathSwitchRequestAcknowledgeIEs Pointer to the IES structure.
 **/
int s1ap_encode_pathswitchrequestacknowledgeies(
    PathSwitchRequestAcknowledge_t *pathSwitchRequestAcknowledge,
    PathSwitchRequestAcknowledgeIEs_t *pathSwitchRequestAcknowledgeIEs);

/** \brief Decode function for OverloadStartIEs ies.
 * \param overloadStartIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_overloadstarties(
    OverloadStartIEs_t *overloadStartIEs,
    ANY_t *any_p);

/** \brief Encode function for OverloadStartIEs ies.
 *  \param overloadStart Pointer to the ASN1 structure.
 *  \param overloadStartIEs Pointer to the IES structure.
 **/
int s1ap_encode_overloadstarties(
    OverloadStart_t *overloadStart,
    OverloadStartIEs_t *overloadStartIEs);

/** \brief Decode function for NASNonDeliveryIndicationIEs ies.
 * \param nasNonDeliveryIndicationIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_nasnondeliveryindicationies(
    NASNonDeliveryIndicationIEs_t *nasNonDeliveryIndicationIEs,
    ANY_t *any_p);

/** \brief Encode function for NASNonDeliveryIndicationIEs ies.
 *  \param nasNonDeliveryIndication Pointer to the ASN1 structure.
 *  \param nasNonDeliveryIndicationIEs Pointer to the IES structure.
 **/
int s1ap_encode_nasnondeliveryindicationies(
    NASNonDeliveryIndication_t *nasNonDeliveryIndication,
    NASNonDeliveryIndicationIEs_t *nasNonDeliveryIndicationIEs);

/** \brief Decode function for HandoverCancelAcknowledgeIEs ies.
 * \param handoverCancelAcknowledgeIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_handovercancelacknowledgeies(
    HandoverCancelAcknowledgeIEs_t *handoverCancelAcknowledgeIEs,
    ANY_t *any_p);

/** \brief Encode function for HandoverCancelAcknowledgeIEs ies.
 *  \param handoverCancelAcknowledge Pointer to the ASN1 structure.
 *  \param handoverCancelAcknowledgeIEs Pointer to the IES structure.
 **/
int s1ap_encode_handovercancelacknowledgeies(
    HandoverCancelAcknowledge_t *handoverCancelAcknowledge,
    HandoverCancelAcknowledgeIEs_t *handoverCancelAcknowledgeIEs);

/** \brief Decode function for ENBStatusTransferIEs ies.
 * \param enbStatusTransferIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_enbstatustransferies(
    ENBStatusTransferIEs_t *enbStatusTransferIEs,
    ANY_t *any_p);

/** \brief Encode function for ENBStatusTransferIEs ies.
 *  \param eNBStatusTransfer Pointer to the ASN1 structure.
 *  \param enbStatusTransferIEs Pointer to the IES structure.
 **/
int s1ap_encode_enbstatustransferies(
    ENBStatusTransfer_t *eNBStatusTransfer,
    ENBStatusTransferIEs_t *enbStatusTransferIEs);

/** \brief Decode function for HandoverCancelIEs ies.
 * \param handoverCancelIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_handovercancelies(
    HandoverCancelIEs_t *handoverCancelIEs,
    ANY_t *any_p);

/** \brief Encode function for HandoverCancelIEs ies.
 *  \param handoverCancel Pointer to the ASN1 structure.
 *  \param handoverCancelIEs Pointer to the IES structure.
 **/
int s1ap_encode_handovercancelies(
    HandoverCancel_t *handoverCancel,
    HandoverCancelIEs_t *handoverCancelIEs);

/** \brief Decode function for E-RABReleaseResponseIEs ies.
 * \param e_RABReleaseResponseIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_e_rabreleaseresponseies(
    E_RABReleaseResponseIEs_t *e_RABReleaseResponseIEs,
    ANY_t *any_p);

/** \brief Encode function for E-RABReleaseResponseIEs ies.
 *  \param e_RABReleaseResponse Pointer to the ASN1 structure.
 *  \param e_RABReleaseResponseIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabreleaseresponseies(
    E_RABReleaseResponse_t *e_RABReleaseResponse,
    E_RABReleaseResponseIEs_t *e_RABReleaseResponseIEs);

/** \brief Decode function for UEContextReleaseRequestIEs ies.
 * \param ueContextReleaseRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_uecontextreleaserequesties(
    UEContextReleaseRequestIEs_t *ueContextReleaseRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for UEContextReleaseRequestIEs ies.
 *  \param ueContextReleaseRequest Pointer to the ASN1 structure.
 *  \param ueContextReleaseRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_uecontextreleaserequesties(
    UEContextReleaseRequest_t *ueContextReleaseRequest,
    UEContextReleaseRequestIEs_t *ueContextReleaseRequestIEs);

/** \brief Decode function for TraceStartIEs ies.
 * \param traceStartIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_tracestarties(
    TraceStartIEs_t *traceStartIEs,
    ANY_t *any_p);

/** \brief Encode function for TraceStartIEs ies.
 *  \param traceStart Pointer to the ASN1 structure.
 *  \param traceStartIEs Pointer to the IES structure.
 **/
int s1ap_encode_tracestarties(
    TraceStart_t *traceStart,
    TraceStartIEs_t *traceStartIEs);

/** \brief Decode function for MMEConfigurationTransferIEs ies.
 * \param mmeConfigurationTransferIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_mmeconfigurationtransferies(
    MMEConfigurationTransferIEs_t *mmeConfigurationTransferIEs,
    ANY_t *any_p);

/** \brief Encode function for MMEConfigurationTransferIEs ies.
 *  \param mmeConfigurationTransfer Pointer to the ASN1 structure.
 *  \param mmeConfigurationTransferIEs Pointer to the IES structure.
 **/
int s1ap_encode_mmeconfigurationtransferies(
    MMEConfigurationTransfer_t *mmeConfigurationTransfer,
    MMEConfigurationTransferIEs_t *mmeConfigurationTransferIEs);

/** \brief Decode function for TraceFailureIndicationIEs ies.
 * \param traceFailureIndicationIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_tracefailureindicationies(
    TraceFailureIndicationIEs_t *traceFailureIndicationIEs,
    ANY_t *any_p);

/** \brief Encode function for TraceFailureIndicationIEs ies.
 *  \param traceFailureIndication Pointer to the ASN1 structure.
 *  \param traceFailureIndicationIEs Pointer to the IES structure.
 **/
int s1ap_encode_tracefailureindicationies(
    TraceFailureIndication_t *traceFailureIndication,
    TraceFailureIndicationIEs_t *traceFailureIndicationIEs);

/** \brief Decode function for InitialContextSetupFailureIEs ies.
 * \param initialContextSetupFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_initialcontextsetupfailureies(
    InitialContextSetupFailureIEs_t *initialContextSetupFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for InitialContextSetupFailureIEs ies.
 *  \param initialContextSetupFailure Pointer to the ASN1 structure.
 *  \param initialContextSetupFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_initialcontextsetupfailureies(
    InitialContextSetupFailure_t *initialContextSetupFailure,
    InitialContextSetupFailureIEs_t *initialContextSetupFailureIEs);

/** \brief Decode function for ErrorIndicationIEs ies.
 * \param errorIndicationIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_errorindicationies(
    ErrorIndicationIEs_t *errorIndicationIEs,
    ANY_t *any_p);

/** \brief Encode function for ErrorIndicationIEs ies.
 *  \param errorIndication Pointer to the ASN1 structure.
 *  \param errorIndicationIEs Pointer to the IES structure.
 **/
int s1ap_encode_errorindicationies(
    ErrorIndication_t *errorIndication,
    ErrorIndicationIEs_t *errorIndicationIEs);

/** \brief Decode function for HandoverNotifyIEs ies.
 * \param handoverNotifyIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_handovernotifyies(
    HandoverNotifyIEs_t *handoverNotifyIEs,
    ANY_t *any_p);

/** \brief Encode function for HandoverNotifyIEs ies.
 *  \param handoverNotify Pointer to the ASN1 structure.
 *  \param handoverNotifyIEs Pointer to the IES structure.
 **/
int s1ap_encode_handovernotifyies(
    HandoverNotify_t *handoverNotify,
    HandoverNotifyIEs_t *handoverNotifyIEs);

/** \brief Decode function for DownlinkS1cdma2000tunnelingIEs ies.
 * \param downlinkS1cdma2000tunnelingIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_downlinks1cdma2000tunnelingies(
    DownlinkS1cdma2000tunnelingIEs_t *downlinkS1cdma2000tunnelingIEs,
    ANY_t *any_p);

/** \brief Encode function for DownlinkS1cdma2000tunnelingIEs ies.
 *  \param downlinkS1cdma2000tunneling Pointer to the ASN1 structure.
 *  \param downlinkS1cdma2000tunnelingIEs Pointer to the IES structure.
 **/
int s1ap_encode_downlinks1cdma2000tunnelingies(
    DownlinkS1cdma2000tunneling_t *downlinkS1cdma2000tunneling,
    DownlinkS1cdma2000tunnelingIEs_t *downlinkS1cdma2000tunnelingIEs);

/** \brief Decode function for MMEConfigurationUpdateAcknowledgeIEs ies.
 * \param mmeConfigurationUpdateAcknowledgeIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_mmeconfigurationupdateacknowledgeies(
    MMEConfigurationUpdateAcknowledgeIEs_t *mmeConfigurationUpdateAcknowledgeIEs,
    ANY_t *any_p);

/** \brief Encode function for MMEConfigurationUpdateAcknowledgeIEs ies.
 *  \param mmeConfigurationUpdateAcknowledge Pointer to the ASN1 structure.
 *  \param mmeConfigurationUpdateAcknowledgeIEs Pointer to the IES structure.
 **/
int s1ap_encode_mmeconfigurationupdateacknowledgeies(
    MMEConfigurationUpdateAcknowledge_t *mmeConfigurationUpdateAcknowledge,
    MMEConfigurationUpdateAcknowledgeIEs_t *mmeConfigurationUpdateAcknowledgeIEs);

/** \brief Decode function for HandoverRequestAcknowledgeIEs ies.
 * \param handoverRequestAcknowledgeIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_handoverrequestacknowledgeies(
    HandoverRequestAcknowledgeIEs_t *handoverRequestAcknowledgeIEs,
    ANY_t *any_p);

/** \brief Encode function for HandoverRequestAcknowledgeIEs ies.
 *  \param handoverRequestAcknowledge Pointer to the ASN1 structure.
 *  \param handoverRequestAcknowledgeIEs Pointer to the IES structure.
 **/
int s1ap_encode_handoverrequestacknowledgeies(
    HandoverRequestAcknowledge_t *handoverRequestAcknowledge,
    HandoverRequestAcknowledgeIEs_t *handoverRequestAcknowledgeIEs);

/** \brief Decode function for E-RABSetupRequestIEs ies.
 * \param e_RABSetupRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_e_rabsetuprequesties(
    E_RABSetupRequestIEs_t *e_RABSetupRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for E-RABSetupRequestIEs ies.
 *  \param e_RABSetupRequest Pointer to the ASN1 structure.
 *  \param e_RABSetupRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabsetuprequesties(
    E_RABSetupRequest_t *e_RABSetupRequest,
    E_RABSetupRequestIEs_t *e_RABSetupRequestIEs);

/** \brief Decode function for DownlinkUEAssociatedLPPaTransportIEs ies.
 * \param downlinkUEAssociatedLPPaTransportIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_downlinkueassociatedlppatransporties(
    DownlinkUEAssociatedLPPaTransportIEs_t *downlinkUEAssociatedLPPaTransportIEs,
    ANY_t *any_p);

/** \brief Encode function for DownlinkUEAssociatedLPPaTransportIEs ies.
 *  \param downlinkUEAssociatedLPPaTransport Pointer to the ASN1 structure.
 *  \param downlinkUEAssociatedLPPaTransportIEs Pointer to the IES structure.
 **/
int s1ap_encode_downlinkueassociatedlppatransporties(
    DownlinkUEAssociatedLPPaTransport_t *downlinkUEAssociatedLPPaTransport,
    DownlinkUEAssociatedLPPaTransportIEs_t *downlinkUEAssociatedLPPaTransportIEs);

/** \brief Decode function for KillRequestIEs ies.
 * \param killRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_killrequesties(
    KillRequestIEs_t *killRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for KillRequestIEs ies.
 *  \param killRequest Pointer to the ASN1 structure.
 *  \param killRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_killrequesties(
    KillRequest_t *killRequest,
    KillRequestIEs_t *killRequestIEs);

/** \brief Decode function for E-RABModifyResponseIEs ies.
 * \param e_RABModifyResponseIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_e_rabmodifyresponseies(
    E_RABModifyResponseIEs_t *e_RABModifyResponseIEs,
    ANY_t *any_p);

/** \brief Encode function for E-RABModifyResponseIEs ies.
 *  \param e_RABModifyResponse Pointer to the ASN1 structure.
 *  \param e_RABModifyResponseIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabmodifyresponseies(
    E_RABModifyResponse_t *e_RABModifyResponse,
    E_RABModifyResponseIEs_t *e_RABModifyResponseIEs);

/** \brief Decode function for E-RABReleaseIndicationIEs ies.
 * \param e_RABReleaseIndicationIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_e_rabreleaseindicationies(
    E_RABReleaseIndicationIEs_t *e_RABReleaseIndicationIEs,
    ANY_t *any_p);

/** \brief Encode function for E-RABReleaseIndicationIEs ies.
 *  \param e_RABReleaseIndication Pointer to the ASN1 structure.
 *  \param e_RABReleaseIndicationIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabreleaseindicationies(
    E_RABReleaseIndication_t *e_RABReleaseIndication,
    E_RABReleaseIndicationIEs_t *e_RABReleaseIndicationIEs);

/** \brief Decode function for ENBConfigurationUpdateFailureIEs ies.
 * \param enbConfigurationUpdateFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_enbconfigurationupdatefailureies(
    ENBConfigurationUpdateFailureIEs_t *enbConfigurationUpdateFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for ENBConfigurationUpdateFailureIEs ies.
 *  \param eNBConfigurationUpdateFailure Pointer to the ASN1 structure.
 *  \param enbConfigurationUpdateFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_enbconfigurationupdatefailureies(
    ENBConfigurationUpdateFailure_t *eNBConfigurationUpdateFailure,
    ENBConfigurationUpdateFailureIEs_t *enbConfigurationUpdateFailureIEs);

/** \brief Decode function for WriteReplaceWarningRequestIEs ies.
 * \param writeReplaceWarningRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_writereplacewarningrequesties(
    WriteReplaceWarningRequestIEs_t *writeReplaceWarningRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for WriteReplaceWarningRequestIEs ies.
 *  \param writeReplaceWarningRequest Pointer to the ASN1 structure.
 *  \param writeReplaceWarningRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_writereplacewarningrequesties(
    WriteReplaceWarningRequest_t *writeReplaceWarningRequest,
    WriteReplaceWarningRequestIEs_t *writeReplaceWarningRequestIEs);

/** \brief Decode function for S1SetupFailureIEs ies.
 * \param s1SetupFailureIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_s1setupfailureies(
    S1SetupFailureIEs_t *s1SetupFailureIEs,
    ANY_t *any_p);

/** \brief Encode function for S1SetupFailureIEs ies.
 *  \param s1SetupFailure Pointer to the ASN1 structure.
 *  \param s1SetupFailureIEs Pointer to the IES structure.
 **/
int s1ap_encode_s1setupfailureies(
    S1SetupFailure_t *s1SetupFailure,
    S1SetupFailureIEs_t *s1SetupFailureIEs);

/** \brief Decode function for E-RABModifyRequestIEs ies.
 * \param e_RABModifyRequestIEs Pointer to ASN1 structure in which data will be stored
 *  \param any_p Pointer to the ANY value to decode.
 **/
int s1ap_decode_e_rabmodifyrequesties(
    E_RABModifyRequestIEs_t *e_RABModifyRequestIEs,
    ANY_t *any_p);

/** \brief Encode function for E-RABModifyRequestIEs ies.
 *  \param e_RABModifyRequest Pointer to the ASN1 structure.
 *  \param e_RABModifyRequestIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabmodifyrequesties(
    E_RABModifyRequest_t *e_RABModifyRequest,
    E_RABModifyRequestIEs_t *e_RABModifyRequestIEs);

/** \brief Encode function for E-RABToBeSetupItemBearerSUReqIEs ies.
 *  \param e_RABToBeSetupListBearerSUReq Pointer to the ASN1 structure.
 *  \param e_RABToBeSetupItemBearerSUReqIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabtobesetuplistbearersureq(
    E_RABToBeSetupListBearerSUReq_t *e_RABToBeSetupListBearerSUReq,
    E_RABToBeSetupListBearerSUReqIEs_t *e_RABToBeSetupListBearerSUReqIEs);

/** \brief Decode function for E-RABToBeSetupItemBearerSUReqIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabtobesetuplistbearersureq(
    E_RABToBeSetupListBearerSUReqIEs_t *e_RABToBeSetupListBearerSUReqIEs,
    E_RABToBeSetupListBearerSUReq_t *e_RABToBeSetupListBearerSUReq);

/** \brief Encode function for UE-associatedLogicalS1-ConnectionItemResAck ies.
 *  \param uE_associatedLogicalS1_ConnectionListResAck Pointer to the ASN1 structure.
 *  \param uE_associatedLogicalS1_ConnectionItemResAck Pointer to the IES structure.
 **/
int s1ap_encode_ue_associatedlogicals1_connectionlistresack(
    UE_associatedLogicalS1_ConnectionListResAck_t *uE_associatedLogicalS1_ConnectionListResAck,
    UE_associatedLogicalS1_ConnectionListResAckIEs_t *uE_associatedLogicalS1_ConnectionListResAckIEs);

/** \brief Decode function for UE-associatedLogicalS1-ConnectionItemResAck ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_ue_associatedlogicals1_connectionlistresack(
    UE_associatedLogicalS1_ConnectionListResAckIEs_t *uE_associatedLogicalS1_ConnectionListResAckIEs,
    UE_associatedLogicalS1_ConnectionListResAck_t *uE_associatedLogicalS1_ConnectionListResAck);

/** \brief Encode function for TAIItemIEs ies.
 *  \param taiList Pointer to the ASN1 structure.
 *  \param taiItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_tailist(
    TAIList_t *taiList,
    TAIListIEs_t *taiListIEs);

/** \brief Decode function for TAIItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_tailist(
    TAIListIEs_t *taiListIEs,
    TAIList_t *taiList);

/** \brief Encode function for E-RABAdmittedItemIEs ies.
 *  \param e_RABAdmittedList Pointer to the ASN1 structure.
 *  \param e_RABAdmittedItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabadmittedlist(
    E_RABAdmittedList_t *e_RABAdmittedList,
    E_RABAdmittedListIEs_t *e_RABAdmittedListIEs);

/** \brief Decode function for E-RABAdmittedItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabadmittedlist(
    E_RABAdmittedListIEs_t *e_RABAdmittedListIEs,
    E_RABAdmittedList_t *e_RABAdmittedList);

/** \brief Encode function for E-RABItemIEs ies.
 *  \param e_RABList Pointer to the ASN1 structure.
 *  \param e_RABItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rablist(
    E_RABList_t *e_RABList,
    E_RABListIEs_t *e_RABListIEs);

/** \brief Decode function for E-RABItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rablist(
    E_RABListIEs_t *e_RABListIEs,
    E_RABList_t *e_RABList);

/** \brief Encode function for E-RABToBeSetupItemHOReqIEs ies.
 *  \param e_RABToBeSetupListHOReq Pointer to the ASN1 structure.
 *  \param e_RABToBeSetupItemHOReqIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabtobesetuplisthoreq(
    E_RABToBeSetupListHOReq_t *e_RABToBeSetupListHOReq,
    E_RABToBeSetupListHOReqIEs_t *e_RABToBeSetupListHOReqIEs);

/** \brief Decode function for E-RABToBeSetupItemHOReqIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabtobesetuplisthoreq(
    E_RABToBeSetupListHOReqIEs_t *e_RABToBeSetupListHOReqIEs,
    E_RABToBeSetupListHOReq_t *e_RABToBeSetupListHOReq);

/** \brief Encode function for E-RABSetupItemBearerSUResIEs ies.
 *  \param e_RABSetupListBearerSURes Pointer to the ASN1 structure.
 *  \param e_RABSetupItemBearerSUResIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabsetuplistbearersures(
    E_RABSetupListBearerSURes_t *e_RABSetupListBearerSURes,
    E_RABSetupListBearerSUResIEs_t *e_RABSetupListBearerSUResIEs);

/** \brief Decode function for E-RABSetupItemBearerSUResIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabsetuplistbearersures(
    E_RABSetupListBearerSUResIEs_t *e_RABSetupListBearerSUResIEs,
    E_RABSetupListBearerSURes_t *e_RABSetupListBearerSURes);

/** \brief Encode function for E-RABToBeSetupItemCtxtSUReqIEs ies.
 *  \param e_RABToBeSetupListCtxtSUReq Pointer to the ASN1 structure.
 *  \param e_RABToBeSetupItemCtxtSUReqIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabtobesetuplistctxtsureq(
    E_RABToBeSetupListCtxtSUReq_t *e_RABToBeSetupListCtxtSUReq,
    E_RABToBeSetupListCtxtSUReqIEs_t *e_RABToBeSetupListCtxtSUReqIEs);

/** \brief Decode function for E-RABToBeSetupItemCtxtSUReqIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabtobesetuplistctxtsureq(
    E_RABToBeSetupListCtxtSUReqIEs_t *e_RABToBeSetupListCtxtSUReqIEs,
    E_RABToBeSetupListCtxtSUReq_t *e_RABToBeSetupListCtxtSUReq);

/** \brief Encode function for E-RABSetupItemCtxtSUResIEs ies.
 *  \param e_RABSetupListCtxtSURes Pointer to the ASN1 structure.
 *  \param e_RABSetupItemCtxtSUResIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabsetuplistctxtsures(
    E_RABSetupListCtxtSURes_t *e_RABSetupListCtxtSURes,
    E_RABSetupListCtxtSUResIEs_t *e_RABSetupListCtxtSUResIEs);

/** \brief Decode function for E-RABSetupItemCtxtSUResIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabsetuplistctxtsures(
    E_RABSetupListCtxtSUResIEs_t *e_RABSetupListCtxtSUResIEs,
    E_RABSetupListCtxtSURes_t *e_RABSetupListCtxtSURes);

/** \brief Encode function for E-RABInformationItemIEs ies.
 *  \param e_RABInformationList Pointer to the ASN1 structure.
 *  \param e_RABInformationItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabinformationlist(
    E_RABInformationList_t *e_RABInformationList,
    E_RABInformationListIEs_t *e_RABInformationListIEs);

/** \brief Decode function for E-RABInformationItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabinformationlist(
    E_RABInformationListIEs_t *e_RABInformationListIEs,
    E_RABInformationList_t *e_RABInformationList);

/** \brief Encode function for E-RABReleaseItemBearerRelCompIEs ies.
 *  \param e_RABReleaseListBearerRelComp Pointer to the ASN1 structure.
 *  \param e_RABReleaseItemBearerRelCompIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabreleaselistbearerrelcomp(
    E_RABReleaseListBearerRelComp_t *e_RABReleaseListBearerRelComp,
    E_RABReleaseListBearerRelCompIEs_t *e_RABReleaseListBearerRelCompIEs);

/** \brief Decode function for E-RABReleaseItemBearerRelCompIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabreleaselistbearerrelcomp(
    E_RABReleaseListBearerRelCompIEs_t *e_RABReleaseListBearerRelCompIEs,
    E_RABReleaseListBearerRelComp_t *e_RABReleaseListBearerRelComp);

/** \brief Encode function for E-RABToBeModifiedItemBearerModReqIEs ies.
 *  \param e_RABToBeModifiedListBearerModReq Pointer to the ASN1 structure.
 *  \param e_RABToBeModifiedItemBearerModReqIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabtobemodifiedlistbearermodreq(
    E_RABToBeModifiedListBearerModReq_t *e_RABToBeModifiedListBearerModReq,
    E_RABToBeModifiedListBearerModReqIEs_t *e_RABToBeModifiedListBearerModReqIEs);

/** \brief Decode function for E-RABToBeModifiedItemBearerModReqIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabtobemodifiedlistbearermodreq(
    E_RABToBeModifiedListBearerModReqIEs_t *e_RABToBeModifiedListBearerModReqIEs,
    E_RABToBeModifiedListBearerModReq_t *e_RABToBeModifiedListBearerModReq);

/** \brief Encode function for E-RABToBeSwitchedDLItemIEs ies.
 *  \param e_RABToBeSwitchedDLList Pointer to the ASN1 structure.
 *  \param e_RABToBeSwitchedDLItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabtobeswitcheddllist(
    E_RABToBeSwitchedDLList_t *e_RABToBeSwitchedDLList,
    E_RABToBeSwitchedDLListIEs_t *e_RABToBeSwitchedDLListIEs);

/** \brief Decode function for E-RABToBeSwitchedDLItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabtobeswitcheddllist(
    E_RABToBeSwitchedDLListIEs_t *e_RABToBeSwitchedDLListIEs,
    E_RABToBeSwitchedDLList_t *e_RABToBeSwitchedDLList);

/** \brief Encode function for Bearers-SubjectToStatusTransfer-ItemIEs ies.
 *  \param bearers_SubjectToStatusTransfer_List Pointer to the ASN1 structure.
 *  \param bearers_SubjectToStatusTransfer_ItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_bearers_subjecttostatustransfer_list(
    Bearers_SubjectToStatusTransfer_List_t *bearers_SubjectToStatusTransfer_List,
    Bearers_SubjectToStatusTransfer_ListIEs_t *bearers_SubjectToStatusTransfer_ListIEs);

/** \brief Decode function for Bearers-SubjectToStatusTransfer-ItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_bearers_subjecttostatustransfer_list(
    Bearers_SubjectToStatusTransfer_ListIEs_t *bearers_SubjectToStatusTransfer_ListIEs,
    Bearers_SubjectToStatusTransfer_List_t *bearers_SubjectToStatusTransfer_List);

/** \brief Encode function for E-RABToBeSwitchedULItemIEs ies.
 *  \param e_RABToBeSwitchedULList Pointer to the ASN1 structure.
 *  \param e_RABToBeSwitchedULItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabtobeswitchedullist(
    E_RABToBeSwitchedULList_t *e_RABToBeSwitchedULList,
    E_RABToBeSwitchedULListIEs_t *e_RABToBeSwitchedULListIEs);

/** \brief Decode function for E-RABToBeSwitchedULItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabtobeswitchedullist(
    E_RABToBeSwitchedULListIEs_t *e_RABToBeSwitchedULListIEs,
    E_RABToBeSwitchedULList_t *e_RABToBeSwitchedULList);

/** \brief Encode function for E-RABDataForwardingItemIEs ies.
 *  \param e_RABDataForwardingList Pointer to the ASN1 structure.
 *  \param e_RABDataForwardingItemIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabdataforwardinglist(
    E_RABDataForwardingList_t *e_RABDataForwardingList,
    E_RABDataForwardingListIEs_t *e_RABDataForwardingListIEs);

/** \brief Decode function for E-RABDataForwardingItemIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabdataforwardinglist(
    E_RABDataForwardingListIEs_t *e_RABDataForwardingListIEs,
    E_RABDataForwardingList_t *e_RABDataForwardingList);

/** \brief Encode function for E-RABModifyItemBearerModResIEs ies.
 *  \param e_RABModifyListBearerModRes Pointer to the ASN1 structure.
 *  \param e_RABModifyItemBearerModResIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabmodifylistbearermodres(
    E_RABModifyListBearerModRes_t *e_RABModifyListBearerModRes,
    E_RABModifyListBearerModResIEs_t *e_RABModifyListBearerModResIEs);

/** \brief Decode function for E-RABModifyItemBearerModResIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabmodifylistbearermodres(
    E_RABModifyListBearerModResIEs_t *e_RABModifyListBearerModResIEs,
    E_RABModifyListBearerModRes_t *e_RABModifyListBearerModRes);

/** \brief Encode function for UE-associatedLogicalS1-ConnectionItemRes ies.
 *  \param uE_associatedLogicalS1_ConnectionListRes Pointer to the ASN1 structure.
 *  \param uE_associatedLogicalS1_ConnectionItemRes Pointer to the IES structure.
 **/
int s1ap_encode_ue_associatedlogicals1_connectionlistres(
    UE_associatedLogicalS1_ConnectionListRes_t *uE_associatedLogicalS1_ConnectionListRes,
    UE_associatedLogicalS1_ConnectionListResIEs_t *uE_associatedLogicalS1_ConnectionListResIEs);

/** \brief Decode function for UE-associatedLogicalS1-ConnectionItemRes ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_ue_associatedlogicals1_connectionlistres(
    UE_associatedLogicalS1_ConnectionListResIEs_t *uE_associatedLogicalS1_ConnectionListResIEs,
    UE_associatedLogicalS1_ConnectionListRes_t *uE_associatedLogicalS1_ConnectionListRes);

/** \brief Encode function for E-RABFailedtoSetupItemHOReqAckIEs ies.
 *  \param e_RABFailedtoSetupListHOReqAck Pointer to the ASN1 structure.
 *  \param e_RABFailedtoSetupItemHOReqAckIEs Pointer to the IES structure.
 **/
int s1ap_encode_e_rabfailedtosetuplisthoreqack(
    E_RABFailedtoSetupListHOReqAck_t *e_RABFailedtoSetupListHOReqAck,
    E_RABFailedtoSetupListHOReqAckIEs_t *e_RABFailedtoSetupListHOReqAckIEs);

/** \brief Decode function for E-RABFailedtoSetupItemHOReqAckIEs ies.
 *  \param any_p Pointer to the ANY value to decode.
 *  \param callback Callback function called when any_p is successfully decoded.
 **/
int s1ap_decode_e_rabfailedtosetuplisthoreqack(
    E_RABFailedtoSetupListHOReqAckIEs_t *e_RABFailedtoSetupListHOReqAckIEs,
    E_RABFailedtoSetupListHOReqAck_t *e_RABFailedtoSetupListHOReqAck);

#endif /* S1AP_IES_DEFS_H_ */

