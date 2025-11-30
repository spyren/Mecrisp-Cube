/**
 *  @brief
 *  	Simple buttons (4) or matrix buttons (5x7)
 *
 *  	Simple Buttons
 *  	e.g. for egg timer
 *      Like a keyboard with the characters m (for minutes), s (seconds), S (start/stop),
 *      p (power on/off).
 *
 *  	Matrix Buttons
 *      Buffered calculator buttons
 *
 *      Columns are input ports with pull up resistors and interrupts,
 *      rows are open drain output ports.
 *
 *  @file
 *      button.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-12-15
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

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

extern osMutexId_t BUTTON_MutexID;

void BUTTON_init(void);
int BUTTON_getc(void);
int BUTTON_Ready(void);
int BUTTON_putkey(const char c);

#if BUTTON_MATRIX == 1
extern osSemaphoreId_t BUTTON_SemaphoreID;

#define COL0_Pin 		A3_Pin
#define COL0_GPIO_Port 	A3_GPIO_Port
#define COL1_Pin 		A2_Pin
#define COL1_GPIO_Port 	A2_GPIO_Port
#define COL2_Pin 		D1_Pin
#define COL2_GPIO_Port	D1_GPIO_Port
#define COL3_Pin 		D0_Pin
#define COL3_GPIO_Port	D0_GPIO_Port
#define COL4_Pin 		D13_Pin
#define COL4_GPIO_Port	D13_GPIO_Port
#define ROW0_Pin 		JTDO_Pin
#define ROW0_GPIO_Port	JTDO_GPIO_Port
#define ROW1_Pin 		DONGLE_SPI_CS_Pin
#define ROW1_GPIO_Port	DONGLE_SPI_CS_GPIO_Port
#define ROW2_Pin 		D12_Pin
#define ROW2_GPIO_Port	D12_GPIO_Port
#define ROW3_Pin		D11_Pin
#define ROW3_GPIO_Port	D11_GPIO_Port
#define ROW4_Pin		STLINK_RX_Pin
#define ROW4_GPIO_Port	STLINK_RX_GPIO_Port
#define ROW5_Pin		D9_Pin
#define ROW5_GPIO_Port	D9_GPIO_Port
#define ROW6_Pin		D6_Pin
#define ROW6_GPIO_Port	D6_GPIO_Port

void BUTTON_OnOff(void);

#else
#define BUTTON_D_Pin			A2_Pin
#define BUTTON_D_GPIO_Port		A2_GPIO_Port
#define BUTTON_C_Pin			D0_Pin
#define BUTTON_C_GPIO_Port		D0_GPIO_Port
#define BUTTON_B_Pin			D1_Pin
#define BUTTON_B_GPIO_Port		D1_GPIO_Port
#define BUTTON_A_Pin			A3_Pin
#define BUTTON_A_GPIO_Port		A3_GPIO_Port


#endif

#endif /* INC_BUTTON_H_ */
