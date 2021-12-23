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

#define OLED_4PIN_128X32	0		// the cheap one with 4 pins, Adafruit Raspberry Pi #3527
#define	OLED_BONNET_128X64	1		// Adafruit Bonnet for Raspberry Pi #3531
#define OLED_FEATHER_128X32	2		// Adafruit FeatherWing #2900
#define OLED_FEATHER_128X64	3		// Adafruit FeatherWing #4650
#define OLED_OCTOPUS_128X64	4		// Octopus PIS-1277

#define OLED_LANDSCAPE		0
#define	OLED_PORTRAIT		1

// configure your OLED type
#define OLED_DISPLAY_TYPE	OLED_FEATHER_128X64
#define	OLED_ORIENTATION	OLED_LANDSCAPE

#define OLED_I2C_ADR		60		// 0x3c

#define OLED_SSD1306		0
#define OLED_SH1107			1
#define OLED_DRIVER			OLED_SSD1306

#if OLED_DISPLAY_TYPE == OLED_4PIN_128X32
	#define OLED_X_RESOLUTION	128
	#define OLED_Y_RESOLUTION	32
#elif OLED_DISPLAY_TYPE == OLED_BONNET_128X64
	#define OLED_X_RESOLUTION	128
	#define OLED_Y_RESOLUTION	64
#elif OLED_DISPLAY_TYPE == OLED_FEATHER_128X32
	#define OLED_X_RESOLUTION	128
	#define OLED_Y_RESOLUTION	32
#elif OLED_DISPLAY_TYPE == OLED_FEATHER_128X64
	#undef OLED_DRIVER
	#define OLED_DRIVER			OLED_SH1107
	#if OLED_ORIENTATION == OLED_LANDSCAPE
		#define OLED_X_RESOLUTION	128
		#define OLED_Y_RESOLUTION	64
		#define	SH1107_LANDSCAPE	1
		#define OLED_PAGE_VERTICAL	1
	#else
		#define OLED_X_RESOLUTION	64
		#define OLED_Y_RESOLUTION	128
	#endif
#elif OLED_DISPLAY_TYPE == OLED_OCTOPUS_128X64
	#define OLED_X_RESOLUTION	128
	#define OLED_Y_RESOLUTION	64
#endif

#define OLED_LINES			(OLED_Y_RESOLUTION / 8)

typedef enum {OLED_FONT6X8, OLED_FONT8X8, OLED_FONT8X16, OLED_FONT12X16} OLED_FontT;

void OLED_init(void);
void OLED_sendCommand(const uint8_t *command);
void OLED_clear(void);
void OLED_update(void);
void OLED_setPos(uint8_t x, uint8_t y);
uint8_t OLED_getPosX();
uint8_t OLED_getPosY();
int OLED_putc(int c);
int OLED_puts(const char *s);
void OLED_setFont(OLED_FontT font);
int OLED_readStatus(void);


#endif /* INC_OLED_H_ */
