/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lcd.h"
#include "types.h"
#include "stm32f4xx_hal.h"


// Definizione delle variabili globali
ADC_HandleTypeDef hadc1;

DMA_HandleTypeDef hdma_adc1;

UART_HandleTypeDef huart2;

Lcd_HandleTypeDef lcd;

uint32_t VR[2];  // Buffer per i valori ADC


/*---------------------------------------------------------------------------- Sytem Configuration Function --------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);

void game_init(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles);
void game_update(Lcd_HandleTypeDef *lcd,Dino *dino, Obstacles *obstacles);
void game_draw(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles);
int check_collision(Dino *dino, Obstacles *obstacles);

void set_dino_char(Lcd_HandleTypeDef *lcd);

void serial_print(const char *format, ...);

void joystick_init(ADC_HandleTypeDef *hadc1, uint32_t *VR);
int joystick_up(uint32_t *VR);
int joystick_down(uint32_t *VR);
int joystick_stable(uint32_t *VR);


int score = 0;


int main(void) {
    // HAL configuration
    HAL_Init();

    // System Clock Configuration
    SystemClock_Config();

    // GPIO configuration
    MX_GPIO_Init();

    // Init DMA - Configuration
    MX_DMA_Init();

    // Init ADC ( Analog - to - Digital Conversion )
    MX_ADC1_Init();

    // Setup USART Configuration
    MX_USART2_UART_Init();

    // Configurazione delle porte e dei pin del display LCD
    Lcd_PortType ports[] = { GPIOC, GPIOB, GPIOA, GPIOA };
    Lcd_PinType pins[] = { GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_6 };
    lcd = Lcd_create(ports, pins, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, LCD_4_BIT_MODE);

    // Inizializzazione del display LCD
    Lcd_init(&lcd);

    // Inizializzazione ADC con DMA
    HAL_ADC_Start_DMA(&hadc1, VR, 2);

    // Mostra il messaggio di benvenuto
    Lcd_clear(&lcd);
    Lcd_cursor(&lcd, 0, 1);
    Lcd_string(&lcd, " Welcome in ");
    Lcd_cursor(&lcd, 1, 1);
    Lcd_string(&lcd, " Dino-Game ");
    HAL_Delay(2000); // Mostra il messaggio di benvenuto per 2 secondi
    Lcd_clear(&lcd);

    // Inizializza il gioco
    Dino dino;
    Obstacles obstacles;
    game_init(&lcd, &dino, &obstacles);

    // Ciclo principale del gioco
    while (1) {

    	joystick_init(&hadc1, VR);

    	int joy_Y = VR[1];
    	serial_print("Attuale valore di joy_Y : %d\n",joy_Y);
    	// Aggiorna lo stato del gioco
        game_update(&lcd, &dino, &obstacles);
        game_draw(&lcd, &dino, &obstacles);

        // Controlla se c'è una collisione
        if (check_collision(&dino, &obstacles)==1) {
            Lcd_clear(&lcd);
            Lcd_cursor(&lcd, 0, 0);
            Lcd_string(&lcd, "Game Over!");

            char score_str[16];
            sprintf(score_str, "Score: %d", score);
            Lcd_cursor(&lcd, 1, 0);
            Lcd_string(&lcd, score_str);

            while (1) {
                HAL_Delay(1000); // Mantiene la schermata di Game Over
            }
        }

        score += 1; // Incrementa il punteggio

        // Difficoltà del gioco - diminuzione del ritardo per aumentare la velocità
        int dino_difficulty = 200;
        int max_dino_difficulty = 30;

        HAL_Delay(dino_difficulty);
        if (dino_difficulty > max_dino_difficulty) {
            dino_difficulty -= 2;
        }
    }

    return 0;
}

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



void game_update(Lcd_HandleTypeDef *lcd, Dino *dino, Obstacles *obstacles) {
    // Gestisci input dal joystick
    if (joystick_up(VR) == 1) {

    	serial_print("Sono qui! Dopo il Joystick_up\n");
        dino->jump = true;
        dino->down = false;
        dino->position_row = 0;
    }
    else if (joystick_down(VR) == 1) {

    	serial_print("Sono qui! Dopo il Joystick_down\n");
        dino->jump = false;
        dino->down = true;
        dino->position_row = 1;

    } else if(joystick_stable(VR) == 1 ){

    	serial_print("Sono qui! Dopo il Joystick_stable\n");
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
            if (obstacles->position_col[i] < 16) { // Cambiato a 16 per includere la colonna 15
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


void serial_print(const char *format, ...) {
    char buffer[100]; // Buffer per la stringa formattata
    va_list args;     // Lista dei parametri variabili

    // Inizializza la lista dei parametri
    va_start(args, format);
    // Format the string using vsnprintf to safely handle variable arguments
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args); // Chiude la lista dei parametri

    // Invia il messaggio tramite UART
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, strlen(buffer), HAL_MAX_DELAY);
}



void joystick_init(ADC_HandleTypeDef *hadc1, uint32_t *VR) {
    // Inizializza il joystick (se necessario)
    // La configurazione dell'ADC e del DMA dovrebbe essere già fatta nel main.c
    HAL_ADC_Start_DMA(hadc1, VR, 2);
}


int joystick_up(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_Y = VR[1];
    //serial_print("Sono qui nel joystick-up() , value : %d\n",joy_Y);

    // Soglia per rilevare il movimento verso l'alto (da calibrare)
    int threshold_up = 500;

    // Ritorna 1 se il joystick è spostato verso l'alto, altrimenti 0
    return (joy_Y < threshold_up) ? 1 : 0;
}


int joystick_down(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_Y = VR[1];
    //serial_print("Sono qui nel joystick-down() , value : %d\n",joy_Y);

    // Soglia per rilevare il movimento verso il basso (da calibrare)
    return (joy_Y > 3200) ? 1 : 0;
}

int joystick_stable(uint32_t *VR) {
    // Leggi il valore del joystick dall'asse Y
    int joy_Y = VR[1];
    //serial_print("Sono qui nel joystick-down() , value : %d\n",joy_Y);

    return (joy_Y>=3000 && joy_Y<=3200) ? 1: 0;
}


/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */

	/** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	hadc1.Init.ScanConvMode = ENABLE;
	hadc1.Init.ContinuousConvMode = ENABLE;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.NbrOfConversion = 2;
	hadc1.Init.DMAContinuousRequests = DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_10;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}

	/** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	 */
	sConfig.Channel = ADC_CHANNEL_11;
	sConfig.Rank = 2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA2_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA2_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LD2_Pin|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LD2_Pin PA6 PA7 */
	GPIO_InitStruct.Pin = LD2_Pin|GPIO_PIN_6|GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : PC7 */
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PB4 PB5 PB6 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
