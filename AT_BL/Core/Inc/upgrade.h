/*
 * upgrade.h
 *
 *  Created on: Oct 16, 2026
 *      Author: Karen Sahakyan
 */

#ifndef SRC_UPGRADE_H_
#define SRC_UPGRADE_H_

#include "stm32f3xx.h"

#define MAGIC_NUMBER       0xDEADBEEF
#define BKP_REG            RTC->BKP0R  // Backup Register 0

// Define a target Flash address (Last page of 256KB Flash on STM32F302CCT6)
#define FLASH_MAGIC_ADDRESS  0x0803F800

void Clear_Magic_Number(void);
uint16_t Magic_Number(void);
void Set_Magic_Number(void);

uint32_t Read_Magic_Number_From_Flash(void);
HAL_StatusTypeDef Write_Magic_Number_To_Flash(uint32_t magic_value);
HAL_StatusTypeDef Clear_Magic_Number_From_Flash(void);
#endif /* SRC_UPGRADE_H_ */
