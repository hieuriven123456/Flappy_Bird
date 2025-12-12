#include "ring_buffer.h"

void EventRB_Init(EventRingBuffer *rb) {
    rb->head = 0;
    rb->tail = 0;
    for (int i = 0; i < EVENT_POOL_SIZE; i++)
        rb->pool[i].used = 0;
}

int EventRB_Post(EventRingBuffer *rb, EventType evt) {
    uint8_t next = (rb->tail + 1) % EVENT_POOL_SIZE;
    if (next == rb->head) {
        // full
        return 0;
    }
    rb->pool[rb->tail].type = evt;
    rb->pool[rb->tail].used = 1;
    rb->tail = next;
    return 1;
}

int EventRB_Get(EventRingBuffer *rb, EventMsg *out) {
    if (rb->head == rb->tail) return 0; // empty
    *out = rb->pool[rb->head];
    rb->pool[rb->head].used = 0;
    rb->head = (rb->head + 1) % EVENT_POOL_SIZE;
    return 1;
}
