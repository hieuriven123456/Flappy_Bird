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

    switch (flag_gameState) {
        case GAME_WAIT_START:
            Game_DrawWaitStart();
            break;

        case GAME_PLATFORM:
            Game_DrawPlatform();
            break;

        case GAME_PLAYING:
            Game_Draw_Bird();
            if (nopipe_mode) {
                Draw_Nopipe_game();
                ssd1306_UpdateScreen();
                return;
            }
            Draw_Game_Normal();
            break;

        case GAME_OVER:
            Game_DrawGameOver();
            break;

        case GAME_HOME:
            Draw_Game_Home();
            break;

        case GAME_MENU:
            Game_Menu_Main_Draw();
            break;

        case GAME_MENU_DIFFICULTY_SELECT:
            Game_Menu_Diff();
            break;

        case GAME_MENU_GRAVITY_SELECT:
            Game_Menu_Gravity();
            break;

        case GAME_HISTORY:
            Draw_History();
            break;

        case GAME_BIRD_SELECT:
            Draw_Bird_selected();
            break;

        case GAME_RANK:
            Draw_Rank();
            break;
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
    // kiem tra het 3 giây
				if (HAL_GetTick() - nopipe_start_time >= 3000) {
        nopipe_mode = 0;
        Pipes_ResetNormal(); 
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
