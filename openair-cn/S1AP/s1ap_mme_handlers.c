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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "mme_config.h"
#include "assertions.h"
#include "conversions.h"

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
#include "s1ap_mme_encoder.h"
#include "s1ap_mme_handlers.h"
#include "s1ap_mme_nas_procedures.h"

#include "s1ap_mme_itti_messaging.h"

#include "s1ap_mme.h"
#include "s1ap_mme_ta.h"

static int s1ap_generate_s1_setup_response(eNB_description_t *eNB_association);
static int s1ap_mme_generate_ue_context_release_command(
    ue_description_t *ue_ref);

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
    {
        0, s1ap_mme_handle_initial_context_setup_response,
        s1ap_mme_handle_initial_context_setup_failure
    }, /* InitialContextSetup */
    { 0, 0, 0 }, /* Paging */
    { 0, 0, 0 }, /* downlinkNASTransport */
    { s1ap_mme_handle_initial_ue_message, 0, 0 }, /* initialUEMessage */
    { s1ap_mme_handle_uplink_nas_transport, 0, 0 }, /* uplinkNASTransport */
    { 0, 0, 0 }, /* Reset */
    { 0, 0, 0 }, /* ErrorIndication */
    { s1ap_mme_handle_nas_non_delivery, 0, 0 }, /* NASNonDeliveryIndication */
    { s1ap_mme_handle_s1_setup_request, 0, 0 }, /* S1Setup */
    { 0, 0, 0 }, /* UEContextReleaseRequest */
    { 0, 0, 0 }, /* DownlinkS1cdma2000tunneling */
    { 0, 0, 0 }, /* UplinkS1cdma2000tunneling */
    { 0, 0, 0 }, /* UEContextModification */
    { s1ap_mme_handle_ue_cap_indication, 0, 0 }, /* UECapabilityInfoIndication*/
    {
        s1ap_mme_handle_ue_context_release_request,
        s1ap_mme_handle_ue_context_release_complete, 0
    }, /* UEContextRelease*/
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

int s1ap_mme_handle_message(uint32_t assoc_id, uint32_t stream,
                            struct s1ap_message_s *message)
{
    /* Checking procedure Code and direction of message */
    if ((message->procedureCode > (sizeof(messages_callback) / (3 * sizeof(
                                       s1ap_message_decoded_callback)))) ||
            (message->direction > S1AP_PDU_PR_unsuccessfulOutcome)) {

        S1AP_DEBUG("[SCTP %d] Either procedureCode %d or direction %d exceed expected\n",
                   assoc_id, (int)message->procedureCode, (int)message->direction);
        return -1;
    }
    /* No handler present.
    * This can mean not implemented or no procedure for eNB (wrong message).
    */
    if (messages_callback[message->procedureCode][message->direction-1] == NULL) {
        S1AP_DEBUG("[SCTP %d] No handler for procedureCode %d in %s\n", assoc_id,
                   (int)message->procedureCode,
                   direction2String[(int)message->direction]);
        return -2;
    }

    /* Calling the right handler */
    return (*messages_callback[message->procedureCode][message->direction-1])(
               assoc_id, stream, message);
}

int s1ap_mme_set_cause(S1ap_Cause_t *cause_p, S1ap_Cause_PR cause_type, long cause_value)
{
    DevAssert(cause_p != NULL);

    cause_p->present = cause_type;

    switch(cause_type)
    {
        case S1ap_Cause_PR_radioNetwork:
            cause_p->choice.misc = cause_value;
            break;
        case S1ap_Cause_PR_transport:
            cause_p->choice.transport = cause_value;
            break;
        case S1ap_Cause_PR_nas:
            cause_p->choice.nas = cause_value;
            break;
        case S1ap_Cause_PR_protocol:
            cause_p->choice.protocol = cause_value;
            break;
        case S1ap_Cause_PR_misc:
            cause_p->choice.misc = cause_value;
            break;
        default:
            return -1;
    }
    return 0;
}

int s1ap_mme_generate_s1_setup_failure(
    uint32_t assoc_id, S1ap_Cause_PR cause_type, long cause_value,
    long time_to_wait)
{
    uint8_t *buffer_p;
    uint32_t length;
    s1ap_message message;
    S1ap_S1SetupFailureIEs_t *s1_setup_failure_p;

    memset(&message, 0, sizeof(s1ap_message));

    s1_setup_failure_p = &message.msg.s1ap_S1SetupFailureIEs;

    message.procedureCode = S1ap_ProcedureCode_id_S1Setup;
    message.direction     = S1AP_PDU_PR_unsuccessfulOutcome;

    s1ap_mme_set_cause(&s1_setup_failure_p->cause, cause_type, cause_value);

    /* Include the optional field time to wait only if the value is > -1 */
    if (time_to_wait > -1) {
        s1_setup_failure_p->presenceMask |= S1AP_S1SETUPFAILUREIES_TIMETOWAIT_PRESENT;
        s1_setup_failure_p->timeToWait = time_to_wait;
    }

    if (s1ap_mme_encode_pdu(&message, &buffer_p, &length) < 0) {
        S1AP_ERROR("Failed to encode s1 setup failure\n");
        return -1;
    }

    return s1ap_mme_itti_send_sctp_request(buffer_p, length, assoc_id, 0);
}

////////////////////////////////////////////////////////////////////////////////
//************************** Management procedures ***************************//
////////////////////////////////////////////////////////////////////////////////

int s1ap_mme_handle_s1_setup_request(uint32_t assoc_id, uint32_t stream,
                                     struct s1ap_message_s *message)
{
    if (hss_associated)
    {
        S1ap_S1SetupRequestIEs_t *s1SetupRequest_p;
        eNB_description_t *eNB_association;
        uint32_t eNB_id = 0;
        char *eNB_name = NULL;
        int ta_ret;
        uint16_t max_enb_connected;

        DevAssert(message != NULL);

        s1SetupRequest_p = &message->msg.s1ap_S1SetupRequestIEs;

        /* We received a new valid S1 Setup Request on a stream != 0.
         * This should not happen -> reject eNB s1 setup request.
         */
        if (stream != 0) {
            S1AP_ERROR("Received new s1 setup request on stream != 0\n");
            /* Send a s1 setup failure with protocol cause unspecified */
            return s1ap_mme_generate_s1_setup_failure(assoc_id, S1ap_Cause_PR_protocol,
                                                      S1ap_CauseProtocol_unspecified, -1);
        }

        S1AP_DEBUG("New s1 setup request incoming from ");
        if (s1SetupRequest_p->presenceMask & S1AP_S1SETUPREQUESTIES_ENBNAME_PRESENT)
        {
            S1AP_DEBUG("%*s ", s1SetupRequest_p->eNBname.size, s1SetupRequest_p->eNBname.buf);
            eNB_name = (char *)s1SetupRequest_p->eNBname.buf;
        }
        if (s1SetupRequest_p->global_ENB_ID.eNB_ID.present == S1ap_ENB_ID_PR_homeENB_ID) {
            // Home eNB ID = 28 bits
            uint8_t *eNB_id_buf =
                s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.homeENB_ID.buf;
            if (s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID.size != 28) {
                //TODO: handle case were size != 28 -> notify ? reject ?
            }
            eNB_id = (eNB_id_buf[0] << 20) + (eNB_id_buf[1] << 12) +
                     (eNB_id_buf[2] << 4) + ((eNB_id_buf[3] & 0xf0) >> 4);
            S1AP_DEBUG("home eNB id: %07x\n", eNB_id);
        } else {
            // Macro eNB = 20 bits
            uint8_t *eNB_id_buf =
                s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID.buf;
            if (s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID.size != 20) {
                //TODO: handle case were size != 20 -> notify ? reject ?
            }
            eNB_id = (eNB_id_buf[0] << 12) + (eNB_id_buf[1] << 4) + ((
                         eNB_id_buf[2] & 0xf0) >> 4);
            S1AP_DEBUG("macro eNB id: %05x\n", eNB_id);
        }

        config_read_lock(&mme_config);
        max_enb_connected = mme_config.max_eNBs;
        config_unlock(&mme_config);

        if (nb_eNB_associated == max_enb_connected) {
            S1AP_ERROR("There is too much eNB connected to MME, rejecting the association\n");
            S1AP_DEBUG("Connected = %d, maximum allowed = %d\n", nb_eNB_associated,
                       max_enb_connected);

            /* Send an overload cause... */
            return s1ap_mme_generate_s1_setup_failure(assoc_id, S1ap_Cause_PR_misc,
                                                      S1ap_CauseMisc_control_processing_overload,
                                                      S1ap_TimeToWait_v20s);
        }

        /* If none of the provided PLMNs/TAC match the one configured in MME,
         * the s1 setup should be rejected with a cause set to Unknown PLMN.
         */
        ta_ret = s1ap_mme_compare_ta_lists(&s1SetupRequest_p->supportedTAs);

        /* eNB and MME have no common PLMN */
        if (ta_ret != TA_LIST_RET_OK) {
            S1AP_ERROR("No Common PLMN with eNB, generate_s1_setup_failure\n");
            return s1ap_mme_generate_s1_setup_failure(assoc_id, S1ap_Cause_PR_misc,
                                                      S1ap_CauseMisc_unknown_PLMN,
                                                      S1ap_TimeToWait_v20s);
        }

        S1AP_DEBUG("Adding eNB to the list of served eNBs\n");

        if ((eNB_association = s1ap_is_eNB_id_in_list(eNB_id)) == NULL) {
            /* eNB has not been fount in list of associated eNB,
             * Add it to the tail of list and initialize data */
            if ((eNB_association = s1ap_is_eNB_assoc_id_in_list(assoc_id)) == NULL) {
                /* ?? */
                return -1;
            } else {
                eNB_association->s1_state = S1AP_RESETING;
                eNB_association->eNB_id = eNB_id;
                eNB_association->default_paging_drx = s1SetupRequest_p->defaultPagingDRX;

                if (eNB_name != NULL) {
                    memcpy(eNB_association->eNB_name, s1SetupRequest_p->eNBname.buf,
                        s1SetupRequest_p->eNBname.size);
                    eNB_association->eNB_name[s1SetupRequest_p->eNBname.size] = '\0';
                }
            }
        } else {
            eNB_association->s1_state = S1AP_RESETING;
            /* eNB has been fount in list, consider the s1 setup request as a reset connection,
             * reseting any previous UE state if sctp association is != than the previous one */
            if (eNB_association->sctp_assoc_id != assoc_id) {
                S1ap_S1SetupFailureIEs_t s1SetupFailure;

                memset(&s1SetupFailure, 0, sizeof(s1SetupFailure));

                /* Send an overload cause... */
                s1SetupFailure.cause.present = S1ap_Cause_PR_misc; //TODO: send the right cause
                s1SetupFailure.cause.choice.misc = S1ap_CauseMisc_control_processing_overload;
                S1AP_ERROR("Rejeting s1 setup request as eNB id %d is already associated to an active sctp association"
                           "Previous known: %d, new one: %d\n",
                           eNB_id, eNB_association->sctp_assoc_id, assoc_id);
    //             s1ap_mme_encode_s1setupfailure(&s1SetupFailure,
    //                                            receivedMessage->msg.s1ap_sctp_new_msg_ind.assocId);
                return -1;
            }
            /* TODO: call the reset procedure */
        }
        s1ap_dump_eNB(eNB_association);
        return s1ap_generate_s1_setup_response(eNB_association);
    } else {
        /* Can not process the request, MME is not connected to HSS */
        S1AP_ERROR("Rejecting s1 setup request Can not process the request, MME is not connected to HSS\n");
        return s1ap_mme_generate_s1_setup_failure(assoc_id, S1ap_Cause_PR_misc,
                                                  S1ap_CauseMisc_unspecified, -1);
    }
}

static
int s1ap_generate_s1_setup_response(eNB_description_t *eNB_association)
{
    int i;
    int enc_rval = 0;
    S1ap_S1SetupResponseIEs_t *s1_setup_response_p;
    S1ap_ServedGUMMEIsItem_t   servedGUMMEI;
    s1ap_message message;
    uint8_t *buffer;
    uint32_t length;

    DevAssert(eNB_association != NULL);

    // Generating response
    memset(&message, 0, sizeof(s1ap_message));
    memset(&servedGUMMEI, 0, sizeof(servedGUMMEI));

    s1_setup_response_p = &message.msg.s1ap_S1SetupResponseIEs;

    config_read_lock(&mme_config);

    s1_setup_response_p->relativeMMECapacity = mme_config.relative_capacity;

    /* Use the gummei parameters provided by configuration */
    for (i = 0; i < mme_config.gummei.nb_plmns; i++) {
        S1ap_PLMNidentity_t *plmn;

        /* FIXME: free object from list once encoded */
        plmn = calloc(1, sizeof(*plmn));
        MCC_MNC_TO_PLMNID(
            mme_config.gummei.plmn_mcc[i],
            mme_config.gummei.plmn_mnc[i],
            mme_config.gummei.plmn_mnc_len[i],
            plmn);
        ASN_SEQUENCE_ADD(&servedGUMMEI.servedPLMNs.list, plmn);
    }
    for (i = 0; i < mme_config.gummei.nb_mme_gid; i++) {
        S1ap_MME_Group_ID_t *mme_gid;

        /* FIXME: free object from list once encoded */
        mme_gid = calloc(1, sizeof(*mme_gid));
        INT16_TO_OCTET_STRING(mme_config.gummei.mme_gid[i], mme_gid);
        ASN_SEQUENCE_ADD(&servedGUMMEI.servedGroupIDs.list, mme_gid);
    }
    for (i = 0; i < mme_config.gummei.nb_mmec; i++) {
        S1ap_MME_Code_t *mmec;

        /* FIXME: free object from list once encoded */
        mmec = calloc(1, sizeof(*mmec));
        INT8_TO_OCTET_STRING(mme_config.gummei.mmec[i], mmec);
        ASN_SEQUENCE_ADD(&servedGUMMEI.servedMMECs.list, mmec);
    }

    config_unlock(&mme_config);

    /* The MME is only serving E-UTRAN RAT, so the list contains only one element */
    ASN_SEQUENCE_ADD(&s1_setup_response_p->servedGUMMEIs, &servedGUMMEI);

    message.procedureCode = S1ap_ProcedureCode_id_S1Setup;
    message.direction     = S1AP_PDU_PR_successfulOutcome;

    enc_rval = s1ap_mme_encode_pdu(&message, &buffer, &length);
    /* Failed to encode s1 setup response... */
    if (enc_rval < 0) {
        S1AP_DEBUG("Removed eNB %d\n", eNB_association->sctp_assoc_id);
        s1ap_remove_eNB(eNB_association);
    } else {
        /* Consider the response as sent. S1AP is ready to accept UE contexts */
        eNB_association->s1_state = S1AP_READY;
    }

    /* Non-UE signalling -> stream 0 */
    return s1ap_mme_itti_send_sctp_request(buffer, length, eNB_association->sctp_assoc_id,
                                            0);
}

int s1ap_mme_handle_ue_cap_indication(uint32_t assoc_id, uint32_t stream,
                                      struct s1ap_message_s *message)
{
    ue_description_t *ue_ref;
    S1ap_UECapabilityInfoIndicationIEs_t *ue_cap_p;

    DevAssert(message != NULL);

    ue_cap_p = &message->msg.s1ap_UECapabilityInfoIndicationIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(ue_cap_p->mme_ue_s1ap_id)) == NULL) {
        S1AP_DEBUG("No UE is attached to this mme UE s1ap id: 0x%08x\n",
                   (uint32_t)ue_cap_p->mme_ue_s1ap_id);
        return -1;
    }
    if (ue_ref->eNB_ue_s1ap_id != ue_cap_p->eNB_UE_S1AP_ID) {
        S1AP_DEBUG("Mismatch in eNB UE S1AP ID, known: 0x%06x, received: 0x%06x\n",
                   ue_ref->eNB_ue_s1ap_id, (uint32_t)ue_cap_p->eNB_UE_S1AP_ID);
        return -1;
    }

    /* Just display a warning when message received over wrong stream */
    if (ue_ref->sctp_stream_recv != stream) {
        S1AP_ERROR("Received ue capability indication for "
        "(MME UE S1AP ID/eNB UE S1AP ID) (0x%08x/0x%06x) over wrong stream "
        "expecting %u, received on %u\n",
        (uint32_t)ue_cap_p->mme_ue_s1ap_id, ue_ref->eNB_ue_s1ap_id,
        ue_ref->sctp_stream_recv, stream);
    }

    /* Forward the ue capabilities to MME application layer */
    {
        MessageDef        *message_p;
        s1ap_ue_cap_ind_t *ue_cap_ind_p;

        message_p = itti_alloc_new_message(TASK_S1AP, S1AP_UE_CAPABILITIES_IND);

        DevAssert(message_p != NULL);

        ue_cap_ind_p = &message_p->ittiMsg.s1ap_ue_cap_ind;
        ue_cap_ind_p->eNB_ue_s1ap_id = ue_ref->eNB_ue_s1ap_id;
        ue_cap_ind_p->mme_ue_s1ap_id = ue_ref->mme_ue_s1ap_id;

        DevCheck(ue_cap_p->ueRadioCapability.size < 100,
                 100, ue_cap_p->ueRadioCapability.size, 0);

        memcpy(ue_cap_ind_p->radio_capabilities, ue_cap_p->ueRadioCapability.buf,
               ue_cap_p->ueRadioCapability.size);

        ue_cap_ind_p->radio_capabilities_length = ue_cap_p->ueRadioCapability.size;

        return itti_send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//******************* Context Management procedures **************************//
////////////////////////////////////////////////////////////////////////////////

int s1ap_mme_handle_initial_context_setup_response(
    uint32_t assoc_id,
    uint32_t stream,
    struct s1ap_message_s *message)
{

    S1ap_InitialContextSetupResponseIEs_t *initialContextSetupResponseIEs_p = NULL;
    S1ap_E_RABSetupItemCtxtSURes_t        *eRABSetupItemCtxtSURes_p         = NULL;
    ue_description_t                      *ue_ref                           = NULL;
    MessageDef                            *message_p                        = NULL;

    initialContextSetupResponseIEs_p = &message->msg.s1ap_InitialContextSetupResponseIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(
        (uint32_t)initialContextSetupResponseIEs_p->mme_ue_s1ap_id)) == NULL) {
        S1AP_DEBUG("No UE is attached to this mme UE s1ap id: 0x%08x %u(10)\n",
                   (uint32_t)initialContextSetupResponseIEs_p->mme_ue_s1ap_id,
                   (uint32_t)initialContextSetupResponseIEs_p->mme_ue_s1ap_id);
        return -1;
    }
    if (ue_ref->eNB_ue_s1ap_id !=
            initialContextSetupResponseIEs_p->eNB_UE_S1AP_ID) {
        S1AP_DEBUG("Mismatch in eNB UE S1AP ID, known: 0x%06x %u(10), received: 0x%06x %u(10)\n",
                ue_ref->eNB_ue_s1ap_id,
                ue_ref->eNB_ue_s1ap_id,
                (uint32_t)initialContextSetupResponseIEs_p->eNB_UE_S1AP_ID,
                (uint32_t)initialContextSetupResponseIEs_p->eNB_UE_S1AP_ID);
        return -1;
    }

    if (initialContextSetupResponseIEs_p->e_RABSetupListCtxtSURes.s1ap_E_RABSetupItemCtxtSURes.count
            != 1) {
        S1AP_DEBUG("E-RAB creation has failed\n");
        return -1;
    }

    ue_ref->s1_ue_state = S1AP_UE_CONNECTED;

    message_p = itti_alloc_new_message(TASK_S1AP, MME_APP_INITIAL_CONTEXT_SETUP_RSP);

    AssertFatal(message_p != NULL,"itti_alloc_new_message Failed");
    memset((void*)&message_p->ittiMsg.mme_app_initial_context_setup_rsp,
            0,
            sizeof(mme_app_initial_context_setup_rsp_t));

    /* Bad, very bad cast... */
    eRABSetupItemCtxtSURes_p = (S1ap_E_RABSetupItemCtxtSURes_t *)
    initialContextSetupResponseIEs_p->e_RABSetupListCtxtSURes.s1ap_E_RABSetupItemCtxtSURes.array[0];


    MME_APP_INITIAL_CONTEXT_SETUP_RSP(message_p).mme_ue_s1ap_id                      = ue_ref->mme_ue_s1ap_id;
    MME_APP_INITIAL_CONTEXT_SETUP_RSP(message_p).eps_bearer_id                       = eRABSetupItemCtxtSURes_p->e_RAB_ID ;
    MME_APP_INITIAL_CONTEXT_SETUP_RSP(message_p).bearer_s1u_enb_fteid.ipv4           = 1; // TO DO
    MME_APP_INITIAL_CONTEXT_SETUP_RSP(message_p).bearer_s1u_enb_fteid.ipv6           = 0; // TO DO
    MME_APP_INITIAL_CONTEXT_SETUP_RSP(message_p).bearer_s1u_enb_fteid.interface_type = S1_U_ENODEB_GTP_U;
    MME_APP_INITIAL_CONTEXT_SETUP_RSP(message_p).bearer_s1u_enb_fteid.teid           = htonl(*((uint32_t *)eRABSetupItemCtxtSURes_p->gTP_TEID.buf));
    memcpy(&MME_APP_INITIAL_CONTEXT_SETUP_RSP(message_p).bearer_s1u_enb_fteid.ipv4_address,
            eRABSetupItemCtxtSURes_p->transportLayerAddress.buf,
            4);
    return itti_send_msg_to_task(TASK_MME_APP, INSTANCE_DEFAULT, message_p);
}



int s1ap_mme_handle_ue_context_release_request(uint32_t assoc_id,
        uint32_t stream, struct s1ap_message_s *message)
{
    S1ap_UEContextReleaseRequestIEs_t *ueContextReleaseRequest_p;
    ue_description_t *ue_ref = NULL;

    ueContextReleaseRequest_p = &message->msg.s1ap_UEContextReleaseRequestIEs;

    /* The UE context release procedure is initiated if the cause is != than user inactivity.
     * TS36.413 #8.3.2.2.
     */
    if (ueContextReleaseRequest_p->cause.present == S1ap_Cause_PR_radioNetwork) {
        if (ueContextReleaseRequest_p->cause.choice.radioNetwork ==
            S1ap_CauseRadioNetwork_user_inactivity) {
            return -1;
        }
    }
    if ((ue_ref = s1ap_is_ue_mme_id_in_list(
                      ueContextReleaseRequest_p->mme_ue_s1ap_id)) == NULL) {
        /* MME doesn't know the MME UE S1AP ID provided.
         * TODO
         */
        return -1;
    } else {
        if (ue_ref->eNB_ue_s1ap_id == (ueContextReleaseRequest_p->eNB_UE_S1AP_ID &
                                       0x00ffffff)) {
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

static int s1ap_mme_generate_ue_context_release_command(
    ue_description_t *ue_ref)
{
    uint8_t *buffer;
    uint32_t length;

    s1ap_message message;
    S1ap_UEContextReleaseCommandIEs_t *ueContextReleaseCommandIEs_p;

    if (ue_ref == NULL) {
        return -1;
    }
    memset(&message, 0, sizeof(s1ap_message));

    message.procedureCode = S1ap_ProcedureCode_id_UEContextRelease;
    message.direction     = S1AP_PDU_PR_successfulOutcome;

    ueContextReleaseCommandIEs_p = &message.msg.s1ap_UEContextReleaseCommandIEs;

    /* Fill in ID pair */
    ueContextReleaseCommandIEs_p->uE_S1AP_IDs.present = S1ap_UE_S1AP_IDs_PR_uE_S1AP_ID_pair;
    ueContextReleaseCommandIEs_p->uE_S1AP_IDs.choice.uE_S1AP_ID_pair.mME_UE_S1AP_ID =
        ue_ref->mme_ue_s1ap_id;
    ueContextReleaseCommandIEs_p->uE_S1AP_IDs.choice.uE_S1AP_ID_pair.eNB_UE_S1AP_ID =
        ue_ref->eNB_ue_s1ap_id;
    ueContextReleaseCommandIEs_p->uE_S1AP_IDs.choice.uE_S1AP_ID_pair.iE_Extensions =
        NULL;

    ueContextReleaseCommandIEs_p->cause.present = S1ap_Cause_PR_radioNetwork;
    ueContextReleaseCommandIEs_p->cause.choice.radioNetwork =
    S1ap_CauseRadioNetwork_release_due_to_eutran_generated_reason;

    if (s1ap_mme_encode_pdu(&message, &buffer, &length) < 0) {
        return -1;
    }
    return s1ap_mme_itti_send_sctp_request(buffer, length, ue_ref->eNB->sctp_assoc_id,
                                           ue_ref->sctp_stream_send);
}

int s1ap_mme_handle_ue_context_release_complete(uint32_t assoc_id,
        uint32_t stream, struct s1ap_message_s *message)
{
    S1ap_UEContextReleaseCompleteIEs_t *ueContextReleaseComplete_p;
    ue_description_t *ue_ref = NULL;

    ueContextReleaseComplete_p = &message->msg.s1ap_UEContextReleaseCompleteIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(
                      ueContextReleaseComplete_p->mme_ue_s1ap_id)) == NULL) {
        /* MME doesn't know the MME UE S1AP ID provided.
         * TODO
         */
        return -1;
    }
    /* eNB has sent a release complete message. We can safely remove UE context.
     * TODO: inform NAS and remove e-RABS.
     */
    s1ap_remove_ue(ue_ref);
    S1AP_DEBUG("Removed UE %u\n",
               (uint32_t)ueContextReleaseComplete_p->mme_ue_s1ap_id);
    return 0;
}

int s1ap_mme_handle_initial_context_setup_failure(uint32_t assoc_id,
        uint32_t stream, struct s1ap_message_s *message)
{
    S1ap_InitialContextSetupFailureIEs_t *initialContextSetupFailureIEs_p;

    ue_description_t *ue_ref = NULL;

    initialContextSetupFailureIEs_p = &message->msg.s1ap_InitialContextSetupFailureIEs;

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(
        initialContextSetupFailureIEs_p->mme_ue_s1ap_id)) == NULL)
    {
        /* MME doesn't know the MME UE S1AP ID provided. */
        return -1;
    }
    if (ue_ref->eNB_ue_s1ap_id != (initialContextSetupFailureIEs_p->eNB_UE_S1AP_ID
                                   & 0x00ffffff)) {
        return -1;
    }
    S1AP_DEBUG("Removed UE %u\n",
               (uint32_t)initialContextSetupFailureIEs_p->mme_ue_s1ap_id);
    s1ap_remove_ue(ue_ref);
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//************************ Handover signalling *******************************//
////////////////////////////////////////////////////////////////////////////////

int s1ap_mme_handle_path_switch_request(uint32_t assoc_id, uint32_t stream,
                                        struct s1ap_message_s *message)
{
    S1ap_PathSwitchRequestIEs_t *pathSwitchRequest_p;
    ue_description_t *ue_ref;
    uint32_t          eNB_ue_s1ap_id;

    pathSwitchRequest_p = &message->msg.s1ap_PathSwitchRequestIEs;

    // eNB UE S1AP ID is limited to 24 bits
    eNB_ue_s1ap_id = (uint32_t)(pathSwitchRequest_p->eNB_UE_S1AP_ID & 0x00ffffff);

    S1AP_DEBUG("Path Switch Request message received from eNB UE S1AP ID: %d\n",
               (int)eNB_ue_s1ap_id);

    if ((ue_ref = s1ap_is_ue_mme_id_in_list(
        pathSwitchRequest_p->sourceMME_UE_S1AP_ID)) == NULL)
    {
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

int s1ap_handle_sctp_deconnection(uint32_t assoc_id)
{
    int current_ue_index = 0;
    int handled_ues      = 0;
    int i;

    MessageDef *message_p   = NULL;

    ue_description_t  *ue_ref = NULL;
    eNB_description_t *eNB_association;

    /* Checking that the assoc id has a valid eNB attached to. */
    eNB_association = s1ap_is_eNB_assoc_id_in_list(assoc_id);
    if (eNB_association == NULL) {
        S1AP_ERROR("No eNB attached to this assoc_id: %d\n",
                   assoc_id);
        return -1;
    }

    STAILQ_FOREACH(ue_ref, &eNB_association->ue_list_head, ue_entries)
    {
        /* Ask for a release of each UE context associated to the eNB */
        if (current_ue_index == 0) {
            message_p = itti_alloc_new_message(TASK_S1AP, S1AP_ENB_DEREGISTERED_IND);
        }

        S1AP_ENB_DEREGISTERED_IND(message_p).mme_ue_s1ap_id[current_ue_index] = ue_ref->mme_ue_s1ap_id;

        if (current_ue_index == 0 && handled_ues > 0) {
            S1AP_ENB_DEREGISTERED_IND(message_p).nb_ue_to_deregister = S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE;
            itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
        }

        handled_ues++;
        current_ue_index = handled_ues % S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE;
    }

    if ((handled_ues % S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE) != 0) {
        S1AP_ENB_DEREGISTERED_IND(message_p).nb_ue_to_deregister = current_ue_index;
        for (i = current_ue_index; i < S1AP_ITTI_UE_PER_DEREGISTER_MESSAGE; i++) {
            S1AP_ENB_DEREGISTERED_IND(message_p).mme_ue_s1ap_id[current_ue_index] = 0;
        }
        itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
    }

    s1ap_remove_eNB(eNB_association);
    s1ap_dump_eNB_list();

    S1AP_DEBUG("Removed eNB attached to assoc_id: %d\n",
               assoc_id);
    return 0;
}

int s1ap_handle_new_association(sctp_new_peer_t *sctp_new_peer_p)
{
    eNB_description_t *eNB_association;

    DevAssert(sctp_new_peer_p != NULL);

    /* Checking that the assoc id has a valid eNB attached to. */
    if ((eNB_association = s1ap_is_eNB_assoc_id_in_list(sctp_new_peer_p->assoc_id)) == NULL) {
        S1AP_DEBUG("Create eNB context for assoc_id: %d\n",
                   sctp_new_peer_p->assoc_id);
        /* Create new context */
        eNB_association = s1ap_new_eNB();
        if (eNB_association == NULL) {
            /* We failed to allocate memory */
            /* TODO: send reject there */
            S1AP_ERROR("Failed to allocate eNB context for assoc_id: %d\n",
                       sctp_new_peer_p->assoc_id);
        }
    } else {
        S1AP_DEBUG("eNB context already exists for assoc_id: %d, update it\n",
                   sctp_new_peer_p->assoc_id);
    }

    eNB_association->sctp_assoc_id = sctp_new_peer_p->assoc_id;

    /* Fill in in and out number of streams available on SCTP connection. */
    eNB_association->instreams     = sctp_new_peer_p->instreams;
    eNB_association->outstreams    = sctp_new_peer_p->outstreams;

    /* initialize the next sctp stream to 1 as 0 is reserved for non
     * ue associated signalling.
     */
    eNB_association->next_sctp_stream = 1;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//************************* E-RAB management *********************************//
////////////////////////////////////////////////////////////////////////////////

// NOT CALLED
int s1ap_handle_create_session_response(SgwCreateSessionResponse
                                        *session_response_p)
{
    /* We received create session response from S-GW on S11 interface abstraction.
     * At least one bearer has been established. We can now send s1ap initial context setup request
     * message to eNB.
     */
    char supportedAlgorithms[] = { 0x02, 0xa0 };
    char securityKey[] = { 0xfd, 0x23, 0xad, 0x22, 0xd0, 0x21, 0x02, 0x90, 0x19, 0xed,
                           0xcf, 0xc9, 0x78, 0x44, 0xba, 0xbb, 0x34, 0x6e, 0xff, 0x89,
                           0x1c, 0x3a, 0x56, 0xf0, 0x81, 0x34, 0xdd, 0xee, 0x19, 0x55,
                           0xf2, 0x1f
                         };

    ue_description_t *ue_ref = NULL;
    s1ap_message message;
    uint8_t *buffer_p;
    uint32_t length;

    S1ap_InitialContextSetupRequestIEs_t *initialContextSetupRequest_p;
    S1ap_E_RABToBeSetupItemCtxtSUReq_t    e_RABToBeSetup;

    AssertFatal(0, "Not called anymore");
    DevAssert(session_response_p != NULL);

    DevCheck(session_response_p->bearer_context_created.cause == REQUEST_ACCEPTED,
             REQUEST_ACCEPTED, session_response_p->bearer_context_created.cause, 0);

    if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 0) &&
        (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 0))
    {

        S1AP_ERROR("No IP address provided for transport layer address\n"
                   "         -->Sending Intial context setup failure\n");
        return -1;
    }

    if ((ue_ref = s1ap_is_teid_in_list(session_response_p->s11_sgw_teid.teid)) == NULL) {
        S1AP_DEBUG("[%d] is not attached to any UE context\n",
                   session_response_p->s11_sgw_teid.teid);
        return -1;
    }

    memset(&message, 0, sizeof(s1ap_message));
    memset(&e_RABToBeSetup, 0, sizeof(S1ap_E_RABToBeSetupItemCtxtSUReq_t));

    message.procedureCode = S1ap_ProcedureCode_id_InitialContextSetup;
    message.direction     = S1AP_PDU_PR_initiatingMessage;

    initialContextSetupRequest_p = &message.msg.s1ap_InitialContextSetupRequestIEs;

    initialContextSetupRequest_p->mme_ue_s1ap_id = ue_ref->mme_ue_s1ap_id;
    initialContextSetupRequest_p->eNB_UE_S1AP_ID = ue_ref->eNB_ue_s1ap_id;

    /* uEaggregateMaximumBitrateDL and uEaggregateMaximumBitrateUL expressed in term of bits/sec */
//     asn_int642INTEGER(
//         &initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL,
//         100000000UL);
//     asn_int642INTEGER(
//         &initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL,
//         50000000UL);

    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL = 100000000UL;
    initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL = 50000000UL;

    e_RABToBeSetup.e_RAB_ID =
        session_response_p->bearer_context_created.eps_bearer_id; /* ??? */
    e_RABToBeSetup.e_RABlevelQoSParameters.qCI = 0; // ??
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel
        = 15; //No priority
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability
    = S1ap_Pre_emptionCapability_shall_not_trigger_pre_emption;
    e_RABToBeSetup.e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability
    = S1ap_Pre_emptionVulnerability_not_pre_emptable;

//     e_RABToBeSetup.gTP_TEID.buf  = calloc(4, sizeof(uint8_t));
//     INT32_TO_BUFFER(session_response_p->bearer_context_created.s1u_sgw_fteid.teid,
//                     e_RABToBeSetup.gTP_TEID.buf);
//     e_RABToBeSetup.gTP_TEID.size = 4;
        DevCheck(session_response_p->bearer_context_created.s1u_sgw_fteid.teid != 0,
                 session_response_p->bearer_context_created.s1u_sgw_fteid.teid, 0, 0);
    INT32_TO_OCTET_STRING(
        session_response_p->bearer_context_created.s1u_sgw_fteid.teid,
        &e_RABToBeSetup.gTP_TEID);
    if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 1) &&
            (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 0)) {
        /* Only IPv4 supported */
        INT32_TO_BIT_STRING(
            session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address,
            &e_RABToBeSetup.transportLayerAddress);
    } else if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 0) &&
               (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 1)) {
        /* Only IPv6 supported */
        e_RABToBeSetup.transportLayerAddress.buf = calloc(16, sizeof(uint8_t));
        memcpy(e_RABToBeSetup.transportLayerAddress.buf,
               (void *)&session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6_address,
               16);
        e_RABToBeSetup.transportLayerAddress.size = 16;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
    } else if ((session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 == 1) &&
               (session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6 == 1)) {
        /* Both IPv4 and IPv6 supported */
        e_RABToBeSetup.transportLayerAddress.buf = calloc(20, sizeof(uint8_t));
        e_RABToBeSetup.transportLayerAddress.size = 20;
        e_RABToBeSetup.transportLayerAddress.bits_unused = 0;
        memcpy(e_RABToBeSetup.transportLayerAddress.buf,
               &session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address, 4);
        memcpy(e_RABToBeSetup.transportLayerAddress.buf + 4,
               session_response_p->bearer_context_created.s1u_sgw_fteid.ipv6_address, 16);
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

    initialContextSetupRequest_p->securityKey.buf = (uint8_t *)
            securityKey; /* 256 bits length */
    initialContextSetupRequest_p->securityKey.size = 32;
    initialContextSetupRequest_p->securityKey.bits_unused = 0;

    if (s1ap_mme_encode_pdu(&message, &buffer_p, &length) < 0) {
        return -1;
    }

    return s1ap_mme_itti_send_sctp_request(buffer_p, length, ue_ref->eNB->sctp_assoc_id,
                                           ue_ref->sctp_stream_send);

//     return s1ap_mme_encode_initial_context_setup_request(&initialContextSetupRequest,
//             ue_ref);
}
