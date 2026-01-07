#include "draw_game.h"
#include "config.h"
#include "game.h"

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
// ve ong
void drawPipe(int x, int gap_y, int gap_height) {
    int pipe_width = 12;


    ssd1306_DrawFilledRectangle(x, 0, x + pipe_width, gap_y, White);

    ssd1306_DrawFilledRectangle(x - 1, gap_y - 3, x + pipe_width + 1, gap_y, White);

  
    ssd1306_DrawFilledRectangle(x, gap_y + gap_height, x + pipe_width, SCREEN_HEIGHT, White);
   
    ssd1306_DrawFilledRectangle(x - 1, gap_y + gap_height, x + pipe_width + 1, gap_y + gap_height + 3, White);

  
    ssd1306_DrawFilledRectangle(x + 2, 0, x + pipe_width - 2, gap_y - 3, Black);
    ssd1306_DrawFilledRectangle(x + 2, gap_y + gap_height + 3, x + pipe_width - 2, SCREEN_HEIGHT, Black);
}
// ve 
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
// ve mui ten 
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
// ve
void drawZigZagBottom(int y_start) {
    int zig_height = 4;
    int zig_width  = 6;

    for (int x = 0; x < SCREEN_WIDTH; x += zig_width*2) {
        // Khoi len
        ssd1306_DrawFilledRectangle(x, y_start, x + zig_width, SCREEN_HEIGHT, White);

        // Rang cua nho phia tren
        ssd1306_DrawFilledRectangle(x + zig_width, y_start,
                                    x + zig_width + zig_width, y_start + zig_height,
                                    White);
    }
}
void drawZigZagTop(int y_end) {
    int zig_height = 4;          // do cao rang cua
    int zig_width  = 6;          // do rong moi rang

    for (int x = 0; x < SCREEN_WIDTH; x += zig_width*2) {
        // Khoi xuong
        ssd1306_DrawFilledRectangle(x, 0, x + zig_width, y_end, White);

        // Rang cua nho phia duoi
        ssd1306_DrawFilledRectangle(x + zig_width, y_end - zig_height,
                                    x + zig_width + zig_width, y_end, White);
    }
}

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

void Game_DrawOptionMenu(const char* title, const char* options[], int num_options, int selected)
{
    ssd1306_Fill(Black);

    // Title
    int title_len = strlen(title);
    int title_x = (128 - title_len * 7) / 2; // Font_7x10
    ssd1306_SetCursor(title_x, 0);
    ssd1306_WriteString((char*)title, Font_7x10, White);

    // Options
    for (int i = 0; i < num_options; i++)
    {
        int y = 20 + i*16;
				
        // Highlight option dang chon
        if (i == selected)
            ssd1306_DrawFilledRectangle(0, y-2, 128, y+12, White); // nen trang
        else
            ssd1306_DrawRectangle(0, y-2, 128, y+12, White);        // khung trang

        
        ssd1306_SetCursor(SCREEN_WIDTH/2 - 16, y);
        if (i == selected)
            ssd1306_WriteString((char*)options[i], Font_7x10, Black); 
        else
            ssd1306_WriteString((char*)options[i], Font_7x10, White);
    }

    // Huong dan
    ssd1306_SetCursor(0, 60);
    ssd1306_WriteString("Press BTN to select", Font_6x8, White);

    ssd1306_UpdateScreen();
}
void Game_Menu_Main_Draw()
{
    ssd1306_Fill(Black);
    // Title
    const char* title = "OPTIONS";
    int title_x = (128 - strlen(title) * 7) / 2;
    ssd1306_SetCursor(title_x, 0);
    ssd1306_WriteString((char*)title, Font_7x10, White);

    // Menu Items: Difficulty và Gravity
    const char* menu_items[2] = {"PLAY", "Gravity"};
    for (int i = 0; i < 2; i++)
    {
        int y = 20 + i*16; // v? trí t?ng m?c

        // Highlight neu dang chon
        if (menu_index == i)
        {
            ssd1306_DrawFilledRectangle(0, y-2, 127, y+12, White); // n?n tr?ng
            ssd1306_SetCursor(5, y);
            ssd1306_WriteString((char*)menu_items[i], Font_7x10, Black); // ch? den
        }
        else
        {
            ssd1306_DrawRectangle(0, y-2, 127, y+12, White); // khung tr?ng
            ssd1306_SetCursor(5, y);
            ssd1306_WriteString((char*)menu_items[i], Font_7x10, White);
        }

        // Hien thi giá tri hien tai ben phai
        char buf[16];
        if (i == 0) // Difficulty
        {
            if (game_difficulty == DIFFICULTY_EASY) snprintf(buf, sizeof(buf), "EASY");
            else if (game_difficulty == DIFFICULTY_MEDIUM) snprintf(buf, sizeof(buf), "MEDIUM");
            else snprintf(buf, sizeof(buf), "HARD");
        }
        else if (i == 1) // Gravity
        {
            if (selected_gravity == GRAVITY_MARS) snprintf(buf, sizeof(buf), "MARS");
            else if (selected_gravity == GRAVITY_EARTH) snprintf(buf, sizeof(buf), "EARTH");
            else snprintf(buf, sizeof(buf), "JUPITER");
        }

        int right_x0 = 76;     // bat dau cot phai
				int right_x1 = 127;    // ket thúc cot phai
				int right_width = right_x1 - right_x0;

				int text_w = strlen(buf) * 7;   // moi ky tu 7px (Font_7x10)
				int cx = right_x0 + (right_width - text_w) / 2;

				ssd1306_SetCursor(cx, y);
				ssd1306_WriteString(buf, Font_7x10, (menu_index == i) ? Black : White);

    }

    // huong dan
    ssd1306_SetCursor(16, 52);
    ssd1306_WriteString("-> BTN13: Select", Font_6x8, White);

    ssd1306_UpdateScreen();
}

void Game_DrawGameOver(void)
{
    ssd1306_SetCursor(20, 25);
    ssd1306_WriteString("GAME OVER", Font_11x18, White);

    ssd1306_SetCursor(25, 50);
    ssd1306_WriteString("PRESS BTN", Font_7x10, White);
}
void Game_DrawWaitStart(void)
{
    ssd1306_SetCursor(10, 25);
    ssd1306_WriteString("PRESS BTN", Font_11x18, White);

    ssd1306_SetCursor(25, 50);
    ssd1306_WriteString("TO START", Font_7x10, White);
}
void Game_DrawPlatform(void)
{
				int bmp_width = 95;
				int bmp_height = 64;
				int x = (SSD1306_WIDTH - bmp_width) / 2;  
				int y = (SSD1306_HEIGHT - bmp_height) / 2; 
				ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird, 95, 64, x, y, 1);
}
void Game_Draw_Bird(void){
	ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird1, 16, 10, bird.x, bird.y, 1);
				if (selected_bird == 0) {
					ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird1, 16, 10, bird.x, bird.y, 1);
				} else if (selected_bird == 1) {
				ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird2, 12, 12, bird.x, bird.y, 0);
				} else if (selected_bird == 2) {
				ssd1306_DrawBitmapTransparent(epd_bitmap_flappybird3, 13, 10, bird.x, bird.y, 0);
			}
}
void Game_Menu_Diff(void)
{
	const char* diff_options[3] = {"EASY", "MEDIUM", "HARD"};
    Game_DrawOptionMenu("SELECT DIFFICULTY", diff_options, 3, selected_option);
}
void Game_Menu_Gravity(void)
{
		 const char* grav_options[3] = {"MARS", "EARTH", "JUPITER"};
    Game_DrawOptionMenu("SELECT GRAVITY", grav_options, 3, selected_option);
}
void Draw_Rank(void)
	{
	const char *title = "RANKING";
    int title_len = strlen(title);
    int title_x = (128 - title_len * 7) / 2;  // Font_7x10: moi ky tu 7px
    ssd1306_SetCursor(title_x, 0);
    ssd1306_WriteString((char*)title, Font_7x10, White);

    
    for (int i = 0; i < 4; i++) {
        char buf[20];
        sprintf(buf, "Rank %d. %d pts", i + 1, high_scores[i]);
        int buf_len = strlen(buf);
        int buf_x = (128 - buf_len * 6) / 2;   // Font_6x8: moi ky tu 6px
        ssd1306_SetCursor(buf_x, 20 + i * 12);
        ssd1306_WriteString(buf, Font_6x8, White);
    }
}

void Draw_Game_Home(void)
{
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

void Draw_Bird_selected(void)
{
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
void Draw_History(void)
{
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

void Draw_Nopipe_game(void)
{
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
}

void Draw_Game_Normal(void)
{
	
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



