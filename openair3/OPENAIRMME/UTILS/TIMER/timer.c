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
#include <signal.h>
#include <time.h>
#include <errno.h>

#include "intertask_interface.h"
#include "timer.h"

#include "queue.h"

#define SIG SIGRTMIN

#ifndef TMR_DEBUG
# define TMR_DEBUG(x, args...) do { fprintf(stdout, "[TMR] [D]"x, ##args); } while(0)
#endif
#ifndef TMR_ERROR
# define TMR_ERROR(x, args...) do { fprintf(stdout, "[TMR] [E]"x, ##args); } while(0)
#endif

struct timer_elm_s {
    uint32_t                  task_id;  ///< Task ID which has requested the timer
    timer_t                   timer;    ///< Unique timer id
    timer_type_t              type;     ///< Timer type
    STAILQ_ENTRY(timer_elm_s) entries;  ///< Pointer to next element
};

/* TIMER task thread: read messages from other tasks */
static pthread_t timer_task_thread;

static STAILQ_HEAD(timer_list_head, timer_elm_s) timer_queue;

static pthread_mutex_t timer_list_mutex;

#define TIMER_SEARCH(var, timerfield, timervalue) \
do {        \
    STAILQ_FOREACH(var, &timer_queue, entries) {    \
        if (((var)->timerfield == timervalue))      \
            break;                                  \
    }                                               \
} while(0)

static void timer_expiry_handler(int sig, siginfo_t *si_p, void *arg_p) {

    TMR_DEBUG("Received timer ex");

    if (si_p->si_ptr == NULL) {
        /* Pointer may not exist */
        TMR_DEBUG("Received timer expiry for non existing timer\n");
    } else {
        struct timer_elm_s *timer_p;
        MessageDef         *message_p;
        TimerHasExpired    *timer_expired_p;
        uint32_t            task_id;

        /* Get back pointer to timer list element */
        timer_p = (struct timer_elm_s *)si_p->si_ptr;
        task_id = timer_p->task_id;
        message_p = alloc_new_message(TASK_TIMER, timer_p->task_id, TIMER_HAS_EXPIRED);

        timer_expired_p = &message_p->msg.timerHasExpired;
        timer_expired_p->timer_id = (long)timer_p->timer;

        /* Timer is a one shot timer, remove it */
        if (timer_p->type == TIMER_ONE_SHOT) {
            if (timer_delete(timer_p->timer) < 0) {
                TMR_DEBUG("Failed to delete timer %ld\n", (long)timer_p->timer);
            }
            pthread_mutex_lock(&timer_list_mutex);
            STAILQ_REMOVE(&timer_queue, timer_p, timer_elm_s, entries);
            pthread_mutex_unlock(&timer_list_mutex);
            free(timer_p);
            timer_p = NULL;
        }

        /* Notify task of timer expiry */
        if (send_msg_to_task(task_id, message_p) < 0) {
            TMR_DEBUG("Failed to send msg TIMER_HAS_EXPIRED to task %u\n", task_id);
        }
    }
}

int timer_setup(
    uint32_t      interval_sec,
    uint32_t      interval_us,
    uint32_t      task_id,
    timer_type_t  type,
    long         *timer_id) {

    struct sigevent     se;
    struct itimerspec   its;
    struct sigaction    sa;
    struct timer_elm_s *timer_p;
    timer_t             timer;

    /* Setting up signal handler */
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = timer_expiry_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIG, &sa, NULL) < 0) {
        TMR_ERROR("Failed to setup signal handler (%s)\n", strerror(errno));
        return -1;
    }

    /* Allocate new timer list element */
    timer_p = malloc(sizeof(struct timer_elm_s));
    if (timer_p == NULL) {
        TMR_ERROR("Failed to create new timer element\n");
        return -1;
    }
    timer_p->task_id = task_id;

    /* Setting up alarm */
    /* Set and enable alarm */
    se.sigev_notify = SIGEV_SIGNAL;
    se.sigev_signo = SIG;
    se.sigev_value.sival_ptr = timer_p;
    /* At the timer creation, the timer structure will be filled in with timer_id,
     * which is unique for this process. This id is allocated by kernel and the
     * value might be used to distinguish timers.
     */
    if (timer_create(CLOCK_REALTIME, &se, &timer) < 0) {
        TMR_ERROR("Failed to create timer: (%s:%d)\n", strerror(errno), errno);
        free(timer_p);
        return -1;
    }

    /* Fill in the first expiration value. */
    its.it_value.tv_sec  = interval_sec;
    its.it_value.tv_nsec = interval_us * 1000;

    if (type == TIMER_PERIODIC) {
        /* Asked for periodic timer. We set the interval time */
        its.it_interval.tv_sec  = interval_sec;
        its.it_interval.tv_nsec = interval_us * 1000;
    } else {
        /* Asked for one-shot timer. Do not set the interval field */
        its.it_interval.tv_sec  = 0;
        its.it_interval.tv_nsec = 0;
    }
    timer_settime(timer, 0, &its, NULL);
    /* Simply set the timer_id argument. so it can be used by caller */
    *timer_id = (long)timer;
    TMR_DEBUG("Requesting new %s timer with id 0x%08lu that expires within "
              "%d sec and %d usec\n",
              type == TIMER_PERIODIC ? "periodic" : "single shot",
              *timer_id, interval_sec, interval_us);

    timer_p->timer = timer;

    /* Lock the queue and insert the timer at the tail */
    pthread_mutex_lock(&timer_list_mutex);
    STAILQ_INSERT_TAIL(&timer_queue, timer_p, entries);
    pthread_mutex_unlock(&timer_list_mutex);

    return 0;
}

int timer_remove(long timer_id) {
    struct timer_elm_s *timer_p;
    int rc = 0;

    TIMER_SEARCH(timer_p, timer, ((timer_t)timer_id));

    /* We didn't find the timer in list */
    if (timer_p == NULL) {
        return -1;
    }

    pthread_mutex_lock(&timer_list_mutex);
    STAILQ_REMOVE(&timer_queue, timer_p, timer_elm_s, entries);
    pthread_mutex_unlock(&timer_list_mutex);

    if (timer_delete(timer_p->timer) < 0) {
        TMR_DEBUG("Failed to delete timer %ld\n", (long)timer_p->timer);
        rc = -1;
    }
    free(timer_p);
    timer_p = NULL;
    return rc;
}

static void *timer_intertask_interface(void *args_p) {
    while(1) {
        MessageDef *received_message_p;
        receive_msg(TASK_TIMER, &received_message_p);
        switch(received_message_p->messageId) {
            default:
            {
                TMR_DEBUG("Unknown message ID %d\n", received_message_p->messageId);
            } break;
        }
        free(received_message_p);
        received_message_p = NULL;
    }
    return NULL;
}

int timer_init(const mme_config_t *mme_config) {
//     sigset_t     set;

    TMR_DEBUG("Initializing TIMER task interface\n");

    STAILQ_INIT(&timer_queue);

//     sigemptyset(&set);
//     /* Enables all signals for timer task. */
//     if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0) {
//         perror("pthread_sigmask");
//     }

    if (pthread_create(&timer_task_thread, NULL, &timer_intertask_interface, NULL) < 0) {
        TMR_ERROR("udp pthread_create (%s)\n", strerror(errno));
        return -1;
    }
    TMR_DEBUG("Initializing TIMER task interface: DONE\n");
    return 0;
}
