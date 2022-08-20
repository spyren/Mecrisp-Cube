/*
 * rt_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_RTSPI_H_
#define INC_RTSPI_H_

extern osMutexId_t RTSPI_MutexID;

void RTSPI_init(void);
void RTSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
void RTSPI_WriteData(const uint8_t *Data, uint16_t DataLength);
void RTSPI_ReadData(const uint8_t *Data, uint16_t DataLength);
void RTSPI_Write(uint8_t Value);
void RTSPI_WriteEPD(uint8_t *Data, uint16_t DataLength);
void RTSPI_WriteMIP(uint8_t *Data, uint16_t DataLength);


#endif /* INC_RTSPI_H_ */
