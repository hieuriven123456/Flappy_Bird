#include "game.h"
#include "config.h"
#include "draw_game.h"

const char* HOME_ITEMS[] = {
    "Start",
    "Menu",
    "History",
    "Bird Select",
		"Ranking",
    "Exit"
};
GameState flag_gameState = GAME_HOME;

//setup ban dau
Difficulty game_difficulty = DIFFICULTY_EASY;
GravityLevel selected_gravity = GRAVITY_EARTH;

int home_view_start = 0; 
int menu_index = 0;  

uint8_t menu_option = 0;
uint8_t home_option = 0;
uint8_t selected_bird = 0;
Bird bird;
Pipe pipes[MAX_PIPES];
ScoreHistory history[5];
uint8_t num_pipes = 1;
uint8_t score = 0;



int8_t last_scores[5] = {0};
uint8_t history_count = 0;
int8_t high_scores[4] = {0, 0, 0, 0};
uint8_t gameOver = 0;
uint8_t selected_option=0;
uint8_t cups = 0;        


// khe nopipe
int nopipe_mode = 0;
uint32_t nopipe_start_time = 0;
int nopipe_gap_y = 0;
int nopipe_gap_height = 30;   // Khe rong

// 5 diem thi dc 1 cup
uint8_t next_reward_score = 5; 

// do kho game
const int cup_unlock_thresholds[] = {0, 5, 10};



void Pipes_ResetNormal() {
    int start_x = 90;  
    for (int i = 0; i < num_pipes; i++) {
        pipes[i].x = start_x + i * 70;
        pipes[i].gap_height = 20 + rand() % 12;
        pipes[i].gap_y = 8 + rand() % (SCREEN_HEIGHT - pipes[i].gap_height - 8);
        pipes[i].passed = 0;
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
			
if (nopipe_mode) {

  
    if (selected_bird == 0)
        ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird1, 16, 10, bird.x, bird.y, 1);
    else if (selected_bird == 1)
        ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird2, 12, 12, bird.x, bird.y, 0);
    else
        ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird3, 13, 10, bird.x, bird.y, 0);

    drawZigZagTop(nopipe_gap_y);
		drawZigZagBottom(nopipe_gap_y + nopipe_gap_height);


  
    char buf[12];
    sprintf(buf, "%d", score);
    int text_width = strlen(buf) * 7;
    int cx = (SCREEN_WIDTH - text_width) / 2;
    ssd1306_SetCursor(cx, 0);
    ssd1306_WriteString(buf, Font_7x10, White);

   
    ssd1306_UpdateScreen();
    return;
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

    for (int i = 0; i < 5; i++) {  
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
			Game_Menu_Main_Draw();
}
		else if(flag_gameState == GAME_MENU_DIFFICULTY_SELECT)
{
    const char* diff_options[3] = {"EASY", "MEDIUM", "HARD"};
    Game_DrawOptionMenu("SELECT DIFFICULTY", diff_options, 3, selected_option);
}
else if(flag_gameState == GAME_MENU_GRAVITY_SELECT)
{
    const char* grav_options[3] = {"MARS", "EARTH", "JUPITER"};
    Game_DrawOptionMenu("SELECT GRAVITY", grav_options, 3, selected_option);
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
        

  
        int unlocked = 0;
        if (i == 0) unlocked = 1;
        else if (i == 1 && cups >= cup_unlock_thresholds[1]) unlocked = 1;
        else if (i == 2 && cups >= cup_unlock_thresholds[2]) unlocked = 1;

        if (i == selected_bird) ssd1306_DrawRectangle(x-2, y-2, x+18, y+18, White);
        else ssd1306_DrawFilledRectangle(x-2, y-2, x+18, y+18, Black);
			
      

       
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
	else if (flag_gameState == GAME_RANK) {
   
    const char *title = "RANKING";
    int title_len = strlen(title);
    int title_x = (128 - title_len * 7) / 2;  // Font_7x10: mo?i ký tu 7px
    ssd1306_SetCursor(title_x, 0);
    ssd1306_WriteString((char*)title, Font_7x10, White);

    
    for (int i = 0; i < 4; i++) {
        char buf[20];
        sprintf(buf, "Rank %d. %d pts", i + 1, high_scores[i]);
        int buf_len = strlen(buf);
        int buf_x = (128 - buf_len * 6) / 2;   // Font_6x8: moi ký tu 6px
        ssd1306_SetCursor(buf_x, 20 + i * 12);
        ssd1306_WriteString(buf, Font_6x8, White);
    }
}

    ssd1306_UpdateScreen();
		
}


void Game_Init(void) {
		  if (game_difficulty == DIFFICULTY_EASY) 
        num_pipes = 2;
    else if (game_difficulty == DIFFICULTY_MEDIUM)
        num_pipes = 2;
    else
        num_pipes = 3;
	  next_reward_score = 5;
		bird.x = 20;
    bird.y = SCREEN_HEIGHT / 2;
    bird.vy = 0;
    int gap_between = 45;

    for (int i = 0; i < num_pipes; i++) {
        pipes[i].x = SCREEN_WIDTH + i * gap_between;
        pipes[i].gap_y = 10 + rand() % (SCREEN_HEIGHT - 32);
        pipes[i].gap_height = 25 + rand() % 12;
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

void UpdateRanking(int newScore) {
    for (int i = 0; i < 4; i++) {
        if (newScore > high_scores[i]) {
            for (int j = 3; j > i; j--) {
                high_scores[j] = high_scores[j - 1];
            }
            high_scores[i] = newScore;
            break;
        }
    }
}
void Game_Update(void) {
    if (gameOver) return;
		if (nopipe_mode) {
    // kiem tra het 5 giây
				if (HAL_GetTick() - nopipe_start_time >= 3000) {
        nopipe_mode = 0;
        Pipes_ResetNormal(); // ch? reset ?ng
        return;
				}

    // cap nhat bird
				float gravity_force = (selected_gravity == GRAVITY_JUPITER) ? 3.0f :(selected_gravity == GRAVITY_MARS)  ? 1.5f : 2.0f;
				bird.vy += gravity_force;
				bird.y += bird.vy;

    // kiem tra va cham voi khe
				if (bird.y <= nopipe_gap_y || bird.y >= nopipe_gap_y + nopipe_gap_height) {
        gameOver = 1;
        SaveScoreToHistory();
        UpdateRanking(score);
        flag_gameState = GAME_OVER;
				Signal_SetMode(SIGNAL_MODE_GAME_OVER);
        return;
			}
		//

		return; 
		}
    int pipe_speed = (game_difficulty == DIFFICULTY_HARD) ? 4 :(game_difficulty == DIFFICULTY_MEDIUM) ? 3 : 2;

    float gravity_force = (selected_gravity == GRAVITY_JUPITER) ? 3.0f :(selected_gravity == GRAVITY_MARS)  ? 1.5f : 2.0f;
		int bird_width = (selected_bird == 0) ? 16 :(selected_bird == 1) ? 12 : 13;
		int bird_height = (selected_bird == 0) ? 10 :(selected_bird == 1) ? 12 : 10;
    bird.vy += gravity_force;
    bird.y += bird.vy;
    for (int i = 0; i < num_pipes; i++) {
        pipes[i].x -= pipe_speed;

    // kiem tra vuot ong de tang diem
    if (!pipes[i].passed && pipes[i].x + 12 < bird.x && pipes[i].x > -20) {
         pipes[i].passed = 1;
         score++;
            if (score >= next_reward_score) {
                cups++;
                next_reward_score += 5;
            }
						if (score > 0 && score % 10 == 0 && !nopipe_mode) {
        nopipe_mode = 1;
        nopipe_start_time = HAL_GetTick();
        nopipe_gap_y = 10 + rand() % (SCREEN_HEIGHT - nopipe_gap_height - 10);
    }
        }
    // reset ong khi di het man hinh
		if (pipes[i].x < -20) {
		int16_t max_x = pipes[0].x;
    for (int j = 1; j < num_pipes; j++)
					if (pipes[j].x > max_x) max_x = pipes[j].x;
					int gap_between = 70;
					pipes[i].x = max_x + gap_between;
			
				int base_gap =
						(game_difficulty == DIFFICULTY_EASY) ? 28 : (game_difficulty == DIFFICULTY_MEDIUM) ? 26 :24; // HARD
				int random_range = (game_difficulty == DIFFICULTY_EASY)? 8 :(game_difficulty == DIFFICULTY_MEDIUM) ? 7 : 6;
				pipes[i].gap_height = base_gap + rand() % random_range;

	
    // Gioi han khong cho qua be
				if (pipes[i].gap_height < 12) pipes[i].gap_height = 12;

    // Random vi tri Y
				pipes[i].gap_y = 8 + rand() % (SCREEN_HEIGHT - pipes[i].gap_height - 8);
				pipes[i].passed = 0;
		}
	// kiem tra va cham voi ong
				if (bird.x + bird_width >= pipes[i].x && bird.x <= pipes[i].x + 12) {
					if (bird.y <= pipes[i].gap_y || bird.y + bird_height -5 >= pipes[i].gap_y + pipes[i].gap_height) {
                gameOver = 1;
                SaveScoreToHistory();
                UpdateRanking(score);
                flag_gameState = GAME_OVER;
								Signal_SetMode(SIGNAL_MODE_GAME_OVER);
                return;
            }
        }
   }
    // kiem tra roi thoat ra ngoai man 
    if (bird.y <= 0 || bird.y >= SCREEN_HEIGHT - 3) {
        gameOver = 1;
        SaveScoreToHistory();
        UpdateRanking(score);
        flag_gameState = GAME_OVER;
				Signal_SetMode(SIGNAL_MODE_GAME_OVER);
        return;
    }
}

void Bird_Jump(void) {
    if (!gameOver) {
        bird.vy = -6;
    } else {
        Game_Init();
    }
}
