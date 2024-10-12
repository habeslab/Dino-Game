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
#include <stdint.h>
#include "lcd.h"
#include "stm32f4xx_hal.h"













// Definizione delle variabili globali

ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;
UART_HandleTypeDef huart2;

Lcd_HandleTypeDef lcd;

uint32_t VR[2];  // Buffer per i valori ADC

volatile uint32_t joy_X = 0;
volatile uint32_t joy_Y = 0;

volatile bool joy_SW = false;

int joy_SW_pin = 0; // Pin digitale connesso al pulsante del joystick

int joy_X_pin = 0;  // Pin analogico connesso all'output X del joystick

int joy_Y_pin = 0;  // Pin analogico connesso all'output Y del joystick






/*---------------------------------------------------------------------------- Sytem Configuration Function --------------------------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);


void read_joystick();
void dino_game();
void draw_dino_background();
void dino_generate_obstacles();
void set_dino_char();

// Mat is position of the LCD display
int mat[2][16] = {
		{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8},
		{8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8}
};

// Score of the players
int score = 0;

// Variables that indicates the objects on the LCD display
const int dino_object_max = 4;
// Object that appear on the LCD display
int dino_object_count = 0;
// Cactus apperaed on the LCD display
int dino_cactus_appeared = 0;
// Bird Appeared on the LCD Display
int dino_bird_appeared = 0;

// Dino Life
bool dino_dead = false;

const int dino_jump_duration = 4; // Number of frame that dino stay up with jump

// Count JUMP time
int dino_jump_time = 0; // Dino is down when the value is 0

uint8_t animation_tick = 0; // Temporary value for the animation




int main(void) {

	// HAL configuration
	HAL_Init();

	// System Clock Configuration
	SystemClock_Config();

	// GPIO configuration
	MX_GPIO_Init();

	// Init DMA - Configuration
	MX_DMA_Init();

	// Init ADC ( Analog - to - Digital Conversion
	MX_ADC1_Init();

	// Setup USART Configuration
	MX_USART2_UART_Init();


	// Lcd_PortType ports[] = { D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port };
	Lcd_PortType ports[] = { GPIOC, GPIOB, GPIOA, GPIOA };
	// Lcd_PinType pins[] = {D4_Pin, D5_Pin, D6_Pin, D7_Pin};
	Lcd_PinType pins[] = {GPIO_PIN_7, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_6};
	// Lcd_create(ports, pins, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin, LCD_4_BIT_MODE);
	lcd = Lcd_create(ports, pins, GPIOB, GPIO_PIN_5, GPIOB, GPIO_PIN_4, LCD_4_BIT_MODE);
	// Init LCD Port
	Lcd_init(&lcd);

	// HAL configuration ADC in DMA
	HAL_ADC_Start_DMA(&hadc1, VR, 2);

	// DinoGame
	set_dino_char();
	// Setting - Cursor
	Lcd_cursor(&lcd, 0, 1);
	// Printing on LCD
	Lcd_string(&lcd, " Welcome in ");

	Lcd_cursor(&lcd, 1, 1);

	Lcd_string(&lcd, " Dino-Game ");

	HAL_Delay(600);
	// Clear display LCD
	Lcd_clear(&lcd);


	while (1) {

		// Difficulty of the game - Lower or Higher Speed
		int dino_difficulty = 200;

		int max_dino_difficulty = 30;

		// Start Dino-GAME
		dino_game();

		score += 1;

		if (dino_dead) {

			Lcd_clear(&lcd);

			// Setting cursor on LCD display
			Lcd_cursor(&lcd, 1, 1);
			// Write Data
			Lcd_data(&lcd, 1);
			HAL_Delay(100);

			Lcd_clear(&lcd);
			HAL_Delay(100);

			// Setting the cursor
			Lcd_cursor(&lcd, 1, 1);
			Lcd_data(&lcd, 1);
			HAL_Delay(100);

			Lcd_clear(&lcd);
			HAL_Delay(100);

			Lcd_cursor(&lcd, 1, 1);
			Lcd_data(&lcd, 1);
			HAL_Delay(100);


			Lcd_clear(&lcd);

			char score_str[16];
			// Debug - Printing Score
			sprintf(score_str, "Score: %d", score);
			// Printing Score on LCD display
			Lcd_string(&lcd, score_str);

			while (1) {
				HAL_Delay(1000);
			}
		}

		HAL_Delay(dino_difficulty);

		if (dino_difficulty > max_dino_difficulty) {
			dino_difficulty -= 2;
		}
	}
}

void read_joystick() {
	// I valori dell'ADC vengono aggiornati dal DMA, quindi possiamo leggerli direttamente

    HAL_ADC_Start_DMA(&hadc1, VR, 2);
	joy_X = VR[0];
	joy_Y = VR[1];
	joy_SW = HAL_GPIO_ReadPin(GPIOA, joy_SW_pin);

}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == hadc1.Instance) {
		// Trasferimento completato, i valori in VR[0] e VR[1] sono aggiornati
		// Potresti voler eseguire ulteriori operazioni qui, se necessario
	}
}


void dino_game() {
	// Read Joystick
	read_joystick();

	bool btnDown = (joy_X < 300);
	bool btnUp = (joy_X > 700);

	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 15; j++) {
			mat[i][j] = mat[i][j + 1];
		}
	}

	if (mat[1][0] == 5 || mat[1][0] == 7) {
		dino_object_count -= 1;
	}
	mat[0][15] = 8;
	mat[1][15] = 8;

	// Generate Obstacles on LCD display
	dino_generate_obstacles();
	// Draw dino BackGround
	draw_dino_background();

	if (btnDown == false && dino_jump_time == 0) {
		// Set cursor position on the second row
		Lcd_cursor(&lcd, 1, 1);
		if (mat[1][1] == 7 || mat[1][1] == 5) {
			dino_dead = true;
		}
		// write Lcd data on display
		Lcd_data(&lcd, 1);
	}
	if (btnDown == false && dino_jump_time != 0) {

		Lcd_cursor(&lcd, 1, 0);

		if (mat[1][0] == 6) {
			dino_dead = true;
		}

		Lcd_data(&lcd, 2);
	}
	if (btnUp == true && dino_jump_time == 0) {
		dino_jump_time = dino_jump_duration;
	}
	if (btnDown == true) {

		Lcd_cursor(&lcd, 1, 1);

		if (mat[1][1] == 5) {
			dino_dead = true;
		}

		Lcd_data(&lcd, 3); // Adjust to write the custom character
		dino_jump_time = 0;
	}

	Lcd_cursor(&lcd, 15, 0);

	Lcd_data(&lcd, 0);

	dino_jump_time = (dino_jump_time > 0) ? dino_jump_time - 1 : 0;
	animation_tick = (animation_tick == 0) ? 1 : 0;
}

void draw_dino_background() {
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 16; j++) {
			Lcd_cursor(&lcd, i, j);
			if (mat[i][j] == 8) {
				Lcd_string(&lcd, " ");
			} else {
				Lcd_data(&lcd, mat[i][j]);
			}
		}
	}
}

void dino_generate_obstacles() {

	bool cactus_spawn = false;
	bool bird_spawn = false;
	if (dino_cactus_appeared >= 3 && dino_bird_appeared >= 3) {
		cactus_spawn = true;
	}
	if (dino_cactus_appeared >= 3 && dino_bird_appeared >= 3) {
		bird_spawn = true;
	}

	int DinoRandNumber = rand() % 10 + 1;

	if (DinoRandNumber < 4 && dino_object_count < dino_object_max) {

		if (bird_spawn && cactus_spawn) {

			int randint = rand() % 10 + 1;

			if (randint < 5) {
				mat[0][15] = 6;
				mat[1][15] = 7;
				dino_bird_appeared = -1;
			} else {
				mat[1][15] = 5;
				dino_cactus_appeared = -1;
			}
			dino_object_count += 1;
		}

		if (bird_spawn && !cactus_spawn) {
			mat[0][15] = 6;
			mat[1][15] = 7;
			dino_bird_appeared = -1;
			dino_object_count += 1;
		}

		if (!bird_spawn && cactus_spawn) {
			mat[1][15] = 5;
			dino_cactus_appeared = -1;
			dino_object_count += 1;
		}
	}
	dino_bird_appeared += 1;
	dino_cactus_appeared += 1;
}

void set_dino_char() {
	// array di caratteri personalizzati su LCD
	uint8_t sun[8] = {
			0x00,
			0x00,
			0x1C,
			0x1F,
			0x1F,
			0x1F,
			0x1C,
			0x00
	};
	uint8_t dino_run1[8] = {
			0x07,
			0x05,
			0x07,
			0x04,
			0x17,
			0x1E,
			0x0E,
			0x1B
	};
	uint8_t dino_run2[8] = {
			0x07,
			0x05,
			0x07,
			0x04,
			0x17,
			0x1E,
			0x0F,
			0x0C
	};
	uint8_t cactus[8] = {
			0x04,
			0x04,
			0x05,
			0x15,
			0x17,
			0x1C,
			0x04,
			0x04
	};
	uint8_t dino_duck1[8] = {
			0x00,
			0x00,
			0x03,
			0x05,
			0x17,
			0x1E,
			0x0F,
			0x08
	};
	uint8_t dino_duck2[8] = {
			0x00,
			0x00,
			0x03,
			0x05,
			0x17,
			0x1E,
			0x0E,
			0x12
	};
	uint8_t bird_top[8] = {
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x07,
			0x0E,
			0x1C
	};
	uint8_t bird_bottom[8] = {
			0x1C,
			0x0E,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00,
			0x00
	};
	// Define the character on the screen
	// Printing LCD display image
	Lcd_define_char(&lcd, 0, sun);
	Lcd_define_char(&lcd, 1, dino_run1);
	Lcd_define_char(&lcd, 2, dino_run2);
	Lcd_define_char(&lcd, 3, dino_duck1);
	Lcd_define_char(&lcd, 4, dino_duck2);
	Lcd_define_char(&lcd, 5, cactus);
	Lcd_define_char(&lcd, 6, bird_top);
	Lcd_define_char(&lcd, 7, bird_bottom);

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
