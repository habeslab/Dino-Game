// dino-game.h
#ifndef DINO_GAME_H // Include guard to prevent multiple inclusions of this header file
#define DINO_GAME_H

#include "stm32f4xx_hal.h" // Include necessary HAL library for STM32
#include "lcd.h"          // Include the LCD display library
#include "types.h"        // Include custom type definitions

/**
 * @brief Initializes the game by setting up the LCD, Dino, and Obstacles.
 *
 * This function prepares the game environment by initializing
 * the LCD display and setting the initial positions and states
 * of the Dino and obstacles.
 *
 * @param lcd Pointer to the Lcd_HandleTypeDef structure for the LCD.
 * @param dino Pointer to the Dino structure that holds the character's state.
 * @param obstacles Pointer to the Obstacles structure that holds the positions of obstacles.
 */
void game_init(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles);

/**
 * @brief Updates the game state based on joystick input.
 *
 * This function processes the joystick inputs to update the positions
 * and states of the Dino and obstacles in the game. It handles actions
 * like jumping and ducking.
 *
 * @param VR Pointer to an array containing joystick values.
 * @param lcd Pointer to the Lcd_HandleTypeDef structure for the LCD.
 * @param dino Pointer to the Dino structure for the character's state.
 * @param obstacles Pointer to the Obstacles structure for obstacle positions.
 */
void game_update(uint32_t *VR, Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles);

/**
 * @brief Draws the current state of the game on the LCD display.
 *
 * This function renders the Dino and obstacles on the LCD screen
 * based on their current positions and states, updating the visual
 * representation of the game.
 *
 * @param lcd Pointer to the Lcd_HandleTypeDef structure for the LCD.
 * @param dino Pointer to the Dino structure for the character's state.
 * @param obstacles Pointer to the Obstacles structure for obstacle positions.
 */
void game_draw(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles);

/**
 * @brief Checks for collision between the Dino and obstacles.
 *
 * This function determines if the Dino has collided with any obstacles
 * in the game. It returns 1 if a collision is detected, otherwise 0.
 *
 * @param dino Pointer to the Dino structure for the character's state.
 * @param obstacles Pointer to the Obstacles structure for obstacle positions.
 * @return 1 if a collision is detected; 0 otherwise.
 */
int check_collision(Dino *dino, Obstacles *obstacles);

/**
 * @brief Sets the character representation of the Dino on the LCD display.
 *
 * This function defines how the Dino character is visually represented
 * on the LCD screen by setting the appropriate character map.
 *
 * @param lcd Pointer to the Lcd_HandleTypeDef structure for the LCD.
 */
void set_dino_char(Lcd_HandleTypeDef *lcd);

#endif // DINO_GAME_H
