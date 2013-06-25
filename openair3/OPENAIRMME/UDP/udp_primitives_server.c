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
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

#include "intertask_interface.h"
#include "udp_primitives_server.h"

#define IPV4_ADDR    "%u.%u.%u.%u"
#define IPV4_ADDR_FORMAT(aDDRESS)               \
    (uint8_t)((aDDRESS)  & 0x000000ff),         \
    (uint8_t)(((aDDRESS) & 0x0000ff00) >> 8 ),  \
    (uint8_t)(((aDDRESS) & 0x00ff0000) >> 16),  \
    (uint8_t)(((aDDRESS) & 0xff000000) >> 24)

#ifndef UDP_DEBUG
# define UDP_DEBUG(x, args...) do { fprintf(stdout, "[UDP] [D]"x, ##args); } while(0)
# define UDP_ERROR(x, args...) do { fprintf(stderr, "[UDP] [E]"x, ##args); } while(0)
#endif

/* Reader thread: reads messages from network */
static pthread_t udp_recv_thread;
/* UDP task thread: read messages from other tasks */
static pthread_t udp_task_thread;

void *udp_receiver_thread(void *args_p);
static int udp_create_socket(int port, char *address);
static int udp_fd = -1;

static int udp_create_socket(int port, char *address) {
    struct sockaddr_in addr;
    int                fd;

    UDP_DEBUG("Creating new listen socket on address "IPV4_ADDR" and port %d\n",
              IPV4_ADDR_FORMAT(inet_addr(address)), port);

    /* Create UDP socket */
    if ((fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        /* Socket creation has failed... */
        UDP_ERROR("Socket creation failed (%s)\n", strerror(errno));
        return fd;
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0) {
        /* Bind failed */
        UDP_ERROR("Socket bind failed (%s)\n", strerror(errno));
        close(fd);
        return -1;
    }

    udp_fd = fd;

    if (pthread_create(&udp_recv_thread, NULL, &udp_receiver_thread, &fd) < 0) {
        UDP_ERROR("Pthred_create failed (%s)\n", strerror(errno));
        return -1;
    }
    return fd;
}

void *udp_receiver_thread(void *args_p) {
    int *fd_p = (int*)args_p;
    int  fd = *fd_p;

    while (1) {
        uint8_t            buffer[1024];
        int                n;
        socklen_t          from_len;
        struct sockaddr_in addr;

        from_len = (socklen_t)sizeof(struct sockaddr_in);

        if ((n = recvfrom(fd, buffer, sizeof(buffer), 0,
            (struct sockaddr *)&addr, &from_len)) < 0) {
            UDP_ERROR("Recvfrom failed %s\n", strerror(errno));
            break;
        } else {
            MessageDef    *message_p;
            Gtpv1uDataReq *gtpv1u_data_req_p;
            uint8_t *forwarded_buffer = NULL;

            forwarded_buffer = calloc(n, sizeof(uint8_t));

            memcpy(forwarded_buffer, buffer, n);

            message_p = (MessageDef *)malloc(sizeof(MessageDef));
            message_p->messageId = GTPV1U_DATA_REQ;
            message_p->originTaskId = TASK_UDP;
            message_p->destinationTaskId = TASK_GTPV1_U;
            gtpv1u_data_req_p = &message_p->msg.gtpv1uDataReq;
            gtpv1u_data_req_p->buffer        = forwarded_buffer;
            gtpv1u_data_req_p->buffer_length = n;
            gtpv1u_data_req_p->port          = htons(addr.sin_port);
            gtpv1u_data_req_p->peer_addr     = addr.sin_addr.s_addr;
            UDP_DEBUG("Msg of length %d received from %s:%u\n",
                      n, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            if (send_msg_to_task(TASK_GTPV1_U, message_p) < 0) {
                UDP_DEBUG("Failed to send message %d to task %d\n",
                          GTPV1U_DATA_REQ, TASK_GTPV1_U);
                break;
            }
        }
    }
    close(fd);
    fd = -1;
    free(args_p);
    return NULL;
}

static void *udp_intertask_interface(void *args_p) {
    while(1) {
        MessageDef *receivedMessage;
        receive_msg(TASK_UDP, &receivedMessage);
        switch(receivedMessage->messageId) {
            case UDP_INIT:
            {
                UdpInit *init_p;
                init_p = &receivedMessage->msg.udpInit;
                udp_create_socket(init_p->port, init_p->address);
            } break;
            case UDP_DATA_REQ:
            {
                UdpDataReq *udp_data_req_p;
                struct sockaddr_in peer_addr;
                udp_data_req_p = &receivedMessage->msg.udpDataReq;

                memset(&peer_addr, 0, sizeof(struct sockaddr_in));

                peer_addr.sin_family       = AF_INET;
                peer_addr.sin_port         = htons(udp_data_req_p->peer_port);
                peer_addr.sin_addr.s_addr  = (udp_data_req_p->peer_address);

                UDP_DEBUG("Sending message of size %u to "IPV4_ADDR" and port %u\n",
                          udp_data_req_p->buffer_length,
                          IPV4_ADDR_FORMAT(udp_data_req_p->peer_address),
                          udp_data_req_p->peer_port);

                sendto(udp_fd, udp_data_req_p->buffer,
                       udp_data_req_p->buffer_length, 0,
                       (struct sockaddr *)&peer_addr, sizeof(struct sockaddr_in));
            } break;
            default:
            {
                UDP_DEBUG("Unknown message ID %d\n", receivedMessage->messageId);
            } break;
        }
        free(receivedMessage);
        receivedMessage = NULL;
    }
    return NULL;
}

int udp_init(const mme_config_t *mme_config) {
    UDP_DEBUG("Initializing UDP task interface\n");
    if (pthread_create(&udp_task_thread, NULL, &udp_intertask_interface, NULL) < 0) {
        UDP_ERROR("udp pthread_create (%s)\n", strerror(errno));
        return -1;
    }
    UDP_DEBUG("Initializing UDP task interface: DONE\n");
    return 0;
}
