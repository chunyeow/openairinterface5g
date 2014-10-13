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

/*! \file s1ap_eNB_handlers.c
 * \brief s1ap messages handlers for eNB part
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

#include <stdint.h>

#include "intertask_interface.h"

#include "asn1_conversions.h"

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"
// #include "s1ap_eNB.h"
#include "s1ap_eNB_defs.h"
#include "s1ap_eNB_handlers.h"
#include "s1ap_eNB_decoder.h"

#include "s1ap_eNB_ue_context.h"
#include "s1ap_eNB_trace.h"
#include "s1ap_eNB_nas_procedures.h"
#include "s1ap_eNB_management_procedures.h"

#include "s1ap_eNB_default_values.h"

#include "assertions.h"
#include "conversions.h"

static
int s1ap_eNB_handle_s1_setup_response(uint32_t               assoc_id,
                                      uint32_t               stream,
                                      struct s1ap_message_s *message_p);
static
int s1ap_eNB_handle_s1_setup_failure(uint32_t               assoc_id,
                                     uint32_t               stream,
                                     struct s1ap_message_s *message_p);

static
int s1ap_eNB_handle_initial_context_request(uint32_t               assoc_id,
                                            uint32_t               stream,
                                            struct s1ap_message_s *message_p);

static
int s1ap_eNB_handle_ue_context_release(uint32_t               assoc_id,
                                            uint32_t               stream,
                                            struct s1ap_message_s *s1ap_message_p);

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
    { s1ap_eNB_handle_initial_context_request, 0, 0 }, /* InitialContextSetup */
    { 0, 0, 0 }, /* Paging */
    { s1ap_eNB_handle_nas_downlink, 0, 0 }, /* downlinkNASTransport */
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
    { 0, 0, 0 }, /* UECapabilityInfoIndication */
    { s1ap_eNB_handle_ue_context_release, 0, 0 }, /* UEContextRelease */
    { 0, 0, 0 }, /* eNBStatusTransfer */
    { 0, 0, 0 }, /* MMEStatusTransfer */
    { s1ap_eNB_handle_deactivate_trace, 0, 0 }, /* DeactivateTrace */
    { s1ap_eNB_handle_trace_start, 0, 0 }, /* TraceStart */
    { 0, 0, 0 }, /* TraceFailureIndication */
    { 0, 0, 0 }, /* ENBConfigurationUpdate */
    { 0, 0, 0 }, /* MMEConfigurationUpdate */
    { 0, 0, 0 }, /* LocationReportingControl */
    { 0, 0, 0 }, /* LocationReportingFailureIndication */
    { 0, 0, 0 }, /* LocationReport */
    { 0, 0, 0 }, /* OverloadStart */
    { 0, 0, 0 }, /* OverloadStop */
    { 0, 0, 0 }, /* WriteReplaceWarning */
    { 0, 0, 0 }, /* eNBDirectInformationTransfer */
    { 0, 0, 0 }, /* MMEDirectInformationTransfer */
    { 0, 0, 0 }, /* PrivateMessage */
    { 0, 0, 0 }, /* eNBConfigurationTransfer */
    { 0, 0, 0 }, /* MMEConfigurationTransfer */
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

void s1ap_handle_s1_setup_message(s1ap_eNB_mme_data_t *mme_desc_p, int sctp_shutdown) {
    if (sctp_shutdown) {
        /* A previously connected MME has been shutdown */

        /* TODO check if it was used by some eNB and send a message to inform these eNB if there is no more associated MME */
        if (mme_desc_p->state == S1AP_ENB_STATE_CONNECTED)
        {
            mme_desc_p->state = S1AP_ENB_STATE_DISCONNECTED;

            if (mme_desc_p->s1ap_eNB_instance->s1ap_mme_associated_nb > 0) {
                /* Decrease associated MME number */
                mme_desc_p->s1ap_eNB_instance->s1ap_mme_associated_nb --;
            }

            /* If there are no more associated MME, inform eNB app */
            if (mme_desc_p->s1ap_eNB_instance->s1ap_mme_associated_nb == 0) {
              MessageDef                 *message_p;

              message_p = itti_alloc_new_message(TASK_S1AP, S1AP_DEREGISTERED_ENB_IND);
              S1AP_DEREGISTERED_ENB_IND(message_p).nb_mme = 0;
              itti_send_msg_to_task(TASK_ENB_APP, mme_desc_p->s1ap_eNB_instance->instance, message_p);
            }
        }
    } else {
        /* Check that at least one setup message is pending */
        DevCheck(mme_desc_p->s1ap_eNB_instance->s1ap_mme_pending_nb > 0, mme_desc_p->s1ap_eNB_instance->instance,
                 mme_desc_p->s1ap_eNB_instance->s1ap_mme_pending_nb, 0);

        if (mme_desc_p->s1ap_eNB_instance->s1ap_mme_pending_nb > 0) {
            /* Decrease pending messages number */
            mme_desc_p->s1ap_eNB_instance->s1ap_mme_pending_nb --;
        }

        /* If there are no more pending messages, inform eNB app */
        if (mme_desc_p->s1ap_eNB_instance->s1ap_mme_pending_nb == 0) {
          MessageDef                 *message_p;

          message_p = itti_alloc_new_message(TASK_S1AP, S1AP_REGISTER_ENB_CNF);
          S1AP_REGISTER_ENB_CNF(message_p).nb_mme = mme_desc_p->s1ap_eNB_instance->s1ap_mme_associated_nb;
          itti_send_msg_to_task(TASK_ENB_APP, mme_desc_p->s1ap_eNB_instance->instance, message_p);
        }
    }
}

int s1ap_eNB_handle_message(uint32_t assoc_id, int32_t stream,
                            const uint8_t * const data, const uint32_t data_length)
{
    struct s1ap_message_s message;

    DevAssert(data != NULL);

    memset(&message, 0, sizeof(struct s1ap_message_s));

    if (s1ap_eNB_decode_pdu(&message, data, data_length) < 0) {
        S1AP_ERROR("Failed to decode PDU\n");
        return -1;
    }
    /* Checking procedure Code and direction of message */
    if (message.procedureCode > sizeof(messages_callback) / (3 * sizeof(
                s1ap_message_decoded_callback))
            || (message.direction > S1AP_PDU_PR_unsuccessfulOutcome)) {
        S1AP_ERROR("[SCTP %d] Either procedureCode %d or direction %d exceed expected\n",
                   assoc_id, message.procedureCode, message.direction);
        return -1;
    }
    /* No handler present.
     * This can mean not implemented or no procedure for eNB (wrong direction).
     */
    if (messages_callback[message.procedureCode][message.direction-1] == NULL) {
        S1AP_ERROR("[SCTP %d] No handler for procedureCode %d in %s\n",
                   assoc_id, message.procedureCode,
                   direction2String[message.direction]);
        return -1;
    }

    /* Calling the right handler */
    return (*messages_callback[message.procedureCode][message.direction-1])
        (assoc_id, stream, &message);
}

static
int s1ap_eNB_handle_s1_setup_failure(uint32_t               assoc_id,
                                     uint32_t               stream,
                                     struct s1ap_message_s *message_p)
{
    S1ap_S1SetupFailureIEs_t   *s1_setup_failure_p;
    s1ap_eNB_mme_data_t        *mme_desc_p;

    DevAssert(message_p != NULL);

    s1_setup_failure_p = &message_p->msg.s1ap_S1SetupFailureIEs;

    /* S1 Setup Failure == Non UE-related procedure -> stream 0 */
    if (stream != 0) {
        S1AP_WARN("[SCTP %d] Received s1 setup failure on stream != 0 (%d)\n",
                  assoc_id, stream);
    }

    if ((mme_desc_p = s1ap_eNB_get_MME(NULL, assoc_id, 0)) == NULL) {
        S1AP_ERROR("[SCTP %d] Received S1 setup response for non existing "
                   "MME context\n", assoc_id);
        return -1;
    }

    if ((s1_setup_failure_p->cause.present == S1ap_Cause_PR_misc) &&
        (s1_setup_failure_p->cause.choice.misc == S1ap_CauseMisc_unspecified)) {
        S1AP_WARN("Received s1 setup failure for MME... MME is not ready\n");
    } else {
        S1AP_ERROR("Received s1 setup failure for MME... please check your parameters\n");
    }

    mme_desc_p->state = S1AP_ENB_STATE_WAITING;
    s1ap_handle_s1_setup_message(mme_desc_p, 0);

    return 0;
}

static
int s1ap_eNB_handle_s1_setup_response(uint32_t               assoc_id,
                                      uint32_t               stream,
                                      struct s1ap_message_s *message_p)
{
    S1ap_S1SetupResponseIEs_t *s1SetupResponse_p;
    s1ap_eNB_mme_data_t       *mme_desc_p;
    int i;

    DevAssert(message_p != NULL);

    s1SetupResponse_p = &message_p->msg.s1ap_S1SetupResponseIEs;

    /* S1 Setup Response == Non UE-related procedure -> stream 0 */
    if (stream != 0) {
        S1AP_ERROR("[SCTP %d] Received s1 setup response on stream != 0 (%d)\n",
                   assoc_id, stream);
        return -1;
    }

    if ((mme_desc_p = s1ap_eNB_get_MME(NULL, assoc_id, 0)) == NULL) {
        S1AP_ERROR("[SCTP %d] Received S1 setup response for non existing "
                   "MME context\n", assoc_id);
        return -1;
    }

    /* The list of served gummei can contain at most 8 elements.
     * LTE related gummei is the first element in the list, i.e with an id of 0.
     */
    DevAssert(s1SetupResponse_p->servedGUMMEIs.list.count == 1);

    for (i = 0; i < s1SetupResponse_p->servedGUMMEIs.list.count; i++) {
        struct S1ap_ServedGUMMEIsItem *gummei_item_p;
        struct served_gummei_s        *new_gummei_p;
        int j;

        gummei_item_p = (struct S1ap_ServedGUMMEIsItem *)
                        s1SetupResponse_p->servedGUMMEIs.list.array[i];
        new_gummei_p = calloc(1, sizeof(struct served_gummei_s));

        STAILQ_INIT(&new_gummei_p->served_plmns);
        STAILQ_INIT(&new_gummei_p->served_group_ids);
        STAILQ_INIT(&new_gummei_p->mme_codes);

        for (j = 0; j < gummei_item_p->servedPLMNs.list.count; j++) {
            S1ap_PLMNidentity_t *plmn_identity_p;
            struct plmn_identity_s *new_plmn_identity_p;

            plmn_identity_p = gummei_item_p->servedPLMNs.list.array[i];
            new_plmn_identity_p = calloc(1, sizeof(struct plmn_identity_s));
            TBCD_TO_MCC_MNC(plmn_identity_p, new_plmn_identity_p->mcc,
                            new_plmn_identity_p->mnc, new_plmn_identity_p->mnc_digit_length);
            STAILQ_INSERT_TAIL(&new_gummei_p->served_plmns, new_plmn_identity_p, next);
            new_gummei_p->nb_served_plmns++;
        }
        for (j = 0; j < gummei_item_p->servedGroupIDs.list.count; j++) {
            S1ap_MME_Group_ID_t           *mme_group_id_p;
            struct served_group_id_s *new_group_id_p;

            mme_group_id_p = gummei_item_p->servedGroupIDs.list.array[i];
            new_group_id_p = calloc(1, sizeof(struct served_group_id_s));
            OCTET_STRING_TO_INT16(mme_group_id_p, new_group_id_p->mme_group_id);
            STAILQ_INSERT_TAIL(&new_gummei_p->served_group_ids, new_group_id_p, next);
            new_gummei_p->nb_group_id++;
        }
        for (j = 0; j < gummei_item_p->servedMMECs.list.count; j++) {
            S1ap_MME_Code_t        *mme_code_p;
            struct mme_code_s *new_mme_code_p;

            mme_code_p = gummei_item_p->servedMMECs.list.array[i];
            new_mme_code_p = calloc(1, sizeof(struct mme_code_s));

            OCTET_STRING_TO_INT8(mme_code_p, new_mme_code_p->mme_code);
            STAILQ_INSERT_TAIL(&new_gummei_p->mme_codes, new_mme_code_p, next);
            new_gummei_p->nb_mme_code++;
        }
        STAILQ_INSERT_TAIL(&mme_desc_p->served_gummei, new_gummei_p, next);
    }
    /* Free contents of the list */
    ASN_STRUCT_FREE_CONTENTS_ONLY(asn_DEF_S1ap_ServedGUMMEIs,
                                  (void *)&s1SetupResponse_p->servedGUMMEIs);
    /* Set the capacity of this MME */
    mme_desc_p->relative_mme_capacity = s1SetupResponse_p->relativeMMECapacity;
    /* Optionaly set the mme name */
    if (s1SetupResponse_p->presenceMask & S1AP_S1SETUPRESPONSEIES_MMENAME_PRESENT) {
        mme_desc_p->mme_name = calloc(s1SetupResponse_p->mmEname.size + 1, sizeof(char));
        memcpy(mme_desc_p->mme_name, s1SetupResponse_p->mmEname.buf,
               s1SetupResponse_p->mmEname.size);
        /* Convert the mme name to a printable string */
        mme_desc_p->mme_name[s1SetupResponse_p->mmEname.size] = '\0';
    }
    /* The association is now ready as eNB and MME know parameters of each other.
     * Mark the association as UP to enable UE contexts creation.
     */
    mme_desc_p->state = S1AP_ENB_STATE_CONNECTED;
    mme_desc_p->s1ap_eNB_instance->s1ap_mme_associated_nb ++;
    s1ap_handle_s1_setup_message(mme_desc_p, 0);

#if 0
    /* We call back our self
     * -> generate a dummy initial UE message
     */
    {
        s1ap_nas_first_req_t s1ap_nas_first_req;

        memset(&s1ap_nas_first_req, 0, sizeof(s1ap_nas_first_req_t));

        s1ap_nas_first_req.rnti = 0xC03A;
        s1ap_nas_first_req.establishment_cause = RRC_CAUSE_MO_DATA;
        s1ap_nas_first_req.ue_identity.presenceMask = UE_IDENTITIES_gummei;

        s1ap_nas_first_req.ue_identity.gummei.mcc = 208;
        s1ap_nas_first_req.ue_identity.gummei.mnc = 34;
        s1ap_nas_first_req.ue_identity.gummei.mme_code = 0;
        s1ap_nas_first_req.ue_identity.gummei.mme_group_id = 0;

        /* NAS Attach request with IMSI */
        static uint8_t nas_attach_req_imsi[] =
        {
            0x07, 0x41,
            /* EPS Mobile identity = IMSI */
            0x71, 0x08, 0x29, 0x80, 0x43, 0x21, 0x43, 0x65, 0x87,
            0xF9,
            /* End of EPS Mobile Identity */
            0x02, 0xE0, 0xE0, 0x00, 0x20, 0x02, 0x03,
            0xD0, 0x11, 0x27, 0x1A, 0x80, 0x80, 0x21, 0x10, 0x01, 0x00, 0x00,
            0x10, 0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0x83, 0x06, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0A, 0x00, 0x52, 0x12, 0xF2,
            0x01, 0x27, 0x11,
        };

        /* NAS Attach request with GUTI */
        static uint8_t nas_attach_req_guti[] =
        {
            0x07, 0x41,
            /* EPS Mobile identity = IMSI */
            0x71, 0x0B, 0xF6, 0x12, 0xF2, 0x01, 0x80, 0x00, 0x01, 0xE0, 0x00,
            0xDA, 0x1F,
            /* End of EPS Mobile Identity */
            0x02, 0xE0, 0xE0, 0x00, 0x20, 0x02, 0x03,
            0xD0, 0x11, 0x27, 0x1A, 0x80, 0x80, 0x21, 0x10, 0x01, 0x00, 0x00,
            0x10, 0x81, 0x06, 0x00, 0x00, 0x00, 0x00, 0x83, 0x06, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x0D, 0x00, 0x00, 0x0A, 0x00, 0x52, 0x12, 0xF2,
            0x01, 0x27, 0x11,
        };

        s1ap_nas_first_req.nas_pdu.buffer = nas_attach_req_guti;
        s1ap_nas_first_req.nas_pdu.length = sizeof(nas_attach_req_guti);

        s1ap_eNB_handle_nas_first_req(mme_desc_p->s1ap_eNB_instance->instance,
                                      &s1ap_nas_first_req);
    }
#endif

    return 0;
}

static
int s1ap_eNB_handle_initial_context_request(uint32_t               assoc_id,
                                            uint32_t               stream,
                                            struct s1ap_message_s *s1ap_message_p)
{
    int i;

    s1ap_eNB_mme_data_t   *mme_desc_p       = NULL;
    s1ap_eNB_ue_context_t *ue_desc_p        = NULL;
    MessageDef            *message_p        = NULL;

    S1ap_InitialContextSetupRequestIEs_t *initialContextSetupRequest_p;
    DevAssert(s1ap_message_p != NULL);

    initialContextSetupRequest_p = &s1ap_message_p->msg.s1ap_InitialContextSetupRequestIEs;

    if ((mme_desc_p = s1ap_eNB_get_MME(NULL, assoc_id, 0)) == NULL) {
        S1AP_ERROR("[SCTP %d] Received initial context setup request for non "
                   "existing MME context\n", assoc_id);
        return -1;
    }
    if ((ue_desc_p = s1ap_eNB_get_ue_context(mme_desc_p->s1ap_eNB_instance,
                     initialContextSetupRequest_p->eNB_UE_S1AP_ID)) == NULL) {
        S1AP_ERROR("[SCTP %d] Received initial context setup request for non "
        "existing UE context 0x%06x\n", assoc_id,
        initialContextSetupRequest_p->eNB_UE_S1AP_ID);
        return -1;
    }

    /* Initial context request = UE-related procedure -> stream != 0 */
    if (stream != ue_desc_p->stream) {
        S1AP_ERROR("[SCTP %d] Received UE-related procedure on stream (%d) whereas expecting (%d)\n",
                   assoc_id, stream, ue_desc_p->stream);
        return -1;
    }

    ue_desc_p->mme_ue_s1ap_id = initialContextSetupRequest_p->mme_ue_s1ap_id;

    message_p        = itti_alloc_new_message(TASK_S1AP, S1AP_INITIAL_CONTEXT_SETUP_REQ);

    S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).ue_initial_id  = ue_desc_p->ue_initial_id;
    ue_desc_p->ue_initial_id = 0;

    S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).eNB_ue_s1ap_id = ue_desc_p->eNB_ue_s1ap_id;
    S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).nb_of_e_rabs =
    initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq.s1ap_E_RABToBeSetupItemCtxtSUReq.count;

    S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).ue_ambr.br_ul = 64;// TO DO(bitrate_t)(initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateUL);
    S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).ue_ambr.br_dl = 1024;//(bitrate_t)(initialContextSetupRequest_p->uEaggregateMaximumBitrate.uEaggregateMaximumBitRateDL);

    S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).security_capabilities.encryption_algorithms =
    BIT_STRING_to_uint16(&initialContextSetupRequest_p->ueSecurityCapabilities.encryptionAlgorithms);
    S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).security_capabilities.integrity_algorithms =
    BIT_STRING_to_uint16(&initialContextSetupRequest_p->ueSecurityCapabilities.integrityProtectionAlgorithms);

    /* Copy the security key */
    memcpy(&S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).security_key,
           initialContextSetupRequest_p->securityKey.buf, initialContextSetupRequest_p->securityKey.size);

    for (i = 0; i < initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq.s1ap_E_RABToBeSetupItemCtxtSUReq.count; i++)
    {
        S1ap_E_RABToBeSetupItemCtxtSUReq_t *item_p;

        item_p = (S1ap_E_RABToBeSetupItemCtxtSUReq_t *)initialContextSetupRequest_p->e_RABToBeSetupListCtxtSUReq.s1ap_E_RABToBeSetupItemCtxtSUReq.array[i];

        S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].e_rab_id = item_p->e_RAB_ID;

        if (item_p->nAS_PDU != NULL) {
            /* Only copy NAS pdu if present */
            S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].nas_pdu.length = item_p->nAS_PDU->size;

            S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].nas_pdu.buffer =
            malloc(sizeof(uint8_t) * item_p->nAS_PDU->size);

            memcpy(S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].nas_pdu.buffer,
                   item_p->nAS_PDU->buf, item_p->nAS_PDU->size);
        } else {
            S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].nas_pdu.length = 0;
            S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].nas_pdu.buffer = NULL;
        }

        /* Set the transport layer address */
        memcpy(S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].sgw_addr.buffer,
               item_p->transportLayerAddress.buf, item_p->transportLayerAddress.size);
        S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].sgw_addr.length =
        item_p->transportLayerAddress.size * 8 - item_p->transportLayerAddress.bits_unused;

        /* GTP tunnel endpoint ID */
        OCTET_STRING_TO_INT32(&item_p->gTP_TEID, S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].gtp_teid);

        /* Set the QOS informations */
        S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].qos.qci = item_p->e_RABlevelQoSParameters.qCI;

        S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].qos.allocation_retention_priority.priority_level =
        item_p->e_RABlevelQoSParameters.allocationRetentionPriority.priorityLevel;
        S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].qos.allocation_retention_priority.pre_emp_capability =
        item_p->e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionCapability;
        S1AP_INITIAL_CONTEXT_SETUP_REQ(message_p).e_rab_param[i].qos.allocation_retention_priority.pre_emp_vulnerability =
        item_p->e_RABlevelQoSParameters.allocationRetentionPriority.pre_emptionVulnerability;
    }

    itti_send_msg_to_task(TASK_RRC_ENB, ue_desc_p->eNB_instance->instance, message_p);

    return 0;
}


static
int s1ap_eNB_handle_ue_context_release(uint32_t               assoc_id,
                                            uint32_t               stream,
                                            struct s1ap_message_s *s1ap_message_p)
{
    int i;

    s1ap_eNB_mme_data_t   *mme_desc_p       = NULL;
    s1ap_eNB_ue_context_t *ue_desc_p        = NULL;
    MessageDef            *message_p        = NULL;

    S1ap_UEContextReleaseCommandIEs_t *ueContextReleaseCommand_p;
    DevAssert(s1ap_message_p != NULL);

    ueContextReleaseCommand_p = &s1ap_message_p->msg.s1ap_UEContextReleaseCommandIEs;

    if ((mme_desc_p = s1ap_eNB_get_MME(NULL, assoc_id, 0)) == NULL) {
        S1AP_ERROR("[SCTP %d] Received UE context release command for non "
                   "existing MME context\n", assoc_id);
        return -1;
    }

#warning "TODO"
    S1AP_ERROR("s1ap_eNB_handle_ue_context_release to be continued\n");
    return 0;
}

