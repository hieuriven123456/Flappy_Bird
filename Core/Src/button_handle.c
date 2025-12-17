#include "button_handle.h"

void Button_Handle(uint16_t GPIO_Pin)
{
    static uint32_t last = 0;
    if (HAL_GetTick() - last < 150) return;
    last = HAL_GetTick();

    if (GPIO_Pin == GPIO_PIN_3)
    {
        EventRB_Post(&gEventRB, EVT_BTN_NEXT);
    }
    else if (GPIO_Pin == GPIO_PIN_13)
    {
        EventRB_Post(&gEventRB, EVT_BTN_MENU);
    }
    else if (GPIO_Pin == GPIO_PIN_4)
    {
        Signal_StopAll();
        flag_gameState = GAME_HOME;
        Game_Init();
        EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
    }
}
