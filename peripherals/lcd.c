/**
 *  @brief
 *  	LCD driver based on the controller Sitronix ST7567S.
 *
 *  	Resolution 128x64, monochrome.
 *  	A page consists of 128 columns (horizontally, x) with 8 pixels (vertically, y).
 *  	The 8 pixels are in one byte.
 *  	There 8 pages in a 128x64 display.
 *  	A page is 8 pixel wide stripe on the display.
 *  	4 wire SPI interface.
 *  	The display RAM can not be read over the SPI, therefore the display content is
 *  	mirrored in a buffer.
 *  	See https://www.mikrocontroller.net/topic/54860 for the fonts.
 *  @file
 *      lcd.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2023-11-25
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
#include "lcd.h"
#include "d_spi.h"

#include "font6x8.h"
#include "font8x8.h"
#include "font8x14.h"
#include "font12x16.h"

#if LCDISPLAY == 1
#include "spyr.h"

// Macros
// ******
#define  membersof(x) (sizeof(x) / sizeof(x[0]))

#define ST756X_CMD_ON_OFF 			0b10101110 /**< 0:0 Switch Display ON/OFF: last bit */
#define ST756X_CMD_SET_LINE 		0b01000000 /**< 0:0 Set Start Line: last 6 bits  */
#define ST756X_CMD_SET_PAGE 		0b10110000 /**< 0:0 Set Page address: last 4 bits */
#define ST756X_CMD_SET_COLUMN_MSB 	0b00010000 /**< 0:0 Set Column MSB: last 4 bits */
#define ST756X_CMD_SET_COLUMN_LSB 	0b00000000 /**< 0:0 Set Column LSB: last 4 bits */
#define ST756X_CMD_SEG_DIRECTION 	0b10100000 /**< 0:0 Reverse scan direction of SEG: last bit */
#define ST756X_CMD_INVERSE_DISPLAY 	0b10100110 /**< 0:0 Invert display: last bit */
#define ST756X_CMD_ALL_PIXEL_ON 	0b10100100 /**< 0:0 Set all pixel on: last bit */
#define ST756X_CMD_BIAS_SELECT 		0b10100010 /**< 0:0 Select 1/9(0) or 1/7(1) bias: last bit */
#define ST756X_CMD_R_M_W 			0b11100000 /**< 0:0 Enter Read Modify Write mode: read+0, write+1 */
#define ST756X_CMD_END 				0b11101110 /**< 0:0 Exit Read Modify Write mode */
#define ST756X_CMD_RESET 			0b11100010 /**< 0:0 Software Reset */
#define ST756X_CMD_COM_DIRECTION 	0b11000000 /**< 0:0 Com direction reverse: +0b1000 */
#define ST756X_CMD_POWER_CONTROL 	0b00101000 /**< 0:0 Power control: last 3 bits VB:VR:VF */
#define ST756X_CMD_REGULATION_RATIO 0b00100000 /**< 0:0 Regulation resistor ration: last 3bits */
#define ST756X_CMD_SET_EV 			0b10000001 /**< 0:0 Set electronic volume: 5 bits in next byte */
#define ST756X_CMD_SET_BOOSTER 		0b11111000 /**< 0:0 Set Booster level, 4X(0) or 5X(1): last bit in next byte */
#define ST756X_CMD_NOP 				0b11100011 /**< 0:0 No operation */

#define ST756X_BIAS					0b00000000 	/**< 1 1/7, 0 1/9 */
#define ST756X_RATIO				0b110 		/**<  */
#define ST756X_CONTRAST				28

// Private function prototypes
// ***************************
static void setPos(uint8_t x, uint8_t y);
static void putGlyph6x8(int ch);
static void putGlyph8x8(int ch);
static void putGlyph8x16(int ch);
static void putGlyph12x16(int ch);
static int autowrap(int ch, int width, int row);
void postwrap(int width, int row);

// Global Variables
// ****************

// Hardware resources
// ******************

// RTOS resources
// **************

// Private Variables
// *****************

static uint8_t lcdReady = FALSE;

static uint8_t CurrentPosX = 0;
static uint8_t CurrentPosY = 0;

static LCD_FontT CurrentFont = LCD_FONT6X8;

typedef union {
   uint8_t blob[(LCD_LINES) * LCD_X_RESOLUTION];
   uint8_t rows[LCD_LINES][LCD_X_RESOLUTION];	// rows are pages except for feather 128x64 display
} display_buffer_t;

static display_buffer_t *display_buffer;

static const uint8_t display_reset[] =	{ 1, ST756X_CMD_RESET };
static const uint8_t set_bias[] =	    { 1, ST756X_CMD_BIAS_SELECT | ST756X_BIAS };
static const uint8_t set_seg[] =	    { 1, ST756X_CMD_SEG_DIRECTION };
static const uint8_t set_com[] =	    { 1, ST756X_CMD_COM_DIRECTION | 0b1000 };
static const uint8_t set_dir[] =	    { 1, ST756X_CMD_SET_LINE };
static const uint8_t set_ratio[] =	    { 1, ST756X_CMD_REGULATION_RATIO | ST756X_RATIO };
static const uint8_t set_contrast[] =	{ 2, ST756X_CMD_SET_EV, ST756X_CONTRAST} ;
static const uint8_t enable_power[] =	{ 1, ST756X_CMD_POWER_CONTROL | 0b111 };

static const uint8_t display_off[] =	{ 1, ST756X_CMD_ON_OFF };			// Display OFF (sleep mode)
static const uint8_t display_on[] =		{ 1, ST756X_CMD_ON_OFF | 0x01 };	// Display ON in normal mode

static const uint8_t pixel_off[] =		{ 1, ST756X_CMD_ALL_PIXEL_ON | 0b0};
static const uint8_t pixel_on[] =		{ 1, ST756X_CMD_ALL_PIXEL_ON | 0b1};

static const uint8_t *st7567_init_sequence[] = {	// Initialization Sequence
		display_reset,
		set_bias,
		set_seg,
		set_com,
		set_dir,
		set_ratio,
		set_contrast,
		enable_power
};

static const uint8_t *st7567_on_sequence[] = {	// display on
		pixel_off,
		display_on
};

static const uint8_t *st7567_off_sequence[] = {	// display on
		display_off,
		pixel_on
};


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the LCD controller.
 *  @return
 *      None
 */
void LCD_init(void) {
	uint8_t i;

	HAL_GPIO_WritePin(DISPLAY_RST_GPIO_Port, DISPLAY_RST_Pin, GPIO_PIN_RESET);	// reset LCD
	osDelay(20);
	HAL_GPIO_WritePin(DISPLAY_RST_GPIO_Port, DISPLAY_RST_Pin, GPIO_PIN_SET);

	lcdReady = TRUE;

	display_buffer = pvPortMalloc(sizeof(display_buffer_t));

	for (i = 0; i < membersof(st7567_init_sequence); i++) {
		LCD_sendCommand(st7567_init_sequence[i]);
	}

	LCD_clear();
	LCD_setPos(127-spyr_height-8, 0);
	LCD_putXBM(spyr_bits, spyr_width, spyr_height);

	LCD_setPos(0, 0);
	LCD_setFont(LCD_FONT8X8);
	LCD_puts("Mecrisp-");
	LCD_setPos(93, 0);
	LCD_puts("Cube\r\n");
	LCD_puts("v" MECRISP_CUBE_TAG "\r\n\r\n");

	LCD_setFont(LCD_FONT6X8);
	LCD_puts(BOARD "\r\n");
	LCD_puts("Forth for\r\n");
	LCD_puts("the STM32WB \r\n");
	LCD_puts("(c)2023\r\n");
	LCD_puts("peter@spyr.ch");

	for (i = 0; i < membersof(st7567_on_sequence); i++) {
		LCD_sendCommand(st7567_on_sequence[i]);
	}

}


/**
 *  @brief
 *      Writes a char to the LCD. Blocking until char is written into the controller memory.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int LCD_putc(int c) {
	if (!lcdReady) {
		return EOF;
	}

	if (c == '\r') {
		// carriage return
		LCD_setPos(0, CurrentPosY);
		return 0;
	}

	switch (CurrentFont) {
	case LCD_FONT6X8:
		putGlyph6x8(c);
		break;
	case LCD_FONT8X8:
		putGlyph8x8(c);
		break;
	case LCD_FONT8X16:
		putGlyph8x16(c);
		break;
	case LCD_FONT12X16:
		putGlyph12x16(c);
		break;
	}

	return 0;
}


/**
 *  @brief
 *      LCD ready for next char.
 *  @return
 *      FALSE if the transfer is ongoing.
 */
int LCD_Ready(void) {
	return TRUE;
}


/**
 *  @brief
 *      Writes a line (string) to the LCD. Blocking until
 *      string can be written.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      s  string to write. C style string.
 *  @return
 *      Return EOF on error, 0 on success.
 */
int LCD_puts(const char *s) {
	int i=0;

	while (s[i] != 0) {
		if (LCD_putc(s[i])) {
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
void LCD_setPos(uint8_t x, uint8_t y) {
	if (!lcdReady) {
		return;
	}

	if ((x >= 0 && x < LCD_X_RESOLUTION) && (y >=0 && y < LCD_Y_RESOLUTION/8)) {
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
uint8_t LCD_getPosX() {
	return CurrentPosX;
}


/**
 *  @brief
 *  	Gets the current vertical position
 *  @return
 *      Y vertical position, max. 3 for 128x32 or 7 for 128x64 displays.
 */
uint8_t LCD_getPosY() {
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
void LCD_setFont(LCD_FontT font) {
	CurrentFont = font;
}


/**
 *  @brief
 *  	Clears the LCD display
 *  @return
 *      none
 */
void LCD_clear(void) {
	uint8_t i;

	if (!lcdReady) {
		return;
	}

	memset(display_buffer->blob, 0, sizeof(display_buffer->blob));
	display_buffer->blob[0] =  0x40;  // write data

	for (i=0; i<LCD_LINES; i++) {
		LCD_setPos(0, i);
		osMutexAcquire(DSPI_MutexID, osWaitForever);
		HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// Data
		HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
		DSPI_WriteData(display_buffer->blob +1, 128);
		HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
		osMutexRelease(DSPI_MutexID);
	}
	LCD_setPos(0, 0);
	display_buffer->blob[0] =  0;
}


/**
 *  @brief
 *  	Update the LCD display
 *  @return
 *      none
 */
void LCD_update(void) {
	int i;
	int j;
	int oldx = CurrentPosX;
	int oldy = CurrentPosY;

	uint8_t buf[9];

	if (!lcdReady) {
		return;
	}

	buf[0] = 0x40;
	LCD_setPos(0, 0);

	for (i=0; i<LCD_LINES; i++) {
		for (j=0; j<(LCD_X_RESOLUTION/8); j++) {
			memcpy(buf+1, &display_buffer->rows[i][j*8], 8);

			LCD_setPos(j*8, i);
			osMutexAcquire(DSPI_MutexID, osWaitForever);
			HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// Data
			HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
			DSPI_WriteData(buf+1, 8);
			HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
			osMutexRelease(DSPI_MutexID);
		}
	}

	LCD_setPos(oldx, oldy);

}

/**
 *  @brief
 *      Sends a command to the LCD controller
 *
 *      Max. length of a command is 4 bytes.
 *  @param[in]
 *  	First byte contains the length of the command.
 *  @return
 *      None
 */
void LCD_sendCommand(const uint8_t *command) {
	uint8_t buf[5];

	if (!lcdReady) {
		return;
	}

	buf[0] = 0x00; // write command
	memcpy(&buf[1], &command[1], command[0]);
	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_RESET);	// command
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, command[0]);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);
}


/**
 *  @brief
 *      Read the status and ID byte from the LCD controller
 *  @return
 *      None Bit7: BUSY; Bit6: ON 0, OFF 1; Bit5..7: ID 000111
 */
int LCD_readStatus(void) {
	uint8_t status=0;

	return status;
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
void LCD_writeColumn(uint8_t column) {
	uint8_t buf[2];

	if (autowrap(' ', 1, 1)) {
		return ;
	}

	display_buffer->rows[CurrentPosY][CurrentPosX] = column;
	buf[0] = 0x40;  // write data

	// copy into I2C array
	buf[1] = display_buffer->rows[CurrentPosY][CurrentPosX];

	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 1);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);

	postwrap(1, 1);
}


/**
 *  @brief
 *      Read a column (8 pixels) from the current position
 *  @return
 *      Column
 */
int LCD_readColumn(void) {
	return display_buffer->rows[CurrentPosY][CurrentPosX];
}


/**
 *  @brief
 *      Put XBM image to the LCD display
 *  @param[in]
 *  	image		image array (magick image.png -rotate 90 -flop image.xbm)
 *  @param[in]
 *  	width in pixel
 *  @return
 *  	height in pixel
 */
void LCD_putXBM(char* image, int width, int height) {
	int line;
	int column;
	int i=0;

	uint8_t x = CurrentPosX;
	uint8_t y = CurrentPosY;

	for (column=0; column<height; column++) {
		for (line=0; line<(width/8); line++) {
			LCD_setPos(column+x, line+y);
			LCD_writeColumn(image[i]);
			i++;
		}
	}

}


// Private Functions
// *****************

/**
 *  @brief
 *      Set the display position (page and column address)
 *  @param[in]
 *  	x 0 .. 127, column
 *  @param[in]
 *  	y 0 .. LCD_LINES, page/line
 *  @return
 *      None
 */
static void setPos(uint8_t x, uint8_t y) {
	uint8_t buf[4];

	buf[0] = 0x00; // write command
	buf[1] = 0xb0 + y; // page address
	buf[2] = ((x & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	buf[3] = x & 0x0f; // | 0x01 // Set Lower Column Start Address

	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_RESET);	// command
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 3);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);
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
	uint8_t buf[7];
	uint8_t i;

	if (autowrap(ch, 6, 1)) {
		return ;
	}

	// fill the buffer with 6 columns
	for (i = 0; i < 6; i++) {
		display_buffer->rows[CurrentPosY][CurrentPosX+i] = FONT6X8_getColumn(ch, i);
	}

	buf[0] = 0x40;  // write data

	// copy into I2C array
	for (i = 0; i < 6; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}

	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 6);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);

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

	// copy into I2C array
	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 8);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);

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

	// copy into I2C array
	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 8);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);

	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosX+i][CurrentPosY+1];
	}
	setPos(CurrentPosX, CurrentPosY+1);
	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 8);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);

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

	// copy into I2C array
	for (i = 0; i < 12; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 12);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);

	for (i = 0; i < 12; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosX+i][CurrentPosY+1];
	}
	setPos(CurrentPosX, CurrentPosY+1);
	osMutexAcquire(DSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(DISPLAY_DI_GPIO_Port, DISPLAY_DI_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
	DSPI_WriteData(buf+1, 12);
	HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(DSPI_MutexID);

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
		if (CurrentPosY >= LCD_LINES) {
			CurrentPosY = 0;
		}
		LCD_setPos(CurrentPosX, CurrentPosY);
		return 1;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= width) {
			LCD_setPos(CurrentPosX-width, CurrentPosY);
		}
		return 1;
	}

	if (CurrentPosX > LCD_X_RESOLUTION - width) {
		// auto wrap line
		CurrentPosY += row;
		if (CurrentPosY >= LCD_LINES) {
			// auto wrap display
			CurrentPosY = 0;
		}
		LCD_setPos(0, CurrentPosY);
	}
	return 0;
}


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

	if (CurrentPosX >= LCD_X_RESOLUTION) {
		// auto wrap
		CurrentPosY += row;
		if (CurrentPosY >= LCD_LINES) {
			CurrentPosY = 0;
		}
		LCD_setPos(0, CurrentPosY);
	}
}


#endif
