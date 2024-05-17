/**
 *  @brief
 *  	Cross-Plex LED driver based on the controller IS31FL3731.
 *
 *  	Resolution 15x7, monochrome.
 *  	The 7 vertical pixels are in one byte.
 *  	There are 8 independent frames.
 *  	I2C Interface, address 0x74.
 *  	See https://www.mikrocontroller.net/topic/54860 for the fonts.
 *  @file
 *      oled.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2021-04-30
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

#ifndef INC_PLEX_H_
#define INC_PLEX_H_

#define PLEX_I2C_ADR		0x74

//#define	BONNET				1

#define PLEX_X_RESOLUTION	15
#define PLEX_Y_RESOLUTION	7

typedef enum {PLEX_FONT6X8, PLEX_FONT8X8} PLEX_FontT;

void PLEX_init(void);
void PLEX_clear(void);
void PLEX_setPos(uint8_t x);
uint8_t PLEX_getPosX();
int PLEX_putc(int c);
int PLEX_puts(const char *s);
void PLEX_setFont(PLEX_FontT font);
void PLEX_shutdown(uint8_t status);
void PLEX_setColumn(uint8_t col, uint8_t leds, int brightness);
int PLEX_getColumn(uint8_t col);
void PLEX_setPixel(uint8_t col, uint8_t row, int brightness);
void PLEX_setFrame(uint8_t frame);
int PLEX_getFrame(void);
void PLEX_setDisplay(uint8_t frame);
int PLEX_getDisplay(void);

#endif /* INC_PLEX_H_ */
