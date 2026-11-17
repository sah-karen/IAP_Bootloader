/**
  ******************************************************************************
  * @file    IAP_Main/Src/menu.c 
  * @author  MCD Application Team
  * @brief   This file provides the software which contains the main menu routine.
  *          The main menu gives the options of:
  *             - downloading a new binary file, 
  *             - uploading internal flash memory,
  *             - executing the binary file already loaded 
  *             - configuring the write protection of the Flash sectors where the 
  *               user loads his binary file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */ 

/** @addtogroup STM32F3xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "common.h"
#include "flash_if.h"
#include "menu.h"
#include "ymodem.h"
#include "upgrade.h"
#include "usart.h."

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction JumpToApplication;
uint32_t JumpAddress;
uint8_t aFileName[FILE_NAME_LENGTH];

/* Private function prototypes -----------------------------------------------*/
void SerialDownload(void);
void SerialUpload(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Download a file via serial port
  * @param  None
  * @retval None
  */
void SerialDownload(void)
{
  uint8_t number[11] = {0};
  uint32_t size = 0;
  COM_StatusTypeDef result;

  Serial_PutString((uint8_t *)"Waiting for the file to be sent ... (press 'a' to abort)\n\r");
  result = Ymodem_Receive( &size );
  if (result == COM_OK)
  {
    Serial_PutString((uint8_t *)"\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
    Serial_PutString(aFileName);
    Int2Str(number, size);
    Serial_PutString((uint8_t *)"\n\r Size: ");
    Serial_PutString(number);
    Serial_PutString((uint8_t *)" Bytes\r\n");
    Serial_PutString((uint8_t *)"-------------------\n");
  }
  else if (result == COM_LIMIT)
  {
    Serial_PutString((uint8_t *)"\n\n\rThe image size is higher than the allowed space memory!\n\r");
  }
  else if (result == COM_DATA)
  {
    Serial_PutString((uint8_t *)"\n\n\rVerification failed!\n\r");
  }
  else if (result == COM_ABORT)
  {
    Serial_PutString((uint8_t *)"\r\n\nAborted by user.\n\r");
  }
  else
  {
    Serial_PutString((uint8_t *)"\n\rFailed to receive the file!\n\r");
  }
}

uint16_t JumpToApp(void)
{
    /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
    if (((*(__IO uint32_t*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
    {
    	Clear_Magic_Number();
    	//CleanHandover();
        Serial_PutString((uint8_t *)"Start program execution......\r\n\n");
        //Disable all interrupts and clear any pending ones
        // do not forget enable it in application
        //__disable_irq();
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		JumpToApplication = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		JumpToApplication();
    }
    return 0;
}

/**
  * @brief  Upload a file via serial port.
  * @param  None
  * @retval None
  */
void SerialUpload(void)
{
  uint8_t status = 0;

  Serial_PutString((uint8_t *)"\n\n\rSelect Receive File\n\r");

  HAL_UART_Receive(&huart1, &status, 1, RX_TIMEOUT);
  if ( status == CRC16)
  {
    /* Transmit the flash image through ymodem protocol */
    status = Ymodem_Transmit((uint8_t*)APPLICATION_ADDRESS, (const uint8_t*)"UploadedFlashImage.bin", USER_FLASH_SIZE);

    if (status != 0)
    {
      Serial_PutString((uint8_t *)"\n\rError Occurred while Transmitting File\n\r");
    }
    else
    {
      Serial_PutString((uint8_t *)"\n\rFile uploaded successfully \n\r");
    }
  }
}

/**
  * @brief  Display the Main Menu on HyperTerminal
  * @param  None
  * @retval None
  */
void Main_Menu(void)
{
  uint8_t key = 0;

  Serial_PutString((uint8_t *)"\r\n\r\n");

  Serial_PutString((uint8_t *)"\r\n=================== Main Menu ==========================");
  Serial_PutString((uint8_t *)"\r\n  Download image to the internal Flash --------------- 1");
  //Serial_PutString((uint8_t *)"\r\n  Upload image from the internal Flash ------------- 2");
  Serial_PutString((uint8_t *)"\r\n  Execute the loaded application ------------------------ 3");
  Serial_PutString((uint8_t *)"\r\n========================================================\r\n\r\n");
  uint32_t time_out_m = HAL_GetTick();
  /* Clean the input path */
  __HAL_UART_FLUSH_DRREGISTER(&huart1);
  while (1)
  {


	
    /* Receive key */
    //HAL_UART_Receive(&huart1, &key, 1, RX_TIMEOUT);

    if (SerialKeyPressed(&key))
    {
		switch (key)
		{
		case '1' :
		  /* Download user application in the Flash */
		  SerialDownload();
		  break;
	//    case '2' :
		  /* Upload user application from the Flash */
	//      SerialUpload();
	//        break; // Do nor use Upload
		case '3' :
			if (!JumpToApp())
			{
				return;
			}
		  break;
		default:
			Serial_PutString((uint8_t *)"Invalid Number ! ==> The number should be either 1, 3\r\n");
		break;
		}
    }
    else
    {
    	if (HAL_GetTick()- time_out_m > 1000 ) // 200ms
    	{
    	   Serial_PutByte('M'); // Menu mode
    	   time_out_m = HAL_GetTick();
    	    /* Clean the input path */
    	    __HAL_UART_FLUSH_DRREGISTER(&huart1);
    	}
    }
  }
}

/**
  * @}
  */
