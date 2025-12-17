#include "AO_game.h"
#include "game.h"
#include "signal_handle.h"


void GameAO_Init(void)
{
    Game_Init();
    flag_gameState = GAME_HOME;
}
void GameAO_Handle(EventMsg *e)
{
    switch(flag_gameState)
    {
        case GAME_HOME:
            if(e->type == EVT_BTN_MENU)
            {
                switch(home_option)
                {
                    case 0: flag_gameState = GAME_WAIT_START; break;
                    case 1: flag_gameState = GAME_MENU; break;
                    case 2: flag_gameState = GAME_HISTORY; break;
                    case 3: flag_gameState = GAME_BIRD_SELECT; break;
                    case 4: flag_gameState = GAME_RANK; break;
                    case 5: flag_gameState = GAME_PLATFORM; break;
                }
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            else if(e->type == EVT_BTN_NEXT)
            {
                home_option = (home_option + 1) % HOME_ITEM_COUNT;
                if (home_option >= home_view_start + 3)
                    home_view_start = home_option - 2;
                else if (home_option < home_view_start)
                    home_view_start = home_option;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_WAIT_START:
            if(e->type == EVT_BTN_MENU)
            {
                flag_gameState = GAME_PLAYING;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_PLAYING:
            if(e->type == EVT_BTN_NEXT)
            {
                Bird_Jump();
            }
            else if(e->type == EVT_TIMER_TICK)
            {
                Game_Update();
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_MENU:
            if(e->type == EVT_BTN_MENU)
            {
                if(menu_index == MENU_DIFFICULTY)
                {
                    flag_gameState = GAME_MENU_DIFFICULTY_SELECT;
                    selected_option = game_difficulty;
                }
                else if(menu_index == MENU_GRAVITY)
                {
                    flag_gameState = GAME_MENU_GRAVITY_SELECT;
                    selected_option = selected_gravity;
                }
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            else if(e->type == EVT_BTN_NEXT)
            {
                menu_index = (menu_index + 1) % 2;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_MENU_DIFFICULTY_SELECT:
            if(e->type == EVT_BTN_MENU)
            {
                game_difficulty = (Difficulty)selected_option;
                flag_gameState = GAME_MENU;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            else if(e->type == EVT_BTN_NEXT)
            {
                selected_option = (selected_option + 1) % 3;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_MENU_GRAVITY_SELECT:
            if(e->type == EVT_BTN_MENU)
            {
                selected_gravity = (GravityLevel)selected_option;
                flag_gameState = GAME_MENU;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            else if(e->type == EVT_BTN_NEXT)
            {
                selected_option = (selected_option + 1) % 3;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_BIRD_SELECT:
            if(e->type == EVT_BTN_NEXT)
            {
                selected_bird = (selected_bird + 1) % 3;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            else if(e->type == EVT_BTN_MENU)
            {
                if(selected_bird > 0 && cups < cup_unlock_thresholds[selected_bird])
                    selected_bird = 0;
                flag_gameState = GAME_HOME;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_OVER:
            if(e->type == EVT_BTN_MENU)
            {
                Signal_StopAll();
                flag_gameState = GAME_HOME;
                Game_Init();
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        case GAME_HISTORY:
        case GAME_RANK:
        case GAME_PLATFORM:
            if(e->type == EVT_BTN_MENU)
            {
                flag_gameState = GAME_HOME;
                EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            }
            break;

        default:
            break;
    }
}
