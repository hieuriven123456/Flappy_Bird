#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   64
#define MAX_PIPES 3
#define HOME_ITEM_COUNT 6


typedef enum {
    GAME_HOME = 0,
		GAME_PLATFORM,
    GAME_MENU,
    GAME_WAIT_START,
    GAME_PLAYING,
		GAME_MENU_DIFFICULTY_SELECT,
    GAME_MENU_GRAVITY_SELECT,
    GAME_OVER,
		GAME_HISTORY,
		GAME_RANK,
		GAME_BIRD_SELECT
} GameState;
extern GameState flag_gameState;
typedef enum {
    GRAVITY_MARS = 0,
    GRAVITY_EARTH,
    GRAVITY_JUPITER
} GravityLevel;

typedef enum {
    DIFFICULTY_EASY = 0,
    DIFFICULTY_MEDIUM,
    DIFFICULTY_HARD
} Difficulty;
typedef enum {
    MENU_DIFFICULTY = 0,
    MENU_GRAVITY
} MenuItem;






#endif
