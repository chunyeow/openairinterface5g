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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "tree.h"
#include "queue.h"

#include "intertask_interface.h"

#include "s1ap_eNB_default_values.h"

#include "s1ap_common.h"
#include "s1ap_ies_defs.h"

#include "s1ap_eNB_defs.h"
#include "s1ap_eNB.h"
#include "s1ap_eNB_encoder.h"
#include "s1ap_eNB_handlers.h"
#include "s1ap_eNB_nnsf.h"

#include "s1ap_eNB_nas_procedures.h"
#include "s1ap_eNB_management_procedures.h"

#include "s1ap_eNB_itti_messaging.h"

#include "sctp_primitives_client.h"

#include "assertions.h"
#include "conversions.h"

s1ap_eNB_internal_data_t s1ap_eNB_internal_data;

static int s1ap_eNB_generate_s1_setup_request(
    s1ap_eNB_instance_t *instance_p, s1ap_eNB_mme_data_t *s1ap_mme_data_p);

RB_GENERATE(s1ap_mme_map, s1ap_eNB_mme_data_s, entry, s1ap_eNB_compare_assoc_id);

inline int s1ap_eNB_compare_assoc_id(
    struct s1ap_eNB_mme_data_s *p1, struct s1ap_eNB_mme_data_s *p2)
{
    if (p1->assoc_id == -1) {
        if (p1->cnx_id < p2->cnx_id) {
            return -1;
        }
        if (p1->cnx_id > p2->cnx_id) {
            return 1;
        }
    } else {
        if (p1->assoc_id < p2->assoc_id) {
            return -1;
        }
        if (p1->assoc_id > p2->assoc_id) {
            return 1;
        }
    }

    /* Matching reference */
    return 0;
}

inline struct s1ap_eNB_mme_data_s *s1ap_eNB_get_MME(
    s1ap_eNB_instance_t *instance_p,
    int32_t assoc_id, uint16_t cnx_id)
{
    struct s1ap_eNB_mme_data_s  temp;
    struct s1ap_eNB_mme_data_s *found;

    memset(&temp, 0, sizeof(struct s1ap_eNB_mme_data_s));

    temp.assoc_id = assoc_id;
    temp.cnx_id   = cnx_id;

    if (instance_p == NULL) {
        STAILQ_FOREACH(instance_p, &s1ap_eNB_internal_data.s1ap_eNB_instances_head,
                       s1ap_eNB_entries)
        {
            found = RB_FIND(s1ap_mme_map, &instance_p->s1ap_mme_head, &temp);
            if (found != NULL) {
                return found;
            }
        }
    } else {
        return RB_FIND(s1ap_mme_map, &instance_p->s1ap_mme_head, &temp);
    }

    return NULL;
}

// int s1ap_run(eNB_mme_desc_t *eNB_desc_p)
// {
//     int ret = 0;
//     struct s1ap_eNB_mme_data_s *mme_p;
// 
//     DevAssert(eNB_desc_p != NULL);
// 
//     RB_FOREACH(mme_p, s1ap_mme_map, &eNB_desc_p->s1ap_mme_head) {
//         struct sctp_queue_item_s *item_p;
// 
//         /* Run the SCTP part for each MME */
//         sctp_run(&mme_p->sctp_data);
// 
//         S1AP_DEBUG("Entering s1ap_run for eNB %d: %d packet(s) to handle\n",
//                    eNB_desc_p->eNB_id, mme_p->sctp_data.queue_length);
// 
//         /* Handle every message in the queue */
//         TAILQ_FOREACH(item_p, &mme_p->sctp_data.sctp_queue, entry) {
//             /* Handle the message in S1AP */
//             s1ap_eNB_handle_message(eNB_desc_p, item_p);
//             /* Remove the packet from the list and update data */
//             TAILQ_REMOVE(&mme_p->sctp_data.sctp_queue, item_p, entry);
//             ret += item_p->length;
//             mme_p->sctp_data.queue_size -= item_p->length;
//             mme_p->sctp_data.queue_length--;
//             /* Deallocate memory as the message has been handled */
//             free(item_p->buffer);
//             free(item_p);
//         }
//     }
//     return ret;
// }

static s1ap_eNB_instance_t *s1ap_eNB_get_instance(uint8_t mod_id)
{
    s1ap_eNB_instance_t *temp = NULL;

    STAILQ_FOREACH(temp, &s1ap_eNB_internal_data.s1ap_eNB_instances_head,
                   s1ap_eNB_entries)
    {
        if (temp->mod_id == mod_id) {
            /* Matching occurence */
            return temp;
        }
    }

    return NULL;
}

static void s1ap_eNB_register_mme(s1ap_eNB_instance_t *instance_p,
                                  net_ip_address_t    *mme_ip_address,
                                  net_ip_address_t    *local_ip_addr)
{
    MessageDef                 *message_p;
    sctp_new_association_req_t *sctp_new_association_req_p;
    s1ap_eNB_mme_data_t        *s1ap_mme_data_p;

    DevAssert(instance_p != NULL);
    DevAssert(mme_ip_address != NULL);

    message_p = itti_alloc_new_message(TASK_S1AP, SCTP_NEW_ASSOCIATION_REQ);

    sctp_new_association_req_p = &message_p->msg.sctp_new_association_req;

    sctp_new_association_req_p->port = S1AP_PORT_NUMBER;
    sctp_new_association_req_p->ppid = S1AP_SCTP_PPID;

    memcpy(&sctp_new_association_req_p->remote_address, mme_ip_address,
           sizeof(*mme_ip_address));

    /* Create new MME descriptor */
    s1ap_mme_data_p = calloc(1, sizeof(*s1ap_mme_data_p));
    DevAssert(s1ap_mme_data_p != NULL);

    s1ap_mme_data_p->cnx_id                = s1ap_eNB_internal_data.global_cnx_id;
    sctp_new_association_req_p->ulp_cnx_id = s1ap_mme_data_p->cnx_id;
    s1ap_eNB_internal_data.global_cnx_id++;

    s1ap_mme_data_p->assoc_id          = -1;
    s1ap_mme_data_p->s1ap_eNB_instance = instance_p;

    STAILQ_INIT(&s1ap_mme_data_p->served_gummei);

    /* Insert the new descriptor in list of known MME
     * but not yet associated.
     */
    RB_INSERT(s1ap_mme_map, &instance_p->s1ap_mme_head, s1ap_mme_data_p);

    itti_send_msg_to_task(TASK_SCTP, INSTANCE_DEFAULT, message_p);
}

void s1ap_eNB_handle_register_eNB(s1ap_register_eNB_t *s1ap_register_eNB)
{
    s1ap_eNB_instance_t *new_instance;
    uint8_t index;

    DevAssert(s1ap_register_eNB != NULL);

    /* Look if the provided mod id already exists
     * If so notify user...
     */
    new_instance = s1ap_eNB_get_instance(s1ap_register_eNB->mod_id);
    DevAssert(new_instance == NULL);

    new_instance = calloc(1, sizeof(s1ap_eNB_instance_t));
    DevAssert(new_instance != NULL);

    RB_INIT(&new_instance->s1ap_ue_head);
    RB_INIT(&new_instance->s1ap_mme_head);

    /* Copy usefull parameters */
    new_instance->mod_id      = s1ap_register_eNB->mod_id;
    new_instance->eNB_name    = s1ap_register_eNB->eNB_name;
    new_instance->eNB_id      = s1ap_register_eNB->eNB_id;
    new_instance->cell_type   = s1ap_register_eNB->cell_type;
    new_instance->tac         = s1ap_register_eNB->tac;
    new_instance->mcc         = s1ap_register_eNB->mcc;
    new_instance->mnc         = s1ap_register_eNB->mnc;
    new_instance->default_drx = s1ap_register_eNB->default_drx;

    STAILQ_INSERT_TAIL(&s1ap_eNB_internal_data.s1ap_eNB_instances_head,
                       new_instance, s1ap_eNB_entries);

    S1AP_DEBUG("Registered new eNB with mod_id %u and %s eNB id %u\n",
               s1ap_register_eNB->mod_id,
               s1ap_register_eNB->cell_type == CELL_MACRO_ENB ? "macro" : "home",
               s1ap_register_eNB->eNB_id);

    DevCheck(s1ap_register_eNB->nb_mme <= S1AP_MAX_NB_MME_IP_ADDRESS,
             S1AP_MAX_NB_MME_IP_ADDRESS, s1ap_register_eNB->nb_mme, 0);

    /* Trying to connect to provided list of MME ip address */
    for (index = 0; index < s1ap_register_eNB->nb_mme; index++) {
        s1ap_eNB_register_mme(new_instance, &s1ap_register_eNB->mme_ip_address[index],
                              &s1ap_register_eNB->enb_ip_address);
    }
}

void s1ap_eNB_handle_sctp_association_resp(sctp_new_association_resp_t *sctp_new_association_resp)
{
    s1ap_eNB_instance_t *instance_p;
    s1ap_eNB_mme_data_t *s1ap_mme_data_p;

    DevAssert(sctp_new_association_resp != NULL);

    instance_p = s1ap_eNB_get_instance(sctp_new_association_resp->mod_id);
    DevAssert(instance_p != NULL);

    s1ap_mme_data_p = s1ap_eNB_get_MME(instance_p, -1,
                                       sctp_new_association_resp->ulp_cnx_id);
    DevAssert(s1ap_mme_data_p != NULL);

    if (sctp_new_association_resp->sctp_state != SCTP_STATE_ESTABLISHED) {
        S1AP_WARN("Received unsuccessful result for SCTP association (%u), mod_id %u, cnx_id %u\n",
                  sctp_new_association_resp->sctp_state,
                  sctp_new_association_resp->mod_id,
                  sctp_new_association_resp->ulp_cnx_id);
    }

    /* Update parameters */
    s1ap_mme_data_p->assoc_id    = sctp_new_association_resp->assoc_id;
    s1ap_mme_data_p->in_streams  = sctp_new_association_resp->in_streams;
    s1ap_mme_data_p->out_streams = sctp_new_association_resp->out_streams;

    /* Prepare new S1 Setup Request */
    s1ap_eNB_generate_s1_setup_request(instance_p, s1ap_mme_data_p);
}

static
void s1ap_eNB_handle_sctp_data_ind(sctp_data_ind_t *sctp_data_ind)
{
    DevAssert(sctp_data_ind != NULL);

    s1ap_eNB_handle_message(sctp_data_ind->assoc_id, sctp_data_ind->stream,
                            sctp_data_ind->buffer, sctp_data_ind->buffer_length);

    free(sctp_data_ind->buffer);
}

void *s1ap_eNB_task(void *arg)
{
    MessageDef *received_msg = NULL;

    S1AP_DEBUG("Starting S1AP layer\n");

    memset(&s1ap_eNB_internal_data, 0, sizeof(s1ap_eNB_internal_data));
    STAILQ_INIT(&s1ap_eNB_internal_data.s1ap_eNB_instances_head);

    itti_mark_task_ready(TASK_S1AP);

    while (1) {
        itti_receive_msg(TASK_S1AP, &received_msg);

        switch (received_msg->header.messageId) {
            case TERMINATE_MESSAGE:
                itti_exit_task();
                break;
            case S1AP_REGISTER_ENB: {
                /* Register a new eNB.
                 * in Virtual mode eNBs will be distinguished using the mod_id/
                 * Each eNB has to send an S1AP_REGISTER_ENB message with its
                 * own parameters.
                 */
                s1ap_eNB_handle_register_eNB(&received_msg->msg.s1ap_register_eNB);
            } break;
            case SCTP_NEW_ASSOCIATION_RESP: {
                s1ap_eNB_handle_sctp_association_resp(&received_msg->msg.sctp_new_association_resp);
            } break;
            case SCTP_DATA_IND: {
                s1ap_eNB_handle_sctp_data_ind(&received_msg->msg.sctp_data_ind);
            } break;
            default:
                S1AP_ERROR("Received unhandled message with id %d\n",
                           received_msg->header.messageId);
                break;
        }

        free(received_msg);

        received_msg = NULL;
    }
    return NULL;
}

static int s1ap_eNB_generate_s1_setup_request(
    s1ap_eNB_instance_t *instance_p, s1ap_eNB_mme_data_t *s1ap_mme_data_p)
{
    s1ap_message         message;
    S1SetupRequestIEs_t *s1SetupRequest_p;
    PLMNidentity_t       plmnIdentity;
    SupportedTAs_Item_t  ta;
    uint8_t             *buffer;
    uint32_t             len;
    int                  ret;

    DevAssert(instance_p != NULL);
    DevAssert(s1ap_mme_data_p != NULL);

    memset(&message, 0, sizeof(s1ap_message));

    message.direction     = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = ProcedureCode_id_S1Setup;
    message.criticality   = Criticality_reject;

    s1SetupRequest_p = &message.msg.s1SetupRequestIEs;
    memset((void *)&plmnIdentity, 0, sizeof(PLMNidentity_t));

    memset((void *)&ta, 0, sizeof(SupportedTAs_Item_t));

    s1ap_mme_data_p->state = S1AP_ENB_STATE_WAITING;

    s1SetupRequest_p->global_ENB_ID.eNB_ID.present = ENB_ID_PR_macroENB_ID;
    MACRO_ENB_ID_TO_BIT_STRING(instance_p->eNB_id,
                               &s1SetupRequest_p->global_ENB_ID.eNB_ID.choice.macroENB_ID);
    MCC_MNC_TO_PLMNID(instance_p->mcc, instance_p->mnc,
                      &s1SetupRequest_p->global_ENB_ID.pLMNidentity);

    INT16_TO_OCTET_STRING(instance_p->tac, &ta.tAC);
    MCC_MNC_TO_TBCD(instance_p->mcc, instance_p->mnc, &plmnIdentity);

    ASN_SEQUENCE_ADD(&ta.broadcastPLMNs.list, &plmnIdentity);
    ASN_SEQUENCE_ADD(&s1SetupRequest_p->supportedTAs.list, &ta);

    s1SetupRequest_p->defaultPagingDRX = instance_p->default_drx;

    if (instance_p->eNB_name != NULL) {
        s1SetupRequest_p->presenceMask |= S1SETUPREQUESTIES_ENBNAME_PRESENT;
        OCTET_STRING_fromBuf(&s1SetupRequest_p->eNBname, instance_p->eNB_name,
                             strlen(instance_p->eNB_name));
    }

    if (s1ap_eNB_encode_pdu(&message, &buffer, &len) < 0) {
        S1AP_ERROR("Failed to encode S1 setup request\n");
        return -1;
    }

    /* Non UE-Associated signalling -> stream = 0 */
    s1ap_eNB_itti_send_sctp_data_req(s1ap_mme_data_p->assoc_id, buffer, len, 0);

    return ret;
}

// int s1ap_eNB_generate_initial_UE_message(eNB_mme_desc_t       *eNB_desc_p,
//                                          s1ap_nas_first_req_t  nas_req_p)
// {
//     s1ap_message message;
//     struct s1ap_eNB_mme_data_s   *mme_desc_p;
//     struct s1ap_eNB_ue_context_s *ue_desc_p;
//     InitialUEMessageIEs_t        *initial_ue_message_p;
// 
//     uint8_t  *buffer;
//     uint32_t  length;
// 
//     DevAssert(eNB_desc_p != NULL);
// 
//     memset(&message, 0, sizeof(s1ap_message));
// 
//     message.direction = S1AP_PDU_PR_initiatingMessage;
//     message.procedureCode = ProcedureCode_id_initialUEMessage;
// 
//     initial_ue_message_p = &message.msg.initialUEMessageIEs;
// 
//     /* Select the MME corresponding to the provided GUMMEI.
//      * If no MME corresponds to the GUMMEI, the function selects the MME with the
//      * highest capacity.
//      * In case eNB has no MME associated, the eNB should inform RRC and discard
//      * this request.
//      */
//     if (nas_req_p.ue_identity.present == GUMMEI_PROVIDED) {
//         mme_desc_p = s1ap_eNB_nnsf_select_mme_by_gummei(
//             eNB_desc_p,
//             nas_req_p.establishment_cause, nas_req_p.ue_identity.identity.gummei);
//     } else {
//         mme_desc_p = s1ap_eNB_nnsf_select_mme_by_mme_code(
//             eNB_desc_p,
//             nas_req_p.establishment_cause, nas_req_p.ue_identity.identity.s_tmsi.mme_code);
//     }
//     if (mme_desc_p == NULL) {
//         S1AP_WARN("No MME is associated to the eNB\n");
//         // TODO: Inform RRC
//         return -1;
//     }
// 
//     /* The eNB should allocate a unique eNB UE S1AP ID for this UE. The value
//      * will be used for the duration of the connectivity.
//      */
//     if ((ue_desc_p = s1ap_eNB_allocate_new_UE_context()) == NULL) {
//         return -1;
//     }
// 
//     /* Keep a reference to the selected MME */
//     ue_desc_p->mme_ref = mme_desc_p;
//     ue_desc_p->rnti    = nas_req_p.rnti;
// 
//     do {
//         struct s1ap_eNB_ue_context_s *collision_p;
// 
//         /* Peek a random value for the eNB_ue_s1ap_id */
//         ue_desc_p->eNB_ue_s1ap_id = (random() + random()) & 0x00ffffff;
//         if ((collision_p = RB_INSERT(s1ap_ue_map, &eNB_desc_p->s1ap_ue_head, ue_desc_p))
//                 == NULL) {
//             /* Break the loop as the id is not already used by another UE */
//             break;
//         }
//     } while(1);
// 
//     initial_ue_message_p->eNB_UE_S1AP_ID = ue_desc_p->eNB_ue_s1ap_id;
//     /* Prepare the NAS PDU */
//     initial_ue_message_p->nas_pdu.buf  = nas_req_p.nas_pdu.buffer;
//     initial_ue_message_p->nas_pdu.size = nas_req_p.nas_pdu.length;
// 
//     /* Set the establishment cause according to those provided by RRC */
//     DevCheck(nas_req_p.establishment_cause <= RRC_CAUSE_MAX,
//              nas_req_p.establishment_cause, 0, 0);
//     initial_ue_message_p->rrC_Establishment_Cause = nas_req_p.establishment_cause;
// 
//     if (nas_req_p.ue_identity.present == S_TMSI_PROVIDED) {
//         initial_ue_message_p->presenceMask |= INITIALUEMESSAGEIES_S_TMSI_PRESENT;
// 
//         MME_CODE_TO_OCTET_STRING(nas_req_p.ue_identity.identity.s_tmsi.mme_code,
//                                  &initial_ue_message_p->s_tmsi.mMEC);
//         M_TMSI_TO_OCTET_STRING(nas_req_p.ue_identity.identity.s_tmsi.m_tmsi,
//                                &initial_ue_message_p->s_tmsi.m_TMSI);
//     } else {
//         initial_ue_message_p->presenceMask |= INITIALUEMESSAGEIES_GUMMEI_ID_PRESENT;
// 
//         MCC_MNC_TO_PLMNID(nas_req_p.ue_identity.identity.gummei.mcc,
//                           nas_req_p.ue_identity.identity.gummei.mnc,
//                           &initial_ue_message_p->gummei_id.pLMN_Identity);
//         MME_GID_TO_OCTET_STRING(nas_req_p.ue_identity.identity.gummei.mme_group_id,
//                                 &initial_ue_message_p->gummei_id.mME_Group_ID);
//         MME_CODE_TO_OCTET_STRING(nas_req_p.ue_identity.identity.gummei.mme_code,
//                                  &initial_ue_message_p->gummei_id.mME_Code);
//     }
// 
//     /* Assuming TAI is the TAI from the cell */
//     INT16_TO_OCTET_STRING(eNB_desc_p->tac, &initial_ue_message_p->tai.tAC);
//     MCC_MNC_TO_PLMNID(eNB_desc_p->mcc, eNB_desc_p->mnc,
//                       &initial_ue_message_p->tai.pLMNidentity);
// 
//     /* Set the EUTRAN CGI
//      * The cell identity is defined on 28 bits but as we use macro enb id,
//      * we have to pad.
//      */
//     MACRO_ENB_ID_TO_CELL_IDENTITY(eNB_desc_p->eNB_id,
//                                   &initial_ue_message_p->eutran_cgi.cell_ID);
//     MCC_MNC_TO_TBCD(eNB_desc_p->mcc, eNB_desc_p->mnc,
//                     &initial_ue_message_p->eutran_cgi.pLMNidentity);
// 
//     if (s1ap_eNB_encode_pdu(&message, &buffer, &length) < 0) {
//         /* Failed to encode message */
//         return -1;
//     }
// 
//     /* Update the current S1AP UE state */
//     ue_desc_p->ue_state = S1AP_UE_WAITING_CSR;
// 
//     /* Send encoded message over sctp */
//     return sctp_send_msg(&mme_desc_p->sctp_data, S1AP_SCTP_PPID, 1, buffer, length);
// }

