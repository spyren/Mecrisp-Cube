/**
 *  @brief
 *      Secure Digital Memory Card block read and write.
 *
 *      SPI is used as interface. No chip select.
 *  @file
 *      dotstar.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2022-12-04
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

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "rt_spi.h"
#include "dotstar.h"

#if DOTSTAR == 1

// Defines
// *******

// Private typedefs
// ****************


// Private function prototypes
// ***************************

// Global Variables
// ****************

// RTOS resources
// **************


// Hardware resources
// ******************


// Private Variables
// *****************
uint32_t pixels[72 + 2];	// max. 72 DotStar pixels

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the dotstar.
 *  @return
 *      None
 */
void DOTSTAR_init(void) {
}


/**
 *  @brief
 *	    Set the DotStar RGB LEDs.
 *
 *	@param[in]
 *      buffer    array of pixels
 *	@param[in]
 *      len       array length
 *  @return
 *      none
 *
 */
void DOTSTAR_setPixels(uint32_t *buffer, uint32_t len) {

	// only one thread is allowed to use the SPI
	osMutexAcquire(RTSPI_MutexID, osWaitForever);

	memcpy(pixels+1, buffer, 4*len);
	pixels[0] = 0x0;
	pixels[len+1] = 0xffffffff;

	/* Write the block data to SD */
	RTSPI_WriteData((uint8_t*)pixels, len*4+8);

	osMutexRelease(RTSPI_MutexID);
}


/**
 *  @brief
 *	    Set the dotstar RGB LED.
 *
 *	@param[in]
 *      rgb    Lowest (1st) byte blue, 2nd byte green, 3th byte red, highest byte intensity 0..31
 *  @return
 *      none
 *
 */
void DOTSTAR_setPixel(uint32_t irgb) {
	uint8_t buffer[4];

	buffer[0] = (irgb >> 24) | 0xd0;	// brightness
	buffer[1] = irgb & 0xFF;			// blue
	buffer[2] = (irgb >> 8) & 0xFF;		// green
	buffer[3] = (irgb >> 16) & 0xFF;	// red
	DOTSTAR_setPixels((uint32_t*)buffer, 1);
}

#endif
