/**
 *  @brief
 *  	For severe failures e.g. stack overflow the only way to recover for
 *  	embedded systems is a reset (warm restart, abort).
 *  	Other asserts (unknown events) can be ignored because they do not
 *  	have fatal consequences. But for debugging during the development
 *  	cycle, all asserts should be observed. Therefore Mecrisp-Cube has
 *  	two types of asserts: fatal asserts and non-fatal asserts. T
 *  	he non-fatal asserts can be ignored in production systems.
 *
 *   	RTC registers are used for accounting the asserts occurred.
 *
 *  @file
 *      assert.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2021-12-32
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
#include <stdio.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "clock.h"
#include "myassert.h"

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
static int assert_occurred = FALSE;

static const char* const assert_msg[] = {
		"", 								// 0
		"ASSERT_HARD_FAULT",				// 1
		"ASSERT_MEM_MANAGE_FAULT",			// 2
		"ASSERT_BUS_FAULT",					// 3
		"ASSERT_USAGE_FAULT",				// 4

		"ASSERT_CPU2_HARD_FAULT",			// 5

		"ASSERT_FORTH_UNEXPECTED_EXIT",		// 6
		"ASSERT_STACK_OVERFLOW",			// 7
		"ASSERT_MALLOC_FAILED",				// 8
		"ASSERT_MUTEX_CREATION",			// 9
		"ASSERT_SEMAPHORE_CREATION",		// 10
		"ASSERT_QUEUE_CREATION",			// 11
		"ASSERT_THREAD_CREATION",			// 12
		"ASSERT_EVENT_FLAGS_CREATION",		// 13

		"ASSERT_CDC_SIGINT",				// 14
		"ASSERT_UART_SIGINT",				// 15
		"ASSERT_UART_ERROR_CALLBACK",		// 16
		"ASSERT_UART_FIFO",					// 17

		"ASSERT_FLASH_UNLOCK",				// 18
		"ASSERT_FLASH_LOCK",				// 19

		"ASSERT_FREERTOS"					// 20
};


// Public Functions
// ****************

/**
 *  @brief
 *      Initialize the ASSERT.
 *
 *      Has to be called during the initialization phase, but not too early.
 *      Otherwise an assert loop could happen.
 *  @return
 *      None
 */
void ASSERT_init(void) {
	if (RTC_Backup.assert == RTC_MAGIC_COOKIE) {
		assert_occurred = TRUE;
		RTC_Backup.assert_cnt += 1;
		RTC_Backup.assert = 0;
	} else {
		assert_occurred = FALSE;
	}

}


/**
 *  @brief
 *      ASSERT occurred since cold startup?
 *
 *  @return
 *      true if an assert occurred
 */
int ASSERT_occurred(void) {
	return assert_occurred;
}


/**
 *  @brief
 *      How many times asserts occurred since cold startup?
 *
 *  @return
 *      assert bites
 *
 */
int ASSERT_getCount(void) {
	return RTC_Backup.assert_cnt;
}

/**
 *  @brief
 *      Get assert ID
 *
 *  @return
 *      assert ID
 *
 */
int ASSERT_getId(void) {
	return RTC_Backup.assert_id;
}

/**
 *  @brief
 *      Get assert param e.g. where the assert occured
 *
 *  @return
 *      assert param
 *
 */
int ASSERT_getParam(void) {
	return RTC_Backup.assert_param;
}

/**
 *  @brief
 *      Get assert message
 *
 *  @return
 *      assert param
 *
 */
char* ASSERT_getMsg(int index) {
	return  (char*) assert_msg[index];
}


// Private Functions
// *****************



// Callbacks
// *********
