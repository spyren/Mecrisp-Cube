/**
 *  @brief
 *      Secure Digital Memory Card block read and write.
 *
 *      SPI is used as interface.
 *  @file
 *      sd.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-06-03
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
#include "sd.h"



// Private function prototypes
// ***************************

// Global Variables
// ****************

// RTOS resources
// **************

static osMutexId_t SD_MutexID;
static const osMutexAttr_t SD_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t SD_SemaphoreID;


// Private Variables
// *****************


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the Secure Digital Memory Card.
 *  @return
 *      None
 */
void SD_init(void) {
	SD_MutexID = osMutexNew(&SD_MutexAttr);
	if (SD_MutexID == NULL) {
		Error_Handler();
	}

	SD_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (SD_SemaphoreID == NULL) {
		Error_Handler();
	}
}


// Private Functions
// *****************


// Callbacks
// *********

