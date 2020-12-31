/**
 *  @brief
*  @brief
 *      Buffered I2C (or IIC) communication.
 *
 *      Using interrupt for I2C1 peripheral. Separate threads for transmitting
 *      and receiving data. CMSIS-RTOS Mutex for mutual-exclusion I2C resource.
 *      CMSIS-RTOS queues as buffers.
 *  @file
 *      iic.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-12-29
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



#ifndef INC_IIC_H_
#define INC_IIC_H_

extern osMutexId_t UART_MutexID;

void IIC_init(void);
void IIC_setDevice(uint16_t dev);
int IIC_getMessage(uint8_t *RxBuffer, uint32_t size);
int IIC_putMessage(uint8_t *TxBuffer, uint32_t size);
int IIC_putGetMessage(uint8_t *TxBuffer, uint32_t TxSize, uint8_t *RxBuffer, uint32_t RxSize);

#endif /* INC_UART_H_ */
