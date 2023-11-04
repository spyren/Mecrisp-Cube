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
 *      mip.c
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

// System include files
// ********************
#include "cmsis_os.h"
#include <stdio.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "mip.h"
#include "iic.h"
#include "font6x8.h"
#include "font8x8.h"
#include "font8x14.h"
#include "font12x16.h"

#if MIP == 1

// Macros
// ******
#define  membersof(x) (sizeof(x) / sizeof(x[0]))

// Private function prototypes
// ***************************
static void setPos(uint8_t x, uint8_t y);
static void putGlyph6x8(int ch);
static void putGlyph8x8(int ch);
static void putGlyph8x16(int ch);
static void putGlyph12x16(int ch);
static int autowrap(int ch, int width, int row);
#ifdef MIP_PAGE_VERTICAL
static void transpose_page(int page, int upper, uint8_t *buf);
#endif
void postwrap(int width, int row);

// Global Variables
// ****************

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;

// RTOS resources
// **************

// Private Variables
// *****************

static uint8_t mipReady = FALSE;

static uint8_t CurrentPosX = 0;
static uint8_t CurrentPosY = 0;

static MIP_FontT CurrentFont = MIP_FONT6X8;

typedef union {
   uint8_t blob[(MIP_LINES) * MIP_X_RESOLUTION];
   uint8_t rows[MIP_LINES][MIP_X_RESOLUTION];	// rows are pages except for feather 128x64 display
} display_buffer_t;

static display_buffer_t *display_buffer;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the MIP controller.
 *  @return
 *      None
 */
void MIP_init(void) {


	display_buffer = pvPortMalloc(sizeof(display_buffer_t));

	MIP_clear();
	MIP_setPos(0,0);
	MIP_setFont(MIP_FONT8X8);
	MIP_puts("Mecrisp-Cube");
	MIP_setFont(MIP_FONT6X8);
	MIP_puts(MECRISP_CUBE_TAG);
#ifdef DEBUG
	MIP_puts("\r\nDebug\r\n");
#else
	MIP_puts("\r\n\r\n");
#endif
	MIP_puts("Forth for the STM32WB\r\n");
	MIP_puts("(c)2022 peter@spyr.ch");
}


/**
 *  @brief
 *      Writes a char to the MIP. Blocking until char is written into the controller memory.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int MIP_putc(int c) {
	if (!mipReady) {
		return EOF;
	}

	if (c == '\r') {
		// carriage return
		MIP_setPos(0, CurrentPosY);
		return 0;
	}

	switch (CurrentFont) {
	case MIP_FONT6X8:
		putGlyph6x8(c);
		break;
	case MIP_FONT8X8:
		putGlyph8x8(c);
		break;
	case MIP_FONT8X16:
		putGlyph8x16(c);
		break;
	case MIP_FONT12X16:
		putGlyph12x16(c);
		break;
	}

	return 0;
}


/**
 *  @brief
 *      MIP ready for next char.
 *  @return
 *      FALSE if the transfer is ongoing.
 */
int MIP_Ready(void) {
	return TRUE;
}


/**
 *  @brief
 *      Writes a line (string) to the MIP. Blocking until
 *      string can be written.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      s  string to write. C style string.
 *  @return
 *      Return EOF on error, 0 on success.
 */
int MIP_puts(const char *s) {
	int i=0;

	while (s[i] != 0) {
		if (MIP_putc(s[i])) {
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
void MIP_setPos(uint8_t x, uint8_t y) {
	if (!mipReady) {
		return;
	}

	if ((x >= 0 && x < MIP_X_RESOLUTION) && (y >=0 && y < MIP_Y_RESOLUTION/8)) {
		// valid position
		setPos(x, y);
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
uint8_t MIP_getPosX() {
	return CurrentPosX;
}


/**
 *  @brief
 *  	Gets the current vertical position
 *  @return
 *      Y vertical position, max. 3 for 128x32 or 7 for 128x64 displays.
 */
uint8_t MIP_getPosY() {
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
void MIP_setFont(MIP_FontT font) {
	CurrentFont = font;
}


/**
 *  @brief
 *  	Clears the MIP display
 *  @return
 *      none
 */
void MIP_clear(void) {
	uint8_t i;

	if (!mipReady) {
		return;
	}

	memset(display_buffer->blob, 0, sizeof(display_buffer->blob));
	display_buffer->blob[0] =  0x40;  // write data

#ifdef MIP_PAGE_VERTICAL
	for (i=0; i<(128/8); i++) {
		MIP_setPos(i*8, 0);
		IIC_putMessage(display_buffer->blob, 65, MIP_I2C_ADR);
	}
#else
	for (i=0; i<MIP_LINES; i++) {
		MIP_setPos(0, i);
//		IIC_setDevice(MIP_I2C_ADR);
//		IIC_putMessage(display_buffer->blob, 129);
	}
#endif
	MIP_setPos(0, 0);
	display_buffer->blob[0] =  0;
}


/**
 *  @brief
 *  	Update the MIP display
 *  @return
 *      none
 */
void MIP_update(void) {
	int i;
	int j;
	int oldx = CurrentPosX;
	int oldy = CurrentPosY;

	uint8_t buf[9];

	if (!mipReady) {
		return;
	}

	buf[0] = 0x40;
	MIP_setPos(0, 0);

#ifdef MIP_PAGE_VERTICAL
	for (i=0; i<MIP_LINES; i++) {
		for (j=0; j<(MIP_X_RESOLUTION/8); j++) {
			transpose_page(0, 1, buf);
			MIP_setPos(j*8, i);
		}
	}

#else
	for (i=0; i<MIP_LINES; i++) {
		for (j=0; j<(MIP_X_RESOLUTION/8); j++) {
			memcpy(buf+1, &display_buffer->rows[i][j*8], 8);

			MIP_setPos(j*8, i);
//			IIC_setDevice(MIP_I2C_ADR);
//			IIC_putMessage(buf, 9);
		}
	}
#endif
	MIP_setPos(oldx, oldy);

}

/**
 *  @brief
 *      Sends a command to the MIP controller
 *
 *      Max. length of a command is 4 bytes.
 *  @param[in]
 *  	First byte contains the length of the command.
 *  @return
 *      None
 */
void MIP_sendCommand(const uint8_t *command) {
	uint8_t buf[5];

	if (!mipReady) {
		return;
	}

	buf[0] = 0x00; // write command
	memcpy(&buf[1], &command[1], command[0]);
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, command[0]+1);
}


/**
 *  @brief
 *      Read the status and ID byte from the MIP controller
 *  @return
 *      None Bit7: BUSY; Bit6: ON 0, OFF 1; Bit5..7: ID 000111
 */
int MIP_readStatus(void) {
	uint8_t status;

//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_getMessage(&status, 1);
	return status;
}


// Private Functions
// *****************

/**
 *  @brief
 *      Set the display position (page and column address)
 *  @param[in]
 *  	x 0 .. 127, column
 *  @param[in]
 *  	y 0 .. MIP_LINES, page/line
 *  @return
 *      None
 */
static void setPos(uint8_t x, uint8_t y) {
	uint8_t buf[4];

	buf[0] = 0x00; // write command
#ifdef MIP_PAGE_VERTICAL
	buf[1] = 0xb0 + x/8; // page address
	buf[2] = (((y*8) & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	buf[3] = (y*8) & 0x0f; // | 0x01 // Set Lower Column Start Address
#else
	buf[1] = 0xb0 + y; // page address
	buf[2] = ((x & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	buf[3] = x & 0x0f; // | 0x01 // Set Lower Column Start Address
#endif
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, 4);
}


/**
 *  @brief
 *      Put a glyph in 6x8 font to the display
 *  @param[in]
 *  	ch code page 850
 *  @return
 *      None
 */
static void putGlyph6x8(int ch) {
#ifdef	MIP_PAGE_VERTICAL
	uint8_t buf[9];
#else
	uint8_t buf[7];
#endif
	uint8_t i;

	if (autowrap(ch, 6, 1)) {
		return ;
	}

	// fill the buffer with 6 columns
	for (i = 0; i < 6; i++) {
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT6X8_getColumn(ch, i);
	}

	buf[0] = 0x40;  // write data

#ifdef	MIP_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 6) >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#else
	// copy into I2C array
	for (i = 0; i < 6; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, 7);
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
	uint8_t buf[9];
	uint8_t i;

	if (autowrap(ch, 8, 1)) {
		return ;
	}

	// fill the buffer with 8 columns
	for (i = 0; i < 8; i++) {
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT8X8_getColumn(ch, i);
	}

	buf[0] = 0x40;  // write data

#ifdef	MIP_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 8) >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#else
	// copy into I2C array
	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, 9);
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
	uint8_t buf[9];
	uint8_t i;

	if (autowrap(ch, 8, 2)) {
		return ;
	}

	// fill the buffer with 8 columns on 2 lines (pages)
	for (i = 0; i < 8; i++) {
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT8X14_getUpperColumn(ch, i);
		display_buffer->rows[CurrentPosY+1][CurrentPosX+i] = FONT8X14_getLowerColumn(ch, i);
	}

	buf[0] = 0x40;  // write data

#ifdef	MIP_PAGE_VERTICAL
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
	// copy into I2C array
	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, 9);

	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosX+i][CurrentPosY+1];
	}
	setPos(CurrentPosX, CurrentPosY+1);
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, 9);

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
	uint8_t buf[13];
	uint8_t i;

	if (autowrap(ch, 12, 2)) {
		return ;
	}

	// fill the buffer with 12 columns on 2 lines (pages)
	for (i = 0; i < 12; i++) {
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT12X16_getUpperColumn(ch, i);
		display_buffer->rows[CurrentPosY+1][CurrentPosX+i] = FONT12X16_getLowerColumn(ch, i);
	}

	buf[0] = 0x40;  // write data

#ifdef	MIP_PAGE_VERTICAL
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
	// copy into I2C array
	for (i = 0; i < 12; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, 13);

	for (i = 0; i < 12; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosX+i][CurrentPosY+1];
	}
	setPos(CurrentPosX, CurrentPosY+1);
//	IIC_setDevice(MIP_I2C_ADR);
//	IIC_putMessage(buf, 13);

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
		if (CurrentPosY >= MIP_LINES) {
			CurrentPosY = 0;
		}
		MIP_setPos(CurrentPosX, CurrentPosY);
		return 1;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= width) {
			MIP_setPos(CurrentPosX-width, CurrentPosY);
		}
		return 1;
	}

	if (CurrentPosX > MIP_X_RESOLUTION - width) {
		// auto wrap line
		CurrentPosY += row;
		if (CurrentPosY >= MIP_LINES) {
			// auto wrap display
			CurrentPosY = 0;
		}
		MIP_setPos(0, CurrentPosY);
	}
	return 0;
}

#ifdef MIP_PAGE_VERTICAL
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
	setPos(col, row);
	IIC_putMessage(buf, 9, MIP_I2C_ADR);
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
void postwrap(int width, int row) {
	CurrentPosX += width;

	if (CurrentPosX >= MIP_X_RESOLUTION) {
		// auto wrap
		CurrentPosY += row;
		if (CurrentPosY >= MIP_LINES) {
			CurrentPosY = 0;
		}
		MIP_setPos(0, CurrentPosY);
#ifdef MIP_PAGE_VERTICAL
	} else {
		MIP_setPos(CurrentPosX, CurrentPosY);
#endif
	}
}


#endif
