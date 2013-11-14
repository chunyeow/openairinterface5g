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
#include <stdint.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netinet/sctp.h>

#include <arpa/inet.h>

#include "assertions.h"
#include "queue.h"

#include "intertask_interface.h"

#include "sctp_common.h"
#include "sctp_eNB_itti_messaging.h"

struct sctp_cnx_list_elm_s {
    STAILQ_ENTRY(sctp_cnx_list_elm_s) entries;

    /* Socket descriptor of connection */
    int sd;

    /* IN/OUT streams */
    uint16_t in_streams;
    uint16_t out_streams;

    /* Configured PPID */
    uint16_t ppid;

    /* Association id */
    int32_t assoc_id;

    /* Nb of messages received on interface */
    uint32_t nb_messages;

    /* Task id of the task who asked for this connection */
    task_id_t task_id;

    /* Instance */
    instance_t instance;

    /* Upper layer identifier */
    uint16_t cnx_id;
};

static STAILQ_HEAD(sctp_cnx_list_head, sctp_cnx_list_elm_s) sctp_cnx_list;
static uint16_t sctp_nb_cnx = 0;

struct sctp_cnx_list_elm_s *sctp_get_cnx(int32_t assoc_id, int sd)
{
    struct sctp_cnx_list_elm_s *elm;

    STAILQ_FOREACH(elm, &sctp_cnx_list, entries)
    {
        if (assoc_id != -1) {
            if (elm->assoc_id == assoc_id) {
                return elm;
            }
        } else {
            if (elm->sd == sd) {
                return elm;
            }
        }
    }

    return NULL;
}

void sctp_handle_new_association_req(
    const instance_t instance,
    const task_id_t requestor,
    const sctp_new_association_req_t * const sctp_new_association_req_p)
{
    int     sd;
    int32_t assoc_id;

    struct sctp_initmsg         init;
    struct sctp_event_subscribe events;

    struct sctp_cnx_list_elm_s *sctp_cnx = NULL;

    /* Prepare a new SCTP association as requested by upper layer and try to connect
     * to remote host.
     */
    DevAssert(sctp_new_association_req_p != NULL);

    /* Create new socket with IPv6 affinity */
    if ((sd = socket(AF_INET6, SOCK_STREAM, IPPROTO_SCTP)) < 0) {
        SCTP_ERROR("Socket creation failed: %s\n", strerror(errno));
        return;
    }

    /* Add the socket to list of fd monitored by ITTI */
    itti_subscribe_event_fd(TASK_SCTP, sd);

    /* Request a number of in/out streams */
    init.sinit_num_ostreams  = SCTP_OUT_STREAMS;
    init.sinit_max_instreams = SCTP_IN_STREAMS;
    init.sinit_max_attempts  = SCTP_MAX_ATTEMPTS;

    SCTP_DEBUG("Requesting (%d %d) (in out) streams\n", init.sinit_num_ostreams,
               init.sinit_max_instreams);

    if (setsockopt(sd, IPPROTO_SCTP, SCTP_INITMSG,
                   &init, (socklen_t)sizeof(struct sctp_initmsg)) < 0) {
        SCTP_ERROR("Setsockopt IPPROTO_SCTP_INITMSG failed: %s\n",
                   strerror(errno));
        close(sd);
        return;
    }

    /* Subscribe to all events */
    memset((void *)&events, 1, sizeof(struct sctp_event_subscribe));
    if (setsockopt(sd, IPPROTO_SCTP, SCTP_EVENTS, &events,
                   sizeof(struct sctp_event_subscribe)) < 0) {
        SCTP_ERROR("Setsockopt IPPROTO_SCTP_EVENTS failed: %s\n",
                   strerror(errno));
        close(sd);
        return;
    }

    /* Mark the socket as non-blocking */
    if (fcntl(sd, F_SETFL, O_NONBLOCK) < 0) {
        SCTP_ERROR("fcntl F_SETFL O_NONBLOCK failed: %s\n",
                   strerror(errno));
        close(sd);
        return;
    }

    /* SOCK_STREAM socket type requires an explicit connect to the remote host
     * address and port.
     * Only use IPv4 for the first connection attempt
     */
    {
        uint8_t address_index = 0;
        uint8_t used_address  = sctp_new_association_req_p->remote_address.ipv6 +
                                sctp_new_association_req_p->remote_address.ipv4;
        struct sockaddr_in addr[used_address];

        memset(addr, 0, used_address * sizeof(struct sockaddr_in));

        if (sctp_new_association_req_p->remote_address.ipv6 == 1) {
            if (inet_pton(AF_INET6, sctp_new_association_req_p->remote_address.ipv6_address,
                &addr[address_index].sin_addr.s_addr) != 1)
            {
                SCTP_ERROR("Failed to convert ipv6 address %*s to network type\n",
                           strlen(sctp_new_association_req_p->remote_address.ipv6_address),
                           sctp_new_association_req_p->remote_address.ipv6_address);
                close(sd);
                return;
            }

            SCTP_DEBUG("Converted ipv6 address %*s to network type\n",
                       strlen(sctp_new_association_req_p->remote_address.ipv6_address),
                       sctp_new_association_req_p->remote_address.ipv6_address);

            addr[address_index].sin_family = AF_INET6;
            addr[address_index].sin_port   = htons(sctp_new_association_req_p->port);
            address_index++;
        }
        if (sctp_new_association_req_p->remote_address.ipv4 == 1) {
            if (inet_pton(AF_INET, sctp_new_association_req_p->remote_address.ipv4_address,
                &addr[address_index].sin_addr.s_addr) != 1)
            {
                SCTP_ERROR("Failed to convert ipv4 address %*s to network type\n",
                           strlen(sctp_new_association_req_p->remote_address.ipv4_address),
                           sctp_new_association_req_p->remote_address.ipv4_address);
                close(sd);
                return;
            }

            SCTP_DEBUG("Converted ipv4 address %*s to network type\n",
                       strlen(sctp_new_association_req_p->remote_address.ipv4_address),
                       sctp_new_association_req_p->remote_address.ipv4_address);

            addr[address_index].sin_family = AF_INET;
            addr[address_index].sin_port   = htons(sctp_new_association_req_p->port);
            address_index++;
        }

        SCTP_DEBUG("Connecting...\n");

        /* Connect to remote host and port */
        if (sctp_connectx(sd, (struct sockaddr *)addr, used_address, &assoc_id) < 0)
        {
            /* sctp_connectx on non-blocking socket return EINPROGRESS */
            if (errno != EINPROGRESS) {
                SCTP_ERROR("Connect failed: %s\n", strerror(errno));
                sctp_itti_send_association_resp(
                    requestor, instance, -1, sctp_new_association_req_p->ulp_cnx_id,
                    SCTP_STATE_UNREACHABLE, 0, 0);
                close(sd);
                return;
            }
        }
    }

    sctp_cnx = calloc(1, sizeof(*sctp_cnx));

    sctp_cnx->sd       = sd;
    sctp_cnx->task_id  = requestor;
    sctp_cnx->cnx_id   = sctp_new_association_req_p->ulp_cnx_id;
    sctp_cnx->ppid     = sctp_new_association_req_p->ppid;
    sctp_cnx->instance = instance;

    /* Insert new element at end of list */
    STAILQ_INSERT_TAIL(&sctp_cnx_list, sctp_cnx, entries);
    sctp_nb_cnx++;

    SCTP_DEBUG("Inserted new descriptor for sd %d in list, nb elements %u, assoc_id %d\n",
                sd, sctp_nb_cnx, assoc_id);
}

void sctp_send_data(instance_t instance, task_id_t task_id, sctp_data_req_t *sctp_data_req_p)
{
    struct sctp_cnx_list_elm_s *sctp_cnx = NULL;

    DevAssert(sctp_data_req_p != NULL);

    sctp_cnx = sctp_get_cnx(sctp_data_req_p->assoc_id, 0);

    if (sctp_cnx == NULL) {
        SCTP_ERROR("Failed to find SCTP description for assoc_id %d\n",
                   sctp_data_req_p->assoc_id);
        /* TODO: notify upper layer */
        return;
    }

    if (sctp_data_req_p->stream >= sctp_cnx->out_streams) {
        SCTP_ERROR("Requested stream (%u) >= nb out streams\n",
                   sctp_data_req_p->stream, sctp_cnx->out_streams);
        return;
    }

    /* Send message on specified stream of the sd association
     * NOTE: PPID should be defined in network order
     */
    if (sctp_sendmsg(sctp_cnx->sd, sctp_data_req_p->buffer,
                     sctp_data_req_p->buffer_length, NULL, 0,
                     htonl(sctp_cnx->ppid), 0, sctp_data_req_p->stream, 0, 0) < 0) {
        SCTP_ERROR("Sctp_sendmsg failed: %s\n", strerror(errno));
        /* TODO: notify upper lkayer */
        return;
    }

    SCTP_DEBUG("Successfully sent %u bytes on stream %d for assoc_id %u\n",
               sctp_data_req_p->buffer_length, sctp_data_req_p->stream,
               sctp_cnx->assoc_id);
}

static
inline void sctp_eNB_read_from_socket(struct sctp_cnx_list_elm_s *sctp_cnx)
{
    int flags = 0, n;
    socklen_t from_len;
    struct sctp_sndrcvinfo sinfo;

    struct sockaddr_in addr;
    uint8_t buffer[SCTP_RECV_BUFFER_SIZE];

    DevAssert(sctp_cnx != NULL);

    memset((void *)&addr, 0, sizeof(struct sockaddr_in));
    from_len = (socklen_t)sizeof(struct sockaddr_in);
    memset((void *)&sinfo, 0, sizeof(struct sctp_sndrcvinfo));

    n = sctp_recvmsg(sctp_cnx->sd, (void *)buffer, SCTP_RECV_BUFFER_SIZE,
                     (struct sockaddr *)&addr, &from_len,
                     &sinfo, &flags);
    if (n < 0) {
        SCTP_DEBUG("An error occured during read\n");
        SCTP_ERROR("sctp_recvmsg: %s:%d\n", strerror(errno), errno);
        return;
    }
    if (flags & MSG_NOTIFICATION) {
        union sctp_notification *snp;
        snp = (union sctp_notification *)buffer;

        SCTP_DEBUG("Received notification for sd %d, type %u\n",
                   sctp_cnx->sd, snp->sn_header.sn_type);

        /* Client deconnection */
        if (SCTP_SHUTDOWN_EVENT == snp->sn_header.sn_type) {
            DevMessage("Other peer has requested a com down -> not handled\n");
//             return sctp_handle_com_down(snp->sn_shutdown_event.sse_assoc_id);
            itti_unsubscribe_event_fd(TASK_SCTP, sctp_cnx->sd);
        }
        /* Association has changed. */
        else if (SCTP_ASSOC_CHANGE == snp->sn_header.sn_type) {
            struct sctp_assoc_change *sctp_assoc_changed;
            sctp_assoc_changed = &snp->sn_assoc_change;

            SCTP_DEBUG("Client association changed: %d\n", sctp_assoc_changed->sac_state);

            /* New physical association requested by a peer */
            switch (sctp_assoc_changed->sac_state) {
                case SCTP_COMM_UP: {
                    if (sctp_get_peeraddresses(sctp_cnx->sd, NULL, NULL) != 0)
                    {
                        /* TODO Failure -> notify upper layer */
                    } else {
                        sctp_get_sockinfo(sctp_cnx->sd, &sctp_cnx->in_streams,
                                          &sctp_cnx->out_streams, &sctp_cnx->assoc_id);
                    }

                    SCTP_DEBUG("Comm up notified for sd %d, assigned assoc_id %d\n",
                               sctp_cnx->sd, sctp_cnx->assoc_id);

                    sctp_itti_send_association_resp(
                        sctp_cnx->task_id, sctp_cnx->instance, sctp_cnx->assoc_id,
                        sctp_cnx->cnx_id, SCTP_STATE_ESTABLISHED,
                        sctp_cnx->out_streams, sctp_cnx->in_streams);

                } break;
                default:
                    break;
            }
        }
    } else {
        sctp_cnx->nb_messages++;

        if (sinfo.sinfo_ppid != sctp_cnx->ppid) {
            /* Mismatch in Payload Protocol Identifier,
             * may be we received unsollicited traffic from stack other than S1AP.
             */
            SCTP_ERROR("Received data from peer with unsollicited PPID %d, expecting %d\n",
                       sinfo.sinfo_ppid, sctp_cnx->ppid);
        }

        SCTP_DEBUG("[%d][%d] Msg of length %d received from port %u, on stream %d, PPID %d\n",
                   sinfo.sinfo_assoc_id, sctp_cnx->sd, n, ntohs(addr.sin_port),
                   sinfo.sinfo_stream, sinfo.sinfo_ppid);

        sctp_itti_send_new_message_ind(sctp_cnx->task_id,
                                       sinfo.sinfo_assoc_id,
                                       buffer, n, sinfo.sinfo_stream);
    }
}

void sctp_eNB_flush_sockets(struct epoll_event *events, int nb_events)
{
    int i;
    struct sctp_cnx_list_elm_s *sctp_cnx = NULL;

    if (events == NULL) {
        return;
    }

    for (i = 0; i < nb_events; i++) {
        sctp_cnx = sctp_get_cnx(-1, events[i].data.fd);
        if (sctp_cnx == NULL) {
            continue;
        }
        SCTP_DEBUG("Found data for descriptor %d\n", events[i].data.fd);
        sctp_eNB_read_from_socket(sctp_cnx);
    }
}

void *sctp_eNB_task(void *arg)
{
    int                 nb_events;
    struct epoll_event *events;
    MessageDef         *received_msg = NULL;

    SCTP_DEBUG("Starting SCTP layer\n");

    STAILQ_INIT(&sctp_cnx_list);

    itti_mark_task_ready(TASK_SCTP);

    while (1) {
        itti_receive_msg(TASK_SCTP, &received_msg);

        /* Check if there is a packet to handle */
        if (received_msg != NULL) {
            switch (received_msg->header.messageId) {
                case TERMINATE_MESSAGE:
                    itti_exit_task();
                    break;
                case SCTP_NEW_ASSOCIATION_REQ: {
                    sctp_handle_new_association_req(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                                    received_msg->header.originTaskId,
                                                    &received_msg->msg.sctp_new_association_req);
                } break;
                case SCTP_DATA_REQ: {
                    sctp_send_data(ITTI_MESSAGE_GET_INSTANCE(received_msg),
                                   received_msg->header.originTaskId,
                                   &received_msg->msg.sctp_data_req);
                } break;
                default:
                    SCTP_ERROR("Received unhandled message with id %d\n",
                            received_msg->header.messageId);
                    break;
            }
        }
        free(received_msg);
        received_msg = NULL;

        nb_events = itti_get_events(TASK_SCTP, &events);
        /* Now handle notifications for other sockets */
        sctp_eNB_flush_sockets(events, nb_events);
    }
    return NULL;
}
