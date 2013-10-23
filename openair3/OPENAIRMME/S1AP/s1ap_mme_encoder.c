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

int s1ap_mme_encode_s1setupresponse(S1SetupResponseIEs_t *s1SetupResponseIEs, uint32_t assocId) {
    S1SetupResponse_t  s1SetupResponse;
    S1SetupResponse_t *s1SetupResponse_p = &s1SetupResponse;
    MessageDef *message_p;

    memset(s1SetupResponse_p, 0, sizeof(S1SetupResponse_t));

    if (s1ap_encode_s1setupresponseies(s1SetupResponse_p, s1SetupResponseIEs) < 0) {
        return -1;
    }

    message_p = (MessageDef *)malloc(sizeof(MessageDef));

    message_p->messageId = SCTP_NEW_DATA_REQ;
    message_p->originTaskId = TASK_S1AP;
    message_p->destinationTaskId = TASK_SCTP;
    /* Signalling non UE associated -> stream 0 */
    message_p->msg.sctpNewDataReq.stream = 0;
    message_p->msg.sctpNewDataReq.assocId = assocId;

    if (s1ap_generate_successfull_outcome(&message_p->msg.sctpNewDataReq.buffer,
                                          &message_p->msg.sctpNewDataReq.bufLen,
                                          ProcedureCode_id_S1Setup,
                                          Criticality_reject,
                                          &asn_DEF_S1SetupResponse,
                                          s1SetupResponse_p) < 0) {
        free(message_p);
        return -1;
    }

    return itti_send_msg_to_task(TASK_SCTP, message_p);
}

int s1ap_mme_encode_s1setupfailure(S1SetupFailureIEs_t *s1SetupFailureIEs, uint32_t assocId) {
    S1SetupFailure_t  s1SetupFailure;
    S1SetupFailure_t *s1SetupFailure_p = &s1SetupFailure;
    MessageDef *message_p;

    memset(s1SetupFailure_p, 0, sizeof(S1SetupFailure_t));

    if (s1ap_encode_s1setupfailureies(s1SetupFailure_p, s1SetupFailureIEs) < 0) {
        return -1;
    }

    message_p = (MessageDef *)malloc(sizeof(MessageDef));

    message_p->messageId = SCTP_NEW_DATA_REQ;
    message_p->originTaskId = TASK_S1AP;
    message_p->destinationTaskId = TASK_SCTP;
    /* Signalling non UE associated -> stream 0 */
    message_p->msg.sctpNewDataReq.stream = 0;
    message_p->msg.sctpNewDataReq.assocId = assocId;

    if (s1ap_generate_unsuccessfull_outcome(&message_p->msg.sctpNewDataReq.buffer,
                                            &message_p->msg.sctpNewDataReq.bufLen,
                                            ProcedureCode_id_S1Setup,
                                            Criticality_reject,
                                            &asn_DEF_S1SetupFailure,
                                            s1SetupFailure_p) < 0) {
        free(message_p);
        return -1;
    }

    return itti_send_msg_to_task(TASK_SCTP, message_p);
}

int s1ap_mme_encode_initial_context_setup_request(InitialContextSetupRequestIEs_t *initialContextSetupRequestIEs,
                                                  ue_description_t              *ue_ref) {
    InitialContextSetupRequest_t  initialContextSetupRequest;
    InitialContextSetupRequest_t *initialContextSetupRequest_p = &initialContextSetupRequest;
    MessageDef                   *message_p;

    memset(initialContextSetupRequest_p, 0, sizeof(InitialContextSetupRequest_t));

    if (s1ap_encode_initialcontextsetuprequesties(initialContextSetupRequest_p, initialContextSetupRequestIEs) < 0) {
        return -1;
    }

    message_p = (MessageDef *)malloc(sizeof(MessageDef));

    message_p->messageId = SCTP_NEW_DATA_REQ;
    message_p->originTaskId = TASK_S1AP;
    message_p->destinationTaskId = TASK_SCTP;
    /* Signalling non UE associated -> stream 0 */
    message_p->msg.sctpNewDataReq.stream = ue_ref->sctp_stream_send;
    message_p->msg.sctpNewDataReq.assocId = ue_ref->eNB->sctp_assoc_id;

    if (s1ap_generate_initiating_message(&message_p->msg.sctpNewDataReq.buffer,
                                         &message_p->msg.sctpNewDataReq.bufLen,
                                         ProcedureCode_id_InitialContextSetup,
                                         Criticality_reject,
                                         &asn_DEF_InitialContextSetupRequest,
                                         initialContextSetupRequest_p) < 0) {
        free(message_p);
        return -1;
    }

    return itti_send_msg_to_task(TASK_SCTP, message_p);
}

int s1ap_mme_encode_downlink_nas_transport(DownlinkNASTransportIEs_t *downlinkNasTransportIEs,
                                           ue_description_t        *ue_ref) {
    DownlinkNASTransport_t  downlinkNasTransport;
    DownlinkNASTransport_t *downlinkNasTransport_p = &downlinkNasTransport;
    MessageDef             *message_p;

    memset(downlinkNasTransport_p, 0, sizeof(DownlinkNASTransport_t));

    /* Convert IE structure into asn1 message */
    if (s1ap_encode_downlinknastransporties(downlinkNasTransport_p, downlinkNasTransportIEs) < 0) {
        return -1;
    }

    message_p = (MessageDef *)malloc(sizeof(MessageDef));

    message_p->messageId = SCTP_NEW_DATA_REQ;
    message_p->originTaskId = TASK_S1AP;
    message_p->destinationTaskId = TASK_SCTP;
    message_p->msg.sctpNewDataReq.stream = ue_ref->sctp_stream_send;
    message_p->msg.sctpNewDataReq.assocId = ue_ref->eNB->sctp_assoc_id;

    /* Generate Initiating message for the list of IEs */
    if (s1ap_generate_initiating_message(&message_p->msg.sctpNewDataReq.buffer,
                                         &message_p->msg.sctpNewDataReq.bufLen,
                                         ProcedureCode_id_downlinkNASTransport,
                                         Criticality_reject,
                                         &asn_DEF_DownlinkNASTransport,
                                         downlinkNasTransport_p) < 0) {
        free(message_p);
        return -1;
    }

    return itti_send_msg_to_task(TASK_SCTP, message_p);
}

int s1ap_mme_encode_ue_context_release_command(UEContextReleaseCommandIEs_t *ueContextReleaseCommandIEs,
                                               ue_description_t           *ue_ref) {
    UEContextReleaseCommand_t  ueContextReleaseCommand;
    UEContextReleaseCommand_t *ueContextReleaseCommand_p = &ueContextReleaseCommand;
    MessageDef                *message_p;

    memset(ueContextReleaseCommand_p, 0, sizeof(UEContextReleaseCommand_t));
    message_p = (MessageDef *)malloc(sizeof(MessageDef));

    message_p->messageId = SCTP_NEW_DATA_REQ;
    message_p->originTaskId = TASK_S1AP;
    message_p->destinationTaskId = TASK_SCTP;
    message_p->msg.sctpNewDataReq.stream = ue_ref->sctp_stream_send;
    message_p->msg.sctpNewDataReq.assocId = ue_ref->eNB->sctp_assoc_id;

    /* Convert IE structure into asn1 message */
    if (s1ap_encode_uecontextreleasecommandies(ueContextReleaseCommand_p, ueContextReleaseCommandIEs) < 0) {
        goto error;
    }

    /* Generate Initiating message for the list of IEs */
    if (s1ap_generate_initiating_message(&message_p->msg.sctpNewDataReq.buffer,
        &message_p->msg.sctpNewDataReq.bufLen,
        ProcedureCode_id_UEContextRelease,
        Criticality_reject,
        &asn_DEF_UEContextReleaseCommand,
        ueContextReleaseCommand_p) < 0)
        goto error;

    return itti_send_msg_to_task(TASK_SCTP, message_p);
error:
    free(message_p);
    return -1;
}
