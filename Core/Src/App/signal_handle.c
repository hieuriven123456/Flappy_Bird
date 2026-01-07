#include "signal_handle.h"
#include "gpio.h"

#define LED_PORT      GPIOB
#define LED_PIN       GPIO_PIN_8

#define BUZZ_PORT      GPIOB
#define BUZZ_PIN       GPIO_PIN_0

static SignalMode currentMode = SIGNAL_MODE_NONE;
static uint32_t lastToggle = 0;

void Signal_SetMode(SignalMode mode)
{
    currentMode = mode;

    if (mode == SIGNAL_MODE_NONE) {
        HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(BUZZ_PORT, BUZZ_PIN  , GPIO_PIN_RESET);
    }

    if (mode == SIGNAL_MODE_GAME_OVER) {
        lastToggle = HAL_GetTick();
    }
}

void Signal_Update(void)
{
    uint32_t now = HAL_GetTick();

    switch (currentMode)
    {
        case SIGNAL_MODE_GAME_OVER:
            if (now - lastToggle >= 150) {
                HAL_GPIO_TogglePin(LED_PORT, LED_PIN);
                HAL_GPIO_TogglePin(BUZZ_PORT, BUZZ_PIN );
                lastToggle = now;
            }
        break;

        default:
        break;
    }
}
void Signal_StopAll(void) {
    // Tat LED
    HAL_GPIO_WritePin(LED_PORT, LED_PIN, GPIO_PIN_RESET);

    // Tat buzzer
    HAL_GPIO_WritePin(BUZZ_PORT, BUZZ_PIN , GPIO_PIN_RESET);

     currentMode = SIGNAL_MODE_NONE;

    // Reset toggle timer
    lastToggle = 0;
}
