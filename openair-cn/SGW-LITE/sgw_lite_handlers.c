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

#include "assertions.h"
#include "common_types.h"
#include "intertask_interface.h"

#include "sgw_lite_defs.h"
#include "sgw_lite_handlers.h"
#include "sgw_lite_context_manager.h"
#include "sgw_lite.h"

extern sgw_app_t sgw_app;

int sgw_lite_handle_create_session_request(SgwCreateSessionRequest *session_req_p)
{
    mme_sgw_tunnel_t*                          new_endpoint                                  = NULL;
    s_plus_p_gw_eps_bearer_context_information_t* s_plus_p_gw_eps_bearer_context_information = NULL;
    sgw_eps_bearer_entry_t*                    eps_bearer_entry_p                              = NULL;
    MessageDef*                                message_p                                     = NULL;

    /* Upon reception of create session request from MME,
     * S-GW should create UE, eNB and MME contexts and forward message to P-GW.
     */
    if (session_req_p->rat_type != RAT_EUTRAN) {
        SPGW_APP_WARNING("Received session request with RAT != RAT_TYPE_EUTRAN: type %d\n",
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
        SPGW_APP_WARNING("F-TEID parameter mismatch\n");
        return -1;
    }
    new_endpoint = sgw_lite_cm_create_s11_tunnel(session_req_p->sender_fteid_for_cp.teid, sgw_lite_get_new_S11_tunnel_id());
    if (new_endpoint == NULL) {
        SPGW_APP_WARNING("Could not create new tunnel endpoint between S-GW and MME "
                    "for S11 abstraction\n");
        return -1;
    }

    SPGW_APP_DEBUG("Rx CREATE-SESSION-REQUEST MME S11 teid %u S-GW S11 teid %u APN %s EPS bearer Id %d\n", new_endpoint->remote_teid, new_endpoint->local_teid, session_req_p->apn, session_req_p->bearer_to_create.eps_bearer_id);
    SPGW_APP_DEBUG("                          IMSI %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",IMSI(&session_req_p->imsi));

    s_plus_p_gw_eps_bearer_context_information = sgw_lite_cm_create_bearer_context_information_in_collection(new_endpoint->local_teid);
    if (s_plus_p_gw_eps_bearer_context_information != NULL) {
        /* We try to create endpoint for S11 interface. A NULL endpoint means that
        * either the teid is already in list of known teid or ENOMEM error has been
        * raised during malloc.
        */

        //--------------------------------------------------
        // copy informations from create session request to bearer context information
        //--------------------------------------------------
        memcpy(s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.imsi.digit, session_req_p->imsi.digit, IMSI_DIGITS_MAX);
        memcpy(s_plus_p_gw_eps_bearer_context_information->pgw_eps_bearer_context_information.imsi.digit, session_req_p->imsi.digit, IMSI_DIGITS_MAX);
        s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.imsi_unauthenticated_indicator = 1;
        s_plus_p_gw_eps_bearer_context_information->pgw_eps_bearer_context_information.imsi_unauthenticated_indicator = 1;

        s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.mme_teid_for_S11               = session_req_p->sender_fteid_for_cp.teid;
        s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.s_gw_teid_for_S11_S4           = new_endpoint->local_teid;
        s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.trxn                           = session_req_p->trxn;
        s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.peer_ip                        = session_req_p->peer_ip;
        // may use ntohl or reverse, will see

        FTEID_T_2_IP_ADDRESS_T((&session_req_p->sender_fteid_for_cp) , (&s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.mme_ip_address_for_S11));

        //--------------------------------------
        // PDN connection
        //--------------------------------------
        /*pdn_connection = sgw_lite_cm_create_pdn_connection();

        if (pdn_connection == NULL) {
            // Malloc failed, may be ENOMEM error
            SPGW_APP_ERROR("Failed to create new PDN connection\n");
            return -1;
        }*/
        memset(&s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connection, 0, sizeof(sgw_pdn_connection_t));
        s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers = hashtbl_create(12, NULL, NULL);
        if ( s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers == NULL) {
            SPGW_APP_ERROR("Failed to create eps bearers collection object\n");
            DevMessage("Failed to create eps bearers collection object\n");
            return -1;
        }
        
        if (session_req_p->apn) {
        	s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connection.apn_in_use     =  strdup(session_req_p->apn);
        } else {
        	s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connection.apn_in_use     = "NO APN";
        }
        s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connection.default_bearer =  session_req_p->bearer_to_create.eps_bearer_id;

        //obj_hashtbl_insert(s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connections, pdn_connection->apn_in_use, strlen(pdn_connection->apn_in_use), pdn_connection);
        //--------------------------------------
        // EPS bearer entry
        //--------------------------------------
        eps_bearer_entry_p = sgw_lite_cm_create_eps_bearer_entry_in_collection(
        		s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
        		session_req_p->bearer_to_create.eps_bearer_id);

        sgw_lite_display_s11teid2mme_mappings();
        sgw_lite_display_s11_bearer_context_information_mapping();

        if (eps_bearer_entry_p == NULL) {
            SPGW_APP_ERROR("Failed to create new EPS bearer entry\n");
            // TO DO FREE new_bearer_context_information_p and by cascade...
            return -1;

        }
        eps_bearer_entry_p->eps_bearer_qos =  session_req_p->bearer_to_create.bearer_level_qos;


        //s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_informationteid = teid;

        /* Trying to insert the new tunnel into the tree.
         * If collision_p is not NULL (0), it means tunnel is already present.
         */

        //s_plus_p_gw_eps_bearer_context_information->sgw_eps_bearer_context_informations_gw_ip_address_for_S11_S4 =

        /* Establishing EPS bearer. Requesting S1-U (GTPV1-U) task to create a
        * tunnel for S1 user plane interface. If status in response is successfull (0),
        * the tunnel endpoint is locally ready.
        */
        message_p = alloc_new_message(TASK_SPGW_APP, GTPV1U_CREATE_TUNNEL_REQ);
        if (message_p == NULL) {
        	sgw_lite_cm_remove_s11_tunnel(new_endpoint->remote_teid);
            return -1;
        }

        message_p->msg.gtpv1uCreateTunnelReq.context_teid  = new_endpoint->local_teid;
        message_p->msg.gtpv1uCreateTunnelReq.eps_bearer_id = session_req_p->bearer_to_create.eps_bearer_id;
        SPGW_APP_DEBUG("Tx GTPV1U_CREATE_TUNNEL_REQ -> TASK_GTPV1_U, Context: S-GW S11 teid %u eps bearer id %d (from session req)\n",
        		message_p->msg.gtpv1uCreateTunnelReq.context_teid,
        		message_p->msg.gtpv1uCreateTunnelReq.eps_bearer_id);
        return send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_p);
    } else {
        SPGW_APP_WARNING("Could not create new transaction for SESSION_CREATE message\n");
        free(new_endpoint);
        return -1;
    }
}

int sgw_lite_handle_sgi_endpoint_created(SGICreateEndpointResp *resp_p)
{
    task_id_t to_task;

    SgwCreateSessionResponse                     *create_session_response_p        = NULL;
    s_plus_p_gw_eps_bearer_context_information_t *new_bearer_context_information_p = NULL;
    MessageDef                                   *message_p                        = NULL;
    hashtbl_rc_t                                  hash_rc;

    SPGW_APP_DEBUG("Rx SGI_CREATE_ENDPOINT_RESPONSE,Context: S11 teid %u, SGW S1U teid %u EPS bearer id %u\n",
    		resp_p->context_teid, resp_p->sgw_S1u_teid, resp_p->eps_bearer_id);
    hash_rc = hashtbl_get(sgw_app.s11_bearer_context_information_hashtable, resp_p->context_teid, (void**)&new_bearer_context_information_p);

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    message_p = alloc_new_message(TASK_SPGW_APP, SGW_CREATE_SESSION_RESPONSE);

    if (message_p == NULL) {
        return -1;
    }

    create_session_response_p = &message_p->msg.sgwCreateSessionResponse;

    if (hash_rc == HASH_TABLE_OK) {
        create_session_response_p->teid = new_bearer_context_information_p->sgw_eps_bearer_context_information.mme_teid_for_S11;

        /* Preparing to send create session response on S11 abstraction interface.
         *  we set the cause value regarding the S1-U bearer establishment result status.
         */
        if (resp_p->status == 0) {
            uint32_t address = sgw_app.sgw_ip_address_for_S1u_S12_S4_up;
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.teid           = resp_p->sgw_S1u_teid;
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.interface_type = S1_U_SGW_GTP_U;
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4           = 1;
            /* Should be filled in with S-GW S1-U local address. Running everything on localhost for now */
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address   = address;

            /* Set the Cause information from bearer context created.
             * "Request accepted" is returned when the GTPv2 entity has accepted a control plane request.
             */
            create_session_response_p->cause                        = REQUEST_ACCEPTED;
            create_session_response_p->bearer_context_created.cause = REQUEST_ACCEPTED;
        } else {
            create_session_response_p->cause                        = M_PDN_APN_NOT_ALLOWED;
            create_session_response_p->bearer_context_created.cause = M_PDN_APN_NOT_ALLOWED;
        }
        create_session_response_p->s11_sgw_teid.teid                    = resp_p->context_teid;
        create_session_response_p->bearer_context_created.eps_bearer_id = resp_p->eps_bearer_id;
        create_session_response_p->trxn                                 = new_bearer_context_information_p->sgw_eps_bearer_context_information.trxn;
        create_session_response_p->peer_ip                              = new_bearer_context_information_p->sgw_eps_bearer_context_information.peer_ip;
    } else {
        create_session_response_p->cause                        = CONTEXT_NOT_FOUND;
        create_session_response_p->bearer_context_created.cause = CONTEXT_NOT_FOUND;
    }

    SPGW_APP_DEBUG("Tx CREATE-SESSION-RESPONSE MME -> %s, teid %u S-GW teid %u S1U teid %u EPS bearer id %u status %d\n",
                   to_task == TASK_MME_APP ? "TASK_MME_APP" : "TASK_S11",
                   create_session_response_p->teid,
                   create_session_response_p->s11_sgw_teid.teid,
                   create_session_response_p->bearer_context_created.s1u_sgw_fteid.teid,
                   create_session_response_p->bearer_context_created.eps_bearer_id,
                   create_session_response_p->bearer_context_created.cause);
    return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
}

int sgw_lite_handle_gtpv1uCreateTunnelResp(Gtpv1uCreateTunnelResp *endpoint_created_p)
{
    task_id_t to_task;

    SgwCreateSessionResponse                          *create_session_response_p        = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_context_information_p = NULL;
    SGICreateEndpointReq                              *sgi_create_endpoint_req_p        = NULL;
    MessageDef                                        *message_p                        = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p               = NULL;
    hashtbl_rc_t                                       hash_rc;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx GTPV1U_CREATE_TUNNEL_RESP, Context S-GW S11 teid %u, S-GW S1U teid %u EPS bearer id %u status %d\n",
                   endpoint_created_p->context_teid,
                   endpoint_created_p->S1u_teid,
                   endpoint_created_p->eps_bearer_id,
                   endpoint_created_p->status);
    hash_rc = hashtbl_get(sgw_app.s11_bearer_context_information_hashtable, endpoint_created_p->context_teid, (void**)&new_bearer_context_information_p);

    if (hash_rc == HASH_TABLE_OK) {
        hash_rc = hashtbl_get (new_bearer_context_information_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers, endpoint_created_p->eps_bearer_id, (void**)&eps_bearer_entry_p);

        DevAssert(hash_rc == HASH_TABLE_OK);

        SPGW_APP_DEBUG("Updated eps_bearer_entry_p eps_b_id %u with SGW S1U teid %u\n", endpoint_created_p->eps_bearer_id, endpoint_created_p->S1u_teid);
        eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up = endpoint_created_p->S1u_teid;

        sgw_lite_display_s11_bearer_context_information_mapping();

        /* SEND IP_FW_CREATE_IP_ENDPOINT_REQUEST to FW_IP task */
        message_p = alloc_new_message(TASK_SPGW_APP, SGI_CREATE_ENDPOINT_REQUEST);
        if (message_p == NULL) {
            return -1;
        }
        sgi_create_endpoint_req_p = &message_p->msg.sgiCreateEndpointReq;
        // IP forward will forward packets to this teid
        sgi_create_endpoint_req_p->context_teid  = endpoint_created_p->context_teid;
        sgi_create_endpoint_req_p->sgw_S1u_teid  = endpoint_created_p->S1u_teid;
        sgi_create_endpoint_req_p->eps_bearer_id = endpoint_created_p->eps_bearer_id;

        //create_sgi_endpoint_req_p->pdn_type = new_bearer_context_information_p->
        // TO DO TFT, QOS
        return send_msg_to_task(TASK_FW_IP, INSTANCE_DEFAULT, message_p);
    } else {
        SPGW_APP_DEBUG("Rx SGW_S1U_ENDPOINT_CREATED, Context: teid %u NOT FOUND\n", endpoint_created_p->context_teid);
        message_p = alloc_new_message(TASK_SPGW_APP, SGW_CREATE_SESSION_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        create_session_response_p = &message_p->msg.sgwCreateSessionResponse;
        memset(create_session_response_p, 0, sizeof(SgwCreateSessionResponse));
        create_session_response_p->cause                        = CONTEXT_NOT_FOUND;
        create_session_response_p->bearer_context_created.cause = CONTEXT_NOT_FOUND;
        return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
}

int sgw_lite_handle_gtpv1uUpdateTunnelResp(Gtpv1uUpdateTunnelResp *endpoint_updated_p)
{
    SgwModifyBearerResponse                           *modify_response_p                = NULL;
    SGIUpdateEndpointReq                              *update_request_p                 = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_context_information_p = NULL;
    MessageDef                                        *message_p                        = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p               = NULL;
    hashtbl_rc_t                                       hash_rc;
    task_id_t                                          to_task;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx GTPV1U_UPDATE_TUNNEL_RESP, Context teid %u, SGW S1U teid %u, eNB S1U teid %u, EPS bearer id %u, status %d\n",
                   endpoint_updated_p->context_teid,
                   endpoint_updated_p->sgw_S1u_teid,
                   endpoint_updated_p->enb_S1u_teid,
                   endpoint_updated_p->eps_bearer_id,
                   endpoint_updated_p->status);


    hash_rc = hashtbl_get(sgw_app.s11_bearer_context_information_hashtable, endpoint_updated_p->context_teid, (void**)&new_bearer_context_information_p);

    if (hash_rc == HASH_TABLE_OK) {
    	hash_rc = hashtbl_get (new_bearer_context_information_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers, endpoint_updated_p->eps_bearer_id, (void**)&eps_bearer_entry_p);

        if ((hash_rc == HASH_TABLE_KEY_NOT_EXISTS) || (hash_rc == HASH_TABLE_BAD_PARAMETER_HASHTABLE)) {
            SPGW_APP_DEBUG("Sending SGW_MODIFY_BEARER_RESPONSE trxn %p bearer %u CONTEXT_NOT_FOUND (sgw_eps_bearers)\n",
                    new_bearer_context_information_p->sgw_eps_bearer_context_information.trxn,
                    endpoint_updated_p->eps_bearer_id);

            message_p = alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
            if (message_p == NULL) {
                return -1;
            }
            modify_response_p = &message_p->msg.sgwModifyBearerResponse;
            memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));
            modify_response_p->present                                      = MODIFY_BEARER_RESPONSE_REM;
            modify_response_p->choice.bearer_for_removal.eps_bearer_id      = endpoint_updated_p->eps_bearer_id;
            modify_response_p->choice.bearer_for_removal.cause              = CONTEXT_NOT_FOUND;
            modify_response_p->cause                                        = CONTEXT_NOT_FOUND;
            modify_response_p->trxn                                         = new_bearer_context_information_p->sgw_eps_bearer_context_information.trxn;

            return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
        } else if (hash_rc == HASH_TABLE_OK) {

            message_p = alloc_new_message(TASK_SPGW_APP, SGI_UPDATE_ENDPOINT_REQUEST);
            if (message_p == NULL) {
                return -1;
            }
            update_request_p = &message_p->msg.sgiUpdateEndpointReq;
            memset(update_request_p, 0, sizeof(SGIUpdateEndpointReq));
            update_request_p->context_teid = endpoint_updated_p->context_teid;
            update_request_p->sgw_S1u_teid = endpoint_updated_p->sgw_S1u_teid;
            update_request_p->enb_S1u_teid = endpoint_updated_p->enb_S1u_teid;
            update_request_p->eps_bearer_id = endpoint_updated_p->eps_bearer_id;

            return send_msg_to_task(TASK_FW_IP, INSTANCE_DEFAULT, message_p);
        }
    } else {
        SPGW_APP_DEBUG("Sending SGW_MODIFY_BEARER_RESPONSE trxn %p bearer %u CONTEXT_NOT_FOUND (s11_bearer_context_information_hashtable)\n",
                new_bearer_context_information_p->sgw_eps_bearer_context_information.trxn,
                endpoint_updated_p->eps_bearer_id);

        message_p = alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        modify_response_p = &message_p->msg.sgwModifyBearerResponse;
        memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));

        modify_response_p->present                                      = MODIFY_BEARER_RESPONSE_REM;
        modify_response_p->choice.bearer_for_removal.eps_bearer_id      = endpoint_updated_p->eps_bearer_id;
        modify_response_p->choice.bearer_for_removal.cause              = CONTEXT_NOT_FOUND;
        modify_response_p->cause                                        = CONTEXT_NOT_FOUND;
        modify_response_p->trxn                                         = new_bearer_context_information_p->sgw_eps_bearer_context_information.trxn;
        return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
    return -1;
}

int sgw_lite_handle_sgi_endpoint_updated(SGIUpdateEndpointResp *resp_p)
{
    SgwModifyBearerResponse                           *modify_response_p              = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_context_information_p = NULL;
    MessageDef                                        *message_p                      = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p               = NULL;
    hashtbl_rc_t                                       hash_rc;
    task_id_t                                          to_task;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE, Context teid %u, SGW S1U teid %u, eNB S1U teid %u, EPS bearer id %u, status %d\n",
                   resp_p->context_teid,
                   resp_p->sgw_S1u_teid,
                   resp_p->enb_S1u_teid,
                   resp_p->eps_bearer_id,
                   resp_p->status);

    message_p = alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
    if (message_p == NULL) {
        return -1;
    }
    modify_response_p = &message_p->msg.sgwModifyBearerResponse;
    memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));

    hash_rc = hashtbl_get(sgw_app.s11_bearer_context_information_hashtable, resp_p->context_teid, (void**)&new_bearer_context_information_p);


    if (hash_rc == HASH_TABLE_OK) {
        hash_rc = hashtbl_get (new_bearer_context_information_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
                               resp_p->eps_bearer_id,
                               (void**)&eps_bearer_entry_p);

        if ((hash_rc == HASH_TABLE_KEY_NOT_EXISTS) || (hash_rc == HASH_TABLE_BAD_PARAMETER_HASHTABLE)) {
            SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE: CONTEXT_NOT_FOUND (pdn_connection.sgw_eps_bearers context)\n");
            modify_response_p->teid                                         = resp_p->context_teid; // TO BE CHECKED IF IT IS THIS TEID
            modify_response_p->present                                      = MODIFY_BEARER_RESPONSE_REM;
            modify_response_p->choice.bearer_for_removal.eps_bearer_id      = resp_p->eps_bearer_id;
            modify_response_p->choice.bearer_for_removal.cause              = CONTEXT_NOT_FOUND;
            modify_response_p->cause                                        = CONTEXT_NOT_FOUND;
            modify_response_p->trxn                                         = 0;

            return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
        } else if (hash_rc == HASH_TABLE_OK) {
            SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE: REQUEST_ACCEPTED\n");

            // accept anyway
            modify_response_p->teid                                         = resp_p->context_teid; // TO BE CHECKED IF IT IS THIS TEID
            modify_response_p->present                              = MODIFY_BEARER_RESPONSE_MOD;
            modify_response_p->choice.bearer_modified.eps_bearer_id = resp_p->eps_bearer_id;
            modify_response_p->choice.bearer_modified.cause         = REQUEST_ACCEPTED;
            modify_response_p->cause                                = REQUEST_ACCEPTED;
            modify_response_p->trxn                                 = new_bearer_context_information_p->sgw_eps_bearer_context_information.trxn;
        }
        return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    } else {
        SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE: CONTEXT_NOT_FOUND (S11 context)\n");

        modify_response_p->teid                                         = resp_p->context_teid; // TO BE CHECKED IF IT IS THIS TEID
        modify_response_p->present                                      = MODIFY_BEARER_RESPONSE_REM;
        modify_response_p->choice.bearer_for_removal.eps_bearer_id      = resp_p->eps_bearer_id;
        modify_response_p->choice.bearer_for_removal.cause              = CONTEXT_NOT_FOUND;
        modify_response_p->cause                                        = CONTEXT_NOT_FOUND;
        modify_response_p->trxn                                         = 0;

        return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
}

int sgw_lite_handle_modify_bearer_request(SgwModifyBearerRequest *modify_bearer_p)
{
    SgwModifyBearerResponse                           *modify_response_p                = NULL;
    Gtpv1uUpdateTunnelReq                             *gtpv1u_update_tunnel_req_p       = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_context_information_p = NULL;
    MessageDef                                        *message_p                        = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p               = NULL;
    hashtbl_rc_t                                       hash_rc;
    task_id_t                                          to_task;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, teid %u, EPS bearer id %u\n",
                   modify_bearer_p->teid,
                   modify_bearer_p->bearer_context_to_modify.eps_bearer_id);

    sgw_lite_display_s11teid2mme_mappings();
    sgw_lite_display_s11_bearer_context_information_mapping();

    hash_rc = hashtbl_get(sgw_app.s11_bearer_context_information_hashtable, modify_bearer_p->teid, (void**)&new_bearer_context_information_p);

    if (hash_rc == HASH_TABLE_OK) {

        new_bearer_context_information_p->sgw_eps_bearer_context_information.pdn_connection.default_bearer = modify_bearer_p->bearer_context_to_modify.eps_bearer_id;
        new_bearer_context_information_p->sgw_eps_bearer_context_information.trxn = modify_bearer_p->trxn;

        hash_rc = hashtbl_is_key_exists (new_bearer_context_information_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers, modify_bearer_p->bearer_context_to_modify.eps_bearer_id);

        if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
            message_p = alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
            if (message_p == NULL) {
                return -1;
            }
            modify_response_p = &message_p->msg.sgwModifyBearerResponse;
            memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));
            modify_response_p->present                                      = MODIFY_BEARER_RESPONSE_REM;
            modify_response_p->choice.bearer_for_removal.eps_bearer_id      = modify_bearer_p->bearer_context_to_modify.eps_bearer_id;
            modify_response_p->choice.bearer_for_removal.cause              = CONTEXT_NOT_FOUND;
            modify_response_p->cause                                        = CONTEXT_NOT_FOUND;
            modify_response_p->trxn                                         = modify_bearer_p->trxn;
            return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
        } else if (hash_rc == HASH_TABLE_OK) {
            // TO DO
            hash_rc = hashtbl_get (new_bearer_context_information_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers, modify_bearer_p->bearer_context_to_modify.eps_bearer_id, (void**)&eps_bearer_entry_p);
            FTEID_T_2_IP_ADDRESS_T( (&modify_bearer_p->bearer_context_to_modify.s1_eNB_fteid) , (&eps_bearer_entry_p->enb_ip_address_for_S1u) );
            eps_bearer_entry_p->enb_teid_for_S1u = modify_bearer_p->bearer_context_to_modify.s1_eNB_fteid.teid;

            // UPDATE GTPV1U mapping tables with eNB references (teid, addresses)
            message_p = alloc_new_message(TASK_SPGW_APP, GTPV1U_UPDATE_TUNNEL_REQ);
            if (message_p == NULL) {
                return -1;
            }
            gtpv1u_update_tunnel_req_p = &message_p->msg.gtpv1uUpdateTunnelReq;
            memset(gtpv1u_update_tunnel_req_p, 0, sizeof(Gtpv1uUpdateTunnelReq));
            gtpv1u_update_tunnel_req_p->context_teid           = modify_bearer_p->teid;
            gtpv1u_update_tunnel_req_p->sgw_S1u_teid           = eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up;     ///< SGW S1U local Tunnel Endpoint Identifier
            gtpv1u_update_tunnel_req_p->enb_S1u_teid           = eps_bearer_entry_p->enb_teid_for_S1u;                ///< eNB S1U Tunnel Endpoint Identifier
            gtpv1u_update_tunnel_req_p->enb_ip_address_for_S1u = eps_bearer_entry_p->enb_ip_address_for_S1u;
            gtpv1u_update_tunnel_req_p->eps_bearer_id          = eps_bearer_entry_p->eps_bearer_id;
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->context_teid           = %u\n",modify_bearer_p->teid);
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->sgw_S1u_teid           = %u\n",gtpv1u_update_tunnel_req_p->sgw_S1u_teid);
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->enb_S1u_teid           = %u\n",gtpv1u_update_tunnel_req_p->enb_S1u_teid);
            //SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->enb_ip_address_for_S1u = %u\n",modify_bearer_p->enb_ip_address_for_S1u);
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->eps_bearer_id          = %u\n",gtpv1u_update_tunnel_req_p->eps_bearer_id);

            return send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_p);
        }
    } else {
        message_p = alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        modify_response_p = &message_p->msg.sgwModifyBearerResponse;

        modify_response_p->present                                      = MODIFY_BEARER_RESPONSE_REM;
        modify_response_p->choice.bearer_for_removal.eps_bearer_id      = modify_bearer_p->bearer_context_to_modify.eps_bearer_id;
        modify_response_p->choice.bearer_for_removal.cause              = CONTEXT_NOT_FOUND;
        modify_response_p->cause                                        = CONTEXT_NOT_FOUND;
        modify_response_p->trxn                                         = modify_bearer_p->trxn;
        return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
    return -1;
}

int sgw_lite_handle_delete_session_request(SgwDeleteSessionRequest *delete_session_req_p)
{
    task_id_t    to_task;
    hashtbl_rc_t hash_rc;

    SgwDeleteSessionResponse                     *delete_session_resp_p;
    MessageDef                                   *message_p     = NULL;
    s_plus_p_gw_eps_bearer_context_information_t *ctx_p         = NULL;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    message_p = alloc_new_message(TASK_SPGW_APP, SGW_DELETE_SESSION_RESPONSE);
    if (message_p == NULL) {
        return -1;
    }
    delete_session_resp_p = &message_p->msg.sgwDeleteSessionResponse;

    SPGW_APP_WARNING("Delete session handler needs to be completed...\n");

    if (delete_session_req_p->indication_flags & OI_FLAG) {
        SPGW_APP_DEBUG("OI flag is set for this message indicating the request"
                       "should be forwarded to P-GW entity\n");
    }

    hash_rc = hashtbl_get(sgw_app.s11_bearer_context_information_hashtable,
                          delete_session_req_p->teid,
                          (void**)&ctx_p);

    if (hash_rc == HASH_TABLE_OK) {
        if ((delete_session_req_p->sender_fteid_for_cp.ipv4 != 0) &&
            (delete_session_req_p->sender_fteid_for_cp.ipv6 != 0)) {
            /* Sender F-TEID IE present */
            if (delete_session_resp_p->teid != ctx_p->sgw_eps_bearer_context_information.mme_teid_for_S11) {
                delete_session_resp_p->teid  = ctx_p->sgw_eps_bearer_context_information.mme_teid_for_S11;
                delete_session_resp_p->cause = INVALID_PEER;
                SPGW_APP_DEBUG("Mismatch in MME Teid for CP\n");
            } else {
                delete_session_resp_p->teid = delete_session_req_p->sender_fteid_for_cp.teid;
            }
        } else {
            delete_session_resp_p->cause = REQUEST_ACCEPTED;
            delete_session_resp_p->teid  = ctx_p->sgw_eps_bearer_context_information.mme_teid_for_S11;
        }
        delete_session_resp_p->trxn    = delete_session_req_p->trxn;
        delete_session_resp_p->peer_ip = delete_session_req_p->peer_ip;

        return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    } else {
        /* Context not found... set the cause to CONTEXT_NOT_FOUND
         * 3GPP TS 29.274 #7.2.10.1
         */
        message_p = alloc_new_message(TASK_SPGW_APP, SGW_DELETE_SESSION_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        delete_session_resp_p = &message_p->msg.sgwDeleteSessionResponse;

        if ((delete_session_req_p->sender_fteid_for_cp.ipv4 == 0) &&
            (delete_session_req_p->sender_fteid_for_cp.ipv6 == 0)) {
            delete_session_resp_p->teid = 0;
        } else {
            delete_session_resp_p->teid = delete_session_req_p->sender_fteid_for_cp.teid;
        }
        delete_session_resp_p->cause   = CONTEXT_NOT_FOUND;
        delete_session_resp_p->trxn    = delete_session_req_p->trxn;
        delete_session_resp_p->peer_ip = delete_session_req_p->peer_ip;

        return send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }

    return -1;
}
