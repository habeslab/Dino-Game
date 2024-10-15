
#include "lcd.h"
#include "types.h"
#include "dino-game.h"
#include "joystick.h"
#include <stdlib.h>

void game_init(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles) {
    Lcd_init(lcd);

    set_dino_char(lcd); // Imposta i caratteri personalizzati

    dino->position_row = 0;
    dino->position_col = 0;
    dino->jump = false;
    dino->down = false;

    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles->position_row[i] = rand() % 2; // Genera ostacoli casualmente tra row 0 e row 1
        obstacles->position_col[i] = 16 + (rand() % 32); // Genera ostacoli in posizioni casuali
    }
}



void game_update(uint32_t *VR,Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles) {
    // Gestisci input dal joystick
    if (joystick_up(VR) == 1) {


        dino->jump = true;
        dino->down = false;
        dino->position_row = 0;
    }
    else if (joystick_down(VR) == 1) {

        dino->jump = false;
        dino->down = true;
        dino->position_row = 1;

    } else if(joystick_stable(VR) == 1 ){

        dino->jump = false;
        dino->down = false;
        dino->position_row = 1;
    }

    // Muovi gli ostacoli
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        obstacles->position_col[i]--;
        if (obstacles->position_col[i] < 0) {
            // Riposiziona l'ostacolo
            obstacles->position_row[i] = rand() % 2; // Genera ostacoli casualmente tra row 0 e row 1
            obstacles->position_col[i] = 16 + (rand() % 32);
        }
    }
}



/**
 *
 */
void game_draw(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles) {

	Lcd_clear(lcd);

    // Draw sun
	Lcd_cursor(lcd, 0, 15);
	Lcd_write_data(lcd, 0); //sun


    // Draw Dino

    // CASE 1 : Normal Running
    if ( (dino->jump== false) && (dino->down==false) && dino->position_row == 1) {

        Lcd_cursor(lcd, dino->position_row, dino->position_col);
        Lcd_string(lcd, "\x01"); // dino_run1
    // CASE 2 : JUMP situation
    }else if( (dino->jump==true) && dino->position_row==0){
    	Lcd_cursor(lcd, dino->position_row, dino->position_col);
		Lcd_string(lcd, "\x02"); // dino_run1
    // CASE 3: DOWN situation
    }else if( (dino->down == true) && dino->position_row==1){

    	Lcd_cursor(lcd, dino->position_row, dino->position_col);
		Lcd_string(lcd, "\x03"); // dino_down1

    }

    // Disegna gli ostacoli
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            if (obstacles->position_col[i] < 15) { // Cambiato a 16 per includere la colonna 15
                Lcd_cursor(lcd, obstacles->position_row[i], obstacles->position_col[i]);

                // Genera ostacoli in base alla row
                if (obstacles->position_row[i] == 1) {
                    // Cactus devono essere nella row 0
                    Lcd_string(lcd, "\x05"); // cactus
                } else if (obstacles->position_row[i] == 0) {
                    // Gli uccelli devono essere nella row 1
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

