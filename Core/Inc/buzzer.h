#ifndef SPEAKER_HANDLE_H
#define SPEAKER_HANDLE_H

#include "gpio.h"
#include <stdint.h>

void Speaker_On(void);
void Speaker_Off(void);

/**
 * @brief Bat buzzer trong khoang thoi gian non-blocking
 */
void Speaker_Beep_Start(uint32_t duration_ms);

/**
 * @brief Cap nhat buzzer non-blocking, goi liên tuc trong main loop
 */
void Speaker_Beep_Update(void);

#endif
