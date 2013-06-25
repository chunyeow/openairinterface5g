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

#include <pthread.h>

#include "intertask_interface.h"
#include "sgw_lite_defs.h"
#include "sgw_lite_handlers.h"
#include "mme_config.h"

static pthread_t sgw_lite_task_thread;

static void *sgw_lite_intertask_interface(void *args_p) {
    while(1) {
        MessageDef *received_message_p;
        receive_msg(TASK_SGW_LITE, &received_message_p);
        switch(received_message_p->messageId) {
            case SGW_CREATE_SESSION_REQUEST:
            {
                /* We received a create session request from MME (with GTP abstraction here)
                 * procedures might be:
                 *      E-UTRAN Initial Attach
                 *      UE requests PDN connectivity
                 */
                sgw_lite_handle_create_session_request(&received_message_p->msg.sgwCreateSessionRequest);
            } break;
            case SGW_MODIFY_BEARER_REQUEST:
            {
                sgw_lite_handle_modify_bearer_request(&received_message_p->msg.sgwModifyBearerRequest);
            } break;
            case SGW_S1U_ENDPOINT_CREATED:
            {
                SGW_DEBUG("Received teid for S1-U: %d and status: %s\n",
                          received_message_p->msg.sgwS1UEndpointCreated.teid,
                          received_message_p->msg.sgwS1UEndpointCreated.status == 0 ? "Success" : "Failure");
                sgw_lite_handle_s1u_endpoint_created(&received_message_p->msg.sgwS1UEndpointCreated);
            } break;
            default:
            {
                SGW_DEBUG("Unknown message ID %d\n", received_message_p->messageId);
            } break;
        }
        free(received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int sgw_lite_init(const mme_config_t *mme_config) {
    SGW_DEBUG("Initializing SGW LITE task interface\n");
    if (pthread_create(&sgw_lite_task_thread, NULL, &sgw_lite_intertask_interface, NULL) < 0) {
        perror("pthread_create");
        return -1;
    }
    SGW_DEBUG("Initializing SGW LITE task interface: DONE\n");
    return 0;
}
