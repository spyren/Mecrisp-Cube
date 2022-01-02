/**
 *  @brief
 *      Window watchdog.
 *
 *		There is a thread only for kicking (regularly feed) the watchdog.
 *		This thread has the second lowest priority (just higher than the
 *		idle thread), if any task does not want to give up control, the
 *		watchdog thread does not get any more CPU time and cannot feed
 *		the watchdog and the watchdog will bite after the timeout.
 *
 *		(+) WWDG clock (Hz) = PCLK1 / (4096 * Prescaler)
 *   	clock = 32'000'000 / (4096 * 64) = 122 Hz
 *   	(+) WWDG timeout (mS) = 1000 * (T[5;0] + 1) / WWDG clock (Hz)
 *   	timeout = 1000 * 64 / 122 = 524 ms
 *   	window  = 400 ms = 48
 *
 *   	RTC registers are used for accounting the watchdog bites.
 *
 *  @file
 *      watchdog.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2021-12-27
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
#include "wwdg.h"
#include "clock.h"
#include "watchdog.h"

// Private function prototypes
// ***************************
static void WATCHDOG_Thread(void *argument);

// Global Variables
// ****************

// Hardware resources
// ******************
extern WWDG_HandleTypeDef hwwdg;

// RTOS resources
// **************

// Definitions for WATCHDOG thread
static osThreadId_t WATCHDOG_ThreadId;
static const osThreadAttr_t WATCHDOG_ThreadAttr = {
		.name = "WATCHDOG_Thread",
		.priority = (osPriority_t) osPriorityIdle + 1, // just above IDLE
		.stack_size = 128 * 6
};


// Private Variables
// *****************
static int bitten = FALSE;


// Public Functions
// ****************

/**
 *  @brief
 *      Initialize the WATCHDOG.
 *  @return
 *      None
 */
void WATCHDOG_init(void) {
	if (RTC_Backup.watchdog == RTC_MAGIC_COOKIE) {
		bitten = TRUE;
		RTC_Backup.watchdog_bites += 1;
		RTC_Backup.watchdog = 0;
	} else {
		bitten = FALSE;
	}

}


/**
 *  @brief
 *      Activate the WATCHDOG.
 *  @return
 *      None
 */
void WATCHDOG_activate(void) {
	RTC_Backup.watchdog = 0;
	// creation of WATCHDOG_Thread
	WATCHDOG_ThreadId = osThreadNew(WATCHDOG_Thread, NULL, &WATCHDOG_ThreadAttr);
	if (WATCHDOG_ThreadId == NULL) {
		Error_Handler();
	}
}


/**
 *  @brief
 *      Has the WATCHDOG bitten?
 *
 *  @return
 *      true if the watchdog has bitten
 */
int WATCHDOG_bitten(void) {
	return bitten;
}


/**
 *  @brief
 *      How many times has the watchdog bitten since cold startup?
 *
 *  @return
 *      watchdog bites
 *
 */
int WATCHDOG_bites(void) {
	return RTC_Backup.watchdog_bites;
}

/**
 *  @brief
 *      Get address where the watchdog bit
 *
 *  @return
 *      Address
 *
 */
int WATCHDOG_adr(void) {
	return RTC_Backup.watchdog_adr;
}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the WATCHDOG thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void WATCHDOG_Thread(void *argument) {
	MX_WWDG_Init();
	// Infinite loop
	for(;;) {
		// wait 450 ms
		osDelay(450);
		HAL_WWDG_Refresh(&hwwdg); // feed the watchdog
	}
}


// Callbacks
// *********

/**
  * @brief  WWDG Early Wakeup callback.
  * @param  hwwdg  pointer to a WWDG_HandleTypeDef structure that contains
  *                the configuration information for the specified WWDG module.
  * @retval None
  */
void HAL_WWDG_EarlyWakeupCallback(WWDG_HandleTypeDef *hwwdg) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hwwdg);
	RTC_Backup.watchdog = RTC_MAGIC_COOKIE;
	Error_Handler();
}

