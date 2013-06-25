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
#include <pthread.h>
#include <assert.h>

#include "queue.h"

// Locally define macro to enable task id to string table
#define INTERTASK_C

#include "intertask_interface.h"
#include "intertask_interface_dump.h"

static int itti_debug = 0;

#define ITTI_DEBUG(x, args...) do { if (itti_debug) fprintf(stdout, "[ITTI][D]"x, ##args); } \
    while(0)
#define ITTI_ERROR(x, args...) do { fprintf(stdout, "[ITTI][E]"x, ##args); } \
    while(0)

/* This list acts as a FIFO of messages received by tasks (RRC, NAS, ...) */
struct message_list_s {
    STAILQ_ENTRY(message_list_s) next_element;

    MessageDef *msg;          ///< Pointer to the message

    uint32_t message_number;          ///< Unique message number
    uint32_t message_priority;        ///< Message priority
};

/* Message queues: declare a queue for each task */
static STAILQ_HEAD(message_queue_head, message_list_s) message_queue[TASK_MAX];
/* Number of messages in the queue */
static uint32_t message_in_queue[TASK_MAX];
/* Mutex for the message queue */
static pthread_mutex_t message_queue_mutex[TASK_MAX];
/* Conditinal Var for message queue and task synchro */
static pthread_cond_t  mssage_queue_cond_var[TASK_MAX];

/* Current message number. Incremented every call to send_msg_to_task */
static uint32_t message_number = 0;

/* Map message priority to message id */
const struct message_priority_s messages_priorities[MESSAGES_ID_MAX] = {
    #define MESSAGE_DEF(iD, pRIO, sTRUCT) { iD, pRIO },

    #include "gtpv1_u_messages_def.h"
    #include "sctp_messages_def.h"
    #include "s1ap_messages_def.h"
    #include "s6a_messages_def.h"
    #include "timer_messages_def.h"
    #include "udp_messages_def.h"

    #undef MESSAGE_DEF
};

static inline uint32_t get_message_priority(MessagesIds message_id) {
    uint32_t i;

    for (i = 0; i < MESSAGES_ID_MAX; i++) {
        if ((messages_priorities[i]).id == message_id)
            return ((messages_priorities[i]).priority);
    }
    return 0;
}

int send_broadcast_message(MessageDef *message_p) {
    uint32_t i;
    int ret;

    for (i = TASK_FIRST; i < TASK_MAX; i++) {
        ret = send_msg_to_task(i, message_p);
        if (ret < 0) {
            ITTI_ERROR("Failed to send broadcast message to task (%u:%s)\n",
                       i, tasks_string[i]);
        }
    }

    return ret;
}

inline MessageDef *alloc_new_message(
    TaskId      origin_task_id,
    TaskId      destination_task_id,
    MessagesIds message_id) {

    MessageDef *temp = NULL;

    if ((origin_task_id >= TASK_MAX) || (destination_task_id >= TASK_MAX) ||
        (message_id >= MESSAGES_ID_MAX))
        return NULL;

    temp = (MessageDef *)malloc(sizeof(MessageDef));
    if (temp == NULL)
        return temp;

    temp->messageId = message_id;
    temp->originTaskId = origin_task_id;
    temp->destinationTaskId = destination_task_id;

    return temp;
}

int send_msg_to_task(TaskId task_id, MessageDef *message)
{
    struct message_list_s *new;
    uint32_t priority;

    assert(message != NULL);
    assert(task_id < TASK_MAX);

    priority = get_message_priority(message->messageId);

    // Lock the mutex to get exclusive access to the list
    pthread_mutex_lock(&message_queue_mutex[task_id]);
    // Allocate new list element
    new = (struct message_list_s *)malloc(sizeof(struct message_list_s));

    //Increment message number
    message_number++;

    // Fill in members
    new->msg              = message;
    new->message_number   = message_number;
    new->message_priority = priority;

    if (STAILQ_EMPTY(&message_queue[task_id])) {
        STAILQ_INSERT_HEAD(&message_queue[task_id], new, next_element);
    } else {
        struct message_list_s *insert_after = NULL;
        struct message_list_s *temp;

        STAILQ_FOREACH(temp, &message_queue[task_id], next_element) {
            struct message_list_s *next;
            next = STAILQ_NEXT(temp, next_element);
            /* Increment message priority to create a sort of
             * priority based scheduler */
            if (temp->message_priority < TASK_PRIORITY_MAX)
                temp->message_priority++;
            if (next && next->message_priority < priority) {
                insert_after = temp;
            }
        }
        if (insert_after == NULL) {
            STAILQ_INSERT_TAIL(&message_queue[task_id], new, next_element);
        } else {
            STAILQ_INSERT_AFTER(&message_queue[task_id], insert_after, new, next_element);
        }
    }

    // Update the number of messages in the queue
    message_in_queue[task_id]++;
    if (message_in_queue[task_id] == 1) {
        // Emit a signal to wake up target task thread
        pthread_cond_signal(&mssage_queue_cond_var[task_id]);
    }
    // Release the mutex
    pthread_mutex_unlock(&message_queue_mutex[task_id]);
    ITTI_DEBUG("Message %d with priority %d succesfully sent to task_id (%u:%s)\n",
               message_number, priority, task_id, tasks_string[task_id]);
    return 0;
}

void receive_msg(TaskId task_id, MessageDef **received_msg)
{
    assert(task_id < TASK_MAX);
    assert(received_msg != NULL);

    // Lock the mutex to get exclusive access to the list
    pthread_mutex_lock(&message_queue_mutex[task_id]);

    if (message_in_queue[task_id] == 0) {
        ITTI_DEBUG("Message in queue[(%u:%s)] == 0, waiting\n",
                   task_id, tasks_string[task_id]);
        // Wait while list == 0
        pthread_cond_wait(&mssage_queue_cond_var[task_id], &message_queue_mutex[task_id]);
        ITTI_DEBUG("Receiver thread queue[(%u:%s)] got new message notification\n",
                   task_id, tasks_string[task_id]);
    }

    if (!STAILQ_EMPTY(&message_queue[task_id])) {
        struct message_list_s *temp = STAILQ_FIRST(&message_queue[task_id]);

        // Update received_msg reference
        *received_msg = temp->msg;

        /* Remove message from queue */
        STAILQ_REMOVE_HEAD(&message_queue[task_id], next_element);
        free(temp);
        message_in_queue[task_id]--;
    }
    // Release the mutex
    pthread_mutex_unlock(&message_queue_mutex[task_id]);
}

void intertask_interface_init(const mme_config_t *mme_config)
{
    int i;
    /* Initializing each queue and related stuff */
    for (i = 0; i < TASK_MAX; i++) {
        STAILQ_INIT(&message_queue[i]);
        message_in_queue[i] = 0;
        // Initialize mutexes
        pthread_mutex_init(&message_queue_mutex[i], NULL);
        // Initialize Cond vars
        pthread_cond_init(&mssage_queue_cond_var[i], NULL);
    }
//     itti_init();
}
