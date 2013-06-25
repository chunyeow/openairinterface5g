/*
 * $Id: tqueue.c 1.49 06/02/28 18:57:32+02:00 anttit@tcs.hut.fi $
 *
 * This file is part of the MIPL Mobile IPv6 for Linux.
 * 
 * Authors:
 *  Antti Tuominen <anttit@tcs.hut.fi>
 *  Ville Nuorvala <vnuorval@tcs.hut.fi>
 *
 * Copyright 2001-2005 GO-Core Project
 * Copyright 2003-2006 Helsinki University of Technology
 *
 * MIPL Mobile IPv6 for Linux is free software; you can redistribute
 * it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; version 2 of
 * the License.
 *
 * MIPL Mobile IPv6 for Linux is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MIPL Mobile IPv6 for Linux; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#include "debug.h"
#include "util.h"
#include "tqueue.h"

LIST_HEAD(tq_list);

static volatile int killed = 0;
static pthread_mutex_t mutex;
static pthread_cond_t cond;
static pthread_t tq_runner;

static volatile struct tq_elem *curr = NULL;

static inline int is_first_task(struct tq_elem *tqe)
{
	return (tq_list.next == &tqe->list);
}

static void *runner(void *arg);

/**
 * taskqueue_init - initialize task queue
 * @h_runner: thread handle
 *
 * Initializes task queue and creates a task runner thread.
 **/
int taskqueue_init(void)
{
	pthread_mutexattr_t mattrs;
	pthread_mutexattr_init(&mattrs);
	pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_FAST_NP);
	if (pthread_mutex_init(&mutex, &mattrs) ||
	    pthread_cond_init(&cond, NULL) ||
	    pthread_create(&tq_runner, NULL, runner, NULL))
		return -1;
	return 0;
}

/**
 * taskqueue_destroy - destroy task queue
 * @h_runner: task runner thread handle
 *
 * Destroys task queue and deletes all entries.  Task runner will
 * complete pending task, if taskqueue_destroy() is called mid task.
 **/
void taskqueue_destroy(void)
{
	struct list_head *l, *n;
	pthread_mutex_lock(&mutex);
	list_for_each_safe(l, n, &tq_list) {
		struct tq_elem *tqe;
		list_del(l);
		tqe = list_entry(l, struct tq_elem, list);
		tsclear(tqe->expires);
	}
	killed = 1;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
	pthread_join(tq_runner, NULL);
}

/**
 * sorted_insert - insert queue item sorted by expiration time
 * @tqi: task queue entry to insert
 *
 * Inserts @tqi to task queue sorted by expiration time.
 **/
static inline void sorted_insert(struct tq_elem *tqi)
{
	struct list_head *l;

	list_for_each(l, &tq_list) {
		struct tq_elem *cur = list_entry(l, struct tq_elem, list);
		if (tsbefore(cur->expires, tqi->expires)) {
			list_add_tail(&tqi->list, &cur->list);
			return;
		}
	}
	list_add_tail(&tqi->list, &tq_list);
}

/**
 * del_task - delete task from list
 * @elem: task queue element to remove
 *
 * Deletes task queue element @elem.  Element is removed from the list
 * (not freed), and expire time is set to zero.  Returns 0 on success,
 * otherwise negative error code.
 **/
int del_task(struct tq_elem *elem)
{
	assert(elem != NULL);

	pthread_mutex_lock(&mutex);
	list_del(&elem->list);
	tsclear(elem->expires);
	if (elem == curr)
		curr = NULL;
	pthread_mutex_unlock(&mutex);
	
	return 0;
}

/**
 * add_task_abs - add new task with task to task queue
 * @expires: absolute expiry time
 * @tqi: task entry
 * @task: task to execute on expiry
 *
 * Adds @task to task queue.  Task will expire in @ms milliseconds.
 * Task @data is stored with the entry.  @tqi points to a buffer which
 * hold the actual task queue entry.
 **/
int add_task_abs(const struct timespec *expires,
		 struct tq_elem *tqi, void (*task)(struct tq_elem *))
{
	pthread_mutex_lock(&mutex);
	if (tsisset(tqi->expires)) {
		list_del(&tqi->list);
	}
	if (tqi == curr)
		curr = NULL;
	tqi->expires = *expires;
	tqi->task = task;
	tqi->thread = pthread_self();
	sorted_insert(tqi);
	if (is_first_task(tqi))
		pthread_cond_signal(&cond);

	pthread_mutex_unlock(&mutex);

	return 0;
}

int task_interrupted(void)
{
	int ret;
	pthread_mutex_lock(&mutex);
	ret = (curr == NULL);
	pthread_mutex_unlock(&mutex);
	return ret;
}

/**
 * runner - run expiring tasks
 * @arg: NULL
 *
 **/
static void *runner(__attribute__ ((unused)) void *arg)
{
	pthread_dbg("thread started");
	pthread_mutex_lock(&mutex);
	for (;;) {
		struct timespec now;

		if (killed)
			break;

		if (list_empty(&tq_list))
			pthread_cond_wait(&cond, &mutex);

		clock_gettime(CLOCK_REALTIME, &now);

		while (!list_empty(&tq_list)) {
			struct tq_elem *first;
			first = list_entry(tq_list.next, struct tq_elem, list);
			if (tsbefore(first->expires, now)) {
				pthread_cond_timedwait(&cond, &mutex,
						       &first->expires);
				break;
			}
			list_del(&first->list);
			tsclear(first->expires);
			curr = first;
			pthread_mutex_unlock(&mutex);
			first->task(first);
			pthread_mutex_lock(&mutex);
			curr = NULL;
		}		
	}
	pthread_mutex_unlock(&mutex);
	pthread_exit(NULL);
}
