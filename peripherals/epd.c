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
 *		  - Gates  0 .. 249, Y direction (column)
 *		  - Source 0 .. 121, X direction (page)
 *
 *  	The display RAM can not be read, therefore the display content is
 *  	mirrored in a frame buffer.
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
#include "stm32wbxx_ll_spi.h"
#include "font6x8.h"
#include "font8x8.h"
#include "font8x14.h"
#include "font12x16.h"

#if EPD == 1

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
static int autowrap(int ch, int width, int row);
#ifdef EPD_PAGE_VERTICAL
static void transpose_page(int page, int upper, uint8_t *buf);
#endif
static void postwrap(int width, int row);

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
   uint8_t blob[(EPD_LINES) * EPD_X_RESOLUTION];
   uint8_t rows[EPD_LINES][EPD_X_RESOLUTION];
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
	uint8_t buf[5];
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_SET);		// no chip select
	GPIO_InitStruct.Pin = EPD_ECS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(EPD_ECS_GPIO_Port, &GPIO_InitStruct);

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

	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_RESET);		// activate reset
	HAL_GPIO_WritePin(EPD_RST_GPIO_Port, EPD_RST_Pin, GPIO_PIN_SET);

	if (busy_wait()) {
		// timeout -> no display connected
		epdReady = FALSE;
		return;
	} else {
		epdReady = TRUE;
	}

	display_buffer = pvPortMalloc(sizeof(display_buffer_t));

	buf[0] = 1;
	buf[1] = SSD1680_SW_RESET;
	EPD_sendCommand(buf);
	busy_wait();

	// Set display size and driver output control
	// 250-1
	buf[0] = 4;
	buf[1] = SSD1680_DRIVER_CONTROL;
	buf[2] = 0xf9;
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

	// border color
	// A [1:0] GS Transition setting for VBD: 01 LUT1
	// A [2]   GS Transition control:          1 Follow LUT
	// A [5:4] Fix Level Setting for VBD:     00 VSS
	// A [7:6] Select VBD option:             00 GS Transition
	buf[0] = 2;
	buf[1] = SSD1680_WRITE_BORDER;
	buf[2] = 0x05;
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

	// Set RAM X address counter
	// 1
	buf[0] = 2;
	buf[1] = SSD1680_SET_RAMXCOUNT;
	buf[2] = 1;
	EPD_sendCommand(buf);

	// Set RAM Y address counter
	// 0
	buf[0] = 3;
	buf[1] = SSD1680_SET_RAMYCOUNT;
	buf[2] = 0;
	buf[2] = 0;
	EPD_sendCommand(buf);

	EPD_clear();
	EPD_setPos(0,0);
	EPD_setFont(EPD_FONT8X8);
	EPD_puts("Mecrisp-Cube");
	EPD_setFont(EPD_FONT6X8);
	EPD_puts(MECRISP_CUBE_TAG);
#ifdef DEBUG
	EPD_puts("\r\nDebug\r\n");
#else
	EPD_puts("\r\n\r\n");
#endif
	EPD_puts("Forth for the STM32WB\r\n");
	EPD_puts("(c)2022 peter@spyr.ch");
}


/**
 *  @brief
 *      Sends a command to the EPD controller
 *
 *      Max. length of a command is 4 bytes.
 *  @param[in]
 *  	First byte contains the length of the command.
 *  @return
 *      None
 */
void EPD_sendCommand(const uint8_t *command) {
	uint8_t buf[5];

	if (!epdReady) {
		return;
	}

	// only one thread is allowed to use the SPI
	osMutexAcquire(RTSPI_MutexID, osWaitForever);

	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_1EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_LOW);
	//	LL_SPI_SetTransferBitOrder(LL_SPI_LSB_FIRST);

	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_RESET); 	// chip select

	HAL_GPIO_WritePin(EPD_DC_GPIO_Port, EPD_DC_Pin, GPIO_PIN_RESET);		// command
	memcpy(buf, &command[1], command[0]);
	RTSPI_WriteData(buf, command[0]);

	// chip deselect
	HAL_GPIO_WritePin(EPD_ECS_GPIO_Port, EPD_ECS_Pin, GPIO_PIN_SET);

	LL_SPI_SetClockPhase(SPI1, LL_SPI_PHASE_2EDGE);
	LL_SPI_SetClockPolarity(SPI1, LL_SPI_POLARITY_HIGH);
	//	LL_SPI_SetTransferBitOrder(LL_SPI_LSB_FIRST);

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
 *      x  horizontal position, max. (128 / 6) -1, depends on font.
 *  @param[in]
 *      y  vertical position (line), max. 3 for 128x32 or 7 for 128x64 displays.
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
	uint8_t i;

	if (!epdReady) {
		return;
	}

	memset(display_buffer->blob, 0, sizeof(display_buffer->blob));
	display_buffer->blob[0] =  0x40;  // write data

#ifdef EPD_PAGE_VERTICAL
	for (i=0; i<(128/8); i++) {
		EPD_setPos(i*8, 0);
		IIC_setDevice(EPD_I2C_ADR);
		IIC_putMessage(display_buffer->blob, 65);
	}
#else
	for (i=0; i<EPD_LINES; i++) {
		EPD_setPos(0, i);
//		IIC_setDevice(EPD_I2C_ADR);
//		IIC_putMessage(display_buffer->blob, 129);
	}
#endif
	EPD_setPos(0, 0);
	display_buffer->blob[0] =  0;
}


/**
 *  @brief
 *  	Update the EPD display
 *  @return
 *      none
 */
void EPD_update(void) {
	int i;
	int j;
	int oldx = CurrentPosX;
	int oldy = CurrentPosY;

	uint8_t buf[9];

	if (!epdReady) {
		return;
	}

	buf[0] = 0x40;
	EPD_setPos(0, 0);

#ifdef EPD_PAGE_VERTICAL
	for (i=0; i<EPD_LINES; i++) {
		for (j=0; j<(EPD_X_RESOLUTION/8); j++) {
			transpose_page(0, 1, buf);
			EPD_setPos(j*8, i);
		}
	}

#else
	for (i=0; i<EPD_LINES; i++) {
		for (j=0; j<(EPD_X_RESOLUTION/8); j++) {
			memcpy(buf+1, &display_buffer->rows[i][j*8], 8);

			EPD_setPos(j*8, i);
//			IIC_setDevice(EPD_I2C_ADR);
//			IIC_putMessage(buf, 9);
		}
	}
#endif
	EPD_setPos(oldx, oldy);

}


// Private Functions
// *****************

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
#else
//	uint8_t buf[7];
#endif
	uint8_t i;

	if (autowrap(ch, 6, 1)) {
		return ;
	}

	// fill the buffer with 6 columns
	for (i = 0; i < 6; i++) {
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT6X8_getColumn(ch, i);
	}


#ifdef	EPD_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 6) >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#else
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
//	uint8_t buf[9];
	uint8_t i;

	if (autowrap(ch, 8, 1)) {
		return ;
	}

	// fill the buffer with 8 columns
	for (i = 0; i < 8; i++) {
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT8X8_getColumn(ch, i);
	}

#ifdef	EPD_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 8) >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#else
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
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT8X14_getUpperColumn(ch, i);
		display_buffer->rows[CurrentPosY+1][CurrentPosX+i] = FONT8X14_getLowerColumn(ch, i);
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
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT12X16_getUpperColumn(ch, i);
		display_buffer->rows[CurrentPosY+1][CurrentPosX+i] = FONT12X16_getLowerColumn(ch, i);
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


#endif