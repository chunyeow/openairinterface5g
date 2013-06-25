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
 * Created on: 2012-11-13 11:44:11.998347
 * from /homes/roux/trunk-clean/openair2/S1AP/MESSAGES/ASN1/R9.8/S1AP-PDU-Contents.asn
 ******************************************************************************/
#include "s1ap_common.h"
#include "s1ap_ies_defs.h"

int s1ap_encode_mmeconfigurationupdateies(
    MMEConfigurationUpdate_t *mmeConfigurationUpdate,
    MMEConfigurationUpdateIEs_t *mmeConfigurationUpdateIEs) {

    IE_t *ie;

    /* Optional field */
    if ((mmeConfigurationUpdateIEs->presenceMask & MMECONFIGURATIONUPDATEIES_MMENAME_PRESENT)
        == MMECONFIGURATIONUPDATEIES_MMENAME_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MMEname,
                              Criticality_ignore,
                              &asn_DEF_MMEname,
                              &mmeConfigurationUpdateIEs->mmEname)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&mmeConfigurationUpdate->mmeConfigurationUpdate_ies.list, ie);
    }

    /* Optional field */
    if ((mmeConfigurationUpdateIEs->presenceMask & MMECONFIGURATIONUPDATEIES_SERVEDGUMMEIS_PRESENT)
        == MMECONFIGURATIONUPDATEIES_SERVEDGUMMEIS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_ServedGUMMEIs,
                              Criticality_reject,
                              &asn_DEF_ServedGUMMEIs,
                              &mmeConfigurationUpdateIEs->servedGUMMEIs)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&mmeConfigurationUpdate->mmeConfigurationUpdate_ies.list, ie);
    }

    /* Optional field */
    if ((mmeConfigurationUpdateIEs->presenceMask & MMECONFIGURATIONUPDATEIES_RELATIVEMMECAPACITY_PRESENT)
        == MMECONFIGURATIONUPDATEIES_RELATIVEMMECAPACITY_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RelativeMMECapacity,
                              Criticality_reject,
                              &asn_DEF_RelativeMMECapacity,
                              &mmeConfigurationUpdateIEs->relativeMMECapacity)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&mmeConfigurationUpdate->mmeConfigurationUpdate_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_uecontextmodificationrequesties(
    UEContextModificationRequest_t *ueContextModificationRequest,
    UEContextModificationRequestIEs_t *ueContextModificationRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &ueContextModificationRequestIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &ueContextModificationRequestIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);

    /* Optional field */
    if ((ueContextModificationRequestIEs->presenceMask & UECONTEXTMODIFICATIONREQUESTIES_SECURITYKEY_PRESENT)
        == UECONTEXTMODIFICATIONREQUESTIES_SECURITYKEY_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SecurityKey,
                              Criticality_reject,
                              &asn_DEF_SecurityKey,
                              &ueContextModificationRequestIEs->securityKey)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);
    }

    /* Optional field */
    if ((ueContextModificationRequestIEs->presenceMask & UECONTEXTMODIFICATIONREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT)
        == UECONTEXTMODIFICATIONREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SubscriberProfileIDforRFP,
                              Criticality_ignore,
                              &asn_DEF_SubscriberProfileIDforRFP,
                              &ueContextModificationRequestIEs->subscriberProfileIDforRFP)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);
    }

    /* Optional field */
    if ((ueContextModificationRequestIEs->presenceMask & UECONTEXTMODIFICATIONREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == UECONTEXTMODIFICATIONREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_uEaggregateMaximumBitrate,
                              Criticality_ignore,
                              &asn_DEF_UEAggregateMaximumBitrate,
                              &ueContextModificationRequestIEs->uEaggregateMaximumBitrate)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);
    }

    /* Optional field */
    if ((ueContextModificationRequestIEs->presenceMask & UECONTEXTMODIFICATIONREQUESTIES_CSFALLBACKINDICATOR_PRESENT)
        == UECONTEXTMODIFICATIONREQUESTIES_CSFALLBACKINDICATOR_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSFallbackIndicator,
                              Criticality_reject,
                              &asn_DEF_CSFallbackIndicator,
                              &ueContextModificationRequestIEs->csFallbackIndicator)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);
    }

    /* Optional field */
    if ((ueContextModificationRequestIEs->presenceMask & UECONTEXTMODIFICATIONREQUESTIES_UESECURITYCAPABILITIES_PRESENT)
        == UECONTEXTMODIFICATIONREQUESTIES_UESECURITYCAPABILITIES_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UESecurityCapabilities,
                              Criticality_reject,
                              &asn_DEF_UESecurityCapabilities,
                              &ueContextModificationRequestIEs->ueSecurityCapabilities)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);
    }

    /* Optional field */
    if ((ueContextModificationRequestIEs->presenceMask & UECONTEXTMODIFICATIONREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT)
        == UECONTEXTMODIFICATIONREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSGMembershipStatus,
                              Criticality_ignore,
                              &asn_DEF_CSGMembershipStatus,
                              &ueContextModificationRequestIEs->csgMembershipStatus)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);
    }

    /* Optional field */
    if ((ueContextModificationRequestIEs->presenceMask & UECONTEXTMODIFICATIONREQUESTIES_LAI_PRESENT)
        == UECONTEXTMODIFICATIONREQUESTIES_LAI_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RegisteredLAI,
                              Criticality_ignore,
                              &asn_DEF_LAI,
                              &ueContextModificationRequestIEs->registeredLAI)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationRequest->ueContextModificationRequest_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_enbdirectinformationtransferies(
    ENBDirectInformationTransfer_t *eNBDirectInformationTransfer,
    ENBDirectInformationTransferIEs_t *enbDirectInformationTransferIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Inter_SystemInformationTransferTypeEDT,
                          Criticality_reject,
                          &asn_DEF_Inter_SystemInformationTransferType,
                          &enbDirectInformationTransferIEs->inter_SystemInformationTransferTypeEDT)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&eNBDirectInformationTransfer->eNBDirectInformationTransfer_ies.list, ie);

    return 0;
}

int s1ap_encode_resetacknowledgeies(
    ResetAcknowledge_t *resetAcknowledge,
    ResetAcknowledgeIEs_t *resetAcknowledgeIEs) {

    IE_t *ie;

    /* Optional field */
    if ((resetAcknowledgeIEs->presenceMask & RESETACKNOWLEDGEIES_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_PRESENT)
        == RESETACKNOWLEDGEIES_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionListResAck,
                              Criticality_ignore,
                              &asn_DEF_UE_associatedLogicalS1_ConnectionListResAck,
                              &resetAcknowledgeIEs->uE_associatedLogicalS1_ConnectionListResAck)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&resetAcknowledge->resetAcknowledge_ies.list, ie);
    }

    /* Optional field */
    if ((resetAcknowledgeIEs->presenceMask & RESETACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == RESETACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &resetAcknowledgeIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&resetAcknowledge->resetAcknowledge_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_pagingies(
    Paging_t *paging,
    PagingIEs_t *pagingIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UEIdentityIndexValue,
                          Criticality_ignore,
                          &asn_DEF_UEIdentityIndexValue,
                          &pagingIEs->ueIdentityIndexValue)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&paging->paging_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UEPagingID,
                          Criticality_ignore,
                          &asn_DEF_UEPagingID,
                          &pagingIEs->uePagingID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&paging->paging_ies.list, ie);

    /* Optional field */
    if ((pagingIEs->presenceMask & PAGINGIES_PAGINGDRX_PRESENT)
        == PAGINGIES_PAGINGDRX_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_pagingDRX,
                              Criticality_ignore,
                              &asn_DEF_PagingDRX,
                              &pagingIEs->pagingDRX)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&paging->paging_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CNDomain,
                          Criticality_ignore,
                          &asn_DEF_CNDomain,
                          &pagingIEs->cnDomain)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&paging->paging_ies.list, ie);

    TAIList_t taiList;

    memset(&taiList, 0, sizeof(TAIList_t));

    if (s1ap_encode_tailist(&taiList, &pagingIEs->taiList) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TAIList,
                          Criticality_ignore,
                          &asn_DEF_TAIList,
                          &taiList)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&paging->paging_ies.list, ie);

    /* Optional field */
    if ((pagingIEs->presenceMask & PAGINGIES_CSG_IDLIST_PRESENT)
        == PAGINGIES_CSG_IDLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSG_IdList,
                              Criticality_ignore,
                              &asn_DEF_CSG_IdList,
                              &pagingIEs->csG_IdList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&paging->paging_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_pathswitchrequesties(
    PathSwitchRequest_t *pathSwitchRequest,
    PathSwitchRequestIEs_t *pathSwitchRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &pathSwitchRequestIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequest->pathSwitchRequest_ies.list, ie);

    E_RABToBeSwitchedDLList_t e_RABToBeSwitchedDLList;

    memset(&e_RABToBeSwitchedDLList, 0, sizeof(E_RABToBeSwitchedDLList_t));

    if (s1ap_encode_e_rabtobeswitcheddllist(&e_RABToBeSwitchedDLList, &pathSwitchRequestIEs->e_RABToBeSwitchedDLList) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSwitchedDLList,
                          Criticality_reject,
                          &asn_DEF_E_RABToBeSwitchedDLList,
                          &e_RABToBeSwitchedDLList)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequest->pathSwitchRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SourceMME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &pathSwitchRequestIEs->sourceMME_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequest->pathSwitchRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_EUTRAN_CGI,
                          Criticality_ignore,
                          &asn_DEF_EUTRAN_CGI,
                          &pathSwitchRequestIEs->eutran_cgi)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequest->pathSwitchRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TAI,
                          Criticality_ignore,
                          &asn_DEF_TAI,
                          &pathSwitchRequestIEs->tai)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequest->pathSwitchRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UESecurityCapabilities,
                          Criticality_ignore,
                          &asn_DEF_UESecurityCapabilities,
                          &pathSwitchRequestIEs->ueSecurityCapabilities)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequest->pathSwitchRequest_ies.list, ie);

    return 0;
}

int s1ap_encode_deactivatetraceies(
    DeactivateTrace_t *deactivateTrace,
    DeactivateTraceIEs_t *deactivateTraceIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &deactivateTraceIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&deactivateTrace->deactivateTrace_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &deactivateTraceIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&deactivateTrace->deactivateTrace_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_UTRAN_Trace_ID,
                          Criticality_ignore,
                          &asn_DEF_E_UTRAN_Trace_ID,
                          &deactivateTraceIEs->e_UTRAN_Trace_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&deactivateTrace->deactivateTrace_ies.list, ie);

    return 0;
}

int s1ap_encode_locationreportingcontrolies(
    LocationReportingControl_t *locationReportingControl,
    LocationReportingControlIEs_t *locationReportingControlIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &locationReportingControlIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReportingControl->locationReportingControl_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &locationReportingControlIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReportingControl->locationReportingControl_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RequestType,
                          Criticality_ignore,
                          &asn_DEF_RequestType,
                          &locationReportingControlIEs->requestType)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReportingControl->locationReportingControl_ies.list, ie);

    return 0;
}

int s1ap_encode_enbconfigurationupdateacknowledgeies(
    ENBConfigurationUpdateAcknowledge_t *eNBConfigurationUpdateAcknowledge,
    ENBConfigurationUpdateAcknowledgeIEs_t *enbConfigurationUpdateAcknowledgeIEs) {

    IE_t *ie;

    /* Optional field */
    if ((enbConfigurationUpdateAcknowledgeIEs->presenceMask & ENBCONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == ENBCONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &enbConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationUpdateAcknowledge->eNBConfigurationUpdateAcknowledge_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_handoverfailureies(
    HandoverFailure_t *handoverFailure,
    HandoverFailureIEs_t *handoverFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &handoverFailureIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverFailure->handoverFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &handoverFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverFailure->handoverFailure_ies.list, ie);

    /* Optional field */
    if ((handoverFailureIEs->presenceMask & HANDOVERFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == HANDOVERFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &handoverFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverFailure->handoverFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_locationreportingfailureindicationies(
    LocationReportingFailureIndication_t *locationReportingFailureIndication,
    LocationReportingFailureIndicationIEs_t *locationReportingFailureIndicationIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &locationReportingFailureIndicationIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReportingFailureIndication->locationReportingFailureIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &locationReportingFailureIndicationIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReportingFailureIndication->locationReportingFailureIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &locationReportingFailureIndicationIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReportingFailureIndication->locationReportingFailureIndication_ies.list, ie);

    return 0;
}

int s1ap_encode_downlinknastransporties(
    DownlinkNASTransport_t *downlinkNASTransport,
    DownlinkNASTransportIEs_t *downlinkNASTransportIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &downlinkNASTransportIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkNASTransport->downlinkNASTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &downlinkNASTransportIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkNASTransport->downlinkNASTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_NAS_PDU,
                          Criticality_reject,
                          &asn_DEF_NAS_PDU,
                          &downlinkNASTransportIEs->nas_pdu)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkNASTransport->downlinkNASTransport_ies.list, ie);

    /* Optional field */
    if ((downlinkNASTransportIEs->presenceMask & DOWNLINKNASTRANSPORTIES_HANDOVERRESTRICTIONLIST_PRESENT)
        == DOWNLINKNASTRANSPORTIES_HANDOVERRESTRICTIONLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_HandoverRestrictionList,
                              Criticality_ignore,
                              &asn_DEF_HandoverRestrictionList,
                              &downlinkNASTransportIEs->handoverRestrictionList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&downlinkNASTransport->downlinkNASTransport_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_mmeconfigurationupdatefailureies(
    MMEConfigurationUpdateFailure_t *mmeConfigurationUpdateFailure,
    MMEConfigurationUpdateFailureIEs_t *mmeConfigurationUpdateFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &mmeConfigurationUpdateFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&mmeConfigurationUpdateFailure->mmeConfigurationUpdateFailure_ies.list, ie);

    /* Optional field */
    if ((mmeConfigurationUpdateFailureIEs->presenceMask & MMECONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT)
        == MMECONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TimeToWait,
                              Criticality_ignore,
                              &asn_DEF_TimeToWait,
                              &mmeConfigurationUpdateFailureIEs->timeToWait)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&mmeConfigurationUpdateFailure->mmeConfigurationUpdateFailure_ies.list, ie);
    }

    /* Optional field */
    if ((mmeConfigurationUpdateFailureIEs->presenceMask & MMECONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == MMECONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &mmeConfigurationUpdateFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&mmeConfigurationUpdateFailure->mmeConfigurationUpdateFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_uecontextreleasecompleteies(
    UEContextReleaseComplete_t *ueContextReleaseComplete,
    UEContextReleaseCompleteIEs_t *ueContextReleaseCompleteIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &ueContextReleaseCompleteIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextReleaseComplete->ueContextReleaseComplete_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &ueContextReleaseCompleteIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextReleaseComplete->ueContextReleaseComplete_ies.list, ie);

    /* Optional field */
    if ((ueContextReleaseCompleteIEs->presenceMask & UECONTEXTRELEASECOMPLETEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == UECONTEXTRELEASECOMPLETEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &ueContextReleaseCompleteIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextReleaseComplete->ueContextReleaseComplete_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_s1setupresponseies(
    S1SetupResponse_t *s1SetupResponse,
    S1SetupResponseIEs_t *s1SetupResponseIEs) {

    IE_t *ie;

    /* Optional field */
    if ((s1SetupResponseIEs->presenceMask & S1SETUPRESPONSEIES_MMENAME_PRESENT)
        == S1SETUPRESPONSEIES_MMENAME_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MMEname,
                              Criticality_ignore,
                              &asn_DEF_MMEname,
                              &s1SetupResponseIEs->mmEname)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&s1SetupResponse->s1SetupResponse_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_ServedGUMMEIs,
                          Criticality_reject,
                          &asn_DEF_ServedGUMMEIs,
                          &s1SetupResponseIEs->servedGUMMEIs)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&s1SetupResponse->s1SetupResponse_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RelativeMMECapacity,
                          Criticality_ignore,
                          &asn_DEF_RelativeMMECapacity,
                          &s1SetupResponseIEs->relativeMMECapacity)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&s1SetupResponse->s1SetupResponse_ies.list, ie);

    /* Optional field */
    if ((s1SetupResponseIEs->presenceMask & S1SETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1SETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &s1SetupResponseIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&s1SetupResponse->s1SetupResponse_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_handoverpreparationfailureies(
    HandoverPreparationFailure_t *handoverPreparationFailure,
    HandoverPreparationFailureIEs_t *handoverPreparationFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &handoverPreparationFailureIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverPreparationFailure->handoverPreparationFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &handoverPreparationFailureIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverPreparationFailure->handoverPreparationFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &handoverPreparationFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverPreparationFailure->handoverPreparationFailure_ies.list, ie);

    /* Optional field */
    if ((handoverPreparationFailureIEs->presenceMask & HANDOVERPREPARATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == HANDOVERPREPARATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &handoverPreparationFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverPreparationFailure->handoverPreparationFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_killresponseies(
    KillResponse_t *killResponse,
    KillResponseIEs_t *killResponseIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MessageIdentifier,
                          Criticality_reject,
                          &asn_DEF_MessageIdentifier,
                          &killResponseIEs->messageIdentifier)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&killResponse->killResponse_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SerialNumber,
                          Criticality_reject,
                          &asn_DEF_SerialNumber,
                          &killResponseIEs->serialNumber)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&killResponse->killResponse_ies.list, ie);

    /* Optional field */
    if ((killResponseIEs->presenceMask & KILLRESPONSEIES_BROADCASTCANCELLEDAREALIST_PRESENT)
        == KILLRESPONSEIES_BROADCASTCANCELLEDAREALIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_BroadcastCancelledAreaList,
                              Criticality_ignore,
                              &asn_DEF_BroadcastCancelledAreaList,
                              &killResponseIEs->broadcastCancelledAreaList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&killResponse->killResponse_ies.list, ie);
    }

    /* Optional field */
    if ((killResponseIEs->presenceMask & KILLRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == KILLRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &killResponseIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&killResponse->killResponse_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_uplinkueassociatedlppatransporties(
    UplinkUEAssociatedLPPaTransport_t *uplinkUEAssociatedLPPaTransport,
    UplinkUEAssociatedLPPaTransportIEs_t *uplinkUEAssociatedLPPaTransportIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &uplinkUEAssociatedLPPaTransportIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkUEAssociatedLPPaTransport->uplinkUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &uplinkUEAssociatedLPPaTransportIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkUEAssociatedLPPaTransport->uplinkUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Routing_ID,
                          Criticality_reject,
                          &asn_DEF_Routing_ID,
                          &uplinkUEAssociatedLPPaTransportIEs->routing_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkUEAssociatedLPPaTransport->uplinkUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_LPPa_PDU,
                          Criticality_reject,
                          &asn_DEF_LPPa_PDU,
                          &uplinkUEAssociatedLPPaTransportIEs->lpPa_PDU)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkUEAssociatedLPPaTransport->uplinkUEAssociatedLPPaTransport_ies.list, ie);

    return 0;
}

int s1ap_encode_enbconfigurationupdateies(
    ENBConfigurationUpdate_t *eNBConfigurationUpdate,
    ENBConfigurationUpdateIEs_t *enbConfigurationUpdateIEs) {

    IE_t *ie;

    /* Optional field */
    if ((enbConfigurationUpdateIEs->presenceMask & ENBCONFIGURATIONUPDATEIES_ENBNAME_PRESENT)
        == ENBCONFIGURATIONUPDATEIES_ENBNAME_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNBname,
                              Criticality_ignore,
                              &asn_DEF_ENBname,
                              &enbConfigurationUpdateIEs->eNBname)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationUpdate->eNBConfigurationUpdate_ies.list, ie);
    }

    /* Optional field */
    if ((enbConfigurationUpdateIEs->presenceMask & ENBCONFIGURATIONUPDATEIES_SUPPORTEDTAS_PRESENT)
        == ENBCONFIGURATIONUPDATEIES_SUPPORTEDTAS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SupportedTAs,
                              Criticality_reject,
                              &asn_DEF_SupportedTAs,
                              &enbConfigurationUpdateIEs->supportedTAs)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationUpdate->eNBConfigurationUpdate_ies.list, ie);
    }

    /* Optional field */
    if ((enbConfigurationUpdateIEs->presenceMask & ENBCONFIGURATIONUPDATEIES_CSG_IDLIST_PRESENT)
        == ENBCONFIGURATIONUPDATEIES_CSG_IDLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSG_IdList,
                              Criticality_reject,
                              &asn_DEF_CSG_IdList,
                              &enbConfigurationUpdateIEs->csG_IdList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationUpdate->eNBConfigurationUpdate_ies.list, ie);
    }

    /* Optional field */
    if ((enbConfigurationUpdateIEs->presenceMask & ENBCONFIGURATIONUPDATEIES_PAGINGDRX_PRESENT)
        == ENBCONFIGURATIONUPDATEIES_PAGINGDRX_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_DefaultPagingDRX,
                              Criticality_ignore,
                              &asn_DEF_PagingDRX,
                              &enbConfigurationUpdateIEs->defaultPagingDRX)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationUpdate->eNBConfigurationUpdate_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_initialcontextsetuprequesties(
    InitialContextSetupRequest_t *initialContextSetupRequest,
    InitialContextSetupRequestIEs_t *initialContextSetupRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &initialContextSetupRequestIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &initialContextSetupRequestIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_uEaggregateMaximumBitrate,
                          Criticality_reject,
                          &asn_DEF_UEAggregateMaximumBitrate,
                          &initialContextSetupRequestIEs->uEaggregateMaximumBitrate)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);

    E_RABToBeSetupListCtxtSUReq_t e_RABToBeSetupListCtxtSUReq;

    memset(&e_RABToBeSetupListCtxtSUReq, 0, sizeof(E_RABToBeSetupListCtxtSUReq_t));

    if (s1ap_encode_e_rabtobesetuplistctxtsureq(&e_RABToBeSetupListCtxtSUReq, &initialContextSetupRequestIEs->e_RABToBeSetupListCtxtSUReq) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSetupListCtxtSUReq,
                          Criticality_reject,
                          &asn_DEF_E_RABToBeSetupListCtxtSUReq,
                          &e_RABToBeSetupListCtxtSUReq)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UESecurityCapabilities,
                          Criticality_reject,
                          &asn_DEF_UESecurityCapabilities,
                          &initialContextSetupRequestIEs->ueSecurityCapabilities)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SecurityKey,
                          Criticality_reject,
                          &asn_DEF_SecurityKey,
                          &initialContextSetupRequestIEs->securityKey)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_TRACEACTIVATION_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_TRACEACTIVATION_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TraceActivation,
                              Criticality_ignore,
                              &asn_DEF_TraceActivation,
                              &initialContextSetupRequestIEs->traceActivation)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_HandoverRestrictionList,
                              Criticality_ignore,
                              &asn_DEF_HandoverRestrictionList,
                              &initialContextSetupRequestIEs->handoverRestrictionList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UERadioCapability,
                              Criticality_ignore,
                              &asn_DEF_UERadioCapability,
                              &initialContextSetupRequestIEs->ueRadioCapability)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SubscriberProfileIDforRFP,
                              Criticality_ignore,
                              &asn_DEF_SubscriberProfileIDforRFP,
                              &initialContextSetupRequestIEs->subscriberProfileIDforRFP)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_CSFALLBACKINDICATOR_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_CSFALLBACKINDICATOR_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSFallbackIndicator,
                              Criticality_reject,
                              &asn_DEF_CSFallbackIndicator,
                              &initialContextSetupRequestIEs->csFallbackIndicator)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SRVCCOperationPossible,
                              Criticality_ignore,
                              &asn_DEF_SRVCCOperationPossible,
                              &initialContextSetupRequestIEs->srvccOperationPossible)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSGMembershipStatus,
                              Criticality_ignore,
                              &asn_DEF_CSGMembershipStatus,
                              &initialContextSetupRequestIEs->csgMembershipStatus)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupRequestIEs->presenceMask & INITIALCONTEXTSETUPREQUESTIES_LAI_PRESENT)
        == INITIALCONTEXTSETUPREQUESTIES_LAI_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RegisteredLAI,
                              Criticality_ignore,
                              &asn_DEF_LAI,
                              &initialContextSetupRequestIEs->registeredLAI)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupRequest->initialContextSetupRequest_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_mmedirectinformationtransferies(
    MMEDirectInformationTransfer_t *mmeDirectInformationTransfer,
    MMEDirectInformationTransferIEs_t *mmeDirectInformationTransferIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Inter_SystemInformationTransferTypeMDT,
                          Criticality_reject,
                          &asn_DEF_Inter_SystemInformationTransferType,
                          &mmeDirectInformationTransferIEs->inter_SystemInformationTransferTypeMDT)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&mmeDirectInformationTransfer->mmeDirectInformationTransfer_ies.list, ie);

    return 0;
}

int s1ap_encode_uplinknonueassociatedlppatransporties(
    UplinkNonUEAssociatedLPPaTransport_t *uplinkNonUEAssociatedLPPaTransport,
    UplinkNonUEAssociatedLPPaTransportIEs_t *uplinkNonUEAssociatedLPPaTransportIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Routing_ID,
                          Criticality_reject,
                          &asn_DEF_Routing_ID,
                          &uplinkNonUEAssociatedLPPaTransportIEs->routing_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkNonUEAssociatedLPPaTransport->uplinkNonUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_LPPa_PDU,
                          Criticality_reject,
                          &asn_DEF_LPPa_PDU,
                          &uplinkNonUEAssociatedLPPaTransportIEs->lpPa_PDU)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkNonUEAssociatedLPPaTransport->uplinkNonUEAssociatedLPPaTransport_ies.list, ie);

    return 0;
}

int s1ap_encode_reseties(
    Reset_t *reset,
    ResetIEs_t *resetIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &resetIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&reset->reset_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_ResetType,
                          Criticality_reject,
                          &asn_DEF_ResetType,
                          &resetIEs->resetType)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&reset->reset_ies.list, ie);

    return 0;
}

int s1ap_encode_uecontextreleasecommandies(
    UEContextReleaseCommand_t *ueContextReleaseCommand,
    UEContextReleaseCommandIEs_t *ueContextReleaseCommandIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UE_S1AP_IDs,
                          Criticality_reject,
                          &asn_DEF_UE_S1AP_IDs,
                          &ueContextReleaseCommandIEs->uE_S1AP_IDs)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextReleaseCommand->ueContextReleaseCommand_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &ueContextReleaseCommandIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextReleaseCommand->ueContextReleaseCommand_ies.list, ie);

    return 0;
}

int s1ap_encode_uecapabilityinfoindicationies(
    UECapabilityInfoIndication_t *ueCapabilityInfoIndication,
    UECapabilityInfoIndicationIEs_t *ueCapabilityInfoIndicationIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &ueCapabilityInfoIndicationIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueCapabilityInfoIndication->ueCapabilityInfoIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &ueCapabilityInfoIndicationIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueCapabilityInfoIndication->ueCapabilityInfoIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UERadioCapability,
                          Criticality_ignore,
                          &asn_DEF_UERadioCapability,
                          &ueCapabilityInfoIndicationIEs->ueRadioCapability)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueCapabilityInfoIndication->ueCapabilityInfoIndication_ies.list, ie);

    return 0;
}

int s1ap_encode_uplinks1cdma2000tunnelingies(
    UplinkS1cdma2000tunneling_t *uplinkS1cdma2000tunneling,
    UplinkS1cdma2000tunnelingIEs_t *uplinkS1cdma2000tunnelingIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &uplinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &uplinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000RATType,
                          Criticality_reject,
                          &asn_DEF_Cdma2000RATType,
                          &uplinkS1cdma2000tunnelingIEs->cdma2000RATType)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000SectorID,
                          Criticality_reject,
                          &asn_DEF_Cdma2000SectorID,
                          &uplinkS1cdma2000tunnelingIEs->cdma2000SectorID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);

    /* Optional field */
    if ((uplinkS1cdma2000tunnelingIEs->presenceMask & UPLINKS1CDMA2000TUNNELINGIES_CDMA2000HOREQUIREDINDICATION_PRESENT)
        == UPLINKS1CDMA2000TUNNELINGIES_CDMA2000HOREQUIREDINDICATION_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000HORequiredIndication,
                              Criticality_ignore,
                              &asn_DEF_Cdma2000HORequiredIndication,
                              &uplinkS1cdma2000tunnelingIEs->cdma2000HORequiredIndication)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);
    }

    /* Optional field */
    if ((uplinkS1cdma2000tunnelingIEs->presenceMask & UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXSRVCCINFO_PRESENT)
        == UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXSRVCCINFO_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000OneXSRVCCInfo,
                              Criticality_reject,
                              &asn_DEF_Cdma2000OneXSRVCCInfo,
                              &uplinkS1cdma2000tunnelingIEs->cdma2000OneXSRVCCInfo)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);
    }

    /* Optional field */
    if ((uplinkS1cdma2000tunnelingIEs->presenceMask & UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXRAND_PRESENT)
        == UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXRAND_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000OneXRAND,
                              Criticality_reject,
                              &asn_DEF_Cdma2000OneXRAND,
                              &uplinkS1cdma2000tunnelingIEs->cdma2000OneXRAND)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000PDU,
                          Criticality_reject,
                          &asn_DEF_Cdma2000PDU,
                          &uplinkS1cdma2000tunnelingIEs->cdma2000PDU)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);

    /* Optional field */
    if ((uplinkS1cdma2000tunnelingIEs->presenceMask & UPLINKS1CDMA2000TUNNELINGIES_EUTRANROUNDTRIPDELAYESTIMATIONINFO_PRESENT)
        == UPLINKS1CDMA2000TUNNELINGIES_EUTRANROUNDTRIPDELAYESTIMATIONINFO_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_EUTRANRoundTripDelayEstimationInfo,
                              Criticality_ignore,
                              &asn_DEF_EUTRANRoundTripDelayEstimationInfo,
                              &uplinkS1cdma2000tunnelingIEs->eutranRoundTripDelayEstimationInfo)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&uplinkS1cdma2000tunneling->uplinkS1cdma2000tunneling_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_writereplacewarningresponseies(
    WriteReplaceWarningResponse_t *writeReplaceWarningResponse,
    WriteReplaceWarningResponseIEs_t *writeReplaceWarningResponseIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MessageIdentifier,
                          Criticality_reject,
                          &asn_DEF_MessageIdentifier,
                          &writeReplaceWarningResponseIEs->messageIdentifier)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&writeReplaceWarningResponse->writeReplaceWarningResponse_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SerialNumber,
                          Criticality_reject,
                          &asn_DEF_SerialNumber,
                          &writeReplaceWarningResponseIEs->serialNumber)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&writeReplaceWarningResponse->writeReplaceWarningResponse_ies.list, ie);

    /* Optional field */
    if ((writeReplaceWarningResponseIEs->presenceMask & WRITEREPLACEWARNINGRESPONSEIES_BROADCASTCOMPLETEDAREALIST_PRESENT)
        == WRITEREPLACEWARNINGRESPONSEIES_BROADCASTCOMPLETEDAREALIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_BroadcastCompletedAreaList,
                              Criticality_ignore,
                              &asn_DEF_BroadcastCompletedAreaList,
                              &writeReplaceWarningResponseIEs->broadcastCompletedAreaList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningResponse->writeReplaceWarningResponse_ies.list, ie);
    }

    /* Optional field */
    if ((writeReplaceWarningResponseIEs->presenceMask & WRITEREPLACEWARNINGRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == WRITEREPLACEWARNINGRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &writeReplaceWarningResponseIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningResponse->writeReplaceWarningResponse_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_e_rabreleasecommandies(
    E_RABReleaseCommand_t *e_RABReleaseCommand,
    E_RABReleaseCommandIEs_t *e_RABReleaseCommandIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &e_RABReleaseCommandIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseCommand->e_RABReleaseCommand_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &e_RABReleaseCommandIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseCommand->e_RABReleaseCommand_ies.list, ie);

    /* Optional field */
    if ((e_RABReleaseCommandIEs->presenceMask & E_RABRELEASECOMMANDIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == E_RABRELEASECOMMANDIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_uEaggregateMaximumBitrate,
                              Criticality_reject,
                              &asn_DEF_UEAggregateMaximumBitrate,
                              &e_RABReleaseCommandIEs->uEaggregateMaximumBitrate)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABReleaseCommand->e_RABReleaseCommand_ies.list, ie);
    }

    E_RABList_t e_RABToBeReleasedList;

    memset(&e_RABToBeReleasedList, 0, sizeof(E_RABList_t));

    if (s1ap_encode_e_rablist(&e_RABToBeReleasedList, &e_RABReleaseCommandIEs->e_RABToBeReleasedList) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeReleasedList,
                          Criticality_ignore,
                          &asn_DEF_E_RABList,
                          &e_RABToBeReleasedList)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseCommand->e_RABReleaseCommand_ies.list, ie);

    /* Optional field */
    if ((e_RABReleaseCommandIEs->presenceMask & E_RABRELEASECOMMANDIES_NAS_PDU_PRESENT)
        == E_RABRELEASECOMMANDIES_NAS_PDU_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_NAS_PDU,
                              Criticality_ignore,
                              &asn_DEF_NAS_PDU,
                              &e_RABReleaseCommandIEs->nas_pdu)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABReleaseCommand->e_RABReleaseCommand_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_celltraffictraceies(
    CellTrafficTrace_t *cellTrafficTrace,
    CellTrafficTraceIEs_t *cellTrafficTraceIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &cellTrafficTraceIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&cellTrafficTrace->cellTrafficTrace_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &cellTrafficTraceIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&cellTrafficTrace->cellTrafficTrace_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_UTRAN_Trace_ID,
                          Criticality_ignore,
                          &asn_DEF_E_UTRAN_Trace_ID,
                          &cellTrafficTraceIEs->e_UTRAN_Trace_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&cellTrafficTrace->cellTrafficTrace_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_EUTRAN_CGI,
                          Criticality_ignore,
                          &asn_DEF_EUTRAN_CGI,
                          &cellTrafficTraceIEs->eutran_cgi)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&cellTrafficTrace->cellTrafficTrace_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TraceCollectionEntityIPAddress,
                          Criticality_ignore,
                          &asn_DEF_TransportLayerAddress,
                          &cellTrafficTraceIEs->traceCollectionEntityIPAddress)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&cellTrafficTrace->cellTrafficTrace_ies.list, ie);

    return 0;
}

int s1ap_encode_pathswitchrequestfailureies(
    PathSwitchRequestFailure_t *pathSwitchRequestFailure,
    PathSwitchRequestFailureIEs_t *pathSwitchRequestFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &pathSwitchRequestFailureIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequestFailure->pathSwitchRequestFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &pathSwitchRequestFailureIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequestFailure->pathSwitchRequestFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &pathSwitchRequestFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequestFailure->pathSwitchRequestFailure_ies.list, ie);

    /* Optional field */
    if ((pathSwitchRequestFailureIEs->presenceMask & PATHSWITCHREQUESTFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == PATHSWITCHREQUESTFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &pathSwitchRequestFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&pathSwitchRequestFailure->pathSwitchRequestFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_initialcontextsetupresponseies(
    InitialContextSetupResponse_t *initialContextSetupResponse,
    InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &initialContextSetupResponseIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupResponse->initialContextSetupResponse_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &initialContextSetupResponseIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupResponse->initialContextSetupResponse_ies.list, ie);

    E_RABSetupListCtxtSURes_t e_RABSetupListCtxtSURes;

    memset(&e_RABSetupListCtxtSURes, 0, sizeof(E_RABSetupListCtxtSURes_t));

    if (s1ap_encode_e_rabsetuplistctxtsures(&e_RABSetupListCtxtSURes, &initialContextSetupResponseIEs->e_RABSetupListCtxtSURes) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABSetupListCtxtSURes,
                          Criticality_ignore,
                          &asn_DEF_E_RABSetupListCtxtSURes,
                          &e_RABSetupListCtxtSURes)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupResponse->initialContextSetupResponse_ies.list, ie);

    /* Optional field */
    if ((initialContextSetupResponseIEs->presenceMask & INITIALCONTEXTSETUPRESPONSEIES_E_RABLIST_PRESENT)
        == INITIALCONTEXTSETUPRESPONSEIES_E_RABLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABFailedToSetupListCtxtSURes,
                              Criticality_ignore,
                              &asn_DEF_E_RABList,
                              &initialContextSetupResponseIEs->e_RABFailedToSetupListCtxtSURes)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupResponse->initialContextSetupResponse_ies.list, ie);
    }

    /* Optional field */
    if ((initialContextSetupResponseIEs->presenceMask & INITIALCONTEXTSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == INITIALCONTEXTSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &initialContextSetupResponseIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupResponse->initialContextSetupResponse_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_mmestatustransferies(
    MMEStatusTransfer_t *mmeStatusTransfer,
    MMEStatusTransferIEs_t *mmeStatusTransferIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &mmeStatusTransferIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&mmeStatusTransfer->mmeStatusTransfer_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &mmeStatusTransferIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&mmeStatusTransfer->mmeStatusTransfer_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer,
                          Criticality_reject,
                          &asn_DEF_ENB_StatusTransfer_TransparentContainer,
                          &mmeStatusTransferIEs->eNB_StatusTransfer_TransparentContainer)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&mmeStatusTransfer->mmeStatusTransfer_ies.list, ie);

    return 0;
}

int s1ap_encode_s1setuprequesties(
    S1SetupRequest_t *s1SetupRequest,
    S1SetupRequestIEs_t *s1SetupRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Global_ENB_ID,
                          Criticality_reject,
                          &asn_DEF_Global_ENB_ID,
                          &s1SetupRequestIEs->global_ENB_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&s1SetupRequest->s1SetupRequest_ies.list, ie);

    /* Optional field */
    if ((s1SetupRequestIEs->presenceMask & S1SETUPREQUESTIES_ENBNAME_PRESENT)
        == S1SETUPREQUESTIES_ENBNAME_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNBname,
                              Criticality_ignore,
                              &asn_DEF_ENBname,
                              &s1SetupRequestIEs->eNBname)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&s1SetupRequest->s1SetupRequest_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SupportedTAs,
                          Criticality_reject,
                          &asn_DEF_SupportedTAs,
                          &s1SetupRequestIEs->supportedTAs)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&s1SetupRequest->s1SetupRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_DefaultPagingDRX,
                          Criticality_ignore,
                          &asn_DEF_PagingDRX,
                          &s1SetupRequestIEs->defaultPagingDRX)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&s1SetupRequest->s1SetupRequest_ies.list, ie);

    /* Optional field */
    if ((s1SetupRequestIEs->presenceMask & S1SETUPREQUESTIES_CSG_IDLIST_PRESENT)
        == S1SETUPREQUESTIES_CSG_IDLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSG_IdList,
                              Criticality_reject,
                              &asn_DEF_CSG_IdList,
                              &s1SetupRequestIEs->csG_IdList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&s1SetupRequest->s1SetupRequest_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_handoverrequiredies(
    HandoverRequired_t *handoverRequired,
    HandoverRequiredIEs_t *handoverRequiredIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &handoverRequiredIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &handoverRequiredIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_HandoverType,
                          Criticality_reject,
                          &asn_DEF_HandoverType,
                          &handoverRequiredIEs->handoverType)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &handoverRequiredIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TargetID,
                          Criticality_reject,
                          &asn_DEF_TargetID,
                          &handoverRequiredIEs->targetID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);

    /* Optional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_DIRECT_FORWARDING_PATH_AVAILABILITY_PRESENT)
        == HANDOVERREQUIREDIES_DIRECT_FORWARDING_PATH_AVAILABILITY_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Direct_Forwarding_Path_Availability,
                              Criticality_ignore,
                              &asn_DEF_Direct_Forwarding_Path_Availability,
                              &handoverRequiredIEs->direct_Forwarding_Path_Availability)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    /* Optional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_SRVCCHOINDICATION_PRESENT)
        == HANDOVERREQUIREDIES_SRVCCHOINDICATION_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SRVCCHOIndication,
                              Criticality_reject,
                              &asn_DEF_SRVCCHOIndication,
                              &handoverRequiredIEs->srvcchoIndication)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Source_ToTarget_TransparentContainer,
                          Criticality_reject,
                          &asn_DEF_Source_ToTarget_TransparentContainer,
                          &handoverRequiredIEs->source_ToTarget_TransparentContainer)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);

    /* Optional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_SOURCE_TOTARGET_TRANSPARENTCONTAINER_PRESENT)
        == HANDOVERREQUIREDIES_SOURCE_TOTARGET_TRANSPARENTCONTAINER_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Source_ToTarget_TransparentContainer_Secondary,
                              Criticality_reject,
                              &asn_DEF_Source_ToTarget_TransparentContainer,
                              &handoverRequiredIEs->source_ToTarget_TransparentContainer_Secondary)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    /* Conditional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_MSCLASSMARK2_PRESENT)
        == HANDOVERREQUIREDIES_MSCLASSMARK2_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MSClassmark2,
                              Criticality_reject,
                              &asn_DEF_MSClassmark2,
                              &handoverRequiredIEs->msClassmark2)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    /* Conditional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_MSCLASSMARK3_PRESENT)
        == HANDOVERREQUIREDIES_MSCLASSMARK3_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MSClassmark3,
                              Criticality_ignore,
                              &asn_DEF_MSClassmark3,
                              &handoverRequiredIEs->msClassmark3)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    /* Optional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_CSG_ID_PRESENT)
        == HANDOVERREQUIREDIES_CSG_ID_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSG_Id,
                              Criticality_reject,
                              &asn_DEF_CSG_Id,
                              &handoverRequiredIEs->csG_Id)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    /* Optional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_CELLACCESSMODE_PRESENT)
        == HANDOVERREQUIREDIES_CELLACCESSMODE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CellAccessMode,
                              Criticality_reject,
                              &asn_DEF_CellAccessMode,
                              &handoverRequiredIEs->cellAccessMode)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    /* Optional field */
    if ((handoverRequiredIEs->presenceMask & HANDOVERREQUIREDIES_PS_SERVICENOTAVAILABLE_PRESENT)
        == HANDOVERREQUIREDIES_PS_SERVICENOTAVAILABLE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_PS_ServiceNotAvailable,
                              Criticality_ignore,
                              &asn_DEF_PS_ServiceNotAvailable,
                              &handoverRequiredIEs->pS_ServiceNotAvailable)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequired->handoverRequired_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_uplinknastransporties(
    UplinkNASTransport_t *uplinkNASTransport,
    UplinkNASTransportIEs_t *uplinkNASTransportIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &uplinkNASTransportIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkNASTransport->uplinkNASTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &uplinkNASTransportIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkNASTransport->uplinkNASTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_NAS_PDU,
                          Criticality_reject,
                          &asn_DEF_NAS_PDU,
                          &uplinkNASTransportIEs->nas_pdu)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkNASTransport->uplinkNASTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_EUTRAN_CGI,
                          Criticality_ignore,
                          &asn_DEF_EUTRAN_CGI,
                          &uplinkNASTransportIEs->eutran_cgi)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkNASTransport->uplinkNASTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TAI,
                          Criticality_ignore,
                          &asn_DEF_TAI,
                          &uplinkNASTransportIEs->tai)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&uplinkNASTransport->uplinkNASTransport_ies.list, ie);

    return 0;
}

int s1ap_encode_locationreporties(
    LocationReport_t *locationReport,
    LocationReportIEs_t *locationReportIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &locationReportIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReport->locationReport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &locationReportIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReport->locationReport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_EUTRAN_CGI,
                          Criticality_ignore,
                          &asn_DEF_EUTRAN_CGI,
                          &locationReportIEs->eutran_cgi)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReport->locationReport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TAI,
                          Criticality_ignore,
                          &asn_DEF_TAI,
                          &locationReportIEs->tai)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReport->locationReport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RequestType,
                          Criticality_ignore,
                          &asn_DEF_RequestType,
                          &locationReportIEs->requestType)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&locationReport->locationReport_ies.list, ie);

    return 0;
}

int s1ap_encode_uecontextmodificationfailureies(
    UEContextModificationFailure_t *ueContextModificationFailure,
    UEContextModificationFailureIEs_t *ueContextModificationFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &ueContextModificationFailureIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextModificationFailure->ueContextModificationFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &ueContextModificationFailureIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextModificationFailure->ueContextModificationFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &ueContextModificationFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextModificationFailure->ueContextModificationFailure_ies.list, ie);

    /* Optional field */
    if ((ueContextModificationFailureIEs->presenceMask & UECONTEXTMODIFICATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == UECONTEXTMODIFICATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &ueContextModificationFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&ueContextModificationFailure->ueContextModificationFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_enbconfigurationtransferies(
    ENBConfigurationTransfer_t *eNBConfigurationTransfer,
    ENBConfigurationTransferIEs_t *enbConfigurationTransferIEs) {

    IE_t *ie;

    /* Optional field */
    if ((enbConfigurationTransferIEs->presenceMask & ENBCONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT)
        == ENBCONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SONConfigurationTransferECT,
                              Criticality_ignore,
                              &asn_DEF_SONConfigurationTransfer,
                              &enbConfigurationTransferIEs->sonConfigurationTransferECT)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationTransfer->eNBConfigurationTransfer_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_downlinknonueassociatedlppatransporties(
    DownlinkNonUEAssociatedLPPaTransport_t *downlinkNonUEAssociatedLPPaTransport,
    DownlinkNonUEAssociatedLPPaTransportIEs_t *downlinkNonUEAssociatedLPPaTransportIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Routing_ID,
                          Criticality_reject,
                          &asn_DEF_Routing_ID,
                          &downlinkNonUEAssociatedLPPaTransportIEs->routing_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkNonUEAssociatedLPPaTransport->downlinkNonUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_LPPa_PDU,
                          Criticality_reject,
                          &asn_DEF_LPPa_PDU,
                          &downlinkNonUEAssociatedLPPaTransportIEs->lpPa_PDU)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkNonUEAssociatedLPPaTransport->downlinkNonUEAssociatedLPPaTransport_ies.list, ie);

    return 0;
}

int s1ap_encode_initialuemessageies(
    InitialUEMessage_t *initialUEMessage,
    InitialUEMessageIEs_t *initialUEMessageIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &initialUEMessageIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_NAS_PDU,
                          Criticality_reject,
                          &asn_DEF_NAS_PDU,
                          &initialUEMessageIEs->nas_pdu)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TAI,
                          Criticality_reject,
                          &asn_DEF_TAI,
                          &initialUEMessageIEs->tai)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_EUTRAN_CGI,
                          Criticality_ignore,
                          &asn_DEF_EUTRAN_CGI,
                          &initialUEMessageIEs->eutran_cgi)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RRC_Establishment_Cause,
                          Criticality_ignore,
                          &asn_DEF_RRC_Establishment_Cause,
                          &initialUEMessageIEs->rrC_Establishment_Cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);

    /* Optional field */
    if ((initialUEMessageIEs->presenceMask & INITIALUEMESSAGEIES_S_TMSI_PRESENT)
        == INITIALUEMESSAGEIES_S_TMSI_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_S_TMSI,
                              Criticality_reject,
                              &asn_DEF_S_TMSI,
                              &initialUEMessageIEs->s_tmsi)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);
    }

    /* Optional field */
    if ((initialUEMessageIEs->presenceMask & INITIALUEMESSAGEIES_CSG_ID_PRESENT)
        == INITIALUEMESSAGEIES_CSG_ID_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSG_Id,
                              Criticality_reject,
                              &asn_DEF_CSG_Id,
                              &initialUEMessageIEs->csG_Id)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);
    }

    /* Optional field */
    if ((initialUEMessageIEs->presenceMask & INITIALUEMESSAGEIES_GUMMEI_PRESENT)
        == INITIALUEMESSAGEIES_GUMMEI_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_GUMMEI_ID,
                              Criticality_reject,
                              &asn_DEF_GUMMEI,
                              &initialUEMessageIEs->gummei_id)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);
    }

    /* Optional field */
    if ((initialUEMessageIEs->presenceMask & INITIALUEMESSAGEIES_CELLACCESSMODE_PRESENT)
        == INITIALUEMESSAGEIES_CELLACCESSMODE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CellAccessMode,
                              Criticality_reject,
                              &asn_DEF_CellAccessMode,
                              &initialUEMessageIEs->cellAccessMode)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialUEMessage->initialUEMessage_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_e_rabsetupresponseies(
    E_RABSetupResponse_t *e_RABSetupResponse,
    E_RABSetupResponseIEs_t *e_RABSetupResponseIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &e_RABSetupResponseIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABSetupResponse->e_RABSetupResponse_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &e_RABSetupResponseIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABSetupResponse->e_RABSetupResponse_ies.list, ie);

    /* Optional field */
    if ((e_RABSetupResponseIEs->presenceMask & E_RABSETUPRESPONSEIES_E_RABSETUPLISTBEARERSURES_PRESENT)
        == E_RABSETUPRESPONSEIES_E_RABSETUPLISTBEARERSURES_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABSetupListBearerSURes,
                              Criticality_ignore,
                              &asn_DEF_E_RABSetupListBearerSURes,
                              &e_RABSetupResponseIEs->e_RABSetupListBearerSURes)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABSetupResponse->e_RABSetupResponse_ies.list, ie);
    }

    /* Optional field */
    if ((e_RABSetupResponseIEs->presenceMask & E_RABSETUPRESPONSEIES_E_RABLIST_PRESENT)
        == E_RABSETUPRESPONSEIES_E_RABLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABFailedToSetupListBearerSURes,
                              Criticality_ignore,
                              &asn_DEF_E_RABList,
                              &e_RABSetupResponseIEs->e_RABFailedToSetupListBearerSURes)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABSetupResponse->e_RABSetupResponse_ies.list, ie);
    }

    /* Optional field */
    if ((e_RABSetupResponseIEs->presenceMask & E_RABSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == E_RABSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &e_RABSetupResponseIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABSetupResponse->e_RABSetupResponse_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_pathswitchrequestacknowledgeies(
    PathSwitchRequestAcknowledge_t *pathSwitchRequestAcknowledge,
    PathSwitchRequestAcknowledgeIEs_t *pathSwitchRequestAcknowledgeIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &pathSwitchRequestAcknowledgeIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequestAcknowledge->pathSwitchRequestAcknowledge_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &pathSwitchRequestAcknowledgeIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequestAcknowledge->pathSwitchRequestAcknowledge_ies.list, ie);

    /* Optional field */
    if ((pathSwitchRequestAcknowledgeIEs->presenceMask & PATHSWITCHREQUESTACKNOWLEDGEIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == PATHSWITCHREQUESTACKNOWLEDGEIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_uEaggregateMaximumBitrate,
                              Criticality_ignore,
                              &asn_DEF_UEAggregateMaximumBitrate,
                              &pathSwitchRequestAcknowledgeIEs->uEaggregateMaximumBitrate)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&pathSwitchRequestAcknowledge->pathSwitchRequestAcknowledge_ies.list, ie);
    }

    /* Optional field */
    if ((pathSwitchRequestAcknowledgeIEs->presenceMask & PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBESWITCHEDULLIST_PRESENT)
        == PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBESWITCHEDULLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSwitchedULList,
                              Criticality_ignore,
                              &asn_DEF_E_RABToBeSwitchedULList,
                              &pathSwitchRequestAcknowledgeIEs->e_RABToBeSwitchedULList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&pathSwitchRequestAcknowledge->pathSwitchRequestAcknowledge_ies.list, ie);
    }

    /* Optional field */
    if ((pathSwitchRequestAcknowledgeIEs->presenceMask & PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABLIST_PRESENT)
        == PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeReleasedList,
                              Criticality_ignore,
                              &asn_DEF_E_RABList,
                              &pathSwitchRequestAcknowledgeIEs->e_RABToBeReleasedList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&pathSwitchRequestAcknowledge->pathSwitchRequestAcknowledge_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SecurityContext,
                          Criticality_reject,
                          &asn_DEF_SecurityContext,
                          &pathSwitchRequestAcknowledgeIEs->securityContext)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&pathSwitchRequestAcknowledge->pathSwitchRequestAcknowledge_ies.list, ie);

    /* Optional field */
    if ((pathSwitchRequestAcknowledgeIEs->presenceMask & PATHSWITCHREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == PATHSWITCHREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &pathSwitchRequestAcknowledgeIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&pathSwitchRequestAcknowledge->pathSwitchRequestAcknowledge_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_overloadstarties(
    OverloadStart_t *overloadStart,
    OverloadStartIEs_t *overloadStartIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_OverloadResponse,
                          Criticality_reject,
                          &asn_DEF_OverloadResponse,
                          &overloadStartIEs->overloadResponse)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&overloadStart->overloadStart_ies.list, ie);

    return 0;
}

int s1ap_encode_nasnondeliveryindicationies(
    NASNonDeliveryIndication_t *nasNonDeliveryIndication,
    NASNonDeliveryIndicationIEs_t *nasNonDeliveryIndicationIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &nasNonDeliveryIndicationIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&nasNonDeliveryIndication->nasNonDeliveryIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &nasNonDeliveryIndicationIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&nasNonDeliveryIndication->nasNonDeliveryIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_NAS_PDU,
                          Criticality_ignore,
                          &asn_DEF_NAS_PDU,
                          &nasNonDeliveryIndicationIEs->nas_pdu)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&nasNonDeliveryIndication->nasNonDeliveryIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &nasNonDeliveryIndicationIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&nasNonDeliveryIndication->nasNonDeliveryIndication_ies.list, ie);

    return 0;
}

int s1ap_encode_handovercancelacknowledgeies(
    HandoverCancelAcknowledge_t *handoverCancelAcknowledge,
    HandoverCancelAcknowledgeIEs_t *handoverCancelAcknowledgeIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &handoverCancelAcknowledgeIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverCancelAcknowledge->handoverCancelAcknowledge_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &handoverCancelAcknowledgeIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverCancelAcknowledge->handoverCancelAcknowledge_ies.list, ie);

    /* Optional field */
    if ((handoverCancelAcknowledgeIEs->presenceMask & HANDOVERCANCELACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == HANDOVERCANCELACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &handoverCancelAcknowledgeIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverCancelAcknowledge->handoverCancelAcknowledge_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_enbstatustransferies(
    ENBStatusTransfer_t *eNBStatusTransfer,
    ENBStatusTransferIEs_t *enbStatusTransferIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &enbStatusTransferIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&eNBStatusTransfer->eNBStatusTransfer_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &enbStatusTransferIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&eNBStatusTransfer->eNBStatusTransfer_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer,
                          Criticality_reject,
                          &asn_DEF_ENB_StatusTransfer_TransparentContainer,
                          &enbStatusTransferIEs->eNB_StatusTransfer_TransparentContainer)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&eNBStatusTransfer->eNBStatusTransfer_ies.list, ie);

    return 0;
}

int s1ap_encode_handovercancelies(
    HandoverCancel_t *handoverCancel,
    HandoverCancelIEs_t *handoverCancelIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &handoverCancelIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverCancel->handoverCancel_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &handoverCancelIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverCancel->handoverCancel_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &handoverCancelIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverCancel->handoverCancel_ies.list, ie);

    return 0;
}

int s1ap_encode_e_rabreleaseresponseies(
    E_RABReleaseResponse_t *e_RABReleaseResponse,
    E_RABReleaseResponseIEs_t *e_RABReleaseResponseIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &e_RABReleaseResponseIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseResponse->e_RABReleaseResponse_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &e_RABReleaseResponseIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseResponse->e_RABReleaseResponse_ies.list, ie);

    /* Optional field */
    if ((e_RABReleaseResponseIEs->presenceMask & E_RABRELEASERESPONSEIES_E_RABRELEASELISTBEARERRELCOMP_PRESENT)
        == E_RABRELEASERESPONSEIES_E_RABRELEASELISTBEARERRELCOMP_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABReleaseListBearerRelComp,
                              Criticality_ignore,
                              &asn_DEF_E_RABReleaseListBearerRelComp,
                              &e_RABReleaseResponseIEs->e_RABReleaseListBearerRelComp)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABReleaseResponse->e_RABReleaseResponse_ies.list, ie);
    }

    /* Optional field */
    if ((e_RABReleaseResponseIEs->presenceMask & E_RABRELEASERESPONSEIES_E_RABLIST_PRESENT)
        == E_RABRELEASERESPONSEIES_E_RABLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABFailedToReleaseList,
                              Criticality_ignore,
                              &asn_DEF_E_RABList,
                              &e_RABReleaseResponseIEs->e_RABFailedToReleaseList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABReleaseResponse->e_RABReleaseResponse_ies.list, ie);
    }

    /* Optional field */
    if ((e_RABReleaseResponseIEs->presenceMask & E_RABRELEASERESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == E_RABRELEASERESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &e_RABReleaseResponseIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABReleaseResponse->e_RABReleaseResponse_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_uecontextreleaserequesties(
    UEContextReleaseRequest_t *ueContextReleaseRequest,
    UEContextReleaseRequestIEs_t *ueContextReleaseRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &ueContextReleaseRequestIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextReleaseRequest->ueContextReleaseRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &ueContextReleaseRequestIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextReleaseRequest->ueContextReleaseRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &ueContextReleaseRequestIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&ueContextReleaseRequest->ueContextReleaseRequest_ies.list, ie);

    return 0;
}

int s1ap_encode_tracestarties(
    TraceStart_t *traceStart,
    TraceStartIEs_t *traceStartIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &traceStartIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&traceStart->traceStart_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &traceStartIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&traceStart->traceStart_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TraceActivation,
                          Criticality_ignore,
                          &asn_DEF_TraceActivation,
                          &traceStartIEs->traceActivation)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&traceStart->traceStart_ies.list, ie);

    return 0;
}

int s1ap_encode_mmeconfigurationtransferies(
    MMEConfigurationTransfer_t *mmeConfigurationTransfer,
    MMEConfigurationTransferIEs_t *mmeConfigurationTransferIEs) {

    IE_t *ie;

    /* Optional field */
    if ((mmeConfigurationTransferIEs->presenceMask & MMECONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT)
        == MMECONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SONConfigurationTransferMCT,
                              Criticality_ignore,
                              &asn_DEF_SONConfigurationTransfer,
                              &mmeConfigurationTransferIEs->sonConfigurationTransferMCT)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&mmeConfigurationTransfer->mmeConfigurationTransfer_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_tracefailureindicationies(
    TraceFailureIndication_t *traceFailureIndication,
    TraceFailureIndicationIEs_t *traceFailureIndicationIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &traceFailureIndicationIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&traceFailureIndication->traceFailureIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &traceFailureIndicationIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&traceFailureIndication->traceFailureIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_UTRAN_Trace_ID,
                          Criticality_ignore,
                          &asn_DEF_E_UTRAN_Trace_ID,
                          &traceFailureIndicationIEs->e_UTRAN_Trace_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&traceFailureIndication->traceFailureIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &traceFailureIndicationIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&traceFailureIndication->traceFailureIndication_ies.list, ie);

    return 0;
}

int s1ap_encode_initialcontextsetupfailureies(
    InitialContextSetupFailure_t *initialContextSetupFailure,
    InitialContextSetupFailureIEs_t *initialContextSetupFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &initialContextSetupFailureIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupFailure->initialContextSetupFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &initialContextSetupFailureIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupFailure->initialContextSetupFailure_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &initialContextSetupFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&initialContextSetupFailure->initialContextSetupFailure_ies.list, ie);

    /* Optional field */
    if ((initialContextSetupFailureIEs->presenceMask & INITIALCONTEXTSETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == INITIALCONTEXTSETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &initialContextSetupFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&initialContextSetupFailure->initialContextSetupFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_errorindicationies(
    ErrorIndication_t *errorIndication,
    ErrorIndicationIEs_t *errorIndicationIEs) {

    IE_t *ie;

    /* Optional field */
    if ((errorIndicationIEs->presenceMask & ERRORINDICATIONIES_MME_UE_S1AP_ID_PRESENT)
        == ERRORINDICATIONIES_MME_UE_S1AP_ID_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                              Criticality_ignore,
                              &asn_DEF_MME_UE_S1AP_ID,
                              &errorIndicationIEs->mme_ue_s1ap_id)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&errorIndication->errorIndication_ies.list, ie);
    }

    /* Optional field */
    if ((errorIndicationIEs->presenceMask & ERRORINDICATIONIES_ENB_UE_S1AP_ID_PRESENT)
        == ERRORINDICATIONIES_ENB_UE_S1AP_ID_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                              Criticality_ignore,
                              &asn_DEF_ENB_UE_S1AP_ID,
                              &errorIndicationIEs->eNB_UE_S1AP_ID)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&errorIndication->errorIndication_ies.list, ie);
    }

    /* Optional field */
    if ((errorIndicationIEs->presenceMask & ERRORINDICATIONIES_CAUSE_PRESENT)
        == ERRORINDICATIONIES_CAUSE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                              Criticality_ignore,
                              &asn_DEF_Cause,
                              &errorIndicationIEs->cause)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&errorIndication->errorIndication_ies.list, ie);
    }

    /* Optional field */
    if ((errorIndicationIEs->presenceMask & ERRORINDICATIONIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == ERRORINDICATIONIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &errorIndicationIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&errorIndication->errorIndication_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_handovernotifyies(
    HandoverNotify_t *handoverNotify,
    HandoverNotifyIEs_t *handoverNotifyIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &handoverNotifyIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverNotify->handoverNotify_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &handoverNotifyIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverNotify->handoverNotify_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_EUTRAN_CGI,
                          Criticality_ignore,
                          &asn_DEF_EUTRAN_CGI,
                          &handoverNotifyIEs->eutran_cgi)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverNotify->handoverNotify_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TAI,
                          Criticality_ignore,
                          &asn_DEF_TAI,
                          &handoverNotifyIEs->tai)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverNotify->handoverNotify_ies.list, ie);

    return 0;
}

int s1ap_encode_downlinks1cdma2000tunnelingies(
    DownlinkS1cdma2000tunneling_t *downlinkS1cdma2000tunneling,
    DownlinkS1cdma2000tunnelingIEs_t *downlinkS1cdma2000tunnelingIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &downlinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkS1cdma2000tunneling->downlinkS1cdma2000tunneling_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &downlinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkS1cdma2000tunneling->downlinkS1cdma2000tunneling_ies.list, ie);

    /* Optional field */
    if ((downlinkS1cdma2000tunnelingIEs->presenceMask & DOWNLINKS1CDMA2000TUNNELINGIES_E_RABDATAFORWARDINGLIST_PRESENT)
        == DOWNLINKS1CDMA2000TUNNELINGIES_E_RABDATAFORWARDINGLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABSubjecttoDataForwardingList,
                              Criticality_ignore,
                              &asn_DEF_E_RABDataForwardingList,
                              &downlinkS1cdma2000tunnelingIEs->e_RABSubjecttoDataForwardingList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&downlinkS1cdma2000tunneling->downlinkS1cdma2000tunneling_ies.list, ie);
    }

    /* Optional field */
    if ((downlinkS1cdma2000tunnelingIEs->presenceMask & DOWNLINKS1CDMA2000TUNNELINGIES_CDMA2000HOSTATUS_PRESENT)
        == DOWNLINKS1CDMA2000TUNNELINGIES_CDMA2000HOSTATUS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000HOStatus,
                              Criticality_ignore,
                              &asn_DEF_Cdma2000HOStatus,
                              &downlinkS1cdma2000tunnelingIEs->cdma2000HOStatus)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&downlinkS1cdma2000tunneling->downlinkS1cdma2000tunneling_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000RATType,
                          Criticality_reject,
                          &asn_DEF_Cdma2000RATType,
                          &downlinkS1cdma2000tunnelingIEs->cdma2000RATType)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkS1cdma2000tunneling->downlinkS1cdma2000tunneling_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_cdma2000PDU,
                          Criticality_reject,
                          &asn_DEF_Cdma2000PDU,
                          &downlinkS1cdma2000tunnelingIEs->cdma2000PDU)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkS1cdma2000tunneling->downlinkS1cdma2000tunneling_ies.list, ie);

    return 0;
}

int s1ap_encode_mmeconfigurationupdateacknowledgeies(
    MMEConfigurationUpdateAcknowledge_t *mmeConfigurationUpdateAcknowledge,
    MMEConfigurationUpdateAcknowledgeIEs_t *mmeConfigurationUpdateAcknowledgeIEs) {

    IE_t *ie;

    /* Optional field */
    if ((mmeConfigurationUpdateAcknowledgeIEs->presenceMask & MMECONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == MMECONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &mmeConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&mmeConfigurationUpdateAcknowledge->mmeConfigurationUpdateAcknowledge_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_handoverrequestacknowledgeies(
    HandoverRequestAcknowledge_t *handoverRequestAcknowledge,
    HandoverRequestAcknowledgeIEs_t *handoverRequestAcknowledgeIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &handoverRequestAcknowledgeIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequestAcknowledge->handoverRequestAcknowledge_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &handoverRequestAcknowledgeIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequestAcknowledge->handoverRequestAcknowledge_ies.list, ie);

    E_RABAdmittedList_t e_RABAdmittedList;

    memset(&e_RABAdmittedList, 0, sizeof(E_RABAdmittedList_t));

    if (s1ap_encode_e_rabadmittedlist(&e_RABAdmittedList, &handoverRequestAcknowledgeIEs->e_RABAdmittedList) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABAdmittedList,
                          Criticality_ignore,
                          &asn_DEF_E_RABAdmittedList,
                          &e_RABAdmittedList)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequestAcknowledge->handoverRequestAcknowledge_ies.list, ie);

    /* Optional field */
    if ((handoverRequestAcknowledgeIEs->presenceMask & HANDOVERREQUESTACKNOWLEDGEIES_E_RABFAILEDTOSETUPLISTHOREQACK_PRESENT)
        == HANDOVERREQUESTACKNOWLEDGEIES_E_RABFAILEDTOSETUPLISTHOREQACK_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABFailedToSetupListHOReqAck,
                              Criticality_ignore,
                              &asn_DEF_E_RABFailedtoSetupListHOReqAck,
                              &handoverRequestAcknowledgeIEs->e_RABFailedToSetupListHOReqAck)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequestAcknowledge->handoverRequestAcknowledge_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Target_ToSource_TransparentContainer,
                          Criticality_reject,
                          &asn_DEF_Target_ToSource_TransparentContainer,
                          &handoverRequestAcknowledgeIEs->target_ToSource_TransparentContainer)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&handoverRequestAcknowledge->handoverRequestAcknowledge_ies.list, ie);

    /* Optional field */
    if ((handoverRequestAcknowledgeIEs->presenceMask & HANDOVERREQUESTACKNOWLEDGEIES_CSG_ID_PRESENT)
        == HANDOVERREQUESTACKNOWLEDGEIES_CSG_ID_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CSG_Id,
                              Criticality_ignore,
                              &asn_DEF_CSG_Id,
                              &handoverRequestAcknowledgeIEs->csG_Id)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequestAcknowledge->handoverRequestAcknowledge_ies.list, ie);
    }

    /* Optional field */
    if ((handoverRequestAcknowledgeIEs->presenceMask & HANDOVERREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == HANDOVERREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &handoverRequestAcknowledgeIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&handoverRequestAcknowledge->handoverRequestAcknowledge_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_e_rabsetuprequesties(
    E_RABSetupRequest_t *e_RABSetupRequest,
    E_RABSetupRequestIEs_t *e_RABSetupRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &e_RABSetupRequestIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABSetupRequest->e_RABSetupRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &e_RABSetupRequestIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABSetupRequest->e_RABSetupRequest_ies.list, ie);

    /* Optional field */
    if ((e_RABSetupRequestIEs->presenceMask & E_RABSETUPREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == E_RABSETUPREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_uEaggregateMaximumBitrate,
                              Criticality_reject,
                              &asn_DEF_UEAggregateMaximumBitrate,
                              &e_RABSetupRequestIEs->uEaggregateMaximumBitrate)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABSetupRequest->e_RABSetupRequest_ies.list, ie);
    }

    E_RABToBeSetupListBearerSUReq_t e_RABToBeSetupListBearerSUReq;

    memset(&e_RABToBeSetupListBearerSUReq, 0, sizeof(E_RABToBeSetupListBearerSUReq_t));

    if (s1ap_encode_e_rabtobesetuplistbearersureq(&e_RABToBeSetupListBearerSUReq, &e_RABSetupRequestIEs->e_RABToBeSetupListBearerSUReq) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSetupListBearerSUReq,
                          Criticality_reject,
                          &asn_DEF_E_RABToBeSetupListBearerSUReq,
                          &e_RABToBeSetupListBearerSUReq)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABSetupRequest->e_RABSetupRequest_ies.list, ie);

    return 0;
}

int s1ap_encode_downlinkueassociatedlppatransporties(
    DownlinkUEAssociatedLPPaTransport_t *downlinkUEAssociatedLPPaTransport,
    DownlinkUEAssociatedLPPaTransportIEs_t *downlinkUEAssociatedLPPaTransportIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &downlinkUEAssociatedLPPaTransportIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkUEAssociatedLPPaTransport->downlinkUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &downlinkUEAssociatedLPPaTransportIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkUEAssociatedLPPaTransport->downlinkUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Routing_ID,
                          Criticality_reject,
                          &asn_DEF_Routing_ID,
                          &downlinkUEAssociatedLPPaTransportIEs->routing_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkUEAssociatedLPPaTransport->downlinkUEAssociatedLPPaTransport_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_LPPa_PDU,
                          Criticality_reject,
                          &asn_DEF_LPPa_PDU,
                          &downlinkUEAssociatedLPPaTransportIEs->lpPa_PDU)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&downlinkUEAssociatedLPPaTransport->downlinkUEAssociatedLPPaTransport_ies.list, ie);

    return 0;
}

int s1ap_encode_killrequesties(
    KillRequest_t *killRequest,
    KillRequestIEs_t *killRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MessageIdentifier,
                          Criticality_reject,
                          &asn_DEF_MessageIdentifier,
                          &killRequestIEs->messageIdentifier)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&killRequest->killRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SerialNumber,
                          Criticality_reject,
                          &asn_DEF_SerialNumber,
                          &killRequestIEs->serialNumber)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&killRequest->killRequest_ies.list, ie);

    /* Optional field */
    if ((killRequestIEs->presenceMask & KILLREQUESTIES_WARNINGAREALIST_PRESENT)
        == KILLREQUESTIES_WARNINGAREALIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_WarningAreaList,
                              Criticality_ignore,
                              &asn_DEF_WarningAreaList,
                              &killRequestIEs->warningAreaList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&killRequest->killRequest_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_e_rabmodifyresponseies(
    E_RABModifyResponse_t *e_RABModifyResponse,
    E_RABModifyResponseIEs_t *e_RABModifyResponseIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &e_RABModifyResponseIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABModifyResponse->e_RABModifyResponse_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_ignore,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &e_RABModifyResponseIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABModifyResponse->e_RABModifyResponse_ies.list, ie);

    /* Optional field */
    if ((e_RABModifyResponseIEs->presenceMask & E_RABMODIFYRESPONSEIES_E_RABMODIFYLISTBEARERMODRES_PRESENT)
        == E_RABMODIFYRESPONSEIES_E_RABMODIFYLISTBEARERMODRES_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABModifyListBearerModRes,
                              Criticality_ignore,
                              &asn_DEF_E_RABModifyListBearerModRes,
                              &e_RABModifyResponseIEs->e_RABModifyListBearerModRes)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABModifyResponse->e_RABModifyResponse_ies.list, ie);
    }

    /* Optional field */
    if ((e_RABModifyResponseIEs->presenceMask & E_RABMODIFYRESPONSEIES_E_RABLIST_PRESENT)
        == E_RABMODIFYRESPONSEIES_E_RABLIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABFailedToModifyList,
                              Criticality_ignore,
                              &asn_DEF_E_RABList,
                              &e_RABModifyResponseIEs->e_RABFailedToModifyList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABModifyResponse->e_RABModifyResponse_ies.list, ie);
    }

    /* Optional field */
    if ((e_RABModifyResponseIEs->presenceMask & E_RABMODIFYRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == E_RABMODIFYRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &e_RABModifyResponseIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABModifyResponse->e_RABModifyResponse_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_e_rabreleaseindicationies(
    E_RABReleaseIndication_t *e_RABReleaseIndication,
    E_RABReleaseIndicationIEs_t *e_RABReleaseIndicationIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &e_RABReleaseIndicationIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseIndication->e_RABReleaseIndication_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &e_RABReleaseIndicationIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseIndication->e_RABReleaseIndication_ies.list, ie);

    E_RABList_t e_RABReleasedList;

    memset(&e_RABReleasedList, 0, sizeof(E_RABList_t));

    if (s1ap_encode_e_rablist(&e_RABReleasedList, &e_RABReleaseIndicationIEs->e_RABReleasedList) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABReleasedList,
                          Criticality_ignore,
                          &asn_DEF_E_RABList,
                          &e_RABReleasedList)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABReleaseIndication->e_RABReleaseIndication_ies.list, ie);

    return 0;
}

int s1ap_encode_enbconfigurationupdatefailureies(
    ENBConfigurationUpdateFailure_t *eNBConfigurationUpdateFailure,
    ENBConfigurationUpdateFailureIEs_t *enbConfigurationUpdateFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &enbConfigurationUpdateFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&eNBConfigurationUpdateFailure->eNBConfigurationUpdateFailure_ies.list, ie);

    /* Optional field */
    if ((enbConfigurationUpdateFailureIEs->presenceMask & ENBCONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT)
        == ENBCONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TimeToWait,
                              Criticality_ignore,
                              &asn_DEF_TimeToWait,
                              &enbConfigurationUpdateFailureIEs->timeToWait)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationUpdateFailure->eNBConfigurationUpdateFailure_ies.list, ie);
    }

    /* Optional field */
    if ((enbConfigurationUpdateFailureIEs->presenceMask & ENBCONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == ENBCONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &enbConfigurationUpdateFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&eNBConfigurationUpdateFailure->eNBConfigurationUpdateFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_writereplacewarningrequesties(
    WriteReplaceWarningRequest_t *writeReplaceWarningRequest,
    WriteReplaceWarningRequestIEs_t *writeReplaceWarningRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MessageIdentifier,
                          Criticality_reject,
                          &asn_DEF_MessageIdentifier,
                          &writeReplaceWarningRequestIEs->messageIdentifier)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_SerialNumber,
                          Criticality_reject,
                          &asn_DEF_SerialNumber,
                          &writeReplaceWarningRequestIEs->serialNumber)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);

    /* Optional field */
    if ((writeReplaceWarningRequestIEs->presenceMask & WRITEREPLACEWARNINGREQUESTIES_WARNINGAREALIST_PRESENT)
        == WRITEREPLACEWARNINGREQUESTIES_WARNINGAREALIST_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_WarningAreaList,
                              Criticality_ignore,
                              &asn_DEF_WarningAreaList,
                              &writeReplaceWarningRequestIEs->warningAreaList)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_RepetitionPeriod,
                          Criticality_reject,
                          &asn_DEF_RepetitionPeriod,
                          &writeReplaceWarningRequestIEs->repetitionPeriod)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);

    /* Optional field */
    if ((writeReplaceWarningRequestIEs->presenceMask & WRITEREPLACEWARNINGREQUESTIES_EXTENDEDREPETITIONPERIOD_PRESENT)
        == WRITEREPLACEWARNINGREQUESTIES_EXTENDEDREPETITIONPERIOD_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_ExtendedRepetitionPeriod,
                              Criticality_reject,
                              &asn_DEF_ExtendedRepetitionPeriod,
                              &writeReplaceWarningRequestIEs->extendedRepetitionPeriod)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);
    }

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_NumberofBroadcastRequest,
                          Criticality_reject,
                          &asn_DEF_NumberofBroadcastRequest,
                          &writeReplaceWarningRequestIEs->numberofBroadcastRequest)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);

    /* Optional field */
    if ((writeReplaceWarningRequestIEs->presenceMask & WRITEREPLACEWARNINGREQUESTIES_WARNINGTYPE_PRESENT)
        == WRITEREPLACEWARNINGREQUESTIES_WARNINGTYPE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_WarningType,
                              Criticality_ignore,
                              &asn_DEF_WarningType,
                              &writeReplaceWarningRequestIEs->warningType)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);
    }

    /* Optional field */
    if ((writeReplaceWarningRequestIEs->presenceMask & WRITEREPLACEWARNINGREQUESTIES_WARNINGSECURITYINFO_PRESENT)
        == WRITEREPLACEWARNINGREQUESTIES_WARNINGSECURITYINFO_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_WarningSecurityInfo,
                              Criticality_ignore,
                              &asn_DEF_WarningSecurityInfo,
                              &writeReplaceWarningRequestIEs->warningSecurityInfo)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);
    }

    /* Optional field */
    if ((writeReplaceWarningRequestIEs->presenceMask & WRITEREPLACEWARNINGREQUESTIES_DATACODINGSCHEME_PRESENT)
        == WRITEREPLACEWARNINGREQUESTIES_DATACODINGSCHEME_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_DataCodingScheme,
                              Criticality_ignore,
                              &asn_DEF_DataCodingScheme,
                              &writeReplaceWarningRequestIEs->dataCodingScheme)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);
    }

    /* Optional field */
    if ((writeReplaceWarningRequestIEs->presenceMask & WRITEREPLACEWARNINGREQUESTIES_WARNINGMESSAGECONTENTS_PRESENT)
        == WRITEREPLACEWARNINGREQUESTIES_WARNINGMESSAGECONTENTS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_WarningMessageContents,
                              Criticality_ignore,
                              &asn_DEF_WarningMessageContents,
                              &writeReplaceWarningRequestIEs->warningMessageContents)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);
    }

    /* Optional field */
    if ((writeReplaceWarningRequestIEs->presenceMask & WRITEREPLACEWARNINGREQUESTIES_CONCURRENTWARNINGMESSAGEINDICATOR_PRESENT)
        == WRITEREPLACEWARNINGREQUESTIES_CONCURRENTWARNINGMESSAGEINDICATOR_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_ConcurrentWarningMessageIndicator,
                              Criticality_reject,
                              &asn_DEF_ConcurrentWarningMessageIndicator,
                              &writeReplaceWarningRequestIEs->concurrentWarningMessageIndicator)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&writeReplaceWarningRequest->writeReplaceWarningRequest_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_s1setupfailureies(
    S1SetupFailure_t *s1SetupFailure,
    S1SetupFailureIEs_t *s1SetupFailureIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Cause,
                          Criticality_ignore,
                          &asn_DEF_Cause,
                          &s1SetupFailureIEs->cause)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&s1SetupFailure->s1SetupFailure_ies.list, ie);

    /* Optional field */
    if ((s1SetupFailureIEs->presenceMask & S1SETUPFAILUREIES_TIMETOWAIT_PRESENT)
        == S1SETUPFAILUREIES_TIMETOWAIT_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TimeToWait,
                              Criticality_ignore,
                              &asn_DEF_TimeToWait,
                              &s1SetupFailureIEs->timeToWait)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&s1SetupFailure->s1SetupFailure_ies.list, ie);
    }

    /* Optional field */
    if ((s1SetupFailureIEs->presenceMask & S1SETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT)
        == S1SETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_CriticalityDiagnostics,
                              Criticality_ignore,
                              &asn_DEF_CriticalityDiagnostics,
                              &s1SetupFailureIEs->criticalityDiagnostics)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&s1SetupFailure->s1SetupFailure_ies.list, ie);
    }

    return 0;
}

int s1ap_encode_e_rabmodifyrequesties(
    E_RABModifyRequest_t *e_RABModifyRequest,
    E_RABModifyRequestIEs_t *e_RABModifyRequestIEs) {

    IE_t *ie;

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_MME_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_MME_UE_S1AP_ID,
                          &e_RABModifyRequestIEs->mme_ue_s1ap_id)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABModifyRequest->e_RABModifyRequest_ies.list, ie);

    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_eNB_UE_S1AP_ID,
                          Criticality_reject,
                          &asn_DEF_ENB_UE_S1AP_ID,
                          &e_RABModifyRequestIEs->eNB_UE_S1AP_ID)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABModifyRequest->e_RABModifyRequest_ies.list, ie);

    /* Optional field */
    if ((e_RABModifyRequestIEs->presenceMask & E_RABMODIFYREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT)
        == E_RABMODIFYREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_uEaggregateMaximumBitrate,
                              Criticality_reject,
                              &asn_DEF_UEAggregateMaximumBitrate,
                              &e_RABModifyRequestIEs->uEaggregateMaximumBitrate)) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABModifyRequest->e_RABModifyRequest_ies.list, ie);
    }

    E_RABToBeModifiedListBearerModReq_t e_RABToBeModifiedListBearerModReq;

    memset(&e_RABToBeModifiedListBearerModReq, 0, sizeof(E_RABToBeModifiedListBearerModReq_t));

    if (s1ap_encode_e_rabtobemodifiedlistbearermodreq(&e_RABToBeModifiedListBearerModReq, &e_RABModifyRequestIEs->e_RABToBeModifiedListBearerModReq) < 0) return -1;
    if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeModifiedListBearerModReq,
                          Criticality_reject,
                          &asn_DEF_E_RABToBeModifiedListBearerModReq,
                          &e_RABToBeModifiedListBearerModReq)) == NULL) {
        return -1;
    }
    ASN_SEQUENCE_ADD(&e_RABModifyRequest->e_RABModifyRequest_ies.list, ie);

    return 0;
}

int s1ap_encode_e_rabtobesetuplistbearersureq(
    E_RABToBeSetupListBearerSUReq_t *e_RABToBeSetupListBearerSUReq,
    E_RABToBeSetupListBearerSUReqIEs_t *e_RABToBeSetupListBearerSUReqIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABToBeSetupListBearerSUReqIEs->e_RABToBeSetupItemBearerSUReq.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSetupItemBearerSUReq,
                              Criticality_reject,
                              &asn_DEF_E_RABToBeSetupItemBearerSUReq,
                              e_RABToBeSetupListBearerSUReqIEs->e_RABToBeSetupItemBearerSUReq.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABToBeSetupListBearerSUReq->list, ie);
    }
    return 0;
}

int s1ap_encode_ue_associatedlogicals1_connectionlistresack(
    UE_associatedLogicalS1_ConnectionListResAck_t *uE_associatedLogicalS1_ConnectionListResAck,
    UE_associatedLogicalS1_ConnectionListResAckIEs_t *uE_associatedLogicalS1_ConnectionListResAckIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < uE_associatedLogicalS1_ConnectionListResAckIEs->uE_associatedLogicalS1_ConnectionItemResAck.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem,
                              Criticality_ignore,
                              &asn_DEF_UE_associatedLogicalS1_ConnectionItem,
                              uE_associatedLogicalS1_ConnectionListResAckIEs->uE_associatedLogicalS1_ConnectionItemResAck.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&uE_associatedLogicalS1_ConnectionListResAck->list, ie);
    }
    return 0;
}

int s1ap_encode_tailist(
    TAIList_t *taiList,
    TAIListIEs_t *taiListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < taiListIEs->taiItem.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_TAIItem,
                              Criticality_ignore,
                              &asn_DEF_TAIItem,
                              taiListIEs->taiItem.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&taiList->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabadmittedlist(
    E_RABAdmittedList_t *e_RABAdmittedList,
    E_RABAdmittedListIEs_t *e_RABAdmittedListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABAdmittedListIEs->e_RABAdmittedItem.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABAdmittedItem,
                              Criticality_ignore,
                              &asn_DEF_E_RABAdmittedItem,
                              e_RABAdmittedListIEs->e_RABAdmittedItem.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABAdmittedList->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rablist(
    E_RABList_t *e_RABList,
    E_RABListIEs_t *e_RABListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABListIEs->e_RABItem.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABItem,
                              Criticality_ignore,
                              &asn_DEF_E_RABItem,
                              e_RABListIEs->e_RABItem.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABList->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabtobesetuplisthoreq(
    E_RABToBeSetupListHOReq_t *e_RABToBeSetupListHOReq,
    E_RABToBeSetupListHOReqIEs_t *e_RABToBeSetupListHOReqIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABToBeSetupListHOReqIEs->e_RABToBeSetupItemHOReq.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSetupItemHOReq,
                              Criticality_reject,
                              &asn_DEF_E_RABToBeSetupItemHOReq,
                              e_RABToBeSetupListHOReqIEs->e_RABToBeSetupItemHOReq.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABToBeSetupListHOReq->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabsetuplistbearersures(
    E_RABSetupListBearerSURes_t *e_RABSetupListBearerSURes,
    E_RABSetupListBearerSUResIEs_t *e_RABSetupListBearerSUResIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABSetupListBearerSUResIEs->e_RABSetupItemBearerSURes.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABSetupItemBearerSURes,
                              Criticality_ignore,
                              &asn_DEF_E_RABSetupItemBearerSURes,
                              e_RABSetupListBearerSUResIEs->e_RABSetupItemBearerSURes.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABSetupListBearerSURes->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabtobesetuplistctxtsureq(
    E_RABToBeSetupListCtxtSUReq_t *e_RABToBeSetupListCtxtSUReq,
    E_RABToBeSetupListCtxtSUReqIEs_t *e_RABToBeSetupListCtxtSUReqIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABToBeSetupListCtxtSUReqIEs->e_RABToBeSetupItemCtxtSUReq.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSetupItemCtxtSUReq,
                              Criticality_reject,
                              &asn_DEF_E_RABToBeSetupItemCtxtSUReq,
                              e_RABToBeSetupListCtxtSUReqIEs->e_RABToBeSetupItemCtxtSUReq.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABToBeSetupListCtxtSUReq->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabsetuplistctxtsures(
    E_RABSetupListCtxtSURes_t *e_RABSetupListCtxtSURes,
    E_RABSetupListCtxtSUResIEs_t *e_RABSetupListCtxtSUResIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABSetupListCtxtSUResIEs->e_RABSetupItemCtxtSURes.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABSetupItemCtxtSURes,
                              Criticality_ignore,
                              &asn_DEF_E_RABSetupItemCtxtSURes,
                              e_RABSetupListCtxtSUResIEs->e_RABSetupItemCtxtSURes.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABSetupListCtxtSURes->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabinformationlist(
    E_RABInformationList_t *e_RABInformationList,
    E_RABInformationListIEs_t *e_RABInformationListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABInformationListIEs->e_RABInformationItem.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABInformationListItem,
                              Criticality_ignore,
                              &asn_DEF_E_RABInformationListItem,
                              e_RABInformationListIEs->e_RABInformationItem.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABInformationList->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabreleaselistbearerrelcomp(
    E_RABReleaseListBearerRelComp_t *e_RABReleaseListBearerRelComp,
    E_RABReleaseListBearerRelCompIEs_t *e_RABReleaseListBearerRelCompIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABReleaseListBearerRelCompIEs->e_RABReleaseItemBearerRelComp.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABReleaseItemBearerRelComp,
                              Criticality_ignore,
                              &asn_DEF_E_RABReleaseItemBearerRelComp,
                              e_RABReleaseListBearerRelCompIEs->e_RABReleaseItemBearerRelComp.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABReleaseListBearerRelComp->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabtobemodifiedlistbearermodreq(
    E_RABToBeModifiedListBearerModReq_t *e_RABToBeModifiedListBearerModReq,
    E_RABToBeModifiedListBearerModReqIEs_t *e_RABToBeModifiedListBearerModReqIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABToBeModifiedListBearerModReqIEs->e_RABToBeModifiedItemBearerModReq.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeModifiedItemBearerModReq,
                              Criticality_reject,
                              &asn_DEF_E_RABToBeModifiedItemBearerModReq,
                              e_RABToBeModifiedListBearerModReqIEs->e_RABToBeModifiedItemBearerModReq.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABToBeModifiedListBearerModReq->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabtobeswitcheddllist(
    E_RABToBeSwitchedDLList_t *e_RABToBeSwitchedDLList,
    E_RABToBeSwitchedDLListIEs_t *e_RABToBeSwitchedDLListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABToBeSwitchedDLListIEs->e_RABToBeSwitchedDLItem.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSwitchedDLItem,
                              Criticality_reject,
                              &asn_DEF_E_RABToBeSwitchedDLItem,
                              e_RABToBeSwitchedDLListIEs->e_RABToBeSwitchedDLItem.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABToBeSwitchedDLList->list, ie);
    }
    return 0;
}

int s1ap_encode_bearers_subjecttostatustransfer_list(
    Bearers_SubjectToStatusTransfer_List_t *bearers_SubjectToStatusTransfer_List,
    Bearers_SubjectToStatusTransfer_ListIEs_t *bearers_SubjectToStatusTransfer_ListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < bearers_SubjectToStatusTransfer_ListIEs->bearers_SubjectToStatusTransfer_Item.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_Bearers_SubjectToStatusTransfer_Item,
                              Criticality_ignore,
                              &asn_DEF_Bearers_SubjectToStatusTransfer_Item,
                              bearers_SubjectToStatusTransfer_ListIEs->bearers_SubjectToStatusTransfer_Item.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&bearers_SubjectToStatusTransfer_List->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabtobeswitchedullist(
    E_RABToBeSwitchedULList_t *e_RABToBeSwitchedULList,
    E_RABToBeSwitchedULListIEs_t *e_RABToBeSwitchedULListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABToBeSwitchedULListIEs->e_RABToBeSwitchedULItem.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABToBeSwitchedULItem,
                              Criticality_ignore,
                              &asn_DEF_E_RABToBeSwitchedULItem,
                              e_RABToBeSwitchedULListIEs->e_RABToBeSwitchedULItem.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABToBeSwitchedULList->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabdataforwardinglist(
    E_RABDataForwardingList_t *e_RABDataForwardingList,
    E_RABDataForwardingListIEs_t *e_RABDataForwardingListIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABDataForwardingListIEs->e_RABDataForwardingItem.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABDataForwardingItem,
                              Criticality_ignore,
                              &asn_DEF_E_RABDataForwardingItem,
                              e_RABDataForwardingListIEs->e_RABDataForwardingItem.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABDataForwardingList->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabmodifylistbearermodres(
    E_RABModifyListBearerModRes_t *e_RABModifyListBearerModRes,
    E_RABModifyListBearerModResIEs_t *e_RABModifyListBearerModResIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABModifyListBearerModResIEs->e_RABModifyItemBearerModRes.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABModifyItemBearerModRes,
                              Criticality_ignore,
                              &asn_DEF_E_RABModifyItemBearerModRes,
                              e_RABModifyListBearerModResIEs->e_RABModifyItemBearerModRes.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABModifyListBearerModRes->list, ie);
    }
    return 0;
}

int s1ap_encode_ue_associatedlogicals1_connectionlistres(
    UE_associatedLogicalS1_ConnectionListRes_t *uE_associatedLogicalS1_ConnectionListRes,
    UE_associatedLogicalS1_ConnectionListResIEs_t *uE_associatedLogicalS1_ConnectionListResIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < uE_associatedLogicalS1_ConnectionListResIEs->uE_associatedLogicalS1_ConnectionItemRes.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem,
                              Criticality_reject,
                              &asn_DEF_UE_associatedLogicalS1_ConnectionItem,
                              uE_associatedLogicalS1_ConnectionListResIEs->uE_associatedLogicalS1_ConnectionItemRes.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&uE_associatedLogicalS1_ConnectionListRes->list, ie);
    }
    return 0;
}

int s1ap_encode_e_rabfailedtosetuplisthoreqack(
    E_RABFailedtoSetupListHOReqAck_t *e_RABFailedtoSetupListHOReqAck,
    E_RABFailedtoSetupListHOReqAckIEs_t *e_RABFailedtoSetupListHOReqAckIEs) {

    int i;
    IE_t *ie;

    for (i = 0; i < e_RABFailedtoSetupListHOReqAckIEs->e_RABFailedtoSetupItemHOReqAck.count; i++) {
        if ((ie = s1ap_new_ie(ProtocolIE_ID_id_E_RABFailedtoSetupItemHOReqAck,
                              Criticality_ignore,
                              &asn_DEF_E_RABFailedToSetupItemHOReqAck,
                              e_RABFailedtoSetupListHOReqAckIEs->e_RABFailedtoSetupItemHOReqAck.array[i])) == NULL) {
            return -1;
        }
        ASN_SEQUENCE_ADD(&e_RABFailedtoSetupListHOReqAck->list, ie);
    }
    return 0;
}

