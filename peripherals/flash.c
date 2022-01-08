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
	EraseInitStruct.Banks        = FLASH_BANK_1;
	EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
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

	if (Address < 0x08040000 || Address >= 0x080C0000) {
		Error_Handler();
		return -1;
	}

	// only one thread is allowed to use the flash
	osMutexAcquire(FLASH_MutexID, osWaitForever);

	if (HAL_FLASH_Unlock() == HAL_ERROR) {
		Error_Handler();
	}

	return_value = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address, word1);
	if (return_value != HAL_OK) {
		return_value = HAL_ERROR;
		Error_Handler();
	} else {
		return_value = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, Address+4, word2);
		if (return_value != HAL_OK) {
			return_value = HAL_ERROR;
			Error_Handler();
		}
	}

	if (HAL_FLASH_Lock() == HAL_ERROR) {
		Error_Handler();
	}

	osMutexRelease(FLASH_MutexID);
	return return_value;
}


/**
 *  @brief
 *      Erases a page/sector (16/64/128 KiB) in the flash.
 *
 *      16 KiB sectors
 *		0x08000000  0x08003FFF  0
 *		0x08004000  0x08007FFF  1
 *		0x08008000  0x0800BFFF  2
 *		0x0800C000  0x0800FFFF  3
 *		64 KiB sector
 *		0x08010000  0x0801FFFF  4
 *		128 KiB sectors
 *		0x08020000  0x0803FFFF  5
 *		0x08040000  0x0805FFFF  6  here starts the flash dictionary
 *		0x08060000  0x0807FFFF  7
 *		0x08080000  0x0809FFFF  8
 *		0x080A0000  0x080BFFFF  9
 *		0x080C0000  0x080DFFFF  10
 *		0x080E0000  0x080FFFFF  11
 *
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
	}

	if ( (Address >= 0x08000000) && (Address <= 0x08003FFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_0;
	} else if ( (Address >= 0x08004000) && (Address <= 0x08007FFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_1;
	} else if ( (Address >= 0x08008000) && (Address <= 0x0800BFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_2;
	} else if ( (Address >= 0x0800C000) && (Address <= 0x0800FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_3;
	} else if ( (Address >= 0x08010000) && (Address <= 0x0801FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_4;
	} else if ( (Address >= 0x08020000) && (Address <= 0x0803FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_5;
	} else if ( (Address >= 0x08040000) && (Address <= 0x0805FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_6;
	} else if ( (Address >= 0x08060000) && (Address <= 0x0807FFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_7;
	} else if ( (Address >= 0x08080000) && (Address <= 0x08007FFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_8;
	} else if ( (Address >= 0x080A0000) && (Address <= 0x080BFFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_9;
	} else if ( (Address >= 0x080C0000) && (Address <= 0x080DFFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_10;
	} else if ( (Address >= 0x080E0000) && (Address <= 0x080FFFFF) ) {
		EraseInitStruct.Sector = FLASH_SECTOR_11;
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




