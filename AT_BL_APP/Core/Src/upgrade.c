/*
 * upgrade.c
 *
 *  Created on: Oct 16, 2026
 *      Author: Karen Sahakyan
 */
#include "upgrade.h"
#include "stm32f3xx_hal.h"

void Set_Magic_Number(void) {
    // 1. Enable power to the backup domain
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_DBP;

    // 2. Write the Magic Number
    BKP_REG = MAGIC_NUMBER;
    PWR->CR &= ~PWR_CR_DBP;
}


void Clear_Magic_Number(void)
{
	// 1. Enable access to the Backup Domain (needed to read/write RTC registers)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	// Clear the magic number so we don't loop back here after the next reset
	BKP_REG = 0;
	// Disable the backup domain access
	PWR->CR &= ~PWR_CR_DBP;
}

uint16_t Magic_Number(void)
{
	// 1. Enable access to the Backup Domain (needed to read/write RTC registers)
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_DBP;
	uint16_t ret = 0;
	// 2. Check if the "Magic Number" is present
	if (BKP_REG == MAGIC_NUMBER) {
		ret = 1;
	}
	// Disable the backup domain access
	PWR->CR &= ~PWR_CR_DBP;
	return ret;
}


uint32_t Read_Magic_Number_From_Flash(void) {
    // Flash can be read directly like normal RAM via pointer dereference
    return *(volatile uint32_t *)FLASH_MAGIC_ADDRESS;
}

/**
 * @brief  Write the magic number to Flash memory.
 *         Because Flash requires an erase before writing, this function
 *         erases the page first, then programs the 32-bit word.
 * @param  magic_value: The value to write (e.g., 0xDEADBEEF)
 * @retval HAL_StatusTypeDef: HAL_OK on success, or an error code.
 */
HAL_StatusTypeDef Write_Magic_Number_To_Flash(uint32_t magic_value) {
    HAL_StatusTypeDef status;

    status = Clear_Magic_Number_From_Flash();
    if (status != HAL_OK) {
        return status;
    }

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
           return status;
    }

    // 2. Program the 32-bit word into Flash
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_MAGIC_ADDRESS, magic_value);

    // 3. Lock the Flash control register back up to prevent accidental modifications
    HAL_FLASH_Lock();

    return status;
}

/**
 * @brief  Erase/Clear the magic number from Flash (resets the page to 0xFFFFFFFF).
 * @retval HAL_StatusTypeDef: HAL_OK on success, or an error code.
 */
HAL_StatusTypeDef Clear_Magic_Number_From_Flash(void) {
    HAL_StatusTypeDef status;
    uint32_t pageError = 0;

    // 1. Unlock Flash
    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) {
        return status;
    }

    // 2. Configure Erase parameters
    FLASH_EraseInitTypeDef eraseInit;
    eraseInit.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInit.PageAddress = FLASH_MAGIC_ADDRESS;
    eraseInit.NbPages     = 1;

    // 3. Perform Page Erase
    status = HAL_FLASHEx_Erase(&eraseInit, &pageError);

    // 4. Lock Flash
    HAL_FLASH_Lock();

    return status;
}

