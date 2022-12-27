/*
 * fd_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_FDSPI_H_
#define INC_FDSPI_H_

#define FDSPI_N25Q128		0
#define FDSPI_W25Q128		1
#define FDSPI_MX25L12835	2
#define FDSPI_S25FL128    	3

#define FDSPI_DEVICE 		FDSPI_W25Q128

void FDSPI_init(void);
int FDSPI_writeData(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
int FDSPI_readData(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
int FDSPI_eraseChip(void);
int FDSPI_eraseBlock(uint32_t BlockAddress);
int FDSPI_eraseSector(uint32_t flash_addr);

#endif /* INC_FDSPI_H_ */
