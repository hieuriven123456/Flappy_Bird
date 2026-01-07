#ifndef GAME_AO_H
#define GAME_AO_H


#include "EVT_RB.h"
#include "config.h"

extern EventRingBuffer gEventRB;
extern uint8_t selected_bird;
extern GameState flag_gameState;
extern int menu_index;
extern Difficulty game_difficulty;
extern GravityLevel selected_gravity;


void GameAO_Init(void);
void GameAO_Handle(EventMsg *e);

#endif

