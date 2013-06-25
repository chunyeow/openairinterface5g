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

#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//WARNING: sctp requires libsctp-dev and -lsctp as linker option
#include <netinet/sctp.h>
#include <arpa/inet.h>

#include <pthread.h>

#if defined(ENB_MODE)
# include "log.h"
# define SCTP_ERROR(x, args...) LOG_E(SCTP, x, ##args)
# define SCTP_DEBUG(x, args...) LOG_D(SCTP, x, ##args)
#else
# define SCTP_ERROR(x, args...) do { fprintf(stderr, "[SCTP][E]"x, ##args); } while(0)
# define SCTP_DEBUG(x, args...) do { fprintf(stdout, "[SCTP][D]"x, ##args); } while(0)
#endif

#define SCTP_RECV_BUFFER_SIZE 1024

#include "sctp_primitives_client.h"

/* SCTP descriptor. Describe an SCTP association. */
struct sctp_descriptor_s {
    struct sctp_descriptor_s *next_desc;    ///< Next SCTP descriptor in list

    int                fd;                  ///< Socket descriptor
    uint32_t           sctpAssocId;         ///< SCTP asscociation ID
    uint32_t           ppid;                ///< Payload Protocol Identifier
    pthread_t          recvThread;          ///< Receiver thread
    sctp_recv_callback recv_callback;       ///< Data received callback
};

//Local variables declarations
struct sctp_descriptor_s *sctp_list_head = NULL;
struct sctp_descriptor_s *sctp_list_tail = NULL;
int sctp_nb_association = 0;

//Local functions declarations
static int sctp_create_receiver_thread(struct sctp_descriptor_s *sctp_ref);
void *sctp_recv_msg(void *arg_p);

/* Add new SCTP association to the list */
static struct sctp_descriptor_s *sctp_add_new_association(void) {
    struct sctp_descriptor_s *sctp_ref;

    sctp_ref = malloc(sizeof(struct sctp_descriptor_s));

    /* Nothing allocated */
    if (sctp_ref == NULL) return NULL;
    /* No element in list */
    if (sctp_list_head == NULL) {
        sctp_list_head = sctp_list_tail = sctp_ref;
    } else {
        sctp_list_tail->next_desc = sctp_ref;
        sctp_list_tail = sctp_ref;
    }
    sctp_ref->next_desc = NULL;
    sctp_nb_association++;
    return sctp_ref;
}

/* Look for an SCTP association in the list */
static struct sctp_descriptor_s *sctp_is_assoc_id_in_list(uint32_t assocId) {
    struct sctp_descriptor_s *sctp_ref;

    sctp_ref = sctp_list_head;

    while(sctp_ref != NULL) {
        if (sctp_ref->sctpAssocId == assocId)
            return sctp_ref;
        sctp_ref = sctp_ref->next_desc;
    }
    return NULL;
}

/* Perform an implict connection to remote host in case of SOCK_SEQPACKET socket type.
 * Otherwise perform an explicit connect (remote host should accept connection).
 */
int sctp_connect_to_remote_host(
    const char *ip_addr,
    uint16_t    port,
    uint32_t    ppid,
    void       *args,
    sctp_connected_callback connected_callback,
    sctp_recv_callback upperlayer_recv) {

    int fd, i;
    struct sockaddr_in addr;

    struct sctp_event_subscribe events;
    struct sctp_status status;
    struct sctp_initmsg init;

    struct sctp_descriptor_s *sctp_ref;

    assert(ip_addr != NULL);

    /* Create new socket */
#if defined(SOCK_STREAM)
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) < 0) {
#else
    if ((fd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP)) < 0) {
#endif
        SCTP_ERROR("Socket creation failed: %s\n", strerror(errno));
        return -1;
    }

    memset((void *)&init, 0, sizeof(struct sctp_initmsg));

    /* Request a number of in/out streams */
    init.sinit_num_ostreams = 128;
    init.sinit_max_instreams = 128;
    init.sinit_max_attempts = 5;

    SCTP_DEBUG("Requesting (%d %d) (in out) streams\n", init.sinit_num_ostreams, init.sinit_max_instreams);

    if (setsockopt(fd, IPPROTO_SCTP, SCTP_INITMSG,
        &init, (socklen_t)sizeof(struct sctp_initmsg)) < 0)
    {
        SCTP_ERROR("Setsockopt IPPROTO_SCTP_INITMSG failed: %s\n", strerror(errno));
        return -1;
    }

    /* Subscribe to all events */
    memset((void *)&events, 1, sizeof(struct sctp_event_subscribe));
    if (setsockopt(fd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(struct sctp_event_subscribe)) < 0) {
        SCTP_ERROR("Setsockopt IPPROTO_SCTP_EVENTS failed: %s\n", strerror(errno));
        return -1;
    }

#if defined (SOCK_STREAM)
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip_addr);
    /* Connect to remote host and port */
    if (connect(fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        SCTP_ERROR("Connect to %s:%u failed: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
        return -1;
    }
#endif

    /* Get SCTP status */
    i = sizeof(status);
    memset(&status, 0, sizeof(struct sctp_status));
    if(getsockopt(fd, IPPROTO_SCTP, SCTP_STATUS, &status, (socklen_t *)&i) < 0)
    {
        SCTP_ERROR("Getsockopt IPPROTO_SCTP failed: %s\n", strerror(errno));
        return -1;
    }

    SCTP_DEBUG("SCTP Status:\n--------\n");
    SCTP_DEBUG("assoc id  = %d\n", status.sstat_assoc_id);
    SCTP_DEBUG("state     = %d\n", status.sstat_state);
    SCTP_DEBUG("instrms   = %d\n", status.sstat_instrms);
    SCTP_DEBUG("outstrms  = %d\n--------\n", status.sstat_outstrms);

    if ((sctp_ref = sctp_add_new_association()) == NULL) {
        return -1;
    }

    sctp_ref->fd = fd;
    sctp_ref->ppid = ppid;
    sctp_ref->sctpAssocId = status.sstat_assoc_id;
    sctp_ref->recv_callback = upperlayer_recv;

    /* Create new receiver thread */
    sctp_create_receiver_thread(sctp_ref);

    /* Call the connected callback (if any provided). */
    if (connected_callback != NULL) {
        connected_callback(args, status.sstat_assoc_id, status.sstat_instrms, status.sstat_outstrms);
    }

    return 0;
}

void sctp_disconnect(uint32_t assocId) {
    struct sctp_descriptor_s *sctp_ref;

    if ((sctp_ref = sctp_is_assoc_id_in_list(assocId)) == NULL) {
        return;
    }
    // Closing connection
    close(sctp_ref->fd);

    //TODO: remove association from list
}

/* Send buffer to SCTP association */
int sctp_send_msg(uint32_t sctpAssocId, uint16_t stream, const uint8_t *buffer, const uint32_t length)
{
    int fd;
    struct sctp_descriptor_s *sctp_ref;

    assert(buffer != NULL);

    if ((sctp_ref = sctp_is_assoc_id_in_list(sctpAssocId)) == NULL) {
        /* Can't find assoc id in list of active associations */
        return -1;
    }

    fd = sctp_ref->fd;

    /* Send message on specified stream of the fd association */
    if (sctp_sendmsg(fd, (const void *)buffer, length, NULL, 0,
        ntohs(sctp_ref->ppid), 0, stream, 0, 0) < 0) {
        SCTP_ERROR("Scpt_sendmsg failed: %s\n", strerror(errno));
        return -1;
    }

    SCTP_DEBUG("Successfully sent %d bytes on stream %d\n", length, stream);

    return 0;
}

void *sctp_recv_msg(void *arg_p)
{
    struct sctp_descriptor_s *sctp_ref;
    struct sctp_sndrcvinfo    sinfo;
    int fd;

    sctp_ref = (struct sctp_descriptor_s*)arg_p;
    fd = sctp_ref->fd;

    while(1)
    {
        int flags = 0, n;
        struct sockaddr_in addr;
        socklen_t from_len;
        uint8_t buffer[SCTP_RECV_BUFFER_SIZE];

        memset((void *)&addr, 0, sizeof(struct sockaddr_in));
        from_len = (socklen_t)sizeof(struct sockaddr_in);
        memset((void *)&sinfo, 0, sizeof(struct sctp_sndrcvinfo));
        n = sctp_recvmsg(fd, (void*)buffer, SCTP_RECV_BUFFER_SIZE,
                         (struct sockaddr *)&addr, &from_len,
                         &sinfo, &flags);

        if (n == 0)
        {
            SCTP_DEBUG("Server deconnected as recv returned 0\n");
            break;
        }
        if (flags & MSG_NOTIFICATION)
        {
            union sctp_notification *snp;
            snp = (union sctp_notification *)buffer;

            /* Client deconnection */
            if (SCTP_SHUTDOWN_EVENT == snp->sn_header.sn_type)
            {
                SCTP_DEBUG("Notification received: server deconnected\n");
                break;
            }
            /* Association has changed */
            else if (SCTP_ASSOC_CHANGE == snp->sn_header.sn_type)
            {
                SCTP_DEBUG("Notification received: server association changed\n");
            }
        }
        else
        {
            SCTP_DEBUG("[FD %d] Msg of length %d received from %s:%u on stream %d, PPID %d, assoc_id %d\n",
                    sctp_ref->sctpAssocId, n, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port),
                    sinfo.sinfo_stream, sinfo.sinfo_ppid, sinfo.sinfo_assoc_id);
            //calling given recv callback
            if (sctp_ref->recv_callback != NULL) {
                uint8_t *buffer2;

                buffer2 = malloc(n * sizeof(uint8_t));
                memcpy(buffer2, buffer, n);
                sctp_ref->recv_callback(sinfo.sinfo_assoc_id, sinfo.sinfo_stream, buffer2, n);
            }
        }
    }

    //TODO: handle deconnection
    return NULL;
}

static int sctp_create_receiver_thread(struct sctp_descriptor_s *sctp_ref) {

    assert(sctp_ref != NULL);

    if (pthread_create(&sctp_ref->recvThread, NULL, &sctp_recv_msg, (void *)sctp_ref) < 0)
    {
        SCTP_ERROR("Pthread_create failed: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

void sctp_terminate(void) {
    struct sctp_descriptor_s *sctp_ref = sctp_list_head;

    while (sctp_ref != NULL) {
        sctp_disconnect(sctp_ref->sctpAssocId);
        sctp_ref = sctp_ref->next_desc;
    }
}
