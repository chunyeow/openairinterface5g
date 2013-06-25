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
#include <string.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
//WARNING: sctp requires libsctp-dev and -lsctp as linker option
#include <netinet/sctp.h>

#include <arpa/inet.h>

#include <pthread.h>

#include "intertask_interface.h"
#include "sctp_primitives_server.h"
#include "mme_config.h"

#define IPV4_ADDR    "%u.%u.%u.%u"
#define IPV4_ADDR_FORMAT(aDDRESS)               \
    (uint8_t)((aDDRESS)  & 0x000000ff),         \
    (uint8_t)(((aDDRESS) & 0x0000ff00) >> 8 ),  \
    (uint8_t)(((aDDRESS) & 0x00ff0000) >> 16),  \
    (uint8_t)(((aDDRESS) & 0xff000000) >> 24)

#ifndef SCTP_DEBUG
# define SCTP_DEBUG(x, args...) do { fprintf(stdout, "[SCTP][D]"x, ##args); } while(0)
#endif
#ifndef SCTP_ERROR
# define SCTP_ERROR(x, args...) do { fprintf(stdout, "[SCTP][E]"x, ##args); } while(0)
#endif

#undef SCTP_DUMP_LIST

struct sctp_arg_s {
    int      fd;
    uint32_t ppid;
};

// Thread used to receive messages from upper layers
static pthread_t sctpThread;
// Thread used to handle sctp messages
static pthread_t assocThread;

// List of connected peers
static sctp_descriptor_t *available_connections_head = NULL;
static sctp_descriptor_t *available_connections_tail = NULL;
static uint32_t number_of_connections = 0;

// LOCAL FUNCTIONS prototypes
void *sctp_receiver_thread(void *args_p);
static int sctp_send_msg(int32_t sctp_assoc_id, uint16_t stream, const uint8_t *buffer, const uint32_t length);

// Association list related local functions prototypes
static sctp_descriptor_t* sctp_is_assoc_in_list(int32_t assoc_id);
static sctp_descriptor_t* sctp_add_new_peer(void);

static sctp_descriptor_t* sctp_add_new_peer(void) {
    sctp_descriptor_t *new_sctp_descriptor;

    new_sctp_descriptor = malloc(sizeof(sctp_descriptor_t));
    if (new_sctp_descriptor == NULL) {
        return NULL;
    }
    new_sctp_descriptor->next_assoc = NULL;
    new_sctp_descriptor->previous_assoc = NULL;

    if (available_connections_tail == NULL) {
        available_connections_head = new_sctp_descriptor;
        available_connections_tail = available_connections_head;
    } else {
        new_sctp_descriptor->previous_assoc = available_connections_tail;
        available_connections_tail->next_assoc = new_sctp_descriptor;
        available_connections_tail = new_sctp_descriptor;
    }
    number_of_connections++;
    return new_sctp_descriptor;
}

static sctp_descriptor_t* sctp_is_assoc_in_list(int32_t assoc_id) {
    sctp_descriptor_t *assoc_desc;
    if (assoc_id < 0) return NULL;
    for (assoc_desc = available_connections_head;
         assoc_desc;
         assoc_desc = assoc_desc->next_assoc) {
        if (assoc_desc->assoc_id == assoc_id) break;
    }
    return assoc_desc;
}

static int sctp_remove_assoc_from_list(int32_t assoc_id) {
    sctp_descriptor_t *assoc_desc;
    /* Association not in the list */
    if ((assoc_desc = sctp_is_assoc_in_list(assoc_id)) == NULL) return -1;

    if (assoc_desc->next_assoc == NULL) {
        if (assoc_desc->previous_assoc == NULL) {
            /* Head and tail */
            available_connections_head = available_connections_tail = NULL;
        } else {
            /* Not head but tail */
            available_connections_tail = assoc_desc->previous_assoc;
            assoc_desc->previous_assoc->next_assoc = NULL;
        }
    } else {
        if (assoc_desc->previous_assoc == NULL) {
            /* Head but not tail */
            available_connections_head = assoc_desc->next_assoc;
            assoc_desc->next_assoc->previous_assoc = NULL;
        } else {
            /* Not head and not tail */
            assoc_desc->previous_assoc->next_assoc = assoc_desc->next_assoc;
            assoc_desc->next_assoc->previous_assoc = assoc_desc->previous_assoc;
        }
    }
    free(assoc_desc);
    number_of_connections --;
    return 0;
}

static void sctp_dump_assoc(sctp_descriptor_t *sctp_desc) {
#if defined(SCTP_DUMP_LIST)
    if (sctp_desc == NULL) return;

    SCTP_DEBUG("fd           : %d\n", sctp_desc->fd);
    SCTP_DEBUG("input streams: %d\n", sctp_desc->instreams);
    SCTP_DEBUG("out streams  : %d\n", sctp_desc->outstreams);
    SCTP_DEBUG("assoc_id     : %d\n", sctp_desc->assoc_id);
#else
    sctp_desc = sctp_desc;
#endif
}

static void sctp_dump_list(void) {
#if defined(SCTP_DUMP_LIST)
    sctp_descriptor_t *sctp_desc;

    sctp_desc = available_connections_head;

    SCTP_DEBUG("SCTP list contains %d associations\n", number_of_connections);

    while (sctp_desc != NULL) {
        sctp_dump_assoc(sctp_desc);
        sctp_desc = sctp_desc->next_assoc;
    }
#else
    sctp_dump_assoc(NULL);
#endif
}

static int sctp_send_msg(int32_t sctp_assoc_id, uint16_t stream, const uint8_t *buffer, const uint32_t length)
{
    sctp_descriptor_t *assoc_desc;

    assert(buffer != NULL);

    if ((assoc_desc = sctp_is_assoc_in_list(sctp_assoc_id)) < 0) {
        SCTP_DEBUG("This assoc id has not been fount in list (%d)\n", sctp_assoc_id);
        return -1;
    }

    /* Send message_p on specified stream of the fd association */
    if (sctp_sendmsg(assoc_desc->fd,
                     (const void *)buffer,
                     length,
                     (struct sockaddr *)&assoc_desc->sin,
                     sizeof(assoc_desc->sin), assoc_desc->ppid, 0, stream, 0, 0) < 0)
    {
        perror("send");
        return -1;
    }

    assoc_desc->messages_sent++;

    SCTP_DEBUG("Successfully sent %d bytes on stream %d\n", length, stream);

    return 0;
}

static int sctp_create_new_connection(int port, char *address, uint32_t ppid) {
    struct sctp_event_subscribe event;
    struct sockaddr_in addr;
    struct sctp_initmsg init;

    struct sctp_arg_s *sctp_arg_p;

    int fd;

#if defined(USE_SOCK_STREAM)
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0) {
#else
    if ((fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0) {
#endif
        perror("socket");
        return -1;
    }

    SCTP_DEBUG("Creating new listen socket on address "IPV4_ADDR" and port %u\n",
               IPV4_ADDR_FORMAT(inet_addr(address)), port);

    memset((void *)&event, 1, sizeof(struct sctp_event_subscribe));
    if (setsockopt(fd, IPPROTO_SCTP, SCTP_EVENTS, &event, sizeof(struct sctp_event_subscribe)) < 0)
    {
        perror("setsockopt");
        return -1;
    }

    memset((void *)&init, 0, sizeof(struct sctp_initmsg));

    /* Request a number of streams */
    init.sinit_num_ostreams  = SCTP_MAX_INSTREAMS;
    init.sinit_max_instreams = SCTP_MAX_OUTSTREAMS;
    init.sinit_max_attempts  = SCTP_MAX_ATTEMPTS;

    if (setsockopt(fd, IPPROTO_SCTP, SCTP_INITMSG, &init, (socklen_t)sizeof(struct sctp_initmsg)) < 0)
    {
        perror("setsockopt");
        return -1;
    }

    memset((void *)&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    // Accept from any address
    addr.sin_addr.s_addr = inet_addr(address);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        perror("bind");
        exit(1);
    }
    if (listen(fd, 5) < 0)
    {
        perror("listen");
        exit(1);
    }

    if ((sctp_arg_p = malloc(sizeof(struct sctp_arg_s))) == NULL) {
        return -1;
    }
    sctp_arg_p->fd = fd;
    sctp_arg_p->ppid = ppid;

    if (pthread_create(&assocThread, NULL, &sctp_receiver_thread, (void*)sctp_arg_p) < 0)
    {
        perror("pthread_create");
        return -1;
    }
    return fd;
}

void *sctp_receiver_thread(void *args_p)
{
    int flags, n;
    int clientsock;
    socklen_t from_len;

    struct sctp_arg_s *sctp_arg_p;
    struct sctp_sndrcvinfo sinfo;
    struct sockaddr_in addr;
    uint8_t buffer[SCTP_BUFFER_SIZE];

    sctp_arg_p = (struct sctp_arg_s *)args_p;
    clientsock = sctp_arg_p->fd;

    while(1)
    {
        flags = 0;
        memset((void *)&addr, 0, sizeof(struct sockaddr_in));
        from_len = (socklen_t)sizeof(struct sockaddr_in);
        memset((void *)&sinfo, 0, sizeof(struct sctp_sndrcvinfo));
        n = sctp_recvmsg(clientsock, (void*)buffer, SCTP_BUFFER_SIZE,
                        (struct sockaddr *)&addr, &from_len,
                        &sinfo, &flags);
        if (n < 0)
        {
            SCTP_DEBUG("An error occured during read");
            perror("sctp_recvmsg");
            break;
        }
        if (flags & MSG_NOTIFICATION)
        {
            union sctp_notification *snp;
            snp = (union sctp_notification *)buffer;

            /* Client deconnection */
            if (SCTP_SHUTDOWN_EVENT == snp->sn_header.sn_type)
            {
                MessageDef *message_p;
                message_p = (MessageDef *)malloc(sizeof(MessageDef));

                SCTP_DEBUG("Client on association %d successfully removed from list\n",
                        snp->sn_shutdown_event.sse_assoc_id);

                message_p->messageId = S1AP_SCTP_ASSOCIATION_CLOSED;
                message_p->originTaskId = TASK_SCTP;
                message_p->destinationTaskId = TASK_S1AP;

                message_p->msg.s1apSctpAssociationClosed.assocId = snp->sn_shutdown_event.sse_assoc_id;
                SCTP_DEBUG("Client on association %d deconnected\n", snp->sn_shutdown_event.sse_assoc_id);

                send_msg_to_task(TASK_S1AP, message_p);
            }
            /* Association has changed. This can be either a new connection or the end. */
            else if (SCTP_ASSOC_CHANGE == snp->sn_header.sn_type)
            {
                struct sctp_assoc_change *sctp_assoc_changed;
                sctp_assoc_changed = &snp->sn_assoc_change;

                SCTP_DEBUG("Client association changed: %d\n", sctp_assoc_changed->sac_state);
                /* New physical association requested by a peer */
                switch (sctp_assoc_changed->sac_state) {
                    case SCTP_COMM_UP:
                    {
                        sctp_descriptor_t *new_association;

                        SCTP_DEBUG("New connection\n");
                        if ((new_association = sctp_add_new_peer()) == NULL) {
                            // TODO: handle this case
                        } else {
                            new_association->fd         = clientsock;
                            new_association->ppid       = sctp_arg_p->ppid;
                            new_association->instreams  = sctp_assoc_changed->sac_inbound_streams;
                            new_association->outstreams = sctp_assoc_changed->sac_outbound_streams;
                            new_association->assoc_id   = sctp_assoc_changed->sac_assoc_id;
                            memcpy((void *)&new_association->sin, (void *)&addr, sizeof(struct sockaddr_in));
                            sctp_dump_list();
                        }
                    } break;
                    case SCTP_SHUTDOWN_COMP:
                    {
                        if (sctp_remove_assoc_from_list(sctp_assoc_changed->sac_assoc_id) < 0) {
                            SCTP_DEBUG("Failed to find client in list\n");
                        }
                        sctp_dump_list();
                    } break;
                    default:
                        break;
                }
            }
        }
        else
        {
            MessageDef *message_p;
            sctp_descriptor_t *association;
            message_p = (MessageDef *)malloc(sizeof(MessageDef));

            if (message_p == NULL)
                continue;

            if ((association = sctp_is_assoc_in_list(sinfo.sinfo_assoc_id)) == NULL) {
                // TODO: handle this case
                free(message_p);
                continue;
            }
            association->messages_recv++;

            if (ntohl(sinfo.sinfo_ppid) != association->ppid) {
                /* Mismatch in Payload Protocol Identifier,
                 * may be we received unsollicited traffic from stack other than S1AP.
                 */
                SCTP_ERROR("Received data from peer with unsollicited PPID %d, expecting %d\n",
                           ntohl(sinfo.sinfo_ppid), association->ppid);
            }

            message_p->messageId = S1AP_SCTP_NEW_MESSAGE_IND;
            message_p->originTaskId = TASK_SCTP;
            message_p->destinationTaskId = TASK_S1AP;
            message_p->msg.s1apSctpNewMessageInd.buffer = malloc(sizeof(uint8_t) * n);
            memcpy((void *)message_p->msg.s1apSctpNewMessageInd.buffer, (void*)buffer, n);
            message_p->msg.s1apSctpNewMessageInd.stream = sinfo.sinfo_stream;
            message_p->msg.s1apSctpNewMessageInd.bufLen = n;
            message_p->msg.s1apSctpNewMessageInd.assocId = sinfo.sinfo_assoc_id;
            message_p->msg.s1apSctpNewMessageInd.instreams = association->instreams;
            message_p->msg.s1apSctpNewMessageInd.outstreams = association->outstreams;
            SCTP_DEBUG("Msg of length %d received from %s:%u on stream %d, PPID %d, assoc_id %d\n",
                    n, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port),
                    sinfo.sinfo_stream, ntohl(sinfo.sinfo_ppid), sinfo.sinfo_assoc_id);

            send_msg_to_task(TASK_S1AP, message_p);
        }
    }
    free(args_p);
    return NULL;
}

static void *sctp_intertask_interface(void *args_p) {
    while(1) {
        MessageDef *receivedMessage;
        receive_msg(TASK_SCTP, &receivedMessage);
        switch(receivedMessage->messageId) {
            case SCTP_S1AP_INIT:
            {
                SctpS1APInit *sctpS1APInitMsg;
                sctpS1APInitMsg = &receivedMessage->msg.sctpS1APInit;
                sctp_create_new_connection(sctpS1APInitMsg->port, sctpS1APInitMsg->address, sctpS1APInitMsg->ppid);
            } break;
            case SCTP_CLOSE_ASSOCIATION:
            {
                
            } break;

            case SCTP_NEW_DATA_REQ:
            {
                SctpNewDataReq *sctpNewS1APDataReq;
                sctpNewS1APDataReq = &receivedMessage->msg.sctpNewDataReq;

                if (sctp_send_msg(sctpNewS1APDataReq->assocId,
                                  sctpNewS1APDataReq->stream,
                                  sctpNewS1APDataReq->buffer,
                                  sctpNewS1APDataReq->bufLen) < 0) {
                    SCTP_DEBUG("Failed to send message over SCTP\n");
                }
            } break;
            default:
            {
                SCTP_DEBUG("Unknown message ID %d\n", receivedMessage->messageId);
            } break;
        }
        free(receivedMessage);
        receivedMessage = NULL;
    }
    return NULL;
}

int sctp_init(const mme_config_t *mme_config) {
    SCTP_DEBUG("Initializing SCTP task interface\n");
    if (pthread_create(&sctpThread, NULL, &sctp_intertask_interface, NULL) < 0) {
        perror("sctp pthread_create");
        return -1;
    }
    SCTP_DEBUG("Initializing SCTP task interface: DONE\n");
    return 0;
}
