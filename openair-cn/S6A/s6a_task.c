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

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "intertask_interface.h"
#include "s6a_defs.h"
#include "s6a_messages.h"

#include "assertions.h"

#if defined(DISABLE_USE_HSS)
# error "Disabling HSS support is currently not supported   \
    Reconfigure with --enable-hss option to disable this error"
#endif

static int gnutls_debug = 0;
struct session_handler *ts_sess_hdl;

s6a_fd_cnf_t s6a_fd_cnf;

void *s6a_thread(void *args);
static void fd_gnutls_debug(int level, const char *str);

static void fd_gnutls_debug(int level, const char *str)
{
    S6A_DEBUG("[GTLS] %s", str);
}

void *s6a_thread(void *args)
{
    itti_mark_task_ready(TASK_S6A);

    while(1) {
        MessageDef *received_message_p = NULL;
        /* Trying to fetch a message from the message queue.
         * If the queue is empty, this function will block till a
         * message is sent to the task.
         */
        itti_receive_msg(TASK_S6A, &received_message_p);
        DevAssert(received_message_p != NULL);

        switch (ITTI_MSG_ID(received_message_p))
        {
            case S6A_UPDATE_LOCATION_REQ: {
                s6a_generate_update_location(&received_message_p->ittiMsg.s6a_update_location_req);
            } break;
            case S6A_AUTH_INFO_REQ: {
                s6a_generate_authentication_info_req(&received_message_p->ittiMsg.s6a_auth_info_req);
            } break;
            default: {
                S6A_DEBUG("Unkwnon message ID %d:%s\n",
                          ITTI_MSG_ID(received_message_p), ITTI_MSG_NAME(received_message_p));
            } break;
        }
        free(received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int s6a_init(const mme_config_t *mme_config_p)
{
    int ret;

    S6A_DEBUG("Initializing S6a interface\n");

    memset(&s6a_fd_cnf, 0, sizeof(s6a_fd_cnf_t));

    if (strcmp(fd_core_version(), FREE_DIAMETER_MINIMUM_VERSION) != 0) {
        S6A_ERROR("Freediameter version %s fount, expecting %s\n", fd_core_version(),
                  FREE_DIAMETER_MINIMUM_VERSION);
        return -1;
    } else {
        S6A_DEBUG("Freediameter version %s\n", fd_core_version());
    }

    /* Initializing freeDiameter core */
    ret = fd_core_initialize();
    if (ret != 0) {
        S6A_ERROR("An error occurred during freeDiameter core library initialization\n");
        return ret;
    }

    /* Set gnutls debug level ? */
    if (gnutls_debug) {
        gnutls_global_set_log_function((gnutls_log_func)fd_gnutls_debug);
        gnutls_global_set_log_level (gnutls_debug);
        S6A_DEBUG("Enabled GNUTLS debug at level %d", gnutls_debug);
    }

    /* Starting freeDiameter core */
    ret = fd_core_start();
    if (ret != 0) {
        S6A_ERROR("An error occurred during freeDiameter core library start\n");
        return ret;
    }

    S6A_DEBUG("Default ext path: %s\n", DEFAULT_EXTENSIONS_PATH);

    ret = fd_core_parseconf(mme_config_p->s6a_config.conf_file);
    if (ret != 0) {
        S6A_ERROR("An error occurred during fd_core_parseconf.\n");
        return ret;
    }

    ret = fd_core_waitstartcomplete();
    if (ret != 0) {
        S6A_ERROR("An error occurred during fd_core_waitstartcomplete.\n");
        return ret;
    }

    ret = s6a_fd_init_dict_objs();
    if (ret != 0) {
        S6A_ERROR("An error occurred during s6a_fd_init_dict_objs.\n");
        return ret;
    }

    /* Trying to connect to peers */
    CHECK_FCT(s6a_fd_new_peer());

    if (itti_create_task(TASK_S6A, &s6a_thread, NULL) < 0) {
        S6A_ERROR("s6a create task\n");
        return -1;
    }
    S6A_DEBUG("Initializing S6a interface: DONE\n");

    return 0;
}
