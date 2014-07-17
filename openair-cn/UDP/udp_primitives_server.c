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
/*! \file udp_primitives_server.c
* \brief
* \author Sebastien ROUX, Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "intertask_interface.h"
#include "udp_primitives_server.h"

#include "assertions.h"
#include "conversions.h"

#define UDP_DEBUG(x, args...) do { fprintf(stdout, "[UDP] [D]"x, ##args); } while(0)
#define UDP_ERROR(x, args...) do { fprintf(stderr, "[UDP] [E]"x, ##args); } while(0)

void *udp_receiver_thread(void *args_p);

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

    UDP_DEBUG("Looking for task %d\n", task_id);

    STAILQ_FOREACH(udp_sock_p, &udp_socket_list, entries) {
        if (udp_sock_p->task_id == task_id) {
            UDP_DEBUG("Found matching task desc\n");
            break;
        }
    }
    return udp_sock_p;
}

static
int udp_create_socket(int port, char *address, task_id_t task_id)
{
    struct sockaddr_in addr;
    int                sd;

    struct udp_socket_desc_s *thread_arg = NULL;

    UDP_DEBUG("Creating new listen socket on address "IPV4_ADDR" and port %u\n",
              IPV4_ADDR_FORMAT(inet_addr(address)), port);

    /* Create UDP socket */
    if ((sd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        /* Socket creation has failed... */
        UDP_ERROR("Socket creation failed (%s)\n", strerror(errno));
        return sd;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);
    if (bind(sd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        /* Bind failed */
        UDP_ERROR("Socket bind failed (%s) for address "IPV4_ADDR" and port %u\n",
                  strerror(errno), IPV4_ADDR_FORMAT(inet_addr(address)), port);
        close(sd);
        return -1;
    }

    thread_arg = calloc(1, sizeof(struct udp_socket_desc_s));

    DevAssert(thread_arg != NULL);

    thread_arg->sd            = sd;
    thread_arg->local_address = address;
    thread_arg->local_port    = port;
    thread_arg->task_id       = task_id;

    if (pthread_create(&thread_arg->listener_thread, NULL,
        &udp_receiver_thread, (void *)thread_arg) < 0) {
        UDP_ERROR("Pthred_create failed (%s)\n", strerror(errno));
        return -1;
    }
    return sd;
}

void *udp_receiver_thread(void *arg_p)
{
    uint8_t buffer[2048];

    struct udp_socket_desc_s *udp_sock_p = (struct udp_socket_desc_s *)arg_p;

    UDP_DEBUG("Inserting new descriptor for task %d, sd %d\n",
              udp_sock_p->task_id, udp_sock_p->sd);
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
            UDP_ERROR("Recvfrom failed %s\n", strerror(errno));
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

            UDP_DEBUG("Msg of length %d received from %s:%u\n",
                      n, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            if (itti_send_msg_to_task(udp_sock_p->task_id, INSTANCE_DEFAULT, message_p) < 0) {
                UDP_DEBUG("Failed to send message %d to task %d\n",
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

static void *udp_intertask_interface(void *args_p)
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
                udp_create_socket(
                    udp_init_p->port,
                    udp_init_p->address,
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
                    UDP_ERROR("Failed to retrieve the udp socket descriptor "
                    "associated with task %d\n", ITTI_MSG_ORIGIN_ID(received_message_p));
                    pthread_mutex_unlock(&udp_socket_list_mutex);
                    if (udp_data_req_p->buffer) {
                        free(udp_data_req_p->buffer);
                    }
                    goto on_error;
                }
                udp_sd = udp_sock_p->sd;
                pthread_mutex_unlock(&udp_socket_list_mutex);

                UDP_DEBUG("[%d] Sending message of size %u to "IPV4_ADDR" and port %u\n",
                          udp_sd, udp_data_req_p->buffer_length,
                          IPV4_ADDR_FORMAT(udp_data_req_p->peer_address),
                          udp_data_req_p->peer_port);

                bytes_written = sendto(udp_sd, udp_data_req_p->buffer,
                                       udp_data_req_p->buffer_length, 0,
                                       (struct sockaddr *)&peer_addr,
                                       sizeof(struct sockaddr_in));

                if (bytes_written != udp_data_req_p->buffer_length) {
                    UDP_ERROR("There was an error while writing to socket "
                    "(%d:%s)\n", errno, strerror(errno));
                }
            } break;

            case TERMINATE_MESSAGE: {
                itti_exit_task();
            } break;

            case MESSAGE_TEST: {
            } break;

            default: {
                UDP_DEBUG("Unkwnon message ID %d:%s\n",
                          ITTI_MSG_ID(received_message_p), ITTI_MSG_NAME(received_message_p));
            } break;
        }
on_error:
        itti_free(ITTI_MSG_ORIGIN_ID(received_message_p), received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int udp_init(const mme_config_t *mme_config_p)
{
    UDP_DEBUG("Initializing UDP task interface\n");

    STAILQ_INIT(&udp_socket_list);

    if (itti_create_task(TASK_UDP, &udp_intertask_interface,
                                        NULL) < 0) {
        UDP_ERROR("udp pthread_create (%s)\n", strerror(errno));
        return -1;
    }
    UDP_DEBUG("Initializing UDP task interface: DONE\n");
    return 0;
}
