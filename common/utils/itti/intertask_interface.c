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

#include "assertions.h"

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include "liblfds611.h"

#include "intertask_interface.h"
#include "intertask_interface_dump.h"

#ifdef RTAI
# include <rtai_shm.h>
#endif

#if defined(OAI_EMU) || defined(RTAI)
# include "vcd_signal_dumper.h"
#endif

/* Includes "intertask_interface_init.h" to check prototype coherence, but
 * disable threads and messages information generation.
 */
#define CHECK_PROTOTYPE_ONLY
#include "intertask_interface_init.h"
#undef CHECK_PROTOTYPE_ONLY

#include "signals.h"
#include "timer.h"

const int itti_debug = 0;
const int itti_debug_poll = 0;

/* Don't flush if using RTAI */
#ifdef RTAI
# define ITTI_DEBUG(x, args...) do { if (itti_debug) rt_printk("[ITTI][D]"x, ##args); } \
    while(0)
# define ITTI_ERROR(x, args...) do { rt_printk("[ITTI][E]"x, ##args); } \
    while(0)
#else
# define ITTI_DEBUG(x, args...) do { if (itti_debug) fprintf(stdout, "[ITTI][D]"x, ##args); fflush (stdout); } \
    while(0)
# define ITTI_ERROR(x, args...) do { fprintf(stdout, "[ITTI][E]"x, ##args); fflush (stdout); } \
    while(0)
#endif

/* Global message size */
#define MESSAGE_SIZE(mESSAGEiD) (sizeof(MessageHeader) + itti_desc.messages_info[mESSAGEiD].size)

#ifndef EFD_SEMAPHORE
# define KERNEL_VERSION_PRE_2_6_30 1
#endif

#ifdef RTAI
# define ITTI_MEM_PAGE_SIZE (1024)
# define ITTI_MEM_SIZE      (16 * 1024 * 1024)
#endif

typedef enum task_state_s {
    TASK_STATE_NOT_CONFIGURED, TASK_STATE_STARTING, TASK_STATE_READY, TASK_STATE_ENDED, TASK_STATE_MAX,
} task_state_t;

/* This list acts as a FIFO of messages received by tasks (RRC, NAS, ...) */
typedef struct message_list_s {
    MessageDef *msg; ///< Pointer to the message

    message_number_t message_number; ///< Unique message number
    uint32_t message_priority; ///< Message priority
} message_list_t;

typedef struct thread_desc_s {
    /* pthread associated with the thread */
    pthread_t task_thread;

    /* State of the thread */
    volatile task_state_t task_state;

    /* This fd is used internally by ITTI. */
    int epoll_fd;

    /* The thread fd */
    int task_event_fd;

    /* Number of events to monitor */
    uint16_t nb_events;

#if defined(KERNEL_VERSION_PRE_2_6_30)
    eventfd_t sem_counter;
#endif

    /* Array of events monitored by the task.
     * By default only one fd is monitored (the one used to received messages
     * from other tasks).
     * More events can be suscribed later by the task itself.
     */
    struct epoll_event *events;

    int epoll_nb_events;

#ifdef RTAI
    /* Flag to mark real time thread */
    unsigned real_time;

    /* Counter to indicate from RTAI threads that messages are pending for the thread */
    unsigned messages_pending;
#endif
} thread_desc_t;

typedef struct task_desc_s {
    /* Queue of messages belonging to the task */
    struct lfds611_queue_state *message_queue;
} task_desc_t;

typedef struct itti_desc_s {
    thread_desc_t *threads;
    task_desc_t   *tasks;

    /* Current message number. Incremented every call to send_msg_to_task */
    message_number_t message_number __attribute__((aligned(8)));

    thread_id_t thread_max;
    task_id_t task_max;
    MessagesIds messages_id_max;

    pthread_t thread_handling_signals;

    const task_info_t *tasks_info;
    const message_info_t *messages_info;

    itti_lte_time_t lte_time;

    int running;
#ifdef RTAI
    pthread_t rt_relay_thread;
#endif

#if defined(OAI_EMU) || defined(RTAI)
    uint64_t vcd_poll_msg;
    uint64_t vcd_receive_msg;
    uint64_t vcd_send_msg;
#endif
} itti_desc_t;

static itti_desc_t itti_desc;

void *itti_malloc(task_id_t task_id, ssize_t size)
{
    void *ptr = NULL;

#ifdef RTAI
//     ptr = rt_malloc(size);
    ptr = malloc(size);
#else
    ptr = malloc(size);
#endif

    DevCheck(ptr != NULL, size, task_id, 0);

    return ptr;
}

void itti_free(task_id_t task_id, void *ptr)
{
    DevAssert(ptr != NULL);
#ifdef RTAI
    free(ptr);
#else
    free(ptr);
#endif
}

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

const char *itti_get_message_name(MessagesIds message_id) {
    DevCheck(message_id < itti_desc.messages_id_max, message_id, itti_desc.messages_id_max, 0);

    return (itti_desc.messages_info[message_id].name);
}

const char *itti_get_task_name(task_id_t task_id)
{
    DevCheck(task_id < itti_desc.task_max, task_id, itti_desc.task_max, 0);

    return (itti_desc.tasks_info[task_id].name);
}

static task_id_t itti_get_current_task_id(void)
{
    task_id_t task_id;
    thread_id_t thread_id;
    pthread_t thread = pthread_self ();

    for (task_id = TASK_FIRST; task_id < itti_desc.task_max; task_id++)
    {
        thread_id = TASK_GET_THREAD_ID(task_id);
        if (itti_desc.threads[thread_id].task_thread == thread)
        {
            return task_id;
        }
    }

    return TASK_UNKNOWN;
}

void itti_update_lte_time(uint32_t frame, uint8_t slot)
{
    itti_desc.lte_time.frame = frame;
    itti_desc.lte_time.slot = slot;
}

int itti_send_broadcast_message(MessageDef *message_p) {
    task_id_t destination_task_id;
    task_id_t origin_task_id;
    thread_id_t origin_thread_id;
    uint32_t thread_id;
    int ret = 0;
    int result;

    DevAssert(message_p != NULL);

    origin_task_id = message_p->ittiMsgHeader.originTaskId;
    origin_thread_id = TASK_GET_THREAD_ID(origin_task_id);

    destination_task_id = TASK_FIRST;
    for (thread_id = THREAD_FIRST; thread_id < itti_desc.thread_max; thread_id++) {
        MessageDef *new_message_p;

        while (thread_id != TASK_GET_THREAD_ID(destination_task_id))
        {
            destination_task_id++;
        }
        /* Skip task that broadcast the message */
        if (thread_id != origin_thread_id) {
            /* Skip tasks which are not running */
            if (itti_desc.threads[thread_id].task_state == TASK_STATE_READY) {
                new_message_p = itti_malloc (origin_task_id, sizeof(MessageDef));
                DevAssert(message_p != NULL);

                memcpy (new_message_p, message_p, sizeof(MessageDef));
                result = itti_send_msg_to_task (destination_task_id, INSTANCE_DEFAULT, new_message_p);
                DevCheck(result >= 0, message_p->ittiMsgHeader.messageId, thread_id, destination_task_id);
            }
        }
    }
    free (message_p);

    return ret;
}

inline MessageDef *itti_alloc_new_message_sized(task_id_t origin_task_id, MessagesIds message_id, MessageHeaderSize size)
{
    MessageDef *temp = NULL;

    DevCheck(message_id < itti_desc.messages_id_max, message_id, itti_desc.messages_id_max, 0);

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_ALLOC_MSG, size);
#endif

    if (origin_task_id == TASK_UNKNOWN)
    {
        /* Try to identify real origin task ID */
        origin_task_id = itti_get_current_task_id();
    }

    temp = itti_malloc (origin_task_id, sizeof(MessageHeader) + size);
    DevAssert(temp != NULL);

    temp->ittiMsgHeader.messageId = message_id;
    temp->ittiMsgHeader.originTaskId = origin_task_id;
    temp->ittiMsgHeader.ittiMsgSize = size;

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_ALLOC_MSG, 0);
#endif

    return temp;
}

inline MessageDef *itti_alloc_new_message(task_id_t origin_task_id, MessagesIds message_id)
{
    return itti_alloc_new_message_sized(origin_task_id, message_id, itti_desc.messages_info[message_id].size);
}

int itti_send_msg_to_task(task_id_t destination_task_id, instance_t instance, MessageDef *message)
{
    thread_id_t destination_thread_id;
    thread_id_t origin_task_id;
    message_list_t *new;
    uint32_t priority;
    message_number_t message_number;
    uint32_t message_id;

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_SEND_MSG,
                                            __sync_or_and_fetch (&itti_desc.vcd_send_msg, 1L << destination_task_id));
#endif

    DevAssert(message != NULL);
    DevCheck(destination_task_id < itti_desc.task_max, destination_task_id, itti_desc.task_max, 0);

    destination_thread_id = TASK_GET_THREAD_ID(destination_task_id);
    message->ittiMsgHeader.destinationTaskId = destination_task_id;
    message->ittiMsgHeader.instance = instance;
    message->ittiMsgHeader.lte_time.frame = itti_desc.lte_time.frame;
    message->ittiMsgHeader.lte_time.slot = itti_desc.lte_time.slot;
    message_id = message->ittiMsgHeader.messageId;
    DevCheck(message_id < itti_desc.messages_id_max, itti_desc.messages_id_max, message_id, 0);

    origin_task_id = ITTI_MSG_ORIGIN_ID(message);

    priority = itti_get_message_priority (message_id);

    /* Increment the global message number */
    message_number = itti_increment_message_number ();

    itti_dump_queue_message (origin_task_id, message_number, message, itti_desc.messages_info[message_id].name,
                             sizeof(MessageHeader) + message->ittiMsgHeader.ittiMsgSize);

    if (destination_task_id != TASK_UNKNOWN)
    {
#if defined(OAI_EMU) || defined(RTAI)
        vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_ITTI_ENQUEUE_MESSAGE, VCD_FUNCTION_IN);
#endif

        if (itti_desc.threads[destination_thread_id].task_state == TASK_STATE_ENDED)
        {
            ITTI_DEBUG("Message %s, number %lu with priority %d can not be sent from %s to queue (%u:%s), ended destination task!\n",
                       itti_desc.messages_info[message_id].name,
                       message_number,
                       priority,
                       itti_get_task_name(origin_task_id),
                       destination_task_id,
                       itti_get_task_name(destination_task_id));
        }
        else
        {
            /* We cannot send a message if the task is not running */
            DevCheck(itti_desc.threads[destination_thread_id].task_state == TASK_STATE_READY, destination_thread_id,
                     itti_desc.threads[destination_thread_id].task_state, message_id);

            /* Allocate new list element */
            new = (message_list_t *) itti_malloc (origin_task_id, sizeof(struct message_list_s));
            DevAssert(new != NULL);

            /* Fill in members */
            new->msg = message;
            new->message_number = message_number;
            new->message_priority = priority;

            /* Enqueue message in destination task queue */
            lfds611_queue_enqueue(itti_desc.tasks[destination_task_id].message_queue, new);

#if defined(OAI_EMU) || defined(RTAI)
            vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_ITTI_ENQUEUE_MESSAGE, VCD_FUNCTION_OUT);
#endif

#ifdef RTAI
            if (itti_desc.threads[TASK_GET_THREAD_ID(origin_task_id)].real_time)
            {
                /* This is a RT task, increase destination task messages pending counter */
                __sync_fetch_and_add (&itti_desc.threads[destination_thread_id].messages_pending, 1);
            }
            else
#endif
            {
                /* Only use event fd for tasks, subtasks will pool the queue */
                if (TASK_GET_PARENT_TASK_ID(destination_task_id) == TASK_UNKNOWN)
                {
                    ssize_t write_ret;
                    eventfd_t sem_counter = 1;

                    /* Call to write for an event fd must be of 8 bytes */
                    write_ret = write (itti_desc.threads[destination_thread_id].task_event_fd, &sem_counter, sizeof(sem_counter));
                    DevCheck(write_ret == sizeof(sem_counter), write_ret, sem_counter, destination_thread_id);
                }
            }

            ITTI_DEBUG("Message %s, number %lu with priority %d successfully sent from %s to queue (%u:%s)\n",
                       itti_desc.messages_info[message_id].name,
                       message_number,
                       priority,
                       itti_get_task_name(origin_task_id),
                       destination_task_id,
                       itti_get_task_name(destination_task_id));
        }
    } else {
        /* This is a debug message to TASK_UNKNOWN, we can release safely release it */
        itti_free(origin_task_id, message);
    }

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_SEND_MSG,
                                            __sync_and_and_fetch (&itti_desc.vcd_send_msg, ~(1L << destination_task_id)));
#endif

    return 0;
}

void itti_subscribe_event_fd(task_id_t task_id, int fd)
{
    thread_id_t thread_id;
    struct epoll_event event;

    DevCheck(task_id < itti_desc.task_max, task_id, itti_desc.task_max, 0);
    DevCheck(fd >= 0, fd, 0, 0);

    thread_id = TASK_GET_THREAD_ID(task_id);
    itti_desc.threads[thread_id].nb_events++;

    /* Reallocate the events */
    itti_desc.threads[thread_id].events = realloc(
        itti_desc.threads[thread_id].events,
        itti_desc.threads[thread_id].nb_events * sizeof(struct epoll_event));

    event.events  = EPOLLIN | EPOLLERR;
    event.data.u64 = 0;
    event.data.fd  = fd;

    /* Add the event fd to the list of monitored events */
    if (epoll_ctl(itti_desc.threads[thread_id].epoll_fd, EPOLL_CTL_ADD, fd,
        &event) != 0)
    {
        ITTI_ERROR("epoll_ctl (EPOLL_CTL_ADD) failed for task %s, fd %d: %s\n",
                   itti_get_task_name(task_id), fd, strerror(errno));
        /* Always assert on this condition */
        DevAssert(0 == 1);
    }

    ITTI_DEBUG("Successfully subscribed fd %d for task %s\n", fd, itti_get_task_name(task_id));
}

void itti_unsubscribe_event_fd(task_id_t task_id, int fd)
{
    thread_id_t thread_id;

    DevCheck(task_id < itti_desc.task_max, task_id, itti_desc.task_max, 0);
    DevCheck(fd >= 0, fd, 0, 0);

    thread_id = TASK_GET_THREAD_ID(task_id);
    /* Add the event fd to the list of monitored events */
    if (epoll_ctl(itti_desc.threads[thread_id].epoll_fd, EPOLL_CTL_DEL, fd, NULL) != 0)
    {
        ITTI_ERROR("epoll_ctl (EPOLL_CTL_DEL) failed for task %s and fd %d: %s\n",
                   itti_get_task_name(task_id), fd, strerror(errno));
        /* Always assert on this condition */
        DevAssert(0 == 1);
    }

    itti_desc.threads[thread_id].nb_events--;
    itti_desc.threads[thread_id].events = realloc(
        itti_desc.threads[thread_id].events,
        itti_desc.threads[thread_id].nb_events * sizeof(struct epoll_event));
}

int itti_get_events(task_id_t task_id, struct epoll_event **events)
{
    thread_id_t thread_id;

    DevCheck(task_id < itti_desc.task_max, task_id, itti_desc.task_max, 0);

    thread_id = TASK_GET_THREAD_ID(task_id);
    *events = itti_desc.threads[thread_id].events;

    return itti_desc.threads[thread_id].epoll_nb_events;
}

static inline void itti_receive_msg_internal_event_fd(task_id_t task_id, uint8_t polling, MessageDef **received_msg)
{
    thread_id_t thread_id;
    int epoll_ret = 0;
    int epoll_timeout = 0;
    int i;

    DevCheck(task_id < itti_desc.task_max, task_id, itti_desc.task_max, 0);
    DevAssert(received_msg != NULL);

    thread_id = TASK_GET_THREAD_ID(task_id);
    *received_msg = NULL;

    if (polling) {
        /* In polling mode we set the timeout to 0 causing epoll_wait to return
         * immediately.
         */
        epoll_timeout = 0;
    } else {
        /* timeout = -1 causes the epoll_wait to wait indefinitely.
         */
        epoll_timeout = -1;
    }

    do {
        epoll_ret = epoll_wait(itti_desc.threads[thread_id].epoll_fd,
                               itti_desc.threads[thread_id].events,
                               itti_desc.threads[thread_id].nb_events,
                               epoll_timeout);
    } while (epoll_ret < 0 && errno == EINTR);

    if (epoll_ret < 0) {
        ITTI_ERROR("epoll_wait failed for task %s: %s\n",
                   itti_get_task_name(task_id), strerror(errno));
        DevAssert(0 == 1);
    }
    if (epoll_ret == 0 && polling) {
        /* No data to read -> return */
        return;
    }

    itti_desc.threads[thread_id].epoll_nb_events = epoll_ret;

    for (i = 0; i < epoll_ret; i++) {
        /* Check if there is an event for ITTI for the event fd */
        if ((itti_desc.threads[thread_id].events[i].events & EPOLLIN) &&
            (itti_desc.threads[thread_id].events[i].data.fd == itti_desc.threads[thread_id].task_event_fd))
        {
            struct message_list_s *message = NULL;
            eventfd_t sem_counter;
            ssize_t   read_ret;

            /* Read will always return 1 */
            read_ret = read (itti_desc.threads[thread_id].task_event_fd, &sem_counter, sizeof(sem_counter));
            DevCheck(read_ret == sizeof(sem_counter), read_ret, sizeof(sem_counter), 0);

#if defined(KERNEL_VERSION_PRE_2_6_30)
            /* Store the value of the semaphore counter */
            itti_desc.threads[task_id].sem_counter = sem_counter - 1;
#endif

            if (lfds611_queue_dequeue (itti_desc.tasks[task_id].message_queue, (void **) &message) == 0) {
                /* No element in list -> this should not happen */
                DevParam(task_id, epoll_ret, 0);
            }
            DevAssert(message != NULL);
            *received_msg = message->msg;
            itti_free (ITTI_MSG_ORIGIN_ID(*received_msg), message);
            /* Mark that the event has been processed */
            itti_desc.threads[thread_id].events[i].events &= ~EPOLLIN;
            return;
        }
    }
}

void itti_receive_msg(task_id_t task_id, MessageDef **received_msg)
{
#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_RECV_MSG,
                                            __sync_and_and_fetch (&itti_desc.vcd_receive_msg, ~(1L << task_id)));
#endif

#if defined(KERNEL_VERSION_PRE_2_6_30)
    /* Store the value of the semaphore counter */
    if (itti_desc.threads[task_id].sem_counter > 0) {
        struct message_list_s *message = NULL;

        if (lfds611_queue_dequeue (itti_desc.tasks[task_id].message_queue, (void **) &message) == 0) {
            /* No element in list -> this should not happen */
            DevParam(task_id, itti_desc.threads[task_id].sem_counter, 0);
        }
        DevAssert(message != NULL);
        *received_msg = message->msg;
        free (message);

        itti_desc.threads[task_id].sem_counter--;
    } else
#endif
    itti_receive_msg_internal_event_fd(task_id, 0, received_msg);

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_RECV_MSG,
                                            __sync_or_and_fetch (&itti_desc.vcd_receive_msg, 1L << task_id));
#endif
}

void itti_poll_msg(task_id_t task_id, MessageDef **received_msg) {
    DevCheck(task_id < itti_desc.task_max, task_id, itti_desc.task_max, 0);
    DevAssert(received_msg != NULL);

    *received_msg = NULL;

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_POLL_MSG,
                                            __sync_or_and_fetch (&itti_desc.vcd_poll_msg, 1L << task_id));
#endif

    {
        struct message_list_s *message;

        if (lfds611_queue_dequeue (itti_desc.tasks[task_id].message_queue, (void **) &message) == 1)
        {
            *received_msg = message->msg;
            free (message);
        }
    }

    if ((itti_debug_poll) && (*received_msg == NULL)) {
        ITTI_DEBUG("No message in queue[(%u:%s)]\n", task_id, itti_get_task_name(task_id));
    }

#if defined(OAI_EMU) || defined(RTAI)
    vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_POLL_MSG,
                                            __sync_and_and_fetch (&itti_desc.vcd_poll_msg, ~(1L << task_id)));
#endif
}

int itti_create_task(task_id_t task_id, void *(*start_routine)(void *), void *args_p) {
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);
    int result;

    DevAssert(start_routine != NULL);
    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);
    DevCheck(itti_desc.threads[thread_id].task_state == TASK_STATE_NOT_CONFIGURED, task_id, thread_id,
             itti_desc.threads[thread_id].task_state);

    itti_desc.threads[thread_id].task_state = TASK_STATE_STARTING;

    ITTI_DEBUG("Create thread for task %s\n", itti_get_task_name(task_id));

    result = pthread_create (&itti_desc.threads[thread_id].task_thread, NULL, start_routine, args_p);
    DevCheck(result >= 0, task_id, thread_id, result);

    /* Wait till the thread is completely ready */
    while (itti_desc.threads[thread_id].task_state != TASK_STATE_READY)
        ;
    return 0;
}

#ifdef RTAI
void itti_set_task_real_time(task_id_t task_id)
{
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);

    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);

    itti_desc.threads[thread_id].real_time = TRUE;
}
#endif

void itti_mark_task_ready(task_id_t task_id)
{
    thread_id_t thread_id = TASK_GET_THREAD_ID(task_id);

    DevCheck(thread_id < itti_desc.thread_max, thread_id, itti_desc.thread_max, 0);

    /* Register the thread in itti dump */
    itti_dump_thread_use_ring_buffer();

    /* Mark the thread as using LFDS queue */
    lfds611_queue_use(itti_desc.tasks[task_id].message_queue);

#ifdef RTAI
    /* Assign low priority to created threads */
    {
        struct sched_param sched_param;
        sched_param.sched_priority = sched_get_priority_min(SCHED_FIFO) + 1;
        sched_setscheduler(0, SCHED_FIFO, &sched_param);
    }
#endif

    itti_desc.threads[thread_id].task_state = TASK_STATE_READY;
}

void itti_exit_task(void) {
#if defined(OAI_EMU) || defined(RTAI)
    task_id_t task_id = itti_get_current_task_id();

    if (task_id > TASK_UNKNOWN)
    {
        vcd_signal_dumper_dump_variable_by_name(VCD_SIGNAL_DUMPER_VARIABLE_ITTI_RECV_MSG,
                                                __sync_and_and_fetch (&itti_desc.vcd_receive_msg, ~(1L << task_id)));
    }
#endif
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

#ifdef RTAI
static void *itti_rt_relay_thread(void *arg)
{
    thread_id_t thread_id;
    unsigned pending_messages;

    while (itti_desc.running)
    {
        usleep (100);

        /* Checks for all non real time tasks if they have pending messages */
        for (thread_id = THREAD_FIRST; thread_id < itti_desc.thread_max; thread_id++)
        {
            if ((itti_desc.threads[thread_id].task_state == TASK_STATE_READY)
                    && (itti_desc.threads[thread_id].real_time == FALSE))
            {
                pending_messages = __sync_fetch_and_and (&itti_desc.threads[thread_id].messages_pending, 0);

                if (pending_messages > 0)
                {
                    ssize_t write_ret;
                    eventfd_t sem_counter = pending_messages;

                    /* Call to write for an event fd must be of 8 bytes */
                    write_ret = write (itti_desc.threads[thread_id].task_event_fd, &sem_counter, sizeof(sem_counter));
                    DevCheck(write_ret == sizeof(sem_counter), write_ret, sem_counter, thread_id);
                }
            }
        }
    }
    return NULL;
}
#endif

int itti_init(task_id_t task_max, thread_id_t thread_max, MessagesIds messages_id_max, const task_info_t *tasks_info,
              const message_info_t *messages_info, const char * const messages_definition_xml, const char * const dump_file_name) {
    task_id_t task_id;
    thread_id_t thread_id;
    int ret;

    itti_desc.message_number = 1;

    ITTI_DEBUG("Init: %d tasks, %d threads, %d messages\n", task_max, thread_max, messages_id_max);

    CHECK_INIT_RETURN(signal_mask());

    /* Saves threads and messages max values */
    itti_desc.task_max = task_max;
    itti_desc.thread_max = thread_max;
    itti_desc.messages_id_max = messages_id_max;
    itti_desc.thread_handling_signals = -1;
    itti_desc.tasks_info = tasks_info;
    itti_desc.messages_info = messages_info;

    /* Allocates memory for tasks info */
    itti_desc.tasks = calloc (itti_desc.task_max, sizeof(task_desc_t));

    /* Allocates memory for threads info */
    itti_desc.threads = calloc (itti_desc.thread_max, sizeof(thread_desc_t));

    /* Initializing each queue and related stuff */
    for (task_id = TASK_FIRST; task_id < itti_desc.task_max; task_id++)
    {
        ITTI_DEBUG("Initializing %stask %s%s%s\n",
                   itti_desc.tasks_info[task_id].parent_task != TASK_UNKNOWN ? "sub-" : "",
                   itti_desc.tasks_info[task_id].name,
                   itti_desc.tasks_info[task_id].parent_task != TASK_UNKNOWN ? " with parent " : "",
                   itti_desc.tasks_info[task_id].parent_task != TASK_UNKNOWN ?
                   itti_get_task_name(itti_desc.tasks_info[task_id].parent_task) : "");

        ITTI_DEBUG("Creating queue of message of size %u\n", itti_desc.tasks_info[task_id].queue_size);

        ret = lfds611_queue_new(&itti_desc.tasks[task_id].message_queue, itti_desc.tasks_info[task_id].queue_size);
        if (ret < 0)
        {
            ITTI_ERROR("lfds611_queue_new failed for task %u\n", task_id);
            DevAssert(0 == 1);
        }
    }

    /* Initializing each thread */
    for (thread_id = THREAD_FIRST; thread_id < itti_desc.thread_max; thread_id++)
    {
        itti_desc.threads[thread_id].task_state = TASK_STATE_NOT_CONFIGURED;

        itti_desc.threads[thread_id].epoll_fd = epoll_create1(0);
        if (itti_desc.threads[thread_id].epoll_fd == -1) {
            ITTI_ERROR("Failed to create new epoll fd: %s\n", strerror(errno));
            /* Always assert on this condition */
            DevAssert(0 == 1);
        }

# if defined(KERNEL_VERSION_PRE_2_6_30)
        /* SR: for kernel versions < 2.6.30 EFD_SEMAPHORE is not defined.
         * A read operation on the event fd will return the 8 byte value.
         */
        itti_desc.threads[thread_id].task_event_fd = eventfd(0, 0);
# else
        itti_desc.threads[thread_id].task_event_fd = eventfd(0, EFD_SEMAPHORE);
# endif
        if (itti_desc.threads[thread_id].task_event_fd == -1)
        {
            ITTI_ERROR("eventfd failed: %s\n", strerror(errno));
            /* Always assert on this condition */
            DevAssert(0 == 1);
        }

        itti_desc.threads[thread_id].nb_events = 1;

        itti_desc.threads[thread_id].events = calloc(1, sizeof(struct epoll_event));

        itti_desc.threads[thread_id].events->events  = EPOLLIN | EPOLLERR;
        itti_desc.threads[thread_id].events->data.fd = itti_desc.threads[thread_id].task_event_fd;

        /* Add the event fd to the list of monitored events */
        if (epoll_ctl(itti_desc.threads[thread_id].epoll_fd, EPOLL_CTL_ADD,
            itti_desc.threads[thread_id].task_event_fd, itti_desc.threads[thread_id].events) != 0)
        {
            ITTI_ERROR("epoll_ctl (EPOLL_CTL_ADD) failed: %s\n", strerror(errno));
            /* Always assert on this condition */
            DevAssert(0 == 1);
        }

        ITTI_DEBUG("Successfully subscribed fd %d for task %s\n",
                   itti_desc.threads[thread_id].task_event_fd, itti_get_task_name(task_id));

#ifdef RTAI
        itti_desc.threads[thread_id].real_time = FALSE;
        itti_desc.threads[thread_id].messages_pending = 0;
#endif
    }

    itti_desc.running = 1;
#ifdef RTAI
    /* Start RT relay thread */
    DevAssert(pthread_create (&itti_desc.rt_relay_thread, NULL, itti_rt_relay_thread, NULL) >= 0);

    rt_global_heap_open();
#endif

#if defined(OAI_EMU) || defined(RTAI)
    itti_desc.vcd_poll_msg = 0;
    itti_desc.vcd_receive_msg = 0;
    itti_desc.vcd_send_msg = 0;
#endif

    itti_dump_init (messages_definition_xml, dump_file_name);

    CHECK_INIT_RETURN(timer_init ());

    return 0;
}

void itti_wait_tasks_end(void) {
    int end = 0;
    int thread_id;
    task_id_t task_id;
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

        task_id = TASK_FIRST;
        for (thread_id = THREAD_FIRST; thread_id < itti_desc.task_max; thread_id++) {
            /* Skip tasks which are not running */
            if (itti_desc.threads[thread_id].task_state == TASK_STATE_READY) {
                while (thread_id != TASK_GET_THREAD_ID(task_id))
                {
                    task_id++;
                }

                result = pthread_tryjoin_np (itti_desc.threads[thread_id].task_thread, NULL);

                ITTI_DEBUG("Thread %s join status %d\n", itti_get_task_name(task_id), result);

                if (result == 0) {
                    /* Thread has terminated */
                    itti_desc.threads[thread_id].task_state = TASK_STATE_ENDED;
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

    itti_desc.running = 0;

    if (ready_tasks > 0) {
        ITTI_DEBUG("Some threads are still running, force exit\n");
        exit (0);
    }

    itti_dump_exit();
}

void itti_send_terminate_message(task_id_t task_id) {
    MessageDef *terminate_message_p;

    terminate_message_p = itti_alloc_new_message (task_id, TERMINATE_MESSAGE);

    itti_send_broadcast_message (terminate_message_p);
}
