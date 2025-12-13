#include "button_handle.h"

/* Debounce cho tat ca cac nut */

uint16_t last_button_pin = 0;

void Button_Handle(uint16_t GPIO_Pin)
{
    static uint32_t last_press = 0;
    if (HAL_GetTick() - last_press < 150) return;
    last_press = HAL_GetTick();

    last_button_pin = GPIO_Pin;

    EventRB_Post(&gEventRB, EVT_BTN_PRESS);
}



void HandleButtonPress(void)
{
    switch(flag_gameState)
    {
        case GAME_HOME:
            switch(home_option)
            {
                case 0: flag_gameState = GAME_WAIT_START; break;
                case 1: flag_gameState = GAME_MENU; break;
                case 2: flag_gameState = GAME_HISTORY; break;
                case 3: flag_gameState = GAME_BIRD_SELECT; break;
                case 4: flag_gameState = GAME_RANK; break;
                case 5: flag_gameState = GAME_PLATFORM; break;
            }
            break;

        case GAME_BIRD_SELECT:
            if(selected_bird > 0 && cups < cup_unlock_thresholds[selected_bird])
                selected_bird = 0;
            flag_gameState = GAME_HOME;
            break;

        case GAME_MENU:
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
            break;

        case GAME_MENU_DIFFICULTY_SELECT:
            game_difficulty = (Difficulty)selected_option;
            flag_gameState = GAME_MENU;
            break;

        case GAME_MENU_GRAVITY_SELECT:
            selected_gravity = (GravityLevel)selected_option;
            flag_gameState = GAME_MENU;
            break;

        case GAME_WAIT_START:
            flag_gameState = GAME_PLAYING;
            break;

        case GAME_PLAYING:
							Bird_Jump();
            break;

        case GAME_OVER:
            Signal_StopAll();
            flag_gameState = GAME_HOME;
            break;

        case GAME_HISTORY:
            flag_gameState = GAME_HOME;
            break;

        default:
            break;
    }

    EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
}
void HandleJumpOrNextOption(void)
{
    switch(flag_gameState)
    {
        

        case GAME_HOME:
            
            home_option = (home_option + 1) % HOME_ITEM_COUNT;

           
            if (home_option >= home_view_start + 3)
                home_view_start = home_option - 2;
            else if (home_option < home_view_start)
                home_view_start = home_option;

            break;

        case GAME_BIRD_SELECT:
        
            selected_bird = (selected_bird + 1) % 3;
            break;

        case GAME_MENU:
            menu_index = (menu_index + 1) % 2; 
            break;

        case GAME_MENU_DIFFICULTY_SELECT:
        case GAME_MENU_GRAVITY_SELECT:
          
            selected_option = (selected_option + 1) % 3; 
            break;

        default:
        
            break;
    }

    
    EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
}
