/*
 * fd_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_FDSPI_H_
#define INC_FDSPI_H_

void FDSPI_init(void);
int FDSPI_writeData(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
int FDSPI_readData(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
int FDSPI_eraseChip(void);
int FDSPI_eraseBlock(uint32_t BlockAddress);

#endif /* INC_FDSPI_H_ */
