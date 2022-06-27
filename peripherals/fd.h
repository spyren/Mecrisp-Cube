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
#define FD_SECTOR_SIZE		0x1000	// 4 KiB sector (for erasing)
#define FD_BLOCKS_PER_SECTOR	(FD_SECTOR_SIZE / FD_BLOCK_SIZE)

#define FD_START_ADDRESS	0x09000000
#define FD_END_ADDRESS		0x09200000


void    FD_init(void);
void    FD_getSize(void);
int 	FD_getBlocks(void);

uint8_t FD_ReadBlocks(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks);
uint8_t FD_WriteBlocks(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks);
uint8_t FD_Erase(uint32_t StartAddr, uint32_t EndAddr);

#endif /* INC_SD_H_ */
