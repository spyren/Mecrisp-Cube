/**
 *  @brief
 *  @file
 *      oled.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-12-30
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

#ifndef INC_OLED_H_
#define INC_OLED_H_


void OLED_init(void);
void OLED_sendCommand(const uint8_t *command);
void OLED_setPos(uint8_t x, uint8_t y);
void OLED_sendChar(char ch);
void OLED_sendCharLarge(char ch);

#endif /* INC_OLED_H_ */
