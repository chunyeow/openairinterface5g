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
#include "timer.h"

#include "mme_app_extern.h"
#include "mme_app_ue_context.h"
#include "mme_app_defs.h"
#include "mme_app_statistics.h"

#include "assertions.h"

mme_app_desc_t mme_app_desc;

void *mme_app_thread(void *args);

void *mme_app_thread(void *args)
{
    itti_mark_task_ready(TASK_MME_APP);

    while(1) {
        MessageDef *received_message_p = NULL;
        /* Trying to fetch a message from the message queue.
         * If the queue is empty, this function will block till a
         * message is sent to the task.
         */
        itti_receive_msg(TASK_MME_APP, &received_message_p);
        DevAssert(received_message_p != NULL);

        switch (ITTI_MSG_ID(received_message_p))
        {
            case S6A_AUTH_INFO_ANS: {
                /* We received the authentication vectors from HSS, trigger a ULR
                 * for now. Normaly should trigger an authentication procedure with UE.
                 */
                mme_app_handle_authentication_info_answer(&received_message_p->ittiMsg.s6a_auth_info_ans);
            } break;

            case S6A_UPDATE_LOCATION_ANS: {
                /* We received the update location answer message from HSS -> Handle it */
                mme_app_create_bearer(&received_message_p->ittiMsg.s6a_update_location_ans);
            } break;

            case SGW_CREATE_SESSION_RESPONSE: {
                mme_app_handle_create_sess_resp(&received_message_p->ittiMsg.sgwCreateSessionResponse);
            } break;

#if defined(DISABLE_USE_NAS)
            case NAS_ATTACH_REQ: {
                mme_app_handle_attach_req(&received_message_p->ittiMsg.nas_attach_req);
            } break;

            case NAS_AUTHENTICATION_RESP: {
                mme_app_handle_nas_auth_resp(&received_message_p->ittiMsg.nas_auth_resp);
            } break;
#else
            case NAS_AUTHENTICATION_PARAM_REQ: {
                mme_app_handle_nas_auth_param_req(&received_message_p->ittiMsg.nas_auth_param_req);
            } break;
#endif
            case TIMER_HAS_EXPIRED: {
                /* Check if it is the statistic timer */
                if (received_message_p->ittiMsg.timer_has_expired.timer_id ==
                    mme_app_desc.statistic_timer_id) {
                    mme_app_statistics_display();
                }
            } break;
            case TERMINATE_MESSAGE: {
                /* Termination message received TODO -> release any data allocated */
                itti_exit_task();
            } break;
            default: {
                MME_APP_DEBUG("Unkwnon message ID %d:%s\n",
                              ITTI_MSG_ID(received_message_p), ITTI_MSG_NAME(received_message_p));
            } break;
        }
        itti_free(ITTI_MSG_ORIGIN_ID(received_message_p), received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int mme_app_init(const mme_config_t *mme_config_p)
{
    MME_APP_DEBUG("Initializing MME applicative layer\n");

    memset(&mme_app_desc, 0, sizeof(mme_app_desc));

    /* Create the thread associated with MME applicative layer */
    if (itti_create_task(TASK_MME_APP, &mme_app_thread, NULL) < 0) {
        MME_APP_ERROR("MME APP create task failed\n");
        return -1;
    }

    mme_app_desc.statistic_timer_period = mme_config_p->mme_statistic_timer;

    /* Request for periodic timer */
    if (timer_setup(mme_config_p->mme_statistic_timer, 0, TASK_MME_APP, INSTANCE_DEFAULT,
        TIMER_PERIODIC, NULL, &mme_app_desc.statistic_timer_id) < 0)
    {
        MME_APP_ERROR("Failed to request new timer for statistics with %ds "
        "of periocidity\n", mme_config_p->mme_statistic_timer);
        mme_app_desc.statistic_timer_id = 0;
    }

    MME_APP_DEBUG("Initializing MME applicative layer: DONE\n");
    return 0;
}
