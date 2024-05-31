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
extern osSemaphoreId_t BUTTON_SemaphoreID;

void BUTTON_init(void);
int BUTTON_getc(void);
int BUTTON_Ready(void);
int BUTTON_putkey(const char c);

#endif /* INC_BUTTON_H_ */
