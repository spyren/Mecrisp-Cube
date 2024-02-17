/**
 *  @brief
 *      Buffered I2C (or IIC) communication.
 *
 *      Using interrupt and DMA for I2C1 peripheral.
 *      CMSIS-RTOS Mutex for mutual-exclusion I2C resource.
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


void IIC_init(void);
int IIC_ready(void);
int IIC_getMessage(uint8_t *RxBuffer, uint32_t size, uint16_t dev);
int IIC_putMessage(uint8_t *TxBuffer, uint32_t size, uint16_t dev);
int IIC_putGetMessage(uint8_t *TxRxBuffer, uint32_t TxSize, uint32_t RxSize, uint16_t dev);

#endif /* INC_UART_H_ */
