#include "button_handle.h"



void Button_Handle(uint16_t GPIO_Pin)
{
    static uint32_t last_press = 0;
    if (HAL_GetTick() - last_press < 150) return;
    last_press = HAL_GetTick();

    
    if (GPIO_Pin == GPIO_PIN_3)
    {
        if (flag_gameState == GAME_BIRD_SELECT)
        {
            selected_bird = (selected_bird + 1) % 3;
            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }

        if (flag_gameState == GAME_MENU)
        {
            menu_index = (menu_index + 1) % MENU_MAX;
            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }

        if (flag_gameState == GAME_MENU_DIFFICULTY_SELECT)
        {
            selected_option = (selected_option + 1) % 3;  // EASY/MED/HARD
            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }

        if (flag_gameState == GAME_MENU_GRAVITY_SELECT)
        {
            selected_option = (selected_option + 1) % 3;  // LOW/NORMAL/HIGH
            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }

        /* Gameplay button */
        EventRB_Post(&gEventRB, EVT_BTN_PRESS);
        return;
    }

    else if (GPIO_Pin == GPIO_PIN_13)
    {
        if (flag_gameState == GAME_HOME)
        {
            if (home_option == 0) flag_gameState = GAME_WAIT_START;
            else if (home_option == 1) flag_gameState = GAME_MENU;
            else if (home_option == 2) flag_gameState = GAME_HISTORY;
            else if (home_option == 3) flag_gameState = GAME_BIRD_SELECT;
            else if (home_option == 4) flag_gameState = GAME_RANK;
            else if (home_option == 5) flag_gameState = GAME_PLATFORM;

            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }

        
        if (flag_gameState == GAME_BIRD_SELECT)
        {
            int unlocked = 0;
            if (selected_bird == 0) unlocked = 1;
            else if (selected_bird == 1 && cups >= cup_unlock_thresholds[1]) unlocked = 1;
            else if (selected_bird == 2 && cups >= cup_unlock_thresholds[2]) unlocked = 1;

            if (!unlocked) selected_bird = 0;

            flag_gameState = GAME_HOME;
            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }

        if (flag_gameState == GAME_MENU)
        {
            if (menu_index == MENU_DIFFICULTY)
            {
                flag_gameState = GAME_MENU_DIFFICULTY_SELECT;
                selected_option = game_difficulty;   // 0..2
            }
            else if (menu_index == MENU_GRAVITY)
            {
                flag_gameState = GAME_MENU_GRAVITY_SELECT;
                selected_option = selected_gravity;  // 0..2
            }

            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }

       if (flag_gameState == GAME_MENU_DIFFICULTY_SELECT)
        {
            game_difficulty = selected_option;
            flag_gameState = GAME_MENU;
            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }
        if (flag_gameState == GAME_MENU_GRAVITY_SELECT)
        {
            selected_gravity = selected_option;
            flag_gameState = GAME_MENU;
            EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
            return;
        }
    }

    else if (GPIO_Pin == GPIO_PIN_4)
    {
        flag_gameState = GAME_HOME;
        Game_Init();
        EventRB_Post(&gEventRB, EVT_DRAW_FRAME);
        return;
    }
}