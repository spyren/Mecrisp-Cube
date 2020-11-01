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
static int flash_page(uint8_t *pData, uint32_t addr, uint8_t block_field);


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

uint8_t scratch_block[FD_PAGE_SIZE]; // protected by FD_MutexID


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
  *
  *     The flash is divided in 4 KiB pages. If any byte in the sectors is not
  *     0xFF the page where the sector belongs to has to be erased. The whole page
  *     has to be saved before.
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
	int i;
	uint8_t block_field; // bit0 block0, bit1 block1 ..

	if (FD_START_ADDRESS + (WriteAddr+NumOfBlocks+1)*FD_BLOCK_SIZE < FD_END_ADDRESS) {
		// valid blocks

		osMutexAcquire(FD_MutexID, osWaitForever);

		int FirstBoundary = WriteAddr % 8;
		int LastBoundary = (WriteAddr + NumOfBlocks) % 8;
		int pages = NumOfBlocks / 8;
		if (FirstBoundary + LastBoundary > 0) {
			pages++;
		}

		uint32_t addr = FD_START_ADDRESS + WriteAddr*FD_BLOCK_SIZE;
		retr = SD_OK;
		for (i=0; i < pages; i++) {
			if (i==0 && i == pages -1) {
				block_field = (0xFF << FirstBoundary) & (0xFF >> (8 - LastBoundary));
			} else if (i==0) {
				block_field = 0xFF << FirstBoundary;
			} else if (i == pages -1) {
				block_field = 0xFF >> LastBoundary;
			} else {
				block_field = 0xFF;
			}
			if (flash_page(pData+i*FD_PAGE_SIZE,
					addr + i*FD_PAGE_SIZE, block_field) != SD_OK) {
				break;
				retr = SD_ERROR;
			}
		}

		osMutexRelease(FD_MutexID);
	} else {
		retr = SD_ERROR;
	}

	/* Return the response */
	return retr;
}


// Private Functions
// *****************

static int flash_page(uint8_t *pData, uint32_t addr, uint8_t block_field) {
	uint8_t retr = SD_OK;
	int i, j;
	uint8_t *byte_p;
	uint8_t erased = TRUE;

	// are the blocks in the page already erased?
	byte_p = (uint8_t *) addr;
	for (i=0; i<(FD_PAGE_SIZE/FD_BLOCK_SIZE); i++) {
		if (block_field >> i) {
			// block belongs to page
			for (j=0; j<FD_BLOCK_SIZE; j++) {
				if (*(byte_p + j) != 0xFF) {
					erased = FALSE;
					break;
				}
			}
		}
		if (!erased) {
			break;
		}
		byte_p += FD_BLOCK_SIZE;
	}

	if (erased) {
		// flash erased
		// flash the blocks
		byte_p = (uint8_t *) addr;
		for (i=0; i<(FD_PAGE_SIZE/FD_BLOCK_SIZE); i++) {
			if (block_field >> i) {
				// block belongs to page -> program
				for (j=0; j < FD_BLOCK_SIZE / 8; j++) {
					if (FLASH_programDouble( (uint32_t) byte_p,
							*(uint32_t *) (pData+i*FD_PAGE_SIZE+j*8),
							*(uint32_t *) (pData+i*FD_PAGE_SIZE+j*8+4) )
							!= HAL_OK) {
						retr = SD_ERROR;
						break;
					}
					byte_p += 8;
				}
			} else {
				byte_p += FD_BLOCK_SIZE;
			}
			if (retr != SD_OK) {
				break;
			}
		}
	} else {
		// flash not erased
		// save the affected records (blocks)
		memcpy(scratch_block, (uint8_t *) addr, FD_PAGE_SIZE);

		// copy the blocks to the scratch buffer
		byte_p = (uint8_t *) addr;
		for (i=0; i<(FD_PAGE_SIZE/FD_BLOCK_SIZE); i++) {
			if (block_field >> i) {
				// block belongs to page -> copy
				memcpy(scratch_block+i*FD_BLOCK_SIZE, byte_p, FD_PAGE_SIZE);
			}
			byte_p += FD_BLOCK_SIZE;
		}

		// erase page
		if (FLASH_erasePage(addr) == HAL_OK) {
			// flash the page
			byte_p = (uint8_t *) addr;
			for (i=0; i<(FD_PAGE_SIZE/8); i++) {
				if (FLASH_programDouble( (uint32_t) byte_p,
						*(uint32_t *) (pData+i*8),
						*(uint32_t *) (pData+i*8+4) )
						!= HAL_OK) {
					retr = SD_ERROR;
					break;
				}
				byte_p += 8;
			}
		} else {
			retr = SD_ERROR;
		}
	}
	return retr;
}



