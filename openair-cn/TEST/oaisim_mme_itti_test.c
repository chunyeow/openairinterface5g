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
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <sched.h>

#include "mme_config.h"
#include "gtpv1u_sgw_defs.h"

#include "intertask_interface_init.h"

#include "sctp_primitives_server.h"
#include "udp_primitives_server.h"
#include "s1ap_mme.h"
#include "log.h"
#include "timer.h"
#include "sgw_lite_defs.h"
#include "ipv4_defs.h"

int main(int argc, char *argv[])
{
    int i = 0;
    int prio = 0;
    mme_config_t mme_config;
    MessageDef *message_p;
    struct sched_param param = {
        .sched_priority = 10,
    };

    config_parse_opt_line(argc, argv, &mme_config);
    fprintf(stdout, "Starting %s ITTI test\n", PACKAGE_STRING);

    if (setpriority(PRIO_PROCESS, 0, prio) < 0) {
        fprintf(stderr, "Cannot assign requested prio: %d\n"
        "%d:%s\n", prio, errno, strerror(errno));
        return -1;
    }

    if (sched_setscheduler(0, SCHED_RR, &param) < 0) {
        fprintf(stderr, "Cannot assign requested scheduler policy\n"
        "%d:%s\n", errno, strerror(errno));
        return -1;
    }

    /* Calling each layer init function */
    log_init(&mme_config);
    intertask_interface_init(THREAD_MAX, MESSAGES_ID_MAX, threads_name, messages_info, messages_definition_xml);
    sctp_init(&mme_config);
    udp_init(&mme_config);
    s1ap_mme_init(&mme_config);
    gtpv1u_init(&mme_config);
    ipv4_init(&mme_config);

    sgw_lite_init(&mme_config);

    message_p = alloc_new_message(TASK_S1AP, MESSAGE_TEST);

    while(i < (1 << 15)) {
        if (send_broadcast_message(message_p) < 0) {
            fprintf(stderr, "Failed to send broadcast message %d\n", i);
        }
        i++;
    }
    fprintf(stderr, "Successfully sent %lu messages", get_current_message_number());

    return 0;
}
