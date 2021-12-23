/**
 *  @brief
 *  	OLED driver based on the controller SSD1315Z (similar to the SSD1306).
 *
 *  	128x64, monochrome.
 *  	A page consists of 128 columns (horizontally, x) with 8 pixels (vertically, y).
 *  	The 8 pixels are in one byte.
 *  	There are 8 pages.
 *  	4-wire SPI Interface, and some control pins OLED_CS, OLED_DC, OLED_RST.
 *  	See https://www.mikrocontroller.net/topic/54860 for the fonts.
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
#include "sd_spi.h"
#include "font6x8.h"
#include "font8x8.h"
#include "font8x14.h"
#include "font12x16.h"

// Macros
// ******
#define  membersof(x) (sizeof(x) / sizeof(x[0]))

#define WRITE_RAM_COMMAND	(0x5C)
#define SET_COLUMN_ADR		(0x15)
#define SET_ROW_ADR			(0x75)

#define SET_DISPLAY_ALL_OFF	(0xA4)
#define SET_DISPLAY_ALL_ON	(0xA5)
#define SET_DISPLAY_NORMAL	(0xA6)
#define SET_DISPLAY_INVERSE	(0xA7)
#define SET_SLEEP_ON        (0xAE)
#define SET_SLEEP_OFF       (0xAF)

#define FRONT_LOCK_DIVIDER	(0xB3)
#define SET_PRE_CHARGE_V	(0xBB)
#define SET_VCOMH_V			(0xBE)
#define SET_CONTRAST_I		(0xC1)
#define MASTER_CONTRAST		(0xC7)
#define SET_MUX_RATIO		(0xCA)
#define SET_COMMAND_LOCK	(0xFD)
#define HORIZONTAL_SCROLL   (0x96)
#define STOP_MOVING			(0x9E)
#define START_MOVING		(0x9F)

// Private function prototypes
// ***************************
static void setPos(uint8_t x, uint8_t y);
static void sendChar6x8(int ch);
static void sendChar8x8(int ch);
static void sendChar8x16(int ch);
static void sendChar12x16(int ch);
void put_message(uint8_t *TxBuffer, uint32_t TxSize);

// Global Variables
// ****************

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;

// RTOS resources
// **************

// Private Variables
// *****************

static uint8_t CurrentPosX = 0;
static uint8_t CurrentPosY = 0;

static OLED_FontT CurrentFont = OLED_FONT6X8;

static const uint8_t unlock[] =         { 2, SET_COMMAND_LOCK, 0x12 };		// unlock driver chip
static const uint8_t display_off[] =	{ 1, SET_SLEEP_ON };			    // Display OFF (sleep mode)
static const uint8_t clk_div_ratio[] =  { 2, FRONT_LOCK_DIVIDER, 0x80 };		// --set display clock divide ratio/oscillator frequency
static const uint8_t mplx_ratio[] =     { 2, SET_MUX_RATIO, OLED_Y_RESOLUTION -1 };		// Set multiplex ratio 32 (1 to 64)
//static const uint8_t display_offset[] = { 2, 0xD3, 0x00 };		// Set display offset. 00 = no offset
//static const uint8_t start_line_adr[] =	{ 1, 0x40 };			// --set start line address
//static const uint8_t dcdc_en[] =		{ 2, 0x8D, 0x14 };		// Set DC-DC enable
//static const uint8_t adr_mode_horiz[] =	{ 2, 0x20, 0b00 };		// Set Memory Addressing Mode
//static const uint8_t page_adr[] = 		{ 1, 0xB0 };			// Set Page Start Address for Page Addressing Mode, 0-7
//static const uint8_t lower_col_adr [] =	{ 1, 0x00 };			// ---set low column address
//static const uint8_t higher_col_adr[] =	{ 1, 0x10 };			// ---set high column address
//static const uint8_t seg_remap[] = 		{ 1, 0xA1 };			// Set Segment Re-map. A0=address mapped; A1=address 127 mapped.
//static const uint8_t com_scan_rev[] = 	{ 1, 0xC8 };			// Set COM Output Scan Direction
#ifdef BONNET
static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x12 };		// Set com pins hardware configuration adafruit bonnet
#else
//static const uint8_t com_pin_hw[] =		{ 2, 0xDA, 0x02 };		// Set com pins hardware configuration
#endif
//static const uint8_t set_contrast[] = 	{ 2, 0x81, 0x8F };		// Set contrast control register
#ifdef BONNET
static const uint8_t pre_charge[] =     { 2, 0xD9, 0xF1 };		// Set pre-charge period adafruit bonnet
#else
//static const uint8_t pre_charge[] =     { 2, 0xD9, 0x22 };		// Set pre-charge period
#endif
#ifdef BONNET
static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x40};		// --set vcomh 0x20,0.77xVcc adafruit bonnet
#else
//static const uint8_t set_vcomh[] =		{ 2, 0xDB, 0x20};		// --set vcomh 0x20,0.77xVcc
#endif
//static const uint8_t ram_to_display[] = { 1, 0xA4 };			// Output RAM to Display
//static const uint8_t div_ratio[] =      { 1, 0xF0 };			// --set divide ratio
static const uint8_t display_normal[] =	{ 1, SET_DISPLAY_NORMAL };			// Set display mode. A6=Normal;
//static const uint8_t display_inverse[] ={ 1, 0xA7 };			// A7=Inverse
static const uint8_t display_on[] =		{ 1, SET_SLEEP_OFF };			// Display ON in normal mode


static const uint8_t *SSD1315_init_sequence[] = {	// Initialization Sequence
		display_off,
//		adr_mode_horiz,
//		page_adr,
//		com_scan_rev,
//		lower_col_adr,
//		higher_col_adr,
//		start_line_adr,
//		set_contrast,
//		seg_remap,
		display_normal,
		mplx_ratio,
//		ram_to_display,
//		display_offset,
		clk_div_ratio,
//		div_ratio, //
//		pre_charge,
//		com_pin_hw,
//		set_vcomh, //
//		dcdc_en,
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

	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET);

	for (i = 0; i < membersof(SSD1315_init_sequence); i++) {
		OLED_sendCommand(SSD1315_init_sequence[i]);
	}
	OLED_clear();
	OLED_setPos(0,0);
	OLED_setFont(OLED_FONT8X8);
	OLED_puts("Mecrisp-Cube\r\n\n");
	OLED_setFont(OLED_FONT6X8);
	OLED_puts("Forth for the STM32WB\r\n");
	OLED_puts("(c)2021 peter@spyr.ch");
#ifdef DEBUG
	OLED_setPos(0,1);
	OLED_puts("Debug");
#endif
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
	return TRUE;
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

	buf[0] = 0x5C;  // write data
	memset(&buf[1], 0, OLED_X_RESOLUTION);
	for (i=0; i<OLED_LINES; i++) {
		OLED_setPos(0, i);
		put_message(buf, 129);
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

	memcpy(&buf[0], &command[1], command[0]);
	put_message(buf, command[0]);
}


// Private Functions
// *****************

static void setPos(uint8_t x, uint8_t y) {
	uint8_t buf[4];
	//	buf[1] = 0xb0 + y; // page address
	//	buf[2] = ((x & 0xf0) >> 4) | 0x10; // Set Higher Column Start Address
	//	buf[3] = x & 0x0f; // | 0x01 // Set Lower Column Start Address

	buf[0] = 0x15;	// set column address
	buf[1] = x; 	// start address
	buf[2] = 127;	// end address
	put_message(buf, 3);

	buf[0] = 0x75;	// set row (page) address
	buf[1] = x; 	// start address
	buf[2] = 127;	// end address

}


static void sendChar6x8(int ch) {
	uint8_t buf[7];
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

	buf[0] = 0x40;  // write data
	for (i = 0; i < 6; i++) {
		buf[i+1] = FONT6X8_getColumn(ch, i);
	}
	put_message(buf, 7);

	CurrentPosX += 6;
	if (CurrentPosX >= OLED_X_RESOLUTION) {
		// auto wrap
		CurrentPosY++;
		if (CurrentPosY*8 >= OLED_Y_RESOLUTION) {
			CurrentPosY = 0;
		}
		OLED_setPos(0, CurrentPosY);
	}

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
	for (i = 0; i < 8; i++) {
		buf[i+1] = FONT8X8_getColumn(ch, i);
	}
	put_message(buf, 9);

	CurrentPosX += 8;
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
	put_message(buf, 9);

	setPos(CurrentPosX, CurrentPosY+1);
	for (i = 0; i < 8; i++) {
		buf[i+1] = FONT8X14_getLowerColumn(ch, i);
	}
	put_message(buf, 9);

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
	put_message(buf, 13);

	setPos(CurrentPosX, CurrentPosY+1);
	for (i = 0; i < 12; i++) {
		buf[i+1] = FONT12X16_getLowerColumn(ch, i);
	}
	put_message(buf, 13);

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
//		SSD1315_setpos(x0,y);
//		SSD1315_send_data_start();
//		for (uint8_t x = x0; x < x1; x++)
//		{
//			SSD1315_send_byte(pgm_read_byte(&bitmap[j++]));
//		}
//		SSD1315_send_data_stop();
//	}
//}


void put_message(uint8_t *TxBuffer, uint32_t TxSize) {
	  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET);
	  HAL_GPIO_WritePin(GPIOC, OLED_DC_Pin, GPIO_PIN_RESET); // first byte is always a command
	  SDSPI_Write(TxBuffer[0]);
	  if (TxSize > 1) {
		  HAL_GPIO_WritePin(GPIOC, OLED_DC_Pin, GPIO_PIN_SET); // remaining bytes are data
		  SDSPI_WriteReadData(TxBuffer+1, TxBuffer+1, TxSize-1);
	  }
	  HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET);
}
