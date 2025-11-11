/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "soft_i2c.h"
#include "gpio.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */

// =========================
// C?u hình chung
// =========================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

typedef enum {
    EVT_NONE = 0,
    EVT_BTN_PRESS,
    EVT_TIMER_TICK,
    EVT_GAME_UPDATE,
    EVT_GAME_OVER,
    EVT_DRAW_FRAME
} EventType;

typedef struct {
    EventType type;
} EventMsg;


typedef enum {
    GAME_HOME = 0,
		GAME_PLATFORM,
    GAME_MENU,
    GAME_WAIT_START,
    GAME_PLAYING,
    GAME_OVER,
		GAME_HISTORY,
		GAME_BIRD_SELECT
} GameState;


typedef enum {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} Difficulty;

typedef struct {
    uint8_t x, y;
    int8_t vy;
} Bird;

typedef struct {
    int8_t x;
    int8_t gap_y;
    int8_t gap_height;
    uint8_t passed;
} Pipe;
typedef struct {
    int score;
    int difficulty;
} ScoreHistory;



const char* HOME_ITEMS[] = {
    "Start",
    "Menu",
    "History",
    "Bird Select",
    "Exit"
};


#define HOME_ITEM_COUNT 5

#define MAX_PIPES 3
#define MAX_POOL 8
int home_view_start = 0; 

static EventMsg msg_pool[MAX_POOL];
static uint8_t msg_head = 0, msg_tail = 0;

GameState flag_gameState = GAME_HOME;
Difficulty game_difficulty = DIFFICULTY_EASY;

uint8_t menu_option = 0;
uint8_t home_option = 0;

uint8_t selected_bird = 0;
Bird bird;
Pipe pipes[MAX_PIPES];
ScoreHistory history[5];
uint8_t num_pipes = 1;
uint8_t score = 0;
int last_scores[5] = {0};
uint8_t history_count = 0;

int gameOver = 0;

int cups = 0;        
    

int next_reward_score = 5; 

const int cup_unlock_thresholds[3] = {0, 0, 0};
static void post_event(EventType evt) {
    msg_pool[msg_tail].type = evt;
    msg_tail = (msg_tail + 1) % MAX_POOL;
}

static int get_event(EventMsg *out) {
    if (msg_head == msg_tail) return 0;
    *out = msg_pool[msg_head];
    msg_head = (msg_head + 1) % MAX_POOL;
    return 1;
}


void ssd1306_DrawFilledRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, SSD1306_COLOR color) {
    for (uint8_t x = x1; x <= x2; x++) {
        for (uint8_t y = y1; y <= y2; y++) {
            ssd1306_DrawPixel(x, y, color);
        }
    }
}


void drawCup(int x, int y) {

    ssd1306_DrawPixel(x+2, y, White);
    ssd1306_DrawPixel(x+3, y, White);
    ssd1306_DrawPixel(x+4, y, White);

    ssd1306_DrawPixel(x+1, y+1, White);
    ssd1306_DrawPixel(x+5, y+1, White);

    ssd1306_DrawPixel(x, y+2, White);
    ssd1306_DrawPixel(x+6, y+2, White);

    ssd1306_DrawPixel(x, y+3, White);
    ssd1306_DrawPixel(x+6, y+3, White);

    for (int i = 1; i <= 5; i++) ssd1306_DrawPixel(x+i, y+4, White); 
    for (int i = 2; i <= 4; i++) ssd1306_DrawPixel(x+i, y+5, White); 
    for (int i = 1; i <= 5; i++) ssd1306_DrawPixel(x+i, y+6, White); 
}

void drawPipe(int x, int gap_y, int gap_height) {
    int pipe_width = 12;


    ssd1306_DrawFilledRectangle(x, 0, x + pipe_width, gap_y, White);

    ssd1306_DrawFilledRectangle(x - 1, gap_y - 3, x + pipe_width + 1, gap_y, White);

  
    ssd1306_DrawFilledRectangle(x, gap_y + gap_height, x + pipe_width, SCREEN_HEIGHT, White);
   
    ssd1306_DrawFilledRectangle(x - 1, gap_y + gap_height, x + pipe_width + 1, gap_y + gap_height + 3, White);

  
    ssd1306_DrawFilledRectangle(x + 2, 0, x + pipe_width - 2, gap_y - 3, Black);
    ssd1306_DrawFilledRectangle(x + 2, gap_y + gap_height + 3, x + pipe_width - 2, SCREEN_HEIGHT, Black);
}
void drawStar(uint8_t x, uint8_t y) {
    ssd1306_DrawPixel(x+3, y, White);
    ssd1306_DrawPixel(x+2, y+1, White);
    ssd1306_DrawPixel(x+4, y+1, White);
    ssd1306_DrawPixel(x+1, y+2, White);
    ssd1306_DrawPixel(x+5, y+2, White);
    ssd1306_DrawPixel(x,   y+3, White);
    ssd1306_DrawPixel(x+6, y+3, White);
    ssd1306_DrawPixel(x+1, y+4, White);
    ssd1306_DrawPixel(x+5, y+4, White);
    ssd1306_DrawPixel(x+2, y+5, White);
    ssd1306_DrawPixel(x+4, y+5, White);
    ssd1306_DrawPixel(x+3, y+6, White);
}
void drawArrow(uint8_t x, uint8_t y) {
    ssd1306_DrawPixel(x, y+2, White);
    ssd1306_DrawPixel(x+1, y+2, White);
    ssd1306_DrawPixel(x+2, y+2, White);
    ssd1306_DrawPixel(x+3, y+2, White);
    ssd1306_DrawPixel(x+4, y+2, White);
    ssd1306_DrawPixel(x+5, y+2, White);
    ssd1306_DrawPixel(x+6, y+2, White);
    ssd1306_DrawPixel(x+5, y+1, White);
    ssd1306_DrawPixel(x+5, y+3, White);
    ssd1306_DrawPixel(x+4, y,   White);
    ssd1306_DrawPixel(x+4, y+4, White);
}
const unsigned char epd_bitmap_flappybird1 []  = {
	0xe7, 0xff, 0xf9, 0xff, 0xe7, 0x7f, 0xef, 0xff, 0xff, 0xff, 0xff, 0x81, 0xfe, 0x00, 0xbe, 0x01, 
	0xdf, 0x03, 0xf9, 0xff
};

const unsigned char epd_bitmap_flappybird2[] = {
	 0x00, 0x00, 0x00, 0x00, 0x1f, 0x80, 0x32, 0x40, 0x72, 0xa0, 0x8a, 0x20, 0x89, 0xe0, 0x73, 0xf0, 
	0x1b, 0xe0, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00
};
const unsigned char epd_bitmap_flappybird3 []  = {
	0x01, 0xc0, 0x0f, 0xf0, 0x0c, 0x10, 0x10, 0x70, 0x7f, 0xf0, 0x3c, 0xa8, 0xfe, 0xf8, 0x14, 0xb8, 
	0x10, 0x38, 0x0f, 0xf8
};
const unsigned char epd_bitmap_flappybird []  = {
	0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xf8, 0x07, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xf1, 0x83, 0xff, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xf3, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
	0xff, 0xff, 0xf3, 0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xf3, 0xfc, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xf1, 0xfe, 0x7f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xf9, 0xfe, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
	0xff, 0xff, 0xf9, 0xff, 0x3c, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xf8, 0xff, 
	0x98, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xfc, 0xff, 0x81, 0xc7, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xfc, 0xff, 0xc8, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 
	0xff, 0xff, 0xfc, 0x7f, 0xcf, 0xfc, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xfe, 0x7f, 
	0xe7, 0xff, 0x01, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xe0, 0x3f, 0xe7, 0xff, 0xe0, 0x07, 
	0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xe0, 0x3f, 0xe7, 0xff, 0x81, 0x80, 0xff, 0xff, 0xff, 0xfe, 
	0xff, 0xff, 0xce, 0x1f, 0xf3, 0xfe, 0x3f, 0xfc, 0x7f, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xcf, 0x1f, 
	0xf3, 0xfc, 0x7f, 0xff, 0x1f, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xcf, 0x8f, 0xfb, 0xf9, 0xff, 0xff, 
	0xcf, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xcf, 0xcf, 0xfb, 0xf3, 0xff, 0xff, 0xe7, 0xff, 0xff, 0xfe, 
	0xff, 0xff, 0xcf, 0xe7, 0xf9, 0xe7, 0xff, 0xff, 0xf3, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xcf, 0xf3, 
	0xfd, 0xe7, 0xff, 0xff, 0xf9, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xcf, 0xfb, 0xfd, 0xcf, 0xff, 0xff, 
	0xf9, 0xff, 0xff, 0xfe, 0xff, 0xff, 0x87, 0xff, 0xfd, 0xcf, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfe, 
	0xff, 0xff, 0x07, 0xff, 0xfc, 0xdf, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfe, 0xff, 0xff, 0x33, 0xff, 
	0xfe, 0xdf, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xfe, 0xff, 0xfe, 0x7b, 0xff, 0xfe, 0xdf, 0xff, 0xfe, 
	0xfe, 0xff, 0xff, 0xfe, 0xff, 0xfc, 0xf9, 0xff, 0xfe, 0xdf, 0xff, 0xfc, 0xfe, 0xff, 0xff, 0xfe, 
	0xff, 0xf9, 0xfc, 0xff, 0xff, 0x5f, 0xff, 0xfc, 0x7e, 0xff, 0xff, 0xfe, 0xff, 0xf9, 0xf0, 0x7f, 
	0xff, 0x5f, 0xff, 0xfc, 0x3c, 0xff, 0xff, 0xfe, 0xff, 0xf3, 0xe7, 0xff, 0xff, 0x4f, 0xff, 0xfe, 
	0x7c, 0xff, 0xff, 0xfe, 0xff, 0xf7, 0xef, 0xff, 0xff, 0x4f, 0xff, 0xfe, 0xfc, 0xff, 0xff, 0xfe, 
	0xf8, 0x27, 0xe7, 0xff, 0xff, 0x67, 0xff, 0xff, 0x80, 0x00, 0x01, 0xfe, 0xf3, 0x0f, 0xf7, 0xff, 
	0xff, 0xe7, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x3e, 0xe7, 0xef, 0xfb, 0xff, 0xff, 0xf3, 0xfe, 0x00, 
	0x01, 0xff, 0x00, 0x1e, 0xe7, 0xff, 0xf9, 0xff, 0xff, 0xf9, 0xf0, 0x00, 0x3e, 0x00, 0x04, 0x0e, 
	0xe7, 0xff, 0xfe, 0xff, 0xff, 0xfc, 0xc0, 0x01, 0xc0, 0x00, 0x00, 0x86, 0xe7, 0xff, 0xff, 0x7f, 
	0xff, 0xfe, 0x00, 0x06, 0x00, 0x00, 0x00, 0x46, 0xe7, 0xff, 0xff, 0x9f, 0xff, 0xfe, 0x00, 0x0f, 
	0xff, 0xff, 0xfc, 0x02, 0xe7, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 
	0xf7, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xf3, 0xff, 0xff, 0xff, 
	0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x06, 0xf3, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 
	0xf9, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1e, 0xf9, 0xff, 0xff, 0xff, 
	0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xfe, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 
	0xfe, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 0xff, 0x3f, 0xff, 0xff, 
	0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x63, 0xfe, 0xff, 0x3f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xfe, 0xff, 0x9f, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 
	0xff, 0xcf, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xff, 0xe7, 0xff, 0xff, 
	0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x03, 0xfe, 0xff, 0xf3, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0xfe, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x07, 0xfe, 
	0xff, 0xfc, 0x7f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfe, 0xff, 0xff, 0x1f, 0xff, 
	0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x1f, 0xfe, 0xff, 0xff, 0x87, 0xff, 0xff, 0xff, 0xe0, 0x00, 
	0x00, 0x00, 0x3f, 0xfe, 0xff, 0xff, 0xe0, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xff, 0xfe, 
	0xff, 0xff, 0xf8, 0x1f, 0xff, 0xff, 0x07, 0xf0, 0x00, 0x0f, 0xff, 0xfe, 0xff, 0xff, 0xff, 0x01, 
	0xff, 0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 0x80, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe
};




void ssd1306_DrawBitmapTransparent( const unsigned char *bitmap, uint8_t w, uint8_t h, int16_t x0, int16_t y0, int invert)
{
    for (uint8_t y = 0; y < h; y++) {
        for (uint8_t x = 0; x < w; x++) {
            uint16_t byteIndex = x / 8 + y * ((w + 7) / 8);
            uint8_t bitMask = 0x80 >> (x % 8);
            int pixel = bitmap[byteIndex] & bitMask;

            if (invert ? !pixel : pixel) {
                ssd1306_DrawPixel(x0 + x, y0 + y, White);
            }
        }
    }
}


void Game_Draw(void)
{
    ssd1306_Fill(Black);

    if (flag_gameState == GAME_WAIT_START) {
        ssd1306_SetCursor(10, 25);
        ssd1306_WriteString("PRESS BTN", Font_11x18, White);
        ssd1306_SetCursor(25, 50);
        ssd1306_WriteString("TO START", Font_7x10, White);
    }
		else if (flag_gameState == GAME_PLATFORM) {
    int bmp_width = 95;
    int bmp_height = 64;

    int x = (SSD1306_WIDTH - bmp_width) / 2;  
    int y = (SSD1306_HEIGHT - bmp_height) / 2; 

   ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird, 95, 64, x, y, 1);
}

    else if (flag_gameState == GAME_PLAYING) {
				ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird1, 16, 10, bird.x, bird.y, 1);
				if (selected_bird == 0) {
					ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird1, 16, 10, bird.x, bird.y, 1);
				} else if (selected_bird == 1) {
				ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird2, 12, 12, bird.x, bird.y, 0);
				} else if (selected_bird == 2) {
				ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird3, 13, 10, bird.x, bird.y, 0);
			}

				for (int i = 0; i < num_pipes; i++) {
						int16_t px = pipes[i].x;
							if (px > -10 && px < SCREEN_WIDTH + 10) { 
								drawPipe(px, pipes[i].gap_y, pipes[i].gap_height);
							}
				}

				int cup_x = 2;
				int cup_y = 0;
				drawCup(cup_x, cup_y);

				char cup_text[8];
				sprintf(cup_text, "x%d", cups);
				ssd1306_SetCursor(cup_x + 12, cup_y); 
				ssd1306_WriteString(cup_text, Font_7x10, White);
 
        char buf[10];
				sprintf(buf, "%d", score);

				int text_width = strlen(buf) * 7;  
				int center_x = (SCREEN_WIDTH - text_width) / 2;
				int y_top = 0;
				ssd1306_SetCursor(center_x, y_top);
				ssd1306_WriteString(buf, Font_7x10, White);   
				ssd1306_SetCursor(92, 0);
        if (game_difficulty == DIFFICULTY_EASY) ssd1306_WriteString("E", Font_7x10, White);
        else if (game_difficulty == DIFFICULTY_MEDIUM) ssd1306_WriteString("M", Font_7x10, White);
        else ssd1306_WriteString("H", Font_7x10, White);
    }

    else if (flag_gameState == GAME_OVER) {
        ssd1306_SetCursor(20, 25);
        ssd1306_WriteString("GAME OVER", Font_11x18, White);
        ssd1306_SetCursor(25, 50);
        ssd1306_WriteString("Press Btn", Font_7x10, White);
    }
		else if (flag_gameState == GAME_HOME) {

    ssd1306_SetCursor(25, 2);
    ssd1306_WriteString("FLAPPY BIRD", Font_7x10, White);

    for (int i = 0; i < 4; i++) {  
        int idx = home_view_start + i;
        if (idx >= HOME_ITEM_COUNT) break;

        int y = 18 + i * 12;  
				int text_width = strlen(HOME_ITEMS[idx]) * 6;  
        int center_x = (128 - text_width) / 2;
        if (idx == home_option) {
            drawArrow(18, y);
            ssd1306_DrawFilledRectangle(center_x - 5, y - 2, center_x + text_width + 5, y + 8, White);

            ssd1306_SetCursor(center_x, y);
            ssd1306_WriteString((char*)HOME_ITEMS[idx], Font_6x8, Black);

        } else {
            ssd1306_SetCursor(center_x, y);
            ssd1306_WriteString((char*)HOME_ITEMS[idx], Font_6x8, White);
        }
    }
}

		else if (flag_gameState == GAME_MENU) {

    ssd1306_SetCursor(25, 5);
    ssd1306_WriteString("SELECT LEVEL", Font_7x10, White);

    const char* labels[3] = {"Easy", "Medium", "Hard"};
    int y_start = 20;

    for (int i = 0; i < 3; i++) {

        int y = y_start + i * 15;

        if (menu_option == i) {
            drawArrow(10, y);  
            ssd1306_DrawFilledRectangle(30, y - 2, 110, y + 10, White);
            ssd1306_SetCursor(45, y);
            ssd1306_WriteString((char*)labels[i], Font_7x10, Black);
        } 
        else {
            ssd1306_SetCursor(45, y);
            ssd1306_WriteString((char*)labels[i], Font_7x10, White);
        }
    }
}

		else if (flag_gameState == GAME_HISTORY) {
						ssd1306_SetCursor(40, 5);
						ssd1306_WriteString("HISTORY", Font_7x10, White);
					if (history_count == 0) {
						ssd1306_SetCursor(25, 30);
						ssd1306_WriteString("No records yet", Font_6x8, White);
					} 
					else {
						for (int i = 0; i < history_count; i++) {
            const char *diffName;
						
            switch (history[i].difficulty) {
                case DIFFICULTY_EASY:   diffName = "Easy"; break;
                case DIFFICULTY_MEDIUM: diffName = "Medium";  break;
                case DIFFICULTY_HARD:   diffName = "Hard"; break;
                default:                diffName = "?";    break;
            }
						
            char line[20];
            sprintf(line, "%d. %d (%s) ", i + 1, history[i].score, diffName);

  
            int y = 20 + i * 10; 
            ssd1306_SetCursor(32, y);
            ssd1306_WriteString(line, Font_6x8, White);
        }
    }

}
	else if (flag_gameState == GAME_BIRD_SELECT) {
    ssd1306_SetCursor(20, 12);
    ssd1306_WriteString("SELECT BIRD", Font_7x10, White);

  
    char cup_text[12];
    sprintf(cup_text, "Cups: %d", cups);
    ssd1306_SetCursor(2, 0);
    ssd1306_WriteString(cup_text, Font_6x8, White);

    for (int i = 0; i < 3; i++) {
        int x = 20 + i * 35;
        int y = 30;
        int cx = x + 8;
        int cy = y + 8;

  
        int unlocked = 0;
        if (i == 0) unlocked = 1;
        else if (i == 1 && cups >= cup_unlock_thresholds[1]) unlocked = 1;
        else if (i == 2 && cups >= cup_unlock_thresholds[2]) unlocked = 1;

        if (i == selected_bird) ssd1306_DrawRectangle(x-2, y-2, x+18, y+18, White);
        else ssd1306_DrawFilledRectangle(x-2, y-2, x+18, y+18, Black);
			
        SSD1306_COLOR bird_color = (i == selected_bird) ? Black : White;

       
        if (i == 0)
        ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird1, 16, 10, x, y, 1);
				else if (i == 1)
        ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird2, 12, 12, x+2, y-1, 0);
				else if (i == 2)
        ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird3, 13, 10, x+1, y, 0);

    
        if (!unlocked) {
            ssd1306_SetCursor(x+3, y+12);
            char buf[8];
            sprintf(buf, "L%d", cup_unlock_thresholds[i]);
            ssd1306_WriteString(buf, Font_6x8, White);
        }
    }

    ssd1306_SetCursor(10, 55);
    ssd1306_WriteString("Press BTN to select", Font_6x8, White);
}


    ssd1306_UpdateScreen();
		
}


void Game_Init(void) {
 
	  next_reward_score = 5;
		bird.x = 20;
    bird.y = SCREEN_HEIGHT / 2;
    bird.vy = 0;
    int gap_between = 48;

    for (int i = 0; i < num_pipes; i++) {
        pipes[i].x = SCREEN_WIDTH + i * gap_between;
        pipes[i].gap_y = 10 + rand() % (SCREEN_HEIGHT - 35);
        pipes[i].gap_height = 23 + rand() % 12;
        pipes[i].passed = 0;
    }

    for (int i = num_pipes; i < MAX_PIPES; i++) {
        pipes[i].x = SCREEN_WIDTH + 200;
        pipes[i].passed = 1;
    }

    score = 0;
    gameOver = 0;
    ssd1306_Fill(Black);
}

void SaveScoreToHistory(void) {
    if (history_count < 5) {
        history[history_count].score = score;
        history[history_count].difficulty = game_difficulty;
        history_count++;
    } else {
        for (int k = 0; k < 4; k++) {
            history[k] = history[k + 1];
        }
        history[4].score = score;
        history[4].difficulty = game_difficulty;
    }
}


void Game_Update(void) {
    if (gameOver) return;

    int pipe_speed = (game_difficulty == DIFFICULTY_HARD) ? 4 :
                     (game_difficulty == DIFFICULTY_MEDIUM) ? 3 : 2;

    bird.vy += 2;
    bird.y += bird.vy;

    for (int i = 0; i < num_pipes; i++) {
        pipes[i].x -= pipe_speed;

        if (!pipes[i].passed && pipes[i].x + 12 < bird.x && pipes[i].x > -20) {
            pipes[i].passed = 1;
            score++;
							if (score >= next_reward_score) {
								cups++;                   
								next_reward_score += 5;  
			}
        }

        if (pipes[i].x < -20) {
            int16_t max_x = pipes[0].x;
            for (int j = 1; j < num_pipes; j++)
                if (pipes[j].x > max_x) max_x = pipes[j].x;

            pipes[i].x = max_x + 60;
            pipes[i].gap_height = 20 + rand() % 12;   // random l?i gap_height m?i
						pipes[i].gap_y = 8 + rand() % (SCREEN_HEIGHT - pipes[i].gap_height - 8);

            pipes[i].passed = 0;
					}
				}
				if (bird.y <= 0 || bird.y >= SCREEN_HEIGHT - 3) {
					gameOver = 1;
					SaveScoreToHistory();
					flag_gameState = GAME_OVER;
				return;
				}
		for (int i = 0; i < num_pipes; i++) {
			int16_t px = pipes[i].x;
			if (bird.x + 3 >= px && bird.x <= px + 12) {
        if (bird.y <= pipes[i].gap_y || bird.y >= pipes[i].gap_y + pipes[i].gap_height) {
            gameOver = 1;
            SaveScoreToHistory(); 
            flag_gameState = GAME_OVER;
            return;
        }
    }
	}
}

void Bird_Jump(void) {
    if (!gameOver) {
        bird.vy = -6;
    } else {
        Game_Init();
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static uint32_t last_press = 0;
    if (HAL_GetTick() - last_press < 150) return;
    last_press = HAL_GetTick();
		
    if (GPIO_Pin == GPIO_PIN_3) {
        post_event(EVT_BTN_PRESS); 
    } 
    else if (GPIO_Pin == GPIO_PIN_13) {
				if (flag_gameState == GAME_HOME) {
						if (home_option == 0) flag_gameState = GAME_WAIT_START;
						else if (home_option == 1) flag_gameState = GAME_MENU;
						else if (home_option == 2) flag_gameState = GAME_HISTORY;
						else if (home_option == 3) flag_gameState = GAME_BIRD_SELECT;
						else if (home_option == 4) flag_gameState = GAME_PLATFORM; // EXIT
				}
				post_event(EVT_DRAW_FRAME);
    }
    else if (flag_gameState == GAME_MENU) {
      
        if (menu_option == 0) {
            game_difficulty = DIFFICULTY_EASY;
            num_pipes = 2;
        } else if (menu_option == 1) {
            game_difficulty = DIFFICULTY_MEDIUM;
            num_pipes = 2;
        } else if (menu_option == 2) {
            game_difficulty = DIFFICULTY_HARD;
            num_pipes = 3;
        }
        Game_Init();
        flag_gameState = GAME_HOME;
        post_event(EVT_DRAW_FRAME);
    }
		else if (flag_gameState == GAME_BIRD_SELECT) {
    
    for (int k = 0; k < 3; k++) {
        uint8_t next = (selected_bird ) % 3;
        int unlocked = 0;
        if (next == 0) unlocked = 1;
        else if (next == 1 && cups >= cup_unlock_thresholds[1]) unlocked = 1;
        else if (next == 2 && cups >= cup_unlock_thresholds[2]) unlocked = 1;

        selected_bird = next;
			flag_gameState = GAME_HOME;
        if (unlocked) break;
        
    }
    post_event(EVT_DRAW_FRAME);
		}

		else if (GPIO_Pin == GPIO_PIN_4) {
        
        flag_gameState = GAME_HOME;
        Game_Init();                 
        post_event(EVT_DRAW_FRAME);  
    }
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    SoftI2C_Init();
    ssd1306_Init();

    Game_Init();
    flag_gameState = GAME_PLATFORM;
    post_event(EVT_DRAW_FRAME);

    uint32_t lastTick = HAL_GetTick();

    while (1) {
        if (HAL_GetTick() - lastTick >= 30) {
            post_event(EVT_TIMER_TICK);
            lastTick = HAL_GetTick();
        }
        EventMsg e;
        while (get_event(&e)) {
            switch (e.type) {
               case EVT_BTN_PRESS:
										if (flag_gameState == GAME_PLATFORM) {
											post_event(EVT_DRAW_FRAME);
										}
										else if (flag_gameState == GAME_HOME) {

										home_option = (home_option + 1) % HOME_ITEM_COUNT;   // 5 m?c

    
										if (home_option >= home_view_start + 3) {
										home_view_start++;
										}
										if (home_view_start + 3 > HOME_ITEM_COUNT) {
										home_view_start = HOME_ITEM_COUNT - 3;
										}
									if (home_option < home_view_start) {
										home_view_start = home_option;
										}
									post_event(EVT_DRAW_FRAME);
}

										else if (flag_gameState == GAME_MENU) {
											menu_option = (menu_option + 1) % 3;
											post_event(EVT_DRAW_FRAME);
										}
										else if (flag_gameState == GAME_WAIT_START) {
   
										int unlocked = 0;
										if (selected_bird == 0) unlocked = 1;
										else if (selected_bird == 1 && cups >= cup_unlock_thresholds[1]) unlocked = 1;
										else if (selected_bird == 2 && cups >= cup_unlock_thresholds[2]) unlocked = 1;
										if (!unlocked) selected_bird = 0;

										flag_gameState = GAME_PLAYING;
										post_event(EVT_DRAW_FRAME);
}

										else if (flag_gameState == GAME_PLAYING) {
											Bird_Jump();
											post_event(EVT_DRAW_FRAME);
										}
										else if (flag_gameState == GAME_OVER) {
											flag_gameState = GAME_HOME;
											post_event(EVT_DRAW_FRAME);
										}
										else if (flag_gameState == GAME_HISTORY) {
											flag_gameState = GAME_HOME;
											post_event(EVT_DRAW_FRAME);
										}
										else if (flag_gameState == GAME_BIRD_SELECT) {
											selected_bird = (selected_bird + 1) % 3; 
											post_event(EVT_DRAW_FRAME);
										}
																			
							break;

						case EVT_TIMER_TICK:
                    if (flag_gameState == GAME_PLAYING) {
                        Game_Update();
                        post_event(EVT_DRAW_FRAME);
                    }
             break;

						case EVT_DRAW_FRAME:
                    Game_Draw();
                    break;

                default: break;
            }
        }
    }
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL4;
  RCC_OscInitStruct.PLL.PLLDIV = RCC_PLL_DIV3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */




