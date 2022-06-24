/*
 * ext_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_EXTSPI_H_
#define INC_EXTSPI_H_

void EXTSPI_init(void);
void EXTSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void EXTSPI_Write(uint8_t Value);


#endif /* INC_EXTSPI_H_ */
