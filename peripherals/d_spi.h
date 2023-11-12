/*
 * d_spi.h
 *
 *  Created on: 17.02.2020
 *      Author: psi
 */

#ifndef INC_DSPI_H_
#define INC_DSPI_H_

extern osMutexId_t DSPI_MutexID;

void DSPI_init(void);
uint32_t* DSPI_getMutex(void);
int DSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength);
int DSPI_WriteData(const uint8_t *Data, uint16_t DataLength);
int DSPI_ReadData(const uint8_t *Data, uint16_t DataLength);
int DSPI_Write(uint8_t Value);

#endif /* INC_DSPI_H_ */
