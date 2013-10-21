#ifndef TIMER_MESSAGES_TYPES_H_
#define TIMER_MESSAGES_TYPES_H_

typedef struct {
    void *arg;
    long  timer_id;
} timer_has_expired_t;

#endif /* TIMER_MESSAGES_TYPES_H_ */
