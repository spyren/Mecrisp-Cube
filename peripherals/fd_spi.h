/*
 * fd_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_FDSPI_H_
#define INC_FDSPI_H_

void FDSPI_init(void);
void FDSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void FDSPI_Write(uint8_t Value);


#endif /* INC_FDSPI_H_ */
