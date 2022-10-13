/**
 *  @brief
 *  	E-Paper Display (EPA, E-Ink) driver for displays SSD1680 Drivers.
 *
 *  	Resolution (from Adafruit):
 *  	  - 2.13" #4195 250x122
 *  	  - 1.54" 200x200,
 *  	  - 2.9"  296x128
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
 *      epd.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2022-08-08
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

#ifndef INC_EPD_H_
#define INC_EPD_H_



#define EPD_LANDSCAPE		0
#define	EPD_PORTRAIT		1

#define EPD_ECS_Pin			D9_Pin			// EPD Chip Select
#define EPD_ECS_GPIO_Port	D9_GPIO_Port
#define EPD_DC_Pin			D11_Pin			// EPD Command/Control
#define EPD_DC_GPIO_Port	D11_GPIO_Port
#define EPD_BUSY_Pin		D12_Pin			// EPD Busy (optional)
#define EPD_BUSY_GPIO_Port	D12_GPIO_Port
#define EPD_RST_Pin			D13_Pin			// EPD Reset (optional)
#define EPD_RST_GPIO_Port	D13_GPIO_Port

// configure your EPD type
#define EPD_DISPLAY_TYPE	EPD_250X122		// EPD_168X144, EPD_400X240
#define	EPD_ORIENTATION		EPD_LANDSCAPE

#if EPD_DISPLAY_TYPE == EPD_250X122
	#define EPD_X_RESOLUTION	250
	#define EPD_Y_RESOLUTION	122			// actually 128
#elif EPD_DISPLAY_TYPE == EPD_200X200
	#define EPD_X_RESOLUTION	200
	#define EPD_Y_RESOLUTION	200
#endif

#define EPD_LINES			(EPD_Y_RESOLUTION / 8 + 1)
#define EPD_COLUMNS			EPD_X_RESOLUTION

#define EPD_BUSY_PIN		1
#define EPD_RST_PIN			1

#define EPD_BUSY_WAIT_MS	500

typedef enum {
	EPD_FONT6X8, EPD_FONT8X8, EPD_FONT8X16, EPD_FONT12X16,
	EPD_FONT8X16B, EPD_FONT8X16S,
	EPD_FONT10X24B, EPD_FONT10X24S,
	EPD_FONT11X24B, EPD_FONT11X24S,
	EPD_FONT16X32B, EPD_FONT16X32S,
	EPD_FONT9X16B, EPD_FONT9X16S,
	EPD_FONT13X24B, EPD_FONT13X24S
} EPD_FontT;

void EPD_init(void);
void EPD_sendCommand(const uint8_t *command);
void EPD_clear(void);
void EPD_update(void);
void EPD_setPos(uint8_t x, uint8_t y);
uint8_t EPD_getPosX();
uint8_t EPD_getPosY();
int EPD_putc(int c);
int EPD_puts(const char *s);
void EPD_setFont(EPD_FontT font);
int EPD_readStatus(void);


#endif /* INC_EPD_H_ */
