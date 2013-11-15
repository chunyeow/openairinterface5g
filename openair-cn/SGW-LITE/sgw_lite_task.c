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
#include <unistd.h>
#include <string.h>
#include <assert.h>

#include "intertask_interface.h"
#include "mme_config.h"
#include "sgw_lite_defs.h"
#include "sgw_lite_handlers.h"
#include "sgw_lite.h"
#include "hashtable.h"

sgw_app_t    sgw_app;

static void *sgw_lite_intertask_interface(void *args_p)
{
    itti_mark_task_ready(TASK_SPGW_APP);

    while(1) {
        MessageDef *received_message_p;
        itti_receive_msg(TASK_SPGW_APP, &received_message_p);

        switch (ITTI_MSG_ID(received_message_p))
        {
            case SGW_CREATE_SESSION_REQUEST: {
                /* We received a create session request from MME (with GTP abstraction here)
                 * procedures might be:
                 *      E-UTRAN Initial Attach
                 *      UE requests PDN connectivity
                 */
                sgw_lite_handle_create_session_request(&received_message_p->msg.sgwCreateSessionRequest);
            } break;

            case SGW_MODIFY_BEARER_REQUEST: {
                sgw_lite_handle_modify_bearer_request(&received_message_p->msg.sgwModifyBearerRequest);
            } break;

            case SGW_DELETE_SESSION_REQUEST: {
                sgw_lite_handle_delete_session_request(&received_message_p->msg.sgwDeleteSessionRequest);
            } break;

            case GTPV1U_CREATE_TUNNEL_RESP: {
                SPGW_APP_DEBUG("Received teid for S1-U: %u and status: %s\n",
                          received_message_p->msg.gtpv1uCreateTunnelResp.S1u_teid ,
                          received_message_p->msg.gtpv1uCreateTunnelResp.status == 0 ? "Success" :
                          "Failure");
                sgw_lite_handle_gtpv1uCreateTunnelResp(&received_message_p->msg.gtpv1uCreateTunnelResp);
            } break;

            case GTPV1U_UPDATE_TUNNEL_RESP: {
                sgw_lite_handle_gtpv1uUpdateTunnelResp(&received_message_p->msg.gtpv1uUpdateTunnelResp);
            } break;

            case SGI_CREATE_ENDPOINT_RESPONSE: {
            	sgw_lite_handle_sgi_endpoint_created(&received_message_p->msg.sgiCreateEndpointResp);
            } break;

            case SGI_UPDATE_ENDPOINT_RESPONSE: {
            	sgw_lite_handle_sgi_endpoint_updated(&received_message_p->msg.sgiUpdateEndpointResp);
            } break;

            default: {
                SPGW_APP_DEBUG("Unkwnon message ID %d:%s\n",
                               ITTI_MSG_ID(received_message_p), ITTI_MSG_NAME(received_message_p));
            } break;
        }
        free(received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int sgw_lite_init(const mme_config_t *mme_config_p)
{
    SPGW_APP_DEBUG("Initializing SPGW-APP  task interface\n");
    if (itti_create_task(TASK_SPGW_APP,
                                        &sgw_lite_intertask_interface, NULL) < 0) {
        perror("pthread_create");
    	SPGW_APP_DEBUG("Initializing SPGW-APP task interface: ERROR\n");
        return -1;
    }

    sgw_app.s11teid2mme_hashtable = hashtbl_create (8192, NULL, NULL);
    if (sgw_app.s11teid2mme_hashtable == NULL) {
        perror("hashtbl_create");
    	SPGW_APP_DEBUG("Initializing SPGW-APP task interface: ERROR\n");
        return -1;
    }

    sgw_app.s1uteid2enb_hashtable = hashtbl_create (8192, NULL, NULL);
    if (sgw_app.s1uteid2enb_hashtable == NULL) {
        perror("hashtbl_create");
    	SPGW_APP_DEBUG("Initializing SPGW-APP task interface: ERROR\n");
        return -1;
    }


    sgw_app.s11_bearer_context_information_hashtable = hashtbl_create (8192, NULL, sgw_lite_cm_free_s_plus_p_gw_eps_bearer_context_information);
    if (sgw_app.s11_bearer_context_information_hashtable == NULL) {
        perror("hashtbl_create");
    	SPGW_APP_DEBUG("Initializing SPGW-APP task interface: ERROR\n");
        return -1;
    }

    sgw_app.sgw_interface_name_for_S1u_S12_S4_up = mme_config_p->ipv4.sgw_interface_name_for_S1u_S12_S4_up;
    sgw_app.sgw_ip_address_for_S1u_S12_S4_up     = mme_config_p->ipv4.sgw_ip_address_for_S1u_S12_S4_up;
    sgw_app.sgw_interface_name_for_S11_S4        = mme_config_p->ipv4.sgw_interface_name_for_S11;
    sgw_app.sgw_ip_address_for_S11_S4            = mme_config_p->ipv4.sgw_ip_address_for_S11;
    //sgw_app.sgw_ip_address_for_S5_S8_cp          = mme_config_p->ipv4.sgw_ip_address_for_S5_S8_cp;
    sgw_app.sgw_ip_address_for_S5_S8_up          = mme_config_p->ipv4.sgw_ip_address_for_S5_S8_up;

    SPGW_APP_DEBUG("Initializing SPGW-APP task interface: DONE\n");
    return 0;
}
