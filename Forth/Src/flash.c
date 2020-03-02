/**
 *  @brief
 *  @file
 *      flash.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-02-19
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
#include "main.h"
#include "flash.h"


// Private function prototypes
// ***************************
static void flash_thread(void *argument);

// Global Variables
// ****************

// Definitions for FLASH thread
osThreadId_t FLASH_ThreadID;
const osThreadAttr_t flash_thread_attributes = {
		.name = "FLASH_Thread",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 128
};

// Private Variables
// *****************

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the FLASH.
 *  @return
 *      None
 */
void FLASH_init(void) {
	// creation of FLASH_Thread
	FLASH_ThreadID = osThreadNew(flash_thread, NULL, &flash_thread_attributes);

}



// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the FLASH thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void flash_thread(void *argument) {
	// Infinite loop
	for(;;) {
		osDelay(1);
	}
}

