/**
 *  @brief
 *      Buffered Flipper buttons (keyboard)
 *
 *  @file
 *      uart.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-02-19
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

#define BUTTON_MINUTE_Pin			A2_Pin
#define BUTTON_MINUTE_GPIO_Port		A2_GPIO_Port
#define BUTTON_SECOND_Pin			D0_Pin
#define BUTTON_SECOND_GPIO_Port		D0_GPIO_Port
#define BUTTON_START_STOP_Pin		D1_Pin
#define BUTTON_START_STOP_GPIO_Port	D1_GPIO_Port
#define BUTTON_POWER_Pin			A3_Pin
#define BUTTON_POWER_GPIO_Port		A3_GPIO_Port


void BUTTON_init(void);
int BUTTON_getc(void);
int BUTTON_Ready(void);
int BUTTON_putkey(const char c);

#endif /* INC_BUTTON_H_ */
