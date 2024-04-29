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
SUBGHZ_HandleTypeDef hsubghz;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

uint8_t payload[64] = "Hello World!";

uint8_t RadioParam[8]  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t interrupts[3] = {0};
uint8_t signal_delay = 100;
uint8_t uart_buff[65] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SUBGHZ_Init(void);
static void MX_USART1_UART_Init(void);
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
  MX_SUBGHZ_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_Delay(300);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
  HAL_Delay(300);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
  HAL_Delay(300);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);

  sprintf(uart_buff, "\n\rOKAAAAAYYYYYY LETS GO! \n\r");
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buff, sizeof(uart_buff), 100);

  // 1. Set Buffer Address
  RadioParam[0] = 0x80U;
  RadioParam[1] = 0x00U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_BUFFERBASEADDRESS, &RadioParam, 2) != HAL_OK)
  {
	  Error_Handler();
  }


  // 2. Write Payload to Buffer
  if (HAL_SUBGHZ_WriteBuffer(&hsubghz, 0x80U, &payload, sizeof(payload)) != HAL_OK)
  {
	  Error_Handler();
  }


  // 3. Set Packet Type
  RadioParam[0] = 0x01U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETTYPE, &RadioParam, 1) != HAL_OK)
  {
	  Error_Handler();
  }


  // 4. Set Frame Format
  RadioParam[0] = 0x00U;
  RadioParam[1] = 0x0CU;
  RadioParam[2] = 0x00U;
  RadioParam[3] = 0x40U;
  RadioParam[4] = 0x01U;
  RadioParam[5] = 0x00U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACKETPARAMS, &RadioParam, 6) != HAL_OK)
  {
	  Error_Handler();
  }


  // 5. Define synchronisation word
  RadioParam[0] = 0x14U;
  RadioParam[1] = 0x24U;

  if (HAL_SUBGHZ_WriteRegisters(&hsubghz, (uint16_t) 0x740, &RadioParam, 2) != HAL_OK)
  {
	  Error_Handler();
  }


  // 6. Define RF Frequency
  RadioParam[0] = 0x33U;
  RadioParam[1] = 0xBCU;
  RadioParam[2] = 0xA1U;
  RadioParam[3] = 0x00U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_RFFREQUENCY, &RadioParam, 4) != HAL_OK)
  {
	  Error_Handler();
  }


  // 7. Set PA Config
  RadioParam[0] = 0x04U;
  RadioParam[1] = 0x00U;
  RadioParam[2] = 0x01U;
  RadioParam[3] = 0x01U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_PACONFIG, &RadioParam, 4) != HAL_OK)
  {
	  Error_Handler();
  }


  // 8.  Set Tx Parameters
  RadioParam[0] = 0x0EU;
  RadioParam[1] = 0x04U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TXPARAMS, &RadioParam, 2) != HAL_OK)
  {
	  Error_Handler();
  }


  // 9. Set Modulation parameter
  RadioParam[0] = 0x07U;
  RadioParam[1] = 0x09U;
  RadioParam[2] = 0x01U;
  RadioParam[3] = 0x00U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_MODULATIONPARAMS, &RadioParam, 4) != HAL_OK)
  {
	  Error_Handler();
  }


  // 10. Configure interrupts
  RadioParam[0] = 0x01U;
  RadioParam[1] = 0x01U;
  RadioParam[2] = 0x00U;
  RadioParam[3] = 0x01U;
  RadioParam[4] = 0x01U;
  RadioParam[5] = 0x00U;
  RadioParam[6] = 0x00U;
  RadioParam[7] = 0x00U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_CFG_DIOIRQ, &RadioParam, 8) != HAL_OK)
  {
	  Error_Handler();
  }


  // 10.1 Clear Interrupts
  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_IRQSTATUS, &interrupts, 3) != HAL_OK)
  {
	  Error_Handler();
  }

  sprintf(uart_buff, "Interrupts after set:  %i %i %i \n\r", interrupts[0], interrupts[1], interrupts[2]);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buff, sizeof(uart_buff), 100);





  // 11. Set Tx
  RadioParam[0] = 0x09U;
  RadioParam[1] = 0xC4U;
  RadioParam[2] = 0x00U;

  if (HAL_SUBGHZ_ExecSetCmd(&hsubghz, RADIO_SET_TX, &RadioParam, 3) != HAL_OK)
  {
	  Error_Handler();
  }


  // Check interrupts

  do
  {
	  sprintf(uart_buff, "Interrupts after send: %i %i %i \n\r", interrupts[0], interrupts[1], interrupts[2]);
	  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buff, sizeof(uart_buff), 100);

	  if (HAL_SUBGHZ_ExecGetCmd(&hsubghz, RADIO_GET_IRQSTATUS, &interrupts, 3) != HAL_OK)
	  {
		  Error_Handler();
	  }

	  if(interrupts[1] = 1)
	  {
		  signal_delay = 1000;
	  }

	  HAL_Delay(2000);
  } while (interrupts[1] != 0 | interrupts[2] != 0);


  sprintf(uart_buff, "Interrupts: %i %i %i \n\r", interrupts[0], interrupts[1], interrupts[2]);
  HAL_UART_Transmit(&huart1, (uint8_t *)uart_buff, sizeof(uart_buff), 100);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	  HAL_Delay(signal_delay);
	  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	  HAL_Delay(signal_delay);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SUBGHZ Initialization Function
  * @param None
  * @retval None
  */
static void MX_SUBGHZ_Init(void)
{

  /* USER CODE BEGIN SUBGHZ_Init 0 */

  /* USER CODE END SUBGHZ_Init 0 */

  /* USER CODE BEGIN SUBGHZ_Init 1 */

  /* USER CODE END SUBGHZ_Init 1 */
  hsubghz.Init.BaudratePrescaler = SUBGHZSPI_BAUDRATEPRESCALER_8;
  if (HAL_SUBGHZ_Init(&hsubghz) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SUBGHZ_Init 2 */

  /* USER CODE END SUBGHZ_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin : PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
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
