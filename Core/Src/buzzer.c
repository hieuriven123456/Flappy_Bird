#include "buzzer.h"

#define SPEAKER_GPIO_Port GPIOB
#define SPEAKER_Pin       GPIO_PIN_0

static uint32_t speaker_end_tick = 0;
static uint8_t speaker_beeping = 0;

void Speaker_On(void)
{
    HAL_GPIO_WritePin(SPEAKER_GPIO_Port, SPEAKER_Pin, GPIO_PIN_SET);
}

void Speaker_Off(void)
{
    HAL_GPIO_WritePin(SPEAKER_GPIO_Port, SPEAKER_Pin, GPIO_PIN_RESET);
}

void Speaker_Beep_Start(uint32_t duration_ms)
{
    Speaker_On();
    speaker_end_tick = HAL_GetTick() + duration_ms;
    speaker_beeping = 1;
}

void Speaker_Beep_Update(void)
{
    if(speaker_beeping && HAL_GetTick() >= speaker_end_tick)
    {
        Speaker_Off();
        speaker_beeping = 0;
    }
}
