// sst.h - Super-Simple Tasker Header (Adapted for STM32)
#ifndef SST_H
#define SST_H

#include <stdint.h>  // For uint8_t, etc.
#include "stm32l1xx_hal.h"  // For HAL_GetTick, interrupts, etc. Adjust for your STM32 series.

// Define max priorities (up to 8 for byte ready-set)
#define SST_MAX_PRIO 4

// Event structure
typedef uint8_t SSTSignal;
typedef uint32_t SSTParam;  // Adjust size if needed
typedef struct {
    SSTSignal sig;
    SSTParam par;
} SSTEvent;

// Task callback (RTC function taking event)
typedef void (*SSTTask)(SSTEvent);

// Task Control Block
typedef struct {
    SSTTask task__;
    uint8_t mask__;
    SSTEvent *queue__;
    uint8_t head__;
    uint8_t tail__;
    uint8_t nUsed__;
    uint8_t end__;
} TaskCB;

// Interrupt priorities (higher than tasks)
#define TICK_ISR_PRIO (SST_MAX_PRIO + 1)
#define BUTTON_ISR_PRIO (SST_MAX_PRIO + 2)

// Macros for interrupt lock/unlock (STM32 specific)
#define SST_INT_LOCK() __disable_irq()
#define SST_INT_UNLOCK() __enable_irq()

// ISR entry/exit macros
#define SST_ISR_ENTRY(pin_, isrPrio_) do { \
    (pin_) = SST_currPrio_; \
    SST_currPrio_ = (isrPrio_); \
    SST_INT_UNLOCK(); \
} while (0)

#define SST_ISR_EXIT(pin_, EOI_command_) do { \
    SST_INT_LOCK(); \
    if (EOI_command_) { (EOI_command_); } \
    SST_currPrio_ = (pin_); \
    SST_schedule_(); \
} while (0)
// External variables and functions
extern uint8_t SST_currPrio_;
extern uint8_t SST_readySet_;
extern TaskCB l_taskCB[];

void SST_schedule_(void);
uint8_t SST_post(uint8_t prio, SSTSignal sig, SSTParam par);
uint8_t SST_mutexLock(uint8_t prioCeiling);
void SST_mutexUnlock(uint8_t orgPrio);
void SST_run(void);
void SST_start(void) {
    // Configure SysTick for 10ms
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 100);  // 10ms if 100Hz
    // Enable EXTI for button (assume MX_GPIO_Init does it)
}  // Application-specific start
void SST_onIdle(void); // Application-specific idle

#endif // SST_H
