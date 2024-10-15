
#include "lcd.h"
#include "types.h"
#include "dino-game.h"
#include "joystick.h"
#include <stdlib.h>

#include "dino-game.h"
#include <stdlib.h>

// Function to ensure obstacles are at least a minimum distance apart
void generate_obstacles(Obstacles *obstacles) {
    obstacles->position_col[0] = 16 + (rand() % 16);
    for (int i = 1; i < MAX_OBSTACLES; i++) {
        obstacles->position_col[i] = obstacles->position_col[i - 1] + MIN_OBSTACLE_DISTANCE;
        obstacles->position_row[i] = rand() % 2; // Randomly position the obstacle in row 0 or 1
    }
}

void game_init(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles) {
    Lcd_init(lcd);

    set_dino_char(lcd); // Set custom characters

    dino->position_row = 0;
    dino->position_col = 0;
    dino->jump = false;
    dino->down = false;

    generate_obstacles(obstacles); // Generate initial obstacles with distance constraints
}

void game_update(uint32_t *VR, Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles) {
    // Handle joystick input
    if (joystick_up(VR) == 1) {
        dino->jump = true;
        dino->down = false;
        dino->position_row = 0;
    } else if (joystick_down(VR) == 1) {
        dino->jump = false;
        dino->down = true;
        dino->position_row = 1;
    } else if (joystick_stable(VR) == 1) {
        dino->jump = false;
        dino->down = false;
        dino->position_row = 1;
    }

    // Move obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles->position_col[i]--;
        if (obstacles->position_col[i] < 0) {
            // Reposition the obstacle with a minimum distance
            obstacles->position_row[i] = rand() % 2; // Randomly position the obstacle in row 0 or 1
            if (i == 0) {
                obstacles->position_col[i] = obstacles->position_col[MAX_OBSTACLES - 1] + MIN_OBSTACLE_DISTANCE + (rand() % 16);
            } else {
                obstacles->position_col[i] = obstacles->position_col[i - 1] + MIN_OBSTACLE_DISTANCE + (rand() % 16);
            }
        }
    }
}

void game_draw(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles) {
    Lcd_clear(lcd);

    // Draw sun
    Lcd_cursor(lcd, 0, 15);
    Lcd_write_data(lcd, 0); // Sun

    // Draw Dino
    if (!dino->jump && !dino->down && dino->position_row == 1) {
        Lcd_cursor(lcd, dino->position_row, dino->position_col);
        Lcd_string(lcd, "\x01"); // dino_run1
    } else if (dino->jump && dino->position_row == 0) {
        Lcd_cursor(lcd, dino->position_row, dino->position_col);
        Lcd_string(lcd, "\x02"); // dino_jump
    } else if (dino->down && dino->position_row == 1) {
        Lcd_cursor(lcd, dino->position_row, dino->position_col);
        Lcd_string(lcd, "\x03"); // dino_down1
    }


    // Draw obstacles
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (obstacles->position_col[i] < 16) { // If obstacle is on screen
            Lcd_cursor(lcd, obstacles->position_row[i], obstacles->position_col[i]);
            if (obstacles->position_row[i] == 1) {
                Lcd_string(lcd, "\x05"); // cactus
            } else if (obstacles->position_row[i] == 0) {
                switch (rand() % 2) {
                    case 0:
                        Lcd_string(lcd, "\x06"); // bird_top
                        break;
                    case 1:
                        Lcd_string(lcd, "\x07"); // bird_bottom
                        break;
                }
            }
        }
    }
}




int check_collision(Dino *dino, Obstacles *obstacles) {
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        if (dino->position_row == obstacles->position_row[i] && obstacles->position_col[i] == 0) {
            return 1; // Collisione rilevata
        }
    }
    return 0; // Nessuna collisione
}



void set_dino_char(Lcd_HandleTypeDef *lcd) {
    uint8_t sun[8] = {
        0x00, 0x00, 0x1C, 0x1F, 0x1F, 0x1F, 0x1C, 0x00
    };
    uint8_t dino_run1[8] = {
        0x07, 0x05, 0x07, 0x04, 0x17, 0x1E, 0x0E, 0x1B
    };
    uint8_t dino_run2[8] = {
        0x07, 0x05, 0x07, 0x04, 0x17, 0x1E, 0x0F, 0x0C
    };
    uint8_t cactus[8] = {
        0x04, 0x04, 0x05, 0x15, 0x17, 0x1C, 0x04, 0x04
    };
    uint8_t dino_duck1[8] = {
        0x00, 0x00, 0x03, 0x05, 0x17, 0x1E, 0x0F, 0x08
    };
    uint8_t dino_duck2[8] = {
        0x00, 0x00, 0x03, 0x05, 0x17, 0x1E, 0x0E, 0x12
    };
    uint8_t bird_top[8] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x0E, 0x1C
    };
    uint8_t bird_bottom[8] = {
        0x1C, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    Lcd_define_char(lcd, 0, sun);
    Lcd_define_char(lcd, 1, dino_run1);
    Lcd_define_char(lcd, 2, dino_run2);
    Lcd_define_char(lcd, 3, dino_duck1);
    Lcd_define_char(lcd, 4, dino_duck2);
    Lcd_define_char(lcd, 5, cactus);
    Lcd_define_char(lcd, 6, bird_top);
    Lcd_define_char(lcd, 7, bird_bottom);
}

