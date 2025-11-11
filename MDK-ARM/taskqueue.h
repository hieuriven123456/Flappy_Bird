#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#define MAX_TASKS 8

typedef void (*TaskCallback)(void *arg);

typedef struct {
    TaskCallback cb;
    void *arg;
} Task;

typedef struct {
    Task tasks[MAX_TASKS];
    int head, tail;
} TaskQueue;

void TaskQueue_Init(TaskQueue *q);
int TaskQueue_IsEmpty(TaskQueue *q);
int TaskQueue_IsFull(TaskQueue *q);
int TaskQueue_Push(TaskQueue *q, TaskCallback cb, void *arg);
int TaskQueue_Pop(TaskQueue *q, Task *t);

#endif
