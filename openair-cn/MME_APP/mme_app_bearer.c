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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "intertask_interface.h"
#include "mme_config.h"

#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"

#include "secu_defs.h"

#include "assertions.h"

int mme_app_create_bearer(s6a_update_location_ans_t *ula_p)
{
    uint64_t imsi;
    uint8_t i;
    task_id_t to_task;

    struct ue_context_s *ue_context;

    MessageDef              *message_p;
    SgwCreateSessionRequest *session_request_p;
    /* Keep the identifier to the default APN */
    context_identifier_t        context_identifier;
    struct apn_configuration_s *default_apn;

    DevAssert(ula_p != NULL);

#if defined(DISABLE_STANDALONE_EPC)
    to_task = TASK_S11;
#else
    to_task = TASK_SPGW_APP;
#endif

    if (ula_p->result.present == S6A_RESULT_BASE) {
        if (ula_p->result.choice.base != DIAMETER_SUCCESS) {
            /* The update location procedure has failed. Notify the NAS layer
             * and don't initiate the bearer creation on S-GW side.
             */
            DevMessage("ULR/ULA procedure returned non success\n");
        }
    } else {
        /* The update location procedure has failed. Notify the NAS layer
         * and don't initiate the bearer creation on S-GW side.
         */
        DevMessage("ULR/ULA procedure returned non success\n");
    }

    MME_APP_STRING_TO_IMSI((char *)ula_p->imsi, &imsi);

    MME_APP_DEBUG("Handling imsi %"IMSI_FORMAT"\n", imsi);

    if ((ue_context = mme_ue_context_exists_imsi(&mme_app_desc.mme_ue_contexts, imsi)) == NULL) {
        MME_APP_ERROR("That's embarrassing as we don't know this IMSI\n");
        return -1;
    }

    ue_context->subscription_known = SUBSCRIPTION_KNOWN;

    ue_context->sub_status = ula_p->subscription_data.subscriber_status;
    ue_context->access_restriction_data = ula_p->subscription_data.access_restriction;

    /* Copy the subscribed ambr to the sgw create session request message */
    memcpy(&ue_context->subscribed_ambr, &ula_p->subscription_data.subscribed_ambr,
           sizeof(ambr_t));
    memcpy(ue_context->msisdn, ula_p->subscription_data.msisdn,
           MSISDN_LENGTH);

    ue_context->rau_tau_timer = ula_p->subscription_data.rau_tau_timer;
    ue_context->access_mode   = ula_p->subscription_data.access_mode;
    ue_context->rau_tau_timer = ula_p->subscription_data.rau_tau_timer;

    memcpy(&ue_context->apn_profile, &ula_p->subscription_data.apn_config_profile,
           sizeof(apn_config_profile_t));

//     mme_app_dump_ue_contexts();

    if (ula_p->subscription_data.subscriber_status != SS_SERVICE_GRANTED) {
        /* HSS rejected the bearer creation or roaming is not allowed for this
         * UE. This result will trigger an ESM Failure message sent to UE.
         */
        DevMessage("Not implemented: ACCESS NOT GRANTED, send ESM Failure to NAS\n");
    }

    message_p = itti_alloc_new_message(TASK_MME_APP, SGW_CREATE_SESSION_REQUEST);

    /* WARNING:
     * Some parameters should be provided by NAS Layer:
     * - ue_time_zone
     * - mei
     * - uli
     * - uci
     * Some parameters should be provided by HSS:
     * - PGW address for CP
     * - paa
     * - ambr
     * and by MME Application layer:
     * - selection_mode
     * Set these parameters with random values for now.
     */

    session_request_p = &message_p->ittiMsg.sgwCreateSessionRequest;
    memset(session_request_p, 0, sizeof(SgwCreateSessionRequest));

    /* As the create session request is the first exchanged message and as
     * no tunnel had been previously setup, the distant teid is set to 0.
     * The remote teid will be provided in the response message.
     */
    session_request_p->teid = 0;

    memcpy(session_request_p->imsi.digit, ula_p->imsi,
           ula_p->imsi_length);
    session_request_p->imsi.length = ula_p->imsi_length;

    /* Copy the MSISDN */
    memcpy(session_request_p->msisdn.digit, ula_p->subscription_data.msisdn,
           ula_p->subscription_data.msisdn_length);
    session_request_p->msisdn.length = ula_p->subscription_data.msisdn_length;

    session_request_p->rat_type = RAT_EUTRAN;

    /* Copy the subscribed ambr to the sgw create session request message */
    memcpy(&session_request_p->ambr, &ula_p->subscription_data.subscribed_ambr,
           sizeof(ambr_t));

    if (ula_p->subscription_data.apn_config_profile.nb_apns == 0) {
        DevMessage("No APN returned by the HSS");
    }

    context_identifier = ula_p->subscription_data.apn_config_profile.context_identifier;
    for (i = 0; i < ula_p->subscription_data.apn_config_profile.nb_apns; i++) {
        default_apn = &ula_p->subscription_data.apn_config_profile.apn_configuration[i];
        /* OK we got our default APN */
        if (default_apn->context_identifier == context_identifier)
            break;
    }

    if (!default_apn) {
        /* Unfortunately we didn't find our default APN... */
        DevMessage("No default APN found");
    }

    memcpy(&session_request_p->bearer_to_create.bearer_level_qos.gbr,
           &default_apn->ambr, sizeof(ambr_t));
    memcpy(&session_request_p->bearer_to_create.bearer_level_qos.mbr,
           &default_apn->ambr, sizeof(ambr_t));

    session_request_p->bearer_to_create.bearer_level_qos.qci =
    default_apn->subscribed_qos.qci;

    session_request_p->bearer_to_create.bearer_level_qos.pvi =
    default_apn->subscribed_qos.allocation_retention_priority.pre_emp_vulnerability;
    session_request_p->bearer_to_create.bearer_level_qos.pci =
    default_apn->subscribed_qos.allocation_retention_priority.pre_emp_capability;
    session_request_p->bearer_to_create.bearer_level_qos.pl =
    default_apn->subscribed_qos.allocation_retention_priority.priority_level;

    /* Asking for default bearer in initial UE message.
     * Use the address of ue_context as unique TEID: Need to find better here
     * and will generate unique id only for 32 bits platforms.
     */
    session_request_p->sender_fteid_for_cp.teid = (uint32_t)ue_context;
    session_request_p->sender_fteid_for_cp.interface_type = S11_MME_GTP_C;
    session_request_p->bearer_to_create.eps_bearer_id = 5;

    ue_context->mme_s11_teid = session_request_p->sender_fteid_for_cp.teid;
    ue_context->sgw_s11_teid = 0;

    memcpy(session_request_p->apn, default_apn->service_selection,
           default_apn->service_selection_length);

    /* Set PDN type for pdn_type and PAA even if this IE is redundant */
    session_request_p->pdn_type = default_apn->pdn_type;
    session_request_p->paa.pdn_type = default_apn->pdn_type;
    if (default_apn->nb_ip_address == 0) {
        /* UE DHCPv4 allocated ip address */
        memset(session_request_p->paa.ipv4_address, 0, 4);
        memset(session_request_p->paa.ipv6_address, 0, 16);
    } else {
        uint8_t j;

        for (j = 0; j < default_apn->nb_ip_address; j++) {
            ip_address_t *ip_address;
            ip_address = &default_apn->ip_address[j];
            if (ip_address->pdn_type == IPv4) {
                memcpy(session_request_p->paa.ipv4_address, ip_address->address.ipv4_address, 4);
            } else if (ip_address->pdn_type == IPv6) {
                memcpy(session_request_p->paa.ipv6_address, ip_address->address.ipv6_address, 16);
            }
//             free(ip_address);
        }
    }

    config_read_lock(&mme_config);
    session_request_p->peer_ip = mme_config.ipv4.sgw_ip_address_for_S11;
    config_unlock(&mme_config);

    session_request_p->serving_network.mcc[0] = ue_context->e_utran_cgi.plmn.MCCdigit1;
    session_request_p->serving_network.mcc[1] = ue_context->e_utran_cgi.plmn.MCCdigit2;
    session_request_p->serving_network.mcc[2] = ue_context->e_utran_cgi.plmn.MCCdigit3;

    session_request_p->serving_network.mnc[0] = ue_context->e_utran_cgi.plmn.MNCdigit1;
    session_request_p->serving_network.mnc[1] = ue_context->e_utran_cgi.plmn.MNCdigit2;
    session_request_p->serving_network.mnc[2] = ue_context->e_utran_cgi.plmn.MNCdigit3;

    session_request_p->selection_mode = MS_O_N_P_APN_S_V;
    return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
}

int mme_app_handle_create_sess_resp(SgwCreateSessionResponse *create_sess_resp_p)
{
    struct ue_context_s *ue_context_p;
    bearer_context_t    *current_bearer_p;

    int16_t bearer_id;

    DevAssert(create_sess_resp_p != NULL);

    MME_APP_DEBUG("Received create session response from S+P-GW\n");

    ue_context_p = mme_ue_context_exists_s11_teid(&mme_app_desc.mme_ue_contexts,
                                                  create_sess_resp_p->teid);
    if (ue_context_p == NULL) {
        MME_APP_DEBUG("We didn't find this teid in list of UE: %08x\n",
                      create_sess_resp_p->teid);
        return -1;
    }

    /* Store the S-GW teid */
    ue_context_p->sgw_s11_teid = create_sess_resp_p->s11_sgw_teid.teid;

    bearer_id = create_sess_resp_p->bearer_context_created.eps_bearer_id/* - 5*/;

    /* Depending on s11 result we have to send reject or accept for bearers */
    DevCheck((bearer_id < BEARERS_PER_UE) && (bearer_id >= 0), bearer_id,
             BEARERS_PER_UE, 0);

    if (create_sess_resp_p->bearer_context_created.cause != REQUEST_ACCEPTED) {
        DevMessage("Cases where bearer cause != REQUEST_ACCEPTED are not handled\n");
    }
    DevAssert(create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.interface_type == S1_U_SGW_GTP_U);

    /* Updating statistics */
    mme_app_desc.mme_ue_contexts.nb_bearers_managed++;
    mme_app_desc.mme_ue_contexts.nb_bearers_since_last_stat++;

    current_bearer_p = &ue_context_p->eps_bearers[bearer_id];

    current_bearer_p->s_gw_teid = create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.teid;
    switch (create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.ipv4 +
        (create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.ipv6 << 1))
    {
        default:
        case 0: {
            /* No address provided: impossible case */
            DevMessage("No ip address for user-plane provided...\n");
        } break;
        case 1: {
            /* Only IPv4 address */
            current_bearer_p->s_gw_address.pdn_type = IPv4;
            memcpy(current_bearer_p->s_gw_address.address.ipv4_address,
                   &create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.ipv4_address, 4);
        } break;
        case 2: {
            /* Only IPv6 address */
            current_bearer_p->s_gw_address.pdn_type = IPv6;
            memcpy(current_bearer_p->s_gw_address.address.ipv6_address,
                   create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.ipv6_address, 16);
        } break;
        case 3: {
            /* Both IPv4 and Ipv6 */
            current_bearer_p->s_gw_address.pdn_type = IPv4_AND_v6;
            memcpy(current_bearer_p->s_gw_address.address.ipv4_address,
                   &create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.ipv4_address, 4);
            memcpy(current_bearer_p->s_gw_address.address.ipv6_address,
                   create_sess_resp_p->bearer_context_created.s1u_sgw_fteid.ipv6_address, 16);
        } break;
    }

    mme_app_dump_ue_contexts(&mme_app_desc.mme_ue_contexts);

    /* Generate attach accepted */
    {
        uint8_t *keNB;
        MessageDef *message_p;

        message_p = itti_alloc_new_message(TASK_MME_APP, NAS_BEARER_PARAM);

        derive_keNB(ue_context_p->vector_in_use->kasme, 156, &keNB);
        memcpy(NAS_BEARER_PARAM(message_p).keNB, keNB, 32);

        free(keNB);

        NAS_BEARER_PARAM(message_p).eNB_ue_s1ap_id = ue_context_p->eNB_ue_s1ap_id;
        NAS_BEARER_PARAM(message_p).mme_ue_s1ap_id = ue_context_p->mme_ue_s1ap_id;

        NAS_BEARER_PARAM(message_p).ebi = bearer_id;

        NAS_BEARER_PARAM(message_p).qci = current_bearer_p->qci;
        NAS_BEARER_PARAM(message_p).prio_level = current_bearer_p->prio_level;
        NAS_BEARER_PARAM(message_p).pre_emp_vulnerability = current_bearer_p->pre_emp_vulnerability;
        NAS_BEARER_PARAM(message_p).pre_emp_capability = current_bearer_p->pre_emp_capability;

        NAS_BEARER_PARAM(message_p).sgw_s1u_teid = current_bearer_p->s_gw_teid;
        memcpy(&NAS_BEARER_PARAM(message_p).sgw_s1u_address,
               &current_bearer_p->s_gw_address, sizeof(ip_address_t));

        memcpy(&NAS_BEARER_PARAM(message_p).ambr, &ue_context_p->subscribed_ambr,
               sizeof(ambr_t));

        return itti_send_msg_to_task(TASK_NAS_MME, INSTANCE_DEFAULT, message_p);
    }

    return 0;
}

