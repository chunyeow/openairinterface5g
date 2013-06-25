#ifndef TIMER_MESSAGES_TYPES_H_
#define TIMER_MESSAGES_TYPES_H_

typedef struct {
    uint32_t expiry_sec;
    uint32_t expiry_usec;
    uint32_t timer_id;
} TimerNewRequest;

typedef struct {
    long timer_id;
}TimerHasExpired;

typedef struct {
    uint32_t timer_id;
} TimerRemove;

#endif /* TIMER_MESSAGES_TYPES_H_ */
