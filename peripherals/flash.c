/**
 *  @brief
 *      HAL Flash routines.
 *
 *      Program and erase are blocking (RTOS)!
 *      The STM32H7 has two flash banks and the access to the flash bank
 *      during program/erase is not possible.
 *      Smallest Flash record is 32 Bytes.
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
#include "myassert.h"

#define PROCESS_ID	11

// Private function prototypes
// ***************************

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

	EraseInitStruct.TypeErase    = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.NbSectors    = 1;
	EraseInitStruct.Banks        = FLASH_BANK_2;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
}

/**
 *  @brief
 *      Programs 32 bytes (8 32bit-words) in the FLASH.
 *  @param[in]
 *      Address  first byte
 *  @param[in]
 *      word1
 *  @return
 *      HAL Status
 */
int FLASH_program32B(uint32_t Address, uint32_t* buffer) {
	int return_value;

	if (Address < 0x08100000 || Address >= 0x08200000) {
		Error_Handler();
		return -1;
	}

	// only one thread is allowed to use the flash
	osMutexAcquire(FLASH_MutexID, osWaitForever);

	if (HAL_FLASH_Unlock() == HAL_ERROR) {
		Error_Handler();
	}

	return_value = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, Address, (uint32_t) buffer);
	if (return_value != HAL_OK) {
		return_value = HAL_ERROR;
		Error_Handler();
	}

	if (HAL_FLASH_Lock() == HAL_ERROR) {
		Error_Handler();
	}

	osMutexRelease(FLASH_MutexID);
	return return_value;
}


/**
 *  @brief
 *      Erases a page/sector (128 KiB) in the flash. Only Bank 2 is used.
 *
 *		128 KiB sectors            Bank
 *		0x08000000  0x0801FFFF  0  1
 *		0x08020000  0x0803FFFF  1  1
 *		0x08040000  0x0805FFFF  2  1
 *		0x08060000  0x0807FFFF  3  1
 *		0x08080000  0x0809FFFF  4  1
 *		0x080A0000  0x080BFFFF  5  1
 *		0x080C0000  0x080DFFFF  6  1
 *		0x080E0000  0x080FFFFF  7  1
 *		0x08100000  0x0811FFFF  0  2
 *		0x08120000  0x0813FFFF  1  2
 *		0x08140000  0x0815FFFF  2  2
 *		0x08160000  0x0817FFFF  3  2
 *		0x08180000  0x0819FFFF  4  2
 *		0x081A0000  0x081BFFFF  5  2
 *		0x081C0000  0x081DFFFF  6  2
 *		0x081E0000  0x081FFFFF  7  2
 *
 *  @todo
 *    use HAL_FLASHEx_Erase_IT
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

	if (HAL_FLASH_Unlock() == HAL_ERROR) {
		Error_Handler();
		return_value = HAL_ERROR;
		osMutexRelease(FLASH_MutexID);
		return return_value;
	}

	if (        (Address >= 0x08100000) && (Address <= 0x0811FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_0;
	} else if ( (Address >= 0x08120000) && (Address <= 0x0813FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_1;
	} else if ( (Address >= 0x08140000) && (Address <= 0x0815FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_2;
	} else if ( (Address >= 0x08160000) && (Address <= 0x0817FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_3;
	} else if ( (Address >= 0x08180000) && (Address <= 0x0819FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_4;
	} else if ( (Address >= 0x081A0000) && (Address <= 0x081BFFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_5;
	} else if ( (Address >= 0x081C0000) && (Address <= 0x081DFFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_6;
	} else if ( (Address >= 0x081E0000) && (Address <= 0x081FFFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_7;
	} else {
		// invalid address
	}

	return_value = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	if (return_value != HAL_OK) {
		return_value = HAL_ERROR;
		Error_Handler();
	}

	if (HAL_FLASH_Lock() == HAL_ERROR) {
		Error_Handler();
	}

	osMutexRelease(FLASH_MutexID);
	return return_value;
}


// Private Functions
// *****************




