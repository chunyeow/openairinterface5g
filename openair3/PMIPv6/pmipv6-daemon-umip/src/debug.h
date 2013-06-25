/* $Id: debug.h 1.12 06/05/07 21:52:42+03:00 anttit@tcs.hut.fi $ */

#ifndef __DEBUG_H__
#define __DEBUG_H__ 1

#include <stdio.h>

#ifdef MIP6_NDEBUG
#define NDEBUG 1
#define dbg(...)
#define cdbg(...)
#define dbg_buf(...)
#define dbg_func(arg, func)
#define BUG(x)
#define pthread_dbg(x)
#define TRACE
static inline int debug_open(const char *path){ return 0; }
static inline void debug_close(void){}
static inline void debug_init(void){}
#else
#define dbg(...) dbgprint(__FUNCTION__, __VA_ARGS__)
#define cdbg(...) dbgprint(NULL, __VA_ARGS__)
#define dbg_buf(data, len, ...) \
	debug_print_buffer(data, len, __FUNCTION__, __VA_ARGS__)
#define dbg_func(arg, func) debug_print_func(arg, func)

#define BUG(x) dbgprint("BUG", "%s %d %s\n", __FUNCTION__, __LINE__, x)
#define TRACE dbgprint(__FUNCTION__, "%d\n", __LINE__)

void dbgprint(const char *fname, const char *fmt, ...);

void debug_print_buffer(const void *data, int len, const char *fname,
			const char *fmt, ...);
void debug_print_func(void *arg, void (*func)(void *arg, void *stream));

int debug_open(const char *path);
void debug_close(void);
void debug_init(void);

#ifndef DEBUG_LOCKING

#define pthread_dbg(x)

#else

#define pthread_dbg(x) dbgprint(__FUNCTION__, "[%x] %s\n", pthread_self(), x)

#define pthread_mutex_lock(x)\
do {\
	dbg("[%x] pthread_mutex_lock(" #x ")\n", pthread_self());\
	pthread_mutex_lock(x);\
} while(0)

#define pthread_mutex_unlock(x)\
do {\
	pthread_mutex_unlock(x);\
	dbg("[%x] pthread_mutex_unlock(" #x ")\n", pthread_self());\
} while(0)

#define pthread_rwlock_rdlock(x)\
do {\
	dbg("[%x] pthread_rwlock_rdlock(" #x ")\n", pthread_self());\
	pthread_rwlock_rdlock(x);\
} while(0)

#define pthread_rwlock_wrlock(x)\
do {\
	dbg("[%x] pthread_rwlock_wrlock(" #x ")\n", pthread_self());\
	pthread_rwlock_wrlock(x);\
} while(0)

#define pthread_rwlock_unlock(x)\
do {\
	pthread_rwlock_unlock(x);\
	dbg("[%x] pthread_rwlock_unlock(" #x ")\n", pthread_self());\
} while(0)

#define pthread_join(x, y)\
do {\
	dbg("[%x] pthread_join([%x], " #y ")\n", pthread_self(), x);\
	pthread_join(x, y);\
} while(0)

#define pthread_cancel(x)\
do {\
	dbg("[%x] pthread_cancel([%x])\n", pthread_self(), x);\
	pthread_cancel(x);\
} while(0)

#define pthread_cond_signal(x)\
do {\
	dbg("[%x] pthread_cond_signal(" #x ")\n", pthread_self());\
	pthread_cond_signal(x);\
} while(0)

#define pthread_cond_wait(x, y)\
do {\
	dbg("[%x] entering pthread_cond_wait(" #x ", " #y ")\n",\
	    pthread_self());\
	pthread_cond_wait(x, y);\
	dbg("[%x] exiting pthread_cond_wait(" #x ", " #y ")\n",\
	    pthread_self());\
} while(0)

#define pthread_cond_timedwait(x, y, z)\
do {\
	dbg("[%x] entering pthread_cond_timedwait(" #x ", " #y ", " #z ")\n",\
	    pthread_self());\
	pthread_cond_timedwait(x, y, z);\
	dbg("[%x] exiting pthread_cond_timedwait(" #x ", " #y ", " #z ")\n",\
	    pthread_self());\
} while(0)

#define pthread_exit(x)\
do {\
	dbg("[%x] pthread_exit(" #x ")\n", pthread_self());\
	pthread_exit(x);\
} while(0)

#endif /* DEBUG_LOCKING */

#endif /* MIP6_DEBUG */

#include <assert.h>

#endif /* __DEBUG_H__ */
