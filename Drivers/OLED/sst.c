// sst.c - Super-Simple Tasker Implementation (Minimal, adapted)
#include "sst.h"

// Global variables
uint8_t SST_currPrio_ = 0xFF;  // Initial high value
uint8_t SST_readySet_ = 0;     // Ready set bitmask

// Task control blocks array (indexed by prio-1)
TaskCB l_taskCB[SST_MAX_PRIO];

// Log2 lookup table for finding highest priority
static uint8_t const log2Lkup[256] = {
    0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
    8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
};

// Event posting
uint8_t SST_post(uint8_t prio, SSTSignal sig, SSTParam par) {
    TaskCB *tcb = &l_taskCB[prio - 1];
    SST_INT_LOCK();
    if (tcb->nUsed__ < tcb->end__) {
        tcb->queue__[tcb->head__].sig = sig;
        tcb->queue__[tcb->head__].par = par;
        if ((++tcb->head__) == tcb->end__) {
            tcb->head__ = 0;
        }
        if ((++tcb->nUsed__) == 1) {
            SST_readySet_ |= tcb->mask__;
            SST_schedule_();
        }
        SST_INT_UNLOCK();
        return 1;
    } else {
        SST_INT_UNLOCK();
        return 0;
    }
}

// Scheduler
void SST_schedule_(void) {
    uint8_t pin = SST_currPrio_;
    uint8_t p;
    while ((p = log2Lkup[SST_readySet_]) > pin) {
        TaskCB *tcb = &l_taskCB[p - 1];
        SSTEvent e = tcb->queue__[tcb->tail__];
        if ((++tcb->tail__) == tcb->end__) {
            tcb->tail__ = 0;
        }
        if ((--tcb->nUsed__) == 0) {
            SST_readySet_ &= ~tcb->mask__;
        }
        SST_currPrio_ = p;
        SST_INT_UNLOCK();
        (*tcb->task__)(e);
        SST_INT_LOCK();
    }
    SST_currPrio_ = pin;
}

// Priority-ceiling mutex
uint8_t SST_mutexLock(uint8_t prioCeiling) {
    uint8_t p;
    SST_INT_LOCK();
    p = SST_currPrio_;
    if (prioCeiling > SST_currPrio_) {
        SST_currPrio_ = prioCeiling;
    }
    SST_INT_UNLOCK();
    return p;
}

void SST_mutexUnlock(uint8_t orgPrio) {
    SST_INT_LOCK();
    if (orgPrio < SST_currPrio_) {
        SST_currPrio_ = orgPrio;
        SST_schedule_();
    }
    SST_INT_UNLOCK();
}

// Run SST
void SST_run(void) {
    SST_start();  // Start interrupts (application-specific)
    SST_INT_LOCK();
    SST_currPrio_ = 0;  // Idle priority
    SST_schedule_();
    SST_INT_UNLOCK();
    for (;;) {  // Idle loop
        SST_onIdle();
    }
}
