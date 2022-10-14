/**
 *  @brief
 *  	E-Paper Display (EPA, E-Ink) driver for displays
 *  	SSD1680 Drivers (max. 176x296).
 *
 *  	Resolution (from Adafruit):
 *  	  - 2.13" #4195 250x122
 *  	  - 1.54" 200x200,
 *  	  - 2.9"  296x128
 *
 *  	SPI Interface
 *  	  . 4-wire interface
 *  	  - Data bits: leftmost pixel first, little-endian format
 *  	  - Data is clocked on the rising edge of SCLK, Mode 0
 *  	    SPI_POLARITY_LOW, SPI_PHASE_1EDGE
 *  	  - serial chip select (ECS) is low active
 *  	  - DC pin: Data high, command low
 *
 *		Display
 *		  - Gates  0 .. 249, Y direction
 *		  - Source 0 .. 121, X direction
 *
 *  	The display RAM can not be read, therefore the display content is
 *  	mirrored in a frame buffer.
 *  	White pixel are 1s, black pixel are 0s.
 *  	One byte display memory contains 8 pixels.
 *  	Most significant bit is first pixel.
 *
 *  	Landscape
 *  	 - resolution 250 * 122 -> 250 columns * 15 1/4 rows
 *  	 - about 41 * 15 chars (8*6 chars)
 *  	 - Gate, Columns, left 249
 *  	 - Source, Rows, top 0
 *
 *  	See https://www.mikrocontroller.net/topic/54860 for the fonts.
 *  @file
 *      epd.c
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

// System include files
// ********************
#include "cmsis_os.h"
#include <stdio.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "epd.h"
#include "rt_spi.h"

#if EPD == 1
#include "stm32wbxx_ll_spi.h"
#include "font6x8.h"
#include "font8x8.h"
#include "font8x14.h"
#include "font12x16.h"
#include "font8x16b.h"
#include "font8x16s.h"
#include "font10x24b.h"
#include "font10x24s.h"
#include "font11x24b.h"
#include "font11x24s.h"
#include "font16x32b.h"
#include "font16x32s.h"
#include "font9x16b.h"
#include "font9x16s.h"
#include "font13x24b.h"
#include "font13x24s.h"
#include "nozzle.h"

// Macros
// ******
#define  membersof(x) (sizeof(x) / sizeof(x[0]))


// commands
#define SSD1680_DRIVER_CONTROL 	0x01
#define SSD1680_GATE_VOLTAGE 	0x03
#define SSD1680_SOURCE_VOLTAGE 	0x04
#define SSD1680_PROGOTP_INITIAL	0x08
#define SSD1680_PROGREG_INITIAL 0x09
#define SSD1680_READREG_INITIAL 0x0A
#define SSD1680_BOOST_SOFTSTART 0x0C
#define SSD1680_DEEP_SLEEP 		0x10
#define SSD1680_DATA_MODE 		0x11
#define SSD1680_SW_RESET 		0x12
#define SSD1680_TEMP_CONTROL 	0x18
#define SSD1680_TEMP_WRITE 		0x1A
#define SSD1680_MASTER_ACTIVATE 0x20
#define SSD1680_DISP_CTRL1 		0x21
#define SSD1680_DISP_CTRL2 		0x22
#define SSD1680_WRITE_RAM1 		0x24
#define SSD1680_WRITE_RAM2 		0x26
#define SSD1680_WRITE_VCOM 		0x2C
#define SSD1680_READ_OTP 		0x2D
#define SSD1680_READ_STATUS 	0x2F
#define SSD1680_WRITE_LUT 		0x32
#define SSD1680_WRITE_BORDER 	0x3C
#define SSD1680_SET_RAMXPOS 	0x44
#define SSD1680_SET_RAMYPOS 	0x45
#define SSD1680_SET_RAMXCOUNT 	0x4E
#define SSD1680_SET_RAMYCOUNT 	0x4F


// Private function prototypes
// ***************************
static int busy_wait(void);
//static void setPos(uint8_t x, uint8_t y);
static void putGlyph6x8(int ch);
static void putGlyph8x8(int ch);
static void putGlyph8x16(int ch);
static void putGlyph12x16(int ch);
static void putGlyph8x16B(int ch);
static void putGlyph8x16S(int ch);
static void putGlyph10x24B(int ch);
static void putGlyph10x24S(int ch);
static void putGlyph11x24B(int ch);
static void putGlyph11x24S(int ch);
static void putGlyph16x32B(int ch);
static void putGlyph16x32S(int ch);
static void putGlyph9x16B(int ch);
static void putGlyph9x16S(int ch);
static void putGlyph13x24B(int ch);
static void putGlyph13x24S(int ch);
static int autowrap(int ch, int width, int row);
#ifdef EPD_PAGE_VERTICAL
static void transpose_page(int page, int upper, uint8_t *buf);
#endif
static void postwrap(int width, int row);
uint8_t bitswap(uint8_t byte);


// Global Variables
// ****************

// Hardware resources
// ******************

// RTOS resources
// **************

// Private Variables
// *****************

static uint8_t epdReady = FALSE;

static uint8_t CurrentPosX = 0;
static uint8_t CurrentPosY = 0;

static EPD_FontT CurrentFont = EPD_FONT6X8;

typedef union {
   uint8_t blob[EPD_LINES * EPD_X_RESOLUTION];
   uint8_t rows[EPD_X_RESOLUTION][EPD_LINES];
} display_buffer_t;

static display_buffer_t *display_buffer;


// Public Functions
// ****************


/**
 *  @brief
 *      Initializes the EPD controller.
 *  @return
 *      None
 */
void EPD_init(void) {
	uint8_t buf[6];
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	// no chip select
	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_SET);
	GPIO_InitStruct.Pin = EPD_ECS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(EPD_ECS_GPIO_Port, &GPIO_InitStruct);

	// command
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
	GPIO_InitStruct.Pin = EPD_DC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(EPD_DC_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = EPD_BUSY_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(EPD_BUSY_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = EPD_RST_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(EPD_RST_GPIO_Port, &GPIO_InitStruct);

	// activate reset
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);

	if (busy_wait()) {
//	if (TRUE) {
		// timeout -> no display connected
		epdReady = FALSE;
		return;
	} else {
		epdReady = TRUE;
	}

	display_buffer = pvPortMalloc(sizeof(display_buffer_t));

	// SW Reset
	buf[0] = 1;
	buf[1] = SSD1680_SW_RESET;
	EPD_sendCommand(buf);
	busy_wait();

	// Set display size and driver output control
	// 250-1
	buf[0] = 4;
	buf[1] = SSD1680_DRIVER_CONTROL;
	buf[2] = EPD_X_RESOLUTION - 1;
	buf[3] = 0;
	buf[4] = 0;
	EPD_sendCommand(buf);

	// Ram data entry mode
	// 1: Y decrement, X increment (pixel line)
	// 3: Y increment, X increment, Adafruit
	buf[0] = 2;
	buf[1] = SSD1680_DATA_MODE;
	buf[2] = 3;
	EPD_sendCommand(buf);

	// set RAM X address range
	buf[0] = 3;
	buf[1] = SSD1680_SET_RAMXPOS;
	buf[2] = 0; 			// start
	buf[3] = EPD_LINES-1;		// end
	EPD_sendCommand(buf);

	// set RAM Y address range
	buf[0] = 5;
	buf[1] = SSD1680_SET_RAMYPOS;
	buf[2] = 0; 					// start lower
	buf[3] = 0; 					// start higer
	buf[4] = EPD_X_RESOLUTION - 1; 	// end lower
	buf[5] = 0; 					// end higer
	EPD_sendCommand(buf);

	// border color
	// A [1:0] GS Transition setting for VBD: 01 LUT1
	// A [2]   GS Transition control:          1 Follow LUT
	// A [5:4] Fix Level Setting for VBD:     00 VSS
	// A [7:6] Select VBD option:             00 GS Transition
	buf[0] = 2;
	buf[1] = SSD1680_WRITE_BORDER;
	buf[2] = 0x01;
	EPD_sendCommand(buf);

	// Vcom Voltage
	// 0x70: -2.8 V
	// 0x36: -1.35 ? Adafruit
	buf[0] = 2;
	buf[1] = SSD1680_WRITE_VCOM;
//	buf[2] = 0x36; // Adafruit
	buf[2] = 0x70;
	EPD_sendCommand(buf);

	// Set gate voltage
	// 0x17: 20 V
	buf[0] = 2;
	buf[1] = SSD1680_GATE_VOLTAGE;
	buf[2] = 0x17;
	EPD_sendCommand(buf);

	// Set source voltage
	buf[0] = 4;
	buf[1] = SSD1680_SOURCE_VOLTAGE;
	buf[2] = 0x41;
	buf[3] = 0x00;
	buf[4] = 0x32;
	EPD_sendCommand(buf);

	// RAM 0x26 bypass : enable
	buf[0] = 3;
	buf[1] = SSD1680_DISP_CTRL1;
	buf[2] = 0x40;
	buf[3] = 0x80;
	EPD_sendCommand(buf);

	// Load Waveform LUT
	// Select internal temperature sensor
	buf[0] = 2;
	buf[1] = SSD1680_TEMP_CONTROL;
	buf[2] = 0x80;
	EPD_sendCommand(buf);
	// Set Display update control: Enable clock, load TS value, load LUT from OTP and Clock off
	buf[0] = 2;
	buf[1] = SSD1680_DISP_CTRL2;
	buf[2] = 0xb1;
	EPD_sendCommand(buf);
	// Master Activation: Run display update sequence which is defined by Command 0x22
	buf[0] = 1;
	buf[1] = SSD1680_MASTER_ACTIVATE;
	EPD_sendCommand(buf);
	busy_wait();

	// Write always into RAM1 (B/W)
	buf[0] = 1;
	buf[1] = SSD1680_WRITE_RAM1;
	EPD_sendCommand(buf);

	// Set RAM X address counter
	buf[0] = 2;
	buf[1] = SSD1680_SET_RAMXCOUNT;
	buf[2] = 0;
	EPD_sendCommand(buf);

	// Set RAM Y address counter
	buf[0] = 3;
	buf[1] = SSD1680_SET_RAMYCOUNT;
	buf[2] = 0;
	buf[3] = 0;
	EPD_sendCommand(buf);

	EPD_clear();
//	memset(display_buffer->blob, 0x0F, 128);
//	EPD_update();

	EPD_setPos(0,0);

//	EPD_setFont(EPD_FONT16X32S);
//	EPD_puts("DejaVu20pt,");
//	EPD_setFont(EPD_FONT16X32B);
//	EPD_puts("15x3\r\n");
//	EPD_setFont(EPD_FONT11X24S);
//	EPD_puts("DejaVu 14 pt, 22x5 Z\r\n");
//	EPD_setFont(EPD_FONT11X24B);
//	EPD_puts("1234567890123456789012\r\n");
//	EPD_setFont(EPD_FONT8X16S);
//	EPD_puts("DejaVu Sans Mono 10 pt, 31x7 Z\r\n");
//	EPD_puts("1234567890123456789012345678901\r\n");
//	EPD_setFont(EPD_FONT8X16B);
//	EPD_puts("1234567890123456789012345678901\r\n");

//	EPD_setFont(EPD_FONT8X16S);
//	EPD_puts("DejaVu Sans Mono 10 pt, 31x7 Z\r\n");
//	EPD_puts("The quick brown fox jumps over\r\n");
//	EPD_puts("the lazy dog.\r\n");
//	EPD_puts("1234567890123456789012345678901\r\n");
//	EPD_setFont(EPD_FONT8X16B);
//	EPD_puts("The quick brown fox jumps over\r\n");
//	EPD_puts("the lazy dog.\r\n");
//	EPD_puts("1234567890123456789012345678901");

//	EPD_setFont(EPD_FONT10X24S);
//	EPD_puts("DejaVu Sans 12 pt, 25x5 Z");
//	EPD_puts("The quick brown fox jumps");
//	EPD_puts("over the lazy dog\r\n");
//	EPD_puts("1234567890123456789012345");
//	EPD_setFont(EPD_FONT10X24B);
//	EPD_puts("1234567890123456789012345");

	//	EPD_setFont(EPD_FONT11X24S);
//	EPD_puts("DejaVu 14 pt, 22x5 Z\r\n");
//	EPD_puts("The quick brown fox ju\r\n");
//	EPD_puts("mps over the lazy dog\r\n");
//	EPD_puts("1234567890123456789012\r\n");
//	EPD_setFont(EPD_FONT11X24B);
//	EPD_puts("1234567890123456789012");

//	EPD_setFont(EPD_FONT16X32S);
//	EPD_puts("DejaVu20pt,15x3\r\n");
//	EPD_puts("123456789012345\r\n");
//	EPD_setFont(EPD_FONT16X32B);
//	EPD_puts("The quick brown\r\n");
//	EPD_puts("fox jumps over\r\n");

//	EPD_setFont(EPD_FONT9X16S);
//	EPD_puts("DejaVuSansMono 11pt, 27x7 Z\r\n");
//	EPD_puts("The quick brown fox jumps\r\n");
//	EPD_puts("over the lazy dog.\r\n");
//	EPD_puts("123456789012345678901234567\r\n");
//	EPD_setFont(EPD_FONT9X16B);
//	EPD_puts("The quick brown fox jumps\r\n");
//	EPD_puts("over the lazy dog.\r\n");
//	EPD_puts("123456789012345678901234567\r\n");

//	EPD_setFont(EPD_FONT13X24S);
//	EPD_puts("DejaVu 16pt, 19x5 Z\r\n");
//	EPD_puts("The quick brown fox\r\n");
//	EPD_puts("jumps over the lazy\r\n");
//	EPD_puts("1234567890123456789\r\n");
//	EPD_setFont(EPD_FONT13X24B);
//	EPD_puts("1234567890123456789");

//	EPD_setFont(EPD_FONT8X8);
//	EPD_puts("IBM PC CGA Font 8x8 px, 31x15 Z\r\n");
//	EPD_puts("The quick brown fox jumps over\r\n");
//	EPD_puts("the lazy dog.\r\n");
//	EPD_puts("1234567890123456789012345678901\r\n");
//	EPD_puts("Lorem ipsum dolor sit amet, con\r\n");
//	EPD_puts("sectetur adipiscing elit, sed \r\n");
//	EPD_puts("do eiusmod tempor incididunt ut\r\n");
//	EPD_puts("labore et dolore magna aliqua.\r\n");
//	EPD_puts("Ut enim ad minim veniam, quis \r\n");
//	EPD_puts("nostrud exercitation ullamco\r\n");
//	EPD_puts("laboris nisi ut aliquip ex ea \r\n");
//	EPD_puts("commodo consequat. Duis aute  \r\n");
//	EPD_puts("irure dolor in reprehenderit in\r\n");
//	EPD_puts("voluptate velit esse cillum dol\r\n");
//	EPD_puts("ore eu fugiat nulla pariatur.");

//	// Small Font and 6 rows
//	EPD_setFont(EPD_FONT13X24B);
//	EPD_puts("Titel\r\n");
//	EPD_setFont(EPD_FONT9X16B);
//	//	EPD_puts("The quick brown fox\r\n");
//	EPD_puts("    Auswahl 1\r\n");
//	EPD_puts("    Auswahl 2\r\n");
//	EPD_puts("  \273 Auswahl 3\r\n");
//	EPD_puts("    Auswahl 4\r\n");
//	EPD_puts("    Auswahl 5\r\n");
//	EPD_puts("    Auswahl 6");

//	// Large Font and 4 rows
//	EPD_setFont(EPD_FONT13X24B);
//	EPD_puts("Titel\r\n");
//	EPD_setFont(EPD_FONT11X24S);
//	//	EPD_puts("The quick brown fox\r\n");
//	EPD_puts("  Auswahl 1\r\n");
//	EPD_puts("  Auswahl 2\r\n");
//	EPD_puts("\273 Auswahl 3\r\n");
//	EPD_puts("  Auswahl 4");

	// Large Font and 4 rows
	EPD_setFont(EPD_FONT11X24B);
	EPD_puts("Titel 14 pt bold\r\n");
	EPD_setFont(EPD_FONT10X24B);
	//	EPD_puts("The quick brown fox\r\n");
	EPD_puts("  Auswahl 1, 12 pt bold\r\n");
	EPD_puts("  Auswahl 2\r\n");
	EPD_puts("\273 Auswahl 3\r\n");
	EPD_puts("  Auswahl 4");

	EPD_setPos(150, 3);
	EPD_putXBM(nozzle_bits, nozzle_width, nozzle_height);

//	EPD_setFont(EPD_FONT6X8);
//	EPD_puts(MECRISP_CUBE_TAG);
//#ifdef DEBUG
//	EPD_puts("\r\nDebug\r\n");
//#else
//	EPD_puts("\r\n\r\n");
//#endif
//	EPD_puts("Forth for the STM32WB\r\n");
//	EPD_puts("(c)2022 peter@spyr.ch");
//
//	EPD_puts("\r\n\r\n");
//	EPD_setFont(EPD_FONT12X16);
//
//	EPD_puts("Mecrisp-Cube ");
//	EPD_puts(MECRISP_CUBE_TAG);
//	EPD_setFont(EPD_FONT8X16);
//#ifdef DEBUG
//	EPD_puts("\r\nDebug\r\n");
//#else
//	EPD_puts("\r\n\r\n");
//#endif
//	EPD_puts("Forth for the STM32WB\r\n");
//	EPD_puts("(c)2022 peter@spyr.ch");


	EPD_update();

}


/**
 *  @brief
 *      Sends a command to the EPD controller
 *
 *      Max. length of a command is 255 bytes.
 *  @param[in]
 *  	First byte contains the length of the command.
 *  @return
 *      None
 */
void EPD_sendCommand(const uint8_t *command) {
	if (!epdReady) {
		return;
	}

	// only one thread is allowed to use the SPI
	osMutexAcquire(RTSPI_MutexID, osWaitForever);

	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
	//	LL_SPI_SetTransferBitOrder(LL_SPI_LSB_FIRST);
	// dummy write to synchronize the CLK
	RTSPI_Write(0xff);

	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);	// command
	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_RESET); 	// chip select
	RTSPI_Write(command[1]);

	if (command[0] > 1) {
		// send parameters
		HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_SET);	// data
		RTSPI_WriteData(&command[2], command[0]-1);
		HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET); // command
	}

	// chip deselect
	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_SET);

	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_2EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_HIGH);
	//	LL_SPI_SetTransferBitOrder(LL_SPI_LSB_FIRST);
	// dummy write to synchronize the CLK
	RTSPI_Write(0xff);

	osMutexRelease(RTSPI_MutexID);
}


/**
 *  @brief
 *      Writes a char to the EPD. Blocking until char is written into the controller memory.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int EPD_putc(int c) {
	if (!epdReady) {
		return EOF;
	}

	if (c == '\r') {
		// carriage return
		EPD_setPos(0, CurrentPosY);
		return 0;
	}

	switch (CurrentFont) {
	case EPD_FONT6X8:
		putGlyph6x8(c);
		break;
	case EPD_FONT8X8:
		putGlyph8x8(c);
		break;
	case EPD_FONT8X16:
		putGlyph8x16(c);
		break;
	case EPD_FONT12X16:
		putGlyph12x16(c);
		break;
	case EPD_FONT8X16B:
		putGlyph8x16B(c);
		break;
	case EPD_FONT8X16S:
		putGlyph8x16S(c);
		break;
	case EPD_FONT10X24B:
		putGlyph10x24B(c);
		break;
	case EPD_FONT10X24S:
		putGlyph10x24S(c);
		break;
	case EPD_FONT11X24B:
		putGlyph11x24B(c);
		break;
	case EPD_FONT11X24S:
		putGlyph11x24S(c);
		break;
	case EPD_FONT16X32B:
		putGlyph16x32B(c);
		break;
	case EPD_FONT16X32S:
		putGlyph16x32S(c);
		break;
	case EPD_FONT9X16B:
		putGlyph9x16B(c);
		break;
	case EPD_FONT9X16S:
		putGlyph9x16S(c);
		break;
	case EPD_FONT13X24B:
		putGlyph13x24B(c);
		break;
	case EPD_FONT13X24S:
		putGlyph13x24S(c);
		break;
	}

	return 0;
}


/**
 *  @brief
 *      EPD ready for next char.
 *  @return
 *      FALSE if the transfer is ongoing.
 */
int EPD_Ready(void) {
	return TRUE;
}


/**
 *  @brief
 *      Writes a line (string) to the EPD. Blocking until
 *      string can be written.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      s  string to write. C style string.
 *  @return
 *      Return EOF on error, 0 on success.
 */
int EPD_puts(const char *s) {
	int i=0;

	while (s[i] != 0) {
		if (EPD_putc(s[i])) {
			return EOF;
		}
		i++;
	}
	return 0;
}


/**
 *  @brief
 *  @param[in]
 *      x  horizontal position, pixel resolution, e.g. 0 to 249 for 250x122 displays.
 *  @param[in]
 *      y  vertical position (line), max. 15 for  250x122 displays..
 *  @return
 *      none
 */
void EPD_setPos(uint8_t x, uint8_t y) {
	if (!epdReady) {
		return;
	}

	if ((x >= 0 && x < EPD_X_RESOLUTION) && (y >=0 && y < EPD_Y_RESOLUTION/8)) {
		// valid position
//		setPos(x, y);
		CurrentPosX = x;
		CurrentPosY = y;
	}
}


/**
 *  @brief
 *  	Gets the current horizontal position
 *  @return
 *      X horizontal position, max. (128 / 6) - 1
 */
uint8_t EPD_getPosX() {
	return CurrentPosX;
}


/**
 *  @brief
 *  	Gets the current vertical position
 *  @return
 *      Y vertical position, max. 3 for 128x32 or 7 for 128x64 displays.
 */
uint8_t EPD_getPosY() {
	return CurrentPosY;
}


/**
 *  @brief
 *  	Set the current font
 *  @param[in]
 *      font
 *  @return
 *      none
 */
void EPD_setFont(EPD_FontT font) {
	CurrentFont = font;
}


/**
 *  @brief
 *  	Clears the EPD display
 *  @return
 *      none
 */
void EPD_clear(void) {
	if (!epdReady) {
		return;
	}

	// fill with 0xff
	memset(display_buffer->blob, 0xff, sizeof(display_buffer->blob));

	EPD_setPos(0, 0);
}


/**
 *  @brief
 *      Write a column (8 pixels) to the current position
 *
 *      Increment the position.
 *  @param[in]
 *  	column
 *  @return
 *      None
 */
void EPD_writeColumn(uint8_t column) {
	if (autowrap(' ', 1, 1)) {
		return ;
	}

	display_buffer->rows[(EPD_COLUMNS-1) - CurrentPosX][CurrentPosY] = ~bitswap(column);

	postwrap(1, 1);
}


/**
 *  @brief
 *      Read a column (8 pixels) from the current position
 *  @return
 *      Column
 */
int EPD_readColumn(void) {
	return ~bitswap(display_buffer->rows[(EPD_COLUMNS-1) - CurrentPosX][CurrentPosY]);
}


/**
 *  @brief
 *  	Update the EPD display
 *  @return
 *      none
 */
void EPD_update(void) {
	uint8_t buf[5];

	if (!epdReady) {
		return;
	}

	// Set RAM X address counter
	buf[0] = 2;
	buf[1] = SSD1680_SET_RAMXCOUNT;
	buf[2] = 0;
	EPD_sendCommand(buf);

	// Set RAM Y address counter
	buf[0] = 3;
	buf[1] = SSD1680_SET_RAMYCOUNT;
	buf[2] = 0;
	buf[3] = 0;
	EPD_sendCommand(buf);

	// only one thread is allowed to use the SPI
	osMutexAcquire(RTSPI_MutexID, osWaitForever);

	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
	//	LL_SPI_SetTransferBitOrder(LL_SPI_LSB_FIRST);
	// dummy write to synchronize the CLK
	RTSPI_Write(0xff);

	// command
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
 	// chip select
	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_RESET);
	RTSPI_Write(SSD1680_WRITE_RAM1);

	// Data
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_SET);
	// copy framebuffer to display
	RTSPI_WriteData(display_buffer->blob, EPD_LINES * EPD_X_RESOLUTION);
	// Command
	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);
	busy_wait();

	// chip deselect
	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_SET);

	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_2EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_HIGH);
	//	LL_SPI_SetTransferBitOrder(LL_SPI_LSB_FIRST);
	// dummy write to synchronize the CLK
	RTSPI_Write(0xff);

	osMutexRelease(RTSPI_MutexID);

	// update display
	buf[0] = 2;
	buf[1] = SSD1680_DISP_CTRL2;
	buf[2] = 0xf7;
	EPD_sendCommand(buf);

	// Master Activation
	buf[0] = 1;
	buf[1] = SSD1680_MASTER_ACTIVATE;
	EPD_sendCommand(buf);
	busy_wait();
}


// Private Functions
// *****************

/**
 *  @brief
 *      Wait for BUSY low or timeout
 *  @return
 *      TRUE busy or Timeout, FALSE busy low or Timeout without BUSY
 */
static int busy_wait(void) {
	int timeout = EPD_BUSY_WAIT_MS;
#if EPD_BUSY_PIN == 1
	// with busy pin
    while (HAL_GPIO_ReadPin(EPD_BUSY_GPIO_Port, EPD_BUSY_Pin) == GPIO_PIN_SET) {
    	// wait for busy low
    	osDelay(1);
    	if (timeout-- == 0) {
    		// timeout occurred
    		return TRUE;
    	}
    }
    return 0;
#else
    delay(EPD_BUSY_WAIT_MS);
    return 0;
#endif
}


///**
// *  @brief
// *      Set the framebuffer position (page and column address)
// *  @param[in]
// *  	x 0 .. 127, column
// *  @param[in]
// *  	y 0 .. EPD_LINES, page/line
// *  @return
// *      None
// */
//static void setPos(uint8_t x, uint8_t y) {
//}


/**
 *  @brief
 *      Put a glyph in 6x8 font to the display
 *  @param[in]
 *  	ch code page 850
 *  @return
 *      None
 */
static void putGlyph6x8(int ch) {
#ifdef	EPD_PAGE_VERTICAL
	uint8_t buf[9];
#endif
	uint8_t i;

	if (autowrap(ch, 6, 1)) {
		return ;
	}

	// fill the buffer with 6 columns
	for (i = 0; i < 6; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY] = ~bitswap(FONT6X8_getColumn(ch, i));
	}


#ifdef	EPD_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 6) >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#endif

	postwrap(6, 1);
}


/**
 *  @brief
 *      Put a glyph in 8x8 font to the display
 *  @param[in]
 *  	ch code page 850
 *  @return
 *      None
 */
static void putGlyph8x8(int ch) {
#ifdef	EPD_PAGE_VERTICAL
//	uint8_t buf[9];
#endif
	uint8_t i;

	if (autowrap(ch, 8, 1)) {
		return ;
	}

	// fill the buffer with 8 columns
	for (i = 0; i < 8; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY] = ~bitswap(FONT8X8_getColumn(ch, i));
	}

#ifdef	EPD_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 8) >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#endif

	postwrap(8, 1);
}


/**
 *  @brief
 *      Put a glyph in 8x16 font to the display
 *  @param[in]
 *  	ch code page 850
 *  @return
 *      None
 */
static void putGlyph8x16(int ch) {
//	uint8_t buf[9];
	uint8_t i;

	if (autowrap(ch, 8, 2)) {
		return ;
	}

	// fill the buffer with 8 columns on 2 lines (pages)
	for (i = 0; i < 8; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT8X14_getUpperColumn(ch, i));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT8X14_getLowerColumn(ch, i));
	}


#ifdef	EPD_PAGE_VERTICAL
	// first page, upper
	transpose_page(0, 1, buf);
	// first page, lower
	transpose_page(0, 0, buf);

	// second page
	if ((CurrentPosX % 8) >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
		transpose_page(1, 0, buf);
	}


#else
#endif

	postwrap(8, 2);
}


/**
 *  @brief
 *      Put a glyph in 12x16 font to the display
 *  @param[in]
 *  	ch code page 850
 *  @return
 *      None
 */
static void putGlyph12x16(int ch) {
//	uint8_t buf[13];
	uint8_t i;

	if (autowrap(ch, 12, 2)) {
		return ;
	}

	// fill the buffer with 12 columns on 2 lines (pages)
	for (i = 0; i < 12; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT12X16_getUpperColumn(ch, i));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT12X16_getLowerColumn(ch, i));
	}

#ifdef	EPD_PAGE_VERTICAL
	// first page, upper
	transpose_page(0, 1, buf);
	// first page, lower
	transpose_page(0, 0, buf);

	// second page, upper
	transpose_page(1, 1, buf);
	// first page, lower
	transpose_page(1, 0, buf);

	// third page
	if ((CurrentPosX % 8) >= 8)  {
		// third page needed
		transpose_page(2, 1, buf);
		transpose_page(2, 0, buf);
	}


#else
#endif
	postwrap(12, 2);
}


/**
 *  @brief
 *      Put a glyph in 8x16 font DejaVu Sans Mono Bold to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph8x16B(int ch) {
	uint8_t i;

	if (autowrap(ch, 8, 2)) {
		return ;
	}

	// fill the buffer with 8 columns on 2 lines (pages)
	for (i = 0; i < 8; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT8X16B_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT8X16B_getScanColumn(ch, i, 1));
	}
	postwrap(8, 2);
}


/**
 *  @brief
 *      Put a glyph in 8x16 font DejaVu Sans Mono Standard to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph8x16S(int ch) {
	uint8_t i;

	if (autowrap(ch, 8, 2)) {
		return ;
	}

	// fill the buffer with 8 columns on 2 lines (pages)
	for (i = 0; i < 8; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT8X16S_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT8X16S_getScanColumn(ch, i, 1));
	}
	postwrap(8, 2);
}


/**
 *  @brief
 *      Put a glyph in 11x24 font DejaVu Sans Mono Bold to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph11x24B(int ch) {
	uint8_t i;

	if (autowrap(ch, 11, 3)) {
		return ;
	}

	// fill the buffer with 11 columns on 3 lines (pages)
	for (i = 0; i < 11; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT11X24B_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT11X24B_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT11X24B_getScanColumn(ch, i, 2));
	}
	postwrap(11, 3);
}


/**
 *  @brief
 *      Put a glyph in 10x24 font DejaVu Sans Mono Standard to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph10x24S(int ch) {
	uint8_t i;

	if (autowrap(ch, 10, 3)) {
		return ;
	}

	// fill the buffer with 12 columns on 3 lines (pages)
	for (i = 0; i < 10; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT10X24S_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT10X24S_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT10X24S_getScanColumn(ch, i, 2));
	}
	postwrap(10, 3);
}


/**
 *  @brief
 *      Put a glyph in 10x24 font DejaVu Sans Mono Bold to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph10x24B(int ch) {
	uint8_t i;

	if (autowrap(ch, 10, 3)) {
		return ;
	}

	// fill the buffer with 12 columns on 3 lines (pages)
	for (i = 0; i < 10; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT10X24B_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT10X24B_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT10X24B_getScanColumn(ch, i, 2));
	}
	postwrap(10, 3);
}


/**
 *  @brief
 *      Put a glyph in 11x24 font DejaVu Sans Mono Standard to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph11x24S(int ch) {
	uint8_t i;

	if (autowrap(ch, 11, 3)) {
		return ;
	}

	// fill the buffer with 11 columns on 3 lines (pages)
	for (i = 0; i < 11; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT11X24S_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT11X24S_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT11X24S_getScanColumn(ch, i, 2));
	}
	postwrap(11, 3);
}


/**
 *  @brief
 *      Put a glyph in 16x32 font DejaVu Sans Mono Bold to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph16x32B(int ch) {
	uint8_t i;

	if (autowrap(ch, 16, 4)) {
		return ;
	}

	// fill the buffer with 16 columns on 4 lines (pages)
	for (i = 0; i < 16; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT16X32B_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT16X32B_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT16X32B_getScanColumn(ch, i, 2));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+3] = ~bitswap(FONT16X32B_getScanColumn(ch, i, 3));
	}
	postwrap(16, 4);
}


/**
 *  @brief
 *      Put a glyph in 16x32 font DejaVu Sans Mono Bold to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph16x32S(int ch) {
	uint8_t i;

	if (autowrap(ch, 16, 4)) {
		return ;
	}

	// fill the buffer with 16 columns on 4 lines (pages)
	for (i = 0; i < 16; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT16X32S_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT16X32S_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT16X32S_getScanColumn(ch, i, 2));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+3] = ~bitswap(FONT16X32S_getScanColumn(ch, i, 3));
	}
	postwrap(16, 4);
}


/**
 *  @brief
 *      Put a glyph in 9x16 font DejaVu Sans Mono Bold to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph9x16B(int ch) {
	uint8_t i;

	if (autowrap(ch, 9, 2)) {
		return ;
	}

	// fill the buffer with 8 columns on 2 lines (pages)
	for (i = 0; i < 9; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT9X16B_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT9X16B_getScanColumn(ch, i, 1));
	}
	postwrap(9, 2);
}


/**
 *  @brief
 *      Put a glyph in 9x16 font DejaVu Sans Mono Standard to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph9x16S(int ch) {
	uint8_t i;

	if (autowrap(ch, 9, 2)) {
		return ;
	}

	// fill the buffer with 8 columns on 2 lines (pages)
	for (i = 0; i < 9; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT9X16S_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT9X16S_getScanColumn(ch, i, 1));
	}
	postwrap(9, 2);
}


/**
 *  @brief
 *      Put a glyph in 13x24 font DejaVu Sans Mono Bold to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph13x24B(int ch) {
	uint8_t i;

	if (autowrap(ch, 13, 3)) {
		return ;
	}

	// fill the buffer with 13 columns on 3 lines (pages)
	for (i = 0; i < 13; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT13X24B_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT13X24B_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT13X24B_getScanColumn(ch, i, 2));
	}
	postwrap(13, 3);
}


/**
 *  @brief
 *      Put a glyph in 13x24 font DejaVu Sans Mono Standard to the display
 *  @param[in]
 *  	ch ISO/IEC 8859 Latin-1
 *  @return
 *      None
 */
static void putGlyph13x24S(int ch) {
	uint8_t i;

	if (autowrap(ch, 13, 3)) {
		return ;
	}

	// fill the buffer with 13 columns on 3 lines (pages)
	for (i = 0; i < 13; i++) {
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY]   = ~bitswap(FONT13X24S_getScanColumn(ch, i, 0));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+1] = ~bitswap(FONT13X24S_getScanColumn(ch, i, 1));
		display_buffer->rows[(EPD_COLUMNS-1)-(CurrentPosX+i)][CurrentPosY+2] = ~bitswap(FONT13X24S_getScanColumn(ch, i, 2));
	}
	postwrap(13, 3);
}


/**
 *  @brief
 *      Put XBM image to the EPD display
 *  @param[in]
 *  	image		image array (magick image.png -rotate 90 -flop image.xbm)
 *  @param[in]
 *  	width in pixel
 *  @return
 *  	height in pixel
 */
void EPD_putXBM(char* image, int width, int height) {
	int line;
	int column;
	int i=0;

	uint8_t x = CurrentPosX;
	uint8_t y = CurrentPosY;

	for (column=0; column<width; column++) {
		for (line=0; line<(height/8); line++) {
			EPD_setPos(column+x, line+y);
			EPD_writeColumn(image[i]);
			i++;
		}
	}

}


/**
 *  @brief
 *      Test for special chars and for free space
 *  @param[in]
 *  	ch character
 *  @param[in]
 *  	width character width
 *  @param[in]
 *  	row per character
 *  @return
 *  	1 finished, 0 ready for glyph
 */
static int autowrap(int ch, int width, int row) {
	if (ch == '\n') {
		// line feed
		CurrentPosY += row;
		if (CurrentPosY >= EPD_LINES) {
			CurrentPosY = 0;
		}
		EPD_setPos(CurrentPosX, CurrentPosY);
		return 1;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= width) {
			EPD_setPos(CurrentPosX-width, CurrentPosY);
		}
		return 1;
	}

	if (CurrentPosX > EPD_X_RESOLUTION - width) {
		// auto wrap line
		CurrentPosY += row;
		if (CurrentPosY >= EPD_LINES) {
			// auto wrap display
			CurrentPosY = 0;
		}
		EPD_setPos(0, CurrentPosY);
	}
	return 0;
}

#ifdef EPD_PAGE_VERTICAL
/**
 *  @brief
 *      Transpose a glyph for one page (stripe) from buffer to display
 *  @param[in]
 *  	page 0, 1 or 2
 *  @param[in]
 *  	upper 1, 0 lower
 *  @param[in]
 *  	buffer for I2C
 *  @return
 *      None
 */
static void transpose_page(int page, int upper, uint8_t *buf) {
	int x, y;
	int column;
	int fragment;
	int row = CurrentPosY;
	int col;
	// check for fragmentation
	fragment = CurrentPosX % 8;

	if (!upper) {
		row++;
	}

	col = CurrentPosX-fragment+page*8;

	memset(buf+1, 0, 8); 	// clear the I2C array
	// transpose glyph and copy into I2C array
	for (y=0; y<8; y++) {
		column = display_buffer->rows[row][col+y];
		if (column) {
			// only needed if a bit is set
			for (x=0; x<8; x++) {
				buf[x+1] |= ((column & 0x01) << y);
				column = column >> 1;
			}
		}
	}
	// set pos to the beginning of the first page
//	setPos(col, row);
}
#endif

/**
 *  @brief
 *      Test for wrap
 *  @param[in]
 *  	width character width
 *  @param[in]
 *  	row per character
 *  @return
 *  	none
 */
static void postwrap(int width, int row) {
	CurrentPosX += width;

	if (CurrentPosX >= EPD_X_RESOLUTION) {
		// auto wrap
		CurrentPosY += row;
		if (CurrentPosY >= EPD_LINES) {
			CurrentPosY = 0;
		}
		EPD_setPos(0, CurrentPosY);
#ifdef EPD_PAGE_VERTICAL
	} else {
		EPD_setPos(CurrentPosX, CurrentPosY);
#endif
	}
}

uint8_t bitswap(uint8_t byte) {
	uint8_t i, out=0;

	for(i=0;i<8;i++) {
		out >>= 1;
		out |= (byte & 0x80);
		byte <<= 1;
	}
	return out;
}



#endif
