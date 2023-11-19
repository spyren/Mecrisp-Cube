/*
 * r_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_RSPI_H_
#define INC_RSPI_H_

extern osMutexId_t RSPI_MutexID;
extern volatile int RSPI_SpiStatus;
extern osSemaphoreId_t RSPI_SemaphoreID;

void RSPI_init(void);
uint32_t* RSPI_getMutex(void);
int RSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
int RSPI_WriteData(const uint8_t *Data, uint16_t DataLength);
int RSPI_ReadData(const uint8_t *Data, uint16_t DataLength);
int RSPI_Write(uint8_t Value);

#endif /* INC_RSPI_H_ */
