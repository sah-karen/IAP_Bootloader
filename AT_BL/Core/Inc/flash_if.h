/**
  ******************************************************************************
  * @file    IAP_Main/Inc/flash_if.h 
  * @author  MCD Application Team
  * @brief   This file provides all the headers of the flash_if functions.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/


//#define ADDR_FLASH_PAGE_255   ((uint32_t)0x0807f800) /* Base @ of Page 255, 2 Kbytes */
#define ADDR_FLASH_PAGE_127   ((uint32_t)0x0803f800) /* skk Base @ of Page 127, 2 Kbytes */
 // 3F800  -- 260kb , 4 0000 - 262kb  800-2kb
/* Error code */
enum
{
  FLASHIF_OK = 0,
  FLASHIF_ERASEKO,
  FLASHIF_WRITINGCTRL_ERROR,
  FLASHIF_WRITING_ERROR,
  FLASHIF_PROTECTION_ERRROR
};


/* Define the address from where user application will be loaded.
   Note: this area is reserved for the IAP code                  */
#define FLASH_PAGE_STEP         FLASH_PAGE_SIZE           /* Size of page : 2 Kbytes */

//#define FOR_AT_BL_APP

#ifdef FOR_AT_BL_APP
#define APPLICATION_ADDRESS     (uint32_t)0x08006000      /* Start user code address: ADDR_FLASH_PAGE_10  24kb for debug */
#else  // for AT-01 and B-03 release build
// For AT-01 Release or B-03 Release builds , offset is 10kb -- 0x2800
#define APPLICATION_ADDRESS     (uint32_t)0x08002800      /* Start user code address: ADDR_FLASH_PAGE_8  16 kb for release*/
#endif
/* Notable Flash addresses */
#define USER_FLASH_END_ADDRESS        ADDR_FLASH_PAGE_127 + FLASH_PAGE_SIZE

/* Define the user application size */
#define USER_FLASH_SIZE               ((uint32_t)0x00003000) /* Small default template application */
//#define USER_FLASH_SIZE               ((uint32_t)12288) /*skk Small default template application */



/* Exported macro ------------------------------------------------------------*/
/* ABS value */
#define ABS_RETURN(x,y)               (((x) < (y)) ? (y) : (x))

/* Exported functions ------------------------------------------------------- */
void FLASH_If_Init(void);
uint32_t FLASH_If_Erase(uint32_t StartSector);
uint32_t FLASH_If_Write(uint32_t destination, uint32_t *p_source, uint32_t length);

#endif  /* __FLASH_IF_H */
