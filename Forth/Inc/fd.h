/*
 * fd.h
 *
 *  Created on: 27.20.2020
 *      Author: psi
 */

#ifndef INC_FD_H_
#define INC_FD_H_


/**
  * @brief  Block Size
  */
#define FD_BLOCK_SIZE		0x200	// 512 bytes in a block
#define FD_PAGE_SIZE		0x1000	// 4 KiB page for the W25Q128
#define FD_BLOCKS_PER_PAGE	(FD_PAGE_SIZE / FD_BLOCK_SIZE)
#define FD_FLASH_RECORD		8		// double word

// 16 MiB
#define FD_START_ADDRESS	0x00000000
#define FD_END_ADDRESS		0x01FFFFFF

void    FD_init(void);
void    FD_getSize(void);
int 	FD_getBlocks(void);

uint8_t FD_ReadBlocks(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t FD_WriteBlocks(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t FD_Erase(uint32_t StartAddr, uint32_t EndAddr);
uint8_t FD_eraseDrive(void);

#endif /* INC_SD_H_ */
