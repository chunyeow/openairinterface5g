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
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file pdcp_netlink.c
* \brief pdcp communication with linux IP interface,
* have a look at http://man7.org/linux/man-pages/man7/netlink.7.html for netlink.
* Read operation from netlink should be achieved in an asynchronous way to avoid
* subframe overload, netlink congestion...
* \author Sebastien ROUX
* \date 2013
* \version 0.1
* @ingroup pdcp
*/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <error.h>
#include <unistd.h>

#include <linux/netlink.h>

#include "assertions.h"
#include "queue.h"
#include "liblfds611.h"

#include "UTIL/LOG/log.h"
#include "UTIL/OCG/OCG.h"
#include "UTIL/OCG/OCG_extern.h"
#include "LAYER2/MAC/extern.h"

#include "pdcp.h"
#include "pdcp_primitives.h"

#define PDCP_QUEUE_NB_ELEMENTS 200

extern char nl_rx_buf[NL_MAX_PAYLOAD];
extern struct nlmsghdr *nas_nlh_rx;
extern struct iovec nas_iov_rx;
extern int nas_sock_fd;
extern struct msghdr nas_msg_rx;

static pthread_t pdcp_netlink_thread;

/* We use lock-free queues between the User-plane driver running in kernel-space
 * and the corresponding entity in User-space.
 * one queue for eNBs (index 0)/one queue for UEs (index 1)
 */
static struct lfds611_queue_state **pdcp_netlink_queue = NULL;
static uint32_t *pdcp_netlink_nb_element = NULL;

static void *pdcp_netlink_thread_fct(void *arg);

int pdcp_netlink_init(void) {

    int i, nb_modules;
    pthread_attr_t attr;
    struct sched_param sched_param;

#if defined(USER_MODE) && defined(OAI_EMU)
    nb_modules = NB_eNB_INST + NB_UE_INST;
#else
    nb_modules = 1;
#endif

    pdcp_netlink_queue = calloc(nb_modules, sizeof(struct lfds611_queue_state*));
    pdcp_netlink_nb_element = malloc(nb_modules * sizeof(uint32_t));

    LOG_I(PDCP, "Creating %d queues for Netlink -> PDCP communication\n",
          nb_modules);

    for (i = 0; i < nb_modules; i++) {
        pdcp_netlink_nb_element[i] = 0;
        if (lfds611_queue_new(&pdcp_netlink_queue[i], PDCP_QUEUE_NB_ELEMENTS) < 0) {
            LOG_E(PDCP, "Failed to create new FIFO for Netlink -> PDCP communcation instance %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    if (pthread_attr_init(&attr) != 0) {
        LOG_E(PDCP, "Failed to initialize pthread attribute for Netlink -> PDCP communication (%d:%s)\n",
              errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    sched_param.sched_priority = 10;

    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    pthread_attr_setschedparam(&attr, &sched_param);

    /* Create one thread that fetchs packets from the netlink.
     * When the netlink fifo is full, packets are silently dropped, this behaviour
     * should be avoided if we want a reliable link.
     */
    if (pthread_create(&pdcp_netlink_thread, &attr, pdcp_netlink_thread_fct, NULL) != 0) {
        LOG_E(PDCP, "Failed to create new thread for Netlink/PDCP communcation (%d:%s)\n",
              errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return 0;
}

int pdcp_netlink_dequeue_element(uint8_t eNB_flag, uint8_t UE_index, uint8_t eNB_index,
                                 struct pdcp_netlink_element_s **data)
{
    int ret = 0;
    module_id_t module_id;

#if defined(USER_MODE) && defined(OAI_EMU)
    module_id = (eNB_flag != 0) ? eNB_index : NB_eNB_INST + UE_index;
#else
    module_id = 0;
#endif

    ret = lfds611_queue_dequeue(pdcp_netlink_queue[module_id], (void **)data);

    if (ret != 0) {
        LOG_D(PDCP, "De-queueing packet for module %d\n", module_id);
    }
    return ret;
}

static
void *pdcp_netlink_thread_fct(void *arg) {
    int len;
    struct pdcp_netlink_element_s *new_data = NULL;
    uint8_t pdcp_read_state;

    pdcp_read_state = 0;
    memset(nl_rx_buf, 0, NL_MAX_PAYLOAD);

    while (1) {

        len = recvmsg(nas_sock_fd, &nas_msg_rx, 0);

        if (len == 0) {
            /* Other peer (kernel) has performed an orderly shutdown
             */
            LOG_E(PDCP, "Kernel module has closed the netlink\n");
            exit(EXIT_FAILURE);
        } else if (len < 0) {
            /* There was an error */
            LOG_E(PDCP, "An error occured while reading netlink (%d:%s)\n",
                  errno, strerror(errno));
            exit(EXIT_FAILURE);
        } else {
            /* Normal read.
             * NOTE: netlink messages can be assembled to form a multipart message
             */
            for (nas_nlh_rx = (struct nlmsghdr *) nl_rx_buf;
                 NLMSG_OK(nas_nlh_rx, len);
                 nas_nlh_rx = NLMSG_NEXT (nas_nlh_rx, len)) {

                /* There is no need to check for nlmsg_type because
                 * the header is not set in our drivers.
                 */
                if (pdcp_read_state == 0) {
                    new_data = malloc(sizeof(struct pdcp_netlink_element_s));

                    if (nas_nlh_rx->nlmsg_len == sizeof (pdcp_data_req_header_t) + sizeof(struct nlmsghdr)) {
                        pdcp_read_state = 1;
                        memcpy((void *)&new_data->pdcp_read_header, (void *)NLMSG_DATA(nas_nlh_rx), sizeof(pdcp_data_req_header_t));
                        LOG_I(PDCP, "[NETLINK] RX pdcp_data_req_header_t inst %u, "
                              "rb_id %u data_size %d\n",
                              new_data->pdcp_read_header.inst, new_data->pdcp_read_header.rb_id,
                              new_data->pdcp_read_header.data_size);
                    } else {
                        LOG_E(PDCP, "[NETLINK] WRONG size %d should be sizeof "
                              "(pdcp_data_req_header_t) + sizeof(struct nlmsghdr)\n",
                              nas_nlh_rx->nlmsg_len);
                    }
                } else {
                    pdcp_read_state = 0;

#ifdef OAI_EMU
                    if (new_data->pdcp_read_header.inst >= oai_emulation.info.nb_enb_local) {
                        new_data->pdcp_read_header.inst = new_data->pdcp_read_header.inst
                        - oai_emulation.info.nb_enb_local + NB_eNB_INST
                        + oai_emulation.info.first_ue_local;
                    } else {
                        new_data->pdcp_read_header.inst = new_data->pdcp_read_header.inst
                        + oai_emulation.info.first_enb_local;
                    }
#else
                    new_data->pdcp_read_header.inst = 0;
#endif
                    new_data->data = malloc(sizeof(uint8_t) * new_data->pdcp_read_header.data_size);
                    /* Copy the data */
                    memcpy(new_data->data, NLMSG_DATA(nas_nlh_rx), new_data->pdcp_read_header.data_size);

                    if (pdcp_netlink_nb_element[new_data->pdcp_read_header.inst]
                        > PDCP_QUEUE_NB_ELEMENTS) {
                        LOG_E(PDCP, "[Mod %02x] We reached maximum number of elements in pdcp queue (%d)\n",
                              new_data->pdcp_read_header.inst, pdcp_netlink_nb_element);
                    }

                    LOG_D(PDCP, "En-queueing packet for module %d\n", new_data->pdcp_read_header.inst);

                    /* Enqueue the element in the right queue */
//                     lfds611_queue_enqueue(pdcp_netlink_queue[new_data->pdcp_read_header.inst], new_data);
                    lfds611_queue_guaranteed_enqueue(pdcp_netlink_queue[new_data->pdcp_read_header.inst], new_data);
                }
            }
        }
    }
    return NULL;
}
