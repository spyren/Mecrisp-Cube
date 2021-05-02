/**
 *  @brief
 *  	Cross-Plex LED driver based on the controller IS31FL3731.
 *
 *  	Resolution 15x7, monochrome.
 *  	The 7 vertical pixels are in one byte.
 *  	There are 8 independent frames.
 *  	I2C Interface, address 0x74.
 *  	See https://www.mikrocontroller.net/topic/54860 for the fonts.
 *  @file
 *      plex.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2021-04-30
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
#include "plex.h"
#include "iic.h"
#include "font6x8.h"
#include "font8x8.h"
#include "oled.h"

// Macros
// ******
#define  membersof(x) (sizeof(x) / sizeof(x[0]))

#define COMMAND_REG		0xFD
#define FRAME_REG		0x00
#define NIGHT_REG		0x0B

#define LED_REG			0x00
#define BLINK_REG		0x12
#define	PWM_REG			0x24

#define CONFIG_REG		0x00
#define PICTURE_REG		0x01
#define PLAY1_REG		0x02
#define PLAY2_REG		0x03
#define OPTION_REG		0x05
#define AUDIO_REG		0x06
#define STATE_REG		0x07
#define BREATH1_REG		0x08
#define BREATH2_REG		0x09
#define SHUTDOWN_REG	0x0A
#define AGC_REG			0x0B
#define ADC_REG			0x0C


// Private function prototypes
// ***************************
static void sendChar6x8(int ch);
static void sendChar8x8(int ch);


// Global Variables
// ****************
//extern const char **font_6x8;
//extern const char **font_8x8;

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;

// RTOS resources
// **************

// Private Variables
// *****************

static uint8_t plexReady = FALSE;

static uint8_t CurrentPosX = 0;
static uint8_t CurrentBrightness = 255;

static uint8_t CurrentFrame = 0;
static uint8_t CurrentDisplay = 0;

static PLEX_FontT CurrentFont = PLEX_FONT6X8;

static uint8_t frame_buffer[8][15];


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the PLEX controller.
 *  @return
 *      None
 */
void PLEX_init(void) {
	if (HAL_I2C_IsDeviceReady(&hi2c1, PLEX_I2C_ADR << 1, 5, 100) != HAL_OK) {
		// PLEX is not ready
		return;
	}
	plexReady = TRUE;

	PLEX_clear();
	PLEX_setFrame(0);
	PLEX_setDisplay(0);
	PLEX_setPos(0);
	PLEX_setFont(PLEX_FONT6X8);
}


/**
 *  @brief
 *      Writes a char to the PLEX. Blocking until char is written into the controller memory.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int PLEX_putc(int c) {
	if (!plexReady) {
		return EOF;
	}

	if (c == '\r') {
		// carriage return
		PLEX_setPos(0);
		return 0;
	}

	switch (CurrentFont) {
	case PLEX_FONT6X8:
		sendChar6x8(c);
		break;
	case PLEX_FONT8X8:
		sendChar8x8(c);
		break;
	}

	return 0;
}


/**
 *  @brief
 *      PLEX ready for next char.
 *  @return
 *      FALSE if the transfer is ongoing.
 */
int PLEX_Ready(void) {
	return (IIC_ready());
}


/**
 *  @brief
 *      Writes a line (string) to the PLEX. Blocking until
 *      string can be written.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      s  string to write. C style string.
 *  @return
 *      Return EOF on error, 0 on success.
 */
int PLEX_puts(const char *s) {
	int i=0;

	while (s[i] != 0) {
		if (PLEX_putc(s[i])) {
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
 *  @return
 *      none
 */
void PLEX_setPos(uint8_t x) {
	if (!plexReady) {
		return;
	}

	if (x >= 0 && x < PLEX_X_RESOLUTION) {
		// valid position
		CurrentPosX = x;
	}
}


/**
 *  @brief
 *  	Gets the current horizontal position
 *  @return
 *      X horizontal position, max. (128 / 6) - 1
 */
uint8_t PLEX_getPosX() {
	return CurrentPosX;
}


/**
 *  @brief
 *  	Sets the font
 *  @param[in]
 *      font
 *  @return
 *      none
 */
void PLEX_setFont(PLEX_FontT font) {
	CurrentFont = font;
}


/**
 *  @brief
 *  	Sets the font
 *  @param[in]
 *      font
 *  @return
 *      none
 */
void PLEX_setPWM(int PWM) {
	CurrentBrightness = PWM;
}


/**
 *  @brief
 *  	Clears the PLEX display
 *  @return
 *      none
 */
void PLEX_clear(void) {
	uint8_t buf[2];
	int8_t old_frame = CurrentFrame;
	uint32_t i, j;

	if (!plexReady) {
		return;
	}

	for (j=0; j<8; j++) {
		PLEX_setFrame(j);

		for (i=0; i<18; i++) {
			PLEX_setColumn(i, 0x00, -1);
		}

		for (i=0; i<18; i++) {
			// no blinking
			buf[0] = BLINK_REG+i;
			buf[1] = 0;
			IIC_putMessage(buf, 2);
		}

		for (i=0; i<144; i++) {
			// max. brightness
			buf[0] = PWM_REG+i;
			buf[1] = 0xff;
			IIC_putMessage(buf, 2);
		}
	}

	PLEX_setFrame(old_frame);
}


/**
 *  @brief
 *      Shutdown the driver
 *  @param[in]
 *  	status   0 shutdown, 1 normal operation
 *  @return
 *      None
 */
void PLEX_shutdown(uint8_t status) {
	uint8_t buf[2];
	uint8_t old_frame = CurrentFrame;

	if (!plexReady) {
		return;
	}

	PLEX_setFrame(NIGHT_REG);

	buf[0] = SHUTDOWN_REG;
	buf[1] = status;
	IIC_setDevice(PLEX_I2C_ADR);
	IIC_putMessage(buf, 2);

	PLEX_setFrame(old_frame);
}

/**
 *  @brief
 *      Set the active frame
 *  @param[in]
 *  	frame   0 .. 7 (datasheet frame 1 .. 8)
 *  @return
 *      None
 */
void PLEX_setFrame(uint8_t frame) {
	uint8_t buf[2];

	if (!plexReady) {
		return;
	}

	buf[0] = COMMAND_REG;
	buf[1] = frame;
	IIC_setDevice(PLEX_I2C_ADR);
	IIC_putMessage(buf, 2);
	CurrentFrame = frame;
}

/**
 *  @brief
 *      Get the active frame
 *  @return
 *      frame number 0 .. 7
 */
int PLEX_getFrame(void) {
	return CurrentFrame;
}

/**
 *  @brief
 *      Set the display frame
 *  @param[in]
 *  	frame   0 .. 7 (datasheet frame 1 .. 8)
 *  @return
 *      None
 */
void PLEX_setDisplay(uint8_t frame) {
	uint8_t buf[2];
	uint8_t old_frame = CurrentFrame;

	if (!plexReady) {
		return;
	}

	PLEX_setFrame(NIGHT_REG);

	buf[0] = PICTURE_REG;
	buf[1] = frame;
	IIC_setDevice(PLEX_I2C_ADR);
	IIC_putMessage(buf, 2);

	PLEX_setFrame(old_frame);
	CurrentDisplay = frame;
}

/**
 *  @brief
 *      Get the display frame
 *  @return
 *  	frame   0 .. 7 (datasheet frame 1 .. 8)
 */
int PLEX_getDisplay(void) {
	return CurrentDisplay;
}


/**
 *  @brief
 *      Write one column
 *  @param[in]
 *  	col   the display column (0 .. 14)
 *  @param[in]
 *  	leds  bit 0 is first row, bit 7 is last row
 *  @param[in]
 *  	brightness  the same brightness for all LEDs in the column, do not change if < 0
 *  @return
 *      None
 */
void PLEX_setColumn(uint8_t col, uint8_t leds, int brightness) {
	uint8_t buf[2];
	uint8_t realcol;
	uint8_t realleds;
	int i;

	if (!plexReady) {
		return;
	}

	frame_buffer[CurrentFrame][col] = leds;

	if (col < 8) {
		realcol = 2*col;
		realleds = __RBIT(leds) >> 24;
	} else {
		realcol = 15 - 2*(col - 8);
		realleds = leds;
	}
	buf[0] = LED_REG+realcol;
	buf[1] = realleds;
	IIC_setDevice(PLEX_I2C_ADR);
	IIC_putMessage(buf, 2);

	if (brightness >= 0) {
		for (i=0; i<8; i++) {
			buf[0] = PWM_REG + 8*realcol + i;
			buf[1] = brightness;
			IIC_putMessage(buf, 2);
		}
	}
}


/**
 *  @brief
 *      Read one column
 *
 *      It is possible to get the frames direct from IS31FL3731 but for this
 *      the device has to be shutdown.
 *  @param[in]
 *  	col   the display column (0 .. 14)
 *  @return
 *      column bit pattern
 */
int PLEX_getColumn(uint8_t col) {
	return frame_buffer[CurrentFrame][col];
}


/**
 *  @brief
 *      Write one pixel
 *  @param[in]
 *  	col  the frame column (0 .. 14)
 *  @param[in]
 *  	row  the frame row (0 .. 6)
 *  @param[in]
 *  	brightness  1 .. 255, 0 reset pixel, -1 set pixel and do not change brightness
 *  @return
 *      None
 */
void PLEX_setPixel(uint8_t col, uint8_t row, int brightness) {
	uint8_t buf[2];
	uint8_t realcol;
	uint8_t realrow;

	if (!plexReady) {
		return;
	}

	if (col < 8) {
		realcol = 2*col;
		realrow = 7 - row;

	} else {
		realcol = 15 - 2*(col - 8);
		realrow = row;
	}

	if (brightness != 0) {
		// set bit
		frame_buffer[CurrentFrame][col] = frame_buffer[CurrentFrame][col] | (1 << row);
	} else {
		// reset bit
		frame_buffer[CurrentFrame][col] = frame_buffer[CurrentFrame][col] & (~ (1 << row));
	}
	PLEX_setColumn(col, frame_buffer[CurrentFrame][col], -1);

	if (brightness > 0) {
		buf[0] = PWM_REG + 8*realcol + realrow;
		buf[1] = brightness;
		IIC_putMessage(buf, 2);
	}
}


/**
 *  @brief
 *      Read one pixel
 *
 *      It is possible to get the frames direct from IS31FL3731 but for this
 *      the device has to be shutdown.
 *  @param[in]
 *  	col  the frame column (0 .. 14)
 *  @param[in]
 *  	row  the frame row (0 .. 6)
 *  @return
 *      pixel bit
 */
int PLEX_getPixel(uint8_t col, uint8_t row) {
	if (frame_buffer[CurrentFrame][col] & (1 << row)) {
		return TRUE;
	} else {
		return FALSE;
	}
}


// Private Functions
// *****************

static void sendChar6x8(int ch) {
	uint8_t i;

	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= 6) {
			PLEX_setPos(CurrentPosX-6);
		}
		return ;
	}

	if (CurrentPosX > PLEX_X_RESOLUTION - 6) {
		// auto wrap
		PLEX_setPos(0);
	}

	for (i = 0; i < 6; i++) {
		PLEX_setColumn(CurrentPosX+i, FONT6X8_getColumn(ch, i), CurrentBrightness);
	}

	CurrentPosX += 6;
	if (CurrentPosX >= PLEX_X_RESOLUTION) {
		// auto wrap
		PLEX_setPos(0);
	}

}


static void sendChar8x8(int ch) {
	uint8_t i;

	if (ch == '\b') {
		// backspace
		if (CurrentPosX >= 8) {
			PLEX_setPos(CurrentPosX-8);
		}
		return;
	}

	if (CurrentPosX > PLEX_X_RESOLUTION - 8) {
		// auto wrap
		PLEX_setPos(0);
	}

	for (i = 0; i < 8; i++) {
		PLEX_setColumn(CurrentPosX+i, FONT8X8_getColumn(ch, i), CurrentBrightness);
	}

	CurrentPosX += 8;
	if (CurrentPosX >= PLEX_X_RESOLUTION) {
		// auto wrap
		PLEX_setPos(0);
	}
}


