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

#include <assert.h>
#include <stdint.h>

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_encoder.h"
#include "s1ap_mme_handlers.h"

#include "s1ap_mme.h"

/* Every time a new UE is associated, increment this variable.
 * But care if it wraps to increment also the s1ap_mme_ue_id_has_wrapped
 * variable. Limit: UINT32_MAX (in stdint.h).
 */
static uint32_t s1ap_mme_ue_id = 0;
static uint8_t  s1ap_mme_ue_id_has_wrapped = 0;

static int s1ap_generate_s1_setup_response(eNB_description_t *eNB_association);
static int s1ap_mme_generate_ue_context_release_command(ue_description_t *ue_ref);

extern MessageDef *receivedMessage;

//Forward declaration
struct s1ap_message_s;

/* Handlers matrix. Only mme related procedures present here.
 */
s1ap_message_decoded_callback messages_callback[][3] = {
    { 0, 0, 0 }, /* HandoverPreparation */
    { 0, 0, 0 }, /* HandoverResourceAllocation */
    { 0, 0, 0 }, /* HandoverNotification */
    { s1ap_mme_handle_path_switch_request, 0, 0 }, /* PathSwitchRequest */
    { 0, 0, 0 }, /* HandoverCancel */
    { 0, 0, 0 }, /* E_RABSetup */
    { 0, 0, 0 }, /* E_RABModify */
    { 0, 0, 0 }, /* E_RABRelease */
    { 0, 0, 0 }, /* E_RABReleaseIndication */
    { 0, s1ap_mme_handle_initial_context_setup_response,
      s1ap_mme_handle_initial_context_setup_failure }, /* InitialContextSetup */
    { 0, 0, 0 }, /* Paging */
    { 0, 0, 0 }, /* downlinkNASTransport */
    { s1ap_mme_handle_initial_ue_message, 0, 0 }, /* initialUEMessage */
    { s1ap_mme_handle_uplink_nas_transport, 0, 0 }, /* uplinkNASTransport */
    { 0, 0, 0 }, /* Reset */
    { 0, 0, 0 }, /* ErrorIndication */
    { 0, 0, 0 }, /* NASNonDeliveryIndication */
    { s1ap_mme_handle_s1_setup_request, 0, 0 }, /* S1Setup */
    { 0, 0, 0 }, /* UEContextReleaseRequest */
    { 0, 0, 0 }, /* DownlinkS1cdma2000tunneling */
    { 0, 0, 0 }, /* UplinkS1cdma2000tunneling */
    { 0, 0, 0 }, /* UEContextModification */
    { 0, 0, 0 }, /* UECapabilityInfoIndication*/
    { s1ap_mme_handle_ue_context_release_request,
      s1ap_mme_handle_ue_context_release_complete, 0 }, /* UEContextRelease*/
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
#if defined(UPDATE_RELEASE_9)
    { 0, 0, 0 }, /* Kill */
    { 0, 0, 0 }, /* DownlinkUEAssociatedLPPaTransport  */
    { 0, 0, 0 }, /* UplinkUEAssociatedLPPaTransport */
    { 0, 0, 0 }, /* DownlinkNonUEAssociatedLPPaTransport */
    { 0, 0, 0 }, /* UplinkNonUEAssociatedLPPaTransport */
#endif
};

static const char *direction2String[] = {
    "", /* Nothing */
    "Originating message", /* originating message */
    "Successfull outcome", /* successfull outcome */
    "UnSuccessfull outcome", /* successfull outcome */
};

int s1ap_mme_handle_message(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    /* Checking procedure Code and direction of message */
    if ((message->procedureCode > (sizeof(messages_callback) / (3 * sizeof(s1ap_message_decoded_callback)))) ||
        (message->direction > S1AP_PDU_PR_unsuccessfulOutcome)) {

        S1AP_DEBUG("[SCTP %d] Either procedureCode %d or direction %d exceed expected\n",
              assocId, message->procedureCode, message->direction);
        return -1;
    }
    /* No handler present.
    * This can mean not implemented or no procedure for eNB (wrong message).
    */
    if (messages_callback[message->procedureCode][message->direction-1] == NULL) {
        S1AP_DEBUG("[SCTP %d] No handler for procedureCode %d in %s\n", assocId, message->procedureCode,
            direction2String[message->direction]);
        return -2;
    }

    /* Calling the right handler */
    return (*messages_callback[message->procedureCode][message->direction-1])(assocId, stream, message);
}

////////////////////////////////////////////////////////////////////////////////
//************************** Management procedures ***************************//
////////////////////////////////////////////////////////////////////////////////

int s1ap_mme_handle_s1_setup_request(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    S1SetupRequestIEs_t *s1SetupRequest_p;
    eNB_description_t *eNB_association;
    uint32_t eNB_id = 0;
    char *eNB_name = NULL;

    s1SetupRequest_p = &message->msg.s1SetupRequestIEs;

    /* We received a new valid S1 Setup Request on a stream != 0.
     * This should not happen -> reject eNB s1 setup request.
     */
    if (receivedMessage->msg.s1apSctpNewMessageInd.stream != 0) {
        S1AP_DEBUG("Received new s1 setup request on stream != 0\n");
        return -1;
    }

    S1AP_DEBUG("New s1 setup request incoming from\n");
    if ((s1SetupRequest_p->presenceMask & S1SETUPREQUESTIES_ENBNAME_PRESENT) ==
        S1SETUPREQUESTIES_ENBNAME_PRESENT) {
        S1AP_DEBUG("- eNB name: %s\n", s1SetupRequest_p->eNBname.buf);
        eNB_name = (char*)s1SetupRequest_p->eNBname.buf;
    }
    if (s1SetupRequest_p->global_ENB_ID.eNB_ID.present == ENB_ID_PR_homeENB_ID) {
        // Home eNB ID = 28 bits
        uint8_t *eNB_id_buf = s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.homeENB_ID.buf;
        if (s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID.size != 28) {
            //TODO: handle case were size != 28 -> notify ? reject ?
        }
        eNB_id = (eNB_id_buf[0] << 20) + (eNB_id_buf[1] << 12) + (eNB_id_buf[2] << 4) + ((eNB_id_buf[3] & 0xf0) >> 4);
        S1AP_DEBUG("- eNB id: %u\n", eNB_id);
    } else {
        // Macro eNB = 20 bits
        uint8_t *eNB_id_buf = s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID.buf;
        if (s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID.size != 20) {
            //TODO: handle case were size != 20 -> notify ? reject ?
        }
        eNB_id = (eNB_id_buf[0] << 12) + (eNB_id_buf[1] << 4) + ((eNB_id_buf[2] & 0xf0) >> 4);
        S1AP_DEBUG("- eNB id: %u\n", eNB_id);
    }

    if (nb_eNB_associated == MAX_NUMBER_OF_ENB) {
        S1SetupFailureIEs_t s1SetupFailure;
        memset(&s1SetupFailure, 0, sizeof(S1SetupFailureIEs_t));

        /* Send an overload cause... */
        s1SetupFailure.cause.present = Cause_PR_misc;
        s1SetupFailure.cause.choice.misc = CauseMisc_control_processing_overload;
        S1AP_DEBUG("There is too much eNB connected to MME, rejecting the association\n");
        S1AP_DEBUG("Connected = %d, maximum allowed = %d\n", nb_eNB_associated, MAX_NUMBER_OF_ENB);
        s1ap_mme_encode_s1setupfailure(&s1SetupFailure, receivedMessage->msg.s1apSctpNewMessageInd.assocId);
        return -1;
    }
    S1AP_DEBUG("Adding eNB to the list of served eNBs\n");

    if ((eNB_association = s1ap_is_eNB_id_in_list(eNB_id)) == NULL) {
        /* eNB has not been fount in list of associated eNB,
         * Add it to the tail of list and initialize data */
        if ((eNB_association = s1ap_new_eNB()) == NULL) {
            /* Failed to allocate new eNB context... 
             * TODO: generate S1SetupFailure message
             */
            return -1;
        }
        eNB_association->s1_state = S1AP_RESETING;
        eNB_association->eNB_id = eNB_id;
        eNB_association->default_paging_drx = s1SetupRequest_p->defaultPagingDRX;

        if (eNB_name != NULL) {
            memcpy(eNB_association->eNB_name, s1SetupRequest_p->eNBname.buf, s1SetupRequest_p->eNBname.size);
        }
        /* initialize the next sctp stream to 1 as 0 is reserved for non
         * ue associated signalling.
         */
        eNB_association->next_sctp_stream = 1;
        /* Fill in in and out number of streams available on SCTP connection. */
        eNB_association->instreams = receivedMessage->msg.s1apSctpNewMessageInd.instreams;
        eNB_association->outstreams = receivedMessage->msg.s1apSctpNewMessageInd.outstreams;
        eNB_association->sctp_assoc_id = assocId;
    } else {
        eNB_association->s1_state = S1AP_RESETING;
        /* eNB has been fount in list, consider the s1 setup request as a reset connection,
         * reseting any previous UE state if sctp association is != than the previous one */
        if (eNB_association->sctp_assoc_id != receivedMessage->msg.s1apSctpNewMessageInd.assocId) {
            S1SetupFailureIEs_t s1SetupFailure;
            memset(&s1SetupFailure, 0, sizeof(S1SetupFailureIEs_t));

            /* Send an overload cause... */
            s1SetupFailure.cause.present = Cause_PR_misc; //TODO: send the right cause
            s1SetupFailure.cause.choice.misc = CauseMisc_control_processing_overload;
            S1AP_DEBUG("Rejeting s1 setup request as eNB id %d is already associated to an active sctp association"
                   "Previous known: %d, new one: %d\n",
                   eNB_id, eNB_association->sctp_assoc_id, receivedMessage->msg.s1apSctpNewMessageInd.assocId);
            s1ap_mme_encode_s1setupfailure(&s1SetupFailure, receivedMessage->msg.s1apSctpNewMessageInd.assocId);
            return -1;
        }
        /* TODO: call the reset procedure */
    }
    s1ap_dump_eNB(eNB_association);
    return s1ap_generate_s1_setup_response(eNB_association);
}

static int s1ap_generate_s1_setup_response(eNB_description_t *eNB_association) {
    int enc_rval = 0;
    S1SetupResponseIEs_t  s1SetupResponse;
    ServedGUMMEIsItem_t servedGUMMEI;
    MME_Code_t          mmeCode;
    MME_Group_ID_t      mmeGroupID;
    PLMNidentity_t      plmn;
    char mmec[]   = { 0x00 };
    char mmegid[] = { 0x00, 0x00 };
    char plmnid[] = { 0x20, 0x38, 0x04 };

    // Generating response
    memset(&s1SetupResponse, 0, sizeof(S1SetupResponseIEs_t));
    memset(&servedGUMMEI, 0, sizeof(ServedGUMMEIsItem_t));
    memset(&mmeCode, 0, sizeof(MME_Code_t));
    memset(&mmeGroupID, 0, sizeof(MME_Group_ID_t));
    memset(&plmn, 0, sizeof(PLMNidentity_t));

    OCTET_STRING_fromBuf(&mmeCode, mmec, 1);
    OCTET_STRING_fromBuf(&mmeGroupID, mmegid, 2);
    OCTET_STRING_fromBuf(&plmn, plmnid, 3);

    s1SetupResponse.relativeMMECapacity = 10;
    ASN_SEQUENCE_ADD(&servedGUMMEI.servedPLMNs, &plmn);
    ASN_SEQUENCE_ADD(&servedGUMMEI.servedGroupIDs, &mmeGroupID);
    ASN_SEQUENCE_ADD(&servedGUMMEI.servedMMECs, &mmeCode);
    ASN_SEQUENCE_ADD(&s1SetupResponse.servedGUMMEIs, &servedGUMMEI);

    enc_rval = s1ap_mme_encode_s1setupresponse(&s1SetupResponse, eNB_association->sctp_assoc_id);
    /* Failed to encode s1 setup response... */
    if (enc_rval < 0) {
        S1AP_DEBUG("Removed eNB %d\n", eNB_association->sctp_assoc_id);
        s1ap_remove_eNB(eNB_association);
    } else {
        /* Consider the response as sent. S1AP is ready to accept UE contexts */
        eNB_association->s1_state = S1AP_READY;
    }
    return enc_rval;
}

////////////////////////////////////////////////////////////////////////////////
//************************** NAS transport ***********************************//
////////////////////////////////////////////////////////////////////////////////

int s1ap_mme_handle_initial_ue_message(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    InitialUEMessageIEs_t *initialUEMessage_p;
    ue_description_t    *ue_ref;
    eNB_description_t   *eNB_ref;
    uint32_t             eNB_ue_s1ap_id;

    initialUEMessage_p = &message->msg.initialUEMessageIEs;

    if ((eNB_ref = s1ap_is_eNB_assoc_id_in_list(assocId)) == NULL) {
        S1AP_DEBUG("Unkwnon eNB on assocId %d\n", assocId);
        return -1;
    }

    // eNB UE S1AP ID is limited to 24 bits
    eNB_ue_s1ap_id = (uint32_t)(initialUEMessage_p->eNB_UE_S1AP_ID & 0x00ffffff);

    S1AP_DEBUG("New Initial UE message received with eNB UE S1AP ID: %d\n",
               (int)eNB_ue_s1ap_id);

    if ((ue_ref = s1ap_is_ue_eNB_id_in_list(eNB_ref, eNB_ue_s1ap_id)) == NULL) {
        /* This UE eNB Id has currently no known s1 association.
         * Create new UE context by associating new mme_ue_s1ap_id.
         * Update eNB UE list.
         * Forward message to NAS.
         */
        if ((ue_ref = s1ap_new_ue(assocId)) == NULL)
            // If we failed to allocate a new UE return -1
            return -1;

        ue_ref->s1_ue_state = S1AP_UE_WAITING_CSR;
        ue_ref->eNB_ue_s1ap_id = eNB_ue_s1ap_id;
        ue_ref->mme_ue_s1ap_id = s1ap_mme_ue_id;
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
        if (s1ap_mme_ue_id_has_wrapped == 0) {
            s1ap_mme_ue_id++;
            if (s1ap_mme_ue_id == 0) s1ap_mme_ue_id_has_wrapped = 1;
        } else {
            /* TODO: should take the first available mme_ue_s1ap_id instead of
             * the s1ap_mme_ue_id variable.
             */
            assert(0);
        }
        s1ap_dump_eNB(ue_ref->eNB);
        {
            MessageDef              *message_p;
            SgwCreateSessionRequest *session_request_p;

            message_p = itti_alloc_new_message(TASK_S1AP, TASK_SGW_LITE, SGW_CREATE_SESSION_REQUEST);
            if (message_p == NULL) return -1;

            /* WARNING:
             * Some parameters should be provided by NAS Layer:
             * - ue_time_zone
             * - pdn type
             * - imsi
             * - msisdn
             * - mei
             * - uli
             * - uci
             * - bearer level qos
             * Some parameters should be provided bt HSS:
             * - PGW address for CP
             * - paa
             * - ambr
             * and by MME Application layer:
             * - selection_mode
             * Set these parameters with random values for now.
             */

            session_request_p = &message_p->msg.sgwCreateSessionRequest;
            memset(session_request_p, 0, sizeof(SgwCreateSessionRequest));

            /* Local MME TEID */
            ue_ref->teid = 1;

            /* As the create session request is the first exchanged message and as
             * no tunnel had been previously setup, the distant teid is set to 0.
             * The remote teid will be provided in the response message.
             */
            session_request_p->teid = 0;
            session_request_p->rat_type = RAT_TYPE_EUTRAN;

            /* Asking for default bearer in initial UE message */
            session_request_p->sender_fteid_for_cp.teid = 1;
            session_request_p->sender_fteid_for_cp.interface_type = S11_MME_GTP_C;
            session_request_p->bearer_to_create.eps_bearer_id = 5;
            session_request_p->bearer_to_create.bearer_level_qos.pci = PRE_EMPTION_CAPABILITY_DISABLED;
            session_request_p->bearer_to_create.bearer_level_qos.mbr_uplink = 50000;
            session_request_p->bearer_to_create.bearer_level_qos.mbr_uplink = 100000;

            /* Set PDN type for pdn_type and PAA even if this IE is redundant */
            session_request_p->pdn_type = IPv4;
            session_request_p->paa.pdn_type = IPv4;
            /* UE DHCPv4 allocated ip address */
            session_request_p->paa.ipv4_address = 0x00000000;

            session_request_p->selection_mode = MS_O_N_P_APN_S_V;
            return itti_send_msg_to_task(TASK_SGW_LITE, message_p);
        }
    }
    return 0;
}

int s1ap_mme_handle_uplink_nas_transport(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    UplinkNASTransportIEs_t *uplinkNASTransport_p;
//     uint32_t eNB_ue_s1ap_id;
    ue_description_t *ue_ref;

    uplinkNASTransport_p = &message->msg.uplinkNASTransportIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(uplinkNASTransport_p->mme_ue_s1ap_id)) == NULL) {
        S1AP_DEBUG("No UE is attached to this mme UE s1ap id: %d\n",
               (int)uplinkNASTransport_p->mme_ue_s1ap_id);
        return -1;
    }
    if (ue_ref->s1_ue_state != S1AP_UE_CONNECTED) {
        S1AP_DEBUG("Received uplink NAS transport while UE in state != S1AP_UE_CONNECTED\n");
        return -1;
    }

    //TODO: forward NAS PDU to NAS
    S1AP_DEBUG("New uplink NAS transport message received\n");
    return 0;
}

int s1ap_generate_downlink_nas_transport(S1apNASNewMessageInd *message) {
    ue_description_t *ue_ref;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(message->mme_ue_s1ap_id)) == NULL) {
        //TODO: inform NAS
    } else {
        /* We have fount the UE in the list.
         * Create new IE list message and encode it.
         */
        DownlinkNASTransportIEs_t downlinkNasTransport;

        memset(&downlinkNasTransport, 0, sizeof(DownlinkNASTransportIEs_t));

        /* Setting UE informations with the ones fount in ue_ref */
        downlinkNasTransport.mme_ue_s1ap_id = ue_ref->mme_ue_s1ap_id;
        downlinkNasTransport.eNB_UE_S1AP_ID = ue_ref->eNB_ue_s1ap_id;
        OCTET_STRING_fromBuf(&downlinkNasTransport.nas_pdu, (char*)message->nas_buffer, message->nas_length);
        s1ap_mme_encode_downlink_nas_transport(&downlinkNasTransport, ue_ref);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//******************* Context Management procedures **************************//
////////////////////////////////////////////////////////////////////////////////

int s1ap_mme_handle_initial_context_setup_response(
    uint32_t assocId,
    uint32_t stream,
    struct s1ap_message_s *message) {

    InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs_p;
    ue_description_t                 *ue_ref;
    SgwModifyBearerRequest           *modify_request_p;
    MessageDef                       *message_p;
    E_RABSetupItemCtxtSURes_t        *eRABSetupItemCtxtSURes_p;

    initialContextSetupResponseIEs_p = &message->msg.initialContextSetupResponseIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(initialContextSetupResponseIEs_p->mme_ue_s1ap_id)) == NULL) {
        S1AP_DEBUG("No UE is attached to this mme UE s1ap id: %d\n",
                   (int)initialContextSetupResponseIEs_p->mme_ue_s1ap_id);
        return -1;
    }
    if (ue_ref->eNB_ue_s1ap_id != initialContextSetupResponseIEs_p->eNB_UE_S1AP_ID) {
        S1AP_DEBUG("Mismatch in eNB UE S1AP ID, known: %d, received: %d\n",
                   ue_ref->eNB_ue_s1ap_id, (int)initialContextSetupResponseIEs_p->eNB_UE_S1AP_ID);
        return -1;
    }

    if (initialContextSetupResponseIEs_p->e_RABSetupListCtxtSURes.e_RABSetupItemCtxtSURes.count != 1) {
        S1AP_DEBUG("E-RAB creation has failed\n");
        return -1;
    }

    ue_ref->s1_ue_state = S1AP_UE_CONNECTED;

    message_p = itti_alloc_new_message(TASK_SGW_LITE, TASK_S1AP, SGW_MODIFY_BEARER_REQUEST);

    if (message_p == NULL) {
        return -1;
    }

    eRABSetupItemCtxtSURes_p = (E_RABSetupItemCtxtSURes_t*)initialContextSetupResponseIEs_p->e_RABSetupListCtxtSURes.e_RABSetupItemCtxtSURes.array[0];

    modify_request_p = &message_p->msg.sgwModifyBearerRequest;
    modify_request_p->teid = ue_ref->teid;
    modify_request_p->bearer_context_to_modify.eps_bearer_id     = eRABSetupItemCtxtSURes_p->e_RAB_ID;
    modify_request_p->bearer_context_to_modify.s1_eNB_fteid.teid = *((uint32_t*)eRABSetupItemCtxtSURes_p->gTP_TEID.buf);
    modify_request_p->bearer_context_to_modify.s1_eNB_fteid.ipv4 = 1;
    memcpy(&modify_request_p->bearer_context_to_modify.s1_eNB_fteid.ipv4_address, eRABSetupItemCtxtSURes_p->transportLayerAddress.buf, 4);

    return itti_send_msg_to_task(TASK_SGW_LITE, message_p);
}

int s1ap_mme_handle_ue_context_release_request(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    UEContextReleaseRequestIEs_t *ueContextReleaseRequest_p;
    ue_description_t *ue_ref = NULL;

    ueContextReleaseRequest_p = &message->msg.ueContextReleaseRequestIEs;

    /* The UE context release procedure is initiated if the cause is != than user inactivity.
     * TS36.413 #8.3.2.2.
     */
    if (ueContextReleaseRequest_p->cause.present == Cause_PR_radioNetwork) {
        if (ueContextReleaseRequest_p->cause.choice.radioNetwork == CauseRadioNetwork_user_inactivity) {
            return -1;
        }
    }
    if ((ue_ref = s1ap_is_ue_mme_id_in_list(ueContextReleaseRequest_p->mme_ue_s1ap_id)) == NULL) {
        /* MME doesn't know the MME UE S1AP ID provided.
         * TODO
         */
        return -1;
    } else {
        if (ue_ref->eNB_ue_s1ap_id == (ueContextReleaseRequest_p->eNB_UE_S1AP_ID & 0x00ffffff)) {
            /* Both eNB UE S1AP ID and MME UE S1AP ID match.
             * -> Send a UE context Release Command to eNB.
             * TODO
             */
        } else {
            // TODO
            return -1;
        }
    }
    return 0;
}

static int s1ap_mme_generate_ue_context_release_command(ue_description_t *ue_ref) {
    UEContextReleaseCommandIEs_t ueContextReleaseCommandIEs;

    if (ue_ref == NULL) {
        return -1;
    }
    memset((void*)&ueContextReleaseCommandIEs, 0, sizeof(UEContextReleaseCommandIEs_t));

    /* Fill in ID pair */
    ueContextReleaseCommandIEs.uE_S1AP_IDs.present = UE_S1AP_IDs_PR_uE_S1AP_ID_pair;
    ueContextReleaseCommandIEs.uE_S1AP_IDs.choice.uE_S1AP_ID_pair.mME_UE_S1AP_ID = ue_ref->mme_ue_s1ap_id;
    ueContextReleaseCommandIEs.uE_S1AP_IDs.choice.uE_S1AP_ID_pair.eNB_UE_S1AP_ID = ue_ref->eNB_ue_s1ap_id;
    ueContextReleaseCommandIEs.uE_S1AP_IDs.choice.uE_S1AP_ID_pair.iE_Extensions = NULL;

    ueContextReleaseCommandIEs.cause.present = Cause_PR_radioNetwork;
    ueContextReleaseCommandIEs.cause.choice.radioNetwork = CauseRadioNetwork_release_due_to_eutran_generated_reason;

    return s1ap_mme_encode_ue_context_release_command(&ueContextReleaseCommandIEs, ue_ref);
}

int s1ap_mme_handle_ue_context_release_complete(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    UEContextReleaseCompleteIEs_t *ueContextReleaseComplete_p;
    ue_description_t *ue_ref = NULL;

    ueContextReleaseComplete_p = &message->msg.ueContextReleaseCompleteIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(ueContextReleaseComplete_p->mme_ue_s1ap_id)) == NULL) {
        /* MME doesn't know the MME UE S1AP ID provided.
         * TODO
         */
        return -1;
    }
    /* eNB has sent a release complete message. We can safely remove UE context.
     * TODO: inform NAS and remove e-RABS.
     */
    s1ap_remove_ue(ue_ref);
    S1AP_DEBUG("Removed UE %u\n", (uint32_t)ueContextReleaseComplete_p->mme_ue_s1ap_id);
    return 0;
}

int s1ap_mme_handle_initial_context_setup_failure(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    InitialContextSetupFailureIEs_t *initialContextSetupFailureIEs_p;
    ue_description_t              *ue_ref = NULL;

    initialContextSetupFailureIEs_p = &message->msg.initialContextSetupFailureIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(initialContextSetupFailureIEs_p->mme_ue_s1ap_id)) == NULL) {
        /* MME doesn't know the MME UE S1AP ID provided. */
        return -1;
    }
    if (ue_ref->eNB_ue_s1ap_id != (initialContextSetupFailureIEs_p->eNB_UE_S1AP_ID & 0x00ffffff)) {
        return -1;
    }
    S1AP_DEBUG("Removed UE %u\n", (uint32_t)initialContextSetupFailureIEs_p->mme_ue_s1ap_id);
    s1ap_remove_ue(ue_ref);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//************************ Handover signalling *******************************//
////////////////////////////////////////////////////////////////////////////////

int s1ap_mme_handle_path_switch_request(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
    PathSwitchRequestIEs_t *pathSwitchRequest_p;
    ue_description_t     *ue_ref;
    uint32_t              eNB_ue_s1ap_id;

    pathSwitchRequest_p = &message->msg.pathSwitchRequestIEs;

    // eNB UE S1AP ID is limited to 24 bits
    eNB_ue_s1ap_id = (uint32_t)(pathSwitchRequest_p->eNB_UE_S1AP_ID & 0x00ffffff);

    S1AP_DEBUG("Path Switch Request message received from eNB UE S1AP ID: %d\n",
               (int)eNB_ue_s1ap_id);

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(pathSwitchRequest_p->sourceMME_UE_S1AP_ID)) == NULL) {
        /* The MME UE S1AP ID provided by eNB doesn't point to any valid UE.
         * MME replies with a PATH SWITCH REQUEST FAILURE message and start operation
         * as described in TS 36.413 [11].
         * TODO
         */
    } else {
        if (ue_ref->eNB_ue_s1ap_id != eNB_ue_s1ap_id) {
            /* Received unique UE eNB ID mismatch with the one known in MME.
             * Handle this case as defined upper.
             * TODO
             */
            return -1;
        }
        //TODO: Switch the eRABs provided
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//************************* E-RAB management *********************************//
////////////////////////////////////////////////////////////////////////////////

// int s1ap_mme_handle_e_rab_setup_response(uint32_t assocId, uint32_t stream, struct s1ap_message_s *message) {
//     
// }

int s1ap_handle_sctp_deconnection(uint8_t assoc_id) {
    eNB_description_t *eNB_association;

    /* Checking that the assoc id has a valid eNB attached to. */
    if ((eNB_association = s1ap_is_eNB_assoc_id_in_list(assoc_id)) == NULL) {
        S1AP_DEBUG("No eNB attached to this assoc_id: %d\n",
                assoc_id);
        return -1;
    }
    s1ap_remove_eNB(eNB_association);
    s1ap_dump_eNB_list();

    S1AP_DEBUG("Removed eNB attached to assoc_id: %d\n",
               assoc_id);
    return 0;
}

int s1ap_handle_create_session_response(SgwCreateSessionResponse *session_response_p) {
    /* We received create session response from S-GW on S11 interface abstraction.
     * At least one bearer has been established. We can now send s1ap initial context setup request
     * message to eNB.
     */
    char supportedAlgorithms[] = { 0x02, 0xa0 };
    char securityKey[] = { 0xfd, 0x23, 0xad, 0x22, 0xd0, 0x21, 0x02, 0x90, 0x19, 0xed,
                            0xcf, 0xc9, 0x78, 0x44, 0xba, 0xbb, 0x34, 0x6e, 0xff, 0x89,
                            0x1c, 0x3a, 0x56, 0xf0, 0x81, 0x34, 0xdd, 0xee, 0x19, 0x55,
                            0xf2, 0x1f };

    ue_description_t *ue_ref = NULL;
    InitialContextSetupRequestIEs_t initialContextSetupRequest;
    E_RABToBeSetupItemCtxtSUReq_t   e_RABToBeSetup;

    if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 0) &&
        (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 0)) {

        S1AP_ERROR("No IP address provided for transport layer address\n"
                   "-->Sending Intial context setup failure\n");
        return -1;
    }

    if ((ue_ref = s1ap_is_teid_in_list(session_response_p->teid)) == NULL) {
        S1AP_DEBUG("Teid %d is not attached to any UE context\n", session_response_p->teid);
        return -1;
    }

    memset(&initialContextSetupRequest, 0, sizeof(InitialContextSetupRequestIEs_t));
    memset(&e_RABToBeSetup, 0, sizeof(E_RABToBeSetupItemCtxtSUReq_t));

    initialContextSetupRequest.mme_ue_s1ap_id = ue_ref->mme_ue_s1ap_id;
    initialContextSetupRequest.eNB_UE_S1AP_ID = ue_ref->eNB_ue_s1ap_id;

    /* uEaggregateMaximumBitrateDL and uEaggregateMaximumBitrateUL expressed in term of bits/sec */
    asn_int642INTEGER(&initialContextSetupRequest.uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL, 100000000UL);
    asn_int642INTEGER(&initialContextSetupRequest.uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL, 50000000UL);

    e_RABToBeSetup.e_RAB_ID = session_response_p->bearer_context_created.eps_bearer_id; /* ??? */
    e_RABToBeSetup.e_RABlevelQoSParameters.qCI = 0; // ??
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel = 15; //No priority
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability = Pre_emptionCapability_shall_not_trigger_pre_emption;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability = Pre_emptionVulnerability_not_pre_emptable;

    e_RABToBeSetup.gTP_TEID.buf  = calloc(4, sizeof(uint8_t));
    memcpy(e_RABToBeSetup.gTP_TEID.buf, &session_response_p->bearer_context_created.s1u_sgw_fteid.teid, 4);
    e_RABToBeSetup.gTP_TEID.size = 4;
    if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 1) &&
        (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 0)) {
        /* Only IPv4 supported */
        e_RABToBeSetup.transportLayerAddress.buf = (uint8_t*)&session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address;
        e_RABToBeSetup.transportLayerAddress.size = 4;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
    } else if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 0) &&
               (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 1)) {
        /* Only IPv6 supported */
        e_RABToBeSetup.transportLayerAddress.buf = (uint8_t*)&session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address;
        e_RABToBeSetup.transportLayerAddress.size = 16;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
    } else if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 1) &&
               (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 1)) {
        /* Both IPv4 and IPv6 supported */
        e_RABToBeSetup.transportLayerAddress.buf = calloc(20, sizeof(uint8_t));
        e_RABToBeSetup.transportLayerAddress.size = 20;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
        memcpy(e_RABToBeSetup.transportLayerAddress.buf, &session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address, 4);
        memcpy(e_RABToBeSetup.transportLayerAddress.buf + 4, session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6_address, 16);
    }

    ASN_SEQUENCE_ADD(&initialContextSetupRequest.e_RABToBeSetupListCtxtSUReq, &e_RABToBeSetup);

    initialContextSetupRequest.ueSecurityCapabilities.encryptionAlgorithms.buf = (uint8_t *)supportedAlgorithms;
    initialContextSetupRequest.ueSecurityCapabilities.encryptionAlgorithms.size = 2;
    initialContextSetupRequest.ueSecurityCapabilities.encryptionAlgorithms.bits_unused = 0;

    initialContextSetupRequest.ueSecurityCapabilities.integrityProtectionAlgorithms.buf = (uint8_t *)supportedAlgorithms;
    initialContextSetupRequest.ueSecurityCapabilities.integrityProtectionAlgorithms.size = 2;
    initialContextSetupRequest.ueSecurityCapabilities.integrityProtectionAlgorithms.bits_unused = 0;

    initialContextSetupRequest.securityKey.buf = (uint8_t*)securityKey; /* 256 bits length */
    initialContextSetupRequest.securityKey.size = 32;
    initialContextSetupRequest.securityKey.bits_unused = 0;

    return s1ap_mme_encode_initial_context_setup_request(&initialContextSetupRequest, ue_ref);
}
