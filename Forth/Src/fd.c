/**
 *  @brief
 *      Serial Flash drive block read and write.
 *
 *      The on-board 2 MiB Serial Flash W25Q16 is used for the flash drive.
 *      API similar to sd card.
 *
 *      From data sheet:
 *      The W25Q16JV array is organized into 8,192 programmable pages of 256-bytes each.
 *      Up to 256 bytes can be programmed at a time. Pages can be erased in groups of
 *      16 (4KB sector erase), groups of 128 (32KB block erase), groups of 256 (64KB
 *      block erase) or the entire chip (chip erase). The W25Q16JVhas 512 erasable
 *      sectors and 32 erasable blocks respectively. The small 4KB sectors allow for
 *      greater flexibility in applications that require data and parameter storage
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


// Defines
// *******
#define FLASH_DUMMY_BYTE	0xFF

// use CCM for scratch sector buffer
#define	CCM_RAM				(0x10000000 + 50 * 1024)

#define W25Q16_PAGES		(8192)
#define W25Q16_SECTORS		(8192/16)

#define W25Q16_PAGE_SIZE	(256)
#define W25Q16_SECTOR_SIZE	(256*16)

// Serial Flash commands
#define READ_CMD    (0x03)		// Read Data
#define WRITE_CMD   (0x02)		// Page Program (max. 256 bytes)
#define WREN_CMD    (0x06)		// Write Enable
#define WRDI_CMD    (0x04)		// Write Disable
#define RDSR_CMD    (0x05)		// Read Status Register 1
#define WRSR_CMD    (0x01)		// Write Status Register 1
#define SE_CMD 		(0x20)		// Sector Erase – erase one sector in memory array

// Private function prototypes
// ***************************
static int flash_sector(uint8_t *pData, uint32_t flash_addr, uint16_t block_field);
static int flash_block(uint8_t *pData, uint32_t flash_addr);
static void erase_sector(uint32_t flash_addr);


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
//	scratch_sector = (uint8_t *) CCM_RAM;
	scratch_sector = pvPortMalloc(W25Q16_SECTOR_SIZE);
	*scratch_sector = 0xaa;
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
	FD_size = W25Q16_PAGES / 4;
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

	if ((ReadAddr+NumOfBlocks+1)*FD_BLOCK_SIZE < FD_END_ADDRESS) {
		// valid blocks
		osMutexAcquire(FD_MutexID, osWaitForever);

		HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
		FDSPI_Write(READ_CMD);
		FDSPI_Write((ReadAddr*FD_BLOCK_SIZE & 0xFF0000) >> 16);
		FDSPI_Write((ReadAddr*FD_BLOCK_SIZE & 0x00FF00) >> 8);
		FDSPI_Write((ReadAddr*FD_BLOCK_SIZE & 0x0000FF));
		FDSPI_WriteReadData(pData, pData, NumOfBlocks * FD_BLOCK_SIZE);
		HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

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

	uint32_t base_block_adr = WriteAddr & (~ (FD_BLOCKS_PER_PAGE -1));

	if ((NumOfBlocks+1)*FD_BLOCK_SIZE < FD_END_ADDRESS) {
		// valid blocks

		osMutexAcquire(FD_MutexID, osWaitForever);

		int FirstBoundary = WriteAddr % FD_BLOCKS_PER_PAGE;
		int LastBoundary = (WriteAddr + NumOfBlocks) % FD_BLOCKS_PER_PAGE;
		int sectors = NumOfBlocks / FD_BLOCKS_PER_PAGE;
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
					block_field = (0xFF << FirstBoundary) & (0xFF >> (FD_BLOCKS_PER_PAGE - LastBoundary));
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
					pData +  i*FD_PAGE_SIZE, 			// dress data source (contiguous)
					base_flash_addr + i*FD_PAGE_SIZE,	// sector base address flash dest
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
  *     flash_addr: sector address (4 KiB sectors).
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

	// read out 4 KiB serial flash sector
	FD_ReadBlocks(scratch_sector, flash_addr, W25Q16_SECTOR_SIZE);

	// are the blocks in the sector already erased?
	byte_p = (uint8_t *) scratch_sector;
	for (i=0; i<FD_BLOCKS_PER_PAGE; i++) {
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

	if (erased) {
		// flash already erased
		// flash the blocks
		for (i=0; i<FD_BLOCKS_PER_PAGE; i++) {
			if (block_field & (1 << i)) {
				// block belongs to sector -> program
				flash_block(pData+i*FD_BLOCK_SIZE, flash_addr+i*FD_BLOCK_SIZE);
			}
		}
	} else {
		// flash not erased

		// copy the blocks to the scratch buffer
		byte_p = pData;
		for (i=0; i<FD_BLOCKS_PER_PAGE; i++) {
			if (block_field & (1 << i)) {
				// block belongs to sector -> copy
				memcpy(scratch_sector+i*FD_BLOCK_SIZE, byte_p, FD_BLOCK_SIZE);
			}
			byte_p += FD_BLOCK_SIZE;
		}

		// erase sector
		erase_sector(flash_addr);
		// flash the sector
		for (i=0; i<FD_BLOCKS_PER_PAGE; i++) {
			flash_block(scratch_sector+i*FD_BLOCK_SIZE, flash_addr+i*FD_BLOCK_SIZE);
		}
	}
	return retr;
}


/**
  * @brief
  *     Writes a block to 2 flash pages.
  * @param
  *     pData: Pointer to the buffer that will contain the data blocks
  * @param
  *     flash_addr: sector address (4 KiB sectors).
  * @retval
  *     FD status
  */
static int flash_block(uint8_t *pData, uint32_t flash_addr) {

	osMutexAcquire(FD_MutexID, osWaitForever);

	// first page
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	FDSPI_Write(WREN_CMD);
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	FDSPI_Write(WRITE_CMD);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0xFF0000) >> 16);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0x00FF00) >> 8);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0x0000FF));
	FDSPI_WriteReadData(pData, pData, W25Q16_PAGE_SIZE);
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

	// second page
	flash_addr += 256;
	pData += 256;
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	FDSPI_Write(WRITE_CMD);
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	FDSPI_Write(WRITE_CMD);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0xFF0000) >> 16);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0x00FF00) >> 8);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0x0000FF));
	FDSPI_WriteReadData(pData, pData, W25Q16_PAGE_SIZE);
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

	osMutexRelease(FD_MutexID);

}


/**
  * @brief
  *     Erases a flash page.
  * @param
  *     flash_addr: sector address (4 KiB sectors).
  * @retval
  *     FD status
  */
static void erase_sector(uint32_t flash_addr) {
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	FDSPI_Write(WREN_CMD);
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_RESET);
	FDSPI_Write(SE_CMD);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0xFF0000) >> 16);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0x00FF00) >> 8);
	FDSPI_Write((flash_addr*FD_BLOCK_SIZE & 0x0000FF));
	HAL_GPIO_WritePin(FLASH_CS_GPIO_Port, FLASH_CS_Pin, GPIO_PIN_SET);

}
