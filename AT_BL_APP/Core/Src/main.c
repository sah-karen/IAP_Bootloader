/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"
#include "upgrade.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define GREEN_LED_REG			GREEN_LED_GPIO_Port->ODR

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define GREEN_LED_BIT			GPIO_ODR_12

#define TOGGLE_BIT(REG, BIT)	((REG) ^= (BIT))

#define GREEN_LED(set)			((set) == SET ? SET_BIT(GREEN_LED_REG, GREEN_LED_BIT) : CLEAR_BIT(GREEN_LED_REG, GREEN_LED_BIT))

static void SetReadoutProtectionLevel1(void)
{
	HAL_FLASH_Unlock();
	HAL_FLASH_OB_Unlock();

	FLASH_OBProgramInitTypeDef OBInit;
	HAL_FLASHEx_OBGetConfig(&OBInit);

	if (OBInit.RDPLevel != OB_RDP_LEVEL_1) {
		OBInit.OptionType = OPTIONBYTE_RDP;
		OBInit.RDPLevel = OB_RDP_LEVEL_1;
		if (HAL_FLASHEx_OBProgram(&OBInit) == HAL_OK) {
			HAL_FLASH_OB_Launch(); // This triggers a System Reset
		}
	}

	HAL_FLASH_OB_Lock();
	HAL_FLASH_Lock();
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  //SetReadoutProtectionLevel1();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  // Re-enable interrupts after bootloader
  //__enable_irq();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */
  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

   HAL_UART_Transmit(&huart1, (uint8_t *)"Inside Application v1.4\r\n", 25, 100);

	if (Magic_Number())
	{
		HAL_UART_Transmit(&huart1, (uint8_t *)"Magic\r\n", 7, 100);
		NVIC_SystemReset();
	}
//	{
//		HAL_UART_Transmit(&huart1, (uint8_t *)"No Magic\r\n", 9, 100);
//		Clear_Magic_Number();
//	}


  /* USER CODE END 2 */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t cmd_upgr[5];
  uint32_t time_out_green_led = HAL_GetTick();


  while (1)
  {
	  if (HAL_GetTick() - time_out_green_led > 1500)
	  {
 		   HAL_UART_Transmit(&huart1, (uint8_t *)"Ap", 2, 100);
		   TOGGLE_BIT(GREEN_LED_REG, GREEN_LED_BIT);
		   time_out_green_led = HAL_GetTick();
      }
	  if (HAL_UART_Receive(&huart1, cmd_upgr, 4, 500) == HAL_OK)
	  {
		  if (cmd_upgr[0] == 'U' &&
				  cmd_upgr[1] == 'P' &&
				  cmd_upgr[2] == 'G' &&
				  cmd_upgr[3] == 'R')
		  {
				Set_Magic_Number();
				//Write_Magic_Number_To_Flash(MAGIC_NUMBER);
				HAL_UART_Transmit(&huart1, (uint8_t *)"UPGR\r\n", 7, 100);
				NVIC_SystemReset();
		  }
		  else if (cmd_upgr[0] == 'S' &&
				  cmd_upgr[1] == 'T' &&
						  cmd_upgr[2] == '6')
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT-01 v1.4\r\n", 12, 100);


	  }
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
#ifdef USE_FULL_ASSERT
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
