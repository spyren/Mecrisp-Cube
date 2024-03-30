/*
 * sd_spi.h
 *
 *  Created on: 2024-03-30
 *      Author: psi
 */

#ifndef INC_SDSPI_H_
#define INC_SDSPI_H_

extern osMutexId_t SDSPI_MutexID;
extern volatile int SDSPI_SpiStatus;
extern osSemaphoreId_t SDSPI_SemaphoreID;

void SDSPI_init(void);
uint32_t* SDSPI_getMutex(void);
int SDSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
int SDSPI_WriteData(const uint8_t *Data, uint16_t DataLength);
int SDSPI_ReadData(const uint8_t *Data, uint16_t DataLength);
int SDSPI_Write(uint8_t Value);

#endif /* INC_SDSPI_H_ */
