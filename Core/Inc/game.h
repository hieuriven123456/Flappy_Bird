#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>  
#include "signal_handle.h"

typedef struct {
    float x, y;
    float vy;
} Bird;

typedef struct {
    int16_t x;
    int8_t gap_y;
    int8_t gap_height;
    uint8_t passed;
} Pipe;
typedef struct {
    int score;
    int difficulty;
} ScoreHistory;


extern Bird bird;
extern uint8_t next_reward_score;

void Game_Init(void);
void Game_Update(void);
void Game_Draw(void);
void Bird_Jump(void);
void Pipes_ResetNormal(void);

#endif
