/*
 * sd_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_SDSPI_H_
#define INC_SDSPI_H_

void SDSPI_init(void);
void SDSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void SDSPI_Write(uint8_t Value);
void SDSPI_WriteEPD(uint8_t *Data, uint16_t DataLength);
void SDSPI_WriteMIP(uint8_t *Data, uint16_t DataLength);


#endif /* INC_SDSPI_H_ */
