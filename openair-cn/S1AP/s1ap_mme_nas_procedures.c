/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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
        s1ap_mme_itti_nas_establish_ind(ue_ref->mme_ue_s1ap_id,
                                        initialUEMessage_p->nas_pdu.buf,
                                        initialUEMessage_p->nas_pdu.size,
                                        initialUEMessage_p->rrC_Establishment_Cause,
                                        tac);
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
        S1ap_DownlinkNASTransportIEs_t *downlinkNasTransport;

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

        s1ap_mme_itti_send_sctp_request(buffer_p, length,
                                        ue_ref->eNB->sctp_assoc_id,
                                        ue_ref->sctp_stream_send);
        //s1ap_mme_itti_nas_downlink_cnf(ue_ref->mme_ue_s1ap_id, AS_SUCCESS);
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
void s1ap_handle_conn_est_cnf(nas_conn_est_cnf_t *nas_conn_est_cnf_p)
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

    DevAssert(nas_conn_est_cnf_p != NULL);

    initial_p = &nas_conn_est_cnf_p->transparent;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(initial_p->mme_ue_s1ap_id)) == NULL) {
        S1AP_DEBUG("This mme ue s1ap id (%08x) is not attached to any UE context\n",
                   initial_p->mme_ue_s1ap_id);
        DevParam(initial_p->mme_ue_s1ap_id, 0, 0);
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
        DevMessage("Failed to encode initial context setup request message\n");
    }

    s1ap_mme_itti_send_sctp_request(buffer_p, length, ue_ref->eNB->sctp_assoc_id,
                                    ue_ref->sctp_stream_send);
}
#endif
