#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <arpa/inet.h>

#include "udp_primitives_client.h"

#include "UTIL/LOG/log.h"

#define IPV4_ADDR    "%u.%u.%u.%u"
#define IPV4_ADDR_FORMAT(aDDRESS)               \
    (uint8_t)((aDDRESS)  & 0x000000ff),         \
    (uint8_t)(((aDDRESS) & 0x0000ff00) >> 8 ),  \
    (uint8_t)(((aDDRESS) & 0x00ff0000) >> 16),  \
    (uint8_t)(((aDDRESS) & 0xff000000) >> 24)

static void *udp_recv_thread(void *arg_p);

int udp_create_connection(char *ip_addr, uint16_t port,
                          udp_data_t *udp_data_p,
                          udp_recv_callback recv_callback,
                          void *arg_p)
{

    udp_data_t *udp_desc;
    int sd;
    struct sockaddr_in sin;

    LOG_I(UDP, "Initializing UDP for local address %s with port %d\n",
          ip_addr, port);

    if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) <= 0) {
        LOG_E(UDP, "Failed to create new socket: (%s:%d)\n",
              strerror(errno), errno);
        return -1;
    }

    memset(&sin, 0, sizeof(struct sockaddr_in));

    sin.sin_family      = AF_INET;
    sin.sin_port        = htons(port);
    if (ip_addr == NULL) {
        sin.sin_addr.s_addr = inet_addr(INADDR_ANY);
    } else {
        sin.sin_addr.s_addr = inet_addr(ip_addr);
    }

    if (bind(sd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) < 0) {
        LOG_E(UDP, "Failed to bind socket: (%s:%d)\n",
              strerror(errno), errno);
        close(sd);
        return -1;
    }

    /* Create a new descriptor for this connection */
    udp_desc = malloc(sizeof(udp_data_t));
    memset(udp_desc, 0, sizeof(udp_data_t));

    udp_desc->sd            = sd;
    udp_desc->recv_callback = recv_callback;
    udp_desc->arg_p         = arg_p;

    memcpy(udp_data_p, udp_desc, sizeof(udp_data_t));

    if (pthread_create(&udp_desc->recv_thread, NULL, udp_recv_thread,
                       (void *)udp_desc) < 0) {
        LOG_E(UDP, "Failed to create new thread: (%s:%d)\n",
              strerror(errno), errno);
        close(sd);
        free(udp_desc);
        return -1;
    }

    LOG_I(UDP, "Initializing UDP for local address %s with port %d: DONE\n",
          ip_addr, port);

    return sd;
}

int udp_send_to(int sd, uint16_t port, uint32_t address, const uint8_t *buffer,
                uint32_t length)
{
    struct sockaddr_in to;
    socklen_t          to_length;

    if (sd <= 0 || ((buffer == NULL) && (length > 0))) {
        LOG_E(UDP, "udp_send_to: bad param\n");
        return -1;
    }

    memset(&to, 0, sizeof(struct sockaddr_in));
    to_length = sizeof(to);

    to.sin_family      = AF_INET;
    to.sin_port        = htons(port);
    to.sin_addr.s_addr = address;

    if (sendto(sd, (void *)buffer, (size_t)length, 0, (struct sockaddr *)&to,
               to_length) < 0) {
        LOG_E(UDP,
              "[SD %d] Failed to send data to "IPV4_ADDR" on port %d, buffer size %u\n",
              sd, IPV4_ADDR_FORMAT(address), port, length);
        return -1;
    }
    LOG_I(UDP, "[SD %d] Successfully sent to "IPV4_ADDR
          " on port %d, buffer size %u, buffer address %x\n",
          sd, IPV4_ADDR_FORMAT(address), port, length, buffer);
    return 0;
}

static void *udp_recv_thread(void *arg_p)
{
    udp_data_t *udp_desc;
    udp_desc = (udp_data_t *)arg_p;
    uint8_t buffer[4096];

    LOG_D(UDP, "Starting receiver thread\n");

    while(1) {
        int     n;

        struct sockaddr_in from;
        socklen_t          from_len;

        n = recvfrom(udp_desc->sd, buffer, sizeof(buffer), 0,
                     (struct sockaddr *)&from, &from_len);

        if (n < 0) {
            LOG_E(UDP, "Recvfrom failed: (%s:%d)\n",
                  strerror(errno), errno);
        } else if (n == 0) {
            LOG_I(UDP, "Peer %s on port %d has performed a shutdown\n",
                  inet_ntoa(from.sin_addr), ntohs(from.sin_port));
            break;
        } else {
            /* Normal read, notify the upper layer of incoming data */
            LOG_D(UDP, "Received UDP message of length %u from %s:%u\n",
                  n, inet_ntoa(from.sin_addr), ntohs(from.sin_port));
            if (udp_desc->recv_callback) {
                udp_desc->recv_callback(from.sin_port, from.sin_addr.s_addr, buffer, n,
                                        udp_desc->arg_p);
            } else {
                LOG_W(UDP, "No recv callback associated to this socket (%d), exiting\n",
                      udp_desc->sd);
                break;
            }
        }
    }
    LOG_I(UDP, "Receiver thread exiting\n");
    free(udp_desc);
    return NULL;
}
