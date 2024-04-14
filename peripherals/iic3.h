/**
 *  @brief
 *      Buffered I2C (or IIC) communication.
 *
 *      Using interrupt and DMA for I2C3 peripheral.
 *      CMSIS-RTOS Mutex for mutual-exclusion I2C resource.
 *      For Flipper external (GPIO) devices-
 *  @file
 *      iic.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-01-02
 *  @remark
 *      Language: C, STM32CubeIDE GCC
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This project Mecrsip-Cube is free software: you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation, either version 3 of
 *      the License, or (at your option) any later version.
 *
 *      Mecrsip-Cube is distributed in the hope that it will be useful, but
 *      WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *      General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.
 */



#ifndef INC_IIC3_H_
#define INC_IIC3_H_

#define IIC3_TIMEOUT	500

extern volatile int IIC3_Status;
extern osSemaphoreId_t IIC3_SemaphoreID;

void IIC3_init(void);
int IIC3_ready(void);
int IIC3_getMessage(uint8_t *RxBuffer, uint32_t size, uint16_t dev);
int IIC3_putMessage(uint8_t *TxBuffer, uint32_t size, uint16_t dev);
int IIC3_putGetMessage(uint8_t *TxRxBuffer, uint32_t TxSize, uint32_t RxSize, uint16_t dev);

#endif /* INC_UART_H_ */
