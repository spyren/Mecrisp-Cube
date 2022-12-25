/**
 *  @brief
 *      Serial Flash drive block read and write.
 *
 *      The external board has 16 MiB Serial Flash Macronix MX25L12835F.
 *      This Flash is used for the flash drive.
 *      API similar to sd card.
 *
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
#include "fd_spi.h"
#include "myassert.h"
#include "n25q128a.h"


// Defines
// *******
#define FLASH_DUMMY_BYTE	0xFF

#define W25Q128_PAGES		(N25Q128A_FLASH_SIZE/N25Q128A_PAGE_SIZE)
#define W25Q128_SECTORS		(N25Q128A_FLASH_SIZE/N25Q128A_SUBSECTOR_SIZE)

#define W25Q128_PAGE_SIZE	(N25Q128A_PAGE_SIZE)
#define W25Q128_SECTOR_SIZE	(N25Q128A_SUBSECTOR_SIZE)


// Private function prototypes
// ***************************
static int flash_sector(uint8_t *pData, uint32_t flash_addr, uint16_t block_field);


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

static uint8_t *scratch_sector; 	// protected by FD_MutexID

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the flash drive.
 *  @return
 *      None
 */
void FD_init(void) {
	FDSPI_init();
	scratch_sector = pvPortMalloc(W25Q128_SECTOR_SIZE);
	*scratch_sector = 0xaa;
	FD_MutexID = osMutexNew(&FD_MutexAttr);
	ASSERT_fatal(FD_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());
}


/**
 *  @brief
 *      Initializes the flash drive and get the size in KiB.
 *  @return
 *      None
 */
void FD_getSize(void) {
	FD_size = W25Q128_PAGES / 4;
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
  *     BlockAdr: Address from where data is to be read. The address is counted
  *                   in blocks of 512bytes
  * @param
  *     NumOfBlocks: Number of FD blocks to read
  * @retval
  *     FD status
  */
uint8_t FD_ReadBlocks(uint8_t *pData, uint32_t BlockAdr, uint32_t NumOfBlocks) {
	uint8_t retr = SD_ERROR;
	uint32_t adr = BlockAdr*FD_BLOCK_SIZE + FD_START_ADDRESS;

	if ((FD_START_ADDRESS + (BlockAdr+NumOfBlocks+1)*FD_BLOCK_SIZE) < FD_END_ADDRESS) {
		// valid blocks
		osMutexAcquire(FD_MutexID, osWaitForever);
		FDSPI_readData(pData, adr, NumOfBlocks * FD_BLOCK_SIZE);
		osMutexRelease(FD_MutexID);

		retr = SD_OK;
	}
	/* Return the reponse */
	return retr;
}


/**
  * @brief
  *     Writes block(s) to a specified address to the internal flash.
  *
  *     The flash is divided in 4 KiB (erasable) sectors.
  *     If any byte in the block is not 0xFF the sector where the block
  *     belongs to has to be erased. The whole sector
  *     has to be saved before.
  * @param
  *     pData: Pointer to the buffer that will contain the data to transmit
  * @param
  *     BlockAdr: Address where the data will be written. The address is counted
  *                   in blocks of 512bytes
  * @param
  *     NumOfBlocks: Number of FD blocks to write
  * @retval
  *     FD status
  */
uint8_t FD_WriteBlocks(uint8_t *pData, uint32_t BlockAdr, uint32_t NumOfBlocks) {
	uint8_t retr = SD_ERROR;
	int i;
	uint8_t block_field; // bit0 block0, bit1 block1 ..

	uint32_t base_block_adr = BlockAdr & (~ (FD_BLOCKS_PER_SECTOR -1));

	if ((NumOfBlocks+1)*FD_BLOCK_SIZE < FD_END_ADDRESS) {
		// valid blocks

		osMutexAcquire(FD_MutexID, osWaitForever);

		int FirstBoundary = BlockAdr % FD_BLOCKS_PER_SECTOR;
		int LastBoundary = (BlockAdr + NumOfBlocks) % FD_BLOCKS_PER_SECTOR;
		int sectors = NumOfBlocks / FD_BLOCKS_PER_SECTOR;
		if (FirstBoundary + LastBoundary > 0) {
			sectors++;
		}

		uint32_t base_flash_addr = base_block_adr * FD_BLOCK_SIZE;
		retr = SD_OK;
		for (i=0; i < sectors; i++) {
			if (i==0 && i == sectors -1) {
				// only one sector
				if (LastBoundary == 0) {
					block_field = 0xFF << FirstBoundary;
				} else {
					block_field = (0xFF << FirstBoundary) & (0xFF >> (FD_BLOCKS_PER_SECTOR - LastBoundary));
				}
			} else if (i==0) {
				// first sector
				block_field = 0xFF << FirstBoundary;
			} else if (i == sectors -1) {
				// last sector
				if (LastBoundary == 0) {
					block_field = 0xFF;
				} else {
					block_field = 0xFF >> LastBoundary;
				}
			} else {
				block_field = 0xFF;
			}
			if (flash_sector(
					pData +  i*FD_SECTOR_SIZE, 			// address data source (contiguous)
					base_flash_addr + i*FD_SECTOR_SIZE,	// sector base address flash dest
					block_field)						// valid blocks bitfield
					!= SD_OK) {
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


/**
  * @brief
  *     Erase Drive (Chip Erase)
  *
  * @retval
  *     FD status
  */
uint8_t FD_eraseDrive(void) {
	uint8_t retr = SD_ERROR;

	osMutexAcquire(FD_MutexID, osWaitForever);
	retr = FDSPI_eraseChip();
	osMutexRelease(FD_MutexID);
	return retr;
}


// Private Functions
// *****************


/**
  * @brief
  *     Writes blocks to a flash sector.
  *
  *     The 512 bytes blocks have to be contiguous and marked in a bitfield.
  * @param
  *     pData: Pointer to the buffer that will contain the data blocks
  * @param
  *     flash_addr: flash address
  * @param
  *     block_field: bit0 is block 0, bit1 is block 1 and so on. 8 blocks.
  * @retval
  *     FD status
  */
static int flash_sector(uint8_t *pData, uint32_t flash_addr, uint16_t block_field) {
	uint8_t retr = SD_OK;
	int i, j;
	uint8_t *byte_p;
	uint8_t erased = TRUE;

	// read out 4 KiB serial flash sector into scratch_sector
	FDSPI_readData(scratch_sector, flash_addr, FD_SECTOR_SIZE);

	// are the blocks in the sector already erased?
	byte_p = (uint8_t *) scratch_sector;
	for (i=0; i<FD_BLOCKS_PER_SECTOR; i++) {
		if (block_field & (1 << i)) {
			// block belongs to sector
			for (j=0; j<FD_BLOCK_SIZE; j++) {
				if (*(byte_p + j) != 0xFF) {
					erased = FALSE;
					break;
				}
			}
			if (!erased) {
				break;
			}
		}
		byte_p += FD_BLOCK_SIZE;
	}

	// copy the blocks to the scratch buffer
	byte_p = pData;
	for (i=0; i<FD_BLOCKS_PER_SECTOR; i++) {
		if (block_field & (1 << i)) {
			// block belongs to sector -> copy
			memcpy(scratch_sector+i*FD_BLOCK_SIZE, byte_p, FD_BLOCK_SIZE);
			byte_p += FD_BLOCK_SIZE;
		}
	}

	if (erased) {
		// flash already erased

		// flash the blocks
		for (i=0; i<FD_BLOCKS_PER_SECTOR; i++) {
			if (block_field & (1 << i)) {
				// block belongs to sector -> program
				FDSPI_writeData(scratch_sector+i*FD_BLOCK_SIZE, flash_addr+i*FD_BLOCK_SIZE, FD_BLOCK_SIZE);
			}
		}
	} else {
		// flash not erased

		// erase sector
		FDSPI_eraseBlock(flash_addr);

		// flash the sector (all blocks)
		for (i=0; i<FD_BLOCKS_PER_SECTOR; i++) {
			FDSPI_writeData(scratch_sector+i*FD_BLOCK_SIZE, flash_addr+i*FD_BLOCK_SIZE, FD_BLOCK_SIZE);
		}
	}
	return retr;
}



