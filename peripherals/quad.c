/**
 *  @brief
 *  	Quad Alphanumeric LED driver based on the controller HT16K33.
 *
 *  	14 + 1 segments, monochrome.
 *  	I2C Interface, address 0x74.
 *  	There is no mutex for the display.
 *  @file
 *      quad.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-12-13
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
#include "quad.h"
#include "iic.h"

#if QUAD == 1

// Macros
// ******

#define DATA_ADR		0x00
#define MAX_ADR			0x0f

#define SYS_SETUP		0x20
#define OSCILATOR_OFF	0x00
#define OSCILATOR_ON	0x01

#define DISPLAY_SETUP	0x80
#define DISPLAY_OFF		0x00
#define DISPLAY_ON		0x01
#define BLINKING_OFF	0x00
#define BLINKING_2HZ	0x02
#define BLINKING_1HZ    0x04
#define BLINKING_0HZ5   0x06

#define DIMMING			0xe0
#define MAX_BRIGHTNESS	0x0f

// Private function prototypes
// ***************************
static void update_display(void);

// Global Variables
// ****************

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;

// RTOS resources
// **************

// Private Variables
// *****************

static const uint16_t quad_font_table[] = {
    0b0000000000000001, // 00
	0b0000000000000010, // 01
	0b0000000000000100, // 02
    0b0000000000001000, // 03
	0b0000000000010000, // 04
	0b0000000000100000, // 05
    0b0000000001000000, // 06
	0b0000000010000000, // 07
	0b0000000100000000, // 08
    0b0000001000000000, // 09
	0b0000010000000000, // 0a
	0b0000100000000000, // 0b
    0b0001000000000000, // 0c
    0b0010000000000000, // 0d
	0b0100000000000000, // 0e
    0b0000000000000000, // 0f
	0b0000000000000000, // 10
	0b0000000000000000, // 11
    0b0000000000000000, // 12
	0b0000000000000000, // 13
	0b0000000000000000, // 14
    0b0000000000000000, // 15
	0b0000000000000000, // 16
	0b0000000000000000, // 17
    0b0001001011001001, // 18
	0b0001010111000000, // 19
	0b0001001011111001, // 1a
    0b0000000011100011, // 1b
	0b0000010100110000, // 1c
	0b0001001011001000, // 1d
    0b0011101000000000, // 1e
	0b0001011100000000, // 1f
    0b0000000000000000, //
    0b0000000000000110, // !
    0b0000001000100000, // "
    0b0001001011001110, // #
    0b0001001011101101, // $
    0b0000110000100100, // %
    0b0010001101011101, // &
    0b0000010000000000, // '
    0b0010010000000000, // (
    0b0000100100000000, // )
    0b0011111111000000, // *
    0b0001001011000000, // +
    0b0000100000000000, // ,
    0b0000000011000000, // -
    0b0100000000000000, // .
    0b0000110000000000, // /
    0b0000110000111111, // 0
    0b0000000000000110, // 1
    0b0000000011011011, // 2
    0b0000000010001111, // 3
    0b0000000011100110, // 4
    0b0010000001101001, // 5
    0b0000000011111101, // 6BLINKING_OFF
    0b0000000000000111, // 7
    0b0000000011111111, // 8
    0b0000000011101111, // 9
    0b0001001000000000, // :
    0b0000101000000000, // ;
    0b0010010000000000, // <
    0b0000000011001000, // =
    0b0000100100000000, // >
    0b0001000010000011, // ?
    0b0000001010111011, // @
    0b0000000011110111, // A
    0b0001001010001111, // B
    0b0000000000111001, // C
    0b0001001000001111, // D
    0b0000000011111001, // E
    0b0000000001110001, // F
    0b0000000010111101, // G
    0b0000000011110110, // H
    0b0001001000001001, // I
    0b0000000000011110, // J
    0b0010010001110000, // K
    0b0000000000111000, // L
    0b0000010100110110, // M
    0b0010000100110110, // N
    0b0000000000111111, // O
    0b0000000011110011, // P
    0b0010000000111111, // Q
    0b0010000011110011, // R
    0b0000000011101101, // S
    0b0001001000000001, // T
    0b0000000000111110, // U
    0b0000110000110000, // V
    0b0010100000110110, // W
    0b0010110100000000, // X
    0b0001010100000000, // Y
    0b0000110000001001, // Z
    0b0000000000111001, // [
    0b0010000100000000, //
    0b0000000000001111, // ]
    0b0000110000000011, // ^
    0b0000000000001000, // _
    0b0000000100000000, // `
    0b0001000001011000, // a
    0b0010000001111000, // b
    0b0000000011011000, // c
    0b0000100010001110, // d
    0b0000100001011000, // e
    0b0000000001110001, // f
    0b0000010010001110, // g
    0b0001000001110000, // h
    0b0001000000000000, // i
    0b0000000000001110, // j
    0b0011011000000000, // k
    0b0000000000110000, // l
    0b0001000011010100, // m
    0b0001000001010000, // n
    0b0000000011011100, // o
    0b0000000101110000, // p
    0b0000010010000110, // q
    0b0000000001010000, // r
    0b0010000010001000, // s
    0b0000000001111000, // t
    0b0000000000011100, // uBLINKING_OFF
    0b0010000000000100, // v
    0b0010100000010100, // w
    0b0010100011000000, // x
    0b0010000000001100, // y
    0b0000100001001000, // z
    0b0000100101001001, // {
    0b0001001000000000, // |
    0b0010010010001001, // }
    0b0000010100100000, // ~
    0b0011111111111111, // del
};

static uint8_t quadReady;
static uint8_t frame_buffer[8];
static uint8_t decimal_points;
static uint8_t CurrentPosX;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the QUAD controller.
 *  @return
 *      None
 */
void QUAD_init(void) {
	if (HAL_I2C_IsDeviceReady(&hi2c1, QUAD_I2C_ADR << 1, 5, 100) != HAL_OK) {
		// QUAD is not ready
		quadReady = FALSE;
		return;
	}
	quadReady = TRUE;

	QUAD_shutdown(1);
	QUAD_blink(BLINKING_OFF);
//	QUAD_blink(BLINKING_2HZ);
	QUAD_dimm(MAX_BRIGHTNESS);
//	QUAD_dimm(7);
	QUAD_clear();
//	QUAD_setDP(8);
	QUAD_puts("ELKE");
}


/**
 *  @brief
 *      Writes a char to the QUAD. Blocking until char is written into the controller memory.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int QUAD_putc(int c) {
	if (quadReady) {
		if (c == '\r') {
			// carriage return
			QUAD_setPosX(0);
		} else {
			frame_buffer[CurrentPosX*2]   = quad_font_table[c] & 0xff;
			frame_buffer[CurrentPosX*2+1] = quad_font_table[c] >> 8;

			if (++CurrentPosX >= 4) {
				CurrentPosX = 0;
			}
			update_display();
		}
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
int QUAD_puts(const char *s) {
int i=0;

	while (s[i] != 0) {
		if (QUAD_putc(s[i])) {
			return EOF;
		}
		i++;
	}
	return 0;
}

/**
 *  @brief
 *      QUAD ready for next char.
 *  @return
 *      FALSE if the transfer is ongoing.
 */
int QUAD_Ready(void) {
	return (IIC_ready());
}


/**
 *  @brief
 *  	Set the current horizontal position
 *  @param[in]
 *      x  horizontal position, max. 3	if (!quadReady) {
 *
 *  @return
 *      none
 */
void QUAD_setPosX(uint8_t x) {
	if (x >= 0 && x < QUAD_X_RESOLUTION) {
		// valid position
		CurrentPosX = x;
	}
}


/**farme_buffer
 *  @brief
 *  	Get the current horizontal position
 *  @return
 *      X horizontal position, max. 3
 */
uint8_t QUAD_getPosX() {
	return CurrentPosX;
}


/**
 *  @brief
 *  	Clears the QUAD display
 *  @return
 *      none
 */
void QUAD_clear(void) {
	memset(frame_buffer, 0, sizeof(frame_buffer));
	decimal_points = 0;
	update_display();
}


/**
 *  @brief
 *      Shutdown the driver
 *  @param[in]
 *  	status   0 shutdown, 1 normal operation
 *  @return
 *      None
 */
void QUAD_shutdown(uint8_t status) {
	uint8_t command = SYS_SETUP | OSCILATOR_OFF;

	if (status) {
		command = SYS_SETUP | OSCILATOR_ON;
	}
	if (quadReady) {
		IIC_putMessage(&command, 1, QUAD_I2C_ADR);
	}
}


/**
 *  @brief
 *      Let the Quad blink
 *  @param[in]
 *  	status
 *  @return
 *      None
 */
void QUAD_blink(uint8_t status) {
	uint8_t command = DISPLAY_SETUP | DISPLAY_ON;

	command |= status;
	if (quadReady) {
		IIC_putMessage(&command, 1, QUAD_I2C_ADR);
	}
}


/**
 *  @brief
 *      Set dimming
 *  @param[in]
 *  	status
 *  @return
 *      None
 */
void QUAD_dimm(uint8_t status) {
	uint8_t command = DIMMING ;

	command |= status;
	if (quadReady) {
		IIC_putMessage(&command, 1, QUAD_I2C_ADR);
	}
}


/**
 *  @brief
 *      Write one segment
 *  @param[in]
 *  	column (0 .. 3)
 *  @param[in]
 *  	segment (0 .. 14)
 *  @return
 *      None
 */
void QUAD_setSegment(uint8_t column, uint8_t segment) {
}


/**
 *  @brief
 *      Read one segment
 *
 *      It is possible to get the frames direct from IS31FL3731 but for this
 *      the device has to be shutdown.
 *  @param[in]
 *  	column (0 .. 3)
 *  @param[in]
 *  	segment (0 .. 14)
 *  @return
 *      segment bit
 */
int QUAD_getSegment(uint8_t col, uint8_t row) {
	return 0;
}


/**
 *  @brief
 *      Set decimal point
 *
 *  @param[in]
 *  	dp (2^0 .. 2^3) from left to right
 *  @return
 *      none
 */
void QUAD_setDP(uint8_t dp) {
	if (dp >=0 && dp <= 0b1111) {
		decimal_points = dp;
		update_display();
	}
}


// Private Functions
// *****************

static void update_display(void) {
	int i;
	uint8_t data[9];

	// decimal point
	for (i=0; i<4; i++) {
		if ((decimal_points >> i) & 0b0001) {
			frame_buffer[i*2+1] |= 0b01000000;
		}
	}

    // write display data
	data[0] = 0; // start address 0
	for (i=0; i<8; i++) {
		data[i+1] = frame_buffer[i];
	}
	IIC_putMessage(data, 9, QUAD_I2C_ADR);
}


#endif // QUAD
