#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <gtk/gtk.h>

#include "../rc.h"

#include "ui_interface.h"
#include "ui_notifications.h"

#include "socket.h"
#include "buffers.h"

#include "xml_parse.h"

#define BUFFER_SIZE 3000
#define MAX_ATTEMPTS    10

socket_data_t socket_data;

typedef struct {
    char     ip_address[16];
    uint16_t port;
} socket_arg_t;

/* Message header is the common part that should never change between
 * remote process and this one.
 */
typedef struct {
    /* The size of this structure */
    uint32_t message_size;
    uint32_t message_type;
} itti_socket_header_t;

typedef struct {
    uint32_t message_number;
    char     signal_name[50];
} itti_dump_message_t;

void *socket_read_data(void *arg);

void *socket_read_data(void *arg)
{
    int ret;
    int attempts = MAX_ATTEMPTS;
    int *sd = &socket_data.sd;
    struct sockaddr_in *si_me = &socket_data.si_me;
    socket_arg_t *socket_arg;

    socket_arg = (socket_arg_t *)arg;

    memset(&socket_data, 0, sizeof(socket_data_t));

    /* Enable the cancel attribute for the thread.
     * Set the cancel type to asynchronous. The thread termination is requested
     * to happen now.
     */
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

    *sd = -1;

    if ((*sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        fprintf(stderr, "socket failed\n");
        goto fail;
    }
    memset((void *)si_me, 0, sizeof(si_me));

    si_me->sin_family = AF_INET;
    si_me->sin_port = htons(socket_arg->port);
    if (inet_aton(socket_arg->ip_address, &si_me->sin_addr) == 0) {
        fprintf(stderr, "inet_aton() failed\n");
        goto fail;
    }

    do {
        ret = connect(*sd, (struct sockaddr *)si_me, sizeof(struct sockaddr_in));
        if (ret == 0) {
            break;
        }
        attempts --;
        sleep(1);
        if (attempts == 0) {
            printf("Cannot connect to %s with port %u:\n"
            "Reached maximum retry count\nEnsure host is alive",
            socket_arg->ip_address, socket_arg->port);
            /* ????? */
            ui_interface.ui_notification_dialog(
                DIALOG_ERROR, "Cannot connect to %s with port %u:\n"
                "Reached maximum retry count\nEnsure host is alive",
                socket_arg->ip_address, socket_arg->port);
            ui_interface.ui_enable_connect_button();

            pthread_exit(NULL);
        }
    } while(ret != 0);

    while(1) {
        uint8_t *data;
        size_t data_length;
        itti_socket_header_t message_header;
        int recv_ret;
        buffer_t *buffer;

        memset(&message_header, 0, sizeof(itti_socket_header_t));

        /* First run acquire message header
         * TODO: check for remote endianness when retrieving the structure...
         */
        recv_ret = recvfrom(socket_data.sd, &message_header, sizeof(itti_socket_header_t), MSG_WAITALL, NULL, 0);
        if (recv_ret == -1) {
            /* Failure case */
            fprintf(stderr, "recvfrom failed\n");
            ui_interface.ui_notification_dialog(
                DIALOG_ERROR, "Unexpected error while reading from socket\n%d:%s",
                errno, strerror(errno));
            ui_interface.ui_enable_connect_button();
            pthread_exit(NULL);
        } else if (recv_ret == 0) {
            /* We lost the connection with other peer or shutdown asked */
            ui_interface.ui_notification_dialog(
                DIALOG_ERROR, "Connection with remote host has been lost");
            ui_interface.ui_enable_connect_button();
            pthread_exit(NULL);
        }

        switch(message_header.message_type) {
            case 1: {
                itti_dump_message_t itti_dump_header;

                memset(&itti_dump_header, 0, sizeof(itti_dump_message_t));

                /* second run: acquire the sub-header for itti dump message type */
                recv_ret = recvfrom(socket_data.sd, &itti_dump_header, sizeof(itti_dump_message_t), MSG_WAITALL, NULL, 0);
                if (recv_ret == -1) {
                    /* Failure case */
                    fprintf(stderr, "recvfrom failed\n");
                    ui_interface.ui_notification_dialog(
                        DIALOG_ERROR, "Unexpected error while reading from socket\n%d:%s",
                        errno, strerror(errno));
                    ui_interface.ui_enable_connect_button();
                    pthread_exit(NULL);
                } else if (recv_ret == 0) {
                    /* We lost the connection with other peer or shutdown asked */
                    ui_interface.ui_notification_dialog(
                        DIALOG_ERROR, "Connection with remote host has been lost");
                    ui_interface.ui_enable_connect_button();
                    pthread_exit(NULL);
                }

                data_length = message_header.message_size - sizeof(itti_socket_header_t) - sizeof(itti_dump_message_t);
                data = malloc(sizeof(uint8_t) * data_length);

                /* third run: acquire the MessageDef part */
                recv_ret = recvfrom(socket_data.sd, data, data_length, MSG_WAITALL, NULL, 0);
                if (recv_ret == -1) {
                    /* Failure case */
                    fprintf(stderr, "recvfrom failed\n");
                    ui_interface.ui_notification_dialog(
                        DIALOG_ERROR, "Unexpected error while reading from socket\n%d:%s",
                        errno, strerror(errno));
                    ui_interface.ui_enable_connect_button();
                    pthread_exit(NULL);
                } else if (recv_ret == 0) {
                    /* We lost the connection with other peer or shutdown asked */
                    ui_interface.ui_notification_dialog(
                        DIALOG_ERROR, "Connection with remote host has been lost");
                    ui_interface.ui_enable_connect_button();
                    pthread_exit(NULL);
                }

                /* Create the new buffer */
                if (buffer_new_from_data(&buffer, data, data_length, 1) != RC_OK) {
                    ui_interface.ui_notification_dialog(
                        DIALOG_ERROR, "Cannot connect to %s with port %u",
                        socket_arg->ip_address, socket_arg->port);
                    ui_interface.ui_enable_connect_button();
                    pthread_exit(NULL);
                }

                buffer->message_number = itti_dump_header.message_number;
                buffer_dump(buffer, stdout);

                CHECK_FCT_DO(buffer_add_to_list(buffer), pthread_exit(NULL));

                ui_interface.ui_tree_view_new_signal_ind(itti_dump_header.message_number,
                                                         itti_dump_header.signal_name);
            } break;
            case 2:
                /* The received message is a statistic signal */
                break;
            case 3: {
                char *xml_definition;
                uint32_t xml_definition_length;

                xml_definition_length = message_header.message_size - sizeof(message_header);

                xml_definition = malloc(xml_definition_length);

                recv_ret = recvfrom(socket_data.sd, xml_definition, xml_definition_length, MSG_WAITALL, NULL, 0);
                if (recv_ret == -1) {
                    /* Failure case */
                    fprintf(stderr, "recvfrom failed\n");
                    ui_interface.ui_notification_dialog(
                        DIALOG_ERROR, "Unexpected error while reading from socket\n%d:%s",
                        errno, strerror(errno));
                    ui_interface.ui_enable_connect_button();
                    pthread_exit(NULL);
                } else if (recv_ret == 0) {
                    /* We lost the connection with other peer or shutdown asked */
                    ui_interface.ui_notification_dialog(
                        DIALOG_ERROR, "Connection with remote host has been lost");
                    ui_interface.ui_enable_connect_button();
                    pthread_exit(NULL);
                }

                fprintf(stdout, "Received XML definition of length %u\n",
                        xml_definition_length);

                xml_parse_buffer(xml_definition, xml_definition_length);
            } break;
            default:
                ui_interface.ui_notification_dialog(
                    DIALOG_ERROR, "Received unknown message type\n");
                break;
        }
    }

    return NULL;

fail:
    ui_interface.ui_notification_dialog(
        DIALOG_ERROR, "Cannot connect to %s with port %u",
        socket_arg->ip_address, socket_arg->port);
    ui_interface.ui_enable_connect_button();
    pthread_exit(NULL);
}

int socket_disconnect_from_remote_host(void)
{
    void *ret_val;

    printf("Closing socket %d\n", socket_data.sd);

    /* Cancel the thread */
    pthread_cancel(socket_data.thread);
    pthread_join(socket_data.thread, &ret_val);

    /* Send shutdown to remote host */
    CHECK_FCT_POSIX(shutdown(socket_data.sd, SHUT_RDWR));
    CHECK_FCT_POSIX(close(socket_data.sd));

    socket_data.sd = -1;
    ui_interface.ui_enable_connect_button();

    return RC_OK;
}

int socket_connect_to_remote_host(const char *remote_ip, const uint16_t port)
{
    socket_arg_t *socket_arg;

    socket_arg = malloc(sizeof(socket_arg_t));

    ui_interface.ui_disable_connect_button();

    memcpy(socket_arg->ip_address, remote_ip, strlen(remote_ip) + 1);
    socket_arg->port = port;

    if (pthread_create(&socket_data.thread, NULL, socket_read_data, socket_arg) != 0) {
        fprintf(stderr, "pthread_create failed\n");
        ui_interface.ui_enable_connect_button();
        return RC_FAIL;
    }

    return RC_OK;
}
