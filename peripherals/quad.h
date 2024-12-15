/**
 *  @brief
 *  	Quad Alphanumeric LED driver based on the controller HT16K33.
 *
 *  	14 + 1 segments, monochrome.
 *  	I2C Interface, address 0x70.
 *  @file
 *      quad.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-12-13
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

#ifndef INC_QUAD_H_
#define INC_QUAD_H_

#define QUAD_I2C_ADR		0x70

#define QUAD_X_RESOLUTION	4

void QUAD_init(void);
void QUAD_clear(void);
void QUAD_setPosX(uint8_t x);
uint8_t QUAD_getPosX();
int QUAD_putc(int c);
int QUAD_puts(const char *s);
void QUAD_shutdown(uint8_t status);
void QUAD_blink(uint8_t status);
void QUAD_dimm(uint8_t status);
void QUAD_setSegment(uint8_t column, uint8_t segment);
int QUAD_getSegment(uint8_t col, uint8_t row);
void QUAD_setDP(uint8_t dp);
#endif /* INC_QUAD_H_ */
