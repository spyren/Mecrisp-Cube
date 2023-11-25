/**
 *  @brief
 *  	RGB and White LED driver based on the controller LP5562.
 *
 *  	8-Bit Current Setting (From 0 mA to 25.5 mA With 100 μA Steps)
 *  	8-Bit PWM Control
 *  	I2C Interface, 7 bit address 0x30.
 *  	400kHz I2C-compatible interface
 *  @file
 *      rgbw.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2023-11-19
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
#include "rgbw.h"
#include "iic.h"

// Macros
// ******

#define LED_CURRENT_RED 50		// 5 mA
#define LED_CURRENT_GREEN 50	// 5 mA
#define LED_CURRENT_BLUE 50		// 5 mA
#define LED_CURRENT_WHITE 150	// 15 mA

#define RGBW_REG_ENABLE 	0x00 	// LOG_EN CHIP_EN[7] ENG1_EXEC[4:5] ENG2_EXEC[3:2] ENG3_EXEC[1:0]
#define RGBW_REG_OP_MODE 	0x01 	// ENG1_MODE[4:5] ENG2_MODE[3:2] ENG3_MODE[1:0]
#define RGBW_REG_B_PWM 		0x02 	// B_PWM[7:0]
#define RGBW_REG_G_PWM 		0x03 	// G_PWM[7:0]
#define RGBW_REG_R_PWM 		0x04 	// R_PWM[7:0]
#define RGBW_REG_B_CURRENT 	0x05 	// B_CURRENT[7:0]
#define RGBW_REG_G_CURRENT 	0x06 	// G_CURRENT[7:0]
#define RGBW_REG_R_CURRENT 	0x07 	// R_CURRENT[7:0]
#define RGBW_REG_CONFIG 	0x08 	// PWM_HF[6] PS_EN[5] CLK_DET_EN[1] INT_CLK_E N[0]
#define RGBW_REG_ENG1 		0x09 	// PC ENG1_PC[3:0]
#define RGBW_REG_ENG2 		0x0A 	// PC ENG2_PC[3:0]
#define RGBW_REG_ENG3 		0x0B 	// PC ENG3_PC[3:0]
#define RGBW_REG_STATUS 	0x0C 	// EXT_CLK_US[3] ED ENG1_INT[2] ENG2_INT[1] ENG3_INT[0]
#define RGBW_REG_RESET 		0x0D 	// RESET[7:0]
#define RGBW_REG_W_PWM 		0x0E 	// W_PWM[7:0]
#define RGBW_REG_W_CURRENT 	0x0F 	// W_CURRENT[7:0]

#define RGBW_CHIP_ENABLE	0b01000000	// chip enable
#define RGBW_CHIP_DISABLE	0b00000000	// chip disable

// Private function prototypes
// ***************************


// Global Variables
// ****************

// Hardware resources
// ******************

// RTOS resources
// **************

// Private Variables
// *****************

static uint8_t rgbwReady = TRUE;

static uint32_t CurrentRGB = 0;
static uint8_t CurrentW = 0;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the RGBW controller.
 *  @return
 *      None
 */
void RGBW_init(void) {
	RGBW_setRegister(RGBW_REG_ENABLE, RGBW_CHIP_ENABLE); // chip enable
	osDelay(2);
	if (RGBW_getRegister(RGBW_REG_ENABLE) != RGBW_CHIP_ENABLE) {
		// RGBW is not ready
		rgbwReady = FALSE;
		return;
	}
	rgbwReady = TRUE;
	// set current
	RGBW_setRegister(RGBW_REG_R_CURRENT, LED_CURRENT_RED); 		// red LED current
	RGBW_setRegister(RGBW_REG_G_CURRENT, LED_CURRENT_GREEN); 	// green LED current
	RGBW_setRegister(RGBW_REG_B_CURRENT, LED_CURRENT_BLUE); 	// blue LED current
	RGBW_setRegister(RGBW_REG_W_CURRENT, LED_CURRENT_WHITE); 	// white LED current
	RGBW_setRGB(0);
	RGBW_setW(0);
}


/**
 *  @brief
 *      Switch off RGBW
 *  @return
 *      None
 */
void RGBW_switchOff(void) {
	RGBW_setRegister(RGBW_REG_ENABLE, RGBW_CHIP_DISABLE); // chip disable
}


/**
 *  @brief
 *  	Set the RGB LED brightness
 *  @param[in]
 *      rgb    Lowest (1st) byte blue, 2nd byte green, 3th byte red
 *  @return
 *      none
 */
void RGBW_setRGB(uint32_t rgb) {
	CurrentRGB = rgb;
	RGBW_setRegister(RGBW_REG_R_PWM, rgb >> 16); 			// red LED PWM
	RGBW_setRegister(RGBW_REG_G_PWM, (rgb >> 8) & 0xFF);	// green LED PWM
	RGBW_setRegister(RGBW_REG_B_PWM, rgb & 0xFF); 			// blue LED PWM
}


/**
 *  @brief
 *  	Get the RGB LED brightness
 *  @return
 *      rgb
 */
uint32_t RGBW_getRGB(void) {
	return CurrentRGB;
}


/**
 *  @brief
 *  	Set the white LED brightness
 *  @param[in]
 *      w    white brightness
 *  @return
 *      none
 */
void RGBW_setW(uint8_t w) {
	CurrentW = w;
	RGBW_setRegister(RGBW_REG_W_PWM, w); 			// white LED PWM
}


/**
 *  @brief
 *  	Get the white LED brightness
 *  @return
 *      white brightness
 */
uint8_t RGBW_getW(void) {
	return CurrentW;
}


/**
 *  @brief
 *      Set register
 *  @param[in]
 *  	reg   register
 *  @param[in]
 *  	data
 *  @return
 *      None
 */
void RGBW_setRegister(uint8_t reg, uint8_t data) {
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = data;
	IIC_putMessage(buf, 2, RGBW_I2C_ADR);
}


/**
 *  @brief
 *      Get register
 *  @param[in]
 *  	reg   register
 *  @return
 *      register data
 */
uint8_t RGBW_getRegister(uint8_t reg) {
	uint8_t buf[2];

	buf[0] = reg;
	IIC_putGetMessage(buf, 1, 1, RGBW_I2C_ADR);
	return buf[0];
}


// Private Functions
// *****************

