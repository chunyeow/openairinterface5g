/* $Id: tqueue.h 1.21 06/05/05 12:14:34+03:00 anttit@tcs.hut.fi $ */

#ifndef __TQUEUE_H__
#define __TQUEUE_H__ 1

#include <time.h>
#include <pthread.h>

#include "list.h"
#include "util.h"

struct tq_elem {
	struct list_head list;
	struct timespec expires;       	/* expire time for task */
	pthread_t thread;		/* who queued this task */
	void (*task)(struct tq_elem *);	/* pointer to task      */
};

#define tq_data(ptr, type, member) \
        container_of(ptr, type, member)

/*
 * Initialize task queue.  Must be done before using anything else.
 */
int taskqueue_init(void);

/*
 * Remove all pending tasks and destroy queue.
 */
void taskqueue_destroy(void);

/* check if task has been interruped while waiting for a lock */
int task_interrupted(void);

/* Add task task(tqi) to be triggered at expires */
int add_task_abs(const struct timespec *expires,
		 struct tq_elem *tqi, void (*task)(struct tq_elem *));

/* Add task to be triggered after expires_in */
static inline int add_task_rel(const struct timespec *expires_in,
			       struct tq_elem *tqi,
			       void (*task)(struct tq_elem *))
{
       struct timespec expire;

       clock_gettime(CLOCK_REALTIME, &expire);
       tsadd(expire, *expires_in, expire);

       return add_task_abs(&expire, tqi, task);
}

/* Delete task from list */
int del_task(struct tq_elem *tqi);

#endif /* __TQUEUE_H__ */
