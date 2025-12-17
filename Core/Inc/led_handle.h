#ifndef LED_HANDLE_H
#define LED_HANDLE_H

#include "main.h"
#include <stdint.h>

void LED_On(void);
void LED_Off(void);

/**
 * @brief Nhay LED trong mot khoang thoi gian, non-blocking
 * @param duration_ms Thoi gian LED sang (ms)
 */
void LED_Blink_Start(uint32_t duration_ms);


void LED_Blink_Update(void);

#endif
