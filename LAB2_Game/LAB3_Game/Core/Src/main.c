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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;

UART_HandleTypeDef hlpuart1;

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
//SPI part
uint8_t SPIRx[10];
uint8_t SPITx[10];

//my game part
int game_winner=0;//0=no 1=ai 2=human
int game_turn=0;//0=player 1=ai
int game_left=5;//amount of lamp that has left
int game_pick=0;//amount of pick for LED

//light (not use anymore)
uint16_t LMode;//for light

//UART
uint8_t RxBuffer[27];
uint8_t TxBuffer[27];
int ReadyToPlay = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_LPUART1_UART_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_LPUART1_UART_Init();
  MX_SPI3_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim3); // start timer
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)"\r\nYou need to PICK 1 or 2 coins VS AI\r\n", strlen("\r\nYou need to PICK 1 or 2 coins VS AI\r\n"), HAL_MAX_DELAY);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Who pick the last coin Win!!\r\n", strlen("Who pick the last coin Win!!\r\n"), HAL_MAX_DELAY);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)"If you're ready please type\r\n", strlen("If you're ready please type\r\n"), HAL_MAX_DELAY);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)"wanna play this game so bad\r\n", strlen("wanna play this game so bad\r\n"), HAL_MAX_DELAY);
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)"and now you can join\r\n", strlen("and now you can join\r\n"), HAL_MAX_DELAY);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  	  AreYouReadyToPlay();
	      SPITxRx_readIO();
	      TikTokGame();
	      CoinLeftUART();
	      LEDdisplay();
  }
  /* USER CODE END 3 */
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x30A0A7FB;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief LPUART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_LPUART1_UART_Init(void)
{

  /* USER CODE BEGIN LPUART1_Init 0 */

  /* USER CODE END LPUART1_Init 0 */

  /* USER CODE BEGIN LPUART1_Init 1 */

  /* USER CODE END LPUART1_Init 1 */
  hlpuart1.Instance = LPUART1;
  hlpuart1.Init.BaudRate = 57600;
  hlpuart1.Init.WordLength = UART_WORDLENGTH_8B;
  hlpuart1.Init.StopBits = UART_STOPBITS_1;
  hlpuart1.Init.Parity = UART_PARITY_NONE;
  hlpuart1.Init.Mode = UART_MODE_TX_RX;
  hlpuart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  hlpuart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  hlpuart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  hlpuart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&hlpuart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&hlpuart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&hlpuart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN LPUART1_Init 2 */

  /* USER CODE END LPUART1_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 16999;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 99;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
  /* DMA1_Channel2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PD2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void AreYouReadyToPlay() {
    while (1) {
        if (ReadyToPlay == 1) // If already ready to play, break the loop
            break;

        memset(RxBuffer, 0, sizeof(RxBuffer)); // Clear the receive buffer

        HAL_StatusTypeDef status = HAL_UART_Receive(&hlpuart1, RxBuffer, 27, HAL_MAX_DELAY);

        if (status == HAL_OK && strcmp((char*)RxBuffer, "wanna play this game so bad") == 0) {//correct condition
            ReadyToPlay=1; //you are ready to play
            HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Welcome Your Turn start!\r\n", strlen("Welcome Your Turn start!\r\n"), HAL_MAX_DELAY);
            break; // Exit the loop once condition is met
        }
        else {
            ReadyToPlay=0;
            HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Do you really wanna play? Just type\r\n", strlen("Do you really wanna play? Just type\r\n"), HAL_MAX_DELAY);
            HAL_UART_Transmit(&hlpuart1, (uint8_t *)"wanna play this game so bad\r\n", strlen("wanna play this game so bad\r\n"), HAL_MAX_DELAY);
        }
    }
}



void SPITxRx_Setup()
{
//CS pulse
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0); // CS Select
HAL_Delay(5);
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1); // CS deSelect
HAL_Delay(5);
}

void SPITxRx_readIO()
{
if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_2))
{
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0); // CS Select
SPITx[0] = 0b01000001;
SPITx[1] = 0x12;
SPITx[2] = 0;
SPITx[3] = 0;
HAL_SPI_TransmitReceive_IT(&hspi3, SPITx, SPIRx, 4);
}
}

void TikTokGame()
{
	// for my reminder
//	int game_winner=0;//0=no 1=ai 2=human
//	int game_turn=0;//0=player 1=ai
//	int game_left=5;//amount of lamp that has left
//	int game_pick=0;//amount of pick for LED
    if(game_turn == 0) // player turn
    {
    	switch (game_left) {
    	    case 5:
    	        // statements
    	        switch (SPIRx[2]) {
    	            case 6: // Button 1 press pick 1
    	                game_pick = 1; // pick1
    	                game_left = 4; // coin left 4
    	                game_winner = 0; // no winner
    	                game_turn = 1; // next is ai turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"You pick 1\r\n", strlen("You pick 1\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 5: // Button 2 press pick 2
    	                game_pick = 2; // pick2
    	                game_left = 3; // coin left 3
    	                game_winner = 0; // no winner
    	                game_turn = 1; // next is ai turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"You pick 2\r\n", strlen("You pick 2\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 7: // Button not press let it loop
    	                break;
    	            case 3: // Button 3 press reset
    	                game_pick = 0; // no pick
    	                game_left = 5; // coin left 5
    	                game_winner = 0; // no winner
    	                game_turn = 0; // human turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Loser gonna keep restart\r\n", strlen("Loser gonna keep restart\r\n"), HAL_MAX_DELAY);
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Your Turn NOW!!\r\n", strlen("Your Turn NOW!!\r\n"), HAL_MAX_DELAY);
    	                break;
    	            default: // multi press do nothing
    	                break;
    	        }
    	        break;
    	    case 3:
    	        // statements
    	        switch (SPIRx[2]) {
    	            case 6: // Button 1 press pick 1
    	                game_pick = 1; // pick1
    	                game_left = 2; // coin left 2
    	                game_winner = 0; // no winner
    	                game_turn = 1; // next is ai turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"You pick 1\r\n", strlen("You pick 1\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 5: // Button 2 press pick 2
    	                game_pick = 2; // pick2
    	                game_left = 1; // coin left 1
    	                game_winner = 0; // no winner
    	                game_turn = 1; // next is ai turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"You pick 2\r\n", strlen("You pick 2\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 7: // Button not press let it loop
    	                break;
    	            case 3: // Button 3 press reset
    	                game_pick = 0; // no pick
    	                game_left = 5; // coin left 5
    	                game_winner = 0; // no winner
    	                game_turn = 0; // human turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Loser gonna keep restart\r\n", strlen("Loser gonna keep restart\r\n"), HAL_MAX_DELAY);
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Your Turn NOW!!\r\n", strlen("Your Turn NOW!!\r\n"), HAL_MAX_DELAY);
    	                break;
    	            default: // multi press do nothing
    	                break;
    	        }
    	        break;
    	    case 2:
    	        // statements
    	        switch (SPIRx[2]) {
    	            case 6: // Button 1 press pick 1
    	                game_pick = 1; // pick1
    	                game_left = 1; // coin left 1
    	                game_winner = 0; // no winner
    	                game_turn = 1; // next is ai turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"You pick 1\r\n", strlen("You pick 1\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 5: // Button 2 press pick 2
    	                game_pick = 2; // pick2
    	                game_left = 0; // coin left 1
    	                game_winner = 2; // human winner
    	                game_turn = 0; // reset to human
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"You pick 2\r\n", strlen("You pick 2\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 7: // Button not press let it loop
    	                break;
    	            case 3: // Button 3 press reset
    	                game_pick = 0; // no pick
    	                game_left = 5; // coin left 5
    	                game_winner = 0; // no winner
    	                game_turn = 0; // human turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Loser gonna keep restart\r\n", strlen("Loser gonna keep restart\r\n"), HAL_MAX_DELAY);
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Your Turn NOW!!\r\n", strlen("Your Turn NOW!!\r\n"), HAL_MAX_DELAY);
    	                break;
    	            default: // multi press do nothing
    	                break;
    	        }
    	        break;
    	    case 1:
    	        // statements
    	        switch (SPIRx[2]) {
    	            case 6: // Button 1 press pick 1
    	                game_pick = 1; // pick1
    	                game_left = 0; // coin left 0
    	                game_winner = 2; // human winner
    	                game_turn = 0; // reset to human
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"You pick 1\r\n", strlen("You pick 1\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 5: // Button 2 press pick 2
    	                game_pick = 1; // force to pick1
    	                game_left = 0; // coin left 1
    	                game_winner = 2; // human winner
    	                game_turn = 0; // reset to human
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Are you blind? I gonna let you pick 1\r\n", strlen("Are you blind? I gonna let you pick 1\r\n"), HAL_MAX_DELAY);
    	                break;
    	            case 7: // Button not press let it loop
    	                break;
    	            case 3: // Button 3 press reset
    	                game_pick = 0; // no pick
    	                game_left = 5; // coin left 5
    	                game_winner = 0; // no winner
    	                game_turn = 0; // human turn
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Loser gonna keep restart\r\n", strlen("Loser gonna keep restart\r\n"), HAL_MAX_DELAY);
    	                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Your Turn NOW!!\r\n", strlen("Your Turn NOW!!\r\n"), HAL_MAX_DELAY);
    	                break;
    	            default: // multi press do nothing
    	                break;
    	        }
    	        break;
    	}
    	//game_turn = 1;//need to put it inside for if not push
    }
    else // ai turn
    {
        switch (game_left)//amount left for ai to think(fix lol)
        {
            case 4:
                // statements
                game_pick = 1; // pick1
                game_left = 3; // coin left 3
                game_winner = 0; // no winner
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"AI pick 1\r\n", strlen("AI pick 1\r\n"), HAL_MAX_DELAY);
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Your Turn NOW!!\r\n", strlen("Your Turn NOW!!\r\n"), HAL_MAX_DELAY);
                break;
            case 3:
                // statements
                game_pick = 1; // pick1
                game_left = 2; // coin left 2
                game_winner = 0; // no winner
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"AI pick 1\r\n", strlen("AI pick 1\r\n"), HAL_MAX_DELAY);
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Your Turn NOW!!\r\n", strlen("Your Turn NOW!!\r\n"), HAL_MAX_DELAY);
                break;
            case 2:
                // statements
                game_pick = 2; // pick2
                game_left = 0; // coin left 0
                game_winner = 1; // ai winner
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"AI pick 2\r\n", strlen("AI pick 2\r\n"), HAL_MAX_DELAY);
                break;
            case 1:
                // statements
                game_pick = 1; // pick1
                game_left = 0; // coin left 0
                game_winner = 1; // ai winner
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"AI pick 1\r\n", strlen("AI pick 1\r\n"), HAL_MAX_DELAY);
                break;
        }
        game_turn = 0;
        //HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Your Turn NOW!!\r\n", strlen("Your Turn NOW!!\r\n"), HAL_MAX_DELAY);
    }
    HAL_Delay(200);//delay
}

void LEDdisplay() {
    // Add your code here to control the LEDs
	switch (game_winner) {
	        case 0:
	            // Still playing
	            break;
	        case 1:
	            // AI wins
	        	//light effect
//	        	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 0); // CS Select
//	        	SPITx[0] = 0b01000000;
//	        	SPITx[1] = 0x01;
//	        	SPITx[2] = 0b00111111;
//	        	HAL_SPI_TransmitReceive_IT(&hspi3, SPITx, SPIRx, 3);
	        	//reset after light
                game_pick = 0; // no pick
                game_left = 5; // coin left 5
                game_winner = 0; // RESET
                game_turn = 0; // human turn
                // Send the message over UART
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"AI Win!! Humanity Lost!\r\n", strlen("AI Win!! Humanity Lost!\r\n"), HAL_MAX_DELAY);
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Try again we believe in you\r\n", strlen("Try again we believe in you\r\n"), HAL_MAX_DELAY);
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Coin left is 5. It's your turn to pick.\r\n", strlen("Coin left is 5. It's your turn to pick.\r\n"), HAL_MAX_DELAY);
	            break;
	        case 2:
	            // Human wins
	        	//reset after light
                game_pick = 0; // no pick
                game_left = 5; // coin left 5
                game_winner = 0; // RESET
                game_turn = 0; // human turn
                // Send the message over UART
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Congrats You are winning!!!\r\n", strlen("Congrats You are winning!!!\r\n"), HAL_MAX_DELAY);
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"But now what?? Let's play again I know You still fun\r\n", strlen("But now what?? Let's play again I know You still fun\r\n"), HAL_MAX_DELAY);
                HAL_UART_Transmit(&hlpuart1, (uint8_t *)"Coin left is 5. It's your turn to pick.\r\n", strlen("Coin left is 5. It's your turn to pick.\r\n"), HAL_MAX_DELAY);
	            break;
	    }
}
void CoinLeftUART() {
	 char coin_left_message[30]; // Buffer to store the message
	    sprintf(coin_left_message, "Coin left is %d\r\r\n", game_left); // Use \r\r\n for proper newline
	    HAL_UART_Transmit(&hlpuart1, (uint8_t *)coin_left_message, strlen(coin_left_message), HAL_MAX_DELAY);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)// timer interrupt for led
{
if(htim == &htim3)
{
//
//	LMode1+=1;
//	if (LMode1>8)
//	{
//	LMode1 = 1;
//	}
}
}

void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
{
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1); //CS dnSelect
}
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
     ex: printf("Wrong parameters value: file %s on line %d\r\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
