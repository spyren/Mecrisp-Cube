/*
 * spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

void SPI_init(void);
void SPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void SPI_Write(uint8_t Value);


#endif /* INC_SPI_H_ */
