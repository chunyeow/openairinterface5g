/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2014 Eurecom

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
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "queue.h"
#include "intertask_interface.h"
#include "assertions.h"
#include "udp_eNB_task.h"

#include "UTIL/LOG/log.h"

#define IPV4_ADDR    "%u.%u.%u.%u"
#define IPV4_ADDR_FORMAT(aDDRESS)               \
    (uint8_t)((aDDRESS)  & 0x000000ff),         \
    (uint8_t)(((aDDRESS) & 0x0000ff00) >> 8 ),  \
    (uint8_t)(((aDDRESS) & 0x00ff0000) >> 16),  \
    (uint8_t)(((aDDRESS) & 0xff000000) >> 24)

static void *udp_receiver_thread(void *arg_p);

struct udp_socket_desc_s {
    int       sd;              /* Socket descriptor to use */

    pthread_t listener_thread; /* Thread affected to recv */

    char     *local_address;   /* Local ipv4 address to use */
    uint16_t  local_port;      /* Local port to use */

    task_id_t task_id;         /* Task who has requested the new endpoint */

    STAILQ_ENTRY(udp_socket_desc_s) entries;
};

static STAILQ_HEAD(udp_socket_list_s, udp_socket_desc_s) udp_socket_list;
static pthread_mutex_t udp_socket_list_mutex = PTHREAD_MUTEX_INITIALIZER;


/* @brief Retrieve the descriptor associated with the task_id
 */
static
struct udp_socket_desc_s *udp_get_socket_desc(task_id_t task_id)
{
    struct udp_socket_desc_s *udp_sock_p = NULL;

    LOG_I(UDP_, "Looking for task %d\n", task_id);

    STAILQ_FOREACH(udp_sock_p, &udp_socket_list, entries) {
        if (udp_sock_p->task_id == task_id) {
            LOG_D(UDP_, "Found matching task desc\n");
            break;
        }
    }
    return udp_sock_p;
}

static
int udp_create_socket(int port, char *ip_addr, task_id_t task_id)
{

    struct udp_socket_desc_s  *thread_arg;
    int                       sd, rc;
    struct sockaddr_in        sin;

    LOG_I(UDP_, "Initializing UDP for local address %s with port %d\n", ip_addr, port);

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    AssertFatal(sd > 0, "UDP: Failed to create new socket: (%s:%d)\n", strerror(errno), errno);

    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(port);
    if (ip_addr == NULL) {
        sin.sin_addr.s_addr = inet_addr(INADDR_ANY);
    } else {
        sin.sin_addr.s_addr = inet_addr(ip_addr);
    }

    if ((rc = bind(sd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in))) < 0) {
        close(sd);
        AssertFatal(rc >= 0, "UDP: Failed to bind socket: (%s:%d)\n\n", strerror(errno), errno);
    }

    /* Create a new descriptor for this connection */
    thread_arg = calloc(1, sizeof(struct udp_socket_desc_s));

    DevAssert(thread_arg != NULL);

    thread_arg->sd            = sd;
    thread_arg->local_address = ip_addr;
    thread_arg->local_port    = port;
    thread_arg->task_id       = task_id;

    if (pthread_create(&thread_arg->listener_thread, NULL,
        &udp_receiver_thread, (void *)thread_arg) < 0) {
        LOG_E(UDP_, "Pthred_create failed (%s)\n", strerror(errno));
        return -1;
    }
    LOG_I(UDP_, "Initializing UDP for local address %s with port %d: DONE\n", ip_addr, port);
    return sd;
}

static void *udp_receiver_thread(void *arg_p)
{
    struct udp_socket_desc_s *udp_sock_p;
    uint8_t                   buffer[2048];

    udp_sock_p = (struct udp_socket_desc_s *)arg_p;
    LOG_D(UDP_, "Inserting new descriptor for task %d, sd %d\n", udp_sock_p->task_id, udp_sock_p->sd);
    pthread_mutex_lock(&udp_socket_list_mutex);
    STAILQ_INSERT_TAIL(&udp_socket_list, udp_sock_p, entries);
    pthread_mutex_unlock(&udp_socket_list_mutex);

    while (1) {
        int                n;
        socklen_t          from_len;
        struct sockaddr_in addr;

        from_len = (socklen_t)sizeof(struct sockaddr_in);

        if ((n = recvfrom(udp_sock_p->sd, buffer, sizeof(buffer), 0,
                          (struct sockaddr *)&addr, &from_len)) < 0) {
            LOG_E(UDP_, "Recvfrom failed %s\n", strerror(errno));
            break;
        } else {
            MessageDef     *message_p = NULL;
            udp_data_ind_t *udp_data_ind_p;
            uint8_t *forwarded_buffer = NULL;

            forwarded_buffer = calloc(n, sizeof(uint8_t));
            memcpy(forwarded_buffer, buffer, n);
            message_p = itti_alloc_new_message(TASK_UDP, UDP_DATA_IND);
            DevAssert(message_p != NULL);
            udp_data_ind_p = &message_p->ittiMsg.udp_data_ind;
            udp_data_ind_p->buffer        = forwarded_buffer;
            udp_data_ind_p->buffer_length = n;
            udp_data_ind_p->peer_port     = htons(addr.sin_port);
            udp_data_ind_p->peer_address  = addr.sin_addr.s_addr;

            LOG_D(UDP_, "Msg of length %d received from %s:%u\n",
                      n, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            if (itti_send_msg_to_task(udp_sock_p->task_id, INSTANCE_DEFAULT, message_p) < 0) {
                LOG_D(UDP_, "Failed to send message %d to task %d\n",
                          UDP_DATA_IND, udp_sock_p->task_id);
                break;
            }
        }
    }
    close(udp_sock_p->sd);
    udp_sock_p->sd = -1;

    pthread_mutex_lock(&udp_socket_list_mutex);
    STAILQ_REMOVE(&udp_socket_list, udp_sock_p, udp_socket_desc_s, entries);
    pthread_mutex_unlock(&udp_socket_list_mutex);

    return NULL;
}


void *udp_eNB_task(void *args_p)
{
    itti_mark_task_ready(TASK_UDP);
    while(1) {
        MessageDef *received_message_p = NULL;
        itti_receive_msg(TASK_UDP, &received_message_p);
        DevAssert(received_message_p != NULL);

        switch (ITTI_MSG_ID(received_message_p))
        {
            case UDP_INIT: {
                udp_init_t *udp_init_p;
                udp_init_p = &received_message_p->ittiMsg.udp_init;
                udp_create_socket(udp_init_p->port, udp_init_p->address,
                                  ITTI_MSG_ORIGIN_ID(received_message_p));
            } break;
            case UDP_DATA_REQ: {
                int     udp_sd = -1;
                ssize_t bytes_written;

                struct udp_socket_desc_s *udp_sock_p = NULL;
                udp_data_req_t           *udp_data_req_p;
                struct sockaddr_in        peer_addr;

                udp_data_req_p = &received_message_p->ittiMsg.udp_data_req;

                memset(&peer_addr, 0, sizeof(struct sockaddr_in));

                peer_addr.sin_family       = AF_INET;
                peer_addr.sin_port         = htons(udp_data_req_p->peer_port);
                peer_addr.sin_addr.s_addr  = udp_data_req_p->peer_address;

                pthread_mutex_lock(&udp_socket_list_mutex);
                udp_sock_p = udp_get_socket_desc(ITTI_MSG_ORIGIN_ID(received_message_p));

                if (udp_sock_p == NULL) {
                    LOG_E(UDP_, "Failed to retrieve the udp socket descriptor "
                    "associated with task %d\n", ITTI_MSG_ORIGIN_ID(received_message_p));
                    pthread_mutex_unlock(&udp_socket_list_mutex);
                    if (udp_data_req_p->buffer) {
                        free(udp_data_req_p->buffer);
                    }
                    goto on_error;
                }
                udp_sd = udp_sock_p->sd;
                pthread_mutex_unlock(&udp_socket_list_mutex);

                LOG_D(UDP_, "[%d] Sending message of size %u to "IPV4_ADDR" and port %u\n",
                          udp_sd, udp_data_req_p->buffer_length,
                          IPV4_ADDR_FORMAT(udp_data_req_p->peer_address),
                          udp_data_req_p->peer_port);

                bytes_written = sendto(udp_sd, udp_data_req_p->buffer,
                                       udp_data_req_p->buffer_length, 0,
                                       (struct sockaddr *)&peer_addr,
                                       sizeof(struct sockaddr_in));

                if (bytes_written != udp_data_req_p->buffer_length) {
                    LOG_E(UDP_, "There was an error while writing to socket "
                    "(%d:%s)\n", errno, strerror(errno));
                }
            } break;
            case TERMINATE_MESSAGE: {
                itti_exit_task();
            } break;
            case MESSAGE_TEST: {
            } break;
            default: {
                LOG_D(UDP_, "Unkwnon message ID %d:%s\n",
                          ITTI_MSG_ID(received_message_p), ITTI_MSG_NAME(received_message_p));
            } break;
        }
on_error:
        free(received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int udp_enb_init(const Enb_properties_t *enb_config_p)
{
    LOG_D(UDP_, "Initializing UDP task interface\n");
    STAILQ_INIT(&udp_socket_list);
    LOG_D(UDP_, "Initializing UDP task interface: DONE\n");
    return 0;
}
