/**
 *  @brief
 *      Flash drive block read and write.
 *
 *      The internal flash from 0x08060000 to 0x080BFFFF (384 KiB) is used
 *      for the flash drive.
 *      API similar to sd card.
 *  @file
 *      fd.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-10-27
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
#include "fd.h"
#include "sd.h"
#include "flash.h"


// Defines
// *******
#define FLASH_DUMMY_BYTE		0xFF


// Private function prototypes
// ***************************


// Global Variables
// ****************

// RTOS resources
// **************

static osMutexId_t FD_MutexID;
static const osMutexAttr_t FD_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};


// Hardware resources
// ******************


// Private Variables
// *****************


int FD_size = 0; // number of blocks

uint8_t scratch_block[FD_BLOCK_SIZE];


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the flash drive.
 *  @return
 *      None
 */
void FD_init(void) {
	FD_MutexID = osMutexNew(&FD_MutexAttr);
	if (FD_MutexID == NULL) {
		Error_Handler();
	}
}


/**
 *  @brief
 *      Initializes the flash drive and get the size in KiB.
 *  @return
 *      None
 */
void FD_getSize(void) {
	FD_size = ((FD_END_ADDRESS + 1) - FD_START_ADDRESS) / 1024;
}

/**
 *  @brief
 *      Get the flash drive size in KiB.
 *  @return
 *      size in KiB
 */
int FD_getBlocks(void) {
	return FD_size;
}


/**
  * @brief
  *     Reads block(s) from a specified address from the flash drive.
  *
  * @param
  *     pData: Pointer to the buffer that will contain the data to transmit
  * @param
  *     ReadAddr: Address from where data is to be read. The address is counted
  *                   in blocks of 512bytes
  * @param
  *     NumOfBlocks: Number of FD blocks to read
  * @retval
  *     FD status
  */
uint8_t FD_ReadBlocks(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks) {
	uint8_t retr = SD_ERROR;

	if (FD_START_ADDRESS + (ReadAddr+NumOfBlocks+1)*FD_BLOCK_SIZE < FD_END_ADDRESS) {
		// valid blocks
		memcpy(pData, (uint8_t *) FD_START_ADDRESS + ReadAddr*FD_BLOCK_SIZE,
				NumOfBlocks*FD_BLOCK_SIZE);
		retr = SD_OK;
	}
	/* Return the reponse */
	return retr;
}


/**
  * @brief
  *     Writes block(s) to a specified address to the internal flash.
  * @param
  *     pData: Pointer to the buffer that will contain the data to transmit
  * @param
  *     WriteAddr: Address from where data is to be written. The address is counted
  *                   in blocks of 512bytes
  * @param
  *     NumOfBlocks: Number of FD blocks to write
  * @retval
  *     FD status
  */
uint8_t FD_WriteBlocks(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks) {
	uint8_t retr = SD_ERROR;
	uint32_t addr;
	int i;

	if (FD_START_ADDRESS + (WriteAddr+NumOfBlocks+1)*FD_BLOCK_SIZE < FD_END_ADDRESS) {
		// valid blocks
		addr = FD_START_ADDRESS + WriteAddr*FD_BLOCK_SIZE;
		for (i=0; i < NumOfBlocks * FD_BLOCK_SIZE / 8; i++) {
			if (FLASH_programDouble( addr,
					*(uint32_t *) (pData+i*8),
					*(uint32_t *) (pData+i*8+4) )
					!= HAL_OK) {
				break;
			}
			addr += 8;
		}

		retr = SD_OK;
	}
	/* Return the response */
	return retr;
}


/**
  * @brief
  *     Erases the specified memory in the internal flash.
  * @param
  *     StartAddr: Start address in Blocks (Size of a block is 512bytes)
  * @param
  *     EndAddr: End address in Blocks (Size of a block is 512bytes)
  * @retval
  *     FD status
  */
uint8_t FD_Erase(uint32_t StartAddr, uint32_t EndAddr) {
	uint8_t retr = SD_ERROR;

	/* Return the response */
	return retr;
}


// Private Functions
// *****************



