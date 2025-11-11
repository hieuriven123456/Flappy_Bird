#include "taskqueue.h"

void TaskQueue_Init(TaskQueue *q) {
    q->head = q->tail = 0;
}

int TaskQueue_IsEmpty(TaskQueue *q) {
    return (q->head == q->tail);
}

int TaskQueue_IsFull(TaskQueue *q) {
    return ((q->head + 1) % MAX_TASKS == q->tail);
}

int TaskQueue_Push(TaskQueue *q, TaskCallback cb, void *arg) {
    if (TaskQueue_IsFull(q)) return 0;
    q->tasks[q->head].cb = cb;
    q->tasks[q->head].arg = arg;
    q->head = (q->head + 1) % MAX_TASKS;
    return 1;
}

int TaskQueue_Pop(TaskQueue *q, Task *t) {
    if (TaskQueue_IsEmpty(q)) return 0;
    *t = q->tasks[q->tail];
    q->tail = (q->tail + 1) % MAX_TASKS;
    return 1;
}
