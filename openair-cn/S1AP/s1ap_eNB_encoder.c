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

/*! \file s1ap_eNB_encoder.c
 * \brief s1ap pdu encode procedures for eNB
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

#include <stdio.h>
#include <string.h>

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_eNB_encoder.h"

#include "assertions.h"
#include "conversions.h"

static inline int s1ap_eNB_encode_initiating(s1ap_message *message,
        uint8_t **buffer,
        uint32_t *len);
static inline int s1ap_eNB_encode_successfull_outcome(s1ap_message *message,
        uint8_t **buffer, uint32_t *len);
static inline int s1ap_eNB_encode_unsuccessfull_outcome(s1ap_message *message,
        uint8_t **buffer, uint32_t *len);
static inline int s1ap_eNB_encode_s1_setup_request(S1SetupRequestIEs_t
        *s1SetupRequestIEs, uint8_t **buffer, uint32_t *length);
static inline int s1ap_eNB_encode_trace_failure(TraceFailureIndicationIEs_t
        *trace_failure_ies_p, uint8_t **buffer,
        uint32_t *length);
static inline int s1ap_eNB_encode_initial_ue_message(InitialUEMessageIEs_t
        *initialUEmessageIEs_p, uint8_t **buffer,
        uint32_t *length);
static inline int s1ap_eNB_encode_uplink_nas_transport(UplinkNASTransportIEs_t
        *uplinkNASTransportIEs,
        uint8_t **buffer,
        uint32_t *length);
static inline int s1ap_eNB_encode_ue_capability_info_indication(
    UECapabilityInfoIndicationIEs_t *ueCapabilityInfoIndicationIEs,
    uint8_t **buffer,
    uint32_t *length);
static inline int s1ap_eNB_encode_initial_context_setup_response(
    InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs,
    uint8_t **buffer,
    uint32_t *length);

int s1ap_eNB_encode_pdu(s1ap_message *message, uint8_t **buffer, uint32_t *len)
{
    DevAssert(message != NULL);
    DevAssert(buffer != NULL);
    DevAssert(len != NULL);

    switch(message->direction) {
        case S1AP_PDU_PR_initiatingMessage:
            return s1ap_eNB_encode_initiating(message, buffer, len);
        case S1AP_PDU_PR_successfulOutcome:
            return s1ap_eNB_encode_successfull_outcome(message, buffer, len);
        case S1AP_PDU_PR_unsuccessfulOutcome:
            return s1ap_eNB_encode_unsuccessfull_outcome(message, buffer, len);
        default:
            S1AP_DEBUG("Unknown message outcome (%d) or not implemented",
                       (int)message->direction);
            break;
    }
    return -1;
}

static inline
int s1ap_eNB_encode_initiating(s1ap_message *message,
        uint8_t **buffer,
        uint32_t *len)
{
    switch(message->procedureCode) {
        case ProcedureCode_id_S1Setup:
            return s1ap_eNB_encode_s1_setup_request(&message->msg.s1SetupRequestIEs,
                                                    buffer, len);
        case ProcedureCode_id_uplinkNASTransport:
            return s1ap_eNB_encode_uplink_nas_transport(&message->msg.uplinkNASTransportIEs,
                    buffer, len);
        case ProcedureCode_id_UECapabilityInfoIndication:
            return s1ap_eNB_encode_ue_capability_info_indication(
                       &message->msg.ueCapabilityInfoIndicationIEs,
                       buffer, len);
        case ProcedureCode_id_initialUEMessage:
            return s1ap_eNB_encode_initial_ue_message(
                &message->msg.initialUEMessageIEs,
                buffer, len);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for initiating message\n",
                       (int)message->procedureCode);
            break;
    }
    return -1;
}

static inline
int s1ap_eNB_encode_successfull_outcome(s1ap_message *message,
    uint8_t **buffer, uint32_t *len)
{
    switch(message->procedureCode) {
        case ProcedureCode_id_InitialContextSetup:
            return s1ap_eNB_encode_initial_context_setup_response(
                       &message->msg.initialContextSetupResponseIEs,
                       buffer, len);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for successfull outcome message\n",
                       (int)message->procedureCode);
            break;
    }
    return -1;
}

static inline
int s1ap_eNB_encode_unsuccessfull_outcome(s1ap_message *message,
    uint8_t **buffer, uint32_t *len)
{
    switch(message->procedureCode) {
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for unsuccessfull outcome message\n",
                       (int)message->procedureCode);
            break;
    }
    return -1;
}

static inline
int s1ap_eNB_encode_ue_capability_info_indication(
    UECapabilityInfoIndicationIEs_t *ueCapabilityInfoIndicationIEs,
    uint8_t             **buffer,
    uint32_t             *length)
{
    UECapabilityInfoIndication_t  ueCapabilityInfoIndication;
    UECapabilityInfoIndication_t *ueCapabilityInfoIndication_p =
        &ueCapabilityInfoIndication;

    memset((void *)ueCapabilityInfoIndication_p, 0,
           sizeof(UECapabilityInfoIndication_t));

    if (s1ap_encode_uecapabilityinfoindicationies(ueCapabilityInfoIndication_p,
            ueCapabilityInfoIndicationIEs) < 0) {
        return -1;
    }

    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            ProcedureCode_id_UECapabilityInfoIndication,
                                            Criticality_ignore,
                                            &asn_DEF_UECapabilityInfoIndication,
                                            ueCapabilityInfoIndication_p);
}

static inline
int s1ap_eNB_encode_uplink_nas_transport(UplinkNASTransportIEs_t
        *uplinkNASTransportIEs,
        uint8_t             **buffer,
        uint32_t             *length)
{
    UplinkNASTransport_t  uplinkNASTransport;
    UplinkNASTransport_t *uplinkNASTransport_p = &uplinkNASTransport;

    memset((void *)uplinkNASTransport_p, 0, sizeof(UplinkNASTransport_t));

    if (s1ap_encode_uplinknastransporties(uplinkNASTransport_p,
                                          uplinkNASTransportIEs) < 0) {
        return -1;
    }

    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            ProcedureCode_id_uplinkNASTransport,
                                            Criticality_ignore,
                                            &asn_DEF_UplinkNASTransport,
                                            uplinkNASTransport_p);
}

static inline
int s1ap_eNB_encode_s1_setup_request(S1SetupRequestIEs_t
        *s1SetupRequestIEs,
        uint8_t             **buffer,
        uint32_t             *length)
{
    S1SetupRequest_t  s1SetupRequest;
    S1SetupRequest_t *s1SetupRequest_p = &s1SetupRequest;

    memset((void *)s1SetupRequest_p, 0, sizeof(S1SetupRequest_t));

    if (s1ap_encode_s1setuprequesties(s1SetupRequest_p, s1SetupRequestIEs) < 0) {
        return -1;
    }

    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            ProcedureCode_id_S1Setup,
                                            Criticality_reject,
                                            &asn_DEF_S1SetupRequest,
                                            s1SetupRequest_p);
}

static inline
int s1ap_eNB_encode_initial_ue_message(InitialUEMessageIEs_t
        *initialUEmessageIEs_p,
        uint8_t             **buffer,
        uint32_t             *length)
{
    InitialUEMessage_t  initialUEMessage;
    InitialUEMessage_t *initialUEMessage_p = &initialUEMessage;

    memset((void *)initialUEMessage_p, 0, sizeof(InitialUEMessage_t));

    if (s1ap_encode_initialuemessageies(initialUEMessage_p,
                                        initialUEmessageIEs_p) < 0) {
        return -1;
    }

    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            ProcedureCode_id_initialUEMessage,
                                            Criticality_reject,
                                            &asn_DEF_InitialUEMessage,
                                            initialUEMessage_p);
}

static inline
int s1ap_eNB_encode_trace_failure(TraceFailureIndicationIEs_t
        *trace_failure_ies_p,
        uint8_t                    **buffer,
        uint32_t                    *length)
{
    TraceFailureIndication_t  trace_failure;
    TraceFailureIndication_t *trace_failure_p = &trace_failure;

    memset((void *)trace_failure_p, 0, sizeof(TraceFailureIndication_t));

    if (s1ap_encode_tracefailureindicationies(trace_failure_p,
            trace_failure_ies_p) < 0) {
        return -1;
    }

    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            ProcedureCode_id_TraceFailureIndication,
                                            Criticality_reject,
                                            &asn_DEF_TraceFailureIndication,
                                            trace_failure_p);
}

static inline
int s1ap_eNB_encode_initial_context_setup_response(
    InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs,
    uint8_t             **buffer,
    uint32_t             *length)
{
    InitialContextSetupResponse_t  initial_context_setup_response;
    InitialContextSetupResponse_t *initial_context_setup_response_p =
        &initial_context_setup_response;

    memset((void *)initial_context_setup_response_p, 0,
           sizeof(InitialContextSetupResponse_t));

    if (s1ap_encode_initialcontextsetupresponseies(initial_context_setup_response_p,
            initialContextSetupResponseIEs) < 0) {
        return -1;
    }

    return s1ap_generate_successfull_outcome(buffer,
            length,
            ProcedureCode_id_InitialContextSetup,
            Criticality_reject,
            &asn_DEF_InitialContextSetupResponse,
            initial_context_setup_response_p);
}
