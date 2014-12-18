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
/*! \file sgw_lite_handlers.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#define SGW_LITE
#define SGW_LITE_HANDLERS_C

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <netinet/in.h>

#include "assertions.h"
#include "conversions.h"
#include "common_types.h"
#include "intertask_interface.h"
#include "mme_config.h"

#include "sgw_lite_defs.h"
#include "sgw_lite_handlers.h"
#include "sgw_lite_context_manager.h"
#include "sgw_lite.h"
#include "pgw_lite_paa.h"
#include "spgw_config.h"

extern sgw_app_t     sgw_app;
extern spgw_config_t spgw_config;

static uint32_t g_gtpv1u_teid = 0;

uint32_t sgw_get_new_teid(void)
{
    g_gtpv1u_teid = g_gtpv1u_teid + 1;
    return g_gtpv1u_teid;
}

int
sgw_lite_handle_create_session_request(
        const SgwCreateSessionRequest * const session_req_pP)
{
    mme_sgw_tunnel_t*                             new_endpoint_p                     = NULL;
    s_plus_p_gw_eps_bearer_context_information_t* s_plus_p_gw_eps_bearer_ctxt_info_p = NULL;
    sgw_eps_bearer_entry_t*                       eps_bearer_entry_p                 = NULL;
    MessageDef*                                   message_p                          = NULL;

    /* Upon reception of create session request from MME,
     * S-GW should create UE, eNB and MME contexts and forward message to P-GW.
     */
    if (session_req_pP->rat_type != RAT_EUTRAN) {
        SPGW_APP_WARN("Received session request with RAT != RAT_TYPE_EUTRAN: type %d\n",
                    session_req_pP->rat_type);
    }

    /* As we are abstracting GTP-C transport, FTeid ip address is useless.
     * We just use the teid to identify MME tunnel. Normally we received either:
     * - ipv4 address if ipv4 flag is set
     * - ipv6 address if ipv6 flag is set
     * - ipv4 and ipv6 if both flags are set
     * Communication between MME and S-GW involves S11 interface so we are expecting
     * S11_MME_GTP_C (11) as interface_type.
     */
    if ((session_req_pP->sender_fteid_for_cp.teid == 0) &&
            (session_req_pP->sender_fteid_for_cp.interface_type != S11_MME_GTP_C)) {
        /* MME sent request with teid = 0. This is not valid... */
        SPGW_APP_WARN("F-TEID parameter mismatch\n");
        return -1;
    }
    new_endpoint_p = sgw_lite_cm_create_s11_tunnel(
            session_req_pP->sender_fteid_for_cp.teid,
            sgw_lite_get_new_S11_tunnel_id());
    if (new_endpoint_p == NULL) {
        SPGW_APP_WARN("Could not create new tunnel endpoint between S-GW and MME "
                    "for S11 abstraction\n");
        return -1;
    }

    SPGW_APP_DEBUG("Rx CREATE-SESSION-REQUEST MME S11 teid %u S-GW S11 teid %u APN %s EPS bearer Id %d\n",
        new_endpoint_p->remote_teid,
        new_endpoint_p->local_teid,
        session_req_pP->apn,
        session_req_pP->bearer_to_create.eps_bearer_id);
    SPGW_APP_DEBUG("                          IMSI %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c\n",
        IMSI(&session_req_pP->imsi));

    s_plus_p_gw_eps_bearer_ctxt_info_p = sgw_lite_cm_create_bearer_context_information_in_collection(new_endpoint_p->local_teid);
    if (s_plus_p_gw_eps_bearer_ctxt_info_p != NULL) {
        /* We try to create endpoint for S11 interface. A NULL endpoint means that
        * either the teid is already in list of known teid or ENOMEM error has been
        * raised during malloc.
        */

        //--------------------------------------------------
        // copy informations from create session request to bearer context information
        //--------------------------------------------------
        memcpy(s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.imsi.digit, session_req_pP->imsi.digit, IMSI_DIGITS_MAX);
        memcpy(s_plus_p_gw_eps_bearer_ctxt_info_p->pgw_eps_bearer_context_information.imsi.digit, session_req_pP->imsi.digit, IMSI_DIGITS_MAX);
        s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.imsi_unauthenticated_indicator = 1;
        s_plus_p_gw_eps_bearer_ctxt_info_p->pgw_eps_bearer_context_information.imsi_unauthenticated_indicator = 1;

        s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_teid_for_S11               = session_req_pP->sender_fteid_for_cp.teid;
        s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.s_gw_teid_for_S11_S4           = new_endpoint_p->local_teid;
        s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn                           = session_req_pP->trxn;
        s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.peer_ip                        = session_req_pP->peer_ip;
        // may use ntohl or reverse, will see

        FTEID_T_2_IP_ADDRESS_T(
            (&session_req_pP->sender_fteid_for_cp) ,
            (&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_ip_address_for_S11));

        //--------------------------------------
        // PDN connection
        //--------------------------------------
        /*pdn_connection = sgw_lite_cm_create_pdn_connection();

        if (pdn_connection == NULL) {
            // Malloc failed, may be ENOMEM error
            SPGW_APP_ERROR("Failed to create new PDN connection\n");
            return -1;
        }*/
        memset(&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection, 0, sizeof(sgw_pdn_connection_t));
        s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers = hashtable_create(12, NULL, NULL);
        if ( s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers == NULL) {
            SPGW_APP_ERROR("Failed to create eps bearers collection object\n");
            DevMessage("Failed to create eps bearers collection object\n");
            return -1;
        }

        if (session_req_pP->apn) {
        	s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.apn_in_use     =  strdup(session_req_pP->apn);
        } else {
        	s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.apn_in_use     = "NO APN";
        }
        s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.default_bearer =  session_req_pP->bearer_to_create.eps_bearer_id;

        //obj_hashtable_insert(s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connections, pdn_connection->apn_in_use, strlen(pdn_connection->apn_in_use), pdn_connection);
        //--------------------------------------
        // EPS bearer entry
        //--------------------------------------
        eps_bearer_entry_p = sgw_lite_cm_create_eps_bearer_entry_in_collection(
        		s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
        		session_req_pP->bearer_to_create.eps_bearer_id);

        sgw_lite_display_s11teid2mme_mappings();
        sgw_lite_display_s11_bearer_context_information_mapping();

        if (eps_bearer_entry_p == NULL) {
            SPGW_APP_ERROR("Failed to create new EPS bearer entry\n");
            // TO DO FREE new_bearer_ctxt_info_p and by cascade...
            return -1;

        }
        eps_bearer_entry_p->eps_bearer_qos =  session_req_pP->bearer_to_create.bearer_level_qos;


        //s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_informationteid = teid;

        /* Trying to insert the new tunnel into the tree.
         * If collision_p is not NULL (0), it means tunnel is already present.
         */

        //s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_informations_gw_ip_address_for_S11_S4 =

        memcpy(&s_plus_p_gw_eps_bearer_ctxt_info_p->sgw_eps_bearer_context_information.saved_message,
                session_req_pP,
                sizeof(SgwCreateSessionRequest));

        /* Establishing EPS bearer. Requesting S1-U (GTPV1-U) task to create a
        * tunnel for S1 user plane interface. If status in response is successfull (0),
        * the tunnel endpoint is locally ready.
        */
        message_p = itti_alloc_new_message(TASK_SPGW_APP, GTPV1U_CREATE_TUNNEL_REQ);
        if (message_p == NULL) {
            sgw_lite_cm_remove_s11_tunnel(new_endpoint_p->remote_teid);
            return -1;
        }
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
        {
            Gtpv1uCreateTunnelResp           createTunnelResp;

            createTunnelResp.context_teid  = new_endpoint_p->local_teid;
            createTunnelResp.eps_bearer_id = session_req_pP->bearer_to_create.eps_bearer_id;
            createTunnelResp.status        = 0x00;
            createTunnelResp.S1u_teid      = sgw_get_new_teid();
            sgw_lite_handle_gtpv1uCreateTunnelResp(&createTunnelResp);
        }
#else
        message_p->ittiMsg.gtpv1uCreateTunnelReq.context_teid  = new_endpoint_p->local_teid;
        message_p->ittiMsg.gtpv1uCreateTunnelReq.eps_bearer_id = session_req_pP->bearer_to_create.eps_bearer_id;
        SPGW_APP_DEBUG("Tx GTPV1U_CREATE_TUNNEL_REQ -> TASK_GTPV1_U, Context: S-GW S11 teid %u eps bearer id %d (from session req)\n",
            message_p->ittiMsg.gtpv1uCreateTunnelReq.context_teid,
            message_p->ittiMsg.gtpv1uCreateTunnelReq.eps_bearer_id);
        return itti_send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_p);
#endif
    } else {
        SPGW_APP_WARN("Could not create new transaction for SESSION_CREATE message\n");
        free(new_endpoint_p);
        new_endpoint_p = NULL;
        return -1;
    }
}



int
sgw_lite_handle_sgi_endpoint_created(
        const SGICreateEndpointResp * const resp_pP)
{
    task_id_t to_task;

    SgwCreateSessionResponse                     *create_session_response_p = NULL;
    s_plus_p_gw_eps_bearer_context_information_t *new_bearer_ctxt_info_p    = NULL;
    MessageDef                                   *message_p                 = NULL;
    hashtable_rc_t                                hash_rc;

    SPGW_APP_DEBUG("Rx SGI_CREATE_ENDPOINT_RESPONSE,Context: S11 teid %u, SGW S1U teid %u EPS bearer id %u\n",
            resp_pP->context_teid, resp_pP->sgw_S1u_teid, resp_pP->eps_bearer_id);
    hash_rc = hashtable_get(sgw_app.s11_bearer_context_information_hashtable, resp_pP->context_teid, (void**)&new_bearer_ctxt_info_p);

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_CREATE_SESSION_RESPONSE);

    if (message_p == NULL) {
        return -1;
    }

    create_session_response_p = &message_p->ittiMsg.sgwCreateSessionResponse;
    memset(create_session_response_p, 0, sizeof(SgwCreateSessionResponse));

    if (hash_rc == HASH_TABLE_OK) {
        create_session_response_p->teid = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.mme_teid_for_S11;

        /* Preparing to send create session response on S11 abstraction interface.
         *  we set the cause value regarding the S1-U bearer establishment result status.
         */
        if (resp_pP->status == 0) {
            uint32_t address = sgw_app.sgw_ip_address_for_S1u_S12_S4_up;
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.teid           = resp_pP->sgw_S1u_teid;
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.interface_type = S1_U_SGW_GTP_U;
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4           = 1;
            /* Should be filled in with S-GW S1-U local address. Running everything on localhost for now */
            create_session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address   = address;


            create_session_response_p->ambr.br_dl = 100000000;
            create_session_response_p->ambr.br_ul = 40000000;

#if defined(ENABLE_USE_GTPU_IN_KERNEL)
            {
                sgw_eps_bearer_entry_t*                       eps_bearer_entry_p                 = NULL;
                hash_rc = hashtable_get (new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
                        resp_pP->eps_bearer_id,
                        (void**)&eps_bearer_entry_p);

                if ((hash_rc == HASH_TABLE_KEY_NOT_EXISTS) || (hash_rc == HASH_TABLE_BAD_PARAMETER_HASHTABLE)) {
                    SPGW_APP_ERROR("ERROR UNABLE TO GET EPS BEARER ENTRY\n");
                } else {
                    AssertFatal(sizeof(eps_bearer_entry_p->paa) == sizeof(resp_pP->paa), "Mismatch in lengths");  // sceptic mode
                    memcpy(&eps_bearer_entry_p->paa,
                                    &resp_pP->paa,
                                    sizeof(PAA_t));
                }
            }
#endif
            memcpy(&create_session_response_p->paa,
                    &resp_pP->paa,
                    sizeof(PAA_t));
            /* Set the Cause information from bearer context created.
             * "Request accepted" is returned when the GTPv2 entity has accepted a control plane request.
             */
            create_session_response_p->cause                        = REQUEST_ACCEPTED;
            create_session_response_p->bearer_context_created.cause = REQUEST_ACCEPTED;
        } else {
            create_session_response_p->cause                        = M_PDN_APN_NOT_ALLOWED;
            create_session_response_p->bearer_context_created.cause = M_PDN_APN_NOT_ALLOWED;
        }
        create_session_response_p->s11_sgw_teid.teid                    = resp_pP->context_teid;
        create_session_response_p->bearer_context_created.eps_bearer_id = resp_pP->eps_bearer_id;
        create_session_response_p->trxn                                 = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
        create_session_response_p->peer_ip                              = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.peer_ip;
    } else {
        create_session_response_p->cause                        = CONTEXT_NOT_FOUND;
        create_session_response_p->bearer_context_created.cause = CONTEXT_NOT_FOUND;
    }

    SPGW_APP_DEBUG("Tx CREATE-SESSION-RESPONSE MME -> %s, S11 MME teid %u S11 S-GW teid %u S1U teid %u S1U addr 0x%x EPS bearer id %u status %d\n",
                   to_task == TASK_MME_APP ? "TASK_MME_APP" : "TASK_S11",
                   create_session_response_p->teid,
                   create_session_response_p->s11_sgw_teid.teid,
                   create_session_response_p->bearer_context_created.s1u_sgw_fteid.teid,
                   create_session_response_p->bearer_context_created.s1u_sgw_fteid.ipv4_address,
                   create_session_response_p->bearer_context_created.eps_bearer_id,
                   create_session_response_p->bearer_context_created.cause);
    return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
}



int
sgw_lite_handle_gtpv1uCreateTunnelResp(
        const Gtpv1uCreateTunnelResp * const endpoint_created_pP)
{
    task_id_t to_task;

    SgwCreateSessionResponse                          *create_session_response_p = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_ctxt_info_p    = NULL;
    SGICreateEndpointReq                              *sgi_create_endpoint_req_p = NULL;
    MessageDef                                        *message_p                 = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p        = NULL;
    hashtable_rc_t                                     hash_rc;
    struct in_addr                                     inaddr ;
    struct in6_addr                                    in6addr                   = IN6ADDR_ANY_INIT;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx GTPV1U_CREATE_TUNNEL_RESP, Context S-GW S11 teid %u, S-GW S1U teid %u EPS bearer id %u status %d\n",
            endpoint_created_pP->context_teid,
            endpoint_created_pP->S1u_teid,
            endpoint_created_pP->eps_bearer_id,
            endpoint_created_pP->status);
    hash_rc = hashtable_get(
            sgw_app.s11_bearer_context_information_hashtable,
            endpoint_created_pP->context_teid,
            (void**)&new_bearer_ctxt_info_p);

    if (hash_rc == HASH_TABLE_OK) {

        hash_rc = hashtable_get (
                new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
                endpoint_created_pP->eps_bearer_id,
                (void**)&eps_bearer_entry_p);

        DevAssert(hash_rc == HASH_TABLE_OK);

        SPGW_APP_DEBUG("Updated eps_bearer_entry_p eps_b_id %u with SGW S1U teid %u\n",
                endpoint_created_pP->eps_bearer_id,
                endpoint_created_pP->S1u_teid);
        eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up = endpoint_created_pP->S1u_teid;

        sgw_lite_display_s11_bearer_context_information_mapping();

#if defined (ENABLE_USE_GTPU_IN_KERNEL)
        SGICreateEndpointResp  sgi_create_endpoint_resp;

        memset(&sgi_create_endpoint_resp, 0, sizeof(SGICreateEndpointResp));
        // IP forward will forward packets to this teid
        sgi_create_endpoint_resp.context_teid  = endpoint_created_pP->context_teid;
        sgi_create_endpoint_resp.sgw_S1u_teid  = endpoint_created_pP->S1u_teid;
        sgi_create_endpoint_resp.eps_bearer_id = endpoint_created_pP->eps_bearer_id;

        // TO DO NOW
        sgi_create_endpoint_resp.paa.pdn_type = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.saved_message.pdn_type;
        switch (sgi_create_endpoint_resp.paa.pdn_type) {
            case IPv4_OR_v6:
                if (pgw_lite_get_free_ipv4_paa_address(&inaddr) == 0) {
                    IN_ADDR_TO_BUFFER(inaddr, sgi_create_endpoint_resp.paa.ipv4_address);
                } else {
                    SPGW_APP_WARN("Failed to allocate IPv4 PAA for PDN type IPv4_OR_v6\n");
                    if (pgw_lite_get_free_ipv6_paa_prefix(&in6addr) == 0) {
                        IN6_ADDR_TO_BUFFER(in6addr, sgi_create_endpoint_resp.paa.ipv6_address);
                    } else {
                        SPGW_APP_ERROR("Failed to allocate IPv6 PAA for PDN type IPv4_OR_v6\n");
                    }
                }
                break;
            case IPv4:
                if (pgw_lite_get_free_ipv4_paa_address(&inaddr) == 0) {
                    IN_ADDR_TO_BUFFER(inaddr, sgi_create_endpoint_resp.paa.ipv4_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv4 PAA for PDN type IPv4\n");
                }
                break;
            case IPv6:
                if (pgw_lite_get_free_ipv6_paa_prefix(&in6addr) == 0) {
                    IN6_ADDR_TO_BUFFER(in6addr, sgi_create_endpoint_resp.paa.ipv6_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv6 PAA for PDN type IPv6\n");
                }
                break;
            case IPv4_AND_v6:
                if (pgw_lite_get_free_ipv4_paa_address(&inaddr) == 0) {
                    IN_ADDR_TO_BUFFER(inaddr, sgi_create_endpoint_resp.paa.ipv4_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv4 PAA for PDN type IPv4_AND_v6\n");
                }
                if (pgw_lite_get_free_ipv6_paa_prefix(&in6addr) == 0) {
                    IN6_ADDR_TO_BUFFER(in6addr, sgi_create_endpoint_resp.paa.ipv6_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv6 PAA for PDN type IPv4_AND_v6\n");
                }
                break;
            default:
                AssertFatal(0,"BAD paa.pdn_type %d", sgi_create_endpoint_resp.paa.pdn_type);
                break;
        }
        sgi_create_endpoint_resp.status         = SGI_STATUS_OK;
        sgw_lite_handle_sgi_endpoint_created(&sgi_create_endpoint_resp);
#else
        /* SEND IP_FW_CREATE_IP_ENDPOINT_REQUEST to FW_IP task */
        message_p = itti_alloc_new_message(TASK_SPGW_APP, SGI_CREATE_ENDPOINT_REQUEST);
        if (message_p == NULL) {
            return -1;
        }

        sgi_create_endpoint_req_p = &message_p->ittiMsg.sgiCreateEndpointReq;
        memset(sgi_create_endpoint_req_p, 0, sizeof(SGICreateEndpointReq));
        // IP forward will forward packets to this teid
        sgi_create_endpoint_req_p->context_teid  = endpoint_created_pP->context_teid;
        sgi_create_endpoint_req_p->sgw_S1u_teid  = endpoint_created_pP->S1u_teid;
        sgi_create_endpoint_req_p->eps_bearer_id = endpoint_created_pP->eps_bearer_id;

        // TO DO NOW
        sgi_create_endpoint_req_p->paa.pdn_type = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.saved_message.pdn_type;
        switch (sgi_create_endpoint_req_p->paa.pdn_type) {
            case IPv4_OR_v6:
                if (pgw_lite_get_free_ipv4_paa_address(&inaddr) == 0) {
                    IN_ADDR_TO_BUFFER(inaddr, sgi_create_endpoint_req_p->paa.ipv4_address);
                } else {
                    SPGW_APP_WARN("Failed to allocate IPv4 PAA for PDN type IPv4_OR_v6\n");
                    if (pgw_lite_get_free_ipv6_paa_prefix(&in6addr) == 0) {
                        IN6_ADDR_TO_BUFFER(in6addr, sgi_create_endpoint_req_p->paa.ipv6_address);
                    } else {
                        SPGW_APP_ERROR("Failed to allocate IPv6 PAA for PDN type IPv4_OR_v6\n");
                    }
                }
                break;
            case IPv4:
                if (pgw_lite_get_free_ipv4_paa_address(&inaddr) == 0) {
                    IN_ADDR_TO_BUFFER(inaddr, sgi_create_endpoint_req_p->paa.ipv4_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv4 PAA for PDN type IPv4\n");
                }
                break;
            case IPv6:
                if (pgw_lite_get_free_ipv6_paa_prefix(&in6addr) == 0) {
                    IN6_ADDR_TO_BUFFER(in6addr, sgi_create_endpoint_req_p->paa.ipv6_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv6 PAA for PDN type IPv6\n");
                }
                break;
            case IPv4_AND_v6:
                if (pgw_lite_get_free_ipv4_paa_address(&inaddr) == 0) {
                    IN_ADDR_TO_BUFFER(inaddr, sgi_create_endpoint_req_p->paa.ipv4_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv4 PAA for PDN type IPv4_AND_v6\n");
                }
                if (pgw_lite_get_free_ipv6_paa_prefix(&in6addr) == 0) {
                    IN6_ADDR_TO_BUFFER(in6addr, sgi_create_endpoint_req_p->paa.ipv6_address);
                } else {
                    SPGW_APP_ERROR("Failed to allocate IPv6 PAA for PDN type IPv4_AND_v6\n");
                }
                break;
            default:
                AssertFatal(0,"BAD paa.pdn_type %d", sgi_create_endpoint_req_p->paa.pdn_type);
                break;
        }

        // TO DO TFT, QOS
        return itti_send_msg_to_task(TASK_FW_IP, INSTANCE_DEFAULT, message_p);
#endif
    } else {
        SPGW_APP_DEBUG("Rx SGW_S1U_ENDPOINT_CREATED, Context: teid %u NOT FOUND\n", endpoint_created_pP->context_teid);
        message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_CREATE_SESSION_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        create_session_response_p = &message_p->ittiMsg.sgwCreateSessionResponse;
        memset(create_session_response_p, 0, sizeof(SgwCreateSessionResponse));
        create_session_response_p->cause                        = CONTEXT_NOT_FOUND;
        create_session_response_p->bearer_context_created.cause = CONTEXT_NOT_FOUND;
        return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
}



int
sgw_lite_handle_gtpv1uUpdateTunnelResp(
        const Gtpv1uUpdateTunnelResp * const endpoint_updated_pP)
{
    SgwModifyBearerResponse                           *modify_response_p      = NULL;
    SGIUpdateEndpointReq                              *update_request_p       = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_ctxt_info_p = NULL;
    MessageDef                                        *message_p              = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p     = NULL;
    hashtable_rc_t                                     hash_rc;
    task_id_t                                          to_task;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx GTPV1U_UPDATE_TUNNEL_RESP, Context teid %u, SGW S1U teid %u, eNB S1U teid %u, EPS bearer id %u, status %d\n",
                   endpoint_updated_pP->context_teid,
                   endpoint_updated_pP->sgw_S1u_teid,
                   endpoint_updated_pP->enb_S1u_teid,
                   endpoint_updated_pP->eps_bearer_id,
                   endpoint_updated_pP->status);


    hash_rc = hashtable_get(sgw_app.s11_bearer_context_information_hashtable, endpoint_updated_pP->context_teid, (void**)&new_bearer_ctxt_info_p);

    if (hash_rc == HASH_TABLE_OK) {
    	hash_rc = hashtable_get (new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers, endpoint_updated_pP->eps_bearer_id, (void**)&eps_bearer_entry_p);

        if ((hash_rc == HASH_TABLE_KEY_NOT_EXISTS) || (hash_rc == HASH_TABLE_BAD_PARAMETER_HASHTABLE)) {
            SPGW_APP_DEBUG("Sending SGW_MODIFY_BEARER_RESPONSE trxn %p bearer %u CONTEXT_NOT_FOUND (sgw_eps_bearers)\n",
                    new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn,
                    endpoint_updated_pP->eps_bearer_id);

            message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
            if (message_p == NULL) {
                return -1;
            }
            modify_response_p = &message_p->ittiMsg.sgwModifyBearerResponse;
            memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));
            modify_response_p->bearer_present                                 = MODIFY_BEARER_RESPONSE_REM;
            modify_response_p->bearer_choice.bearer_for_removal.eps_bearer_id = endpoint_updated_pP->eps_bearer_id;
            modify_response_p->bearer_choice.bearer_for_removal.cause         = CONTEXT_NOT_FOUND;
            modify_response_p->cause                                          = CONTEXT_NOT_FOUND;
            modify_response_p->trxn                                           = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;

            return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
        } else if (hash_rc == HASH_TABLE_OK) {

            message_p = itti_alloc_new_message(TASK_SPGW_APP, SGI_UPDATE_ENDPOINT_REQUEST);
            if (message_p == NULL) {
                return -1;
            }
            update_request_p = &message_p->ittiMsg.sgiUpdateEndpointReq;
            memset(update_request_p, 0, sizeof(SGIUpdateEndpointReq));
            update_request_p->context_teid  = endpoint_updated_pP->context_teid;
            update_request_p->sgw_S1u_teid  = endpoint_updated_pP->sgw_S1u_teid;
            update_request_p->enb_S1u_teid  = endpoint_updated_pP->enb_S1u_teid;
            update_request_p->eps_bearer_id = endpoint_updated_pP->eps_bearer_id;

            return itti_send_msg_to_task(TASK_FW_IP, INSTANCE_DEFAULT, message_p);
        }
    } else {
        SPGW_APP_DEBUG("Sending SGW_MODIFY_BEARER_RESPONSE trxn %p bearer %u CONTEXT_NOT_FOUND (s11_bearer_context_information_hashtable)\n",
                new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn,
                endpoint_updated_pP->eps_bearer_id);

        message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        modify_response_p = &message_p->ittiMsg.sgwModifyBearerResponse;
        memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));

        modify_response_p->bearer_present                                 = MODIFY_BEARER_RESPONSE_REM;
        modify_response_p->bearer_choice.bearer_for_removal.eps_bearer_id = endpoint_updated_pP->eps_bearer_id;
        modify_response_p->bearer_choice.bearer_for_removal.cause         = CONTEXT_NOT_FOUND;
        modify_response_p->cause                                          = CONTEXT_NOT_FOUND;
        modify_response_p->trxn                                           = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
        return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
    return -1;
}



int
sgw_lite_handle_sgi_endpoint_updated(
        const SGIUpdateEndpointResp * const resp_pP)
{
    SgwModifyBearerResponse                           *modify_response_p      = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_ctxt_info_p = NULL;
    MessageDef                                        *message_p              = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p     = NULL;
    hashtable_rc_t                                     hash_rc;
    task_id_t                                          to_task;
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
    static uint8_t                                     iptable_uplink_remove_gtpu = FALSE;
#endif

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE, Context teid %u, SGW S1U teid %u, eNB S1U teid %u, EPS bearer id %u, status %d\n",
                   resp_pP->context_teid,
                   resp_pP->sgw_S1u_teid,
                   resp_pP->enb_S1u_teid,
                   resp_pP->eps_bearer_id,
                   resp_pP->status);

    message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
    if (message_p == NULL) {
        return -1;
    }
    modify_response_p = &message_p->ittiMsg.sgwModifyBearerResponse;
    memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));

    hash_rc = hashtable_get(sgw_app.s11_bearer_context_information_hashtable,
            resp_pP->context_teid,
            (void**)&new_bearer_ctxt_info_p);


    if (hash_rc == HASH_TABLE_OK) {
        hash_rc = hashtable_get (new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
                               resp_pP->eps_bearer_id,
                               (void**)&eps_bearer_entry_p);

        if ((hash_rc == HASH_TABLE_KEY_NOT_EXISTS) || (hash_rc == HASH_TABLE_BAD_PARAMETER_HASHTABLE)) {
            SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE: CONTEXT_NOT_FOUND (pdn_connection.sgw_eps_bearers context)\n");
            modify_response_p->teid                                           = resp_pP->context_teid; // TO BE CHECKED IF IT IS THIS TEID
            modify_response_p->bearer_present                                 = MODIFY_BEARER_RESPONSE_REM;
            modify_response_p->bearer_choice.bearer_for_removal.eps_bearer_id = resp_pP->eps_bearer_id;
            modify_response_p->bearer_choice.bearer_for_removal.cause         = CONTEXT_NOT_FOUND;
            modify_response_p->cause                                          = CONTEXT_NOT_FOUND;
            modify_response_p->trxn                                           = 0;

            return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
        } else if (hash_rc == HASH_TABLE_OK) {
            SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE: REQUEST_ACCEPTED\n");

            // accept anyway
            modify_response_p->teid                                                 = resp_pP->context_teid; // TO BE CHECKED IF IT IS THIS TEID
            modify_response_p->bearer_present                                       = MODIFY_BEARER_RESPONSE_MOD;
            modify_response_p->bearer_choice.bearer_contexts_modified.eps_bearer_id = resp_pP->eps_bearer_id;
            modify_response_p->bearer_choice.bearer_contexts_modified.cause         = REQUEST_ACCEPTED;
            modify_response_p->cause                                                = REQUEST_ACCEPTED;
            modify_response_p->trxn                                                 = new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn;
#if defined (ENABLE_USE_GTPU_IN_KERNEL)
            char             cmd[256];
            int              ret;
            ret = snprintf(cmd,
                    256,
                    "iptables -t mangle -A POSTROUTING -d %u.%u.%u.%u -m mark --mark %u -j GTPUAH --own-ip %u.%u.%u.%u --own-tun %u --peer-ip %u.%u.%u.%u --peer-tun %u --action add",
                    eps_bearer_entry_p->paa.ipv4_address[0],
                    eps_bearer_entry_p->paa.ipv4_address[1],
                    eps_bearer_entry_p->paa.ipv4_address[2],
                    eps_bearer_entry_p->paa.ipv4_address[3],
                    eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up,
                    sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x000000FF,
                    (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x0000FF00) >> 8,
                    (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x00FF0000) >> 16,
                    (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0xFF000000) >> 24,
                    eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up,
                    eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[0],
                    eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[1],
                    eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[2],
                    eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[3],
                    eps_bearer_entry_p->enb_teid_for_S1u
                    );
            if ((ret < 0) || (ret > 256)) {
                SPGW_APP_ERROR("ERROR in preparing downlink tunnel, tune string length\n");
                exit (-1);
            }
            //use API when prototype validated
            ret = spgw_system(cmd, SPGW_ABORT_ON_ERROR, __FILE__, __LINE__);
            if (ret < 0) {
                SPGW_APP_ERROR("ERROR in setting up downlink TUNNEL\n");
            }

            // if default bearer
#warning "TODO define constant for default eps_bearer id"
            if ((resp_pP->eps_bearer_id == 5) && (spgw_config.pgw_config.pgw_masquerade_SGI == 0)) {
                ret = snprintf(cmd,
                                256,
                                "iptables -t mangle -A POSTROUTING -d %u.%u.%u.%u -m mark --mark 0 -j GTPUAH --own-ip %u.%u.%u.%u --own-tun %u --peer-ip %u.%u.%u.%u --peer-tun %u --action add",
                                eps_bearer_entry_p->paa.ipv4_address[0],
                                eps_bearer_entry_p->paa.ipv4_address[1],
                                eps_bearer_entry_p->paa.ipv4_address[2],
                                eps_bearer_entry_p->paa.ipv4_address[3],
                                sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x000000FF,
                                (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x0000FF00) >> 8,
                                (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x00FF0000) >> 16,
                                (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0xFF000000) >> 24,
                                eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up,
                                eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[0],
                                eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[1],
                                eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[2],
                                eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[3],
                                eps_bearer_entry_p->enb_teid_for_S1u
                 );
                 if ((ret < 0) || (ret > 256)) {
                     SPGW_APP_ERROR("ERROR in preparing downlink tunnel, tune string length\n");
                     exit (-1);
                 }
                 //use API when prototype validated
                 ret = spgw_system(cmd, SPGW_ABORT_ON_ERROR, __FILE__, __LINE__);
                 if (ret < 0) {
                     SPGW_APP_ERROR("ERROR in setting up downlink TUNNEL\n");
                 }
            }

            if (iptable_uplink_remove_gtpu == FALSE) {
                ret = snprintf(cmd,
                               256,
                               "iptables -t raw -A OUTPUT -s %u.%u.%u.%u -d %u.%u.%u.%u -p udp --dport 2152 -j GTPURH --own-ip %u.%u.%u.%u --own-tun %u --peer-ip %u.%u.%u.%u --peer-tun %u --action remove",
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[0],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[1],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[2],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[3],
                               sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x000000FF,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x0000FF00) >> 8,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x00FF0000) >> 16,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0xFF000000) >> 24,
                               sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x000000FF,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x0000FF00) >> 8,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x00FF0000) >> 16,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0xFF000000) >> 24,
                               eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up,
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[0],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[1],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[2],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[3],
                               eps_bearer_entry_p->enb_teid_for_S1u);

                if ((ret < 0) || (ret > 256)) {
                    SPGW_APP_ERROR("ERROR in preparing uplink tunnel, tune string length\n");
                    exit (-1);
                }
                ret = spgw_system(cmd, SPGW_ABORT_ON_ERROR, __FILE__, __LINE__);

                ret = snprintf(cmd,
                               256,
                               "iptables -t raw -A PREROUTING -i %s -s %u.%u.%u.%u -d %u.%u.%u.%u -p udp --dport 2152  -j GTPURH --action remove --own-ip %u.%u.%u.%u --own-tun %u --peer-ip %u.%u.%u.%u --peer-tun %u ",
                               sgw_app.sgw_interface_name_for_S1u_S12_S4_up,
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[0],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[1],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[2],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[3],
                               sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x000000FF,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x0000FF00) >> 8,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x00FF0000) >> 16,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0xFF000000) >> 24,
                               sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x000000FF,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x0000FF00) >> 8,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0x00FF0000) >> 16,
                               (sgw_app.sgw_ip_address_for_S1u_S12_S4_up & 0xFF000000) >> 24,
                               eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up,
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[0],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[1],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[2],
                               eps_bearer_entry_p->enb_ip_address_for_S1u.address.ipv4_address[3],
                               eps_bearer_entry_p->enb_teid_for_S1u);
                if ((ret < 0) || (ret > 256)) {
                    SPGW_APP_ERROR("ERROR in preparing uplink tunnel, tune string length\n");
                    exit (-1);
                }
                ret = spgw_system(cmd, SPGW_ABORT_ON_ERROR, __FILE__, __LINE__);
                if (ret < 0) {
                    SPGW_APP_ERROR("ERROR in setting up uplink TUNNEL\n");
                } else {
                    iptable_uplink_remove_gtpu = TRUE;
                }
            }
#endif
        }
        return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    } else {
        SPGW_APP_DEBUG("Rx SGI_UPDATE_ENDPOINT_RESPONSE: CONTEXT_NOT_FOUND (S11 context)\n");

        modify_response_p->teid                                           = resp_pP->context_teid; // TO BE CHECKED IF IT IS THIS TEID
        modify_response_p->bearer_present                                 = MODIFY_BEARER_RESPONSE_REM;
        modify_response_p->bearer_choice.bearer_for_removal.eps_bearer_id = resp_pP->eps_bearer_id;
        modify_response_p->bearer_choice.bearer_for_removal.cause         = CONTEXT_NOT_FOUND;
        modify_response_p->cause                                          = CONTEXT_NOT_FOUND;
        modify_response_p->trxn                                           = 0;

        return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
}



int
sgw_lite_handle_modify_bearer_request(
        const SgwModifyBearerRequest * const modify_bearer_pP)
{
    SgwModifyBearerResponse                           *modify_response_p          = NULL;
    Gtpv1uUpdateTunnelReq                             *gtpv1u_update_tunnel_req_p = NULL;
    s_plus_p_gw_eps_bearer_context_information_t      *new_bearer_ctxt_info_p     = NULL;
    MessageDef                                        *message_p                  = NULL;
    sgw_eps_bearer_entry_t                            *eps_bearer_entry_p         = NULL;
    hashtable_rc_t                                     hash_rc;
    task_id_t                                          to_task;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, teid %u, EPS bearer id %u\n",
                   modify_bearer_pP->teid,
                   modify_bearer_pP->bearer_context_to_modify.eps_bearer_id);

    sgw_lite_display_s11teid2mme_mappings();
    sgw_lite_display_s11_bearer_context_information_mapping();

    hash_rc = hashtable_get(
        sgw_app.s11_bearer_context_information_hashtable,
        modify_bearer_pP->teid,
        (void**)&new_bearer_ctxt_info_p);

    if (hash_rc == HASH_TABLE_OK) {

        new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.default_bearer = modify_bearer_pP->bearer_context_to_modify.eps_bearer_id;
        new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.trxn = modify_bearer_pP->trxn;

        hash_rc = hashtable_is_key_exists (
            new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
            modify_bearer_pP->bearer_context_to_modify.eps_bearer_id);

        if (hash_rc == HASH_TABLE_KEY_NOT_EXISTS) {
            message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
            if (message_p == NULL) {
                return -1;
            }
            modify_response_p = &message_p->ittiMsg.sgwModifyBearerResponse;
            memset(modify_response_p, 0, sizeof(SgwModifyBearerResponse));
            modify_response_p->bearer_present                                 = MODIFY_BEARER_RESPONSE_REM;
            modify_response_p->bearer_choice.bearer_for_removal.eps_bearer_id = modify_bearer_pP->bearer_context_to_modify.eps_bearer_id;
            modify_response_p->bearer_choice.bearer_for_removal.cause         = CONTEXT_NOT_FOUND;
            modify_response_p->cause                                          = CONTEXT_NOT_FOUND;
            modify_response_p->trxn                                           = modify_bearer_pP->trxn;
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, eps_bearer_id %u CONTEXT_NOT_FOUND\n",
                modify_bearer_pP->bearer_context_to_modify.eps_bearer_id);
            return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
        } else if (hash_rc == HASH_TABLE_OK) {
            // TO DO
            hash_rc = hashtable_get (
                new_bearer_ctxt_info_p->sgw_eps_bearer_context_information.pdn_connection.sgw_eps_bearers,
                modify_bearer_pP->bearer_context_to_modify.eps_bearer_id,
                (void**)&eps_bearer_entry_p);

            FTEID_T_2_IP_ADDRESS_T( (&modify_bearer_pP->bearer_context_to_modify.s1_eNB_fteid) , (&eps_bearer_entry_p->enb_ip_address_for_S1u) );
            eps_bearer_entry_p->enb_teid_for_S1u = modify_bearer_pP->bearer_context_to_modify.s1_eNB_fteid.teid;

#if defined (ENABLE_USE_GTPU_IN_KERNEL)
            {
                SGIUpdateEndpointResp sgi_update_end_point_resp;
                sgi_update_end_point_resp.context_teid  = modify_bearer_pP->teid;
                sgi_update_end_point_resp.sgw_S1u_teid  = eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up;
                sgi_update_end_point_resp.enb_S1u_teid  = eps_bearer_entry_p->enb_teid_for_S1u;
                sgi_update_end_point_resp.eps_bearer_id = eps_bearer_entry_p->eps_bearer_id;
                sgi_update_end_point_resp.status        = 0x00;
                sgw_lite_handle_sgi_endpoint_updated(&sgi_update_end_point_resp);
            }
#else
            // UPDATE GTPV1U mapping tables with eNB references (teid, addresses)
            message_p = itti_alloc_new_message(TASK_SPGW_APP, GTPV1U_UPDATE_TUNNEL_REQ);
            if (message_p == NULL) {
                return -1;
            }
            gtpv1u_update_tunnel_req_p = &message_p->ittiMsg.gtpv1uUpdateTunnelReq;
            memset(gtpv1u_update_tunnel_req_p, 0, sizeof(Gtpv1uUpdateTunnelReq));
            gtpv1u_update_tunnel_req_p->context_teid           = modify_bearer_pP->teid;
            gtpv1u_update_tunnel_req_p->sgw_S1u_teid           = eps_bearer_entry_p->s_gw_teid_for_S1u_S12_S4_up;     ///< SGW S1U local Tunnel Endpoint Identifier
            gtpv1u_update_tunnel_req_p->enb_S1u_teid           = eps_bearer_entry_p->enb_teid_for_S1u;                ///< eNB S1U Tunnel Endpoint Identifier
            gtpv1u_update_tunnel_req_p->enb_ip_address_for_S1u = eps_bearer_entry_p->enb_ip_address_for_S1u;
            gtpv1u_update_tunnel_req_p->eps_bearer_id          = eps_bearer_entry_p->eps_bearer_id;
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->context_teid           = %u\n",modify_bearer_pP->teid);
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->sgw_S1u_teid           = %u\n",gtpv1u_update_tunnel_req_p->sgw_S1u_teid);
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->enb_S1u_teid           = %u\n",gtpv1u_update_tunnel_req_p->enb_S1u_teid);
            //SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->enb_ip_address_for_S1u = %u\n",modify_bearer_pP->enb_ip_address_for_S1u);
            SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, gtpv1u_update_tunnel_req_p->eps_bearer_id          = %u\n",gtpv1u_update_tunnel_req_p->eps_bearer_id);

            return itti_send_msg_to_task(TASK_GTPV1_U, INSTANCE_DEFAULT, message_p);
#endif
            }
    } else {
        message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_MODIFY_BEARER_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        modify_response_p = &message_p->ittiMsg.sgwModifyBearerResponse;

        modify_response_p->bearer_present                                 = MODIFY_BEARER_RESPONSE_REM;
        modify_response_p->bearer_choice.bearer_for_removal.eps_bearer_id = modify_bearer_pP->bearer_context_to_modify.eps_bearer_id;
        modify_response_p->bearer_choice.bearer_for_removal.cause         = CONTEXT_NOT_FOUND;
        modify_response_p->cause                                          = CONTEXT_NOT_FOUND;
        modify_response_p->trxn                                           = modify_bearer_pP->trxn;

        SPGW_APP_DEBUG("Rx MODIFY_BEARER_REQUEST, teid %u CONTEXT_NOT_FOUND\n",
            modify_bearer_pP->teid);
        return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }
    return -1;
}



int
sgw_lite_handle_delete_session_request(
        const SgwDeleteSessionRequest * const delete_session_req_pP)
{
    task_id_t    to_task;
    hashtable_rc_t hash_rc;

    SgwDeleteSessionResponse                     *delete_session_resp_p = NULL;
    MessageDef                                   *message_p             = NULL;
    s_plus_p_gw_eps_bearer_context_information_t *ctx_p                 = NULL;

#if defined(ENABLE_STANDALONE_EPC)
    to_task = TASK_MME_APP;
#else
    to_task = TASK_S11;
#endif

    message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_DELETE_SESSION_RESPONSE);
    if (message_p == NULL) {
        return -1;
    }
    delete_session_resp_p = &message_p->ittiMsg.sgwDeleteSessionResponse;

    SPGW_APP_WARN("Delete session handler needs to be completed...\n");

    if (delete_session_req_pP->indication_flags & OI_FLAG) {
        SPGW_APP_DEBUG("OI flag is set for this message indicating the request"
                       "should be forwarded to P-GW entity\n");
    }

    hash_rc = hashtable_get(sgw_app.s11_bearer_context_information_hashtable,
                          delete_session_req_pP->teid,
                          (void**)&ctx_p);

    if (hash_rc == HASH_TABLE_OK) {
        if ((delete_session_req_pP->sender_fteid_for_cp.ipv4 != 0) &&
            (delete_session_req_pP->sender_fteid_for_cp.ipv6 != 0)) {
            /* Sender F-TEID IE present */
            if (delete_session_resp_p->teid != ctx_p->sgw_eps_bearer_context_information.mme_teid_for_S11) {
                delete_session_resp_p->teid  = ctx_p->sgw_eps_bearer_context_information.mme_teid_for_S11;
                delete_session_resp_p->cause = INVALID_PEER;
                SPGW_APP_DEBUG("Mismatch in MME Teid for CP\n");
            } else {
                delete_session_resp_p->teid = delete_session_req_pP->sender_fteid_for_cp.teid;
            }
        } else {
            delete_session_resp_p->cause = REQUEST_ACCEPTED;
            delete_session_resp_p->teid  = ctx_p->sgw_eps_bearer_context_information.mme_teid_for_S11;
        }
        delete_session_resp_p->trxn    = delete_session_req_pP->trxn;
        delete_session_resp_p->peer_ip = delete_session_req_pP->peer_ip;

        return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    } else {
        /* Context not found... set the cause to CONTEXT_NOT_FOUND
         * 3GPP TS 29.274 #7.2.10.1
         */
        message_p = itti_alloc_new_message(TASK_SPGW_APP, SGW_DELETE_SESSION_RESPONSE);
        if (message_p == NULL) {
            return -1;
        }
        delete_session_resp_p = &message_p->ittiMsg.sgwDeleteSessionResponse;

        if ((delete_session_req_pP->sender_fteid_for_cp.ipv4 == 0) &&
            (delete_session_req_pP->sender_fteid_for_cp.ipv6 == 0)) {
            delete_session_resp_p->teid = 0;
        } else {
            delete_session_resp_p->teid = delete_session_req_pP->sender_fteid_for_cp.teid;
        }
        delete_session_resp_p->cause   = CONTEXT_NOT_FOUND;
        delete_session_resp_p->trxn    = delete_session_req_pP->trxn;
        delete_session_resp_p->peer_ip = delete_session_req_pP->peer_ip;

        return itti_send_msg_to_task(to_task, INSTANCE_DEFAULT, message_p);
    }

    return -1;
}
