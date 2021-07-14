/**
 *  @brief
 *  	OLED driver based on the controller SSD1306.
 *
 *  	Resolution 128x32 or 128x64, monochrome.
 *  	I2C Interface.
 *  @file
 *      oled.h
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

#define OLED_I2C_ADR		60

//#define	BONNET				1
//#define	SH1107				1
#define	SH1107_PORTRAIT		1

#ifdef SH1107_PORTRAIT
#define OLED_X_RESOLUTION	64
#else
#define OLED_X_RESOLUTION	128
#endif

#ifdef BONNET
#define OLED_Y_RESOLUTION	64
#endif

#ifdef SH1107_PORTRAIT
#define OLED_Y_RESOLUTION	128
#endif

#ifdef SH1107
#define OLED_Y_RESOLUTION	64
#endif

#ifndef OLED_Y_RESOLUTION
#define OLED_Y_RESOLUTION	32
#endif

#define OLED_LINES			(OLED_Y_RESOLUTION / 8)

typedef enum {OLED_FONT6X8, OLED_FONT8X8, OLED_FONT8X16, OLED_FONT12X16} OLED_FontT;

void OLED_init(void);
void OLED_sendCommand(const uint8_t *command);
void OLED_clear(void);
void OLED_setPos(uint8_t x, uint8_t y);
uint8_t OLED_getPosX();
uint8_t OLED_getPosY();
int OLED_putc(int c);
int OLED_puts(const char *s);
void OLED_setFont(OLED_FontT font);

#endif /* INC_OLED_H_ */
