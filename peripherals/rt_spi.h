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
uint32_t* RTSPI_getMutex(void);
int RTSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
int RTSPI_WriteData(const uint8_t *Data, uint16_t DataLength);
int RTSPI_ReadData(const uint8_t *Data, uint16_t DataLength);
int RTSPI_Write(uint8_t Value);

#endif /* INC_RTSPI_H_ */
