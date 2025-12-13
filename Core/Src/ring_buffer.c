#include "ring_buffer.h"

void EventRB_Init(EventRingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
    rb->size = EVENT_POOL_SIZE;
    for(int i=0; i<rb->size; i++)
        rb->pool[i].used = 0;
}

int EventRB_Post(EventRingBuffer *rb, EventType evt) {
    if(rb->count >= rb->size) return 0; // full

    rb->pool[rb->tail].type = evt;
    rb->pool[rb->tail].used = 1;

    rb->tail = (rb->tail + 1) % rb->size;
    rb->count++;
    return 1;
}


int EventRB_Get(EventRingBuffer *rb, EventMsg *out) {
    if(rb->count == 0) return 0; // empty

    *out = rb->pool[rb->head];
    rb->pool[rb->head].used = 0;

    rb->head = (rb->head + 1) % rb->size;
    rb->count--;
    return 1;
}
