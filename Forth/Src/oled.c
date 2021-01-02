/**
 *  @brief
 *  	OLED driver based on the controller SSD1306.
 *
 *  	Resolution 128x32 or 128x64, monochrome.
 *  	I2C Interface.
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
#include "6x8_vertikal_LSB_1.h"
#include "8x14_vertikal_LSB_1.h"
#include "12x16_vertikal_LSB_1.h"
#include "8x8_vertikal_LSB_1.h"

// Macros
// ******
#define  membersof(x) (sizeof(x) / sizeof(x[0]))

// Private function prototypes
// ***************************
static void setPos(uint8_t x, uint8_t y);
static void sendChar6x8(int ch);
static void sendChar8x16(int ch);
static void sendChar12x16(int ch);
static void sendChar8x8(int ch);


// Global Variables
// ****************

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;

// RTOS resources
// **************

// Private Variables
// *****************

uint8_t oledReady = FALSE;

uint8_t CurrentPosX = 0;
uint8_t CurrentPosY = 0;

OLED_FontT CurrentFont = OLED_FONT6X8;


// Adafruit
static const uint8_t display_off[] =	{ 1, 0xAE };			// Display OFF (sleep mode)
static const uint8_t clk_div_ratio[] =  { 2, 0xD5, 0x80 };		// --set display clock divide ratio/oscillator frequency
static const uint8_t mplx_ratio[] =     { 2, 0xA8, 31 };		// Set multiplex ratio 32 (1 to 64)
//static const uint8_t mplx_ratio[] =     { 2, 0xA8, 38 };		// Set multiplex ratio 39 (1 to 64)
//static const uint8_t mplx_ratio[] =     { 2, 0xA8, 63 };		// Set multiplex ratio 64 (1 to 64)
static const uint8_t display_offset[] = { 2, 0xD3, 0x00 };		// Set display offset. 00 = no offset
static const uint8_t start_line_adr[] =	{ 1, 0x40 };			// --set start line address
static const uint8_t dcdc_en[] =		{ 2, 0x8D, 0x14 };		// Set DC-DC enable
static const uint8_t adr_mode_horiz[] =	{ 2, 0x20, 0b00 };		// Set Memory Addressing Mode
							// 00=Horizontal Addressing Mode; 01=Vertical Addressing Mode;
							// 10=Page Addressing Mode (RESET); 11=Invalid
static const uint8_t page_adr[] = 		{ 1, 0xB0 };			// Set Page Start Address for Page Addressing Mode, 0-7
static const uint8_t lower_col_adr [] =	{ 1, 0x00 };			// ---set low column address
static const uint8_t higher_col_adr[] =	{ 1, 0x10 };			// ---set high column address
static const uint8_t seg_remap[] = 		{ 1, 0xA1 };			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
static const uint8_t com_scan_rev[] = 	{ 1, 0xC8 };			// Set COM Output Scan Direction
static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x02 };		// Set com pins hardware configuration
//static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x12 };		// Set com pins hardware configuration
static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x8F };		// Set contrast control register
//static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x3F };		// Set contrast control register
//static const uint8_t pre_charge[] =     { 2, 0xD9, 0xF1 };		// Set pre-charge period
static const uint8_t pre_charge[] =     { 2, 0xD9, 0x22 };		// Set pre-charge period
//static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x40};		// --set vcomh 0x20,0.77xVcc
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x20};		// --set vcomh 0x20,0.77xVcc
static const uint8_t ram_to_display[] = { 1, 0xA4 };			// Output RAM to Display
						// 0xA4=Output follows RAM content; 0xA5,Output ignores RAM content
static const uint8_t div_ratio[] =      { 1, 0xF0 };			// --set divide ratio
static const uint8_t display_normal[] =	{ 1, 0xA6 };			// Set display mode. A6=Normal;


static const uint8_t display_inverse[] ={ 1, 0xA7 };			// A7=Inverse

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
		div_ratio, //
		pre_charge,
		com_pin_hw,
		set_vcomh, //
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

	if (HAL_I2C_IsDeviceReady(&hi2c1, OLED_I2C_ADR << 1, 5, 100) != HAL_OK) {
		// OLED is not ready
		return;
	}
	oledReady = TRUE;

	for (i = 0; i < membersof(ssd1306_init_sequence); i++) {
//		OLED_sendCommand(ssd1306_init_sequence[i], sizeof(*ssd1306_init_sequence[i]));
		OLED_sendCommand(ssd1306_init_sequence[i]);
	}
	OLED_clear();
	OLED_setPos(0,0);
	OLED_puts("Mecrisp-Cube\n\r");
	OLED_puts("Forth for the STM32WB\n\r\n");
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
	case OLED_FONT8X16:
		sendChar8x16(c);
		break;
	case OLED_FONT12X16:
		sendChar12x16(c);
		break;
	case OLED_FONT8X8:
		sendChar8x8(c);
		break;
	}

	return 0;
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
 *      y  vertical position, max. 3 for 128x32 or 7 for 128x64 displays.
 *  @return
 *      none
 */
void OLED_setPos(uint8_t x, uint8_t y) {
	if (!oledReady) {
		return;
	}

	setPos(x, y);
	CurrentPosX = x;
	CurrentPosY = y;
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

	OLED_setPos(0, 0);
	buf[0] = 0x40;  // write data
	memset(&buf[1], 0, 128);
	for (i=0; i<4; i++) {
		IIC_setDevice(OLED_I2C_ADR);
		IIC_putMessage(buf, 129);
	}
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


// Private Functions
// *****************

static void setPos(uint8_t x, uint8_t y) {
	uint8_t buf[4];

	buf[0] = 0x00; // write command
	buf[1] = 0xb0 + y;
	buf[2] = ((x & 0xf0) >> 4) | 0x10; // | 0x10
	buf[3] = x & 0x0f; // | 0x01
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 4);
}


static void sendChar6x8(int ch) {
	uint8_t buf[7];
	uint8_t i;

	if (ch == '\n') {
		// line feed
		CurrentPosY++;
		OLED_setPos(CurrentPosX, CurrentPosY);
		return;
	}

	if (CurrentPosX > 128 - 6) {
		// auto wrap
		OLED_setPos(0, CurrentPosY+1);
	}

	buf[0] = 0x40;  // write data
	for (i = 0; i < 6; i++) {
		buf[i+1] = font_6x8[ch][i];
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 7);

	CurrentPosX += 6;
}


static void sendChar8x8(int ch) {
	uint8_t buf[9];
	uint8_t i;

	if (ch == '\n') {
		// line feed
		CurrentPosY++;
		OLED_setPos(CurrentPosX, CurrentPosY);
		return;
	}

	if (CurrentPosX > 128 - 8) {
		// auto wrap
		OLED_setPos(0, CurrentPosY+1);
	}

	buf[0] = 0x40;  // write data
	for (i = 0; i < 8; i++) {
		buf[i+1] = font_8x8[ch][i];
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 9);

	CurrentPosX += 8;
}


static void sendChar8x16(int ch) {
	uint8_t buf[9];
	uint8_t i;

	if (ch == '\n') {
		// line feed
		CurrentPosY += 2;
		setPos(CurrentPosX, CurrentPosY);
		return;
	}

	if (CurrentPosX > 128 - 8) {
		OLED_setPos(0, CurrentPosY+2);
	}

	buf[0] = 0x40;  // write data
	for (i = 0; i < 8; i++) {
		buf[i+1] = font_8x14[ch][2*i];
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 9);

	setPos(CurrentPosX, CurrentPosY+1);
	for (i = 0; i < 8; i++) {
		buf[i+1] = font_8x14[ch][2*i+1];
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
		setPos(CurrentPosX, CurrentPosY);
		return;
	}

	if (CurrentPosX > 128 - 12) {
		OLED_setPos(0, CurrentPosY+2);
	}

	buf[0] = 0x40;  // write data
	for (i = 0; i < 12; i++) {
		buf[i+1] = font_12x16[ch][2*i];
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 13);

	setPos(CurrentPosX, CurrentPosY+1);
	for (i = 0; i < 12; i++) {
		buf[i+1] = font_12x16[ch][2*i+1];
	}
	IIC_setDevice(OLED_I2C_ADR);
	IIC_putMessage(buf, 13);

	CurrentPosX += 12;
	setPos(CurrentPosX, CurrentPosY);
}

//void OLED_drawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, const uint8_t bitmap[])
// 128x32/8=512
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

