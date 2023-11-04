/**
 *  @brief
 *  	MIP (Memory-in-Pixel LCD) driver for Sharp displays.
 *
 *  	Resolution 168x144 or 400x240, monochrome.
 *  	The minimum amount of data that can be written to the panel
 *  	with the Write Line command is one line.
 *  	168 bits or 400 bits (21 or 50 bytes), 8 pixels are in one byte.
 *
 *  	SPI Interface
 *  	  - Data bits: leftmost pixel first, little-endian format
 *  	  - Data is clocked on the rising edge of SCLK, Mode 0
 *  	    SPI_POLARITY_LOW, SPI_PHASE_1EDGE
 *  	  - serial chip select (SCS) is high active
 *
 *  	The display RAM can not be read, therefore the display content is
 *  	mirrored in a frame buffer.
 *
 *  	See https://www.mikrocontroller.net/topic/54860 for the fonts.
 *  @file
 *      mip.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2022-05-13
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

#ifndef INC_MIP_H_
#define INC_MIP_H_

#define MIP_LANDSCAPE		0
#define	MIP_PORTRAIT		1

// configure your MIP type
#define MIP_DISPLAY_TYPE	MIP_168X144				// MIP_168X144, MIP_400X240
#define	MIP_ORIENTATION		MIP_LANDSCAPE

#if MIP_DISPLAY_TYPE == MIP_168X144
	#define MIP_X_RESOLUTION	168
	#define MIP_Y_RESOLUTION	144
#elif MIP_DISPLAY_TYPE == MIP_400X240
	#define MIP_X_RESOLUTION	400
	#define MIP_Y_RESOLUTION	240
#endif

#define MIP_LINES			(MIP_Y_RESOLUTION / 8)

typedef enum {MIP_FONT6X8, MIP_FONT8X8, MIP_FONT8X16, MIP_FONT12X16} MIP_FontT;

void MIP_init(void);
void MIP_sendCommand(const uint8_t *command);
void MIP_clear(void);
void MIP_update(void);
void MIP_setPos(uint8_t x, uint8_t y);
uint8_t MIP_getPosX();
uint8_t MIP_getPosY();
int MIP_putc(int c);
int MIP_puts(const char *s);
void MIP_setFont(MIP_FontT font);
int MIP_readStatus(void);


#endif /* INC_MIP_H_ */
