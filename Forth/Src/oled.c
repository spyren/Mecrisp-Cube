/**
 *  @brief
 *  	OLED driver based on the controller SSD1306 (and SH1107).
 *
 *  	Resolution 128x32 or 128x64, monochrome.
 *  	A page consists of 128 columns (horizontally, x) with 8 pixels (vertically, y).
 *  	The 8 pixels are in one byte.
 *  	There are 4 pages in 128x32 display, and 8 pages in a 128x64 display.
 *  	I2C Interface, address 60.
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
#include "iic.h"
#include "font6x8.h"
#include "font8x8.h"
#include "font8x14.h"
#include "font12x16.h"

// Macros
// ******
#define  membersof(x) (sizeof(x) / sizeof(x[0]))

// Private function prototypes
// ***************************
static void setPos(uint8_t x, uint8_t y);
static void sendChar6x8(int ch);
static void sendChar8x8(int ch);
static void sendChar8x16(int ch);
static void sendChar12x16(int ch);


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


static const uint8_t display_off[] =	{ 1, 0xAE };			// Display OFF (sleep mode)

#ifdef SH1107
//static const uint8_t clk_div_ratio[] =  { 2, 0xD5, 0x50 };		// --set display clock divide ratio/oscillator frequency POR
static const uint8_t clk_div_ratio[] =  { 2, 0xD5, 0x41 };		// --set display clock divide ratio/oscillator frequency POR
#ifdef SH1107_LANDSCAPE
static const uint8_t mplx_ratio[] =     { 2, 0xA8, OLED_Y_RESOLUTION -1 };		// Set multiplex ratio 64
#else
static const uint8_t mplx_ratio[] =     { 2, 0xA8, OLED_X_RESOLUTION -1 };		// Set multiplex ratio 64
#endif
#ifdef SH1107_LANDSCAPE
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
#ifdef SH1107_LANDSCAPE
static const uint8_t com_scan_rev[] = 	{ 1, 0xC8 };			// Set COM Output Scan Direction
#else
static const uint8_t com_scan_rev[] = 	{ 1, 0xC0 };			// Set COM Output Scan Direction
#endif
//static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x4F };		// Set contrast control register (128)
static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x6E };		// Set contrast control register (128)
static const uint8_t div_ratio[] =      { 1, 0xE3 };			// NOP
static const uint8_t com_pin_hw[] =     { 1, 0xE3 };			// NOP
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x35};		// --set vcomh 0x20,0.77xVcc
static const uint8_t pre_charge[] =     { 2, 0xD9, 0x22 };		// Set pre-charge period & SH1107
#else
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
#ifdef BONNET
static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x12 };		// Set com pins hardware configuration adafruit bonnet
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x40};		// --set vcomh 0x20,0.77xVcc adafruit bonnet
static const uint8_t pre_charge[] =     { 2, 0xD9, 0xF1 };		// Set pre-charge period adafruit bonnet
#else
static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x02 };		// Set com pins hardware configuration
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x20};		// --set vcomh 0x20,0.77xVcc
static const uint8_t pre_charge[] =     { 2, 0xD9, 0x22 };		// Set pre-charge period & SH1107
#endif

#endif
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

	BSP_setDigitalPin(11, 0); // D11 low (command)
	BSP_setDigitalPinMode(11, 3); // D11 output push pull

	if (HAL_I2C_IsDeviceReady(&hi2c1, OLED_I2C_ADR << 1, 5, 100) != HAL_OK) {
		// OLED is not ready
		return;
	}
	oledReady = TRUE;

	for (i = 0; i < membersof(ssd1306_init_sequence); i++) {
		OLED_sendCommand(ssd1306_init_sequence[i]);
	}
	if (OLED_readStatus() != 7) {
		// the display is BUSY or OFF or the ID is not 7
		;
	}
	OLED_clear();
	OLED_setPos(0,0);
	OLED_setFont(OLED_FONT8X8);
	OLED_puts("Mecrisp-Cube");
	OLED_setFont(OLED_FONT6X8);
	OLED_puts(MECRISP_CUBE_TAG);
#ifdef DEBUG
	OLED_puts("\r\nDebug\r\n");
#else
	OLED_puts("\r\n\r\n");
#endif
	OLED_puts("Forth for the STM32WB\r\n");
	OLED_puts("(c)2021 peter@spyr.ch");

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
		sendChar6x8(c);
		break;
	case OLED_FONT8X8:
		sendChar8x8(c);
		break;
	case OLED_FONT8X16:
		sendChar8x16(c);
		break;
	case OLED_FONT12X16:
		sendChar12x16(c);
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
	return (IIC_ready());
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
	uint8_t buf[129];
	uint8_t i;

	if (!oledReady) {
		return;
	}

	buf[0] = 0x40;  // write data
	memset(&buf[1], 0, 128);
#ifdef SH1107_LANDSCAPE
	for (i=0; i<(128/8); i++) {
		OLED_setPos(i*8, 0);
		IIC_setDevice(OLED_I2C_ADR);
		IIC_putMessage(buf, 65);
	}
#else
	for (i=0; i<OLED_LINES; i++) {
		OLED_setPos(0, i);
		IIC_setDevice(OLED_I2C_ADR);
		IIC_putMessage(buf, 129);
	}
#endif
	OLED_setPos(0, 0);
}


/**
 *  @brief
 *      Sends a command to the OLED controller
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
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, command[0]+1);
}

#ifdef SH1107
/**
 *  @brief
 *      Read the status and ID byte from the OLED controller
 *  @return
 *      None Bit7: BUSY; Bit6: ON 0, OFF 1; Bit5..7: ID 000111
 */
int OLED_readStatus(void) {
	uint8_t status;

	IIC_setDevice(OLED_I2C_ADR);
	IIC_getMessage(&status, 1);
	return status;
}


/**
 *  @brief
 *      Read data from display RAM into a buffer at the Cursor Position
 *
 *         page x   page y   page z   page t
 *       | Byte 0 | Byte 1 | Byte 2 | Byte 3 |
 *       |01234567|01234567|01234567|01234567|
 *       +--------+--------+--------+--------+
 *      0|aaaaaaaa|        |        |        |
 *      1|bbbbbbbb|        |        |        |
 *      2|cccccccc|        |        |        |
 *      3|dddddddd|        |        |        |
 *      4|eeeeeeee|        |        |        |
 *      5|ffffffff|        |        |        |
 *      6|gggggggg|        |        |        |
 *      7|hhhhhhhh|        |        |        |
 *
 *  @param[in]
 *  	Buffer  see above
 *  @return
 *      None
 */
void OLED_readRAM(unsigned int *buffer, const int pages) {
	int byte_i;
	int row_i;

	uint8_t data[8];

	if (!oledReady) {
		return;
	}

	// clear buffer
	for (row_i = 0; row_i < 8; row_i++) {
		buffer[row_i] = 0;
		data[row_i] = 0;
	}

	// read RAM
	for (byte_i = 0; byte_i < pages; byte_i++) {
		if (CurrentPosX+byte_i*8 > OLED_X_RESOLUTION) {
			break;
		}
		setPos(CurrentPosX+byte_i*8, CurrentPosY);

		BSP_setDigitalPin(11, 1); // D11 high (data)
		IIC_setDevice(OLED_I2C_ADR_DATA);
		IIC_getMessage(data, 8);
		BSP_setDigitalPin(11, 0); // D11 low (command)

		// move page to the byte position
		for (row_i = 0; row_i < 8; row_i++) {
			buffer[row_i] |= (data[row_i] << (byte_i*8));
		}
	}
}

/**
 *  @brief
 *      Write data to display RAM at the Cursor Position
 *
 *         page x   page y   page z   page t
 *       | Byte 0 | Byte 1 | Byte 2 | Byte 3 |
 *       |01234567|01234567|01234567|01234567|
 *       +--------+--------+--------+--------+
 *      0|aaaaaaaa|        |        |        |
 *      1|bbbbbbbb|        |        |        |
 *      2|cccccccc|        |        |        |
 *      3|dddddddd|        |        |        |
 *      4|eeeeeeee|        |        |        |
 *      5|ffffffff|        |        |        |
 *      6|gggggggg|        |        |        |
 *      7|hhhhhhhh|        |        |        |
 *
 *  @param[in]
 *  	buffer see above
 *  @return
 *      None
 */
void OLED_writeRAM(unsigned int *buffer, const int pages) {
	int byte_i;
	int row_i;

	uint8_t data[9];

	if (!oledReady) {
		return;
	}

	// write RAM
	data[0] = 0x40;  // write data
	for (byte_i = 0; byte_i < pages; byte_i++) {
		if (CurrentPosX+byte_i*8 > OLED_X_RESOLUTION) {
			break;
		}
		setPos(CurrentPosX+byte_i*8, CurrentPosY);

		// prepare buffer
		for (row_i = 0; row_i < 8; row_i++) {
			data[row_i+1] = (buffer[row_i] << (byte_i*8)) & 0xFF;
		}

		IIC_setDevice(OLED_I2C_ADR);
		IIC_putMessage(data, 9);
		// write direct into the display RAM
//		BSP_setDigitalPin(11, 1); // D11 high (data)
//		IIC_setDevice(OLED_I2C_ADR_DATA);
//		IIC_putMessage(data+1, 8);
//		BSP_setDigitalPin(11, 0); // D11 low (command)
	}
}

#endif

// Private Functions
// *****************

static void setPos(uint8_t x, uint8_t y) {
	uint8_t buf[4];

	buf[0] = 0x00; // write command
#ifdef SH1107_LANDSCAPE
	buf[1] = 0xb0 + x/8; // page address
	buf[2] = (((y*8) & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	buf[3] = (y*8) & 0x0f; // | 0x01 // Set Lower Column Start Address
#else
	buf[1] = 0xb0 + y; // page address
	buf[2] = ((x & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	buf[3] = x & 0x0f; // | 0x01 // Set Lower Column Start Address
#endif
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 4);
}


static void sendChar6x8(int ch) {
#ifdef	SH1107_LANDSCAPE
	uint8_t buf[9];
#else
	uint8_t buf[7];
#endif
	uint8_t i;

	if (ch == '\n') {
		// line feed
		CurrentPosY++;
		if (CurrentPosY*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(CurrentPosX, CurrentPosY);
		return;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= 6) {
			OLED_setPos(CurrentPosX-6, CurrentPosY);
		}
		return ;
	}

	if (CurrentPosX > OLED_X_RESOLUTION - 6) {
		// auto wrap
		CurrentPosY++;
		if (CurrentPosY*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
	}

#ifdef	SH1107_LANDSCAPE
	unsigned int ram[8];

	OLED_readRAM(ram, 2);
	FONT6X8_transposeGlyph(ch, buf);
	for (i = 0; i < 8; i++) {
		// clear 6 bits
		ram[i] &= ~(0x3F << (CurrentPosX % 8));
		// set 6 bits
		ram[i] |= buf[i] << (CurrentPosX % 8);
//		ram[i] |= buf[i];
	}
	OLED_writeRAM(ram, 2);


#else
	for (i = 0; i < 6; i++) {
		buf[i+1] = FONT6X8_getColumn(ch, i);
	}
	buf[0] = 0x40;  // write data
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 7);
#endif

	CurrentPosX += 6;
	if (CurrentPosX >= OLED_X_RESOLUTION) {
		// auto wrap
		CurrentPosY++;
		if (CurrentPosY*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
	}

#ifdef SH1107_LANDSCAPE
	OLED_setPos(CurrentPosX, CurrentPosY);
#endif

}


static void sendChar8x8(int ch) {
	uint8_t buf[9];
	uint8_t i;

	if (ch == '\n') {
		// line feed
		CurrentPosY++;
		if (CurrentPosY*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(CurrentPosX, CurrentPosY);
		return;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= 8) {
			OLED_setPos(CurrentPosX-8, CurrentPosY);
		}
		return;
	}

	if (CurrentPosX > OLED_X_RESOLUTION - 8) {
		// auto wrap
		CurrentPosY++;
		if (CurrentPosY*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
	}

	buf[0] = 0x40;  // write data
#ifdef	SH1107_LANDSCAPE
	for (i = 0; i < 8; i++) {
		buf[i+1] = 0;
	}

	FONT8X8_transposeGlyph(ch, buf+1);
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 9);
#else
	uint8_t tx[2];
	tx[0] = 0x40;  // write data
	uint8_t j;

	// fill the array with 8 columns
	for (i = 0; i < 8; i++) {
		buf[i+1] = FONT8X8_getColumn(ch, i);
	}

	// write rows
	for (j = 0; j < 8; j++) {

		for (i = 0; i < 8; i++) {
			if (buf[i]) {

			}
			tx[1] = bit j buf[i];
		}
		IIC_setDevice(OLED_I2C_ADR);
		IIC_putMessage(tx, 2);
	}


#endif

	CurrentPosX += 8;
#ifdef SH1107_LANDSCAPE
	OLED_setPos(CurrentPosX, CurrentPosY);
#endif
}


static void sendChar8x16(int ch) {
	uint8_t buf[9];
	uint8_t i;

	if (ch == '\n') {
		// line feed
		CurrentPosY += 2;
		if ((CurrentPosY+1)*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		setPos(CurrentPosX, CurrentPosY);
		return;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= 8) {
			OLED_setPos(CurrentPosX-12, CurrentPosY);
		}
		return;
	}

	if (CurrentPosX > OLED_X_RESOLUTION - 8) {
		CurrentPosY += 2;
		if ((CurrentPosY+1)*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
	}

	buf[0] = 0x40;  // write data
	for (i = 0; i < 8; i++) {
		buf[i+1] = FONT8X14_getUpperColumn(ch, i);
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 9);

	setPos(CurrentPosX, CurrentPosY+1);
	for (i = 0; i < 8; i++) {
		buf[i+1] = FONT8X14_getLowerColumn(ch, i);
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 9);

	CurrentPosX += 8;
	setPos(CurrentPosX, CurrentPosY);
}


static void sendChar12x16(int ch) {
	uint8_t buf[13];
	uint8_t i;

	if (ch == '\n') {
		// line feed
		CurrentPosY += 2;
		if ((CurrentPosY+1)*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		setPos(CurrentPosX, CurrentPosY);
		return;
	}
	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= 12) {
			OLED_setPos(CurrentPosX-12, CurrentPosY);
		}
		return;
	}

	if (CurrentPosX > OLED_X_RESOLUTION - 12) {
		CurrentPosY += 2;
		if ((CurrentPosY+1)*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
	}

	buf[0] = 0x40;  // write data
	for (i = 0; i < 12; i++) {
		buf[i+1] = FONT12X16_getUpperColumn(ch, i);
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 13);

	setPos(CurrentPosX, CurrentPosY+1);
	for (i = 0; i < 12; i++) {
		buf[i+1] = FONT12X16_getLowerColumn(ch, i);
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 13);

	CurrentPosX += 12;
	setPos(CurrentPosX, CurrentPosY);
}

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

