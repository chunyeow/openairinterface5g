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

/** @brief Intertask Interface Signal Dumper
 * Allows users to connect their itti_debugger to this process and dump
 * signals exchanged between tasks.
 * @author Sebastien Roux <sebastien.roux@eurecom.fr>
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "assertions.h"
#include "queue.h"

#include "intertask_interface.h"
#include "intertask_interface_dump.h"

#define SIGNAL_NAME_LENGTH  50

/* Declared in intertask_interface.c */
extern int itti_debug;

#define ITTI_DEBUG(x, args...) do { fprintf(stdout, "[ITTI][D]"x, ##args); } \
    while(0)
#define ITTI_ERROR(x, args...) do { fprintf(stdout, "[ITTI][E]"x, ##args); } \
    while(0)

typedef struct itti_queue_item_s {
    STAILQ_ENTRY(itti_queue_item_s) entry;
    void    *data;
    uint32_t data_size;
    uint32_t message_number;
    char     message_name[SIGNAL_NAME_LENGTH];
} itti_queue_item_t;

typedef struct {
    int      sd;
    uint32_t last_message_number;
} itti_client_desc_t;

typedef struct itti_desc_s {
    /* The acceptor thread */
    pthread_t itti_acceptor_thread;
    pthread_t itti_write_thread;

    /* Protect the circular queue */
    pthread_mutex_t queue_mutex;

    /* List of messages to dump.
     * NOTE: we limit the size of this queue to retain only the last exchanged
     * messages. The size can be increased by setting up the ITTI_QUEUE_SIZE_MAX
     * in mme_default_values.h or by putting a custom in the configuration file.
     */
    STAILQ_HEAD(itti_queue_s, itti_queue_item_s) itti_message_queue;
    struct itti_queue_item_s *itti_queue_last;
    uint32_t queue_size;

    int nb_connected;

    itti_client_desc_t itti_clients[ITTI_DUMP_MAX_CON];
} itti_desc_t;

static itti_desc_t itti_queue;

/* Message sent is an intertask dump type */
#define ITTI_DUMP_MESSAGE_TYPE      0x1
#define ITTI_STATISTIC_MESSAGE_TYPE 0x2
#define ITTI_DUMP_XML_DEFINITION    0x3

typedef struct {
    /* The size of this structure */
    uint32_t message_size;
    uint32_t message_type;
} itti_socket_header_t;

typedef struct {
    itti_socket_header_t header;

    uint32_t message_number;
    char signal_name[SIGNAL_NAME_LENGTH];

    /* Message payload is added here, this struct is used as an header */
} itti_dump_message_t;

typedef struct {
    itti_socket_header_t header;
    
} itti_statistic_message_t;

static int itti_dump_send_message(int sd, itti_queue_item_t *message);
static int itti_dump_handle_new_connection(int sd, const char *xml_definition,
                                      uint32_t xml_definition_length);
static int itti_dump_send_xml_definition(const int sd, const char *message_definition_xml,
                                         const uint32_t message_definition_xml_length);

static
int itti_dump_send_message(int sd, itti_queue_item_t *message)
{
    int result = 0;
    itti_dump_message_t *new_message;

    /* Allocate memory for message header and payload */
    uint32_t size = sizeof(itti_dump_message_t) + message->data_size;

    DevCheck(sd > 0, sd, 0, 0);
    DevAssert(message != NULL);

    new_message = calloc(1, size);
    DevAssert(new_message != NULL);

    /* Preparing the header */
    new_message->header.message_size = size;
    new_message->header.message_type = ITTI_DUMP_MESSAGE_TYPE;

    new_message->message_number = message->message_number;
    /* Copy the name, but leaves last byte set to 0 in case name is too long */
    memcpy(new_message->signal_name, message->message_name, SIGNAL_NAME_LENGTH - 1);
    /* Appends message payload */
    memcpy(&new_message[1], message->data, message->data_size);

    if (write(sd, new_message, size) == -1) {
        ITTI_ERROR("[%d] Failed to write message of size %u to socket (%d:%s)\n",
                   sd, size, errno, strerror(errno));
        result = -1;
    }

    free(new_message);
    return result;
}

static int itti_dump_send_xml_definition(const int sd, const char *message_definition_xml,
                                         const uint32_t message_definition_xml_length)
{
    itti_socket_header_t xml_definition_header;

    DevCheck(sd > 0, sd, 0, 0);
    DevAssert(message_definition_xml != NULL);

    ITTI_DEBUG("[%d] Sending XML definition of size %u to observer peer\n",
               sd, message_definition_xml_length);

    xml_definition_header.message_size = sizeof(xml_definition_header)
    + message_definition_xml_length;
    xml_definition_header.message_type = ITTI_DUMP_XML_DEFINITION;

    if (write(sd, &xml_definition_header, sizeof(xml_definition_header)) < 0) {
        ITTI_ERROR("[%d] Failed to write header of size %zu to socket (%d:%s)\n",
                   sd, sizeof(xml_definition_header), errno, strerror(errno));
        return -1;
    }
    if (write(sd, message_definition_xml, message_definition_xml_length) < 0) {
        ITTI_ERROR("[%d] Failed to write XML definition of size %u to socket (%d:%s)\n",
                   sd, message_definition_xml_length, errno, strerror(errno));
        return -1;
    }
    return 0;
}

int itti_dump_queue_message(message_number_t message_number,
                            MessageDef *message_p,
                            const char *message_name,
                            const uint32_t message_size)
{
    itti_queue_item_t *new;
    itti_queue_item_t *head = NULL;
    size_t message_name_length;
    int i;

    DevAssert(message_name != NULL);
    DevAssert(message_p != NULL);

    new = calloc(1, sizeof(itti_queue_item_t));

    if (new == NULL) {
        ITTI_ERROR("Failed to allocate memory (%s:%d)\n",
                   __FILE__, __LINE__);
        return -1;
    }

    new->data = malloc(message_size);

    if (new->data == NULL) {
        ITTI_ERROR("Failed to allocate memory (%s:%d)\n",
                   __FILE__, __LINE__);
        return -1;
    }
    memcpy(new->data, message_p, message_size);
    new->data_size = message_size;
    new->message_number = message_number;

    message_name_length = strlen(message_name) + 1;
    DevCheck(message_name_length <= SIGNAL_NAME_LENGTH, message_name_length,
             SIGNAL_NAME_LENGTH, 0);
    memcpy(new->message_name, message_name, message_name_length);

    /* Lock the queue mutex for writing to insert the new element */
    pthread_mutex_lock(&itti_queue.queue_mutex);

    /* We reached the maximum size for the queue of messages -> remove the head */
    if (itti_queue.queue_size + message_size > ITTI_QUEUE_SIZE_MAX) {
        head = STAILQ_FIRST(&itti_queue.itti_message_queue);
        /* Remove the head */
        STAILQ_REMOVE_HEAD(&itti_queue.itti_message_queue, entry);
    } else {
        itti_queue.queue_size += message_size;
    }
    /* Insert the packet at tail */
    STAILQ_INSERT_TAIL(&itti_queue.itti_message_queue, new, entry);
    itti_queue.itti_queue_last = new;

    /* Release the mutex */
    pthread_mutex_unlock(&itti_queue.queue_mutex);

    for (i = 0; i < ITTI_DUMP_MAX_CON; i++) {
        if (itti_queue.itti_clients[i].sd == -1)
            continue;
        itti_dump_send_message(itti_queue.itti_clients[i].sd, new);
    }

    /* No need to have the mutex locked to free data as at this point the message
     * is no more in the list.
     */
    if (head) {
        free(head->data);
        free(head);
        head = NULL;
    }

    return 0;
}

static void *itti_dump_socket(void *arg_p)
{
    uint32_t message_definition_xml_length;
    char *message_definition_xml;
    int rc;
    int itti_listen_socket, max_sd;
    int on = 1;
    fd_set master_set, working_set;
    struct sockaddr_in servaddr; /* socket address structure */

    ITTI_DEBUG("Creating TCP dump socket on port %u\n", ITTI_PORT);

    message_definition_xml = (char *)arg_p;
    DevAssert(message_definition_xml != NULL);

    message_definition_xml_length = strlen(message_definition_xml) + 1;

    if ((itti_listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ITTI_ERROR("Socket creation failed (%d:%s)\n", errno, strerror(errno));
        pthread_exit(NULL);
    }

    /* Allow socket reuse */
    rc = setsockopt(itti_listen_socket, SOL_SOCKET, SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0) {
        ITTI_ERROR("setsockopt SO_REUSEADDR failed (%d:%s)\n", errno, strerror(errno));
        close(itti_listen_socket);
        pthread_exit(NULL);
    }

    /* Set socket to be non-blocking.
     * NOTE: sockets accepted will inherit this option.
     */
    rc = ioctl(itti_listen_socket, FIONBIO, (char *)&on);
    if (rc < 0) {
        ITTI_ERROR("ioctl FIONBIO (non-blocking) failed (%d:%s)\n", errno, strerror(errno));
        close(itti_listen_socket);
        pthread_exit(NULL);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(ITTI_PORT);

    if (bind(itti_listen_socket, (struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0 ) {
        ITTI_ERROR("Bind failed (%d:%s)\n", errno, strerror(errno));
        pthread_exit(NULL);
    }
    if (listen(itti_listen_socket, 5) < 0 ) {
        ITTI_ERROR("Listen failed (%d:%s)\n", errno, strerror(errno));
        pthread_exit(NULL);
    }

    FD_ZERO(&master_set);
    max_sd = itti_listen_socket;
    FD_SET(itti_listen_socket, &master_set);

    /* Loop waiting for incoming connects or for incoming data
     * on any of the connected sockets.
     */
    while (1) {
        int desc_ready;
        int client_socket = -1;
        int i;

        memcpy(&working_set, &master_set, sizeof(master_set));

        ITTI_DEBUG("Stuck on select\n");

        /* No timeout: select blocks till a new event has to be handled
         * on sd's.
         */
        rc = select(max_sd + 1, &working_set, NULL, NULL, NULL);

        if (rc < 0) {
            ITTI_ERROR("select failed (%d:%s)\n", errno, strerror(errno));
            pthread_exit(NULL);
        }

        desc_ready = rc;
        for (i = 0; i <= max_sd && desc_ready > 0; i++) {
            if (FD_ISSET(i, &working_set)) {
                ITTI_DEBUG("Handling socket %d\n", i);
                desc_ready -= 1;
                /* Check if the socket where data available is the listening
                 * socket.
                 */
                if (i == itti_listen_socket) {
                    do {
                        client_socket = accept(itti_listen_socket, NULL, NULL);
                        if (client_socket < 0) {
                            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                                /* No more new connection */
                                ITTI_DEBUG("No more new connection\n");
                                continue;
                            } else {
                                ITTI_ERROR("accept failed (%d:%s)\n", errno, strerror(errno));
                                pthread_exit(NULL);
                            }
                        }
                        if (itti_dump_handle_new_connection(client_socket, message_definition_xml,
                            message_definition_xml_length) == 0) {
                            /* The socket has been accepted.
                             * We have to update the set to include this new sd.
                             */
                            FD_SET(client_socket, &master_set);
                            if (client_socket > max_sd)
                                max_sd = client_socket;
                        }
                    } while(client_socket != -1);
                } else {
                    /* For now the MME itti dumper should not receive data
                     * other than connection oriented (CLOSE).
                     */
                    uint8_t j;

                    ITTI_DEBUG("Socket %d disconnected\n", i);

                    /* Close the socket and update info related to this connection */
                    close(i);

                    for (j = 0; j < ITTI_DUMP_MAX_CON; j++) {
                        if (itti_queue.itti_clients[j].sd == i)
                            break;
                    }

                    /* In case we don't find the matching sd in list of known
                     * connections -> assert.
                     */
                    DevCheck(j < ITTI_DUMP_MAX_CON, j, ITTI_DUMP_MAX_CON, i);

                    /* Re-initialize the socket to -1 so we can accept new
                     * incoming connections.
                     */
                    itti_queue.itti_clients[j].sd                  = -1;
                    itti_queue.itti_clients[j].last_message_number = 0;
                    itti_queue.nb_connected--;

                    /* Remove the socket from the FD set and update the max sd */
                    FD_CLR(i, &master_set);
                    if (i == max_sd)
                    {
                        if (itti_queue.nb_connected == 0) {
                            /* No more new connection max_sd = itti_listen_socket */
                            max_sd = itti_listen_socket;
                        } else {
                            while (FD_ISSET(max_sd, &master_set) == 0) {
                                max_sd -= 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return NULL;
}

static
int itti_dump_handle_new_connection(int sd, const char *xml_definition, uint32_t xml_definition_length)
{
    if (itti_queue.nb_connected < ITTI_DUMP_MAX_CON) {
        itti_queue_item_t *item;
        uint8_t i;

        for (i = 0; i < ITTI_DUMP_MAX_CON; i++) {
            /* Let's find a place to store the new client */
            if (itti_queue.itti_clients[i].sd == -1) {
                break;
            }
        }

        ITTI_DEBUG("Found place to store new connection: %d\n", i);

        DevCheck(i < ITTI_DUMP_MAX_CON, i, ITTI_DUMP_MAX_CON, sd);
        itti_queue.itti_clients[i].sd = sd;
        itti_queue.nb_connected++;

        ITTI_DEBUG("Socket %d accepted\n", sd);

        /* Send the XML message definition */
        if (itti_dump_send_xml_definition(sd, xml_definition, xml_definition_length) < 0) {
            ITTI_ERROR("Failed to send XML definition\n");
            close (sd);
            return -1;
        }

        /* At this point we have to dump the complete list */
        pthread_mutex_lock(&itti_queue.queue_mutex);
        STAILQ_FOREACH(item, &itti_queue.itti_message_queue, entry) {
            itti_dump_send_message(sd, item);
        }
        pthread_mutex_unlock(&itti_queue.queue_mutex);
    } else {
        ITTI_DEBUG("Socket %d rejected\n", sd);
        /* We have reached max number of users connected...
         * Reject the connection.
         */
        close (sd);
        return -1;
    }

    return 0;
}

int itti_dump_init(const char * const messages_definition_xml)
{
    int i;

    memset(&itti_queue, 0, sizeof(itti_desc_t));

    pthread_mutex_init(&itti_queue.queue_mutex, NULL);
    STAILQ_INIT(&itti_queue.itti_message_queue);
    itti_queue.queue_size = 0;
    itti_queue.nb_connected = 0;

    for(i = 0; i < ITTI_DUMP_MAX_CON; i++) {
        itti_queue.itti_clients[i].sd = -1;
        itti_queue.itti_clients[i].last_message_number = 0;
    }
    if (pthread_create(&itti_queue.itti_acceptor_thread, NULL, &itti_dump_socket,
        (void *)messages_definition_xml) < 0) {
        ITTI_ERROR("pthread_create failed (%d:%s)\n", errno, strerror(errno));
        return -1;
    }
    return 0;
}
