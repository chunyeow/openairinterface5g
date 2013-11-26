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
 * Allows users to connect their itti_analyzer to this process and dump
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
#include <sched.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <sys/eventfd.h>

#include "assertions.h"
#include "liblfds611.h"

#include "intertask_interface.h"
#include "intertask_interface_dump.h"

#define SIGNAL_NAME_LENGTH  48

static const int itti_dump_debug = 0;

#define ITTI_DUMP_DEBUG(x, args...) do { if (itti_dump_debug) fprintf(stdout, "[ITTI][D]"x, ##args); } \
    while(0)
#define ITTI_DUMP_ERROR(x, args...) do { fprintf(stdout, "[ITTI][E]"x, ##args); } \
    while(0)

/* Message sent is an intertask dump type */
#define ITTI_DUMP_MESSAGE_TYPE      0x1
#define ITTI_STATISTIC_MESSAGE_TYPE 0x2
#define ITTI_DUMP_XML_DEFINITION    0x3
/* This signal is not meant to be used by remote analyzer */
#define ITTI_DUMP_EXIT_SIGNAL       0x4

typedef struct itti_dump_queue_item_s {
    void    *data;
    uint32_t data_size;
    uint32_t message_number;
    char     message_name[SIGNAL_NAME_LENGTH];
    uint32_t message_type;
    uint32_t message_size;
} itti_dump_queue_item_t;

typedef struct {
    int      sd;
    uint32_t last_message_number;
} itti_client_desc_t;

typedef struct itti_desc_s {
    /* Asynchronous thread that write to file/accept new clients */
    pthread_t      itti_acceptor_thread;
    pthread_attr_t attr;

    /* List of messages to dump.
     * NOTE: we limit the size of this queue to retain only the last exchanged
     * messages. The size can be increased by setting up the ITTI_QUEUE_MAX_ELEMENTS
     * in mme_default_values.h or by putting a custom in the configuration file.
     */
    struct lfds611_ringbuffer_state *itti_message_queue;

    uint32_t queue_size;

    int nb_connected;

    /* Event fd used to notify new messages (semaphore) */
    int event_fd;

    int itti_listen_socket;

    itti_client_desc_t itti_clients[ITTI_DUMP_MAX_CON];
} itti_desc_t;

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

static itti_desc_t itti_dump_queue;
static FILE *dump_file;

static int itti_dump_send_message(int sd, itti_dump_queue_item_t *message);
static int itti_dump_handle_new_connection(int sd, const char *xml_definition,
                                      uint32_t xml_definition_length);
static int itti_dump_send_xml_definition(const int sd, const char *message_definition_xml,
                                         const uint32_t message_definition_xml_length);

static
int itti_dump_send_message(int sd, itti_dump_queue_item_t *message)
{
    itti_dump_message_t *new_message;
    ssize_t bytes_sent = 0, total_sent = 0;
    uint8_t *data_ptr;

    /* Allocate memory for message header and payload */
    size_t size = sizeof(itti_dump_message_t) + message->data_size;

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

    data_ptr = (uint8_t *)&new_message[0];

    do {
        bytes_sent = send(sd, &data_ptr[total_sent], size - total_sent, 0);
        if (bytes_sent < 0) {
            ITTI_DUMP_ERROR("[%d] Failed to send %zu bytes to socket (%d:%s)\n",
                       sd, size, errno, strerror(errno));
            free(new_message);
            return -1;
        }
        total_sent += bytes_sent;
    } while (total_sent != size);

    free(new_message);
    return total_sent;
}

static void itti_dump_fwrite_message(itti_dump_queue_item_t *message)
{
    itti_socket_header_t header;

    if (dump_file != NULL && message) {

        header.message_size = message->message_size + sizeof(itti_dump_message_t);
        header.message_type = message->message_type;

        fwrite (&header, sizeof(itti_socket_header_t), 1, dump_file);
        fwrite (&message->message_number, sizeof(message->message_number), 1, dump_file);
        fwrite (message->message_name, sizeof(message->message_name), 1, dump_file);
        fwrite (message->data, message->data_size, 1, dump_file);
    }
}

static int itti_dump_send_xml_definition(const int sd, const char *message_definition_xml,
                                         const uint32_t message_definition_xml_length)
{
    itti_socket_header_t *itti_dump_message;
    /* Allocate memory for message header and payload */
    size_t itti_dump_message_size;
    ssize_t bytes_sent = 0, total_sent = 0;
    uint8_t *data_ptr;

    DevCheck(sd > 0, sd, 0, 0);
    DevAssert(message_definition_xml != NULL);

    itti_dump_message_size = sizeof(itti_socket_header_t) + message_definition_xml_length;

    itti_dump_message = calloc(1, itti_dump_message_size);

    ITTI_DUMP_DEBUG("[%d] Sending XML definition message of size %zu to observer peer\n",
               sd, itti_dump_message_size);

    itti_dump_message->message_size = itti_dump_message_size;
    itti_dump_message->message_type = ITTI_DUMP_XML_DEFINITION;

    /* Copying message definition */
    memcpy(&itti_dump_message[1], message_definition_xml, message_definition_xml_length);

    data_ptr = (uint8_t *)&itti_dump_message[0];

    do {
        bytes_sent = send(sd, &data_ptr[total_sent], itti_dump_message_size - total_sent, 0);
        if (bytes_sent < 0) {
            ITTI_DUMP_ERROR("[%d] Failed to send %zu bytes to socket (%d:%s)\n",
                       sd, itti_dump_message_size, errno, strerror(errno));
            free(itti_dump_message);
            return -1;
        }
        total_sent += bytes_sent;
    } while (total_sent != itti_dump_message_size);

    free(itti_dump_message);

    return 0;
}

static int itti_dump_enqueue_message(itti_dump_queue_item_t *new, uint32_t message_size,
                                uint32_t message_type)
{
    ssize_t  write_ret;
    uint64_t sem_counter = 1;

    struct lfds611_freelist_element *new_queue_element = NULL;

    DevAssert(new != NULL);

    new->message_type = message_type;
    new->message_size = message_size;

    new_queue_element = lfds611_ringbuffer_get_write_element(
        itti_dump_queue.itti_message_queue, &new_queue_element, NULL);

    lfds611_freelist_set_user_data_in_element(new_queue_element, (void *)new);

    lfds611_ringbuffer_put_write_element(itti_dump_queue.itti_message_queue,
                                         new_queue_element);

    /* Call to write for an event fd must be of 8 bytes */
    write_ret = write(itti_dump_queue.event_fd, &sem_counter, sizeof(sem_counter));
    DevCheck(write_ret == sizeof(sem_counter), write_ret, sem_counter, 0);

    return 0;
}

int itti_dump_queue_message(message_number_t message_number,
                            MessageDef *message_p,
                            const char *message_name,
                            const uint32_t message_size)
{
    itti_dump_queue_item_t *new;
    size_t message_name_length;
    int i;

    DevAssert(message_name != NULL);
    DevAssert(message_p != NULL);

    new = calloc(1, sizeof(itti_dump_queue_item_t));

    if (new == NULL) {
        ITTI_DUMP_ERROR("Failed to allocate memory (%s:%d)\n",
                   __FILE__, __LINE__);
        return -1;
    }

    new->data = malloc(message_size);

    if (new->data == NULL) {
        ITTI_DUMP_ERROR("Failed to allocate memory (%s:%d)\n",
                   __FILE__, __LINE__);
        return -1;
    }
    memcpy(new->data, message_p, message_size);
    new->data_size      = message_size;
    new->message_number = message_number;

    message_name_length = strlen(message_name) + 1;
    DevCheck(message_name_length <= SIGNAL_NAME_LENGTH, message_name_length,
             SIGNAL_NAME_LENGTH, 0);
    memcpy(new->message_name, message_name, message_name_length);

    itti_dump_enqueue_message(new, message_size, ITTI_DUMP_MESSAGE_TYPE);

    for (i = 0; i < ITTI_DUMP_MAX_CON; i++) {
        if (itti_dump_queue.itti_clients[i].sd == -1)
            continue;
        itti_dump_send_message(itti_dump_queue.itti_clients[i].sd, new);
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
    fd_set read_set, working_set;
    struct sockaddr_in servaddr; /* socket address structure */

    ITTI_DUMP_DEBUG("Creating TCP dump socket on port %u\n", ITTI_PORT);

    message_definition_xml = (char *)arg_p;
    DevAssert(message_definition_xml != NULL);

    message_definition_xml_length = strlen(message_definition_xml) + 1;

    if ((itti_listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        ITTI_DUMP_ERROR("Socket creation failed (%d:%s)\n", errno, strerror(errno));
        pthread_exit(NULL);
    }

    /* Allow socket reuse */
    rc = setsockopt(itti_listen_socket, SOL_SOCKET, SO_REUSEADDR,
                    (char *)&on, sizeof(on));
    if (rc < 0) {
        ITTI_DUMP_ERROR("setsockopt SO_REUSEADDR failed (%d:%s)\n", errno, strerror(errno));
        close(itti_listen_socket);
        pthread_exit(NULL);
    }

    /* Set socket to be non-blocking.
     * NOTE: sockets accepted will inherit this option.
     */
    rc = ioctl(itti_listen_socket, FIONBIO, (char *)&on);
    if (rc < 0) {
        ITTI_DUMP_ERROR("ioctl FIONBIO (non-blocking) failed (%d:%s)\n", errno, strerror(errno));
        close(itti_listen_socket);
        pthread_exit(NULL);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(ITTI_PORT);

    if (bind(itti_listen_socket, (struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        ITTI_DUMP_ERROR("Bind failed (%d:%s)\n", errno, strerror(errno));
        pthread_exit(NULL);
    }
    if (listen(itti_listen_socket, 5) < 0) {
        ITTI_DUMP_ERROR("Listen failed (%d:%s)\n", errno, strerror(errno));
        pthread_exit(NULL);
    }

    FD_ZERO(&read_set);

    /* Add the listener */
    FD_SET(itti_listen_socket, &read_set);

    /* Add the event fd */
    FD_SET(itti_dump_queue.event_fd, &read_set);

    /* Max of both sd */
    max_sd = itti_listen_socket > itti_dump_queue.event_fd ? itti_listen_socket : itti_dump_queue.event_fd;

    itti_dump_queue.itti_listen_socket = itti_listen_socket;

    /* Loop waiting for incoming connects or for incoming data
     * on any of the connected sockets.
     */
    while (1) {
        int desc_ready;
        int client_socket = -1;
        int i;

        memcpy(&working_set, &read_set, sizeof(read_set));

        /* No timeout: select blocks till a new event has to be handled
         * on sd's.
         */
        rc = select(max_sd + 1, &working_set, NULL, NULL, NULL);

        if (rc < 0) {
            ITTI_DUMP_ERROR("select failed (%d:%s)\n", errno, strerror(errno));
            pthread_exit(NULL);
        }

        desc_ready = rc;
        for (i = 0; i <= max_sd && desc_ready > 0; i++)
        {
            if (FD_ISSET(i, &working_set))
            {
                desc_ready -= 1;

                if (i == itti_dump_queue.event_fd) {
                    /* Notification of new element to dump from other tasks */
                    uint64_t sem_counter;
                    ssize_t  read_ret;
                    void    *user_data;
                    int j;

                    struct lfds611_freelist_element *element;

                    /* Read will always return 1 */
                    read_ret = read (itti_dump_queue.event_fd, &sem_counter, sizeof(sem_counter));
                    if (read_ret < 0) {
                        ITTI_DUMP_ERROR("Failed read for semaphore: %s\n", strerror(errno));
                        pthread_exit(NULL);
                    }
                    DevCheck(read_ret == sizeof(sem_counter), read_ret, sizeof(sem_counter), 0);

                    /* Acquire the ring element */
                    lfds611_ringbuffer_get_read_element(itti_dump_queue.itti_message_queue, &element);

                    DevAssert(element != NULL);

                    /* Retrieve user part of the message */
                    lfds611_freelist_get_user_data_from_element(element, &user_data);

                    if (((itti_dump_queue_item_t *)user_data)->message_type == ITTI_DUMP_EXIT_SIGNAL)
                    {
                        close(itti_dump_queue.event_fd);
                        close(itti_dump_queue.itti_listen_socket);

                        lfds611_ringbuffer_put_read_element(itti_dump_queue.itti_message_queue, element);

                        /* Leave the thread as we detected end signal */
                        pthread_exit(NULL);
                    }

                    /* Write message to file */
                    itti_dump_fwrite_message((itti_dump_queue_item_t *)user_data);

                    /* Send message to remote analyzer */
                    for (j = 0; j < ITTI_DUMP_MAX_CON; j++) {
                        if (itti_dump_queue.itti_clients[i].sd > 0) {
                            itti_dump_send_message(itti_dump_queue.itti_clients[i].sd,
                                                   (itti_dump_queue_item_t *)user_data);
                        }
                    }

                    /* We have finished with this element, reinsert it in the ring buffer */
                    lfds611_ringbuffer_put_read_element(itti_dump_queue.itti_message_queue, element);

                    ITTI_DUMP_DEBUG("Write element to file\n");
                } else if (i == itti_listen_socket) {
                    do {
                        client_socket = accept(itti_listen_socket, NULL, NULL);
                        if (client_socket < 0) {
                            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                                /* No more new connection */
                                ITTI_DUMP_DEBUG("No more new connection\n");
                                continue;
                            } else {
                                ITTI_DUMP_ERROR("accept failed (%d:%s)\n", errno, strerror(errno));
                                pthread_exit(NULL);
                            }
                        }
                        if (itti_dump_handle_new_connection(client_socket, message_definition_xml,
                            message_definition_xml_length) == 0)
                        {
                            /* The socket has been accepted.
                             * We have to update the set to include this new sd.
                             */
                            FD_SET(client_socket, &read_set);
                            if (client_socket > max_sd)
                                max_sd = client_socket;
                        }
                    } while(client_socket != -1);
                } else {
                    /* For now the MME itti dumper should not receive data
                     * other than connection oriented (CLOSE).
                     */
                    uint8_t j;

                    ITTI_DUMP_DEBUG("Socket %d disconnected\n", i);

                    /* Close the socket and update info related to this connection */
                    close(i);

                    for (j = 0; j < ITTI_DUMP_MAX_CON; j++) {
                        if (itti_dump_queue.itti_clients[j].sd == i)
                            break;
                    }

                    /* In case we don't find the matching sd in list of known
                     * connections -> assert.
                     */
                    DevCheck(j < ITTI_DUMP_MAX_CON, j, ITTI_DUMP_MAX_CON, i);

                    /* Re-initialize the socket to -1 so we can accept new
                     * incoming connections.
                     */
                    itti_dump_queue.itti_clients[j].sd                  = -1;
                    itti_dump_queue.itti_clients[j].last_message_number = 0;
                    itti_dump_queue.nb_connected--;

                    /* Remove the socket from the FD set and update the max sd */
                    FD_CLR(i, &read_set);
                    if (i == max_sd)
                    {
                        if (itti_dump_queue.nb_connected == 0) {
                            /* No more new connection max_sd = itti_listen_socket */
                            max_sd = itti_listen_socket;
                        } else {
                            while (FD_ISSET(max_sd, &read_set) == 0) {
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
    if (itti_dump_queue.nb_connected < ITTI_DUMP_MAX_CON) {
        uint8_t i;

        for (i = 0; i < ITTI_DUMP_MAX_CON; i++) {
            /* Let's find a place to store the new client */
            if (itti_dump_queue.itti_clients[i].sd == -1) {
                break;
            }
        }

        ITTI_DUMP_DEBUG("Found place to store new connection: %d\n", i);

        DevCheck(i < ITTI_DUMP_MAX_CON, i, ITTI_DUMP_MAX_CON, sd);

        ITTI_DUMP_DEBUG("Socket %d accepted\n", sd);

        /* Send the XML message definition */
        if (itti_dump_send_xml_definition(sd, xml_definition, xml_definition_length) < 0) {
            ITTI_DUMP_ERROR("Failed to send XML definition\n");
            close (sd);
            return -1;
        }

        itti_dump_queue.itti_clients[i].sd = sd;
        itti_dump_queue.nb_connected++;
    } else {
        ITTI_DUMP_DEBUG("Socket %d rejected\n", sd);
        /* We have reached max number of users connected...
         * Reject the connection.
         */
        close (sd);
        return -1;
    }

    return 0;
}

int itti_dump_user_data_init_function(void **user_data, void *user_state)
{
    return 0;
}

/* This function should be called by each thread that will use the ring buffer */
void itti_dump_thread_use_ring_buffer(void)
{
    lfds611_ringbuffer_use(itti_dump_queue.itti_message_queue);
}

int itti_dump_init(const char * const messages_definition_xml, const char * const dump_file_name)
{
    int i, ret;
    struct sched_param scheduler_param;

    scheduler_param.sched_priority = 10;

    if (dump_file_name != NULL)
    {
        dump_file = fopen(dump_file_name, "wb");

        if (dump_file == NULL)
        {
            ITTI_DUMP_ERROR("can not open dump file \"%s\" (%d:%s)\n", dump_file_name, errno, strerror(errno));
        }
        else
        {
            /* Output the XML to file */
            uint32_t message_size = strlen(messages_definition_xml) + 1;
            itti_socket_header_t header;

            header.message_size = sizeof(itti_socket_header_t) + message_size;
            header.message_type = ITTI_DUMP_XML_DEFINITION;

            fwrite (&header, sizeof(itti_socket_header_t), 1, dump_file);
            fwrite (messages_definition_xml, message_size, 1, dump_file);
            fflush (dump_file);
        }
    }

    memset(&itti_dump_queue, 0, sizeof(itti_desc_t));

    ITTI_DUMP_DEBUG("Creating new ring buffer for itti dump of %u elements\n",
                    ITTI_QUEUE_MAX_ELEMENTS);

    if (lfds611_ringbuffer_new(&itti_dump_queue.itti_message_queue,
                               ITTI_QUEUE_MAX_ELEMENTS,
                               NULL,
                               NULL) != 1)
    {
        ITTI_DUMP_ERROR("Failed to create ring buffer...\n");
        /* Always assert on this condition */
        DevAssert(0 == 1);
    }

    itti_dump_queue.event_fd = eventfd(0, EFD_SEMAPHORE);
    if (itti_dump_queue.event_fd == -1)
    {
        ITTI_DUMP_ERROR("eventfd failed: %s\n", strerror(errno));
        /* Always assert on this condition */
        DevAssert(0 == 1);
    }

    itti_dump_queue.queue_size = 0;
    itti_dump_queue.nb_connected = 0;

    for(i = 0; i < ITTI_DUMP_MAX_CON; i++) {
        itti_dump_queue.itti_clients[i].sd = -1;
        itti_dump_queue.itti_clients[i].last_message_number = 0;
    }

    /* initialized with default attributes */
    ret = pthread_attr_init(&itti_dump_queue.attr);
    if (ret < 0) {
        ITTI_DUMP_ERROR("pthread_attr_init failed (%d:%s)\n", errno, strerror(errno));
        DevAssert(0 == 1);
    }

    ret = pthread_attr_setschedpolicy(&itti_dump_queue.attr, SCHED_RR);
    if (ret < 0) {
        ITTI_DUMP_ERROR("pthread_attr_setschedpolicy (SCHED_IDLE) failed (%d:%s)\n", errno, strerror(errno));
        DevAssert(0 == 1);
    }
    ret = pthread_attr_setschedparam(&itti_dump_queue.attr, &scheduler_param);
    if (ret < 0) {
        ITTI_DUMP_ERROR("pthread_attr_setschedparam failed (%d:%s)\n", errno, strerror(errno));
        DevAssert(0 == 1);
    }

    ret = pthread_create(&itti_dump_queue.itti_acceptor_thread, &itti_dump_queue.attr,
                         &itti_dump_socket, (void *)messages_definition_xml);
    if (ret < 0) {
        ITTI_DUMP_ERROR("pthread_create failed (%d:%s)\n", errno, strerror(errno));
        DevAssert(0 == 1);
    }

    return 0;
}

void itti_dump_user_data_delete_function(void *user_data, void *user_state)
{
    if (user_data != NULL)
    {
        itti_dump_queue_item_t *item;

        item = (itti_dump_queue_item_t *)user_data;
        if (item->data != NULL)
        {
            free(item->data);
        }
        free(item);
    }
}

void itti_dump_exit(void)
{
    void *arg;
    itti_dump_queue_item_t *new;

    new = calloc(1, sizeof(itti_dump_queue_item_t));

    /* Send the exit signal to other thread */
    itti_dump_enqueue_message(new, 0, ITTI_DUMP_EXIT_SIGNAL);

    ITTI_DUMP_DEBUG("waiting for dumper thread to finish\n");

    /* wait for the thread to terminate */
    pthread_join(itti_dump_queue.itti_acceptor_thread, &arg);

    ITTI_DUMP_DEBUG("dumper thread correctly exited\n");

    if (dump_file != NULL)
    {
        /* Synchronise file and then close it */
        fclose(dump_file);
        dump_file = NULL;
    }

    if (itti_dump_queue.itti_message_queue)
    {
        lfds611_ringbuffer_delete(itti_dump_queue.itti_message_queue,
                                  itti_dump_user_data_delete_function, NULL);
    }
}
