#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <assert.h>

#include <sys/socket.h>       /*  socket definitions        */
#include <sys/types.h>        /*  socket types              */
#include <arpa/inet.h>        /*  inet (3) funtions         */

#include <pthread.h>

#include "intertask_interface.h"
#include "intertask_interface_dump.h"

#include "queue.h"

struct itti_queue_s {
    struct itti_queue_s *next;
    char                *data;
    uint32_t             length;
};

struct itti_queue_ref {
    pthread_mutex_t       queue_mutex;
    struct itti_queue_s  *itti_queue_head;
    struct itti_queue_s  *itti_queue_tail;
    uint32_t              queue_size;
};

struct itti_queue_ref itti_queue = { PTHREAD_MUTEX_INITIALIZER, NULL, NULL, 0 };
int itti_accept_socket = -1;
pthread_t itti_thread;

int itti_queue_message(MessageDef *message_p) {
    return 0;
}

static void * itti_create_socket(void *arg_p) {
    int itti_conn_socket;
    struct sockaddr_in servaddr; /*  socket address structure  */

    if ((itti_conn_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket");
        return NULL;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(ITTI_PORT);

    if (bind(itti_conn_socket, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0 ) {
        perror("bind");
        return NULL;
    }
    if (listen(itti_conn_socket, 5) < 0 ) {
        perror("listen");
        return NULL;
    }

    /* Wait for a connection */
    if ((itti_accept_socket = accept(itti_conn_socket, NULL, NULL)) < 0) {
        perror("accept");
        return NULL;
    }
    while(1) {
        pthread_mutex_lock(&itti_queue.queue_mutex);
        if (itti_queue.queue_size > 0) {
            /* There is some messages to send */
            assert(itti_queue.itti_queue_tail != NULL);
            
        }
        pthread_mutex_unlock(&itti_queue.queue_mutex);
        sleep(1);
    }
    return NULL;
}

void itti_init(void) {

    if (pthread_create(&itti_thread, NULL, &itti_create_socket, NULL) < 0) {
        perror("pthread_create");
    }
}
