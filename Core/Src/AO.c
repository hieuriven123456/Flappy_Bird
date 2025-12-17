#include "AO.h"


void AO_Init(ActiveObject *ao, StateHandler init_state) {
EventRB_Init(&ao->rb);
ao->state = init_state;
}


void AO_Post(ActiveObject *ao, EventType evt) {
EventRB_Post(&ao->rb, evt);
}


void AO_Dispatch(ActiveObject *ao) {
EventMsg e;
if(EventRB_Get(&ao->rb, &e)) {
ao->state(ao, &e);
}
}

