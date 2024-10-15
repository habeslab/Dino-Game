// joystick.h
#ifndef JOYSTICK_H // Include guard to prevent multiple inclusions of this header file
#define JOYSTICK_H

#include "stm32f4xx_hal.h" // Include necessary HAL library for STM32

/**
 * @brief Initializes the joystick by starting the ADC in DMA mode.
 *
 * This function configures the Analog-to-Digital Converter (ADC)
 * to read the joystick values through Direct Memory Access (DMA).
 *
 * @param hadc1 Pointer to the ADC_HandleTypeDef structure that contains
 *              the configuration information for the specified ADC.
 * @param VR Pointer to an array where the joystick values will be stored.
 */
void joystick_init(ADC_HandleTypeDef *hadc1, uint32_t *VR);

/**
 * @brief Checks if the joystick is moved upwards.
 *
 * This function reads the joystick's Y-axis value and compares it
 * against a defined threshold to determine if it is moved upward.
 *
 * @param VR Pointer to an array containing the joystick values.
 * @return 1 if the joystick is moved upwards; 0 otherwise.
 */
int joystick_up(uint32_t *VR);

/**
 * @brief Checks if the joystick is moved downwards.
 *
 * This function reads the joystick's X-axis value and compares it
 * against a defined threshold to determine if it is moved downwards.
 *
 * @param VR Pointer to an array containing the joystick values.
 * @return 1 if the joystick is moved downwards; 0 otherwise.
 */
int joystick_down(uint32_t *VR);

/**
 * @brief Checks if the joystick is in a stable position.
 *
 * This function reads the joystick's position and determines
 * whether it is in a neutral (stable) state, as defined by specific thresholds.
 *
 * @param VR Pointer to an array containing the joystick values.
 * @return 1 if the joystick is stable; 0 otherwise.
 */
int joystick_stable(uint32_t *VR);


int joystick_left(uint32_t *VR);

int joystick_right(uint32_t *VR);



#endif // JOYSTICK_H

