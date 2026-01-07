#include "led_handle.h"

#define LED_GPIO_Port GPIOB
#define LED_Pin       GPIO_PIN_8

static uint8_t  led_active = 0;
static uint32_t led_end_time = 0;
static uint32_t led_last_toggle = 0;

void LED_On(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
}

void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
}

void LED_Blink_Start(uint32_t duration_ms)
{
    led_active = 1;
    led_end_time = HAL_GetTick() + duration_ms;
    led_last_toggle = HAL_GetTick();
    LED_On();   
}

void LED_Blink_Update(void)
{
    if (!led_active) return;

    uint32_t now = HAL_GetTick();
    if (now - led_last_toggle >= 100) {
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
        led_last_toggle = now;
    }
    if (now >= led_end_time) {
        LED_Off();
        led_active = 0;
    }
}
