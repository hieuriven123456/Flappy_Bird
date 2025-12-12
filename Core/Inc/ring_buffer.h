#ifndef EVENT_RINGBUFFER_H
#define EVENT_RINGBUFFER_H

#include <stdint.h>

#define EVENT_POOL_SIZE 16

typedef enum {
    EVT_NONE = 0,
    EVT_BTN_PRESS,
    EVT_TIMER_TICK,
    EVT_GAME_UPDATE,
    EVT_GAME_OVER,
		EVT_SPEAKER_BEEP, 
    EVT_DRAW_FRAME
} EventType;

typedef struct {
    EventType type;
    uint8_t used;      
} EventMsg;

typedef struct {
    EventMsg pool[EVENT_POOL_SIZE];
    uint8_t head;
    uint8_t tail;
} EventRingBuffer;

void EventRB_Init(EventRingBuffer *rb);
int EventRB_Post(EventRingBuffer *rb, EventType evt);
int EventRB_Get(EventRingBuffer *rb, EventMsg *out);

#endif
