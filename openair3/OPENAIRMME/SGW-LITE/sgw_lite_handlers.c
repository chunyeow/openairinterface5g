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
#include <string.h>

#include "intertask_interface.h"
#include "sgw_lite_defs.h"
#include "sgw_lite_context_manager.h"
#include "sgw_lite_handlers.h"

int sgw_lite_handle_create_session_request(
    SgwCreateSessionRequest *session_req_p) {

    mme_sgw_tunnel_t *new_endpoint;
    MessageDef       *message_p;

    /* Upon reception of create session request from MME,
     * S-GW should create UE, eNB and MME contexts and forward message to P-GW.
     */
    if (session_req_p->rat_type != RAT_TYPE_EUTRAN) {
        SGW_WARNING("Received session request with RAT != RAT_TYPE_EUTRAN: type %d\n",
                    session_req_p->rat_type);
    }

    /* As we are abstracting GTP-C transport, FTeid ip address is useless.
     * We just use the teid to identify MME tunnel. Normally we received either:
     * - ipv4 address if ipv4 flag is set
     * - ipv6 address if ipv6 flag is set
     * - ipv4 and ipv6 if both flags are set
     * Communication between MME and S-GW involves S11 interface so we are expecting
     * S11_MME_GTP_C (11) as interface_type.
     */
    if ((session_req_p->sender_fteid_for_cp.teid == 0) &&
        (session_req_p->sender_fteid_for_cp.interface_type != S11_MME_GTP_C)) {
        /* MME sent request with teid = 0. This is not valid... */
        SGW_WARNING("FTEID parameter mismatch\n");
        return -1;
    }
    /* We try to create endpoint for S11 interface. A NULL endpoint means that
     * either the teid is already in list of known teid or ENOMEM error has been
     * raised during malloc.
     */
    new_endpoint = sgw_lite_cm_create_s11_tunnel(session_req_p->sender_fteid_for_cp.teid);
    if (new_endpoint == NULL) {
        SGW_WARNING("Could not create new tunnel endpoint between S-GW and MME "
                    "for S11 abstraction\n");
        return -1;
    }

    /* Establishing EPS bearer. Requesting S1-U (GTPV1-U) task to create a
     * tunnel for S1 user plane interface. If status in response is successfull (0),
     * the tunnel endpoint is locally ready.
     */
    message_p = alloc_new_message(TASK_SGW_LITE, TASK_GTPV1_U, S1U_CREATE_TUNNEL);
    if (message_p == NULL) {
        mme_sgw_remove_s11_tunnel(new_endpoint);
        return -1;
    }

    return send_msg_to_task(TASK_GTPV1_U, message_p);
}

int sgw_lite_handle_s1u_endpoint_created(SgwS1UEndpointCreated *endpoint_created_p) {
    SgwCreateSessionResponse *create_session_response_p;
    MessageDef               *message_p;

    message_p = alloc_new_message(TASK_SGW_LITE, TASK_S1AP, SGW_CREATE_SESSION_RESPONSE);

    if (message_p == NULL) {
        return -1;
    }

    create_session_response_p = &message_p->msg.sgwCreateSessionResponse;

    memset(create_session_response_p, 0, sizeof(SgwCreateSessionResponse));

    create_session_response_p->teid = 1;

    /* Preparing to send create session response on S11 abstraction interface.
     *  we set the cause value regarding the S1-U bearer establishment result status.
     */
    if (endpoint_created_p->status == 0) {
        uint32_t address = 0x7F000001;
        create_session_response_p->bearer_context_created.s1u_sgw_fteid.teid = endpoint_created_p->teid;
        create_session_response_p->bearer_context_created.s1u_sgw_fteid.interface_type = S1_U_SGW_GTP_U;
        create_session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4 = 1;
        /* Should be filled in with S-GW S1-U local address. Running everything on localhost for now */
        create_session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address = address;

        /* Set the Cause information from bearer context created.
        * "Request accepted" is returned when the GTPv2 entity has accepted a control plane request.
        */
        create_session_response_p->bearer_context_created.cause = REQUEST_ACCEPTED;
    } else {
        create_session_response_p->bearer_context_created.cause = M_PDN_APN_NOT_ALLOWED;
    }

    return send_msg_to_task(TASK_S1AP, message_p);
}

int sgw_lite_handle_modify_bearer_request(SgwModifyBearerRequest *modify_bearer_p) {
    SgwModifyBearerResponse *modify_response_p;
    MessageDef               *message_p;

    message_p = alloc_new_message(TASK_SGW_LITE, TASK_S1AP, SGW_MODIFY_BEARER_RESPONSE);

    if (message_p == NULL) {
        return -1;
    }

    modify_response_p = &message_p->msg.sgwModifyBearerResponse;

    memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));

    return send_msg_to_task(TASK_S1AP, message_p);
}
