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

/*! \file s1ap_mme_encoder.c
 * \brief s1ap encode procedures for MME
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2012
 * \version 0.1
 */

#include "intertask_interface.h"

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_encoder.h"
#include "s1ap_mme.h"

#include "assertions.h"

static inline
int s1ap_mme_encode_initial_context_setup_request(s1ap_message *message_p,
    uint8_t **buffer, uint32_t *length);
static inline
int s1ap_mme_encode_s1setupresponse(s1ap_message *message_p,
                                    uint8_t **buffer, uint32_t *length);
static inline
int s1ap_mme_encode_s1setupfailure(s1ap_message *message_p,
                                   uint8_t **buffer, uint32_t *length);
static inline
int s1ap_mme_encode_ue_context_release_command(s1ap_message *message_p,
    uint8_t **buffer, uint32_t *length);
static inline
int s1ap_mme_encode_downlink_nas_transport(
    s1ap_message *message_p, uint8_t **buffer, uint32_t *length);

static inline
int s1ap_mme_encode_initiating(s1ap_message *message_p,
                               uint8_t **buffer,
                               uint32_t *length);
static inline
int s1ap_mme_encode_successfull_outcome(s1ap_message *message_p,
                                        uint8_t **buffer, uint32_t *len);
static inline
int s1ap_mme_encode_unsuccessfull_outcome(s1ap_message *message_p,
                                          uint8_t **buffer, uint32_t *len);

static inline
int s1ap_mme_encode_initial_context_setup_request(s1ap_message *message_p,
    uint8_t **buffer, uint32_t *length)
{
    S1ap_InitialContextSetupRequest_t  initialContextSetupRequest;
    S1ap_InitialContextSetupRequest_t *initialContextSetupRequest_p =
        &initialContextSetupRequest;

    memset(initialContextSetupRequest_p, 0, sizeof(S1ap_InitialContextSetupRequest_t));

    if (s1ap_encode_s1ap_initialcontextsetuprequesties(initialContextSetupRequest_p,
        &message_p->msg.s1ap_InitialContextSetupRequestIEs) < 0) {
        return -1;
    }

    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            S1ap_ProcedureCode_id_InitialContextSetup,
                                            S1ap_Criticality_reject,
                                            &asn_DEF_S1ap_InitialContextSetupRequest,
                                            initialContextSetupRequest_p);
}

int s1ap_mme_encode_pdu(s1ap_message *message_p, uint8_t **buffer, uint32_t *length)
{
    DevAssert(message_p != NULL);
    DevAssert(buffer != NULL);
    DevAssert(length != NULL);

    switch(message_p->direction) {
        case S1AP_PDU_PR_initiatingMessage:
            return s1ap_mme_encode_initiating(message_p, buffer, length);
        case S1AP_PDU_PR_successfulOutcome:
            return s1ap_mme_encode_successfull_outcome(message_p, buffer, length);
        case S1AP_PDU_PR_unsuccessfulOutcome:
            return s1ap_mme_encode_unsuccessfull_outcome(message_p, buffer, length);
        default:
            S1AP_DEBUG("Unknown message outcome (%d) or not implemented",
                       (int)message_p->direction);
            break;
    }
    return -1;
}

static inline
int s1ap_mme_encode_initiating(s1ap_message *message_p,
                               uint8_t **buffer,
                               uint32_t *length)
{
    switch(message_p->procedureCode) {
        case S1ap_ProcedureCode_id_downlinkNASTransport:
            return s1ap_mme_encode_downlink_nas_transport(message_p, buffer, length);
        case S1ap_ProcedureCode_id_InitialContextSetup:
            return s1ap_mme_encode_initial_context_setup_request(message_p, buffer, length);
        case S1ap_ProcedureCode_id_UEContextRelease:
            return s1ap_mme_encode_ue_context_release_command(message_p, buffer, length);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for initiating message_p\n",
                       (int)message_p->procedureCode);
            break;
    }
    return -1;
}

static inline
int s1ap_mme_encode_successfull_outcome(s1ap_message *message_p,
                                        uint8_t **buffer, uint32_t *length)
{
    switch(message_p->procedureCode) {
        case S1ap_ProcedureCode_id_S1Setup:
            return s1ap_mme_encode_s1setupresponse(message_p, buffer, length);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for successfull outcome message_p\n",
                       (int)message_p->procedureCode);
            break;
    }
    return -1;
}

static inline
int s1ap_mme_encode_unsuccessfull_outcome(s1ap_message *message_p,
                                          uint8_t **buffer, uint32_t *length)
{
    switch(message_p->procedureCode) {
        case S1ap_ProcedureCode_id_S1Setup:
            return s1ap_mme_encode_s1setupfailure(message_p, buffer, length);
        default:
            S1AP_DEBUG("Unknown procedure ID (%d) for unsuccessfull outcome message_p\n",
                       (int)message_p->procedureCode);
            break;
    }
    return -1;
}

static inline
int s1ap_mme_encode_s1setupresponse(s1ap_message *message_p,
                                    uint8_t **buffer, uint32_t *length)
{
    S1ap_S1SetupResponse_t  s1SetupResponse;
    S1ap_S1SetupResponse_t *s1SetupResponse_p = &s1SetupResponse;

    memset(s1SetupResponse_p, 0, sizeof(S1ap_S1SetupResponse_t));

    if (s1ap_encode_s1ap_s1setupresponseies(s1SetupResponse_p, &message_p->msg.s1ap_S1SetupResponseIEs) < 0)
    {
        return -1;
    }

    return s1ap_generate_successfull_outcome(buffer,
                                             length,
                                             S1ap_ProcedureCode_id_S1Setup,
                                             message_p->criticality,
                                             &asn_DEF_S1ap_S1SetupResponse,
                                             s1SetupResponse_p);
}

static inline
int s1ap_mme_encode_s1setupfailure(s1ap_message *message_p,
                                   uint8_t **buffer, uint32_t *length)
{
    S1ap_S1SetupFailure_t  s1SetupFailure;
    S1ap_S1SetupFailure_t *s1SetupFailure_p = &s1SetupFailure;

    memset(s1SetupFailure_p, 0, sizeof(S1ap_S1SetupFailure_t));

    if (s1ap_encode_s1ap_s1setupfailureies(s1SetupFailure_p, &message_p->msg.s1ap_S1SetupFailureIEs) < 0) {
        return -1;
    }

    return s1ap_generate_unsuccessfull_outcome(buffer,
                                               length,
                                               S1ap_ProcedureCode_id_S1Setup,
                                               message_p->criticality,
                                               &asn_DEF_S1ap_S1SetupFailure,
                                               s1SetupFailure_p);
}

static inline
int s1ap_mme_encode_downlink_nas_transport(
    s1ap_message *message_p, uint8_t **buffer, uint32_t *length)
{
    S1ap_DownlinkNASTransport_t  downlinkNasTransport;
    S1ap_DownlinkNASTransport_t *downlinkNasTransport_p = &downlinkNasTransport;

    memset(downlinkNasTransport_p, 0, sizeof(S1ap_DownlinkNASTransport_t));

    /* Convert IE structure into asn1 message_p */
    if (s1ap_encode_s1ap_downlinknastransporties(downlinkNasTransport_p,
        &message_p->msg.s1ap_DownlinkNASTransportIEs) < 0) {
        return -1;
    }

    /* Generate Initiating message_p for the list of IEs */
    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            S1ap_ProcedureCode_id_downlinkNASTransport,
                                            message_p->criticality,
                                            &asn_DEF_S1ap_DownlinkNASTransport,
                                            downlinkNasTransport_p);
}

static inline
int s1ap_mme_encode_ue_context_release_command(s1ap_message *message_p,
                                               uint8_t **buffer, uint32_t *length)
{
    S1ap_UEContextReleaseCommand_t  ueContextReleaseCommand;
    S1ap_UEContextReleaseCommand_t *ueContextReleaseCommand_p = &ueContextReleaseCommand;

    memset(ueContextReleaseCommand_p, 0, sizeof(S1ap_UEContextReleaseCommand_t));

    /* Convert IE structure into asn1 message_p */
    if (s1ap_encode_s1ap_uecontextreleasecommandies(ueContextReleaseCommand_p,
        &message_p->msg.s1ap_UEContextReleaseCommandIEs) < 0) {
        return -1;
    }

    return s1ap_generate_initiating_message(buffer,
                                            length,
                                            S1ap_ProcedureCode_id_UEContextRelease,
                                            message_p->criticality,
                                            &asn_DEF_S1ap_UEContextReleaseCommand,
                                            ueContextReleaseCommand_p);
}
