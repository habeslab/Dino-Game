// utils.h
#ifndef UTILS_H // Include guard to prevent multiple inclusions of this header file
#define UTILS_H

#include <stdio.h>         // Standard input/output definitions
#include <stdarg.h>       // Definitions for variable arguments
#include "stm32f4xx_hal.h" // Include necessary HAL library for STM32

/**
 * @brief Sends formatted data over UART.
 *
 * This function takes a format string and variable arguments to create
 * a formatted string, which is then transmitted via the specified UART
 * interface. It is useful for debugging or sending information to a
 * connected device.
 *
 * @param huart2 Pointer to the UART_HandleTypeDef structure that contains
 *                the configuration information for the specified UART module.
 * @param format The format string that specifies how to format the output.
 * @param ... Variable arguments to format the output according to the format string.
 */
void serial_print(UART_HandleTypeDef *huart2, const char *format, ...);

#endif // UTILS_H
