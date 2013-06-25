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

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_eNB.h"
#include "s1ap_eNB_handlers.h"

//Forward declaration
struct s1ap_message_s;

/* Handlers matrix. Only eNB related procedure present here */
s1ap_message_decoded_callback messages_callback[][3] = {
    { 0, 0, 0 }, /* HandoverPreparation */
    { 0, 0, 0 }, /* HandoverResourceAllocation */
    { 0, 0, 0 }, /* HandoverNotification */
    { 0, 0, 0 }, /* PathSwitchRequest */
    { 0, 0, 0 }, /* HandoverCancel */
    { 0, 0, 0 }, /* E_RABSetup */
    { 0, 0, 0 }, /* E_RABModify */
    { 0, 0, 0 }, /* E_RABRelease */
    { 0, 0, 0 }, /* E_RABReleaseIndication */
    { s1ap_eNB_handle_initial_context_setup,  0, 0 }, /* InitialContextSetup */
    { 0, 0, 0 }, /* Paging */
    { s1ap_eNB_handle_downlink_nas_transport, 0, 0 }, /* downlinkNASTransport */
    { 0, 0, 0 }, /* initialUEMessage */
    { 0, 0, 0 }, /* uplinkNASTransport */
    { 0, 0, 0 }, /* Reset */
    { 0, 0, 0 }, /* ErrorIndication */
    { 0, 0, 0 }, /* NASNonDeliveryIndication */
    { 0, s1ap_eNB_handle_s1_setup_response, s1ap_eNB_handle_s1_setup_failure }, /* S1Setup */
    { 0, 0, 0 }, /* UEContextReleaseRequest */
    { 0, 0, 0 }, /* DownlinkS1cdma2000tunneling */
    { 0, 0, 0 }, /* UplinkS1cdma2000tunneling */
    { 0, 0, 0 }, /* UEContextModification */
    { 0, 0, 0 }, /* UECapabilityInfoIndication*/
    { 0, 0, 0 }, /* UEContextRelease*/
    { 0, 0, 0 }, /* eNBStatusTransfer*/
    { 0, 0, 0 }, /* MMEStatusTransfer*/
    { 0, 0, 0 }, /* DeactivateTrace*/
    { 0, 0, 0 }, /* TraceStart*/
    { 0, 0, 0 }, /* TraceFailureIndication*/
    { 0, 0, 0 }, /* ENBConfigurationUpdate*/
    { 0, 0, 0 }, /* MMEConfigurationUpdate*/
    { 0, 0, 0 }, /* LocationReportingControl*/
    { 0, 0, 0 }, /* LocationReportingFailureIndication*/
    { 0, 0, 0 }, /* LocationReport*/
    { 0, 0, 0 }, /* OverloadStart*/
    { 0, 0, 0 }, /* OverloadStop*/
    { 0, 0, 0 }, /* WriteReplaceWarning*/
    { 0, 0, 0 }, /* eNBDirectInformationTransfer*/
    { 0, 0, 0 }, /* MMEDirectInformationTransfer*/
    { 0, 0, 0 }, /* PrivateMessage*/
    { 0, 0, 0 }, /* eNBConfigurationTransfer*/
    { 0, 0, 0 }, /* MMEConfigurationTransfer*/
    { 0, 0, 0 }, /* CellTrafficTrace */
};

static const char *direction2String[] = {
    "", /* Nothing */
    "Originating message", /* originating message */
    "Successfull outcome", /* successfull outcome */
    "UnSuccessfull outcome", /* successfull outcome */
};

int s1ap_eNB_handle_message(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    /* Checking procedure Code and direction of message */
    if (message->procedureCode > sizeof(messages_callback) / (3 * sizeof(s1ap_message_decoded_callback))
        || (message->direction > S1AP_PDU_PR_unsuccessfulOutcome)) {
        S1AP_ERROR("[SCTP %d] Either procedureCode %d or direction %d exceed expected\n",
              assocId, message->procedureCode, message->direction);
        return -1;
    }
    /* No handler present.
     * This can mean not implemented or no procedure for eNB (wrong direction).
     */
    if (messages_callback[message->procedureCode][message->direction-1] == NULL) {
        S1AP_ERROR("[SCTP %d] No handler for procedureCode %d in %s\n",
                   assocId, message->procedureCode,
                   direction2String[message->direction]);
        return -2;
    }

    /* Calling the right handler */
    return (*messages_callback[message->procedureCode][message->direction-1])(assocId, stream, message);
}

int s1ap_eNB_handle_s1_setup_response(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {

    S1SetupResponseIEs_t *s1SetupResponse_p;
    struct s1ap_eNB_description_s* eNB_ref;

    s1SetupResponse_p = &message->msg.s1SetupResponseIEs;

    /* S1 Setup Response == Non UE-related procedure -> stream 0 */
    if (stream != 0) {
        S1AP_ERROR("[SCTP %d] Received s1 setup response on stream != 0 (%d)\n", assocId, stream);
        return -1;
    }
    if ((eNB_ref = s1ap_get_eNB_assoc_id(assocId)) == NULL) {
        S1AP_ERROR("[SCTP %d] No eNB with this assocId\n", assocId);
        return -1;
    }
    eNB_ref->state = S1AP_ENB_STATE_CONNECTED;
    return 0;
}

int s1ap_eNB_handle_s1_setup_failure(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {

    S1SetupFailureIEs_t *s1SetupFailure_p;
    struct s1ap_eNB_description_s* eNB_ref;

    s1SetupFailure_p = &message->msg.s1SetupFailureIEs;

    /* S1 Setup Response == Non UE-related procedure -> stream 0 */
//     if (stream != 0) {
//         S1AP_ERROR("[SCTP %d] Received s1 setup failure on stream != 0 (%d)\n", assocId, stream);
//         return -1;
//     }
//     if ((eNB_ref = s1ap_get_eNB_assoc_id(assocId)) == NULL) {
//         S1AP_ERROR("[SCTP %d] No eNB with this assocId\n", assocId);
//         return -1;
//     }
//     eNB_ref->state = S1AP_ENB_STATE_CONNECTED;
    return 0;
}

int s1ap_eNB_handle_initial_context_setup(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {

    InitialContextSetupRequestIEs_t *initialContextSetupRequest_p;
    struct s1ap_eNB_UE_description_s* ue_ref;
    int i;

    initialContextSetupRequest_p = &message->msg.initialContextSetupRequestIEs;

    S1AP_DEBUG("[SCTP %d] Received initial context setup request\n", assocId);

    if ((ue_ref = s1ap_get_ue_assoc_id_eNB_ue_s1ap_id(assocId, initialContextSetupRequest_p->eNB_UE_S1AP_ID)) == NULL) {
        S1AP_ERROR("[SCTP %d] No UE context known for eNB UE S1AP ID %d\n",
                   assocId, (int)initialContextSetupRequest_p->eNB_UE_S1AP_ID);
        //TODO: Send failure
        return -1;
    }

    if (stream == 0) {
        S1AP_WARN("[SCTP %d] Received UE related signalling on stream 0\n", assocId);
    }

    /* The recv stream will be checked every time we received a UE signalling message on SCTP interface. */
    ue_ref->stream_recv = stream;
    /* This is the first time we receive the MME UE S1AP ID for the UE, store it and use it as UE identifier over MME */
    ue_ref->mme_UE_s1ap_id = initialContextSetupRequest_p->mme_ue_s1ap_id;

    /* Store aggregate bitrates as defined in TS 36413 #8.3.1.2.
     * Bitrates are expressed in bits/seconds and maximum value is 10Gbits/s
     * so use of 64bits integer is mandatory.
     */
    asn_INTEGER2uint64(&initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL, &ue_ref->maximum_bit_rate_downlink);
    asn_INTEGER2uint64(&initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL, &ue_ref->maximum_bit_rate_uplink);
    /* Store UE security capabilities */
    ue_ref->encryptions_algorithms = *(uint16_t*)(&initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.buf[0]);
    ue_ref->integrity_algorithms =   *(uint16_t*)(&initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.buf[0]);
    /* Store the security key. Will be used for RRC security mode command.
     * Security key = 256bits.
     */
    memcpy((void*)&ue_ref->security_key, (void*)initialContextSetupRequest_p->securityKey.buf, 32);
#if defined(ENB_MODE)
    //TODO: init E-RAB configuration procedure
//     for (i = 0; i < initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq.list.count; i++) {
//         uint8_t *nas_pdu = NULL;
//         uint32_t nas_length = 0;
//         NAS_PDU_t *nas_PDU;
//         E_RABToBeSetupItemBearerSUReqIEs_t e_RABToBeSetupItemBearerSUReqIEs;
// 
//         s1ap_decode_e_rabtobesetupitembearersureqies(&e_RABToBeSetupItemBearerSUReqIEs,
//                                                      initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq.list.array[i]);
// 
//         nas_PDU = &e_RABToBeSetupItemBearerSUReqIEs.e_RABToBeSetupItemBearerSUReq.nAS_PDU;
// 
//         if (nas_PDU != NULL) {
//             nas_pdu = nas_PDU->buf;
//             nas_length = nas_PDU->size;
//         }
//         rrc_eNB_generate_RRCConnectionReconfiguration(ue_ref->eNB->eNB_id, 0, ue_ref->eNB_UE_s1ap_id, nas_pdu, nas_length);
//     }
#endif
    //TODO: forward E-RAB param to RRC
    return 0;
}

int s1ap_eNB_handle_downlink_nas_transport(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {

    DownlinkNASTransportIEs_t *downlinkNASTransport_p;
    struct s1ap_eNB_description_s* eNB_ref;

    downlinkNASTransport_p = &message->msg.downlinkNASTransportIEs;

    S1AP_DEBUG("[SCTP %d] Received downlink NAS transport\n", assocId);

    if (stream == 0) {
        S1AP_WARN("[SCTP %d] Received UE related signalling on stream 0\n", assocId);
    }

    //Forward to RRC
    return 0;
}
