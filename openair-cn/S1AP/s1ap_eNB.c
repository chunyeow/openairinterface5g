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
#include <crypt.h>

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

#include "assertions.h"
#include "conversions.h"

#if !defined(OAI_EMU)
s1ap_eNB_config_t s1ap_config;
#endif

static int s1ap_eNB_generate_s1_setup_request(
    s1ap_eNB_instance_t *instance_p, s1ap_eNB_mme_data_t *s1ap_mme_data_p);

static
void s1ap_eNB_handle_register_eNB(instance_t instance, s1ap_register_eNB_t *s1ap_register_eNB);
static
void s1ap_eNB_handle_sctp_association_resp(instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp);

uint32_t s1ap_generate_eNB_id(void)
{
    char *out;
    char  hostname[50];
    int   ret;
    uint32_t eNB_id;

    /* Retrieve the host name */
    ret = gethostname(hostname, sizeof(hostname));
    DevAssert(ret == 0);

    out = crypt(hostname, "eurecom");
    DevAssert(out != NULL);

    eNB_id = ((out[0] << 24) | (out[1] << 16) | (out[2] << 8) | out[3]);

    return eNB_id;
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

    sctp_new_association_req_p = &message_p->ittiMsg.sctp_new_association_req;

    sctp_new_association_req_p->port = S1AP_PORT_NUMBER;
    sctp_new_association_req_p->ppid = S1AP_SCTP_PPID;

    memcpy(&sctp_new_association_req_p->remote_address, mme_ip_address,
           sizeof(*mme_ip_address));

    /* Create new MME descriptor */
    s1ap_mme_data_p = calloc(1, sizeof(*s1ap_mme_data_p));
    DevAssert(s1ap_mme_data_p != NULL);

    s1ap_mme_data_p->cnx_id                = s1ap_eNB_fetch_add_global_cnx_id();
    sctp_new_association_req_p->ulp_cnx_id = s1ap_mme_data_p->cnx_id;

    s1ap_mme_data_p->assoc_id          = -1;
    s1ap_mme_data_p->s1ap_eNB_instance = instance_p;

    STAILQ_INIT(&s1ap_mme_data_p->served_gummei);

    /* Insert the new descriptor in list of known MME
     * but not yet associated.
     */
    RB_INSERT(s1ap_mme_map, &instance_p->s1ap_mme_head, s1ap_mme_data_p);

    itti_send_msg_to_task(TASK_SCTP, instance_p->instance, message_p);
}

static
void s1ap_eNB_handle_register_eNB(instance_t instance, s1ap_register_eNB_t *s1ap_register_eNB)
{
    s1ap_eNB_instance_t *new_instance;
    uint8_t index;

    DevAssert(s1ap_register_eNB != NULL);

    /* Look if the provided instance already exists
     * If so notify user...
     */
    new_instance = s1ap_eNB_get_instance(instance);
    DevAssert(new_instance == NULL);

    new_instance = calloc(1, sizeof(s1ap_eNB_instance_t));
    DevAssert(new_instance != NULL);

    RB_INIT(&new_instance->s1ap_ue_head);
    RB_INIT(&new_instance->s1ap_mme_head);

    /* Copy usefull parameters */
    new_instance->instance    = instance;
    new_instance->eNB_name    = s1ap_register_eNB->eNB_name;
    new_instance->eNB_id      = s1ap_register_eNB->eNB_id;
    new_instance->cell_type   = s1ap_register_eNB->cell_type;
    new_instance->tac         = s1ap_register_eNB->tac;
    new_instance->mcc         = s1ap_register_eNB->mcc;
    new_instance->mnc         = s1ap_register_eNB->mnc;
    new_instance->default_drx = s1ap_register_eNB->default_drx;

    /* Add the new instance to the list of eNB (meaningfull in virtual mode) */
    s1ap_eNB_insert_new_instance(new_instance);

    S1AP_DEBUG("Registered new eNB[%d] and %s eNB id %u\n",
               instance,
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

static
void s1ap_eNB_handle_sctp_association_resp(instance_t instance, sctp_new_association_resp_t *sctp_new_association_resp)
{
    s1ap_eNB_instance_t *instance_p;
    s1ap_eNB_mme_data_t *s1ap_mme_data_p;

    DevAssert(sctp_new_association_resp != NULL);

    instance_p = s1ap_eNB_get_instance(instance);
    DevAssert(instance_p != NULL);

    s1ap_mme_data_p = s1ap_eNB_get_MME(instance_p, -1,
                                       sctp_new_association_resp->ulp_cnx_id);
    DevAssert(s1ap_mme_data_p != NULL);

    if (sctp_new_association_resp->sctp_state != SCTP_STATE_ESTABLISHED) {
        S1AP_WARN("Received unsuccessful result for SCTP association (%u), instance %d, cnx_id %u\n",
                  sctp_new_association_resp->sctp_state,
                  instance,
                  sctp_new_association_resp->ulp_cnx_id);
        return;
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

    s1ap_eNB_prepare_internal_data();

    itti_mark_task_ready(TASK_S1AP);

    while (1) {
        itti_receive_msg(TASK_S1AP, &received_msg);

        switch (ITTI_MSG_ID(received_msg)) {
            case TERMINATE_MESSAGE:
                itti_exit_task();
                break;
            case S1AP_REGISTER_ENB: {
                /* Register a new eNB.
                 * in Virtual mode eNBs will be distinguished using the mod_id/
                 * Each eNB has to send an S1AP_REGISTER_ENB message with its
                 * own parameters.
                 */
                s1ap_eNB_handle_register_eNB(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                             &received_msg->ittiMsg.s1ap_register_eNB);
            } break;
            case SCTP_NEW_ASSOCIATION_RESP: {
                s1ap_eNB_handle_sctp_association_resp(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                                      &received_msg->ittiMsg.sctp_new_association_resp);
            } break;
            case SCTP_DATA_IND: {
                s1ap_eNB_handle_sctp_data_ind(&received_msg->ittiMsg.sctp_data_ind);
            } break;
            case S1AP_NAS_FIRST_REQ: {
                s1ap_eNB_handle_nas_first_req(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                              &S1AP_NAS_FIRST_REQ(received_msg));
            } break;
            case S1AP_UPLINK_NAS: {
                s1ap_eNB_nas_uplink(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                    &S1AP_UPLINK_NAS(received_msg));
            } break;
            case S1AP_INITIAL_CONTEXT_SETUP_RESP: {
                s1ap_eNB_initial_ctxt_resp(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                           &S1AP_INITIAL_CONTEXT_SETUP_RESP(received_msg));
            } break;
            case S1AP_NAS_NON_DELIVERY_IND: {
                s1ap_eNB_nas_non_delivery_ind(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                              &S1AP_NAS_NON_DELIVERY_IND(received_msg));
            } break;
            default:
                S1AP_ERROR("Received unhandled message: %d:%s\n",
                           ITTI_MSG_ID(received_msg), ITTI_MSG_NAME(received_msg));
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
    s1ap_message message;

    S1ap_S1SetupRequestIEs_t *s1SetupRequest_p;
    S1ap_PLMNidentity_t       plmnIdentity;
    S1ap_SupportedTAs_Item_t  ta;

    uint8_t  *buffer;
    uint32_t len;
    int      ret = 0;

    DevAssert(instance_p != NULL);
    DevAssert(s1ap_mme_data_p != NULL);

    memset(&message, 0, sizeof(s1ap_message));

    message.direction     = S1AP_PDU_PR_initiatingMessage;
    message.procedureCode = S1ap_ProcedureCode_id_S1Setup;
    message.criticality   = S1ap_Criticality_reject;

    s1SetupRequest_p = &message.msg.s1ap_S1SetupRequestIEs;
    memset((void *)&plmnIdentity, 0, sizeof(S1ap_PLMNidentity_t));

    memset((void *)&ta, 0, sizeof(S1ap_SupportedTAs_Item_t));

    s1ap_mme_data_p->state = S1AP_ENB_STATE_WAITING;

    s1SetupRequest_p->global_ENB_ID.eNB_ID.present = S1ap_ENB_ID_PR_macroENB_ID;
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
        s1SetupRequest_p->presenceMask |= S1AP_S1SETUPREQUESTIES_ENBNAME_PRESENT;
        OCTET_STRING_fromBuf(&s1SetupRequest_p->eNBname, instance_p->eNB_name,
                             strlen(instance_p->eNB_name));
    }

    if (s1ap_eNB_encode_pdu(&message, &buffer, &len) < 0) {
        S1AP_ERROR("Failed to encode S1 setup request\n");
        return -1;
    }

    /* Non UE-Associated signalling -> stream = 0 */
    s1ap_eNB_itti_send_sctp_data_req(instance_p->instance, s1ap_mme_data_p->assoc_id, buffer, len, 0);

    return ret;
}
