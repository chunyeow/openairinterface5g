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
 * Created on: 2012-11-13 11:44:11.948325
 * from /homes/roux/trunk-clean/openair2/S1AP/MESSAGES/ASN1/R9.8/S1AP-PDU-Contents.asn
 ******************************************************************************/
#include "s1ap_common.h"
#include "s1ap_ies_defs.h"

int s1ap_decode_mmeconfigurationupdateies(
    MMEConfigurationUpdateIEs_t *mmeConfigurationUpdateIEs,
    ANY_t *any_p) {

    MMEConfigurationUpdate_t  mMEConfigurationUpdate;
    MMEConfigurationUpdate_t *mMEConfigurationUpdate_p = &mMEConfigurationUpdate;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(mmeConfigurationUpdateIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_MMEConfigurationUpdate, (void**)&mMEConfigurationUpdate_p);

    for (i = 0; i < mMEConfigurationUpdate_p->mmeConfigurationUpdate_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = mMEConfigurationUpdate_p->mmeConfigurationUpdate_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_MMEname:
            {
                MMEname_t  mmEname;
                MMEname_t *mmEname_p = &mmEname;
                mmeConfigurationUpdateIEs->presenceMask |= MMECONFIGURATIONUPDATEIES_MMENAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MMEname, (void**)&mmEname_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mmEname failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MMEname, mmEname_p);
                memcpy(&mmeConfigurationUpdateIEs->mmEname, mmEname_p, sizeof(MMEname_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_ServedGUMMEIs:
            {
                ServedGUMMEIs_t  servedGUMMEIs;
                ServedGUMMEIs_t *servedGUMMEIs_p = &servedGUMMEIs;
                mmeConfigurationUpdateIEs->presenceMask |= MMECONFIGURATIONUPDATEIES_SERVEDGUMMEIS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ServedGUMMEIs, (void**)&servedGUMMEIs_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE servedGUMMEIs failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ServedGUMMEIs, servedGUMMEIs_p);
                memcpy(&mmeConfigurationUpdateIEs->servedGUMMEIs, servedGUMMEIs_p, sizeof(ServedGUMMEIs_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_RelativeMMECapacity:
            {
                RelativeMMECapacity_t  relativeMMECapacity;
                RelativeMMECapacity_t *relativeMMECapacity_p = &relativeMMECapacity;
                mmeConfigurationUpdateIEs->presenceMask |= MMECONFIGURATIONUPDATEIES_RELATIVEMMECAPACITY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_RelativeMMECapacity, (void**)&relativeMMECapacity_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE relativeMMECapacity failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_RelativeMMECapacity, relativeMMECapacity_p);
                memcpy(&mmeConfigurationUpdateIEs->relativeMMECapacity, relativeMMECapacity_p, sizeof(RelativeMMECapacity_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message mmeconfigurationupdateies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uecontextmodificationrequesties(
    UEContextModificationRequestIEs_t *ueContextModificationRequestIEs,
    ANY_t *any_p) {

    UEContextModificationRequest_t  uEContextModificationRequest;
    UEContextModificationRequest_t *uEContextModificationRequest_p = &uEContextModificationRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(ueContextModificationRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UEContextModificationRequest, (void**)&uEContextModificationRequest_p);

    for (i = 0; i < uEContextModificationRequest_p->ueContextModificationRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uEContextModificationRequest_p->ueContextModificationRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&ueContextModificationRequestIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&ueContextModificationRequestIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_SecurityKey:
            {
                SecurityKey_t  securityKey;
                SecurityKey_t *securityKey_p = &securityKey;
                ueContextModificationRequestIEs->presenceMask |= UECONTEXTMODIFICATIONREQUESTIES_SECURITYKEY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SecurityKey, (void**)&securityKey_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE securityKey failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SecurityKey, securityKey_p);
                memcpy(&ueContextModificationRequestIEs->securityKey, securityKey_p, sizeof(SecurityKey_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_SubscriberProfileIDforRFP:
            {
                SubscriberProfileIDforRFP_t  subscriberProfileIDforRFP;
                SubscriberProfileIDforRFP_t *subscriberProfileIDforRFP_p = &subscriberProfileIDforRFP;
                ueContextModificationRequestIEs->presenceMask |= UECONTEXTMODIFICATIONREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SubscriberProfileIDforRFP, (void**)&subscriberProfileIDforRFP_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE subscriberProfileIDforRFP failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SubscriberProfileIDforRFP, subscriberProfileIDforRFP_p);
                memcpy(&ueContextModificationRequestIEs->subscriberProfileIDforRFP, subscriberProfileIDforRFP_p, sizeof(SubscriberProfileIDforRFP_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                UEAggregateMaximumBitrate_t  ueAggregateMaximumBitrate;
                UEAggregateMaximumBitrate_t *ueAggregateMaximumBitrate_p = &ueAggregateMaximumBitrate;
                ueContextModificationRequestIEs->presenceMask |= UECONTEXTMODIFICATIONREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEAggregateMaximumBitrate, (void**)&ueAggregateMaximumBitrate_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uEaggregateMaximumBitrate failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEAggregateMaximumBitrate, ueAggregateMaximumBitrate_p);
                memcpy(&ueContextModificationRequestIEs->uEaggregateMaximumBitrate, ueAggregateMaximumBitrate_p, sizeof(UEAggregateMaximumBitrate_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSFallbackIndicator:
            {
                CSFallbackIndicator_t  csFallbackIndicator;
                CSFallbackIndicator_t *csFallbackIndicator_p = &csFallbackIndicator;
                ueContextModificationRequestIEs->presenceMask |= UECONTEXTMODIFICATIONREQUESTIES_CSFALLBACKINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSFallbackIndicator, (void**)&csFallbackIndicator_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csFallbackIndicator failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSFallbackIndicator, csFallbackIndicator_p);
                memcpy(&ueContextModificationRequestIEs->csFallbackIndicator, csFallbackIndicator_p, sizeof(CSFallbackIndicator_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_UESecurityCapabilities:
            {
                UESecurityCapabilities_t  ueSecurityCapabilities;
                UESecurityCapabilities_t *ueSecurityCapabilities_p = &ueSecurityCapabilities;
                ueContextModificationRequestIEs->presenceMask |= UECONTEXTMODIFICATIONREQUESTIES_UESECURITYCAPABILITIES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UESecurityCapabilities, (void**)&ueSecurityCapabilities_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE ueSecurityCapabilities failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UESecurityCapabilities, ueSecurityCapabilities_p);
                memcpy(&ueContextModificationRequestIEs->ueSecurityCapabilities, ueSecurityCapabilities_p, sizeof(UESecurityCapabilities_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSGMembershipStatus:
            {
                CSGMembershipStatus_t  csgMembershipStatus;
                CSGMembershipStatus_t *csgMembershipStatus_p = &csgMembershipStatus;
                ueContextModificationRequestIEs->presenceMask |= UECONTEXTMODIFICATIONREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSGMembershipStatus, (void**)&csgMembershipStatus_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csgMembershipStatus failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSGMembershipStatus, csgMembershipStatus_p);
                memcpy(&ueContextModificationRequestIEs->csgMembershipStatus, csgMembershipStatus_p, sizeof(CSGMembershipStatus_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_RegisteredLAI:
            {
                LAI_t  lai;
                LAI_t *lai_p = &lai;
                ueContextModificationRequestIEs->presenceMask |= UECONTEXTMODIFICATIONREQUESTIES_LAI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_LAI, (void**)&lai_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE registeredLAI failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_LAI, lai_p);
                memcpy(&ueContextModificationRequestIEs->registeredLAI, lai_p, sizeof(LAI_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uecontextmodificationrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_enbdirectinformationtransferies(
    ENBDirectInformationTransferIEs_t *enbDirectInformationTransferIEs,
    ANY_t *any_p) {

    ENBDirectInformationTransfer_t  eNBDirectInformationTransfer;
    ENBDirectInformationTransfer_t *eNBDirectInformationTransfer_p = &eNBDirectInformationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(enbDirectInformationTransferIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ENBDirectInformationTransfer, (void**)&eNBDirectInformationTransfer_p);

    for (i = 0; i < eNBDirectInformationTransfer_p->eNBDirectInformationTransfer_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = eNBDirectInformationTransfer_p->eNBDirectInformationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Inter_SystemInformationTransferTypeEDT:
            {
                Inter_SystemInformationTransferType_t  interSystemInformationTransferType;
                Inter_SystemInformationTransferType_t *interSystemInformationTransferType_p = &interSystemInformationTransferType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Inter_SystemInformationTransferType, (void**)&interSystemInformationTransferType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE inter_SystemInformationTransferTypeEDT failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Inter_SystemInformationTransferType, interSystemInformationTransferType_p);
                memcpy(&enbDirectInformationTransferIEs->inter_SystemInformationTransferTypeEDT, interSystemInformationTransferType_p, sizeof(Inter_SystemInformationTransferType_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message enbdirectinformationtransferies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_resetacknowledgeies(
    ResetAcknowledgeIEs_t *resetAcknowledgeIEs,
    ANY_t *any_p) {

    ResetAcknowledge_t  resetAcknowledge;
    ResetAcknowledge_t *resetAcknowledge_p = &resetAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(resetAcknowledgeIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ResetAcknowledge, (void**)&resetAcknowledge_p);

    for (i = 0; i < resetAcknowledge_p->resetAcknowledge_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = resetAcknowledge_p->resetAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionListResAck:
            {
                UE_associatedLogicalS1_ConnectionListResAck_t  uEassociatedLogicalS1ConnectionListResAck;
                UE_associatedLogicalS1_ConnectionListResAck_t *uEassociatedLogicalS1ConnectionListResAck_p = &uEassociatedLogicalS1ConnectionListResAck;
                resetAcknowledgeIEs->presenceMask |= RESETACKNOWLEDGEIES_UE_ASSOCIATEDLOGICALS1_CONNECTIONLISTRESACK_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UE_associatedLogicalS1_ConnectionListResAck, (void**)&uEassociatedLogicalS1ConnectionListResAck_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uE_associatedLogicalS1_ConnectionListResAck failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UE_associatedLogicalS1_ConnectionListResAck, uEassociatedLogicalS1ConnectionListResAck_p);
                memcpy(&resetAcknowledgeIEs->uE_associatedLogicalS1_ConnectionListResAck, uEassociatedLogicalS1ConnectionListResAck_p, sizeof(UE_associatedLogicalS1_ConnectionListResAck_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                resetAcknowledgeIEs->presenceMask |= RESETACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&resetAcknowledgeIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message resetacknowledgeies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_pagingies(
    PagingIEs_t *pagingIEs,
    ANY_t *any_p) {

    Paging_t  paging;
    Paging_t *paging_p = &paging;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(pagingIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_Paging, (void**)&paging_p);

    for (i = 0; i < paging_p->paging_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = paging_p->paging_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_UEIdentityIndexValue:
            {
                UEIdentityIndexValue_t  ueIdentityIndexValue;
                UEIdentityIndexValue_t *ueIdentityIndexValue_p = &ueIdentityIndexValue;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEIdentityIndexValue, (void**)&ueIdentityIndexValue_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE ueIdentityIndexValue failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEIdentityIndexValue, ueIdentityIndexValue_p);
                memcpy(&pagingIEs->ueIdentityIndexValue, ueIdentityIndexValue_p, sizeof(UEIdentityIndexValue_t));
            } break;
            case ProtocolIE_ID_id_UEPagingID:
            {
                UEPagingID_t  uePagingID;
                UEPagingID_t *uePagingID_p = &uePagingID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEPagingID, (void**)&uePagingID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uePagingID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEPagingID, uePagingID_p);
                memcpy(&pagingIEs->uePagingID, uePagingID_p, sizeof(UEPagingID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_pagingDRX:
            {
                PagingDRX_t  pagingDRX;
                PagingDRX_t *pagingDRX_p = &pagingDRX;
                pagingIEs->presenceMask |= PAGINGIES_PAGINGDRX_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_PagingDRX, (void**)&pagingDRX_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE pagingDRX failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_PagingDRX, pagingDRX_p);
                memcpy(&pagingIEs->pagingDRX, pagingDRX_p, sizeof(PagingDRX_t));
            } break;
            case ProtocolIE_ID_id_CNDomain:
            {
                CNDomain_t  cnDomain;
                CNDomain_t *cnDomain_p = &cnDomain;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CNDomain, (void**)&cnDomain_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cnDomain failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CNDomain, cnDomain_p);
                memcpy(&pagingIEs->cnDomain, cnDomain_p, sizeof(CNDomain_t));
            } break;
            case ProtocolIE_ID_id_TAIList:
            {
                TAIList_t  taiList;
                TAIList_t *taiList_p = &taiList;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TAIList, (void**)&taiList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE taiList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TAIList, taiList_p);
                memcpy(&pagingIEs->taiList, taiList_p, sizeof(TAIList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSG_IdList:
            {
                CSG_IdList_t  csgIdList;
                CSG_IdList_t *csgIdList_p = &csgIdList;
                pagingIEs->presenceMask |= PAGINGIES_CSG_IDLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSG_IdList, (void**)&csgIdList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csG_IdList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSG_IdList, csgIdList_p);
                memcpy(&pagingIEs->csG_IdList, csgIdList_p, sizeof(CSG_IdList_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message pagingies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_pathswitchrequesties(
    PathSwitchRequestIEs_t *pathSwitchRequestIEs,
    ANY_t *any_p) {

    PathSwitchRequest_t  pathSwitchRequest;
    PathSwitchRequest_t *pathSwitchRequest_p = &pathSwitchRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(pathSwitchRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_PathSwitchRequest, (void**)&pathSwitchRequest_p);

    for (i = 0; i < pathSwitchRequest_p->pathSwitchRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = pathSwitchRequest_p->pathSwitchRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&pathSwitchRequestIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_E_RABToBeSwitchedDLList:
            {
                E_RABToBeSwitchedDLList_t  erabToBeSwitchedDLList;
                E_RABToBeSwitchedDLList_t *erabToBeSwitchedDLList_p = &erabToBeSwitchedDLList;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSwitchedDLList, (void**)&erabToBeSwitchedDLList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSwitchedDLList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSwitchedDLList, erabToBeSwitchedDLList_p);
                memcpy(&pathSwitchRequestIEs->e_RABToBeSwitchedDLList, erabToBeSwitchedDLList_p, sizeof(E_RABToBeSwitchedDLList_t));
            } break;
            case ProtocolIE_ID_id_SourceMME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE sourceMME_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&pathSwitchRequestIEs->sourceMME_UE_S1AP_ID, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_EUTRAN_CGI:
            {
                EUTRAN_CGI_t  eutrancgi;
                EUTRAN_CGI_t *eutrancgi_p = &eutrancgi;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_EUTRAN_CGI, (void**)&eutrancgi_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eutran_cgi failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_EUTRAN_CGI, eutrancgi_p);
                memcpy(&pathSwitchRequestIEs->eutran_cgi, eutrancgi_p, sizeof(EUTRAN_CGI_t));
            } break;
            case ProtocolIE_ID_id_TAI:
            {
                TAI_t  tai;
                TAI_t *tai_p = &tai;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TAI, (void**)&tai_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE tai failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TAI, tai_p);
                memcpy(&pathSwitchRequestIEs->tai, tai_p, sizeof(TAI_t));
            } break;
            case ProtocolIE_ID_id_UESecurityCapabilities:
            {
                UESecurityCapabilities_t  ueSecurityCapabilities;
                UESecurityCapabilities_t *ueSecurityCapabilities_p = &ueSecurityCapabilities;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UESecurityCapabilities, (void**)&ueSecurityCapabilities_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE ueSecurityCapabilities failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UESecurityCapabilities, ueSecurityCapabilities_p);
                memcpy(&pathSwitchRequestIEs->ueSecurityCapabilities, ueSecurityCapabilities_p, sizeof(UESecurityCapabilities_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message pathswitchrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_deactivatetraceies(
    DeactivateTraceIEs_t *deactivateTraceIEs,
    ANY_t *any_p) {

    DeactivateTrace_t  deactivateTrace;
    DeactivateTrace_t *deactivateTrace_p = &deactivateTrace;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(deactivateTraceIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_DeactivateTrace, (void**)&deactivateTrace_p);

    for (i = 0; i < deactivateTrace_p->deactivateTrace_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = deactivateTrace_p->deactivateTrace_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&deactivateTraceIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&deactivateTraceIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_E_UTRAN_Trace_ID:
            {
                E_UTRAN_Trace_ID_t  eutranTraceID;
                E_UTRAN_Trace_ID_t *eutranTraceID_p = &eutranTraceID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_UTRAN_Trace_ID, (void**)&eutranTraceID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_UTRAN_Trace_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_UTRAN_Trace_ID, eutranTraceID_p);
                memcpy(&deactivateTraceIEs->e_UTRAN_Trace_ID, eutranTraceID_p, sizeof(E_UTRAN_Trace_ID_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message deactivatetraceies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_locationreportingcontrolies(
    LocationReportingControlIEs_t *locationReportingControlIEs,
    ANY_t *any_p) {

    LocationReportingControl_t  locationReportingControl;
    LocationReportingControl_t *locationReportingControl_p = &locationReportingControl;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(locationReportingControlIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_LocationReportingControl, (void**)&locationReportingControl_p);

    for (i = 0; i < locationReportingControl_p->locationReportingControl_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = locationReportingControl_p->locationReportingControl_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&locationReportingControlIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&locationReportingControlIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_RequestType:
            {
                RequestType_t  requestType;
                RequestType_t *requestType_p = &requestType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_RequestType, (void**)&requestType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE requestType failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_RequestType, requestType_p);
                memcpy(&locationReportingControlIEs->requestType, requestType_p, sizeof(RequestType_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message locationreportingcontrolies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_enbconfigurationupdateacknowledgeies(
    ENBConfigurationUpdateAcknowledgeIEs_t *enbConfigurationUpdateAcknowledgeIEs,
    ANY_t *any_p) {

    ENBConfigurationUpdateAcknowledge_t  eNBConfigurationUpdateAcknowledge;
    ENBConfigurationUpdateAcknowledge_t *eNBConfigurationUpdateAcknowledge_p = &eNBConfigurationUpdateAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(enbConfigurationUpdateAcknowledgeIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ENBConfigurationUpdateAcknowledge, (void**)&eNBConfigurationUpdateAcknowledge_p);

    for (i = 0; i < eNBConfigurationUpdateAcknowledge_p->eNBConfigurationUpdateAcknowledge_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = eNBConfigurationUpdateAcknowledge_p->eNBConfigurationUpdateAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                enbConfigurationUpdateAcknowledgeIEs->presenceMask |= ENBCONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&enbConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message enbconfigurationupdateacknowledgeies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_handoverfailureies(
    HandoverFailureIEs_t *handoverFailureIEs,
    ANY_t *any_p) {

    HandoverFailure_t  handoverFailure;
    HandoverFailure_t *handoverFailure_p = &handoverFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(handoverFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_HandoverFailure, (void**)&handoverFailure_p);

    for (i = 0; i < handoverFailure_p->handoverFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = handoverFailure_p->handoverFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&handoverFailureIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&handoverFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                handoverFailureIEs->presenceMask |= HANDOVERFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&handoverFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message handoverfailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_locationreportingfailureindicationies(
    LocationReportingFailureIndicationIEs_t *locationReportingFailureIndicationIEs,
    ANY_t *any_p) {

    LocationReportingFailureIndication_t  locationReportingFailureIndication;
    LocationReportingFailureIndication_t *locationReportingFailureIndication_p = &locationReportingFailureIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(locationReportingFailureIndicationIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_LocationReportingFailureIndication, (void**)&locationReportingFailureIndication_p);

    for (i = 0; i < locationReportingFailureIndication_p->locationReportingFailureIndication_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = locationReportingFailureIndication_p->locationReportingFailureIndication_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&locationReportingFailureIndicationIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&locationReportingFailureIndicationIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&locationReportingFailureIndicationIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message locationreportingfailureindicationies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_downlinknastransporties(
    DownlinkNASTransportIEs_t *downlinkNASTransportIEs,
    ANY_t *any_p) {

    DownlinkNASTransport_t  downlinkNASTransport;
    DownlinkNASTransport_t *downlinkNASTransport_p = &downlinkNASTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(downlinkNASTransportIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_DownlinkNASTransport, (void**)&downlinkNASTransport_p);

    for (i = 0; i < downlinkNASTransport_p->downlinkNASTransport_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = downlinkNASTransport_p->downlinkNASTransport_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&downlinkNASTransportIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&downlinkNASTransportIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_NAS_PDU:
            {
                NAS_PDU_t  naspdu;
                NAS_PDU_t *naspdu_p = &naspdu;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_NAS_PDU, (void**)&naspdu_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE nas_pdu failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_NAS_PDU, naspdu_p);
                memcpy(&downlinkNASTransportIEs->nas_pdu, naspdu_p, sizeof(NAS_PDU_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_HandoverRestrictionList:
            {
                HandoverRestrictionList_t  handoverRestrictionList;
                HandoverRestrictionList_t *handoverRestrictionList_p = &handoverRestrictionList;
                downlinkNASTransportIEs->presenceMask |= DOWNLINKNASTRANSPORTIES_HANDOVERRESTRICTIONLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_HandoverRestrictionList, (void**)&handoverRestrictionList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE handoverRestrictionList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_HandoverRestrictionList, handoverRestrictionList_p);
                memcpy(&downlinkNASTransportIEs->handoverRestrictionList, handoverRestrictionList_p, sizeof(HandoverRestrictionList_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message downlinknastransporties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_mmeconfigurationupdatefailureies(
    MMEConfigurationUpdateFailureIEs_t *mmeConfigurationUpdateFailureIEs,
    ANY_t *any_p) {

    MMEConfigurationUpdateFailure_t  mMEConfigurationUpdateFailure;
    MMEConfigurationUpdateFailure_t *mMEConfigurationUpdateFailure_p = &mMEConfigurationUpdateFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(mmeConfigurationUpdateFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_MMEConfigurationUpdateFailure, (void**)&mMEConfigurationUpdateFailure_p);

    for (i = 0; i < mMEConfigurationUpdateFailure_p->mmeConfigurationUpdateFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = mMEConfigurationUpdateFailure_p->mmeConfigurationUpdateFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&mmeConfigurationUpdateFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_TimeToWait:
            {
                TimeToWait_t  timeToWait;
                TimeToWait_t *timeToWait_p = &timeToWait;
                mmeConfigurationUpdateFailureIEs->presenceMask |= MMECONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TimeToWait, (void**)&timeToWait_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE timeToWait failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TimeToWait, timeToWait_p);
                memcpy(&mmeConfigurationUpdateFailureIEs->timeToWait, timeToWait_p, sizeof(TimeToWait_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                mmeConfigurationUpdateFailureIEs->presenceMask |= MMECONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&mmeConfigurationUpdateFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message mmeconfigurationupdatefailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uecontextreleasecompleteies(
    UEContextReleaseCompleteIEs_t *ueContextReleaseCompleteIEs,
    ANY_t *any_p) {

    UEContextReleaseComplete_t  uEContextReleaseComplete;
    UEContextReleaseComplete_t *uEContextReleaseComplete_p = &uEContextReleaseComplete;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(ueContextReleaseCompleteIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UEContextReleaseComplete, (void**)&uEContextReleaseComplete_p);

    for (i = 0; i < uEContextReleaseComplete_p->ueContextReleaseComplete_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uEContextReleaseComplete_p->ueContextReleaseComplete_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&ueContextReleaseCompleteIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&ueContextReleaseCompleteIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                ueContextReleaseCompleteIEs->presenceMask |= UECONTEXTRELEASECOMPLETEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&ueContextReleaseCompleteIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uecontextreleasecompleteies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1setupresponseies(
    S1SetupResponseIEs_t *s1SetupResponseIEs,
    ANY_t *any_p) {

    S1SetupResponse_t  s1SetupResponse;
    S1SetupResponse_t *s1SetupResponse_p = &s1SetupResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1SetupResponseIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_S1SetupResponse, (void**)&s1SetupResponse_p);

    for (i = 0; i < s1SetupResponse_p->s1SetupResponse_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = s1SetupResponse_p->s1SetupResponse_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_MMEname:
            {
                MMEname_t  mmEname;
                MMEname_t *mmEname_p = &mmEname;
                s1SetupResponseIEs->presenceMask |= S1SETUPRESPONSEIES_MMENAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MMEname, (void**)&mmEname_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mmEname failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MMEname, mmEname_p);
                memcpy(&s1SetupResponseIEs->mmEname, mmEname_p, sizeof(MMEname_t));
            } break;
            case ProtocolIE_ID_id_ServedGUMMEIs:
            {
                ServedGUMMEIs_t  servedGUMMEIs;
                ServedGUMMEIs_t *servedGUMMEIs_p = &servedGUMMEIs;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ServedGUMMEIs, (void**)&servedGUMMEIs_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE servedGUMMEIs failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ServedGUMMEIs, servedGUMMEIs_p);
                memcpy(&s1SetupResponseIEs->servedGUMMEIs, servedGUMMEIs_p, sizeof(ServedGUMMEIs_t));
            } break;
            case ProtocolIE_ID_id_RelativeMMECapacity:
            {
                RelativeMMECapacity_t  relativeMMECapacity;
                RelativeMMECapacity_t *relativeMMECapacity_p = &relativeMMECapacity;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_RelativeMMECapacity, (void**)&relativeMMECapacity_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE relativeMMECapacity failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_RelativeMMECapacity, relativeMMECapacity_p);
                memcpy(&s1SetupResponseIEs->relativeMMECapacity, relativeMMECapacity_p, sizeof(RelativeMMECapacity_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                s1SetupResponseIEs->presenceMask |= S1SETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&s1SetupResponseIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message s1setupresponseies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_handoverpreparationfailureies(
    HandoverPreparationFailureIEs_t *handoverPreparationFailureIEs,
    ANY_t *any_p) {

    HandoverPreparationFailure_t  handoverPreparationFailure;
    HandoverPreparationFailure_t *handoverPreparationFailure_p = &handoverPreparationFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(handoverPreparationFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_HandoverPreparationFailure, (void**)&handoverPreparationFailure_p);

    for (i = 0; i < handoverPreparationFailure_p->handoverPreparationFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = handoverPreparationFailure_p->handoverPreparationFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&handoverPreparationFailureIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&handoverPreparationFailureIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&handoverPreparationFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                handoverPreparationFailureIEs->presenceMask |= HANDOVERPREPARATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&handoverPreparationFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message handoverpreparationfailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_killresponseies(
    KillResponseIEs_t *killResponseIEs,
    ANY_t *any_p) {

    KillResponse_t  killResponse;
    KillResponse_t *killResponse_p = &killResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(killResponseIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_KillResponse, (void**)&killResponse_p);

    for (i = 0; i < killResponse_p->killResponse_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = killResponse_p->killResponse_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MessageIdentifier:
            {
                MessageIdentifier_t  messageIdentifier;
                MessageIdentifier_t *messageIdentifier_p = &messageIdentifier;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MessageIdentifier, (void**)&messageIdentifier_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE messageIdentifier failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MessageIdentifier, messageIdentifier_p);
                memcpy(&killResponseIEs->messageIdentifier, messageIdentifier_p, sizeof(MessageIdentifier_t));
            } break;
            case ProtocolIE_ID_id_SerialNumber:
            {
                SerialNumber_t  serialNumber;
                SerialNumber_t *serialNumber_p = &serialNumber;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SerialNumber, (void**)&serialNumber_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE serialNumber failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SerialNumber, serialNumber_p);
                memcpy(&killResponseIEs->serialNumber, serialNumber_p, sizeof(SerialNumber_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_BroadcastCancelledAreaList:
            {
                BroadcastCancelledAreaList_t  broadcastCancelledAreaList;
                BroadcastCancelledAreaList_t *broadcastCancelledAreaList_p = &broadcastCancelledAreaList;
                killResponseIEs->presenceMask |= KILLRESPONSEIES_BROADCASTCANCELLEDAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_BroadcastCancelledAreaList, (void**)&broadcastCancelledAreaList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE broadcastCancelledAreaList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_BroadcastCancelledAreaList, broadcastCancelledAreaList_p);
                memcpy(&killResponseIEs->broadcastCancelledAreaList, broadcastCancelledAreaList_p, sizeof(BroadcastCancelledAreaList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                killResponseIEs->presenceMask |= KILLRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&killResponseIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message killresponseies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uplinkueassociatedlppatransporties(
    UplinkUEAssociatedLPPaTransportIEs_t *uplinkUEAssociatedLPPaTransportIEs,
    ANY_t *any_p) {

    UplinkUEAssociatedLPPaTransport_t  uplinkUEAssociatedLPPaTransport;
    UplinkUEAssociatedLPPaTransport_t *uplinkUEAssociatedLPPaTransport_p = &uplinkUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(uplinkUEAssociatedLPPaTransportIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UplinkUEAssociatedLPPaTransport, (void**)&uplinkUEAssociatedLPPaTransport_p);

    for (i = 0; i < uplinkUEAssociatedLPPaTransport_p->uplinkUEAssociatedLPPaTransport_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uplinkUEAssociatedLPPaTransport_p->uplinkUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&uplinkUEAssociatedLPPaTransportIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&uplinkUEAssociatedLPPaTransportIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Routing_ID:
            {
                Routing_ID_t  routingID;
                Routing_ID_t *routingID_p = &routingID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Routing_ID, (void**)&routingID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE routing_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Routing_ID, routingID_p);
                memcpy(&uplinkUEAssociatedLPPaTransportIEs->routing_ID, routingID_p, sizeof(Routing_ID_t));
            } break;
            case ProtocolIE_ID_id_LPPa_PDU:
            {
                LPPa_PDU_t  lpPaPDU;
                LPPa_PDU_t *lpPaPDU_p = &lpPaPDU;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_LPPa_PDU, (void**)&lpPaPDU_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE lpPa_PDU failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_LPPa_PDU, lpPaPDU_p);
                memcpy(&uplinkUEAssociatedLPPaTransportIEs->lpPa_PDU, lpPaPDU_p, sizeof(LPPa_PDU_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uplinkueassociatedlppatransporties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_enbconfigurationupdateies(
    ENBConfigurationUpdateIEs_t *enbConfigurationUpdateIEs,
    ANY_t *any_p) {

    ENBConfigurationUpdate_t  eNBConfigurationUpdate;
    ENBConfigurationUpdate_t *eNBConfigurationUpdate_p = &eNBConfigurationUpdate;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(enbConfigurationUpdateIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ENBConfigurationUpdate, (void**)&eNBConfigurationUpdate_p);

    for (i = 0; i < eNBConfigurationUpdate_p->eNBConfigurationUpdate_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = eNBConfigurationUpdate_p->eNBConfigurationUpdate_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_eNBname:
            {
                ENBname_t  enBname;
                ENBname_t *enBname_p = &enBname;
                enbConfigurationUpdateIEs->presenceMask |= ENBCONFIGURATIONUPDATEIES_ENBNAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENBname, (void**)&enBname_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNBname failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENBname, enBname_p);
                memcpy(&enbConfigurationUpdateIEs->eNBname, enBname_p, sizeof(ENBname_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_SupportedTAs:
            {
                SupportedTAs_t  supportedTAs;
                SupportedTAs_t *supportedTAs_p = &supportedTAs;
                enbConfigurationUpdateIEs->presenceMask |= ENBCONFIGURATIONUPDATEIES_SUPPORTEDTAS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SupportedTAs, (void**)&supportedTAs_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE supportedTAs failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SupportedTAs, supportedTAs_p);
                memcpy(&enbConfigurationUpdateIEs->supportedTAs, supportedTAs_p, sizeof(SupportedTAs_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSG_IdList:
            {
                CSG_IdList_t  csgIdList;
                CSG_IdList_t *csgIdList_p = &csgIdList;
                enbConfigurationUpdateIEs->presenceMask |= ENBCONFIGURATIONUPDATEIES_CSG_IDLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSG_IdList, (void**)&csgIdList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csG_IdList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSG_IdList, csgIdList_p);
                memcpy(&enbConfigurationUpdateIEs->csG_IdList, csgIdList_p, sizeof(CSG_IdList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_DefaultPagingDRX:
            {
                PagingDRX_t  pagingDRX;
                PagingDRX_t *pagingDRX_p = &pagingDRX;
                enbConfigurationUpdateIEs->presenceMask |= ENBCONFIGURATIONUPDATEIES_PAGINGDRX_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_PagingDRX, (void**)&pagingDRX_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE defaultPagingDRX failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_PagingDRX, pagingDRX_p);
                memcpy(&enbConfigurationUpdateIEs->defaultPagingDRX, pagingDRX_p, sizeof(PagingDRX_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message enbconfigurationupdateies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_initialcontextsetuprequesties(
    InitialContextSetupRequestIEs_t *initialContextSetupRequestIEs,
    ANY_t *any_p) {

    InitialContextSetupRequest_t  initialContextSetupRequest;
    InitialContextSetupRequest_t *initialContextSetupRequest_p = &initialContextSetupRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(initialContextSetupRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_InitialContextSetupRequest, (void**)&initialContextSetupRequest_p);

    for (i = 0; i < initialContextSetupRequest_p->initialContextSetupRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = initialContextSetupRequest_p->initialContextSetupRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&initialContextSetupRequestIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&initialContextSetupRequestIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                UEAggregateMaximumBitrate_t  ueAggregateMaximumBitrate;
                UEAggregateMaximumBitrate_t *ueAggregateMaximumBitrate_p = &ueAggregateMaximumBitrate;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEAggregateMaximumBitrate, (void**)&ueAggregateMaximumBitrate_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uEaggregateMaximumBitrate failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEAggregateMaximumBitrate, ueAggregateMaximumBitrate_p);
                memcpy(&initialContextSetupRequestIEs->uEaggregateMaximumBitrate, ueAggregateMaximumBitrate_p, sizeof(UEAggregateMaximumBitrate_t));
            } break;
            case ProtocolIE_ID_id_E_RABToBeSetupListCtxtSUReq:
            {
                E_RABToBeSetupListCtxtSUReq_t  erabToBeSetupListCtxtSUReq;
                E_RABToBeSetupListCtxtSUReq_t *erabToBeSetupListCtxtSUReq_p = &erabToBeSetupListCtxtSUReq;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSetupListCtxtSUReq, (void**)&erabToBeSetupListCtxtSUReq_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSetupListCtxtSUReq failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSetupListCtxtSUReq, erabToBeSetupListCtxtSUReq_p);
                memcpy(&initialContextSetupRequestIEs->e_RABToBeSetupListCtxtSUReq, erabToBeSetupListCtxtSUReq_p, sizeof(E_RABToBeSetupListCtxtSUReq_t));
            } break;
            case ProtocolIE_ID_id_UESecurityCapabilities:
            {
                UESecurityCapabilities_t  ueSecurityCapabilities;
                UESecurityCapabilities_t *ueSecurityCapabilities_p = &ueSecurityCapabilities;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UESecurityCapabilities, (void**)&ueSecurityCapabilities_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE ueSecurityCapabilities failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UESecurityCapabilities, ueSecurityCapabilities_p);
                memcpy(&initialContextSetupRequestIEs->ueSecurityCapabilities, ueSecurityCapabilities_p, sizeof(UESecurityCapabilities_t));
            } break;
            case ProtocolIE_ID_id_SecurityKey:
            {
                SecurityKey_t  securityKey;
                SecurityKey_t *securityKey_p = &securityKey;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SecurityKey, (void**)&securityKey_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE securityKey failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SecurityKey, securityKey_p);
                memcpy(&initialContextSetupRequestIEs->securityKey, securityKey_p, sizeof(SecurityKey_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_TraceActivation:
            {
                TraceActivation_t  traceActivation;
                TraceActivation_t *traceActivation_p = &traceActivation;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_TRACEACTIVATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TraceActivation, (void**)&traceActivation_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE traceActivation failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TraceActivation, traceActivation_p);
                memcpy(&initialContextSetupRequestIEs->traceActivation, traceActivation_p, sizeof(TraceActivation_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_HandoverRestrictionList:
            {
                HandoverRestrictionList_t  handoverRestrictionList;
                HandoverRestrictionList_t *handoverRestrictionList_p = &handoverRestrictionList;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_HANDOVERRESTRICTIONLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_HandoverRestrictionList, (void**)&handoverRestrictionList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE handoverRestrictionList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_HandoverRestrictionList, handoverRestrictionList_p);
                memcpy(&initialContextSetupRequestIEs->handoverRestrictionList, handoverRestrictionList_p, sizeof(HandoverRestrictionList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_UERadioCapability:
            {
                UERadioCapability_t  ueRadioCapability;
                UERadioCapability_t *ueRadioCapability_p = &ueRadioCapability;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_UERADIOCAPABILITY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UERadioCapability, (void**)&ueRadioCapability_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE ueRadioCapability failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UERadioCapability, ueRadioCapability_p);
                memcpy(&initialContextSetupRequestIEs->ueRadioCapability, ueRadioCapability_p, sizeof(UERadioCapability_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_SubscriberProfileIDforRFP:
            {
                SubscriberProfileIDforRFP_t  subscriberProfileIDforRFP;
                SubscriberProfileIDforRFP_t *subscriberProfileIDforRFP_p = &subscriberProfileIDforRFP;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_SUBSCRIBERPROFILEIDFORRFP_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SubscriberProfileIDforRFP, (void**)&subscriberProfileIDforRFP_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE subscriberProfileIDforRFP failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SubscriberProfileIDforRFP, subscriberProfileIDforRFP_p);
                memcpy(&initialContextSetupRequestIEs->subscriberProfileIDforRFP, subscriberProfileIDforRFP_p, sizeof(SubscriberProfileIDforRFP_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSFallbackIndicator:
            {
                CSFallbackIndicator_t  csFallbackIndicator;
                CSFallbackIndicator_t *csFallbackIndicator_p = &csFallbackIndicator;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_CSFALLBACKINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSFallbackIndicator, (void**)&csFallbackIndicator_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csFallbackIndicator failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSFallbackIndicator, csFallbackIndicator_p);
                memcpy(&initialContextSetupRequestIEs->csFallbackIndicator, csFallbackIndicator_p, sizeof(CSFallbackIndicator_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_SRVCCOperationPossible:
            {
                SRVCCOperationPossible_t  srvccOperationPossible;
                SRVCCOperationPossible_t *srvccOperationPossible_p = &srvccOperationPossible;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_SRVCCOPERATIONPOSSIBLE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SRVCCOperationPossible, (void**)&srvccOperationPossible_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE srvccOperationPossible failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SRVCCOperationPossible, srvccOperationPossible_p);
                memcpy(&initialContextSetupRequestIEs->srvccOperationPossible, srvccOperationPossible_p, sizeof(SRVCCOperationPossible_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSGMembershipStatus:
            {
                CSGMembershipStatus_t  csgMembershipStatus;
                CSGMembershipStatus_t *csgMembershipStatus_p = &csgMembershipStatus;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_CSGMEMBERSHIPSTATUS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSGMembershipStatus, (void**)&csgMembershipStatus_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csgMembershipStatus failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSGMembershipStatus, csgMembershipStatus_p);
                memcpy(&initialContextSetupRequestIEs->csgMembershipStatus, csgMembershipStatus_p, sizeof(CSGMembershipStatus_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_RegisteredLAI:
            {
                LAI_t  lai;
                LAI_t *lai_p = &lai;
                initialContextSetupRequestIEs->presenceMask |= INITIALCONTEXTSETUPREQUESTIES_LAI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_LAI, (void**)&lai_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE registeredLAI failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_LAI, lai_p);
                memcpy(&initialContextSetupRequestIEs->registeredLAI, lai_p, sizeof(LAI_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message initialcontextsetuprequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_mmedirectinformationtransferies(
    MMEDirectInformationTransferIEs_t *mmeDirectInformationTransferIEs,
    ANY_t *any_p) {

    MMEDirectInformationTransfer_t  mMEDirectInformationTransfer;
    MMEDirectInformationTransfer_t *mMEDirectInformationTransfer_p = &mMEDirectInformationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(mmeDirectInformationTransferIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_MMEDirectInformationTransfer, (void**)&mMEDirectInformationTransfer_p);

    for (i = 0; i < mMEDirectInformationTransfer_p->mmeDirectInformationTransfer_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = mMEDirectInformationTransfer_p->mmeDirectInformationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Inter_SystemInformationTransferTypeMDT:
            {
                Inter_SystemInformationTransferType_t  interSystemInformationTransferType;
                Inter_SystemInformationTransferType_t *interSystemInformationTransferType_p = &interSystemInformationTransferType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Inter_SystemInformationTransferType, (void**)&interSystemInformationTransferType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE inter_SystemInformationTransferTypeMDT failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Inter_SystemInformationTransferType, interSystemInformationTransferType_p);
                memcpy(&mmeDirectInformationTransferIEs->inter_SystemInformationTransferTypeMDT, interSystemInformationTransferType_p, sizeof(Inter_SystemInformationTransferType_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message mmedirectinformationtransferies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uplinknonueassociatedlppatransporties(
    UplinkNonUEAssociatedLPPaTransportIEs_t *uplinkNonUEAssociatedLPPaTransportIEs,
    ANY_t *any_p) {

    UplinkNonUEAssociatedLPPaTransport_t  uplinkNonUEAssociatedLPPaTransport;
    UplinkNonUEAssociatedLPPaTransport_t *uplinkNonUEAssociatedLPPaTransport_p = &uplinkNonUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(uplinkNonUEAssociatedLPPaTransportIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UplinkNonUEAssociatedLPPaTransport, (void**)&uplinkNonUEAssociatedLPPaTransport_p);

    for (i = 0; i < uplinkNonUEAssociatedLPPaTransport_p->uplinkNonUEAssociatedLPPaTransport_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uplinkNonUEAssociatedLPPaTransport_p->uplinkNonUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Routing_ID:
            {
                Routing_ID_t  routingID;
                Routing_ID_t *routingID_p = &routingID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Routing_ID, (void**)&routingID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE routing_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Routing_ID, routingID_p);
                memcpy(&uplinkNonUEAssociatedLPPaTransportIEs->routing_ID, routingID_p, sizeof(Routing_ID_t));
            } break;
            case ProtocolIE_ID_id_LPPa_PDU:
            {
                LPPa_PDU_t  lpPaPDU;
                LPPa_PDU_t *lpPaPDU_p = &lpPaPDU;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_LPPa_PDU, (void**)&lpPaPDU_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE lpPa_PDU failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_LPPa_PDU, lpPaPDU_p);
                memcpy(&uplinkNonUEAssociatedLPPaTransportIEs->lpPa_PDU, lpPaPDU_p, sizeof(LPPa_PDU_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uplinknonueassociatedlppatransporties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_reseties(
    ResetIEs_t *resetIEs,
    ANY_t *any_p) {

    Reset_t  reset;
    Reset_t *reset_p = &reset;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(resetIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_Reset, (void**)&reset_p);

    for (i = 0; i < reset_p->reset_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = reset_p->reset_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&resetIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            case ProtocolIE_ID_id_ResetType:
            {
                ResetType_t  resetType;
                ResetType_t *resetType_p = &resetType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ResetType, (void**)&resetType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE resetType failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ResetType, resetType_p);
                memcpy(&resetIEs->resetType, resetType_p, sizeof(ResetType_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message reseties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uecontextreleasecommandies(
    UEContextReleaseCommandIEs_t *ueContextReleaseCommandIEs,
    ANY_t *any_p) {

    UEContextReleaseCommand_t  uEContextReleaseCommand;
    UEContextReleaseCommand_t *uEContextReleaseCommand_p = &uEContextReleaseCommand;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(ueContextReleaseCommandIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UEContextReleaseCommand, (void**)&uEContextReleaseCommand_p);

    for (i = 0; i < uEContextReleaseCommand_p->ueContextReleaseCommand_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uEContextReleaseCommand_p->ueContextReleaseCommand_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_UE_S1AP_IDs:
            {
                UE_S1AP_IDs_t  ueS1APIDs;
                UE_S1AP_IDs_t *ueS1APIDs_p = &ueS1APIDs;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UE_S1AP_IDs, (void**)&ueS1APIDs_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uE_S1AP_IDs failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UE_S1AP_IDs, ueS1APIDs_p);
                memcpy(&ueContextReleaseCommandIEs->uE_S1AP_IDs, ueS1APIDs_p, sizeof(UE_S1AP_IDs_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&ueContextReleaseCommandIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uecontextreleasecommandies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uecapabilityinfoindicationies(
    UECapabilityInfoIndicationIEs_t *ueCapabilityInfoIndicationIEs,
    ANY_t *any_p) {

    UECapabilityInfoIndication_t  uECapabilityInfoIndication;
    UECapabilityInfoIndication_t *uECapabilityInfoIndication_p = &uECapabilityInfoIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(ueCapabilityInfoIndicationIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UECapabilityInfoIndication, (void**)&uECapabilityInfoIndication_p);

    for (i = 0; i < uECapabilityInfoIndication_p->ueCapabilityInfoIndication_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uECapabilityInfoIndication_p->ueCapabilityInfoIndication_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&ueCapabilityInfoIndicationIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&ueCapabilityInfoIndicationIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_UERadioCapability:
            {
                UERadioCapability_t  ueRadioCapability;
                UERadioCapability_t *ueRadioCapability_p = &ueRadioCapability;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UERadioCapability, (void**)&ueRadioCapability_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE ueRadioCapability failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UERadioCapability, ueRadioCapability_p);
                memcpy(&ueCapabilityInfoIndicationIEs->ueRadioCapability, ueRadioCapability_p, sizeof(UERadioCapability_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uecapabilityinfoindicationies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uplinks1cdma2000tunnelingies(
    UplinkS1cdma2000tunnelingIEs_t *uplinkS1cdma2000tunnelingIEs,
    ANY_t *any_p) {

    UplinkS1cdma2000tunneling_t  uplinkS1cdma2000tunneling;
    UplinkS1cdma2000tunneling_t *uplinkS1cdma2000tunneling_p = &uplinkS1cdma2000tunneling;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(uplinkS1cdma2000tunnelingIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UplinkS1cdma2000tunneling, (void**)&uplinkS1cdma2000tunneling_p);

    for (i = 0; i < uplinkS1cdma2000tunneling_p->uplinkS1cdma2000tunneling_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uplinkS1cdma2000tunneling_p->uplinkS1cdma2000tunneling_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_cdma2000RATType:
            {
                Cdma2000RATType_t  cdma2000RATType;
                Cdma2000RATType_t *cdma2000RATType_p = &cdma2000RATType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000RATType, (void**)&cdma2000RATType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000RATType failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000RATType, cdma2000RATType_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->cdma2000RATType, cdma2000RATType_p, sizeof(Cdma2000RATType_t));
            } break;
            case ProtocolIE_ID_id_cdma2000SectorID:
            {
                Cdma2000SectorID_t  cdma2000SectorID;
                Cdma2000SectorID_t *cdma2000SectorID_p = &cdma2000SectorID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000SectorID, (void**)&cdma2000SectorID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000SectorID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000SectorID, cdma2000SectorID_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->cdma2000SectorID, cdma2000SectorID_p, sizeof(Cdma2000SectorID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_cdma2000HORequiredIndication:
            {
                Cdma2000HORequiredIndication_t  cdma2000HORequiredIndication;
                Cdma2000HORequiredIndication_t *cdma2000HORequiredIndication_p = &cdma2000HORequiredIndication;
                uplinkS1cdma2000tunnelingIEs->presenceMask |= UPLINKS1CDMA2000TUNNELINGIES_CDMA2000HOREQUIREDINDICATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000HORequiredIndication, (void**)&cdma2000HORequiredIndication_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000HORequiredIndication failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000HORequiredIndication, cdma2000HORequiredIndication_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->cdma2000HORequiredIndication, cdma2000HORequiredIndication_p, sizeof(Cdma2000HORequiredIndication_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_cdma2000OneXSRVCCInfo:
            {
                Cdma2000OneXSRVCCInfo_t  cdma2000OneXSRVCCInfo;
                Cdma2000OneXSRVCCInfo_t *cdma2000OneXSRVCCInfo_p = &cdma2000OneXSRVCCInfo;
                uplinkS1cdma2000tunnelingIEs->presenceMask |= UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXSRVCCINFO_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000OneXSRVCCInfo, (void**)&cdma2000OneXSRVCCInfo_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000OneXSRVCCInfo failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000OneXSRVCCInfo, cdma2000OneXSRVCCInfo_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->cdma2000OneXSRVCCInfo, cdma2000OneXSRVCCInfo_p, sizeof(Cdma2000OneXSRVCCInfo_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_cdma2000OneXRAND:
            {
                Cdma2000OneXRAND_t  cdma2000OneXRAND;
                Cdma2000OneXRAND_t *cdma2000OneXRAND_p = &cdma2000OneXRAND;
                uplinkS1cdma2000tunnelingIEs->presenceMask |= UPLINKS1CDMA2000TUNNELINGIES_CDMA2000ONEXRAND_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000OneXRAND, (void**)&cdma2000OneXRAND_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000OneXRAND failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000OneXRAND, cdma2000OneXRAND_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->cdma2000OneXRAND, cdma2000OneXRAND_p, sizeof(Cdma2000OneXRAND_t));
            } break;
            case ProtocolIE_ID_id_cdma2000PDU:
            {
                Cdma2000PDU_t  cdma2000PDU;
                Cdma2000PDU_t *cdma2000PDU_p = &cdma2000PDU;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000PDU, (void**)&cdma2000PDU_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000PDU failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000PDU, cdma2000PDU_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->cdma2000PDU, cdma2000PDU_p, sizeof(Cdma2000PDU_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_EUTRANRoundTripDelayEstimationInfo:
            {
                EUTRANRoundTripDelayEstimationInfo_t  eutranRoundTripDelayEstimationInfo;
                EUTRANRoundTripDelayEstimationInfo_t *eutranRoundTripDelayEstimationInfo_p = &eutranRoundTripDelayEstimationInfo;
                uplinkS1cdma2000tunnelingIEs->presenceMask |= UPLINKS1CDMA2000TUNNELINGIES_EUTRANROUNDTRIPDELAYESTIMATIONINFO_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_EUTRANRoundTripDelayEstimationInfo, (void**)&eutranRoundTripDelayEstimationInfo_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eutranRoundTripDelayEstimationInfo failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_EUTRANRoundTripDelayEstimationInfo, eutranRoundTripDelayEstimationInfo_p);
                memcpy(&uplinkS1cdma2000tunnelingIEs->eutranRoundTripDelayEstimationInfo, eutranRoundTripDelayEstimationInfo_p, sizeof(EUTRANRoundTripDelayEstimationInfo_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uplinks1cdma2000tunnelingies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_overloadstopies(
    ANY_t *any_p) {

    OverloadStop_t  overloadStop;
    OverloadStop_t *overloadStop_p = &overloadStop;
    int i, decoded = 0;
    assert(any_p != NULL);
    ANY_to_type_aper(any_p, &asn_DEF_OverloadStop, (void**)&overloadStop_p);

    for (i = 0; i < overloadStop_p->overloadStop_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = overloadStop_p->overloadStop_ies.list.array[i];
        switch(ie_p->id) {
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message overloadstopies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_writereplacewarningresponseies(
    WriteReplaceWarningResponseIEs_t *writeReplaceWarningResponseIEs,
    ANY_t *any_p) {

    WriteReplaceWarningResponse_t  writeReplaceWarningResponse;
    WriteReplaceWarningResponse_t *writeReplaceWarningResponse_p = &writeReplaceWarningResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(writeReplaceWarningResponseIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_WriteReplaceWarningResponse, (void**)&writeReplaceWarningResponse_p);

    for (i = 0; i < writeReplaceWarningResponse_p->writeReplaceWarningResponse_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = writeReplaceWarningResponse_p->writeReplaceWarningResponse_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MessageIdentifier:
            {
                MessageIdentifier_t  messageIdentifier;
                MessageIdentifier_t *messageIdentifier_p = &messageIdentifier;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MessageIdentifier, (void**)&messageIdentifier_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE messageIdentifier failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MessageIdentifier, messageIdentifier_p);
                memcpy(&writeReplaceWarningResponseIEs->messageIdentifier, messageIdentifier_p, sizeof(MessageIdentifier_t));
            } break;
            case ProtocolIE_ID_id_SerialNumber:
            {
                SerialNumber_t  serialNumber;
                SerialNumber_t *serialNumber_p = &serialNumber;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SerialNumber, (void**)&serialNumber_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE serialNumber failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SerialNumber, serialNumber_p);
                memcpy(&writeReplaceWarningResponseIEs->serialNumber, serialNumber_p, sizeof(SerialNumber_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_BroadcastCompletedAreaList:
            {
                BroadcastCompletedAreaList_t  broadcastCompletedAreaList;
                BroadcastCompletedAreaList_t *broadcastCompletedAreaList_p = &broadcastCompletedAreaList;
                writeReplaceWarningResponseIEs->presenceMask |= WRITEREPLACEWARNINGRESPONSEIES_BROADCASTCOMPLETEDAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_BroadcastCompletedAreaList, (void**)&broadcastCompletedAreaList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE broadcastCompletedAreaList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_BroadcastCompletedAreaList, broadcastCompletedAreaList_p);
                memcpy(&writeReplaceWarningResponseIEs->broadcastCompletedAreaList, broadcastCompletedAreaList_p, sizeof(BroadcastCompletedAreaList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                writeReplaceWarningResponseIEs->presenceMask |= WRITEREPLACEWARNINGRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&writeReplaceWarningResponseIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message writereplacewarningresponseies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabreleasecommandies(
    E_RABReleaseCommandIEs_t *e_RABReleaseCommandIEs,
    ANY_t *any_p) {

    E_RABReleaseCommand_t  e_RABReleaseCommand;
    E_RABReleaseCommand_t *e_RABReleaseCommand_p = &e_RABReleaseCommand;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(e_RABReleaseCommandIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_E_RABReleaseCommand, (void**)&e_RABReleaseCommand_p);

    for (i = 0; i < e_RABReleaseCommand_p->e_RABReleaseCommand_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = e_RABReleaseCommand_p->e_RABReleaseCommand_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&e_RABReleaseCommandIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&e_RABReleaseCommandIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                UEAggregateMaximumBitrate_t  ueAggregateMaximumBitrate;
                UEAggregateMaximumBitrate_t *ueAggregateMaximumBitrate_p = &ueAggregateMaximumBitrate;
                e_RABReleaseCommandIEs->presenceMask |= E_RABRELEASECOMMANDIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEAggregateMaximumBitrate, (void**)&ueAggregateMaximumBitrate_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uEaggregateMaximumBitrate failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEAggregateMaximumBitrate, ueAggregateMaximumBitrate_p);
                memcpy(&e_RABReleaseCommandIEs->uEaggregateMaximumBitrate, ueAggregateMaximumBitrate_p, sizeof(UEAggregateMaximumBitrate_t));
            } break;
            case ProtocolIE_ID_id_E_RABToBeReleasedList:
            {
                E_RABList_t  erabList;
                E_RABList_t *erabList_p = &erabList;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABList, (void**)&erabList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeReleasedList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABList, erabList_p);
                memcpy(&e_RABReleaseCommandIEs->e_RABToBeReleasedList, erabList_p, sizeof(E_RABList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_NAS_PDU:
            {
                NAS_PDU_t  naspdu;
                NAS_PDU_t *naspdu_p = &naspdu;
                e_RABReleaseCommandIEs->presenceMask |= E_RABRELEASECOMMANDIES_NAS_PDU_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_NAS_PDU, (void**)&naspdu_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE nas_pdu failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_NAS_PDU, naspdu_p);
                memcpy(&e_RABReleaseCommandIEs->nas_pdu, naspdu_p, sizeof(NAS_PDU_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabreleasecommandies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_celltraffictraceies(
    CellTrafficTraceIEs_t *cellTrafficTraceIEs,
    ANY_t *any_p) {

    CellTrafficTrace_t  cellTrafficTrace;
    CellTrafficTrace_t *cellTrafficTrace_p = &cellTrafficTrace;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(cellTrafficTraceIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_CellTrafficTrace, (void**)&cellTrafficTrace_p);

    for (i = 0; i < cellTrafficTrace_p->cellTrafficTrace_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = cellTrafficTrace_p->cellTrafficTrace_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&cellTrafficTraceIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&cellTrafficTraceIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_E_UTRAN_Trace_ID:
            {
                E_UTRAN_Trace_ID_t  eutranTraceID;
                E_UTRAN_Trace_ID_t *eutranTraceID_p = &eutranTraceID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_UTRAN_Trace_ID, (void**)&eutranTraceID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_UTRAN_Trace_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_UTRAN_Trace_ID, eutranTraceID_p);
                memcpy(&cellTrafficTraceIEs->e_UTRAN_Trace_ID, eutranTraceID_p, sizeof(E_UTRAN_Trace_ID_t));
            } break;
            case ProtocolIE_ID_id_EUTRAN_CGI:
            {
                EUTRAN_CGI_t  eutrancgi;
                EUTRAN_CGI_t *eutrancgi_p = &eutrancgi;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_EUTRAN_CGI, (void**)&eutrancgi_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eutran_cgi failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_EUTRAN_CGI, eutrancgi_p);
                memcpy(&cellTrafficTraceIEs->eutran_cgi, eutrancgi_p, sizeof(EUTRAN_CGI_t));
            } break;
            case ProtocolIE_ID_id_TraceCollectionEntityIPAddress:
            {
                TransportLayerAddress_t  transportLayerAddress;
                TransportLayerAddress_t *transportLayerAddress_p = &transportLayerAddress;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TransportLayerAddress, (void**)&transportLayerAddress_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE traceCollectionEntityIPAddress failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TransportLayerAddress, transportLayerAddress_p);
                memcpy(&cellTrafficTraceIEs->traceCollectionEntityIPAddress, transportLayerAddress_p, sizeof(TransportLayerAddress_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message celltraffictraceies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_pathswitchrequestfailureies(
    PathSwitchRequestFailureIEs_t *pathSwitchRequestFailureIEs,
    ANY_t *any_p) {

    PathSwitchRequestFailure_t  pathSwitchRequestFailure;
    PathSwitchRequestFailure_t *pathSwitchRequestFailure_p = &pathSwitchRequestFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(pathSwitchRequestFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_PathSwitchRequestFailure, (void**)&pathSwitchRequestFailure_p);

    for (i = 0; i < pathSwitchRequestFailure_p->pathSwitchRequestFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = pathSwitchRequestFailure_p->pathSwitchRequestFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&pathSwitchRequestFailureIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&pathSwitchRequestFailureIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&pathSwitchRequestFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                pathSwitchRequestFailureIEs->presenceMask |= PATHSWITCHREQUESTFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&pathSwitchRequestFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message pathswitchrequestfailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_initialcontextsetupresponseies(
    InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs,
    ANY_t *any_p) {

    InitialContextSetupResponse_t  initialContextSetupResponse;
    InitialContextSetupResponse_t *initialContextSetupResponse_p = &initialContextSetupResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(initialContextSetupResponseIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_InitialContextSetupResponse, (void**)&initialContextSetupResponse_p);

    for (i = 0; i < initialContextSetupResponse_p->initialContextSetupResponse_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = initialContextSetupResponse_p->initialContextSetupResponse_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&initialContextSetupResponseIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&initialContextSetupResponseIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_E_RABSetupListCtxtSURes:
            {
                E_RABSetupListCtxtSURes_t  erabSetupListCtxtSURes;
                E_RABSetupListCtxtSURes_t *erabSetupListCtxtSURes_p = &erabSetupListCtxtSURes;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABSetupListCtxtSURes, (void**)&erabSetupListCtxtSURes_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABSetupListCtxtSURes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABSetupListCtxtSURes, erabSetupListCtxtSURes_p);
                memcpy(&initialContextSetupResponseIEs->e_RABSetupListCtxtSURes, erabSetupListCtxtSURes_p, sizeof(E_RABSetupListCtxtSURes_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABFailedToSetupListCtxtSURes:
            {
                E_RABList_t  erabList;
                E_RABList_t *erabList_p = &erabList;
                initialContextSetupResponseIEs->presenceMask |= INITIALCONTEXTSETUPRESPONSEIES_E_RABLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABList, (void**)&erabList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABFailedToSetupListCtxtSURes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABList, erabList_p);
                memcpy(&initialContextSetupResponseIEs->e_RABFailedToSetupListCtxtSURes, erabList_p, sizeof(E_RABList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                initialContextSetupResponseIEs->presenceMask |= INITIALCONTEXTSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&initialContextSetupResponseIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message initialcontextsetupresponseies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_mmestatustransferies(
    MMEStatusTransferIEs_t *mmeStatusTransferIEs,
    ANY_t *any_p) {

    MMEStatusTransfer_t  mMEStatusTransfer;
    MMEStatusTransfer_t *mMEStatusTransfer_p = &mMEStatusTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(mmeStatusTransferIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_MMEStatusTransfer, (void**)&mMEStatusTransfer_p);

    for (i = 0; i < mMEStatusTransfer_p->mmeStatusTransfer_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = mMEStatusTransfer_p->mmeStatusTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&mmeStatusTransferIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&mmeStatusTransferIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer:
            {
                ENB_StatusTransfer_TransparentContainer_t  enbStatusTransferTransparentContainer;
                ENB_StatusTransfer_TransparentContainer_t *enbStatusTransferTransparentContainer_p = &enbStatusTransferTransparentContainer;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_StatusTransfer_TransparentContainer, (void**)&enbStatusTransferTransparentContainer_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_StatusTransfer_TransparentContainer failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_StatusTransfer_TransparentContainer, enbStatusTransferTransparentContainer_p);
                memcpy(&mmeStatusTransferIEs->eNB_StatusTransfer_TransparentContainer, enbStatusTransferTransparentContainer_p, sizeof(ENB_StatusTransfer_TransparentContainer_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message mmestatustransferies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1setuprequesties(
    S1SetupRequestIEs_t *s1SetupRequestIEs,
    ANY_t *any_p) {

    S1SetupRequest_t  s1SetupRequest;
    S1SetupRequest_t *s1SetupRequest_p = &s1SetupRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1SetupRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_S1SetupRequest, (void**)&s1SetupRequest_p);

    for (i = 0; i < s1SetupRequest_p->s1SetupRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = s1SetupRequest_p->s1SetupRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Global_ENB_ID:
            {
                Global_ENB_ID_t  globalENBID;
                Global_ENB_ID_t *globalENBID_p = &globalENBID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Global_ENB_ID, (void**)&globalENBID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE global_ENB_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Global_ENB_ID, globalENBID_p);
                memcpy(&s1SetupRequestIEs->global_ENB_ID, globalENBID_p, sizeof(Global_ENB_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_eNBname:
            {
                ENBname_t  enBname;
                ENBname_t *enBname_p = &enBname;
                s1SetupRequestIEs->presenceMask |= S1SETUPREQUESTIES_ENBNAME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENBname, (void**)&enBname_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNBname failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENBname, enBname_p);
                memcpy(&s1SetupRequestIEs->eNBname, enBname_p, sizeof(ENBname_t));
            } break;
            case ProtocolIE_ID_id_SupportedTAs:
            {
                SupportedTAs_t  supportedTAs;
                SupportedTAs_t *supportedTAs_p = &supportedTAs;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SupportedTAs, (void**)&supportedTAs_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE supportedTAs failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SupportedTAs, supportedTAs_p);
                memcpy(&s1SetupRequestIEs->supportedTAs, supportedTAs_p, sizeof(SupportedTAs_t));
            } break;
            case ProtocolIE_ID_id_DefaultPagingDRX:
            {
                PagingDRX_t  pagingDRX;
                PagingDRX_t *pagingDRX_p = &pagingDRX;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_PagingDRX, (void**)&pagingDRX_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE defaultPagingDRX failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_PagingDRX, pagingDRX_p);
                memcpy(&s1SetupRequestIEs->defaultPagingDRX, pagingDRX_p, sizeof(PagingDRX_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSG_IdList:
            {
                CSG_IdList_t  csgIdList;
                CSG_IdList_t *csgIdList_p = &csgIdList;
                s1SetupRequestIEs->presenceMask |= S1SETUPREQUESTIES_CSG_IDLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSG_IdList, (void**)&csgIdList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csG_IdList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSG_IdList, csgIdList_p);
                memcpy(&s1SetupRequestIEs->csG_IdList, csgIdList_p, sizeof(CSG_IdList_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message s1setuprequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_handoverrequiredies(
    HandoverRequiredIEs_t *handoverRequiredIEs,
    ANY_t *any_p) {

    HandoverRequired_t  handoverRequired;
    HandoverRequired_t *handoverRequired_p = &handoverRequired;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(handoverRequiredIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_HandoverRequired, (void**)&handoverRequired_p);

    for (i = 0; i < handoverRequired_p->handoverRequired_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = handoverRequired_p->handoverRequired_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&handoverRequiredIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&handoverRequiredIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_HandoverType:
            {
                HandoverType_t  handoverType;
                HandoverType_t *handoverType_p = &handoverType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_HandoverType, (void**)&handoverType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE handoverType failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_HandoverType, handoverType_p);
                memcpy(&handoverRequiredIEs->handoverType, handoverType_p, sizeof(HandoverType_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&handoverRequiredIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            case ProtocolIE_ID_id_TargetID:
            {
                TargetID_t  targetID;
                TargetID_t *targetID_p = &targetID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TargetID, (void**)&targetID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE targetID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TargetID, targetID_p);
                memcpy(&handoverRequiredIEs->targetID, targetID_p, sizeof(TargetID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_Direct_Forwarding_Path_Availability:
            {
                Direct_Forwarding_Path_Availability_t  directForwardingPathAvailability;
                Direct_Forwarding_Path_Availability_t *directForwardingPathAvailability_p = &directForwardingPathAvailability;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_DIRECT_FORWARDING_PATH_AVAILABILITY_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Direct_Forwarding_Path_Availability, (void**)&directForwardingPathAvailability_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE direct_Forwarding_Path_Availability failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Direct_Forwarding_Path_Availability, directForwardingPathAvailability_p);
                memcpy(&handoverRequiredIEs->direct_Forwarding_Path_Availability, directForwardingPathAvailability_p, sizeof(Direct_Forwarding_Path_Availability_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_SRVCCHOIndication:
            {
                SRVCCHOIndication_t  srvcchoIndication;
                SRVCCHOIndication_t *srvcchoIndication_p = &srvcchoIndication;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_SRVCCHOINDICATION_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SRVCCHOIndication, (void**)&srvcchoIndication_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE srvcchoIndication failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SRVCCHOIndication, srvcchoIndication_p);
                memcpy(&handoverRequiredIEs->srvcchoIndication, srvcchoIndication_p, sizeof(SRVCCHOIndication_t));
            } break;
            case ProtocolIE_ID_id_Source_ToTarget_TransparentContainer:
            {
                Source_ToTarget_TransparentContainer_t  sourceToTargetTransparentContainer;
                Source_ToTarget_TransparentContainer_t *sourceToTargetTransparentContainer_p = &sourceToTargetTransparentContainer;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Source_ToTarget_TransparentContainer, (void**)&sourceToTargetTransparentContainer_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE source_ToTarget_TransparentContainer failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Source_ToTarget_TransparentContainer, sourceToTargetTransparentContainer_p);
                memcpy(&handoverRequiredIEs->source_ToTarget_TransparentContainer, sourceToTargetTransparentContainer_p, sizeof(Source_ToTarget_TransparentContainer_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_Source_ToTarget_TransparentContainer_Secondary:
            {
                Source_ToTarget_TransparentContainer_t  sourceToTargetTransparentContainer;
                Source_ToTarget_TransparentContainer_t *sourceToTargetTransparentContainer_p = &sourceToTargetTransparentContainer;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_SOURCE_TOTARGET_TRANSPARENTCONTAINER_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Source_ToTarget_TransparentContainer, (void**)&sourceToTargetTransparentContainer_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE source_ToTarget_TransparentContainer_Secondary failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Source_ToTarget_TransparentContainer, sourceToTargetTransparentContainer_p);
                memcpy(&handoverRequiredIEs->source_ToTarget_TransparentContainer_Secondary, sourceToTargetTransparentContainer_p, sizeof(Source_ToTarget_TransparentContainer_t));
            } break;
            /* Conditional field */
            case ProtocolIE_ID_id_MSClassmark2:
            {
                MSClassmark2_t  msClassmark2;
                MSClassmark2_t *msClassmark2_p = &msClassmark2;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_MSCLASSMARK2_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MSClassmark2, (void**)&msClassmark2_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE msClassmark2 failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MSClassmark2, msClassmark2_p);
                memcpy(&handoverRequiredIEs->msClassmark2, msClassmark2_p, sizeof(MSClassmark2_t));
            } break;
            /* Conditional field */
            case ProtocolIE_ID_id_MSClassmark3:
            {
                MSClassmark3_t  msClassmark3;
                MSClassmark3_t *msClassmark3_p = &msClassmark3;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_MSCLASSMARK3_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MSClassmark3, (void**)&msClassmark3_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE msClassmark3 failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MSClassmark3, msClassmark3_p);
                memcpy(&handoverRequiredIEs->msClassmark3, msClassmark3_p, sizeof(MSClassmark3_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSG_Id:
            {
                CSG_Id_t  csgId;
                CSG_Id_t *csgId_p = &csgId;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSG_Id, (void**)&csgId_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csG_Id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSG_Id, csgId_p);
                memcpy(&handoverRequiredIEs->csG_Id, csgId_p, sizeof(CSG_Id_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CellAccessMode:
            {
                CellAccessMode_t  cellAccessMode;
                CellAccessMode_t *cellAccessMode_p = &cellAccessMode;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_CELLACCESSMODE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CellAccessMode, (void**)&cellAccessMode_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cellAccessMode failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CellAccessMode, cellAccessMode_p);
                memcpy(&handoverRequiredIEs->cellAccessMode, cellAccessMode_p, sizeof(CellAccessMode_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_PS_ServiceNotAvailable:
            {
                PS_ServiceNotAvailable_t  psServiceNotAvailable;
                PS_ServiceNotAvailable_t *psServiceNotAvailable_p = &psServiceNotAvailable;
                handoverRequiredIEs->presenceMask |= HANDOVERREQUIREDIES_PS_SERVICENOTAVAILABLE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_PS_ServiceNotAvailable, (void**)&psServiceNotAvailable_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE pS_ServiceNotAvailable failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_PS_ServiceNotAvailable, psServiceNotAvailable_p);
                memcpy(&handoverRequiredIEs->pS_ServiceNotAvailable, psServiceNotAvailable_p, sizeof(PS_ServiceNotAvailable_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message handoverrequiredies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uplinknastransporties(
    UplinkNASTransportIEs_t *uplinkNASTransportIEs,
    ANY_t *any_p) {

    UplinkNASTransport_t  uplinkNASTransport;
    UplinkNASTransport_t *uplinkNASTransport_p = &uplinkNASTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(uplinkNASTransportIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UplinkNASTransport, (void**)&uplinkNASTransport_p);

    for (i = 0; i < uplinkNASTransport_p->uplinkNASTransport_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uplinkNASTransport_p->uplinkNASTransport_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&uplinkNASTransportIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&uplinkNASTransportIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_NAS_PDU:
            {
                NAS_PDU_t  naspdu;
                NAS_PDU_t *naspdu_p = &naspdu;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_NAS_PDU, (void**)&naspdu_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE nas_pdu failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_NAS_PDU, naspdu_p);
                memcpy(&uplinkNASTransportIEs->nas_pdu, naspdu_p, sizeof(NAS_PDU_t));
            } break;
            case ProtocolIE_ID_id_EUTRAN_CGI:
            {
                EUTRAN_CGI_t  eutrancgi;
                EUTRAN_CGI_t *eutrancgi_p = &eutrancgi;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_EUTRAN_CGI, (void**)&eutrancgi_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eutran_cgi failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_EUTRAN_CGI, eutrancgi_p);
                memcpy(&uplinkNASTransportIEs->eutran_cgi, eutrancgi_p, sizeof(EUTRAN_CGI_t));
            } break;
            case ProtocolIE_ID_id_TAI:
            {
                TAI_t  tai;
                TAI_t *tai_p = &tai;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TAI, (void**)&tai_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE tai failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TAI, tai_p);
                memcpy(&uplinkNASTransportIEs->tai, tai_p, sizeof(TAI_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uplinknastransporties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_locationreporties(
    LocationReportIEs_t *locationReportIEs,
    ANY_t *any_p) {

    LocationReport_t  locationReport;
    LocationReport_t *locationReport_p = &locationReport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(locationReportIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_LocationReport, (void**)&locationReport_p);

    for (i = 0; i < locationReport_p->locationReport_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = locationReport_p->locationReport_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&locationReportIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&locationReportIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_EUTRAN_CGI:
            {
                EUTRAN_CGI_t  eutrancgi;
                EUTRAN_CGI_t *eutrancgi_p = &eutrancgi;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_EUTRAN_CGI, (void**)&eutrancgi_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eutran_cgi failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_EUTRAN_CGI, eutrancgi_p);
                memcpy(&locationReportIEs->eutran_cgi, eutrancgi_p, sizeof(EUTRAN_CGI_t));
            } break;
            case ProtocolIE_ID_id_TAI:
            {
                TAI_t  tai;
                TAI_t *tai_p = &tai;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TAI, (void**)&tai_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE tai failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TAI, tai_p);
                memcpy(&locationReportIEs->tai, tai_p, sizeof(TAI_t));
            } break;
            case ProtocolIE_ID_id_RequestType:
            {
                RequestType_t  requestType;
                RequestType_t *requestType_p = &requestType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_RequestType, (void**)&requestType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE requestType failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_RequestType, requestType_p);
                memcpy(&locationReportIEs->requestType, requestType_p, sizeof(RequestType_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message locationreporties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uecontextmodificationfailureies(
    UEContextModificationFailureIEs_t *ueContextModificationFailureIEs,
    ANY_t *any_p) {

    UEContextModificationFailure_t  uEContextModificationFailure;
    UEContextModificationFailure_t *uEContextModificationFailure_p = &uEContextModificationFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(ueContextModificationFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UEContextModificationFailure, (void**)&uEContextModificationFailure_p);

    for (i = 0; i < uEContextModificationFailure_p->ueContextModificationFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uEContextModificationFailure_p->ueContextModificationFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&ueContextModificationFailureIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&ueContextModificationFailureIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&ueContextModificationFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                ueContextModificationFailureIEs->presenceMask |= UECONTEXTMODIFICATIONFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&ueContextModificationFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uecontextmodificationfailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_enbconfigurationtransferies(
    ENBConfigurationTransferIEs_t *enbConfigurationTransferIEs,
    ANY_t *any_p) {

    ENBConfigurationTransfer_t  eNBConfigurationTransfer;
    ENBConfigurationTransfer_t *eNBConfigurationTransfer_p = &eNBConfigurationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(enbConfigurationTransferIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ENBConfigurationTransfer, (void**)&eNBConfigurationTransfer_p);

    for (i = 0; i < eNBConfigurationTransfer_p->eNBConfigurationTransfer_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = eNBConfigurationTransfer_p->eNBConfigurationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_SONConfigurationTransferECT:
            {
                SONConfigurationTransfer_t  sonConfigurationTransfer;
                SONConfigurationTransfer_t *sonConfigurationTransfer_p = &sonConfigurationTransfer;
                enbConfigurationTransferIEs->presenceMask |= ENBCONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SONConfigurationTransfer, (void**)&sonConfigurationTransfer_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE sonConfigurationTransferECT failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SONConfigurationTransfer, sonConfigurationTransfer_p);
                memcpy(&enbConfigurationTransferIEs->sonConfigurationTransferECT, sonConfigurationTransfer_p, sizeof(SONConfigurationTransfer_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message enbconfigurationtransferies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_downlinknonueassociatedlppatransporties(
    DownlinkNonUEAssociatedLPPaTransportIEs_t *downlinkNonUEAssociatedLPPaTransportIEs,
    ANY_t *any_p) {

    DownlinkNonUEAssociatedLPPaTransport_t  downlinkNonUEAssociatedLPPaTransport;
    DownlinkNonUEAssociatedLPPaTransport_t *downlinkNonUEAssociatedLPPaTransport_p = &downlinkNonUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(downlinkNonUEAssociatedLPPaTransportIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_DownlinkNonUEAssociatedLPPaTransport, (void**)&downlinkNonUEAssociatedLPPaTransport_p);

    for (i = 0; i < downlinkNonUEAssociatedLPPaTransport_p->downlinkNonUEAssociatedLPPaTransport_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = downlinkNonUEAssociatedLPPaTransport_p->downlinkNonUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Routing_ID:
            {
                Routing_ID_t  routingID;
                Routing_ID_t *routingID_p = &routingID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Routing_ID, (void**)&routingID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE routing_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Routing_ID, routingID_p);
                memcpy(&downlinkNonUEAssociatedLPPaTransportIEs->routing_ID, routingID_p, sizeof(Routing_ID_t));
            } break;
            case ProtocolIE_ID_id_LPPa_PDU:
            {
                LPPa_PDU_t  lpPaPDU;
                LPPa_PDU_t *lpPaPDU_p = &lpPaPDU;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_LPPa_PDU, (void**)&lpPaPDU_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE lpPa_PDU failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_LPPa_PDU, lpPaPDU_p);
                memcpy(&downlinkNonUEAssociatedLPPaTransportIEs->lpPa_PDU, lpPaPDU_p, sizeof(LPPa_PDU_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message downlinknonueassociatedlppatransporties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_initialuemessageies(
    InitialUEMessageIEs_t *initialUEMessageIEs,
    ANY_t *any_p) {

    InitialUEMessage_t  initialUEMessage;
    InitialUEMessage_t *initialUEMessage_p = &initialUEMessage;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(initialUEMessageIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_InitialUEMessage, (void**)&initialUEMessage_p);

    for (i = 0; i < initialUEMessage_p->initialUEMessage_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = initialUEMessage_p->initialUEMessage_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&initialUEMessageIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_NAS_PDU:
            {
                NAS_PDU_t  naspdu;
                NAS_PDU_t *naspdu_p = &naspdu;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_NAS_PDU, (void**)&naspdu_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE nas_pdu failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_NAS_PDU, naspdu_p);
                memcpy(&initialUEMessageIEs->nas_pdu, naspdu_p, sizeof(NAS_PDU_t));
            } break;
            case ProtocolIE_ID_id_TAI:
            {
                TAI_t  tai;
                TAI_t *tai_p = &tai;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TAI, (void**)&tai_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE tai failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TAI, tai_p);
                memcpy(&initialUEMessageIEs->tai, tai_p, sizeof(TAI_t));
            } break;
            case ProtocolIE_ID_id_EUTRAN_CGI:
            {
                EUTRAN_CGI_t  eutrancgi;
                EUTRAN_CGI_t *eutrancgi_p = &eutrancgi;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_EUTRAN_CGI, (void**)&eutrancgi_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eutran_cgi failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_EUTRAN_CGI, eutrancgi_p);
                memcpy(&initialUEMessageIEs->eutran_cgi, eutrancgi_p, sizeof(EUTRAN_CGI_t));
            } break;
            case ProtocolIE_ID_id_RRC_Establishment_Cause:
            {
                RRC_Establishment_Cause_t  rrcEstablishmentCause;
                RRC_Establishment_Cause_t *rrcEstablishmentCause_p = &rrcEstablishmentCause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_RRC_Establishment_Cause, (void**)&rrcEstablishmentCause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE rrC_Establishment_Cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_RRC_Establishment_Cause, rrcEstablishmentCause_p);
                memcpy(&initialUEMessageIEs->rrC_Establishment_Cause, rrcEstablishmentCause_p, sizeof(RRC_Establishment_Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_S_TMSI:
            {
                S_TMSI_t  stmsi;
                S_TMSI_t *stmsi_p = &stmsi;
                initialUEMessageIEs->presenceMask |= INITIALUEMESSAGEIES_S_TMSI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_S_TMSI, (void**)&stmsi_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE s_tmsi failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_S_TMSI, stmsi_p);
                memcpy(&initialUEMessageIEs->s_tmsi, stmsi_p, sizeof(S_TMSI_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSG_Id:
            {
                CSG_Id_t  csgId;
                CSG_Id_t *csgId_p = &csgId;
                initialUEMessageIEs->presenceMask |= INITIALUEMESSAGEIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSG_Id, (void**)&csgId_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csG_Id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSG_Id, csgId_p);
                memcpy(&initialUEMessageIEs->csG_Id, csgId_p, sizeof(CSG_Id_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_GUMMEI_ID:
            {
                GUMMEI_t  gummei;
                GUMMEI_t *gummei_p = &gummei;
                initialUEMessageIEs->presenceMask |= INITIALUEMESSAGEIES_GUMMEI_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_GUMMEI, (void**)&gummei_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE gummei_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_GUMMEI, gummei_p);
                memcpy(&initialUEMessageIEs->gummei_id, gummei_p, sizeof(GUMMEI_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CellAccessMode:
            {
                CellAccessMode_t  cellAccessMode;
                CellAccessMode_t *cellAccessMode_p = &cellAccessMode;
                initialUEMessageIEs->presenceMask |= INITIALUEMESSAGEIES_CELLACCESSMODE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CellAccessMode, (void**)&cellAccessMode_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cellAccessMode failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CellAccessMode, cellAccessMode_p);
                memcpy(&initialUEMessageIEs->cellAccessMode, cellAccessMode_p, sizeof(CellAccessMode_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message initialuemessageies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabsetupresponseies(
    E_RABSetupResponseIEs_t *e_RABSetupResponseIEs,
    ANY_t *any_p) {

    E_RABSetupResponse_t  e_RABSetupResponse;
    E_RABSetupResponse_t *e_RABSetupResponse_p = &e_RABSetupResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(e_RABSetupResponseIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_E_RABSetupResponse, (void**)&e_RABSetupResponse_p);

    for (i = 0; i < e_RABSetupResponse_p->e_RABSetupResponse_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = e_RABSetupResponse_p->e_RABSetupResponse_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&e_RABSetupResponseIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&e_RABSetupResponseIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABSetupListBearerSURes:
            {
                E_RABSetupListBearerSURes_t  erabSetupListBearerSURes;
                E_RABSetupListBearerSURes_t *erabSetupListBearerSURes_p = &erabSetupListBearerSURes;
                e_RABSetupResponseIEs->presenceMask |= E_RABSETUPRESPONSEIES_E_RABSETUPLISTBEARERSURES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABSetupListBearerSURes, (void**)&erabSetupListBearerSURes_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABSetupListBearerSURes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABSetupListBearerSURes, erabSetupListBearerSURes_p);
                memcpy(&e_RABSetupResponseIEs->e_RABSetupListBearerSURes, erabSetupListBearerSURes_p, sizeof(E_RABSetupListBearerSURes_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABFailedToSetupListBearerSURes:
            {
                E_RABList_t  erabList;
                E_RABList_t *erabList_p = &erabList;
                e_RABSetupResponseIEs->presenceMask |= E_RABSETUPRESPONSEIES_E_RABLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABList, (void**)&erabList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABFailedToSetupListBearerSURes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABList, erabList_p);
                memcpy(&e_RABSetupResponseIEs->e_RABFailedToSetupListBearerSURes, erabList_p, sizeof(E_RABList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                e_RABSetupResponseIEs->presenceMask |= E_RABSETUPRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&e_RABSetupResponseIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabsetupresponseies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_pathswitchrequestacknowledgeies(
    PathSwitchRequestAcknowledgeIEs_t *pathSwitchRequestAcknowledgeIEs,
    ANY_t *any_p) {

    PathSwitchRequestAcknowledge_t  pathSwitchRequestAcknowledge;
    PathSwitchRequestAcknowledge_t *pathSwitchRequestAcknowledge_p = &pathSwitchRequestAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(pathSwitchRequestAcknowledgeIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_PathSwitchRequestAcknowledge, (void**)&pathSwitchRequestAcknowledge_p);

    for (i = 0; i < pathSwitchRequestAcknowledge_p->pathSwitchRequestAcknowledge_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = pathSwitchRequestAcknowledge_p->pathSwitchRequestAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&pathSwitchRequestAcknowledgeIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&pathSwitchRequestAcknowledgeIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                UEAggregateMaximumBitrate_t  ueAggregateMaximumBitrate;
                UEAggregateMaximumBitrate_t *ueAggregateMaximumBitrate_p = &ueAggregateMaximumBitrate;
                pathSwitchRequestAcknowledgeIEs->presenceMask |= PATHSWITCHREQUESTACKNOWLEDGEIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEAggregateMaximumBitrate, (void**)&ueAggregateMaximumBitrate_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uEaggregateMaximumBitrate failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEAggregateMaximumBitrate, ueAggregateMaximumBitrate_p);
                memcpy(&pathSwitchRequestAcknowledgeIEs->uEaggregateMaximumBitrate, ueAggregateMaximumBitrate_p, sizeof(UEAggregateMaximumBitrate_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABToBeSwitchedULList:
            {
                E_RABToBeSwitchedULList_t  erabToBeSwitchedULList;
                E_RABToBeSwitchedULList_t *erabToBeSwitchedULList_p = &erabToBeSwitchedULList;
                pathSwitchRequestAcknowledgeIEs->presenceMask |= PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABTOBESWITCHEDULLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSwitchedULList, (void**)&erabToBeSwitchedULList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSwitchedULList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSwitchedULList, erabToBeSwitchedULList_p);
                memcpy(&pathSwitchRequestAcknowledgeIEs->e_RABToBeSwitchedULList, erabToBeSwitchedULList_p, sizeof(E_RABToBeSwitchedULList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABToBeReleasedList:
            {
                E_RABList_t  erabList;
                E_RABList_t *erabList_p = &erabList;
                pathSwitchRequestAcknowledgeIEs->presenceMask |= PATHSWITCHREQUESTACKNOWLEDGEIES_E_RABLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABList, (void**)&erabList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeReleasedList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABList, erabList_p);
                memcpy(&pathSwitchRequestAcknowledgeIEs->e_RABToBeReleasedList, erabList_p, sizeof(E_RABList_t));
            } break;
            case ProtocolIE_ID_id_SecurityContext:
            {
                SecurityContext_t  securityContext;
                SecurityContext_t *securityContext_p = &securityContext;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SecurityContext, (void**)&securityContext_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE securityContext failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SecurityContext, securityContext_p);
                memcpy(&pathSwitchRequestAcknowledgeIEs->securityContext, securityContext_p, sizeof(SecurityContext_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                pathSwitchRequestAcknowledgeIEs->presenceMask |= PATHSWITCHREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&pathSwitchRequestAcknowledgeIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message pathswitchrequestacknowledgeies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_overloadstarties(
    OverloadStartIEs_t *overloadStartIEs,
    ANY_t *any_p) {

    OverloadStart_t  overloadStart;
    OverloadStart_t *overloadStart_p = &overloadStart;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(overloadStartIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_OverloadStart, (void**)&overloadStart_p);

    for (i = 0; i < overloadStart_p->overloadStart_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = overloadStart_p->overloadStart_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_OverloadResponse:
            {
                OverloadResponse_t  overloadResponse;
                OverloadResponse_t *overloadResponse_p = &overloadResponse;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_OverloadResponse, (void**)&overloadResponse_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE overloadResponse failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_OverloadResponse, overloadResponse_p);
                memcpy(&overloadStartIEs->overloadResponse, overloadResponse_p, sizeof(OverloadResponse_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message overloadstarties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_nasnondeliveryindicationies(
    NASNonDeliveryIndicationIEs_t *nasNonDeliveryIndicationIEs,
    ANY_t *any_p) {

    NASNonDeliveryIndication_t  nASNonDeliveryIndication;
    NASNonDeliveryIndication_t *nASNonDeliveryIndication_p = &nASNonDeliveryIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(nasNonDeliveryIndicationIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_NASNonDeliveryIndication, (void**)&nASNonDeliveryIndication_p);

    for (i = 0; i < nASNonDeliveryIndication_p->nasNonDeliveryIndication_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = nASNonDeliveryIndication_p->nasNonDeliveryIndication_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&nasNonDeliveryIndicationIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&nasNonDeliveryIndicationIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_NAS_PDU:
            {
                NAS_PDU_t  naspdu;
                NAS_PDU_t *naspdu_p = &naspdu;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_NAS_PDU, (void**)&naspdu_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE nas_pdu failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_NAS_PDU, naspdu_p);
                memcpy(&nasNonDeliveryIndicationIEs->nas_pdu, naspdu_p, sizeof(NAS_PDU_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&nasNonDeliveryIndicationIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message nasnondeliveryindicationies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_handovercancelacknowledgeies(
    HandoverCancelAcknowledgeIEs_t *handoverCancelAcknowledgeIEs,
    ANY_t *any_p) {

    HandoverCancelAcknowledge_t  handoverCancelAcknowledge;
    HandoverCancelAcknowledge_t *handoverCancelAcknowledge_p = &handoverCancelAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(handoverCancelAcknowledgeIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_HandoverCancelAcknowledge, (void**)&handoverCancelAcknowledge_p);

    for (i = 0; i < handoverCancelAcknowledge_p->handoverCancelAcknowledge_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = handoverCancelAcknowledge_p->handoverCancelAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&handoverCancelAcknowledgeIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&handoverCancelAcknowledgeIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                handoverCancelAcknowledgeIEs->presenceMask |= HANDOVERCANCELACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&handoverCancelAcknowledgeIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message handovercancelacknowledgeies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_enbstatustransferies(
    ENBStatusTransferIEs_t *enbStatusTransferIEs,
    ANY_t *any_p) {

    ENBStatusTransfer_t  eNBStatusTransfer;
    ENBStatusTransfer_t *eNBStatusTransfer_p = &eNBStatusTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(enbStatusTransferIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ENBStatusTransfer, (void**)&eNBStatusTransfer_p);

    for (i = 0; i < eNBStatusTransfer_p->eNBStatusTransfer_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = eNBStatusTransfer_p->eNBStatusTransfer_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&enbStatusTransferIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&enbStatusTransferIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_StatusTransfer_TransparentContainer:
            {
                ENB_StatusTransfer_TransparentContainer_t  enbStatusTransferTransparentContainer;
                ENB_StatusTransfer_TransparentContainer_t *enbStatusTransferTransparentContainer_p = &enbStatusTransferTransparentContainer;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_StatusTransfer_TransparentContainer, (void**)&enbStatusTransferTransparentContainer_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_StatusTransfer_TransparentContainer failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_StatusTransfer_TransparentContainer, enbStatusTransferTransparentContainer_p);
                memcpy(&enbStatusTransferIEs->eNB_StatusTransfer_TransparentContainer, enbStatusTransferTransparentContainer_p, sizeof(ENB_StatusTransfer_TransparentContainer_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message enbstatustransferies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_handovercancelies(
    HandoverCancelIEs_t *handoverCancelIEs,
    ANY_t *any_p) {

    HandoverCancel_t  handoverCancel;
    HandoverCancel_t *handoverCancel_p = &handoverCancel;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(handoverCancelIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_HandoverCancel, (void**)&handoverCancel_p);

    for (i = 0; i < handoverCancel_p->handoverCancel_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = handoverCancel_p->handoverCancel_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&handoverCancelIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&handoverCancelIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&handoverCancelIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message handovercancelies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabreleaseresponseies(
    E_RABReleaseResponseIEs_t *e_RABReleaseResponseIEs,
    ANY_t *any_p) {

    E_RABReleaseResponse_t  e_RABReleaseResponse;
    E_RABReleaseResponse_t *e_RABReleaseResponse_p = &e_RABReleaseResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(e_RABReleaseResponseIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_E_RABReleaseResponse, (void**)&e_RABReleaseResponse_p);

    for (i = 0; i < e_RABReleaseResponse_p->e_RABReleaseResponse_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = e_RABReleaseResponse_p->e_RABReleaseResponse_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&e_RABReleaseResponseIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&e_RABReleaseResponseIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABReleaseListBearerRelComp:
            {
                E_RABReleaseListBearerRelComp_t  erabReleaseListBearerRelComp;
                E_RABReleaseListBearerRelComp_t *erabReleaseListBearerRelComp_p = &erabReleaseListBearerRelComp;
                e_RABReleaseResponseIEs->presenceMask |= E_RABRELEASERESPONSEIES_E_RABRELEASELISTBEARERRELCOMP_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABReleaseListBearerRelComp, (void**)&erabReleaseListBearerRelComp_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABReleaseListBearerRelComp failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABReleaseListBearerRelComp, erabReleaseListBearerRelComp_p);
                memcpy(&e_RABReleaseResponseIEs->e_RABReleaseListBearerRelComp, erabReleaseListBearerRelComp_p, sizeof(E_RABReleaseListBearerRelComp_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABFailedToReleaseList:
            {
                E_RABList_t  erabList;
                E_RABList_t *erabList_p = &erabList;
                e_RABReleaseResponseIEs->presenceMask |= E_RABRELEASERESPONSEIES_E_RABLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABList, (void**)&erabList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABFailedToReleaseList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABList, erabList_p);
                memcpy(&e_RABReleaseResponseIEs->e_RABFailedToReleaseList, erabList_p, sizeof(E_RABList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                e_RABReleaseResponseIEs->presenceMask |= E_RABRELEASERESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&e_RABReleaseResponseIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabreleaseresponseies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_uecontextreleaserequesties(
    UEContextReleaseRequestIEs_t *ueContextReleaseRequestIEs,
    ANY_t *any_p) {

    UEContextReleaseRequest_t  uEContextReleaseRequest;
    UEContextReleaseRequest_t *uEContextReleaseRequest_p = &uEContextReleaseRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(ueContextReleaseRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_UEContextReleaseRequest, (void**)&uEContextReleaseRequest_p);

    for (i = 0; i < uEContextReleaseRequest_p->ueContextReleaseRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = uEContextReleaseRequest_p->ueContextReleaseRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&ueContextReleaseRequestIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&ueContextReleaseRequestIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&ueContextReleaseRequestIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message uecontextreleaserequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_tracestarties(
    TraceStartIEs_t *traceStartIEs,
    ANY_t *any_p) {

    TraceStart_t  traceStart;
    TraceStart_t *traceStart_p = &traceStart;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(traceStartIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_TraceStart, (void**)&traceStart_p);

    for (i = 0; i < traceStart_p->traceStart_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = traceStart_p->traceStart_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&traceStartIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&traceStartIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_TraceActivation:
            {
                TraceActivation_t  traceActivation;
                TraceActivation_t *traceActivation_p = &traceActivation;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TraceActivation, (void**)&traceActivation_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE traceActivation failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TraceActivation, traceActivation_p);
                memcpy(&traceStartIEs->traceActivation, traceActivation_p, sizeof(TraceActivation_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message tracestarties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_mmeconfigurationtransferies(
    MMEConfigurationTransferIEs_t *mmeConfigurationTransferIEs,
    ANY_t *any_p) {

    MMEConfigurationTransfer_t  mMEConfigurationTransfer;
    MMEConfigurationTransfer_t *mMEConfigurationTransfer_p = &mMEConfigurationTransfer;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(mmeConfigurationTransferIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_MMEConfigurationTransfer, (void**)&mMEConfigurationTransfer_p);

    for (i = 0; i < mMEConfigurationTransfer_p->mmeConfigurationTransfer_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = mMEConfigurationTransfer_p->mmeConfigurationTransfer_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_SONConfigurationTransferMCT:
            {
                SONConfigurationTransfer_t  sonConfigurationTransfer;
                SONConfigurationTransfer_t *sonConfigurationTransfer_p = &sonConfigurationTransfer;
                mmeConfigurationTransferIEs->presenceMask |= MMECONFIGURATIONTRANSFERIES_SONCONFIGURATIONTRANSFER_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SONConfigurationTransfer, (void**)&sonConfigurationTransfer_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE sonConfigurationTransferMCT failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SONConfigurationTransfer, sonConfigurationTransfer_p);
                memcpy(&mmeConfigurationTransferIEs->sonConfigurationTransferMCT, sonConfigurationTransfer_p, sizeof(SONConfigurationTransfer_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message mmeconfigurationtransferies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_tracefailureindicationies(
    TraceFailureIndicationIEs_t *traceFailureIndicationIEs,
    ANY_t *any_p) {

    TraceFailureIndication_t  traceFailureIndication;
    TraceFailureIndication_t *traceFailureIndication_p = &traceFailureIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(traceFailureIndicationIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_TraceFailureIndication, (void**)&traceFailureIndication_p);

    for (i = 0; i < traceFailureIndication_p->traceFailureIndication_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = traceFailureIndication_p->traceFailureIndication_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&traceFailureIndicationIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&traceFailureIndicationIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_E_UTRAN_Trace_ID:
            {
                E_UTRAN_Trace_ID_t  eutranTraceID;
                E_UTRAN_Trace_ID_t *eutranTraceID_p = &eutranTraceID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_UTRAN_Trace_ID, (void**)&eutranTraceID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_UTRAN_Trace_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_UTRAN_Trace_ID, eutranTraceID_p);
                memcpy(&traceFailureIndicationIEs->e_UTRAN_Trace_ID, eutranTraceID_p, sizeof(E_UTRAN_Trace_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&traceFailureIndicationIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message tracefailureindicationies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_initialcontextsetupfailureies(
    InitialContextSetupFailureIEs_t *initialContextSetupFailureIEs,
    ANY_t *any_p) {

    InitialContextSetupFailure_t  initialContextSetupFailure;
    InitialContextSetupFailure_t *initialContextSetupFailure_p = &initialContextSetupFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(initialContextSetupFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_InitialContextSetupFailure, (void**)&initialContextSetupFailure_p);

    for (i = 0; i < initialContextSetupFailure_p->initialContextSetupFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = initialContextSetupFailure_p->initialContextSetupFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&initialContextSetupFailureIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&initialContextSetupFailureIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&initialContextSetupFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                initialContextSetupFailureIEs->presenceMask |= INITIALCONTEXTSETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&initialContextSetupFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message initialcontextsetupfailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_errorindicationies(
    ErrorIndicationIEs_t *errorIndicationIEs,
    ANY_t *any_p) {

    ErrorIndication_t  errorIndication;
    ErrorIndication_t *errorIndication_p = &errorIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(errorIndicationIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ErrorIndication, (void**)&errorIndication_p);

    for (i = 0; i < errorIndication_p->errorIndication_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = errorIndication_p->errorIndication_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                errorIndicationIEs->presenceMask |= ERRORINDICATIONIES_MME_UE_S1AP_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&errorIndicationIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                errorIndicationIEs->presenceMask |= ERRORINDICATIONIES_ENB_UE_S1AP_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&errorIndicationIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                errorIndicationIEs->presenceMask |= ERRORINDICATIONIES_CAUSE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&errorIndicationIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                errorIndicationIEs->presenceMask |= ERRORINDICATIONIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&errorIndicationIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message errorindicationies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_handovernotifyies(
    HandoverNotifyIEs_t *handoverNotifyIEs,
    ANY_t *any_p) {

    HandoverNotify_t  handoverNotify;
    HandoverNotify_t *handoverNotify_p = &handoverNotify;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(handoverNotifyIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_HandoverNotify, (void**)&handoverNotify_p);

    for (i = 0; i < handoverNotify_p->handoverNotify_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = handoverNotify_p->handoverNotify_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&handoverNotifyIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&handoverNotifyIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_EUTRAN_CGI:
            {
                EUTRAN_CGI_t  eutrancgi;
                EUTRAN_CGI_t *eutrancgi_p = &eutrancgi;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_EUTRAN_CGI, (void**)&eutrancgi_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eutran_cgi failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_EUTRAN_CGI, eutrancgi_p);
                memcpy(&handoverNotifyIEs->eutran_cgi, eutrancgi_p, sizeof(EUTRAN_CGI_t));
            } break;
            case ProtocolIE_ID_id_TAI:
            {
                TAI_t  tai;
                TAI_t *tai_p = &tai;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TAI, (void**)&tai_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE tai failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TAI, tai_p);
                memcpy(&handoverNotifyIEs->tai, tai_p, sizeof(TAI_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message handovernotifyies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_downlinks1cdma2000tunnelingies(
    DownlinkS1cdma2000tunnelingIEs_t *downlinkS1cdma2000tunnelingIEs,
    ANY_t *any_p) {

    DownlinkS1cdma2000tunneling_t  downlinkS1cdma2000tunneling;
    DownlinkS1cdma2000tunneling_t *downlinkS1cdma2000tunneling_p = &downlinkS1cdma2000tunneling;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(downlinkS1cdma2000tunnelingIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_DownlinkS1cdma2000tunneling, (void**)&downlinkS1cdma2000tunneling_p);

    for (i = 0; i < downlinkS1cdma2000tunneling_p->downlinkS1cdma2000tunneling_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = downlinkS1cdma2000tunneling_p->downlinkS1cdma2000tunneling_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&downlinkS1cdma2000tunnelingIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&downlinkS1cdma2000tunnelingIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABSubjecttoDataForwardingList:
            {
                E_RABDataForwardingList_t  erabDataForwardingList;
                E_RABDataForwardingList_t *erabDataForwardingList_p = &erabDataForwardingList;
                downlinkS1cdma2000tunnelingIEs->presenceMask |= DOWNLINKS1CDMA2000TUNNELINGIES_E_RABDATAFORWARDINGLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABDataForwardingList, (void**)&erabDataForwardingList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABSubjecttoDataForwardingList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABDataForwardingList, erabDataForwardingList_p);
                memcpy(&downlinkS1cdma2000tunnelingIEs->e_RABSubjecttoDataForwardingList, erabDataForwardingList_p, sizeof(E_RABDataForwardingList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_cdma2000HOStatus:
            {
                Cdma2000HOStatus_t  cdma2000HOStatus;
                Cdma2000HOStatus_t *cdma2000HOStatus_p = &cdma2000HOStatus;
                downlinkS1cdma2000tunnelingIEs->presenceMask |= DOWNLINKS1CDMA2000TUNNELINGIES_CDMA2000HOSTATUS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000HOStatus, (void**)&cdma2000HOStatus_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000HOStatus failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000HOStatus, cdma2000HOStatus_p);
                memcpy(&downlinkS1cdma2000tunnelingIEs->cdma2000HOStatus, cdma2000HOStatus_p, sizeof(Cdma2000HOStatus_t));
            } break;
            case ProtocolIE_ID_id_cdma2000RATType:
            {
                Cdma2000RATType_t  cdma2000RATType;
                Cdma2000RATType_t *cdma2000RATType_p = &cdma2000RATType;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000RATType, (void**)&cdma2000RATType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000RATType failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000RATType, cdma2000RATType_p);
                memcpy(&downlinkS1cdma2000tunnelingIEs->cdma2000RATType, cdma2000RATType_p, sizeof(Cdma2000RATType_t));
            } break;
            case ProtocolIE_ID_id_cdma2000PDU:
            {
                Cdma2000PDU_t  cdma2000PDU;
                Cdma2000PDU_t *cdma2000PDU_p = &cdma2000PDU;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cdma2000PDU, (void**)&cdma2000PDU_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cdma2000PDU failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cdma2000PDU, cdma2000PDU_p);
                memcpy(&downlinkS1cdma2000tunnelingIEs->cdma2000PDU, cdma2000PDU_p, sizeof(Cdma2000PDU_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message downlinks1cdma2000tunnelingies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_mmeconfigurationupdateacknowledgeies(
    MMEConfigurationUpdateAcknowledgeIEs_t *mmeConfigurationUpdateAcknowledgeIEs,
    ANY_t *any_p) {

    MMEConfigurationUpdateAcknowledge_t  mMEConfigurationUpdateAcknowledge;
    MMEConfigurationUpdateAcknowledge_t *mMEConfigurationUpdateAcknowledge_p = &mMEConfigurationUpdateAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(mmeConfigurationUpdateAcknowledgeIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_MMEConfigurationUpdateAcknowledge, (void**)&mMEConfigurationUpdateAcknowledge_p);

    for (i = 0; i < mMEConfigurationUpdateAcknowledge_p->mmeConfigurationUpdateAcknowledge_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = mMEConfigurationUpdateAcknowledge_p->mmeConfigurationUpdateAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                mmeConfigurationUpdateAcknowledgeIEs->presenceMask |= MMECONFIGURATIONUPDATEACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&mmeConfigurationUpdateAcknowledgeIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message mmeconfigurationupdateacknowledgeies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_handoverrequestacknowledgeies(
    HandoverRequestAcknowledgeIEs_t *handoverRequestAcknowledgeIEs,
    ANY_t *any_p) {

    HandoverRequestAcknowledge_t  handoverRequestAcknowledge;
    HandoverRequestAcknowledge_t *handoverRequestAcknowledge_p = &handoverRequestAcknowledge;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(handoverRequestAcknowledgeIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_HandoverRequestAcknowledge, (void**)&handoverRequestAcknowledge_p);

    for (i = 0; i < handoverRequestAcknowledge_p->handoverRequestAcknowledge_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = handoverRequestAcknowledge_p->handoverRequestAcknowledge_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&handoverRequestAcknowledgeIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&handoverRequestAcknowledgeIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_E_RABAdmittedList:
            {
                E_RABAdmittedList_t  erabAdmittedList;
                E_RABAdmittedList_t *erabAdmittedList_p = &erabAdmittedList;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABAdmittedList, (void**)&erabAdmittedList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABAdmittedList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABAdmittedList, erabAdmittedList_p);
                memcpy(&handoverRequestAcknowledgeIEs->e_RABAdmittedList, erabAdmittedList_p, sizeof(E_RABAdmittedList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABFailedToSetupListHOReqAck:
            {
                E_RABFailedtoSetupListHOReqAck_t  erabFailedtoSetupListHOReqAck;
                E_RABFailedtoSetupListHOReqAck_t *erabFailedtoSetupListHOReqAck_p = &erabFailedtoSetupListHOReqAck;
                handoverRequestAcknowledgeIEs->presenceMask |= HANDOVERREQUESTACKNOWLEDGEIES_E_RABFAILEDTOSETUPLISTHOREQACK_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABFailedtoSetupListHOReqAck, (void**)&erabFailedtoSetupListHOReqAck_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABFailedToSetupListHOReqAck failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABFailedtoSetupListHOReqAck, erabFailedtoSetupListHOReqAck_p);
                memcpy(&handoverRequestAcknowledgeIEs->e_RABFailedToSetupListHOReqAck, erabFailedtoSetupListHOReqAck_p, sizeof(E_RABFailedtoSetupListHOReqAck_t));
            } break;
            case ProtocolIE_ID_id_Target_ToSource_TransparentContainer:
            {
                Target_ToSource_TransparentContainer_t  targetToSourceTransparentContainer;
                Target_ToSource_TransparentContainer_t *targetToSourceTransparentContainer_p = &targetToSourceTransparentContainer;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Target_ToSource_TransparentContainer, (void**)&targetToSourceTransparentContainer_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE target_ToSource_TransparentContainer failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Target_ToSource_TransparentContainer, targetToSourceTransparentContainer_p);
                memcpy(&handoverRequestAcknowledgeIEs->target_ToSource_TransparentContainer, targetToSourceTransparentContainer_p, sizeof(Target_ToSource_TransparentContainer_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CSG_Id:
            {
                CSG_Id_t  csgId;
                CSG_Id_t *csgId_p = &csgId;
                handoverRequestAcknowledgeIEs->presenceMask |= HANDOVERREQUESTACKNOWLEDGEIES_CSG_ID_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CSG_Id, (void**)&csgId_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE csG_Id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CSG_Id, csgId_p);
                memcpy(&handoverRequestAcknowledgeIEs->csG_Id, csgId_p, sizeof(CSG_Id_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                handoverRequestAcknowledgeIEs->presenceMask |= HANDOVERREQUESTACKNOWLEDGEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&handoverRequestAcknowledgeIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message handoverrequestacknowledgeies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabsetuprequesties(
    E_RABSetupRequestIEs_t *e_RABSetupRequestIEs,
    ANY_t *any_p) {

    E_RABSetupRequest_t  e_RABSetupRequest;
    E_RABSetupRequest_t *e_RABSetupRequest_p = &e_RABSetupRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(e_RABSetupRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_E_RABSetupRequest, (void**)&e_RABSetupRequest_p);

    for (i = 0; i < e_RABSetupRequest_p->e_RABSetupRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = e_RABSetupRequest_p->e_RABSetupRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&e_RABSetupRequestIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&e_RABSetupRequestIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                UEAggregateMaximumBitrate_t  ueAggregateMaximumBitrate;
                UEAggregateMaximumBitrate_t *ueAggregateMaximumBitrate_p = &ueAggregateMaximumBitrate;
                e_RABSetupRequestIEs->presenceMask |= E_RABSETUPREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEAggregateMaximumBitrate, (void**)&ueAggregateMaximumBitrate_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uEaggregateMaximumBitrate failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEAggregateMaximumBitrate, ueAggregateMaximumBitrate_p);
                memcpy(&e_RABSetupRequestIEs->uEaggregateMaximumBitrate, ueAggregateMaximumBitrate_p, sizeof(UEAggregateMaximumBitrate_t));
            } break;
            case ProtocolIE_ID_id_E_RABToBeSetupListBearerSUReq:
            {
                E_RABToBeSetupListBearerSUReq_t  erabToBeSetupListBearerSUReq;
                E_RABToBeSetupListBearerSUReq_t *erabToBeSetupListBearerSUReq_p = &erabToBeSetupListBearerSUReq;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSetupListBearerSUReq, (void**)&erabToBeSetupListBearerSUReq_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSetupListBearerSUReq failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSetupListBearerSUReq, erabToBeSetupListBearerSUReq_p);
                memcpy(&e_RABSetupRequestIEs->e_RABToBeSetupListBearerSUReq, erabToBeSetupListBearerSUReq_p, sizeof(E_RABToBeSetupListBearerSUReq_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabsetuprequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_downlinkueassociatedlppatransporties(
    DownlinkUEAssociatedLPPaTransportIEs_t *downlinkUEAssociatedLPPaTransportIEs,
    ANY_t *any_p) {

    DownlinkUEAssociatedLPPaTransport_t  downlinkUEAssociatedLPPaTransport;
    DownlinkUEAssociatedLPPaTransport_t *downlinkUEAssociatedLPPaTransport_p = &downlinkUEAssociatedLPPaTransport;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(downlinkUEAssociatedLPPaTransportIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_DownlinkUEAssociatedLPPaTransport, (void**)&downlinkUEAssociatedLPPaTransport_p);

    for (i = 0; i < downlinkUEAssociatedLPPaTransport_p->downlinkUEAssociatedLPPaTransport_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = downlinkUEAssociatedLPPaTransport_p->downlinkUEAssociatedLPPaTransport_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&downlinkUEAssociatedLPPaTransportIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&downlinkUEAssociatedLPPaTransportIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_Routing_ID:
            {
                Routing_ID_t  routingID;
                Routing_ID_t *routingID_p = &routingID;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Routing_ID, (void**)&routingID_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE routing_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Routing_ID, routingID_p);
                memcpy(&downlinkUEAssociatedLPPaTransportIEs->routing_ID, routingID_p, sizeof(Routing_ID_t));
            } break;
            case ProtocolIE_ID_id_LPPa_PDU:
            {
                LPPa_PDU_t  lpPaPDU;
                LPPa_PDU_t *lpPaPDU_p = &lpPaPDU;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_LPPa_PDU, (void**)&lpPaPDU_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE lpPa_PDU failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_LPPa_PDU, lpPaPDU_p);
                memcpy(&downlinkUEAssociatedLPPaTransportIEs->lpPa_PDU, lpPaPDU_p, sizeof(LPPa_PDU_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message downlinkueassociatedlppatransporties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_killrequesties(
    KillRequestIEs_t *killRequestIEs,
    ANY_t *any_p) {

    KillRequest_t  killRequest;
    KillRequest_t *killRequest_p = &killRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(killRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_KillRequest, (void**)&killRequest_p);

    for (i = 0; i < killRequest_p->killRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = killRequest_p->killRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MessageIdentifier:
            {
                MessageIdentifier_t  messageIdentifier;
                MessageIdentifier_t *messageIdentifier_p = &messageIdentifier;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MessageIdentifier, (void**)&messageIdentifier_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE messageIdentifier failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MessageIdentifier, messageIdentifier_p);
                memcpy(&killRequestIEs->messageIdentifier, messageIdentifier_p, sizeof(MessageIdentifier_t));
            } break;
            case ProtocolIE_ID_id_SerialNumber:
            {
                SerialNumber_t  serialNumber;
                SerialNumber_t *serialNumber_p = &serialNumber;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SerialNumber, (void**)&serialNumber_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE serialNumber failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SerialNumber, serialNumber_p);
                memcpy(&killRequestIEs->serialNumber, serialNumber_p, sizeof(SerialNumber_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_WarningAreaList:
            {
                WarningAreaList_t  warningAreaList;
                WarningAreaList_t *warningAreaList_p = &warningAreaList;
                killRequestIEs->presenceMask |= KILLREQUESTIES_WARNINGAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_WarningAreaList, (void**)&warningAreaList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE warningAreaList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_WarningAreaList, warningAreaList_p);
                memcpy(&killRequestIEs->warningAreaList, warningAreaList_p, sizeof(WarningAreaList_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message killrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabmodifyresponseies(
    E_RABModifyResponseIEs_t *e_RABModifyResponseIEs,
    ANY_t *any_p) {

    E_RABModifyResponse_t  e_RABModifyResponse;
    E_RABModifyResponse_t *e_RABModifyResponse_p = &e_RABModifyResponse;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(e_RABModifyResponseIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_E_RABModifyResponse, (void**)&e_RABModifyResponse_p);

    for (i = 0; i < e_RABModifyResponse_p->e_RABModifyResponse_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = e_RABModifyResponse_p->e_RABModifyResponse_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&e_RABModifyResponseIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&e_RABModifyResponseIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABModifyListBearerModRes:
            {
                E_RABModifyListBearerModRes_t  erabModifyListBearerModRes;
                E_RABModifyListBearerModRes_t *erabModifyListBearerModRes_p = &erabModifyListBearerModRes;
                e_RABModifyResponseIEs->presenceMask |= E_RABMODIFYRESPONSEIES_E_RABMODIFYLISTBEARERMODRES_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABModifyListBearerModRes, (void**)&erabModifyListBearerModRes_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABModifyListBearerModRes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABModifyListBearerModRes, erabModifyListBearerModRes_p);
                memcpy(&e_RABModifyResponseIEs->e_RABModifyListBearerModRes, erabModifyListBearerModRes_p, sizeof(E_RABModifyListBearerModRes_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_E_RABFailedToModifyList:
            {
                E_RABList_t  erabList;
                E_RABList_t *erabList_p = &erabList;
                e_RABModifyResponseIEs->presenceMask |= E_RABMODIFYRESPONSEIES_E_RABLIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABList, (void**)&erabList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABFailedToModifyList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABList, erabList_p);
                memcpy(&e_RABModifyResponseIEs->e_RABFailedToModifyList, erabList_p, sizeof(E_RABList_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                e_RABModifyResponseIEs->presenceMask |= E_RABMODIFYRESPONSEIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&e_RABModifyResponseIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyresponseies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabreleaseindicationies(
    E_RABReleaseIndicationIEs_t *e_RABReleaseIndicationIEs,
    ANY_t *any_p) {

    E_RABReleaseIndication_t  e_RABReleaseIndication;
    E_RABReleaseIndication_t *e_RABReleaseIndication_p = &e_RABReleaseIndication;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(e_RABReleaseIndicationIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_E_RABReleaseIndication, (void**)&e_RABReleaseIndication_p);

    for (i = 0; i < e_RABReleaseIndication_p->e_RABReleaseIndication_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = e_RABReleaseIndication_p->e_RABReleaseIndication_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&e_RABReleaseIndicationIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&e_RABReleaseIndicationIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_E_RABReleasedList:
            {
                E_RABList_t  erabList;
                E_RABList_t *erabList_p = &erabList;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABList, (void**)&erabList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABReleasedList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABList, erabList_p);
                memcpy(&e_RABReleaseIndicationIEs->e_RABReleasedList, erabList_p, sizeof(E_RABList_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabreleaseindicationies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_enbconfigurationupdatefailureies(
    ENBConfigurationUpdateFailureIEs_t *enbConfigurationUpdateFailureIEs,
    ANY_t *any_p) {

    ENBConfigurationUpdateFailure_t  eNBConfigurationUpdateFailure;
    ENBConfigurationUpdateFailure_t *eNBConfigurationUpdateFailure_p = &eNBConfigurationUpdateFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(enbConfigurationUpdateFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_ENBConfigurationUpdateFailure, (void**)&eNBConfigurationUpdateFailure_p);

    for (i = 0; i < eNBConfigurationUpdateFailure_p->eNBConfigurationUpdateFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = eNBConfigurationUpdateFailure_p->eNBConfigurationUpdateFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&enbConfigurationUpdateFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_TimeToWait:
            {
                TimeToWait_t  timeToWait;
                TimeToWait_t *timeToWait_p = &timeToWait;
                enbConfigurationUpdateFailureIEs->presenceMask |= ENBCONFIGURATIONUPDATEFAILUREIES_TIMETOWAIT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TimeToWait, (void**)&timeToWait_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE timeToWait failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TimeToWait, timeToWait_p);
                memcpy(&enbConfigurationUpdateFailureIEs->timeToWait, timeToWait_p, sizeof(TimeToWait_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                enbConfigurationUpdateFailureIEs->presenceMask |= ENBCONFIGURATIONUPDATEFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&enbConfigurationUpdateFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message enbconfigurationupdatefailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_writereplacewarningrequesties(
    WriteReplaceWarningRequestIEs_t *writeReplaceWarningRequestIEs,
    ANY_t *any_p) {

    WriteReplaceWarningRequest_t  writeReplaceWarningRequest;
    WriteReplaceWarningRequest_t *writeReplaceWarningRequest_p = &writeReplaceWarningRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(writeReplaceWarningRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_WriteReplaceWarningRequest, (void**)&writeReplaceWarningRequest_p);

    for (i = 0; i < writeReplaceWarningRequest_p->writeReplaceWarningRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = writeReplaceWarningRequest_p->writeReplaceWarningRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MessageIdentifier:
            {
                MessageIdentifier_t  messageIdentifier;
                MessageIdentifier_t *messageIdentifier_p = &messageIdentifier;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MessageIdentifier, (void**)&messageIdentifier_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE messageIdentifier failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MessageIdentifier, messageIdentifier_p);
                memcpy(&writeReplaceWarningRequestIEs->messageIdentifier, messageIdentifier_p, sizeof(MessageIdentifier_t));
            } break;
            case ProtocolIE_ID_id_SerialNumber:
            {
                SerialNumber_t  serialNumber;
                SerialNumber_t *serialNumber_p = &serialNumber;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_SerialNumber, (void**)&serialNumber_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE serialNumber failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_SerialNumber, serialNumber_p);
                memcpy(&writeReplaceWarningRequestIEs->serialNumber, serialNumber_p, sizeof(SerialNumber_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_WarningAreaList:
            {
                WarningAreaList_t  warningAreaList;
                WarningAreaList_t *warningAreaList_p = &warningAreaList;
                writeReplaceWarningRequestIEs->presenceMask |= WRITEREPLACEWARNINGREQUESTIES_WARNINGAREALIST_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_WarningAreaList, (void**)&warningAreaList_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE warningAreaList failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_WarningAreaList, warningAreaList_p);
                memcpy(&writeReplaceWarningRequestIEs->warningAreaList, warningAreaList_p, sizeof(WarningAreaList_t));
            } break;
            case ProtocolIE_ID_id_RepetitionPeriod:
            {
                RepetitionPeriod_t  repetitionPeriod;
                RepetitionPeriod_t *repetitionPeriod_p = &repetitionPeriod;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_RepetitionPeriod, (void**)&repetitionPeriod_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE repetitionPeriod failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_RepetitionPeriod, repetitionPeriod_p);
                memcpy(&writeReplaceWarningRequestIEs->repetitionPeriod, repetitionPeriod_p, sizeof(RepetitionPeriod_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_ExtendedRepetitionPeriod:
            {
                ExtendedRepetitionPeriod_t  extendedRepetitionPeriod;
                ExtendedRepetitionPeriod_t *extendedRepetitionPeriod_p = &extendedRepetitionPeriod;
                writeReplaceWarningRequestIEs->presenceMask |= WRITEREPLACEWARNINGREQUESTIES_EXTENDEDREPETITIONPERIOD_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ExtendedRepetitionPeriod, (void**)&extendedRepetitionPeriod_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE extendedRepetitionPeriod failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ExtendedRepetitionPeriod, extendedRepetitionPeriod_p);
                memcpy(&writeReplaceWarningRequestIEs->extendedRepetitionPeriod, extendedRepetitionPeriod_p, sizeof(ExtendedRepetitionPeriod_t));
            } break;
            case ProtocolIE_ID_id_NumberofBroadcastRequest:
            {
                NumberofBroadcastRequest_t  numberofBroadcastRequest;
                NumberofBroadcastRequest_t *numberofBroadcastRequest_p = &numberofBroadcastRequest;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_NumberofBroadcastRequest, (void**)&numberofBroadcastRequest_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE numberofBroadcastRequest failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_NumberofBroadcastRequest, numberofBroadcastRequest_p);
                memcpy(&writeReplaceWarningRequestIEs->numberofBroadcastRequest, numberofBroadcastRequest_p, sizeof(NumberofBroadcastRequest_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_WarningType:
            {
                WarningType_t  warningType;
                WarningType_t *warningType_p = &warningType;
                writeReplaceWarningRequestIEs->presenceMask |= WRITEREPLACEWARNINGREQUESTIES_WARNINGTYPE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_WarningType, (void**)&warningType_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE warningType failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_WarningType, warningType_p);
                memcpy(&writeReplaceWarningRequestIEs->warningType, warningType_p, sizeof(WarningType_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_WarningSecurityInfo:
            {
                WarningSecurityInfo_t  warningSecurityInfo;
                WarningSecurityInfo_t *warningSecurityInfo_p = &warningSecurityInfo;
                writeReplaceWarningRequestIEs->presenceMask |= WRITEREPLACEWARNINGREQUESTIES_WARNINGSECURITYINFO_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_WarningSecurityInfo, (void**)&warningSecurityInfo_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE warningSecurityInfo failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_WarningSecurityInfo, warningSecurityInfo_p);
                memcpy(&writeReplaceWarningRequestIEs->warningSecurityInfo, warningSecurityInfo_p, sizeof(WarningSecurityInfo_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_DataCodingScheme:
            {
                DataCodingScheme_t  dataCodingScheme;
                DataCodingScheme_t *dataCodingScheme_p = &dataCodingScheme;
                writeReplaceWarningRequestIEs->presenceMask |= WRITEREPLACEWARNINGREQUESTIES_DATACODINGSCHEME_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_DataCodingScheme, (void**)&dataCodingScheme_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE dataCodingScheme failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_DataCodingScheme, dataCodingScheme_p);
                memcpy(&writeReplaceWarningRequestIEs->dataCodingScheme, dataCodingScheme_p, sizeof(DataCodingScheme_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_WarningMessageContents:
            {
                WarningMessageContents_t  warningMessageContents;
                WarningMessageContents_t *warningMessageContents_p = &warningMessageContents;
                writeReplaceWarningRequestIEs->presenceMask |= WRITEREPLACEWARNINGREQUESTIES_WARNINGMESSAGECONTENTS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_WarningMessageContents, (void**)&warningMessageContents_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE warningMessageContents failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_WarningMessageContents, warningMessageContents_p);
                memcpy(&writeReplaceWarningRequestIEs->warningMessageContents, warningMessageContents_p, sizeof(WarningMessageContents_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_ConcurrentWarningMessageIndicator:
            {
                ConcurrentWarningMessageIndicator_t  concurrentWarningMessageIndicator;
                ConcurrentWarningMessageIndicator_t *concurrentWarningMessageIndicator_p = &concurrentWarningMessageIndicator;
                writeReplaceWarningRequestIEs->presenceMask |= WRITEREPLACEWARNINGREQUESTIES_CONCURRENTWARNINGMESSAGEINDICATOR_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ConcurrentWarningMessageIndicator, (void**)&concurrentWarningMessageIndicator_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE concurrentWarningMessageIndicator failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ConcurrentWarningMessageIndicator, concurrentWarningMessageIndicator_p);
                memcpy(&writeReplaceWarningRequestIEs->concurrentWarningMessageIndicator, concurrentWarningMessageIndicator_p, sizeof(ConcurrentWarningMessageIndicator_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message writereplacewarningrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_s1setupfailureies(
    S1SetupFailureIEs_t *s1SetupFailureIEs,
    ANY_t *any_p) {

    S1SetupFailure_t  s1SetupFailure;
    S1SetupFailure_t *s1SetupFailure_p = &s1SetupFailure;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(s1SetupFailureIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_S1SetupFailure, (void**)&s1SetupFailure_p);

    for (i = 0; i < s1SetupFailure_p->s1SetupFailure_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = s1SetupFailure_p->s1SetupFailure_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_Cause:
            {
                Cause_t  cause;
                Cause_t *cause_p = &cause;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Cause, (void**)&cause_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE cause failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Cause, cause_p);
                memcpy(&s1SetupFailureIEs->cause, cause_p, sizeof(Cause_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_TimeToWait:
            {
                TimeToWait_t  timeToWait;
                TimeToWait_t *timeToWait_p = &timeToWait;
                s1SetupFailureIEs->presenceMask |= S1SETUPFAILUREIES_TIMETOWAIT_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TimeToWait, (void**)&timeToWait_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE timeToWait failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TimeToWait, timeToWait_p);
                memcpy(&s1SetupFailureIEs->timeToWait, timeToWait_p, sizeof(TimeToWait_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_CriticalityDiagnostics:
            {
                CriticalityDiagnostics_t  criticalityDiagnostics;
                CriticalityDiagnostics_t *criticalityDiagnostics_p = &criticalityDiagnostics;
                s1SetupFailureIEs->presenceMask |= S1SETUPFAILUREIES_CRITICALITYDIAGNOSTICS_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_CriticalityDiagnostics, (void**)&criticalityDiagnostics_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE criticalityDiagnostics failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_CriticalityDiagnostics, criticalityDiagnostics_p);
                memcpy(&s1SetupFailureIEs->criticalityDiagnostics, criticalityDiagnostics_p, sizeof(CriticalityDiagnostics_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message s1setupfailureies", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabmodifyrequesties(
    E_RABModifyRequestIEs_t *e_RABModifyRequestIEs,
    ANY_t *any_p) {

    E_RABModifyRequest_t  e_RABModifyRequest;
    E_RABModifyRequest_t *e_RABModifyRequest_p = &e_RABModifyRequest;
    int i, decoded = 0;
    int tempDecoded = 0;
    assert(any_p != NULL);
    assert(e_RABModifyRequestIEs != NULL);

    ANY_to_type_aper(any_p, &asn_DEF_E_RABModifyRequest, (void**)&e_RABModifyRequest_p);

    for (i = 0; i < e_RABModifyRequest_p->e_RABModifyRequest_ies.list.count; i++) {
        IE_t *ie_p;
        ie_p = e_RABModifyRequest_p->e_RABModifyRequest_ies.list.array[i];
        switch(ie_p->id) {
            case ProtocolIE_ID_id_MME_UE_S1AP_ID:
            {
                MME_UE_S1AP_ID_t  mmeues1apid;
                MME_UE_S1AP_ID_t *mmeues1apid_p = &mmeues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_MME_UE_S1AP_ID, (void**)&mmeues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE mme_ue_s1ap_id failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_MME_UE_S1AP_ID, mmeues1apid_p);
                memcpy(&e_RABModifyRequestIEs->mme_ue_s1ap_id, mmeues1apid_p, sizeof(MME_UE_S1AP_ID_t));
            } break;
            case ProtocolIE_ID_id_eNB_UE_S1AP_ID:
            {
                ENB_UE_S1AP_ID_t  enbues1apid;
                ENB_UE_S1AP_ID_t *enbues1apid_p = &enbues1apid;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_ENB_UE_S1AP_ID, (void**)&enbues1apid_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE eNB_UE_S1AP_ID failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_ENB_UE_S1AP_ID, enbues1apid_p);
                memcpy(&e_RABModifyRequestIEs->eNB_UE_S1AP_ID, enbues1apid_p, sizeof(ENB_UE_S1AP_ID_t));
            } break;
            /* Optional field */
            case ProtocolIE_ID_id_uEaggregateMaximumBitrate:
            {
                UEAggregateMaximumBitrate_t  ueAggregateMaximumBitrate;
                UEAggregateMaximumBitrate_t *ueAggregateMaximumBitrate_p = &ueAggregateMaximumBitrate;
                e_RABModifyRequestIEs->presenceMask |= E_RABMODIFYREQUESTIES_UEAGGREGATEMAXIMUMBITRATE_PRESENT;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UEAggregateMaximumBitrate, (void**)&ueAggregateMaximumBitrate_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uEaggregateMaximumBitrate failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UEAggregateMaximumBitrate, ueAggregateMaximumBitrate_p);
                memcpy(&e_RABModifyRequestIEs->uEaggregateMaximumBitrate, ueAggregateMaximumBitrate_p, sizeof(UEAggregateMaximumBitrate_t));
            } break;
            case ProtocolIE_ID_id_E_RABToBeModifiedListBearerModReq:
            {
                E_RABToBeModifiedListBearerModReq_t  erabToBeModifiedListBearerModReq;
                E_RABToBeModifiedListBearerModReq_t *erabToBeModifiedListBearerModReq_p = &erabToBeModifiedListBearerModReq;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeModifiedListBearerModReq, (void**)&erabToBeModifiedListBearerModReq_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeModifiedListBearerModReq failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeModifiedListBearerModReq, erabToBeModifiedListBearerModReq_p);
                memcpy(&e_RABModifyRequestIEs->e_RABToBeModifiedListBearerModReq, erabToBeModifiedListBearerModReq_p, sizeof(E_RABToBeModifiedListBearerModReq_t));
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabtobesetuplistbearersureq(
    E_RABToBeSetupListBearerSUReqIEs_t *e_RABToBeSetupListBearerSUReqIEs,
    E_RABToBeSetupListBearerSUReq_t *e_RABToBeSetupListBearerSUReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABToBeSetupListBearerSUReq->list.count; i++) {
        IE_t *ie_p = e_RABToBeSetupListBearerSUReq->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABToBeSetupItemBearerSUReq:
            {
                E_RABToBeSetupItemBearerSUReq_t *erabToBeSetupItemBearerSUReq_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSetupItemBearerSUReq, (void**)&erabToBeSetupItemBearerSUReq_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSetupItemBearerSUReq failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSetupItemBearerSUReq, erabToBeSetupItemBearerSUReq_p);
                ASN_SEQUENCE_ADD(&e_RABToBeSetupListBearerSUReqIEs->e_RABToBeSetupItemBearerSUReq, erabToBeSetupItemBearerSUReq_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_ue_associatedlogicals1_connectionlistresack(
    UE_associatedLogicalS1_ConnectionListResAckIEs_t *uE_associatedLogicalS1_ConnectionListResAckIEs,
    UE_associatedLogicalS1_ConnectionListResAck_t *uE_associatedLogicalS1_ConnectionListResAck) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < uE_associatedLogicalS1_ConnectionListResAck->list.count; i++) {
        IE_t *ie_p = uE_associatedLogicalS1_ConnectionListResAck->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem:
            {
                UE_associatedLogicalS1_ConnectionItem_t *uEassociatedLogicalS1ConnectionItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UE_associatedLogicalS1_ConnectionItem, (void**)&uEassociatedLogicalS1ConnectionItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uE_associatedLogicalS1_ConnectionItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UE_associatedLogicalS1_ConnectionItem, uEassociatedLogicalS1ConnectionItem_p);
                ASN_SEQUENCE_ADD(&uE_associatedLogicalS1_ConnectionListResAckIEs->uE_associatedLogicalS1_ConnectionItemResAck, uEassociatedLogicalS1ConnectionItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_tailist(
    TAIListIEs_t *taiListIEs,
    TAIList_t *taiList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < taiList->list.count; i++) {
        IE_t *ie_p = taiList->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_TAIItem:
            {
                TAIItem_t *taiItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_TAIItem, (void**)&taiItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE taiItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_TAIItem, taiItem_p);
                ASN_SEQUENCE_ADD(&taiListIEs->taiItem, taiItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabadmittedlist(
    E_RABAdmittedListIEs_t *e_RABAdmittedListIEs,
    E_RABAdmittedList_t *e_RABAdmittedList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABAdmittedList->list.count; i++) {
        IE_t *ie_p = e_RABAdmittedList->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABAdmittedItem:
            {
                E_RABAdmittedItem_t *erabAdmittedItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABAdmittedItem, (void**)&erabAdmittedItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABAdmittedItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABAdmittedItem, erabAdmittedItem_p);
                ASN_SEQUENCE_ADD(&e_RABAdmittedListIEs->e_RABAdmittedItem, erabAdmittedItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rablist(
    E_RABListIEs_t *e_RABListIEs,
    E_RABList_t *e_RABList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABList->list.count; i++) {
        IE_t *ie_p = e_RABList->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABItem:
            {
                E_RABItem_t *erabItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABItem, (void**)&erabItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABItem, erabItem_p);
                ASN_SEQUENCE_ADD(&e_RABListIEs->e_RABItem, erabItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabtobesetuplisthoreq(
    E_RABToBeSetupListHOReqIEs_t *e_RABToBeSetupListHOReqIEs,
    E_RABToBeSetupListHOReq_t *e_RABToBeSetupListHOReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABToBeSetupListHOReq->list.count; i++) {
        IE_t *ie_p = e_RABToBeSetupListHOReq->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABToBeSetupItemHOReq:
            {
                E_RABToBeSetupItemHOReq_t *erabToBeSetupItemHOReq_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSetupItemHOReq, (void**)&erabToBeSetupItemHOReq_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSetupItemHOReq failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSetupItemHOReq, erabToBeSetupItemHOReq_p);
                ASN_SEQUENCE_ADD(&e_RABToBeSetupListHOReqIEs->e_RABToBeSetupItemHOReq, erabToBeSetupItemHOReq_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabsetuplistbearersures(
    E_RABSetupListBearerSUResIEs_t *e_RABSetupListBearerSUResIEs,
    E_RABSetupListBearerSURes_t *e_RABSetupListBearerSURes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABSetupListBearerSURes->list.count; i++) {
        IE_t *ie_p = e_RABSetupListBearerSURes->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABSetupItemBearerSURes:
            {
                E_RABSetupItemBearerSURes_t *erabSetupItemBearerSURes_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABSetupItemBearerSURes, (void**)&erabSetupItemBearerSURes_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABSetupItemBearerSURes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABSetupItemBearerSURes, erabSetupItemBearerSURes_p);
                ASN_SEQUENCE_ADD(&e_RABSetupListBearerSUResIEs->e_RABSetupItemBearerSURes, erabSetupItemBearerSURes_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabtobesetuplistctxtsureq(
    E_RABToBeSetupListCtxtSUReqIEs_t *e_RABToBeSetupListCtxtSUReqIEs,
    E_RABToBeSetupListCtxtSUReq_t *e_RABToBeSetupListCtxtSUReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABToBeSetupListCtxtSUReq->list.count; i++) {
        IE_t *ie_p = e_RABToBeSetupListCtxtSUReq->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABToBeSetupItemCtxtSUReq:
            {
                E_RABToBeSetupItemCtxtSUReq_t *erabToBeSetupItemCtxtSUReq_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSetupItemCtxtSUReq, (void**)&erabToBeSetupItemCtxtSUReq_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSetupItemCtxtSUReq failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSetupItemCtxtSUReq, erabToBeSetupItemCtxtSUReq_p);
                ASN_SEQUENCE_ADD(&e_RABToBeSetupListCtxtSUReqIEs->e_RABToBeSetupItemCtxtSUReq, erabToBeSetupItemCtxtSUReq_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabsetuplistctxtsures(
    E_RABSetupListCtxtSUResIEs_t *e_RABSetupListCtxtSUResIEs,
    E_RABSetupListCtxtSURes_t *e_RABSetupListCtxtSURes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABSetupListCtxtSURes->list.count; i++) {
        IE_t *ie_p = e_RABSetupListCtxtSURes->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABSetupItemCtxtSURes:
            {
                E_RABSetupItemCtxtSURes_t *erabSetupItemCtxtSURes_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABSetupItemCtxtSURes, (void**)&erabSetupItemCtxtSURes_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABSetupItemCtxtSURes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABSetupItemCtxtSURes, erabSetupItemCtxtSURes_p);
                ASN_SEQUENCE_ADD(&e_RABSetupListCtxtSUResIEs->e_RABSetupItemCtxtSURes, erabSetupItemCtxtSURes_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabinformationlist(
    E_RABInformationListIEs_t *e_RABInformationListIEs,
    E_RABInformationList_t *e_RABInformationList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABInformationList->list.count; i++) {
        IE_t *ie_p = e_RABInformationList->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABInformationListItem:
            {
                E_RABInformationListItem_t *erabInformationListItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABInformationListItem, (void**)&erabInformationListItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABInformationListItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABInformationListItem, erabInformationListItem_p);
                ASN_SEQUENCE_ADD(&e_RABInformationListIEs->e_RABInformationItem, erabInformationListItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabreleaselistbearerrelcomp(
    E_RABReleaseListBearerRelCompIEs_t *e_RABReleaseListBearerRelCompIEs,
    E_RABReleaseListBearerRelComp_t *e_RABReleaseListBearerRelComp) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABReleaseListBearerRelComp->list.count; i++) {
        IE_t *ie_p = e_RABReleaseListBearerRelComp->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABReleaseItemBearerRelComp:
            {
                E_RABReleaseItemBearerRelComp_t *erabReleaseItemBearerRelComp_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABReleaseItemBearerRelComp, (void**)&erabReleaseItemBearerRelComp_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABReleaseItemBearerRelComp failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABReleaseItemBearerRelComp, erabReleaseItemBearerRelComp_p);
                ASN_SEQUENCE_ADD(&e_RABReleaseListBearerRelCompIEs->e_RABReleaseItemBearerRelComp, erabReleaseItemBearerRelComp_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabtobemodifiedlistbearermodreq(
    E_RABToBeModifiedListBearerModReqIEs_t *e_RABToBeModifiedListBearerModReqIEs,
    E_RABToBeModifiedListBearerModReq_t *e_RABToBeModifiedListBearerModReq) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABToBeModifiedListBearerModReq->list.count; i++) {
        IE_t *ie_p = e_RABToBeModifiedListBearerModReq->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABToBeModifiedItemBearerModReq:
            {
                E_RABToBeModifiedItemBearerModReq_t *erabToBeModifiedItemBearerModReq_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeModifiedItemBearerModReq, (void**)&erabToBeModifiedItemBearerModReq_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeModifiedItemBearerModReq failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeModifiedItemBearerModReq, erabToBeModifiedItemBearerModReq_p);
                ASN_SEQUENCE_ADD(&e_RABToBeModifiedListBearerModReqIEs->e_RABToBeModifiedItemBearerModReq, erabToBeModifiedItemBearerModReq_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabtobeswitcheddllist(
    E_RABToBeSwitchedDLListIEs_t *e_RABToBeSwitchedDLListIEs,
    E_RABToBeSwitchedDLList_t *e_RABToBeSwitchedDLList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABToBeSwitchedDLList->list.count; i++) {
        IE_t *ie_p = e_RABToBeSwitchedDLList->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABToBeSwitchedDLItem:
            {
                E_RABToBeSwitchedDLItem_t *erabToBeSwitchedDLItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSwitchedDLItem, (void**)&erabToBeSwitchedDLItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSwitchedDLItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSwitchedDLItem, erabToBeSwitchedDLItem_p);
                ASN_SEQUENCE_ADD(&e_RABToBeSwitchedDLListIEs->e_RABToBeSwitchedDLItem, erabToBeSwitchedDLItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_bearers_subjecttostatustransfer_list(
    Bearers_SubjectToStatusTransfer_ListIEs_t *bearers_SubjectToStatusTransfer_ListIEs,
    Bearers_SubjectToStatusTransfer_List_t *bearers_SubjectToStatusTransfer_List) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < bearers_SubjectToStatusTransfer_List->list.count; i++) {
        IE_t *ie_p = bearers_SubjectToStatusTransfer_List->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_Bearers_SubjectToStatusTransfer_Item:
            {
                Bearers_SubjectToStatusTransfer_Item_t *bearersSubjectToStatusTransferItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_Bearers_SubjectToStatusTransfer_Item, (void**)&bearersSubjectToStatusTransferItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE bearers_SubjectToStatusTransfer_Item failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_Bearers_SubjectToStatusTransfer_Item, bearersSubjectToStatusTransferItem_p);
                ASN_SEQUENCE_ADD(&bearers_SubjectToStatusTransfer_ListIEs->bearers_SubjectToStatusTransfer_Item, bearersSubjectToStatusTransferItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabtobeswitchedullist(
    E_RABToBeSwitchedULListIEs_t *e_RABToBeSwitchedULListIEs,
    E_RABToBeSwitchedULList_t *e_RABToBeSwitchedULList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABToBeSwitchedULList->list.count; i++) {
        IE_t *ie_p = e_RABToBeSwitchedULList->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABToBeSwitchedULItem:
            {
                E_RABToBeSwitchedULItem_t *erabToBeSwitchedULItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABToBeSwitchedULItem, (void**)&erabToBeSwitchedULItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABToBeSwitchedULItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABToBeSwitchedULItem, erabToBeSwitchedULItem_p);
                ASN_SEQUENCE_ADD(&e_RABToBeSwitchedULListIEs->e_RABToBeSwitchedULItem, erabToBeSwitchedULItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabdataforwardinglist(
    E_RABDataForwardingListIEs_t *e_RABDataForwardingListIEs,
    E_RABDataForwardingList_t *e_RABDataForwardingList) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABDataForwardingList->list.count; i++) {
        IE_t *ie_p = e_RABDataForwardingList->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABDataForwardingItem:
            {
                E_RABDataForwardingItem_t *erabDataForwardingItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABDataForwardingItem, (void**)&erabDataForwardingItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABDataForwardingItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABDataForwardingItem, erabDataForwardingItem_p);
                ASN_SEQUENCE_ADD(&e_RABDataForwardingListIEs->e_RABDataForwardingItem, erabDataForwardingItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabmodifylistbearermodres(
    E_RABModifyListBearerModResIEs_t *e_RABModifyListBearerModResIEs,
    E_RABModifyListBearerModRes_t *e_RABModifyListBearerModRes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABModifyListBearerModRes->list.count; i++) {
        IE_t *ie_p = e_RABModifyListBearerModRes->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABModifyItemBearerModRes:
            {
                E_RABModifyItemBearerModRes_t *erabModifyItemBearerModRes_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABModifyItemBearerModRes, (void**)&erabModifyItemBearerModRes_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABModifyItemBearerModRes failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABModifyItemBearerModRes, erabModifyItemBearerModRes_p);
                ASN_SEQUENCE_ADD(&e_RABModifyListBearerModResIEs->e_RABModifyItemBearerModRes, erabModifyItemBearerModRes_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_ue_associatedlogicals1_connectionlistres(
    UE_associatedLogicalS1_ConnectionListResIEs_t *uE_associatedLogicalS1_ConnectionListResIEs,
    UE_associatedLogicalS1_ConnectionListRes_t *uE_associatedLogicalS1_ConnectionListRes) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < uE_associatedLogicalS1_ConnectionListRes->list.count; i++) {
        IE_t *ie_p = uE_associatedLogicalS1_ConnectionListRes->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_UE_associatedLogicalS1_ConnectionItem:
            {
                UE_associatedLogicalS1_ConnectionItem_t *uEassociatedLogicalS1ConnectionItem_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_UE_associatedLogicalS1_ConnectionItem, (void**)&uEassociatedLogicalS1ConnectionItem_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE uE_associatedLogicalS1_ConnectionItem failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_UE_associatedLogicalS1_ConnectionItem, uEassociatedLogicalS1ConnectionItem_p);
                ASN_SEQUENCE_ADD(&uE_associatedLogicalS1_ConnectionListResIEs->uE_associatedLogicalS1_ConnectionItemRes, uEassociatedLogicalS1ConnectionItem_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

int s1ap_decode_e_rabfailedtosetuplisthoreqack(
    E_RABFailedtoSetupListHOReqAckIEs_t *e_RABFailedtoSetupListHOReqAckIEs,
    E_RABFailedtoSetupListHOReqAck_t *e_RABFailedtoSetupListHOReqAck) {

    int i, decoded = 0;
    int tempDecoded = 0;

    for (i = 0; i < e_RABFailedtoSetupListHOReqAck->list.count; i++) {
        IE_t *ie_p = e_RABFailedtoSetupListHOReqAck->list.array[i];
        switch (ie_p->id) {
            case ProtocolIE_ID_id_E_RABFailedtoSetupItemHOReqAck:
            {
                E_RABFailedToSetupItemHOReqAck_t *erabFailedToSetupItemHOReqAck_p;
                tempDecoded = ANY_to_type_aper(&ie_p->value, &asn_DEF_E_RABFailedToSetupItemHOReqAck, (void**)&erabFailedToSetupItemHOReqAck_p);
                if (tempDecoded < 0) {
                    S1AP_DEBUG("Decoding of IE e_RABFailedtoSetupItemHOReqAck failed");
                    return -1;
                }
                decoded += tempDecoded;
                if (asn1_xer_print)
                    xer_fprint(stdout, &asn_DEF_E_RABFailedToSetupItemHOReqAck, erabFailedToSetupItemHOReqAck_p);
                ASN_SEQUENCE_ADD(&e_RABFailedtoSetupListHOReqAckIEs->e_RABFailedtoSetupItemHOReqAck, erabFailedToSetupItemHOReqAck_p);
            } break;
            default:
                S1AP_DEBUG("Unknown protocol IE id (%d) for message e_rabmodifyrequesties", (int)ie_p->id);
                return -1;
        }
    }
    return decoded;
}

