#ifndef ACTIVE_OBJECT_H
#define ACTIVE_OBJECT_H


#include "ring_buffer.h"
#include <stdint.h>


typedef struct ActiveObject ActiveObject;
typedef void (*StateHandler)(ActiveObject *, EventMsg *);


struct ActiveObject {
EventRingBuffer rb;
StateHandler state;
};


void AO_Init(ActiveObject *ao, StateHandler init_state);
void AO_Post(ActiveObject *ao, EventType evt);
void AO_Dispatch(ActiveObject *ao);


#endif