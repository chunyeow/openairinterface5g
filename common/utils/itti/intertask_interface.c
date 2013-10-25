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

#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#include "queue.h"
#include "assertions.h"

#include "intertask_interface.h"
#include "intertask_interface_dump.h"
/* Includes "intertask_interface_init.h" to check prototype coherence, but disable threads and messages information generation */
#define CHECK_PROTOTYPE_ONLY
#include "intertask_interface_init.h"
#undef CHECK_PROTOTYPE_ONLY

#include "signals.h"
#include "timer.h"

int itti_debug = 0;

#define ITTI_DEBUG(x, args...) do { if (itti_debug) fprintf(stdout, "[ITTI][D]"x, ##args); } \
    while(0)
#define ITTI_ERROR(x, args...) do { fprintf(stdout, "[ITTI][E]"x, ##args); } \
    while(0)

/* Global message size */
#define MESSAGE_SIZE(mESSAGEiD) (sizeof(MessageHeader) + itti_desc.messages_info[mESSAGEiD].size)

typedef enum task_state_s {
    TASK_STATE_NOT_CONFIGURED, TASK_STATE_STARTING, TASK_STATE_READY, TASK_STATE_ENDED, TASK_STATE_MAX,
} task_state_t;

/* This list acts as a FIFO of messages received by tasks (RRC, NAS, ...) */
struct message_list_s {
    STAILQ_ENTRY(message_list_s) next_element;

    MessageDef *msg; ///< Pointer to the message

    message_number_t message_number; ///< Unique message number
    uint32_t message_priority; ///< Message priority
};

typedef struct task_desc_s {
    /* Queue of messages belonging to the task */
    STAILQ_HEAD(message_queue_head, message_list_s)
    message_queue;

    /* Number of messages in the queue */
    volatile uint32_t message_in_queue;
    /* Mutex for the message queue */
    pthread_mutex_t message_queue_mutex;
    /* Conditional var for message queue and task synchro */
    pthread_cond_t message_queue_cond_var;
    pthread_t task_thread;
    volatile task_state_t task_state;
} task_desc_t;

struct itti_desc_s {
    task_desc_t *tasks;
    /* Current message number. Incremented every call to send_msg_to_task */
    message_number_t message_number __attribute__((aligned(8)));

    thread_id_t thread_max;
    MessagesIds messages_id_max;

    pthread_t thread_handling_signals;

    const char * const *threads_name;
    const message_info_t *messages_info;
};

static struct itti_desc_s itti_desc;

static inline message_number_t itti_increment_message_number(void) {
    /* Atomic operation supported by GCC: returns the current message number
     * and then increment it by 1.
     * This can be done without mutex.
     */
    return __sync_fetch_and_add (&itti_desc.message_number, 1);
}

static inline uint32_t itti_get_message_priority(MessagesIds message_id) {
    DevCheck(message_id < itti_desc.messages_id_max, message_id, itti_desc.messages_id_max, 0);

    return (itti_desc.messages_info[message_id].priority);
}

char *itti_get_message_name(MessagesIds message_id) {
    DevCheck(message_id < itti_desc.messages_id_max, message_id, itti_desc.messages_id_max, 0);

    return (itti_desc.messages_info[message_id].name);
}

int itti_send_broadcast_message(MessageDef *message_p) {
    thread_id_t origin_thread_id;
    uint32_t i;
    int ret = 0;
    int result;

    DevAssert(message_p != NULL);

    origin_thread_id = TASK_GET_THREAD_ID(message_p->header.originTaskId);

    for (i = THREAD_FIRST; i < itti_desc.thread_max; i++) {
        MessageDef *new_message_p;

        /* Skip task that broadcast the message */
        if (i != origin_thread_id) {
            /* Skip tasks which are not running */
            if (itti_desc.tasks[i].task_state == TASK_STATE_READY) {
                new_message_p = malloc (sizeof(MessageDef));
                DevAssert(message_p != NULL);

                memcpy (new_message_p, message_p, sizeof(MessageDef));
                result = itti_send_msg_to_task (TASK_SHIFT_THREAD_ID(i), INSTANCE_DEFAULT, new_message_p);
                DevCheck(result >= 0, message_p->header.messageId, i, 0);
            }
        }
    }
    free (message_p);

    return ret;
}

inline MessageDef *itti_alloc_new_message(task_id_t origin_task_id, MessagesIds message_id) {
    MessageDef *temp = NULL;

    DevCheck(message_id < itti_desc.messages_id_max, message_id, itti_desc.messages_id_max, 0);

    temp = calloc (1, MESSAGE_SIZE(message_id));
    DevAssert(temp != NULL);

    temp->header.messageId = message_id;
    temp->header.originTaskId = origin_task_id;
    temp->header.size = itti_desc.messages_info[message_id].size;

    return temp;
}

int itti_send_msg_to_task(task_id_t task_id, instance_t instance, MessageDef *message) {
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);
    struct message_list_s *new;
    uint32_t priority;
    message_number_t message_number;
    uint32_t message_id;

    DevAssert(message != NULL);
    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);

    message->header.destinationTaskId = task_id;
    message->header.instance = instance;
    message_id = message->header.messageId;
    DevCheck(message_id < itti_desc.messages_id_max, itti_desc.messages_id_max, message_id, 0);

    priority = itti_get_message_priority (message_id);

    /* Lock the mutex to get exclusive access to the list */
    pthread_mutex_lock (&itti_desc.tasks[thread_id].message_queue_mutex);

    /* We cannot send a message if the task is not running */
    DevCheck(itti_desc.tasks[thread_id].task_state == TASK_STATE_READY, itti_desc.tasks[thread_id].task_state,
             TASK_STATE_READY, thread_id);

    /* Check the number of messages in the queue */
    DevCheck((itti_desc.tasks[thread_id].message_in_queue * sizeof(MessageDef)) < ITTI_QUEUE_SIZE_PER_TASK,
             (itti_desc.tasks[thread_id].message_in_queue * sizeof(MessageDef)), ITTI_QUEUE_SIZE_PER_TASK,
             itti_desc.tasks[thread_id].message_in_queue);

    /* Allocate new list element */
    new = (struct message_list_s *) malloc (sizeof(struct message_list_s));
    DevAssert(new != NULL);

    /* Increment the global message number */
    message_number = itti_increment_message_number ();

    /* Fill in members */
    new->msg = message;
    new->message_number = message_number;
    new->message_priority = priority;

    itti_dump_queue_message (message_number, message, itti_desc.messages_info[message_id].name,
                             MESSAGE_SIZE(message_id));

    if (STAILQ_EMPTY (&itti_desc.tasks[thread_id].message_queue)) {
        STAILQ_INSERT_HEAD (&itti_desc.tasks[thread_id].message_queue, new, next_element);
    }
    else {
//         struct message_list_s *insert_after = NULL;
//         struct message_list_s *temp;
// 
//         /* This method is inefficient... */
//         STAILQ_FOREACH(temp, &itti_desc.tasks[thread_id].message_queue, next_element) {
//             struct message_list_s *next;
//             next = STAILQ_NEXT(temp, next_element);
//             /* Increment message priority to create a sort of
//              * priority based scheduler */
// //             if (temp->message_priority < TASK_PRIORITY_MAX) {
// //                 temp->message_priority++;
// //             }
//             if (next && next->message_priority < priority) {
//                 insert_after = temp;
//                 break;
//             }
//         }
//         if (insert_after == NULL) {
        STAILQ_INSERT_TAIL (&itti_desc.tasks[thread_id].message_queue, new, next_element);
//         } else {
//             STAILQ_INSERT_AFTER(&itti_desc.tasks[thread_id].message_queue, insert_after, new,
//                                 next_element);
//         }
    }

    /* Update the number of messages in the queue */
    itti_desc.tasks[thread_id].message_in_queue++;
    if (itti_desc.tasks[thread_id].message_in_queue == 1) {
        /* Emit a signal to wake up target task thread */
        pthread_cond_signal (&itti_desc.tasks[thread_id].message_queue_cond_var);
    }
    /* Release the mutex */
    pthread_mutex_unlock (&itti_desc.tasks[thread_id].message_queue_mutex);
    ITTI_DEBUG(
            "Message %s, number %lu with priority %d successfully sent to queue (%u:%s)\n",
            itti_desc.messages_info[message_id].name, message_number, priority, thread_id, itti_desc.threads_name[thread_id]);
    return 0;
}

void itti_receive_msg(task_id_t task_id, MessageDef **received_msg) {
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);

    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);
    DevAssert(received_msg != NULL);

    // Lock the mutex to get exclusive access to the list
    pthread_mutex_lock (&itti_desc.tasks[thread_id].message_queue_mutex);

    if (itti_desc.tasks[thread_id].message_in_queue == 0) {
        ITTI_DEBUG("Message in queue[(%u:%s)] == 0, waiting\n", thread_id, itti_desc.threads_name[thread_id]);
        // Wait while list == 0
        pthread_cond_wait (&itti_desc.tasks[thread_id].message_queue_cond_var,
                           &itti_desc.tasks[thread_id].message_queue_mutex);
        ITTI_DEBUG("Receiver queue[(%u:%s)] got new message notification for task %x\n",
                   thread_id, itti_desc.threads_name[thread_id], task_id);
    }

    if (!STAILQ_EMPTY (&itti_desc.tasks[thread_id].message_queue)) {
        struct message_list_s *temp = STAILQ_FIRST (&itti_desc.tasks[thread_id].message_queue);

        /* Update received_msg reference */
        *received_msg = temp->msg;

        /* Remove message from queue */
        STAILQ_REMOVE_HEAD (&itti_desc.tasks[thread_id].message_queue, next_element);
        free (temp);
        itti_desc.tasks[thread_id].message_in_queue--;
    }
    // Release the mutex
    pthread_mutex_unlock (&itti_desc.tasks[thread_id].message_queue_mutex);
}

void itti_poll_msg(task_id_t task_id, instance_t instance, MessageDef **received_msg) {
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);

    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);
    DevAssert(received_msg != NULL);

    *received_msg = NULL;

    if (itti_desc.tasks[thread_id].message_in_queue != 0) {
        struct message_list_s *temp;

        // Lock the mutex to get exclusive access to the list
        pthread_mutex_lock (&itti_desc.tasks[thread_id].message_queue_mutex);

        STAILQ_FOREACH (temp, &itti_desc.tasks[thread_id].message_queue, next_element)
        {
            if ((temp->msg->header.destinationTaskId == task_id)
                    && ((instance == INSTANCE_ALL) || (temp->msg->header.instance == instance))) {
                /* Update received_msg reference */
                *received_msg = temp->msg;

                /* Remove message from queue */
                STAILQ_REMOVE (&itti_desc.tasks[thread_id].message_queue, temp, message_list_s, next_element);
                free (temp);
                itti_desc.tasks[thread_id].message_in_queue--;

                ITTI_DEBUG(
                        "Receiver queue[(%u:%s)] got new message %s, number %lu for task %x\n",
                        thread_id, itti_desc.threads_name[thread_id], itti_desc.messages_info[temp->msg->header.messageId].name, temp->message_number, task_id);
                break;
            }
        }

        // Release the mutex
        pthread_mutex_unlock (&itti_desc.tasks[thread_id].message_queue_mutex);
    }

    if (*received_msg == NULL) {
        ITTI_DEBUG("No message in queue[(%u:%s)] for task %x\n", thread_id, itti_desc.threads_name[thread_id], task_id);
    }
}

int itti_create_task(task_id_t task_id, void *(*start_routine)(void *), void *args_p) {
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);
    int result;

    DevAssert(start_routine != NULL);
    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);
    DevCheck(itti_desc.tasks[thread_id].task_state == TASK_STATE_NOT_CONFIGURED, task_id, thread_id,
             itti_desc.tasks[thread_id].task_state);

    itti_desc.tasks[thread_id].task_state = TASK_STATE_STARTING;

    result = pthread_create (&itti_desc.tasks[thread_id].task_thread, NULL, start_routine, args_p);
    DevCheck(result>= 0, task_id, thread_id, result);

    /* Wait till the thread is completely ready */
    while (itti_desc.tasks[thread_id].task_state != TASK_STATE_READY)
        ;
    return 0;
}

void itti_mark_task_ready(task_id_t task_id) {
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);

    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);

    // Lock the mutex to get exclusive access to the list
    pthread_mutex_lock (&itti_desc.tasks[thread_id].message_queue_mutex);
    itti_desc.tasks[thread_id].task_state = TASK_STATE_READY;

    // Release the mutex
    pthread_mutex_unlock (&itti_desc.tasks[thread_id].message_queue_mutex);
}

void itti_exit_task(void) {
    pthread_exit (NULL);
}

void itti_terminate_tasks(task_id_t task_id) {
// Sends Terminate signals to all tasks.
    itti_send_terminate_message (task_id);

    if (itti_desc.thread_handling_signals >= 0) {
        pthread_kill (itti_desc.thread_handling_signals, SIGUSR1);
    }

    pthread_exit (NULL);
}

int itti_init(thread_id_t thread_max, MessagesIds messages_id_max, const char * const *threads_name,
              const message_info_t *messages_info, const char * const messages_definition_xml) {
    int i;
    itti_desc.message_number = 0;

    ITTI_DEBUG( "Init: %d threads, %d messages\n", thread_max, messages_id_max);

    CHECK_INIT_RETURN(signal_init());

    /* Saves threads and messages max values */
    itti_desc.thread_max = thread_max;
    itti_desc.messages_id_max = messages_id_max;
    itti_desc.thread_handling_signals = -1;
    itti_desc.threads_name = threads_name;
    itti_desc.messages_info = messages_info;

    /* Allocates memory for tasks info */
    itti_desc.tasks = calloc (itti_desc.thread_max, sizeof(task_desc_t));

    /* Initializing each queue and related stuff */
    for (i = THREAD_FIRST; i < itti_desc.thread_max; i++) {
        STAILQ_INIT (&itti_desc.tasks[i].message_queue);
        itti_desc.tasks[i].message_in_queue = 0;

        // Initialize mutexes
        pthread_mutex_init (&itti_desc.tasks[i].message_queue_mutex, NULL);

        // Initialize Cond vars
        pthread_cond_init (&itti_desc.tasks[i].message_queue_cond_var, NULL);
        itti_desc.tasks[i].task_state = TASK_STATE_NOT_CONFIGURED;
    }
    itti_dump_init (messages_definition_xml);

    CHECK_INIT_RETURN(timer_init ());

    return 0;
}

void itti_wait_tasks_end(void) {
    int end = 0;
    int i;
    int ready_tasks;
    int result;
    int retries = 10;

    itti_desc.thread_handling_signals = pthread_self ();

    /* Handle signals here */
    while (end == 0) {
        signal_handle (&end);
    }

    do {
        ready_tasks = 0;

        for (i = THREAD_FIRST; i < itti_desc.thread_max; i++) {
            /* Skip tasks which are not running */
            if (itti_desc.tasks[i].task_state == TASK_STATE_READY) {

                result = pthread_tryjoin_np (itti_desc.tasks[i].task_thread, NULL);

                ITTI_DEBUG("Thread %s join status %d\n", itti_desc.threads_name[i], result);

                if (result == 0) {
                    /* Thread has terminated */
                    itti_desc.tasks[i].task_state = TASK_STATE_ENDED;
                }
                else {
                    /* Thread is still running, count it */
                    ready_tasks++;
                }
            }
        }
        if (ready_tasks > 0) {
            usleep (100 * 1000);
        }
    } while ((ready_tasks > 0) && (retries--));

    if (ready_tasks > 0) {
        ITTI_DEBUG("Some threads are still running, force exit\n");
        exit (0);
    }
}

void itti_send_terminate_message(task_id_t task_id) {
    MessageDef *terminate_message_p;

    terminate_message_p = itti_alloc_new_message (task_id, TERMINATE_MESSAGE);

    itti_send_broadcast_message (terminate_message_p);
}
