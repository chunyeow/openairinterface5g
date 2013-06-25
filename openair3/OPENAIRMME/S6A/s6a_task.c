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
#include <signal.h>

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <freeDiameter/freeDiameter-host.h>
#include <freeDiameter/libfdcore.h>

#include "intertask_interface.h"
#include "s6a_defs.h"

#ifndef S6A_DEBUG
# define S6A_DEBUG(x, args...) do { fprintf(stdout, "[S6A] [D]"x, ##args); } while(0)
#endif
#ifndef S6A_ERROR
# define S6A_ERROR(x, args...) do { fprintf(stdout, "[S6A] [E]"x, ##args); } while(0)
#endif

static pthread_t s6a_task_thread;
static int gnutls_debug = 0;

void* s6a_thread(void *args);
static void fd_gnutls_debug(int level, const char * str);

static void fd_gnutls_debug(int level, const char * str) {
    S6A_DEBUG("[gnutls:%d] %s", level, str);
}

void* s6a_thread(void *args) {
    while(1) {
        MessageDef *received_message = NULL;
        /* Trying to fetch a message from the message queue.
         * If the queue is empty, this function will block till a
         * message is sent to the task.
         */
        receive_msg(TASK_S6A, &received_message);
        assert(received_message != NULL);
        switch(received_message->messageId) {
            default:
            {
                S6A_DEBUG("Unkwnon message ID %d\n", received_message->messageId);
            } break;
        }
        free(received_message);
        received_message = NULL;
    }
    return NULL;
}

int s6a_init(const mme_config_t *mme_config) {
    int ret;
    struct peer_info info;

    S6A_DEBUG("Initializing S6a interface\n");

    memset(&info, 0, sizeof(struct peer_info));

    info.pi_diamid = "localhost";
    info.pi_diamidlen = strlen(info.pi_diamid);
    info.config.pic_flags.pro3 = PI_P3_IP;
    info.config.pic_flags.pro4 = PI_P4_SCTP;
    info.config.pic_flags.sec  = PI_SEC_NONE;

    if (strcmp(fd_core_version(), "1.1.5") != 0)
        S6A_ERROR("Freediameter version %s fount, expecting %s\n", fd_core_version(), "1.1.5");
    else
        S6A_DEBUG("Freediameter version %s\n", fd_core_version());

    /* Initializing freeDiameter core */
    ret = fd_core_initialize();
    if (ret != 0) {
        fprintf(stderr, "An error occurred during freeDiameter core library initialization.\n");
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
        fprintf(stderr, "An error occurred during freeDiameter core library start.\n");
        return ret;
    }

    ret = fd_peer_add(&info, "conf", NULL, NULL);
    if (ret != 0) {
        fprintf(stderr, "An error occurred during fd_peer_add.\n");
        return ret;
    }

    if (pthread_create(&s6a_task_thread, NULL, &s6a_thread, NULL) < 0) {
        perror("s6a phtread_create");
        return -1;
    }
    S6A_DEBUG("Initializing S6a interface: DONE\n");

    return 0;
}
