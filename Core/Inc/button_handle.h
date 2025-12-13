#ifndef BUTTON_H
#define BUTTON_H

#include "config.h"
#include "ring_buffer.h"
#include "gpio.h"
#include "game.h"

extern EventRingBuffer gEventRB;
extern uint8_t selected_bird;
extern GameState flag_gameState;
extern int menu_index;
extern Difficulty game_difficulty;
extern GravityLevel selected_gravity;


void Button_Handle(uint16_t GPIO_Pin);  
void HandleButtonPress(void);
void HandleJumpOrNextOption(void);
#endif
