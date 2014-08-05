/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/

#include <stdio.h>
#include <stdint.h>

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_encoder.h"
#include "s1ap_mme_handlers.h"
#include "s1ap_mme_nas_procedures.h"
#include "s1ap_mme_retransmission.h"

#include "s1ap_mme_itti_messaging.h"

#include "s1ap_mme.h"

#include "intertask_interface.h"
#include "timer.h"

#include "assertions.h"
#include "conversions.h"

/* Every time a new UE is associated, increment this variable.
 * But care if it wraps to increment also the mme_ue_s1ap_id_has_wrapped
 * variable. Limit: UINT32_MAX (in stdint.h).
 */
static uint32_t mme_ue_s1ap_id = 0;
static uint8_t  mme_ue_s1ap_id_has_wrapped = 0;



int s1ap_mme_handle_initial_ue_message(uint32_t assoc_id, uint32_t stream,
                                       struct s1ap_message_s *message)
{
    S1ap_InitialUEMessageIEs_t *initialUEMessage_p;
    ue_description_t      *ue_ref;
    eNB_description_t     *eNB_ref;
    uint32_t               eNB_ue_s1ap_id;

    initialUEMessage_p = &message->msg.s1ap_InitialUEMessageIEs;

    if ((eNB_ref = s1ap_is_eNB_assoc_id_in_list(assoc_id)) == NULL) {
        S1AP_DEBUG("Unkwnon eNB on assoc_id %d\n", assoc_id);
        return -1;
    }

    // eNB UE S1AP ID is limited to 24 bits
    eNB_ue_s1ap_id = (uint32_t)(initialUEMessage_p->eNB_UE_S1AP_ID & 0x00ffffff);

    S1AP_DEBUG("New Initial UE message received with eNB UE S1AP ID: 0x%06x\n",
               eNB_ue_s1ap_id);

    ue_ref = s1ap_is_ue_eNB_id_in_list(eNB_ref, eNB_ue_s1ap_id);
    if (ue_ref == NULL)
    {
        uint16_t tac = 0;

        /* This UE eNB Id has currently no known s1 association.
         * Create new UE context by associating new mme_ue_s1ap_id.
         * Update eNB UE list.
         * Forward message to NAS.
         */
        if ((ue_ref = s1ap_new_ue(assoc_id)) == NULL)
            // If we failed to allocate a new UE return -1
        {
            return -1;
        }

        ue_ref->s1_ue_state = S1AP_UE_WAITING_CSR;
        ue_ref->eNB_ue_s1ap_id = eNB_ue_s1ap_id;
        ue_ref->mme_ue_s1ap_id = (uint32_t)ue_ref;
        // On which stream we received the message
        ue_ref->sctp_stream_recv = stream;
        ue_ref->sctp_stream_send = ue_ref->eNB->next_sctp_stream;

        /* Increment the sctp stream for the eNB association.
         * If the next sctp stream is >= outstream negociated between eNB and MME,
         * wrap to first stream.
         * TODO: search for the first available stream instead.
         */
        if (ue_ref->eNB->next_sctp_stream++ >= ue_ref->eNB->outstreams) {
            ue_ref->eNB->next_sctp_stream = 1;
        }

        /* Increment the unique UE mme s1ap id and
         * take care about overflow case.
         */
        if (mme_ue_s1ap_id_has_wrapped == 0) {
            mme_ue_s1ap_id++;
            if (mme_ue_s1ap_id == 0) {
                mme_ue_s1ap_id_has_wrapped = 1;
            }
        } else {
            /* TODO: should take the first available mme_ue_s1ap_id instead of
             * the mme_ue_s1ap_id variable.
             */
            DevMessage("mme ue s1ap id has wrapped -> case not handled\n");
        }

        s1ap_dump_eNB(ue_ref->eNB);

        OCTET_STRING_TO_TAC(&initialUEMessage_p->tai.tAC, tac);

        /* We received the first NAS transport message: initial UE message.
         * Send a NAS ESTABLISH IND to NAS layer
         */
#if defined(ORIGINAL_CODE)
        s1ap_mme_itti_nas_establish_ind(
                ue_ref->mme_ue_s1ap_id,
                initialUEMessage_p->nas_pdu.buf,
                initialUEMessage_p->nas_pdu.size,
                initialUEMessage_p->rrC_Establishment_Cause,
                tac);
#else
        s1ap_mme_itti_mme_app_establish_ind(
                ue_ref->mme_ue_s1ap_id,
                initialUEMessage_p->nas_pdu.buf,
                initialUEMessage_p->nas_pdu.size,
                initialUEMessage_p->rrC_Establishment_Cause,
                tac);
#endif
    }
    return 0;
}



int s1ap_mme_handle_uplink_nas_transport(uint32_t assoc_id, uint32_t stream,
        struct s1ap_message_s *message)
{
    S1ap_UplinkNASTransportIEs_t *uplinkNASTransport_p;
    ue_description_t *ue_ref;

    uplinkNASTransport_p = &message->msg.s1ap_UplinkNASTransportIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(uplinkNASTransport_p->mme_ue_s1ap_id))
            == NULL) {
        S1AP_DEBUG("No UE is attached to this mme UE s1ap id: %d\n",
                   (int)uplinkNASTransport_p->mme_ue_s1ap_id);
        return -1;
    }
    if (ue_ref->s1_ue_state != S1AP_UE_CONNECTED) {
        S1AP_DEBUG("Received uplink NAS transport while UE in state != S1AP_UE_CONNECTED\n");
        return -1;
    }

    //TODO: forward NAS PDU to NAS
#if defined(DISABLE_USE_NAS)
    DevMessage("Can't go further (TODO)\n");
#else
    s1ap_mme_itti_nas_uplink_ind(uplinkNASTransport_p->mme_ue_s1ap_id, uplinkNASTransport_p->nas_pdu.buf,
                                 uplinkNASTransport_p->nas_pdu.size);
#endif
    return 0;
}



int s1ap_mme_handle_nas_non_delivery(uint32_t assoc_id, uint32_t stream,
                                     struct s1ap_message_s *message)
{
    /* UE associated signalling on stream == 0 is not valid. */
    if (stream == 0) {
        S1AP_DEBUG("Received new nas non delivery on stream == 0\n");
        return -1;
    }
    //TODO: forward NAS PDU to NAS
    DevMessage("TODO: forward NAS PDU to NAS\n");
    return 0;
}

int s1ap_generate_downlink_nas_transport(const uint32_t ue_id, void * const data,
                                         const uint32_t size)
{
    ue_description_t *ue_ref;
    uint8_t          *buffer_p;
    uint32_t          length;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(ue_id)) == NULL) {
        /* If the UE-associated logical S1-connection is not established,
         * the MME shall allocate a unique MME UE S1AP ID to be used for the UE.
         */
        DevMessage("This case is not handled right now\n");

        return -1;
    } else {
        /* We have fount the UE in the list.
         * Create new IE list message and encode it.
         */
        S1ap_DownlinkNASTransportIEs_t *downlinkNasTransport = NULL;

        s1ap_message message;

        memset(&message, 0, sizeof(s1ap_message));

        message.procedureCode = S1ap_ProcedureCode_id_downlinkNASTransport;
        message.direction     = S1AP_PDU_PR_initiatingMessage;

        ue_ref->s1_ue_state = S1AP_UE_CONNECTED;

        downlinkNasTransport = &message.msg.s1ap_DownlinkNASTransportIEs;

        /* Setting UE informations with the ones fount in ue_ref */
        downlinkNasTransport->mme_ue_s1ap_id = ue_ref->mme_ue_s1ap_id;
        downlinkNasTransport->eNB_UE_S1AP_ID = ue_ref->eNB_ue_s1ap_id;

        /* Fill in the NAS pdu */
        OCTET_STRING_fromBuf(&downlinkNasTransport->nas_pdu, (char*)data, size);

        if (s1ap_mme_encode_pdu(&message, &buffer_p, &length) < 0) {
            // TODO: handle something
            return -1;
        }
        S1AP_DEBUG("Send S1ap_ProcedureCode_id_downlinkNASTransport ue_id = 0x%08X mme_ue_s1ap_id = 0x%08X eNB_UE_S1AP_ID = 0x%08X\n",
                ue_id,
                downlinkNasTransport->mme_ue_s1ap_id,
                downlinkNasTransport->eNB_UE_S1AP_ID);

        s1ap_mme_itti_send_sctp_request(buffer_p, length,
                                        ue_ref->eNB->sctp_assoc_id,
                                        ue_ref->sctp_stream_send);
    }
    return 0;
}

#if defined(DISABLE_USE_NAS)
int s1ap_handle_attach_accepted(nas_attach_accept_t *attach_accept_p)
{
    /* We received create session response from S-GW on S11 interface abstraction.
     * At least one bearer has been established. We can now send s1ap initial context setup request
     * message to eNB.
     */
    uint8_t supportedAlgorithms[] = { 0x00, 0x02 };
    uint8_t offset = 0;
    uint8_t *buffer_p;
    uint32_t length;

    ue_description_t *ue_ref = NULL;
    s1ap_message message;
    s1ap_initial_ctxt_setup_req_t *initial_p;

    S1ap_InitialContextSetupRequestIEs_t *initialContextSetupRequest_p;
    S1ap_E_RABToBeSetupItemCtxtSUReq_t    e_RABToBeSetup;

    DevAssert(attach_accept_p != NULL);

    initial_p = &attach_accept_p->transparent;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(initial_p->mme_ue_s1ap_id)) == NULL) {
        S1AP_DEBUG("This mme ue s1ap id (%08x) is not attached to any UE context\n",
                   initial_p->mme_ue_s1ap_id);
        return -1;
    }

    /* Start the outcome response timer.
     * When time is reached, MME consider that procedure outcome has failed.
     */
//     timer_setup(mme_config.s1ap_config.outcome_drop_timer_sec, 0, TASK_S1AP, INSTANCE_DEFAULT,
//                 TIMER_ONE_SHOT,
//                 NULL,
//                 &ue_ref->outcome_response_timer_id);
    /* Insert the timer in the MAP of mme_ue_s1ap_id <-> timer_id */
//     s1ap_timer_insert(ue_ref->mme_ue_s1ap_id, ue_ref->outcome_response_timer_id);

    memset(&message, 0, sizeof(s1ap_message));
    memset(&e_RABToBeSetup, 0, sizeof(S1ap_E_RABToBeSetupItemCtxtSUReq_t));

    message.procedureCode = S1ap_ProcedureCode_id_InitialContextSetup;
    message.direction     = S1AP_PDU_PR_initiatingMessage;

    initialContextSetupRequest_p = &message.msg.s1ap_InitialContextSetupRequestIEs;

    initialContextSetupRequest_p->mme_ue_s1ap_id = (unsigned long)ue_ref->mme_ue_s1ap_id;
    initialContextSetupRequest_p->eNB_UE_S1AP_ID = (unsigned long)ue_ref->eNB_ue_s1ap_id;

    /* uEaggregateMaximumBitrateDL and uEaggregateMaximumBitrateUL expressed in term of bits/sec */
    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL = initial_p->ambr.br_dl;
    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL = initial_p->ambr.br_ul;

    e_RABToBeSetup.e_RAB_ID = initial_p->ebi;
    e_RABToBeSetup.e_RABlevelQoSParameters.qCI = initial_p->qci;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel
    = initial_p->prio_level; //No priority
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability
    = initial_p->pre_emp_capability;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability
    = initial_p->pre_emp_vulnerability;

    /* Set the GTP-TEID. This is the S1-U S-GW TEID */
    INT32_TO_OCTET_STRING(initial_p->teid, &e_RABToBeSetup.gTP_TEID);

    /* S-GW IP address(es) for user-plane */
    if ((initial_p->s_gw_address.pdn_type == IPv4) ||
        (initial_p->s_gw_address.pdn_type == IPv4_AND_v6))
    {
        e_RABToBeSetup.transportLayerAddress.buf = calloc(4, sizeof(uint8_t));
        /* Only IPv4 supported */
        memcpy(e_RABToBeSetup.transportLayerAddress.buf,
               initial_p->s_gw_address.address.ipv4_address,
               4);
        offset += 4;
        e_RABToBeSetup.transportLayerAddress.size = 4;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
    }
    if ((initial_p->s_gw_address.pdn_type == IPv6) ||
        (initial_p->s_gw_address.pdn_type == IPv4_AND_v6))
    {
        if (offset == 0) {
            /* Both IPv4 and IPv6 provided */
            /* TODO: check memory allocation */
            e_RABToBeSetup.transportLayerAddress.buf = calloc(16, sizeof(uint8_t));
        } else {
            /* Only IPv6 supported */
            /* TODO: check memory allocation */
            e_RABToBeSetup.transportLayerAddress.buf
            = realloc(e_RABToBeSetup.transportLayerAddress.buf, (16 + offset) * sizeof(uint8_t));
        }
        memcpy(&e_RABToBeSetup.transportLayerAddress.buf[offset],
               initial_p->s_gw_address.address.ipv6_address,
               16);
        e_RABToBeSetup.transportLayerAddress.size = 16 + offset;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
    }

    ASN_SEQUENCE_ADD(&initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq,
                     &e_RABToBeSetup);

    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.buf =
        (uint8_t *)supportedAlgorithms;
    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.size = 2;
    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.bits_unused
        = 0;

    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.buf
        = (uint8_t *)supportedAlgorithms;
    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.size
        = 2;
    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.bits_unused
        = 0;

    initialContextSetupRequest_p->securityKey.buf  = initial_p->keNB; /* 256 bits length */
    initialContextSetupRequest_p->securityKey.size = 32;
    initialContextSetupRequest_p->securityKey.bits_unused = 0;

    if (s1ap_mme_encode_pdu(&message, &buffer_p, &length) < 0) {
        // TODO: handle something
        return -1;
    }

    return s1ap_mme_itti_send_sctp_request(buffer_p, length, ue_ref->eNB->sctp_assoc_id,
                                           ue_ref->sctp_stream_send);
}
#else
//void s1ap_handle_conn_est_cnf(nas_conn_est_cnf_t *nas_conn_est_cnf_p)
//{
//    /* We received create session response from S-GW on S11 interface abstraction.
//     * At least one bearer has been established. We can now send s1ap initial context setup request
//     * message to eNB.
//     */
//    uint8_t supportedAlgorithms[] = { 0x00, 0x00 };
//    uint8_t offset = 0;
//    uint8_t *buffer_p;
//    uint32_t length, teid = 12;
//
//    ue_description_t *ue_ref = NULL;
//    s1ap_message message;
//
//    S1ap_InitialContextSetupRequestIEs_t *initialContextSetupRequest_p;
//    S1ap_E_RABToBeSetupItemCtxtSUReq_t    e_RABToBeSetup;
//    S1ap_NAS_PDU_t                        nas_pdu;
//
//    DevAssert(nas_conn_est_cnf_p != NULL);
//
//    if ((ue_ref = s1ap_is_ue_mme_id_in_list(nas_conn_est_cnf_p->UEid)) == NULL) {
//        S1AP_DEBUG("This mme ue s1ap id (%08x) is not attached to any UE context\n",
//                nas_conn_est_cnf_p->UEid);
//        DevParam(nas_conn_est_cnf_p->UEid, 0, 0);
//    }
//
//    /* Start the outcome response timer.
//     * When time is reached, MME consider that procedure outcome has failed.
//     */
////     timer_setup(mme_config.s1ap_config.outcome_drop_timer_sec, 0, TASK_S1AP, INSTANCE_DEFAULT,
////                 TIMER_ONE_SHOT,
////                 NULL,
////                 &ue_ref->outcome_response_timer_id);
//    /* Insert the timer in the MAP of mme_ue_s1ap_id <-> timer_id */
////     s1ap_timer_insert(ue_ref->mme_ue_s1ap_id, ue_ref->outcome_response_timer_id);
//
//    memset(&message, 0, sizeof(s1ap_message));
//    memset(&e_RABToBeSetup, 0, sizeof(S1ap_E_RABToBeSetupItemCtxtSUReq_t));
//
//    message.procedureCode = S1ap_ProcedureCode_id_InitialContextSetup;
//    message.direction     = S1AP_PDU_PR_initiatingMessage;
//
//    initialContextSetupRequest_p = &message.msg.s1ap_InitialContextSetupRequestIEs;
//
//    initialContextSetupRequest_p->mme_ue_s1ap_id = (unsigned long)ue_ref->mme_ue_s1ap_id;
//    initialContextSetupRequest_p->eNB_UE_S1AP_ID = (unsigned long)ue_ref->eNB_ue_s1ap_id;
//
//    /* uEaggregateMaximumBitrateDL and uEaggregateMaximumBitrateUL expressed in term of bits/sec */
////    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL = initial_p->ambr.br_dl;
////    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL = initial_p->ambr.br_ul;
//
//    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL = 1024 * 1024;
//    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL = 512 * 1024;
//
////    e_RABToBeSetup.e_RAB_ID = initial_p->ebi;
//    e_RABToBeSetup.e_RAB_ID = 5;
////    e_RABToBeSetup.e_RABlevelQoSParameters.qCI = initial_p->qci;
//    e_RABToBeSetup.e_RABlevelQoSParameters.qCI = 0;
//
//    memset(&nas_pdu, 0, sizeof(nas_pdu));
//
//    nas_pdu.size = nas_conn_est_cnf_p->nasMsg.length;
//    nas_pdu.buf  = nas_conn_est_cnf_p->nasMsg.data;
//
//    e_RABToBeSetup.nAS_PDU = &nas_pdu;
//    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel =
//            S1ap_PriorityLevel_lowest;
//    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability =
//            S1ap_Pre_emptionCapability_shall_not_trigger_pre_emption;
//    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability =
//            S1ap_Pre_emptionVulnerability_not_pre_emptable;
//    INT32_TO_OCTET_STRING(teid, &e_RABToBeSetup.gTP_TEID);
////    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel
////    = initial_p->prio_level; //No priority
////    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability
////    = initial_p->pre_emp_capability;
////    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability
////    = initial_p->pre_emp_vulnerability;
//
//    /* Set the GTP-TEID. This is the S1-U S-GW TEID */
////    INT32_TO_OCTET_STRING(initial_p->teid, &e_RABToBeSetup.gTP_TEID);
//
//    /* S-GW IP address(es) for user-plane */
////    if ((initial_p->s_gw_address.pdn_type == IPv4) ||
////        (initial_p->s_gw_address.pdn_type == IPv4_AND_v6))
////    {
////        e_RABToBeSetup.transportLayerAddress.buf = calloc(4, sizeof(uint8_t));
////        /* Only IPv4 supported */
////        memcpy(e_RABToBeSetup.transportLayerAddress.buf,
////               initial_p->s_gw_address.address.ipv4_address,
////               4);
////        offset += 4;
////        e_RABToBeSetup.transportLayerAddress.size = 4;
////        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
////    }
////    if ((initial_p->s_gw_address.pdn_type == IPv6) ||
////        (initial_p->s_gw_address.pdn_type == IPv4_AND_v6))
////    {
////        if (offset == 0) {
////            /* Both IPv4 and IPv6 provided */
////            /* TODO: check memory allocation */
////            e_RABToBeSetup.transportLayerAddress.buf = calloc(16, sizeof(uint8_t));
////        } else {
////            /* Only IPv6 supported */
////            /* TODO: check memory allocation */
////            e_RABToBeSetup.transportLayerAddress.buf
////            = realloc(e_RABToBeSetup.transportLayerAddress.buf, (16 + offset) * sizeof(uint8_t));
////        }
////        memcpy(&e_RABToBeSetup.transportLayerAddress.buf[offset],
////               initial_p->s_gw_address.address.ipv6_address,
////               16);
////        e_RABToBeSetup.transportLayerAddress.size = 16 + offset;
////        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
////    }
//
//    ASN_SEQUENCE_ADD(&initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq,
//                     &e_RABToBeSetup);
//
//    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.buf =
//        (uint8_t *)supportedAlgorithms;
//    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.size = 2;
//    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.bits_unused
//        = 0;
//
//    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.buf
//        = (uint8_t *)supportedAlgorithms;
//    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.size
//        = 2;
//    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.bits_unused
//        = 0;
//
////    initialContextSetupRequest_p->securityKey.buf  = initial_p->keNB; /* 256 bits length */
//    uint8_t keNB[32];
//    memset(keNB, 0, sizeof(keNB));
//
//    initialContextSetupRequest_p->securityKey.buf = keNB;
//    initialContextSetupRequest_p->securityKey.size = 32;
//    initialContextSetupRequest_p->securityKey.bits_unused = 0;
//
//    if (s1ap_mme_encode_pdu(&message, &buffer_p, &length) < 0) {
//        // TODO: handle something
//        DevMessage("Failed to encode initial context setup request message\n");
//    }
//
//    free(nas_conn_est_cnf_p->nasMsg.data);
//
//    s1ap_mme_itti_send_sctp_request(buffer_p, length, ue_ref->eNB->sctp_assoc_id,
//                                    ue_ref->sctp_stream_send);
//}

void s1ap_handle_conn_est_cnf(const mme_app_connection_establishment_cnf_t * const conn_est_cnf_pP)
{
    /* We received create session response from S-GW on S11 interface abstraction.
     * At least one bearer has been established. We can now send s1ap initial context setup request
     * message to eNB.
     */
    uint8_t                               offset                = 0;
    uint8_t                              *buffer_p              = NULL;
    uint32_t                              length                = 0;
    ue_description_t                     *ue_ref                = NULL;
    S1ap_InitialContextSetupRequestIEs_t *initialContextSetupRequest_p = NULL;
    S1ap_E_RABToBeSetupItemCtxtSUReq_t    e_RABToBeSetup;
    S1ap_NAS_PDU_t                        nas_pdu;
    s1ap_message                          message;

    DevAssert(conn_est_cnf_pP != NULL);

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(conn_est_cnf_pP->nas_conn_est_cnf.UEid)) == NULL) {
        S1AP_DEBUG("This mme ue s1ap id (%08x) is not attached to any UE context\n",
                conn_est_cnf_pP->nas_conn_est_cnf.UEid);

        DevParam(conn_est_cnf_pP->nas_conn_est_cnf.UEid, 0, 0);
    }

    /* Start the outcome response timer.
     * When time is reached, MME consider that procedure outcome has failed.
     */
//     timer_setup(mme_config.s1ap_config.outcome_drop_timer_sec, 0, TASK_S1AP, INSTANCE_DEFAULT,
//                 TIMER_ONE_SHOT,
//                 NULL,
//                 &ue_ref->outcome_response_timer_id);
    /* Insert the timer in the MAP of mme_ue_s1ap_id <-> timer_id */
//     s1ap_timer_insert(ue_ref->mme_ue_s1ap_id, ue_ref->outcome_response_timer_id);

    memset(&message, 0, sizeof(s1ap_message));
    memset(&e_RABToBeSetup, 0, sizeof(S1ap_E_RABToBeSetupItemCtxtSUReq_t));

    message.procedureCode = S1ap_ProcedureCode_id_InitialContextSetup;
    message.direction     = S1AP_PDU_PR_initiatingMessage;

    initialContextSetupRequest_p = &message.msg.s1ap_InitialContextSetupRequestIEs;

    initialContextSetupRequest_p->mme_ue_s1ap_id = (unsigned long)ue_ref->mme_ue_s1ap_id;
    initialContextSetupRequest_p->eNB_UE_S1AP_ID = (unsigned long)ue_ref->eNB_ue_s1ap_id;

    /* uEaggregateMaximumBitrateDL and uEaggregateMaximumBitrateUL expressed in term of bits/sec */
    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL = conn_est_cnf_pP->ambr.br_dl;
    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL = conn_est_cnf_pP->ambr.br_ul;

    e_RABToBeSetup.e_RAB_ID                    = conn_est_cnf_pP->eps_bearer_id; //5;
    e_RABToBeSetup.e_RABlevelQoSParameters.qCI = conn_est_cnf_pP->bearer_qos_qci;


    memset(&nas_pdu, 0, sizeof(nas_pdu));
    nas_pdu.size = conn_est_cnf_pP->nas_conn_est_cnf.nasMsg.length;
    nas_pdu.buf  = conn_est_cnf_pP->nas_conn_est_cnf.nasMsg.data;
    e_RABToBeSetup.nAS_PDU = &nas_pdu;

#if defined(ORIGINAL_S1AP_CODE)
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel =
            S1ap_PriorityLevel_lowest;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability =
            S1ap_Pre_emptionCapability_shall_not_trigger_pre_emption;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability =
            S1ap_Pre_emptionVulnerability_not_pre_emptable;
#else
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel =
            conn_est_cnf_pP->bearer_qos_prio_level;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability =
            conn_est_cnf_pP->bearer_qos_pre_emp_capability;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability =
            conn_est_cnf_pP->bearer_qos_pre_emp_vulnerability;
#endif
    /* Set the GTP-TEID. This is the S1-U S-GW TEID */
    INT32_TO_OCTET_STRING(conn_est_cnf_pP->bearer_s1u_sgw_fteid.teid, &e_RABToBeSetup.gTP_TEID);

    /* S-GW IP address(es) for user-plane */
    if (conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv4)
    {
        e_RABToBeSetup.transportLayerAddress.buf = calloc(4, sizeof(uint8_t));
        /* Only IPv4 supported */
        memcpy(e_RABToBeSetup.transportLayerAddress.buf,
               &conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv4_address,
               4);
        offset += 4;
        e_RABToBeSetup.transportLayerAddress.size = 4;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
    }
    if (conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv6)
    {
        if (offset == 0) {
            /* Both IPv4 and IPv6 provided */
            /* TODO: check memory allocation */
            e_RABToBeSetup.transportLayerAddress.buf = calloc(16, sizeof(uint8_t));
        } else {
            /* Only IPv6 supported */
            /* TODO: check memory allocation */
            e_RABToBeSetup.transportLayerAddress.buf
            = realloc(e_RABToBeSetup.transportLayerAddress.buf, (16 + offset) * sizeof(uint8_t));
        }
        memcpy(&e_RABToBeSetup.transportLayerAddress.buf[offset],
               conn_est_cnf_pP->bearer_s1u_sgw_fteid.ipv6_address,
               16);
        e_RABToBeSetup.transportLayerAddress.size = 16 + offset;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
    }

    ASN_SEQUENCE_ADD(&initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq,
                     &e_RABToBeSetup);

    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.buf =
            (uint8_t *)&conn_est_cnf_pP->security_capabilities_encryption_algorithms;
    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.size = 2;
    initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms.bits_unused
        = 0;

    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.buf =
            (uint8_t *)&conn_est_cnf_pP->security_capabilities_integrity_algorithms;
    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.size
        = 2;
    initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms.bits_unused
        = 0;

    S1AP_DEBUG("security_capabilities_encryption_algorithms 0x%04X\n",
            conn_est_cnf_pP->security_capabilities_encryption_algorithms);
    S1AP_DEBUG("security_capabilities_integrity_algorithms 0x%04X\n",
            conn_est_cnf_pP->security_capabilities_integrity_algorithms);

    if (conn_est_cnf_pP->keNB) {
        initialContextSetupRequest_p->securityKey.buf = malloc(32);
        memcpy(initialContextSetupRequest_p->securityKey.buf,
            conn_est_cnf_pP->keNB,
            32);

        initialContextSetupRequest_p->securityKey.size = 32;
    } else {
        S1AP_DEBUG("No keNB\n");
        initialContextSetupRequest_p->securityKey.buf = NULL;
        initialContextSetupRequest_p->securityKey.size = 0;
    }
    initialContextSetupRequest_p->securityKey.bits_unused = 0;

    if (s1ap_mme_encode_pdu(&message, &buffer_p, &length) < 0) {
        // TODO: handle something
        DevMessage("Failed to encode initial context setup request message\n");
    }

    free(conn_est_cnf_pP->nas_conn_est_cnf.nasMsg.data);

    s1ap_mme_itti_send_sctp_request(
            buffer_p,
            length,
            ue_ref->eNB->sctp_assoc_id,
            ue_ref->sctp_stream_send);
}

#endif
