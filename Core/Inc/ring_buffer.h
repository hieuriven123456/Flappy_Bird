#ifndef EVENT_RINGBUFFER_H
#define EVENT_RINGBUFFER_H

#include <stdint.h>

#define EVENT_POOL_SIZE 16

typedef enum {
    EVT_NONE = 0,
		EVT_BTN_MENU,      
    EVT_BTN_NEXT,     
    EVT_BTN_BACK,    
    EVT_TIMER_TICK,
    EVT_GAME_UPDATE,
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
    uint8_t count;   
    uint8_t size;    
} EventRingBuffer;

void EventRB_Init(EventRingBuffer *rb);
int EventRB_Post(EventRingBuffer *rb, EventType evt);
int EventRB_Get(EventRingBuffer *rb, EventMsg *out);

#endif
