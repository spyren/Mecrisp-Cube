/**
 *  @brief
 *  	OLED driver based on the controller SSD1306 (and SH1107, SSD1315).
 *
 *  	Resolution 128x32 or 128x64, monochrome.
 *  	A page consists of 128 columns (horizontally, x) with 8 pixels (vertically, y).
 *  	The 8 pixels are in one byte.
 *  	There are 4 pages in 128x32 display, and 8 pages in a 128x64 display.
 *  	A page is 8 pixel wide stripe on the display.
 *  	I2C Interface, address 60, or 4 wire SPI interface.
 *  	The display RAM can not be read over the I2C, therefore the display content is
 *  	mirrored in a buffer.
 *  	See https://www.mikrocontroller.net/topic/54860 for the fonts.
 *
 *  	The FeatherWing 128x64 OLED is different. x and y are interchanged,
 *  	that means pages can't be written in the same way :-(
 *  	but portrait is easy to implement.
 *  @file
 *      oled.c
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

// System include files
// ********************
#include "cmsis_os.h"
#include <stdio.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "oled.h"
#ifndef OLED_SPI
#include "iic.h"
#else
#include "rt_spi.h"
#endif

#include "font6x8.h"
#include "font8x8.h"
#include "font8x14.h"
#include "font12x16.h"

#if OLED == 1
#include "mecrisp-cube-logo.h"

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
#ifdef OLED_PAGE_VERTICAL
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

static uint8_t oledReady = FALSE;

static uint8_t CurrentPosX = 0;
static uint8_t CurrentPosY = 0;

static OLED_FontT CurrentFont = OLED_FONT6X8;

typedef union {
   uint8_t blob[(OLED_LINES) * OLED_X_RESOLUTION];
   uint8_t rows[OLED_LINES][OLED_X_RESOLUTION];	// rows are pages except for feather 128x64 display
} display_buffer_t;

static display_buffer_t *display_buffer;

static const uint8_t display_off[] =	{ 1, 0xAE };			// Display OFF (sleep mode)

#if OLED_DRIVER == OLED_SH1107
static const uint8_t clk_div_ratio[] =  { 2, 0xD5, 0x41 };		// --set display clock divide ratio/oscillator frequency POR
#ifdef OLED_PAGE_VERTICAL
static const uint8_t mplx_ratio[] =     { 2, 0xA8, OLED_Y_RESOLUTION -1 };		// Set multiplex ratio 64
#else
static const uint8_t mplx_ratio[] =     { 2, 0xA8, OLED_X_RESOLUTION -1 };		// Set multiplex ratio 64
#endif
#ifdef OLED_PAGE_VERTICAL
static const uint8_t display_offset[] = { 2, 0xD3, 0x20 };		// Set display offset. 00 = no offset
#else
static const uint8_t display_offset[] = { 2, 0xD3, 0x60 };		// Set display offset. 00 = no offset
#endif
static const uint8_t start_line_adr[] = { 2, 0xDC, 0x00 };		// display start line
static const uint8_t dcdc_en[] =		{ 2, 0xAD, 0x8A };		// Set DC-DC enable
static const uint8_t adr_mode_horiz[] =	{ 1, 0x20 };			// Set Memory Addressing Mode?
static const uint8_t page_adr[] = 		{ 1, 0xB0 };			// Set Page Start Address for Page Addressing Mode, 0-7
static const uint8_t lower_col_adr [] =	{ 1, 0x00 };			// ---set low column address
static const uint8_t higher_col_adr[] =	{ 1, 0x10 };			// ---set high column address
static const uint8_t seg_remap[] = 		{ 1, 0xA0 };			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
#ifdef OLED_PAGE_VERTICAL
static const uint8_t com_scan_rev[] = 	{ 1, 0xC8 };			// Set COM Output Scan Direction
#else
static const uint8_t com_scan_rev[] = 	{ 1, 0xC0 };			// Set COM Output Scan Direction
#endif
static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x6E };		// Set contrast control register (128)
static const uint8_t div_ratio[] =      { 1, 0xE3 };			// NOP
static const uint8_t com_pin_hw[] =     { 1, 0xE3 };			// NOP
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x35};		// --set vcomh 0x20,0.77xVcc
static const uint8_t pre_charge[] =     { 2, 0xD9, 0x22 };		// Set pre-charge period & SH1107
#elif OLED_DRIVER == OLED_SSD1306
// SSD1306
static const uint8_t clk_div_ratio[] =  { 2, 0xD5, 0x80 };		// --set display clock divide ratio/oscillator frequency
static const uint8_t mplx_ratio[] =     { 2, 0xA8, OLED_Y_RESOLUTION -1 };		// Set multiplex ratio 32 or 64
static const uint8_t display_offset[] = { 2, 0xD3, 0x00 };		// Set display offset. 00 = no offset
static const uint8_t start_line_adr[] =	{ 1, 0x40 };			// --set start line address
static const uint8_t dcdc_en[] =		{ 2, 0x8D, 0x14 };		// Set DC-DC enable
static const uint8_t adr_mode_horiz[] =	{ 2, 0x20, 0b00 };		// Set Memory Addressing Mode
static const uint8_t page_adr[] = 		{ 1, 0xB0 };			// Set Page Start Address for Page Addressing Mode, 0-7
static const uint8_t lower_col_adr [] =	{ 1, 0x00 };			// ---set low column address
static const uint8_t higher_col_adr[] =	{ 1, 0x10 };			// ---set high column address
static const uint8_t seg_remap[] = 		{ 1, 0xA1 };			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
static const uint8_t com_scan_rev[] = 	{ 1, 0xC8 };			// Set COM Output Scan Direction
static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x8F };		// Set contrast control register
static const uint8_t div_ratio[] =      { 1, 0xF0 };			// --set divide ratio
#if OLED_DISPLAY_TYPE == OLED_BONNET_128X64
static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x12 };		// Set com pins hardware configuration adafruit bonnet
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x40};		// --set vcomh 0x20,0.77xVcc adafruit bonnet
static const uint8_t pre_charge[] =     { 2, 0xD9, 0xF1 };		// Set pre-charge period adafruit bonnet
#else
static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x02 };		// Set com pins hardware configuration
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x20};		// --set vcomh 0x20,0.77xVcc
static const uint8_t pre_charge[] =     { 2, 0xD9, 0x22 };		// Set pre-charge period & SH1107
#endif // BONNET
#elif OLED_DRIVER == OLED_SSD1315
static const uint8_t seg_remap[] = 		{ 1, 0xA1 };			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
static const uint8_t com_scan_rev[] = 	{ 1, 0xC8 };			// Set COM Output Scan Direction
static const uint8_t dcdc_en[] =		{ 2, 0x8D, 0x14 };		// Set DC-DC disable
static const uint8_t adr_mode_horiz[] =	{ 2, 0x20, 0b00 };		// Set Memory Addressing Mode
static const uint8_t start_line_adr[] =	{ 1, 0x40 };			// --set start line address
static const uint8_t clk_div_ratio[] =  { 2, 0xD5, 0x80 };		// --set display clock divide ratio/oscillator frequency (default)
static const uint8_t mplx_ratio[] =     { 2, 0xA8, OLED_Y_RESOLUTION -1 };		// Set multiplex ratio 32 or 64
static const uint8_t display_offset[] = { 2, 0xD3, 0x00 };		// Set display offset. 00 = no offset
static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x7F };		// Set contrast control register (default)
static const uint8_t lower_col_adr [] =	{ 1, 0x00 };			// ---set low column address
static const uint8_t higher_col_adr[] =	{ 1, 0x10 };			// ---set high column address ?
static const uint8_t page_adr[] = 		{ 1, 0xB0 };			// Set Page Start Address for Page Addressing Mode, 0-7

static const uint8_t div_ratio[] =      { 1, 0xE3 };			// --set divide ratio (NOP)
static const uint8_t com_pin_hw[] =		{ 1, 0xE3 };			// Set com pins hardware configuration (NOP)
static const uint8_t set_vcomh[] =		{ 1, 0xE3 };			// --set vcomh (NOP)
static const uint8_t pre_charge[] =     { 1, 0xE3 };			// Set pre-charge period (NOP)
#endif // OLED_DRIVER

static const uint8_t ram_to_display[] = { 1, 0xA4 };			// Output RAM to Display
static const uint8_t display_normal[] =	{ 1, 0xA6 };			// Set display mode. A6=Normal;
//static const uint8_t display_inverse[] ={ 1, 0xA7 };			// A7=Inverse
static const uint8_t display_on[] =		{ 1, 0xAF };			// Display ON in normal mode


static const uint8_t *ssd1306_init_sequence[] = {	// Initialization Sequence
		display_off,
		adr_mode_horiz,
		page_adr,
		com_scan_rev,
		lower_col_adr,
		higher_col_adr,
		start_line_adr,
		set_contrast,
		seg_remap,
		display_normal,
		mplx_ratio,
		ram_to_display,
		display_offset,
		clk_div_ratio,
		div_ratio,
		pre_charge,
		com_pin_hw,
		set_vcomh,
		dcdc_en,
		display_on
};


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the OLED controller.
 *  @return
 *      None
 */
void OLED_init(void) {
	uint8_t i;

#ifndef OLED_SPI
	if (HAL_I2C_IsDeviceReady(&hi2c1, OLED_I2C_ADR << 1, 5, 100) != HAL_OK) {
		// OLED is not ready
		return;
	}
#else
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET);	// reset OLED
	osDelay(20);
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);

#endif
	oledReady = TRUE;

	display_buffer = pvPortMalloc(sizeof(display_buffer_t));

	for (i = 0; i < membersof(ssd1306_init_sequence); i++) {
		OLED_sendCommand(ssd1306_init_sequence[i]);
	}

#ifndef OLED_SPI
	if (OLED_readStatus() == 7) {
		// most probably a SH1107 driver
		;
	} else if (OLED_readStatus() == 3) {
		// most probably a SSD1306 driver
		;
	} else {
		// the display is BUSY or OFF
		;
	}
#endif

//	OLED_clear();
//	OLED_setPos(127-spyr_height-8, 0);
//	OLED_putXBM(spyr_bits, spyr_width, spyr_height);
//
//	OLED_setPos(0, 0);
//	OLED_setFont(OLED_FONT8X8);
//	OLED_puts("Mecrisp-");
//	OLED_setPos(93, 0);
//	OLED_puts("Cube\r\n");
//	OLED_puts("v" MECRISP_CUBE_TAG "\r\n\r\n");
//
//	OLED_setFont(OLED_FONT6X8);
//	OLED_puts(BOARD "\r\n");
//	OLED_puts("Forth for\r\n");
//	OLED_puts("the STM32WB \r\n");
//	OLED_puts("(c)2022\r\n");
//	OLED_puts("peter@spyr.ch");

	OLED_clear();
	OLED_setPos(0, 0);
	OLED_putXBM(mecrisp_cube_logo_bits, mecrisp_cube_logo_width, mecrisp_cube_logo_height);

	OLED_setFont(OLED_FONT12X16);
	OLED_setPos(34, 0);
	OLED_puts("MECRISP\r\n");
	OLED_setPos(34, 2);
	OLED_puts("CUBE4TH\r\n");

	OLED_setFont(OLED_FONT6X8);
	OLED_setPos(0, 5);
	OLED_puts(BOARD " v"  MECRISP_CUBE_TAG "\r\n");
	OLED_puts("Forth for the STM32WB\r\n");
	OLED_puts("(c)2024 peter@spyr.ch");


}


/**
 *  @brief
 *      Switch off the OLED controller.
 *  @return
 *      None
 */
void OLED_switchOff(void) {
	OLED_sendCommand(display_off);
}


/**
 *  @brief
 *      Writes a char to the OLED. Blocking until char is written into the controller memory.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int OLED_putc(int c) {
	if (!oledReady) {
		return EOF;
	}

	if (c == '\r') {
		// carriage return
		OLED_setPos(0, CurrentPosY);
		return 0;
	}

	switch (CurrentFont) {
	case OLED_FONT6X8:
		putGlyph6x8(c);
		break;
	case OLED_FONT8X8:
		putGlyph8x8(c);
		break;
	case OLED_FONT8X16:
		putGlyph8x16(c);
		break;
	case OLED_FONT12X16:
		putGlyph12x16(c);
		break;
	}

	return 0;
}


/**
 *  @brief
 *      OLED ready for next char.
 *  @return
 *      FALSE if the transfer is ongoing.
 */
int OLED_Ready(void) {
#ifndef OLED_SPI
	return (IIC_ready());
#else
	return TRUE;
#endif
}


/**
 *  @brief
 *      Writes a line (string) to the OLED. Blocking until
 *      string can be written.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      s  string to write. C style string.
 *  @return
 *      Return EOF on error, 0 on success.
 */
int OLED_puts(const char *s) {
	int i=0;

	while (s[i] != 0) {
		if (OLED_putc(s[i])) {
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
void OLED_setPos(uint8_t x, uint8_t y) {
	if (!oledReady) {
		return;
	}

	if ((x >= 0 && x < OLED_X_RESOLUTION) && (y >=0 && y < OLED_Y_RESOLUTION/8)) {
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
uint8_t OLED_getPosX() {
	return CurrentPosX;
}


/**
 *  @brief
 *  	Gets the current vertical position
 *  @return
 *      Y vertical position, max. 3 for 128x32 or 7 for 128x64 displays.
 */
uint8_t OLED_getPosY() {
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
void OLED_setFont(OLED_FontT font) {
	CurrentFont = font;
}


/**
 *  @brief
 *  	Clears the OLED display
 *  @return
 *      none
 */
void OLED_clear(void) {
	uint8_t i;

	if (!oledReady) {
		return;
	}

	memset(display_buffer->blob, 0, sizeof(display_buffer->blob));
	display_buffer->blob[0] =  0x40;  // write data

#ifdef OLED_PAGE_VERTICAL
	for (i=0; i<(128/8); i++) {
		OLED_setPos(i*8, 0);
		IIC_putMessage(display_buffer->blob, 65, OLED_I2C_ADR);
	}
#else
	for (i=0; i<OLED_LINES; i++) {
		OLED_setPos(0, i);
#ifndef OLED_SPI
		IIC_putMessage(display_buffer->blob, 129, OLED_I2C_ADR);
#else
		osMutexAcquire(RTSPI_MutexID, osWaitForever);
		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// Data
		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
		RTSPI_WriteData(display_buffer->blob +1, 128);
		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
		osMutexRelease(RTSPI_MutexID);
#endif
	}
#endif
	OLED_setPos(0, 0);
	display_buffer->blob[0] =  0;
}


/**
 *  @brief
 *  	Update the OLED display
 *  @return
 *      none
 */
void OLED_update(void) {
	int i;
	int j;
	int oldx = CurrentPosX;
	int oldy = CurrentPosY;

	uint8_t buf[9];

	if (!oledReady) {
		return;
	}

	buf[0] = 0x40;
	OLED_setPos(0, 0);

#ifdef OLED_PAGE_VERTICAL
	for (i=0; i<OLED_LINES; i++) {
		for (j=0; j<(OLED_X_RESOLUTION/8); j++) {
			transpose_page(0, 1, buf);
			OLED_setPos(j*8, i);
		}
	}

#else
	for (i=0; i<OLED_LINES; i++) {
		for (j=0; j<(OLED_X_RESOLUTION/8); j++) {
			memcpy(buf+1, &display_buffer->rows[i][j*8], 8);

			OLED_setPos(j*8, i);
#ifndef OLED_SPI
			IIC_putMessage(buf, 9, OLED_I2C_ADR);
#else
			osMutexAcquire(RTSPI_MutexID, osWaitForever);
			HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// Data
			HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
			RTSPI_WriteData(buf+1, 8);
			HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
			osMutexRelease(RTSPI_MutexID);
#endif
		}
	}
#endif
	OLED_setPos(oldx, oldy);

}

/**
 *  @brief
 *      Sends a command to the OLED controller
 *
 *      Max. length of a command is 4 bytes.
 *  @param[in]
 *  	First byte contains the length of the command.
 *  @return
 *      None
 */
void OLED_sendCommand(const uint8_t *command) {
	uint8_t buf[5];

	if (!oledReady) {
		return;
	}

	buf[0] = 0x00; // write command
	memcpy(&buf[1], &command[1], command[0]);
#ifndef OLED_SPI
	IIC_putMessage(buf, command[0]+1, OLED_I2C_ADR);
#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);	// command
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, command[0]);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
#endif

}


/**
 *  @brief
 *      Read the status and ID byte from the OLED controller
 *  @return
 *      None Bit7: BUSY; Bit6: ON 0, OFF 1; Bit5..7: ID 000111
 */
int OLED_readStatus(void) {
	uint8_t status=0;

#ifndef OLED_SPI
	IIC_getMessage(&status, 1, OLED_I2C_ADR);
#endif
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
void OLED_writeColumn(uint8_t column) {
#ifdef	OLED_PAGE_VERTICAL
	uint8_t buf[9];
	int i;
#else
	uint8_t buf[2];
#endif

	if (autowrap(' ', 1, 1)) {
		return ;
	}

	display_buffer->rows[CurrentPosY][CurrentPosX] = column;
	buf[0] = 0x40;  // write data

#ifdef	OLED_PAGE_VERTICAL
	// fill the buffer with 8 columns
	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}

	transpose_page(0, 1, buf);

#else

	// copy into I2C array
	buf[1] = display_buffer->rows[CurrentPosY][CurrentPosX];
#ifndef OLED_SPI
	IIC_putMessage(buf, 2, OLED_I2C_ADR);
#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 1);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
#endif
#endif

	postwrap(1, 1);
}


/**
 *  @brief
 *      Read a column (8 pixels) from the current position
 *  @return
 *      Column
 */
int OLED_readColumn(void) {
	return display_buffer->rows[CurrentPosY][CurrentPosX];
}


/**
 *  @brief
 *      Put XBM image to the OLED display
 *  @param[in]
 *  	image		image array (magick image.png -rotate 90 -flop image.xbm)
 *  @param[in]
 *  	width in pixel
 *  @return
 *  	height in pixel
 */
void OLED_putXBM(char* image, int width, int height) {
	int line;
	int column;
	int i=0;

	uint8_t x = CurrentPosX;
	uint8_t y = CurrentPosY;

	for (column=0; column<height; column++) {
		for (line=0; line<(width/8); line++) {
			OLED_setPos(column+x, line+y);
			OLED_writeColumn(image[i]);
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
 *  	y 0 .. OLED_LINES, page/line
 *  @return
 *      None
 */
static void setPos(uint8_t x, uint8_t y) {
	uint8_t buf[4];

	buf[0] = 0x00; // write command
#ifdef OLED_PAGE_VERTICAL
	buf[1] = 0xb0 + x/8; // page address
	buf[2] = (((y*8) & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	buf[3] = (y*8) & 0x0f; // | 0x01 // Set Lower Column Start Address
#else
	buf[1] = 0xb0 + y; // page address
	buf[2] = ((x & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	buf[3] = x & 0x0f; // | 0x01 // Set Lower Column Start Address
#endif
#ifndef OLED_SPI
	IIC_putMessage(buf, 4, OLED_I2C_ADR);
#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);	// command
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 3);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
#endif
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
#ifdef	OLED_PAGE_VERTICAL
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

#ifdef	OLED_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 8) + 6 >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#else
	// copy into I2C array
	for (i = 0; i < 6; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}

	#ifndef OLED_SPI
	IIC_putMessage(buf, 7, OLED_I2C_ADR);
	#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 6);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
	#endif // OLED_SPI
#endif // OLED_PAGE_VERTICAL

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

#ifdef	OLED_PAGE_VERTICAL
	// first page
	transpose_page(0, 1, buf);

	// second page
	if ((CurrentPosX % 8) + 8 >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
	}

#else
	// copy into I2C array
	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
	#ifndef OLED_SPI
	IIC_putMessage(buf, 9, OLED_I2C_ADR);
	#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 8);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
	#endif // OLED_SPI
#endif // OLED_PAGE_VERTICAL

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

#ifdef	OLED_PAGE_VERTICAL
	// first page, upper
	transpose_page(0, 1, buf);
	// first page, lower
	transpose_page(0, 0, buf);

	// second page
	if ((CurrentPosX % 8) + 8 >= 8)  {
		// second page needed
		transpose_page(1, 1, buf);
		transpose_page(1, 0, buf);
	}


#else
	// copy into I2C array
	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
	#ifndef OLED_SPI
	IIC_putMessage(buf, 9, OLED_I2C_ADR);
	#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 8);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
	#endif // OLED_SPI

	for (i = 0; i < 8; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY+i][CurrentPosX+1];
	}
	setPos(CurrentPosX, CurrentPosY+1);
	#ifndef OLED_SPI
	IIC_putMessage(buf, 9, OLED_I2C_ADR);
	#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 8);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
	#endif // OLED_SPI

#endif
	setPos(CurrentPosX+8, CurrentPosY);

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

#ifdef	OLED_PAGE_VERTICAL
	// first page, upper
	transpose_page(0, 1, buf);
	// first page, lower
	transpose_page(0, 0, buf);

	// second page, upper
	transpose_page(1, 1, buf);
	// first page, lower
	transpose_page(1, 0, buf);

	// third page
	if ((CurrentPosX % 8) + 8 >= 8)  {
		// third page needed
		transpose_page(2, 1, buf);
		transpose_page(2, 0, buf);
	}


#else
	// copy into I2C array
	for (i = 0; i < 12; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY][CurrentPosX+i];
	}
	#ifndef OLED_SPI
	IIC_putMessage(buf, 13, OLED_I2C_ADR);
	#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 12);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
	#endif // OLED_SPI

	for (i = 0; i < 12; i++) {
		buf[i+1] = display_buffer->rows[CurrentPosY+1][CurrentPosX+i];
	}
	setPos(CurrentPosX, CurrentPosY+1);
	#ifndef OLED_SPI
	IIC_putMessage(buf, 13, OLED_I2C_ADR);
	#else
	osMutexAcquire(RTSPI_MutexID, osWaitForever);
	HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);	// data
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	RTSPI_WriteData(buf+1, 12);
	HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
	osMutexRelease(RTSPI_MutexID);
	#endif // OLED_SPI

#endif
	setPos(CurrentPosX+12, CurrentPosY);

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
		if (CurrentPosY >= OLED_LINES) {
			CurrentPosY = 0;
		}
		OLED_setPos(CurrentPosX, CurrentPosY);
		return 1;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= width) {
			OLED_setPos(CurrentPosX-width, CurrentPosY);
		}
		return 1;
	}

	if (CurrentPosX > OLED_X_RESOLUTION - width) {
		// auto wrap line
		CurrentPosY += row;
		if (CurrentPosY >= OLED_LINES) {
			// auto wrap display
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
	}
	return 0;
}

#ifdef OLED_PAGE_VERTICAL
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
	IIC_putMessage(buf, 9, OLED_I2C_ADR);
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

	if (CurrentPosX >= OLED_X_RESOLUTION) {
		// auto wrap
		CurrentPosY += row;
		if (CurrentPosY >= OLED_LINES) {
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
#ifdef OLED_PAGE_VERTICAL
	} else {
		OLED_setPos(CurrentPosX, CurrentPosY);
#endif
	}
}


// XPM ? ICO (Favicon)
//void OLED_drawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
// OLED_X_RESOLUTIONx32/8=512
// bitmap?
//{
//	uint16_t j = 0;
//	uint8_t y;
//	if (y1 % 8 == 0) y = y1 / 8;
//	else y = y1 / 8 + 1;
//	for (y = y0; y < y1; y++)
//	{
//		ssd1306_setpos(x0,y);
//		ssd1306_send_data_start();
//		for (uint8_t x = x0; x < x1; x++)
//		{
//			ssd1306_send_byte(pgm_read_byte(&bitmap[j++]));
//		}
//		ssd1306_send_data_stop();
//	}
//}

#endif
