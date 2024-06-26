/**
 *  @brief
 *      HAL Flash routines.
 *
 *      Program and erase are blocking (RTOS)!
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
#include "shci.h"

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "flash.h"
#include "myassert.h"
#include "stm32_lpm.h"

#define PROCESS_ID	11

// Private function prototypes
// ***************************
int aquire_flash(uint8_t Erase);
int release_flash(uint8_t Erase);

// Global Variables
// ****************

// RTOS resources
// **************

static osMutexId_t FLASH_MutexID;
static const osMutexAttr_t FLASH_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};


// Private Variables
// *****************

// Variable used for Erase procedure
static FLASH_EraseInitTypeDef EraseInitStruct;
static uint32_t os_state;

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
	ASSERT_fatal(FLASH_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

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

	if (Address < 0x08040000 || Address >= 0x080C0000) {
		Error_Handler();
		return -1;
	}

	// only one thread is allowed to use the flash
	osMutexAcquire(FLASH_MutexID, osWaitForever);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_FLASH, UTIL_LPM_DISABLE);

	aquire_flash(FALSE);

	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	data.word[0] = word1;
	data.word[1] = word2;
	if (HAL_FLASHEx_IsOperationSuspended()) {
		osDelay(2);
//		Error_Handler();
	}
	return_value = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address,
				data.doubleword);

	release_flash(FALSE);

	UTIL_LPM_SetStopMode(1U << CFG_LPM_FLASH, UTIL_LPM_ENABLE);
	osMutexRelease(FLASH_MutexID);

	if (return_value != HAL_OK) {
		return_value = HAL_ERROR;
		Error_Handler();
	}

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
	uint32_t PageError;

	// only one thread is allowed to use the flash
	osMutexAcquire(FLASH_MutexID, osWaitForever);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_FLASH, UTIL_LPM_DISABLE);

	aquire_flash(TRUE);

	// Clear OPTVERR bit set on virgin samples
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_ALL_ERRORS);

	EraseInitStruct.Page = (Address - FLASH_BASE) / FLASH_PAGE_SIZE;
	if (HAL_FLASHEx_IsOperationSuspended()) {
		osDelay(2);
	}
	return_value = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	if (return_value != HAL_OK) {
		return_value = HAL_ERROR;
		Error_Handler();
	}

	release_flash(TRUE);

	UTIL_LPM_SetStopMode(1U << CFG_LPM_FLASH, UTIL_LPM_ENABLE);
	osMutexRelease(FLASH_MutexID);
	return return_value;
}


// Private Functions
// *****************

// the STM32WB has only one single flash bank, but 2 CPUs
// for details see AN5289 chapter 4.7 Flash memory management

int aquire_flash(uint8_t Erase) {
	int tries = 0;

	while (HAL_HSEM_FastTake(2)) {
		; // busy wait
		if (tries++ > 100) {
			ASSERT_fatal(0, ASSERT_FLASH_UNLOCK, 0);
		}
		osDelay(1);
	}

	ASSERT_fatal(HAL_FLASH_Unlock() != HAL_ERROR, ASSERT_FLASH_UNLOCK, 0);

	if (Erase) {
		SHCI_C2_FLASH_EraseActivity(ERASE_ACTIVITY_ON);
	}

	tries = 0;
	while (TRUE) {
		if (tries++ > 10000) {
			// more than 20 s
			ASSERT_fatal(0, ASSERT_FLASH_UNLOCK, 0);
			tries = 0;
		}

		// PESD bit set?
		if (LL_FLASH_IsActiveFlag_OperationSuspended()) {
			// wait for PESD Flag
			osDelay(2);
			continue;
		}

		// enter critical section
		os_state = osKernelLock();
//		taskENTER_CRITICAL();

		if (HAL_HSEM_IsSemTaken(6)) {
			// exit critical section
			osKernelRestoreLock(os_state);
			osDelay(2);
			continue; // busy wait
		}
		if (HAL_HSEM_FastTake(7)) {
			// exit critical section
			osKernelRestoreLock(os_state);
			osDelay(2);
			continue; // busy wait
		}

		break;
	}
	return HAL_OK;
}


int release_flash(uint8_t Erase) {

	HAL_HSEM_Release(7, 0);

	// exit critical section
	osKernelRestoreLock(os_state);

	if (Erase) {
		SHCI_C2_FLASH_EraseActivity(ERASE_ACTIVITY_OFF);
	}

	ASSERT_nonfatal(HAL_FLASH_Lock() != HAL_ERROR, ASSERT_FLASH_LOCK, 0);

	HAL_HSEM_Release(2, 0);

	return HAL_OK;
}


