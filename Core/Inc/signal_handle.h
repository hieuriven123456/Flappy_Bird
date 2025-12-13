#ifndef SIGNAL_HANDLE_H
#define SIGNAL_HANDLE_H

#include "led_handle.h"
#include "buzzer.h"
#include <stdint.h>

typedef enum {
    SIGNAL_MODE_NONE = 0,
    SIGNAL_MODE_GAME_OVER
} SignalMode;

void Signal_SetMode(SignalMode mode);
void Signal_Update(void);

void Signal_BeepLED(uint32_t duration_ms);


void Signal_Update(void);
void Signal_StopAll(void);
#endif
