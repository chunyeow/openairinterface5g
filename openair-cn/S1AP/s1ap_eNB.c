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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tree.h"
#include "queue.h"

#include "mme_sim.h"
#include "eNB_default_values.h"

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"

#include "s1ap_eNB_defs.h"
#include "s1ap_eNB.h"
#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_handlers.h"
#include "s1ap_eNB_nnsf.h"

#include "s1ap_eNB_nas_procedures.h"
#include "s1ap_eNB_management_procedures.h"

#include "sctp_primitives_client.h"

#include "assertions.h"
#include "conversions.h"

static int s1ap_eNB_generate_s1_setup_request(eNB_mme_desc_t *eNB_desc_p,
                                              s1ap_eNB_mme_data_t *mme_assoc_p);

RB_GENERATE(s1ap_mme_map, s1ap_eNB_mme_data_s, entry, s1ap_eNB_compare_assoc_id);

inline int s1ap_eNB_compare_assoc_id(
    struct s1ap_eNB_mme_data_s *p1, struct s1ap_eNB_mme_data_s *p2)
{
    if (p1->sctp_data.assoc_id < p2->sctp_data.assoc_id) {
        return -1;
    }
    if (p1->sctp_data.assoc_id > p2->sctp_data.assoc_id) {
        return 1;
    }
    return 0;
}

inline struct s1ap_eNB_mme_data_s *s1ap_eNB_get_MME(eNB_mme_desc_t *eNB_desc_p,
        uint32_t assoc_id)
{
    struct s1ap_eNB_mme_data_s temp;

    memset(&temp, 0, sizeof(struct s1ap_eNB_mme_data_s));

    temp.sctp_data.assoc_id = assoc_id;

    return RB_FIND(s1ap_mme_map, &eNB_desc_p->s1ap_mme_head, &temp);
}

int s1ap_run(eNB_mme_desc_t *eNB_desc_p)
{
    int ret = 0;
    struct s1ap_eNB_mme_data_s *mme_p;

    DevAssert(eNB_desc_p != NULL);

    RB_FOREACH(mme_p, s1ap_mme_map, &eNB_desc_p->s1ap_mme_head) {
        struct sctp_queue_item_s *item_p;

        /* Run the SCTP part for each MME */
        sctp_run(&mme_p->sctp_data);

        S1AP_DEBUG("Entering s1ap_run for eNB %d: %d packet(s) to handle\n",
                   eNB_desc_p->eNB_id, mme_p->sctp_data.queue_length);

        /* Handle every message in the queue */
        TAILQ_FOREACH(item_p, &mme_p->sctp_data.sctp_queue, entry) {
            /* Handle the message in S1AP */
            s1ap_eNB_handle_message(eNB_desc_p, item_p);
            /* Remove the packet from the list and update data */
            TAILQ_REMOVE(&mme_p->sctp_data.sctp_queue, item_p, entry);
            ret += item_p->length;
            mme_p->sctp_data.queue_size -= item_p->length;
            mme_p->sctp_data.queue_length--;
            /* Deallocate memory as the message has been handled */
            free(item_p->buffer);
            free(item_p);
        }
    }
    return ret;
}

int s1ap_eNB_init(eNB_mme_desc_t *eNB_desc_p,
                  char **local_ip_addr,  int nb_local_ip,
                  char **remote_ip_addr, int nb_remote_ip)
{
    int i;
    sctp_data_t sctp_data;
    char hostname[30];

    DevAssert(eNB_desc_p != NULL);

    if (eNB_desc_p->eNB_id >= (1 << 20))
        S1AP_WARN("eNB_id exceed limit of 20 bits...\n");

    if (gethostname(hostname, sizeof(hostname)) == -1) {
        hostname[0] = '\0';
    }

    /* Set the eNB name */
    snprintf(eNB_desc_p->eNB_name, S1AP_ENB_NAME_LENGTH_MAX,
             ENB_NAME " %s %u", hostname, eNB_desc_p->eNB_id & 0xFFFFF);

    S1AP_DEBUG("Initializing S1AP layer for eNB %u\n",
               eNB_desc_p->eNB_id);

    /* TODO: RRM configuration... */
    eNB_desc_p->tac    = ENB_TAC;
    eNB_desc_p->mcc    = ENB_MCC;
    eNB_desc_p->mnc    = ENB_MNC;

    RB_INIT(&eNB_desc_p->s1ap_mme_head);
    RB_INIT(&eNB_desc_p->s1ap_ue_head);

    memset(&sctp_data, 0, sizeof(sctp_data_t));

    for (i = 0; i < nb_remote_ip; i++)
    {
        /* Connecting eNB to provided MME IP address and port */
        if (sctp_connect_to_remote_host(local_ip_addr, nb_local_ip,
            remote_ip_addr[i], S1AP_PORT_NUMBER,
            SOCK_STREAM, &sctp_data) <= 0)
        {
            S1AP_ERROR("Failed to connect to %s:%d\n",
                       remote_ip_addr[i], S1AP_PORT_NUMBER);
            return -1;
        } else {
            struct s1ap_eNB_mme_data_s *mme_assoc_p;
            struct s1ap_eNB_mme_data_s *collision_p;

            mme_assoc_p = calloc(1, sizeof(struct s1ap_eNB_mme_data_s));
            mme_assoc_p->nextstream = 1;

            memcpy(&mme_assoc_p->sctp_data, &sctp_data, sizeof(sctp_data_t));

            TAILQ_INIT(&mme_assoc_p->sctp_data.sctp_queue);

            if ((collision_p = RB_INSERT(s1ap_mme_map, &eNB_desc_p->s1ap_mme_head,
                                         mme_assoc_p)) != NULL) {
                S1AP_WARN("Failed to add MME to the tree of associated MME\n");
                free(mme_assoc_p);
                return -1;
            }

            S1AP_DEBUG("[%d] Successfully added MME to the list of known host\n",
                       sctp_data.assoc_id);
            S1AP_DEBUG("[%d] Now tries to send S1 setup request\n",
                       sctp_data.assoc_id);

            STAILQ_INIT(&mme_assoc_p->served_gummei);
            if (s1ap_eNB_generate_s1_setup_request(eNB_desc_p, mme_assoc_p) == -1)
            {
                exit(EXIT_FAILURE);
            }
        }
    }

    S1AP_DEBUG("Initializing S1AP layer for eNB %d: DONE\n", eNB_desc_p->eNB_id);
    return 0;
}

static int s1ap_eNB_generate_s1_setup_request(
    eNB_mme_desc_t      *eNB_desc_p,
    s1ap_eNB_mme_data_t *mme_assoc_p)
{
    s1ap_message message;
    S1SetupRequestIEs_t *s1SetupRequest_p;
    PLMNidentity_t       plmnIdentity;
    SupportedTAs_Item_t  ta;
    uint8_t             *buffer;
    uint32_t             len;
    int                  ret;

    DevAssert(eNB_desc_p != NULL);
    DevAssert(mme_assoc_p != NULL);

    memset(&message, 0, sizeof(s1ap_message));

    message.direction = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = ProcedureCode_id_S1Setup;

    s1SetupRequest_p = &message.msg.s1SetupRequestIEs;
    memset((void *)&plmnIdentity, 0, sizeof(PLMNidentity_t));

    memset((void *)&ta, 0, sizeof(SupportedTAs_Item_t));

    mme_assoc_p->state = S1AP_ENB_STATE_WAITING;

    s1SetupRequest_p->global_ENB_ID.eNB_ID.present = ENB_ID_PR_macroENB_ID;
    MACRO_ENB_ID_TO_BIT_STRING(eNB_desc_p->eNB_id,
                               &s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID);
    MCC_MNC_TO_PLMNID(eNB_desc_p->mcc, eNB_desc_p->mnc,
                      &s1SetupRequest_p->global_ENB_ID.pLMNidentity);

    INT16_TO_OCTET_STRING(eNB_desc_p->tac, &ta.tAC);
    MCC_MNC_TO_TBCD(eNB_desc_p->mcc, eNB_desc_p->mnc, &plmnIdentity);

    ASN_SEQUENCE_ADD(&ta.broadcastPLMNs.list, &plmnIdentity);
    ASN_SEQUENCE_ADD(&s1SetupRequest_p->supportedTAs.list, &ta);

    s1SetupRequest_p->defaultPagingDRX = PagingDRX_v64;

    if (eNB_desc_p->eNB_name != NULL) {
        s1SetupRequest_p->presenceMask |= S1SETUPREQUESTIES_ENBNAME_PRESENT;
        OCTET_STRING_fromBuf(&s1SetupRequest_p->eNBname, eNB_desc_p->eNB_name,
                             strlen(eNB_desc_p->eNB_name));
    }

    if (s1ap_eNB_encode_pdu(&message, &buffer, &len) < 0) {
        S1AP_ERROR("Failed to encode S1 setup request\n");
        return -1;
    }

    if ((ret = sctp_send_msg(&mme_assoc_p->sctp_data, S1AP_SCTP_PPID, 0, buffer,
                             len)) < 0) {
        S1AP_ERROR("Failed to send S1 setup request\n");
    }
    free(buffer);
    return ret;
}

int s1ap_eNB_generate_initial_UE_message(eNB_mme_desc_t       *eNB_desc_p,
                                         s1ap_nas_first_req_t  nas_req_p)
{
    s1ap_message message;
    struct s1ap_eNB_mme_data_s   *mme_desc_p;
    struct s1ap_eNB_ue_context_s *ue_desc_p;
    InitialUEMessageIEs_t        *initial_ue_message_p;

    uint8_t  *buffer;
    uint32_t  length;

    DevAssert(eNB_desc_p != NULL);

    memset(&message, 0, sizeof(s1ap_message));

    message.direction = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = ProcedureCode_id_initialUEMessage;

    initial_ue_message_p = &message.msg.initialUEMessageIEs;

    /* Select the MME corresponding to the provided GUMMEI.
     * If no MME corresponds to the GUMMEI, the function selects the MME with the
     * highest capacity.
     * In case eNB has no MME associated, the eNB should inform RRC and discard
     * this request.
     */
    if (nas_req_p.ue_identity.present == GUMMEI_PROVIDED) {
        mme_desc_p = s1ap_eNB_nnsf_select_mme_by_gummei(
            eNB_desc_p,
            nas_req_p.establishment_cause, nas_req_p.ue_identity.identity.gummei);
    } else {
        mme_desc_p = s1ap_eNB_nnsf_select_mme_by_mme_code(
            eNB_desc_p,
            nas_req_p.establishment_cause, nas_req_p.ue_identity.identity.s_tmsi.mme_code);
    }
    if (mme_desc_p == NULL) {
        S1AP_WARN("No MME is associated to the eNB\n");
        // TODO: Inform RRC
        return -1;
    }

    /* The eNB should allocate a unique eNB UE S1AP ID for this UE. The value
     * will be used for the duration of the connectivity.
     */
    if ((ue_desc_p = s1ap_eNB_allocate_new_UE_context()) == NULL) {
        return -1;
    }

    /* Keep a reference to the selected MME */
    ue_desc_p->mme_ref = mme_desc_p;
    ue_desc_p->rnti    = nas_req_p.rnti;

    do {
        struct s1ap_eNB_ue_context_s *collision_p;

        /* Peek a random value for the eNB_ue_s1ap_id */
        ue_desc_p->eNB_ue_s1ap_id = (random() + random()) & 0x00ffffff;
        if ((collision_p = RB_INSERT(s1ap_ue_map, &eNB_desc_p->s1ap_ue_head, ue_desc_p))
                == NULL) {
            /* Break the loop as the id is not already used by another UE */
            break;
        }
    } while(1);

    initial_ue_message_p->eNB_UE_S1AP_ID = ue_desc_p->eNB_ue_s1ap_id;
    /* Prepare the NAS PDU */
    initial_ue_message_p->nas_pdu.buf  = nas_req_p.nas_pdu.buffer;
    initial_ue_message_p->nas_pdu.size = nas_req_p.nas_pdu.length;

    /* Set the establishment cause according to those provided by RRC */
    DevCheck(nas_req_p.establishment_cause <= RRC_CAUSE_MAX,
             nas_req_p.establishment_cause, 0, 0);
    initial_ue_message_p->rrC_Establishment_Cause = nas_req_p.establishment_cause;

    if (nas_req_p.ue_identity.present == S_TMSI_PROVIDED) {
        initial_ue_message_p->presenceMask |= INITIALUEMESSAGEIES_S_TMSI_PRESENT;

        MME_CODE_TO_OCTET_STRING(nas_req_p.ue_identity.identity.s_tmsi.mme_code,
                                 &initial_ue_message_p->s_tmsi.mMEC);
        M_TMSI_TO_OCTET_STRING(nas_req_p.ue_identity.identity.s_tmsi.m_tmsi,
                               &initial_ue_message_p->s_tmsi.m_TMSI);
    } else {
        initial_ue_message_p->presenceMask |= INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT;

        MCC_MNC_TO_PLMNID(nas_req_p.ue_identity.identity.gummei.mcc,
                          nas_req_p.ue_identity.identity.gummei.mnc,
                          &initial_ue_message_p->gummei_id.pLMN_Identity);
        MME_GID_TO_OCTET_STRING(nas_req_p.ue_identity.identity.gummei.mme_group_id,
                                &initial_ue_message_p->gummei_id.mME_Group_ID);
        MME_CODE_TO_OCTET_STRING(nas_req_p.ue_identity.identity.gummei.mme_code,
                                 &initial_ue_message_p->gummei_id.mME_Code);
    }

    /* Assuming TAI is the TAI from the cell */
    INT16_TO_OCTET_STRING(eNB_desc_p->tac, &initial_ue_message_p->tai.tAC);
    MCC_MNC_TO_PLMNID(eNB_desc_p->mcc, eNB_desc_p->mnc,
                      &initial_ue_message_p->tai.pLMNidentity);

    /* Set the EUTRAN CGI
     * The cell identity is defined on 28 bits but as we use macro enb id,
     * we have to pad.
     */
    MACRO_ENB_ID_TO_CELL_IDENTITY(eNB_desc_p->eNB_id,
                                  &initial_ue_message_p->eutran_cgi.cell_ID);
    MCC_MNC_TO_TBCD(eNB_desc_p->mcc, eNB_desc_p->mnc,
                    &initial_ue_message_p->eutran_cgi.pLMNidentity);

    if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
        /* Failed to encode message */
        return -1;
    }

    /* Update the current S1AP UE state */
    ue_desc_p->ue_state = S1AP_UE_WAITING_CSR;

    /* Send encoded message over sctp */
    return sctp_send_msg(&mme_desc_p->sctp_data, S1AP_SCTP_PPID, 1, buffer, length);
}

int s1ap_eNB_handle_api_req(eNB_mme_desc_t     *eNB_desc_p,
                            s1ap_rrc_api_req_t *api_req_p)
{
    int ret = -1;

    DevAssert(eNB_desc_p != NULL);
    DevAssert(api_req_p != NULL);

    switch(api_req_p->api_req) {
        case S1AP_API_NAS_FIRST_REQ:
            return s1ap_eNB_generate_initial_UE_message(eNB_desc_p,
                api_req_p->msg.first_nas_req);
        case S1AP_API_NAS_UPLINK:
            return s1ap_eNB_nas_uplink(eNB_desc_p, &api_req_p->msg.nas_uplink);
        case S1AP_API_UE_CAP_INFO_IND:
            return s1ap_eNB_ue_capabilities(eNB_desc_p, &api_req_p->msg.ue_cap_info_ind);
        case S1AP_API_INITIAL_CONTEXT_SETUP_RESP:
            return s1ap_eNB_initial_ctxt_resp(eNB_desc_p, &api_req_p->msg.initial_ctxt_resp);
        case S1AP_API_NAS_NON_DELIVERY_IND:
        case S1AP_API_PATH_SWITCH_REQ:
        case S1AP_API_INITIAL_CONTEXT_SETUP_FAIL:
        case S1AP_API_E_RAB_SETUP_RESP:
        case S1AP_API_E_RAB_MODIFY_RESP:
        case S1AP_API_E_RAB_RELEASE_RESP:
        case S1AP_API_RESET:
        case S1AP_API_RESET_ACK:
            S1AP_ERROR("This API type (%02x) is not implemented yet\n",
                       api_req_p->api_req);
            break;
        default:
            S1AP_ERROR("Unknown API type %02x\n", api_req_p->api_req);
            break;
    }
    return ret;
}
