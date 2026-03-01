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
void BUTTON_OnOff(void);

#if BUTTON == 1
#if BUTTON_MATRIX == 1
extern osSemaphoreId_t BUTTON_SemaphoreID;

#define COL0_Pin 		A0_Pin
#define COL0_GPIO_Port 	A0_GPIO_Port
#define COL1_Pin 		A1_Pin
#define COL1_GPIO_Port 	A1_GPIO_Port
#define COL2_Pin 		A2_Pin
#define COL2_GPIO_Port	A2_GPIO_Port
#define COL3_Pin 		A3_Pin
#define COL3_GPIO_Port	A3_GPIO_Port
#define COL4_Pin 		D4_Pin
#define COL4_GPIO_Port	D4_GPIO_Port
#define ROW0_Pin 		D2_Pin
#define ROW0_GPIO_Port	D2_GPIO_Port
#define ROW1_Pin 		D3_Pin
#define ROW1_GPIO_Port	D3_GPIO_Port
#define ROW2_Pin 		D5_Pin
#define ROW2_GPIO_Port	D5_GPIO_Port
#define ROW3_Pin		D6_Pin
#define ROW3_GPIO_Port	D7_GPIO_Port
#define ROW4_Pin		D8_Pin
#define ROW4_GPIO_Port	D8_Port
#define ROW5_Pin		D9_Pin
#define ROW5_GPIO_Port	D9_GPIO_Port
#define ROW6_Pin		D13_Pin
#define ROW6_GPIO_Port	D13_GPIO_Port

#define COL0_IRQ		EXTI0_IRQn
#define COL1_IRQ		EXTI1_IRQn
#define COL2_IRQ		EXTI2_IRQn
#define COL3_IRQ		EXTI3_IRQn
#define COL4_IRQ		EXTI4_IRQn

#define COL0_EXTI_LINE	LL_EXTI_LINE_0
#define COL1_EXTI_LINE	LL_EXTI_LINE_1
#define COL2_EXTI_LINE	LL_EXTI_LINE_2
#define COL3_EXTI_LINE	LL_EXTI_LINE_3
#define COL4_EXTI_LINE	LL_EXTI_LINE_4


#else
#define BUTTON_H_Pin			D9_Pin
#define BUTTON_H_GPIO_Port		D9_GPIO_Port
#define BUTTON_G_Pin			D8_Pin
#define BUTTON_G_GPIO_Port		D8_GPIO_Port
#define BUTTON_F_Pin			D7_Pin
#define BUTTON_F_GPIO_Port		D7_GPIO_Port
#define BUTTON_E_Pin			D6_Pin
#define BUTTON_E_GPIO_Port		D6_GPIO_Port
#define BUTTON_D_Pin			D5_Pin
#define BUTTON_D_GPIO_Port		D5_GPIO_Port
#define BUTTON_C_Pin			D4_Pin
#define BUTTON_C_GPIO_Port		D4_GPIO_Port
#define BUTTON_B_Pin			D3_Pin
#define BUTTON_B_GPIO_Port		D3_GPIO_Port
#define BUTTON_A_Pin			D2_Pin
#define BUTTON_A_GPIO_Port		D2_GPIO_Port

#endif
#endif

#endif /* INC_BUTTON_H_ */
