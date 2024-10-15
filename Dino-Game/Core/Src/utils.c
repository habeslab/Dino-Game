
#include "utils.h"
#include "stm32f4xx_hal.h"
#include <string.h>

void serial_print(UART_HandleTypeDef *huart2,const char *format, ...) {
    char buffer[100]; // Buffer per la stringa formattata
    va_list args;     // Lista dei parametri variabili

    // Inizializza la lista dei parametri
    va_start(args, format);
    // Format the string using vsnprintf to safely handle variable arguments
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args); // Chiude la lista dei parametri

    // Invia il messaggio tramite UART
    HAL_UART_Transmit(huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
}
