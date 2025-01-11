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
