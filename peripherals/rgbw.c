/**
 *  @brief
 *  	RGB and White LED driver based on the controller LP5562.
 *
 *  	8-Bit Current Setting (From 0 mA to 25.5 mA With 100 μA Steps)
 *  	8-Bit PWM Control
 *  	I2C Interface, address 0x30.
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


// Private function prototypes
// ***************************


// Global Variables
// ****************

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;

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
	if (HAL_I2C_IsDeviceReady(&hi2c1, RGBW_I2C_ADR << 1, 5, 100) != HAL_OK) {
		// RGBW is not ready
		rgbwReady = FALSE;
		return;
	}
	rgbwReady = TRUE;
}




/**
 *  @brief
 *      RGBW ready for next char.
 *  @return
 *      FALSE if the transfer is ongoing.
 */
int RGBW_Ready(void) {
	return (IIC_ready());
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
 *  	Set the RGB LED brightness
 *  @param[in]
 *      w    white brightness
 *  @return
 *      none
 */
void RGBW_setW(uint8_t w) {
	CurrentW = w;
}


/**
 *  @brief
 *  	Get the RGB LED brightness
 *  @return
 *      white brightness
 */
uint8_t RGBW_getW(void) {
	return CurrentW;
}


/**
 *  @brief
 *      Shutdown the driver
 *  @param[in]
 *  	status   0 shutdown, 1 normal operation
 *  @return
 *      None
 */
void RGBW_shutdown(uint8_t status) {
	uint8_t buf[2];

	buf[0] = 0;
	buf[1] = status;
	IIC_putMessage(buf, 2, RGBW_I2C_ADR);
}


// Private Functions
// *****************

