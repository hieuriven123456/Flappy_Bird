#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "soft_i2c.h"
#include "config.h"
#include "game.h"
extern Difficulty game_difficulty;
extern GravityLevel selected_gravity;
extern int menu_index; 

void ssd1306_DrawFilledRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color);
void drawCup(int x, int y);
void drawPipe(int x, int gap_y, int gap_height);
void drawStar(uint8_t x, uint8_t y);
void drawArrow(uint8_t x, uint8_t y);
void Pipes_ResetNormal(void);
void ssd1306_DrawBitmapTransparent( const unsigned char *bitmap, uint8_t w, uint8_t h, int16_t x0, int16_t y0, int invert);
void Game_DrawOptionMenu(const char* title, const char* options[], int num_options, int selected);
void drawZigZagBottom(int y_start);
void drawZigZagTop(int y_end);
void Game_Menu_Main_Draw(void);
void Game_DrawGameOver(void);
void Game_DrawWaitStart(void);
void Game_DrawPlatform(void);
void Game_Draw_Bird(void);
void Game_Menu_Diff(void);
void Game_Menu_Gravity(void);
void Draw_Rank(void);
void Draw_Game_Home(void);
void Draw_Bird_selected(void);
void Draw_History(void);
void Draw_Nopipe_game(void);
void Draw_Game_Normal(void);
extern const unsigned char epd_bitmap_flappybird1 [];
extern const unsigned char epd_bitmap_flappybird2 [];
extern const unsigned char epd_bitmap_flappybird3 [];
extern const unsigned char epd_bitmap_flappybird [];
extern int8_t high_scores[4];
extern uint8_t history_count;

#endif
