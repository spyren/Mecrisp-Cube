/**
 *  @brief
 *      HAL Flash routines.
 *
 *      Program and erase are blocking (busy wait)!
 *      The STM32WB has only one flash bank and the access to the flash
 *      during program/erase is not possible.
 *      Erase takes about 20 ms, program 2 ms.
 *  @file
 *      flash.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-02-19
 *  @remark
 *      Language: C, STM32CubeIDE GCC
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This project Mecrsip-Cube is free software: you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation, either version 3 of
 *      the License, or (at your option) any later version.
 *
 *      Mecrsip-Cube is distributed in the hope that it will be useful, but
 *      WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *      General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.
 */

// System include files
// ********************
#include "cmsis_os.h"

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "flash.h"


#define FLASH_END_OF_OPERATION	0x01
#define FLASH_OPERATION_ERROR	0x02

// Private function prototypes
// ***************************

// Global Variables
// ****************

// RTOS resources
// **************

osMutexId_t FLASH_MutexID;
const osMutexAttr_t FLASH_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

osSemaphoreId_t FLASH_SemaphoreID;

// Private Variables
// *****************

// Variable used for Erase procedure
static FLASH_EraseInitTypeDef EraseInitStruct;
static volatile uint32_t PageOrAddress;
static volatile uint8_t FlashError = FALSE;

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the FLASH.
 *  @return
 *      None
 */
void FLASH_init(void) {
	FLASH_MutexID = osMutexNew(&FLASH_MutexAttr);
	FLASH_SemaphoreID = osSemaphoreNew(1, 0, NULL);

	HAL_NVIC_SetPriority(FLASH_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1, 0);
	HAL_NVIC_EnableIRQ(FLASH_IRQn);

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.NbPages     = 1;
}


/**
 *  @brief
 *      Programs 8 bytes (doubleword) in the FLASH.
 *  @param[in]
 *      Address  first byte
 *  @param[in]
 *      word1
 *  @param[in]
 *      word2
 *  @return
 *      HAL Status
 */
int FLASH_programDouble(uint32_t Address, uint32_t word1, uint32_t word2) {
	int return_value;

	union number {
		uint32_t word[2];
		uint64_t doubleword;
	} data;

	// only one thread is allowed to use the flash
	osMutexAcquire(FLASH_MutexID, osWaitForever);

	FlashError = FALSE;
	HAL_FLASH_Unlock();
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	data.word[0] = word1;
	data.word[1] = word2;
	return_value = HAL_FLASH_Program_IT(FLASH_TYPEPROGRAM_DOUBLEWORD, Address,
			data.doubleword);
	// blocked till programming is finished
	osSemaphoreAcquire(FLASH_SemaphoreID, osWaitForever);
	HAL_FLASH_Lock();
	if (FlashError) {
		return_value = HAL_ERROR;
	}

	osMutexRelease(FLASH_MutexID);
	return return_value;
}


/**
 *  @brief
 *      Erases a page (4 KiB) in the flash.
 *  @param[in]
 *      Address  first byte
 *  @return
 *      HAL Status
 */
int FLASH_erasePage(uint32_t Address) {
	int return_value;

	// only one thread is allowed to use the flash
	osMutexAcquire(FLASH_MutexID, osWaitForever);

	FlashError = FALSE;
	HAL_FLASH_Unlock();
	// Clear OPTVERR bit set on virgin samples
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	EraseInitStruct.Page = (Address - FLASH_BASE) / FLASH_PAGE_SIZE;
	return_value = HAL_FLASHEx_Erase_IT(&EraseInitStruct);
	// blocked till programming is finished
	osSemaphoreAcquire(FLASH_SemaphoreID, osWaitForever);
	HAL_FLASH_Lock();
	if (FlashError) {
		return_value = HAL_ERROR;
	}
	osMutexRelease(FLASH_MutexID);
	return return_value;
}


// Private Functions
// *****************


// Callbacks
// *********

/**
  * @brief  FLASH end of operation interrupt callback.
  * @param  ReturnValue The value saved in this parameter depends on the ongoing procedure
  *                  Page Erase: Page which has been erased
  *                  Program: Address which was selected for data program
  * @retval None
  */
void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(ReturnValue);

	PageOrAddress = ReturnValue;
	osSemaphoreRelease(FLASH_SemaphoreID);
}

/**
  * @brief  FLASH operation error interrupt callback.
  * @param  ReturnValue The value saved in this parameter depends on the ongoing procedure
  *                 Page Erase: Page number which returned an error
  *                 Program: Address which was selected for data program
  * @retval None
  */
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(ReturnValue);

	FlashError = TRUE;
	PageOrAddress = ReturnValue;
//	osSemaphoreRelease(FLASH_SemaphoreID);
}

