/**
 *  @brief
 *      Digital Command Control Driver
 *
 *		Using TIM16, resolution 1 us, interrupt every 58 us.
 *		max. 2*116 us for one 0-bit -> min. 4 kBit/s
 *		129 bit in 30 ms, about min. 12 bytes, average 18 bytes
 *		Max. 4 active locomotive slots
 *  @file
 *      dcc.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2025-04-27
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

#include "dcc.h"

// Private function prototypes
// ***************************
static void DCC_Thread(void *argument);

// Global Variables
// ****************

// Hardware resources
// ******************
extern TIM_HandleTypeDef htim16;


// RTOS resources
// **************

// Definitions for DCC thread
static osThreadId_t DCC_ThreadId;
static const osThreadAttr_t DCC_ThreadAttr = {
		.name = "DCC",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 128 * 6
};

static osSemaphoreId_t DCC_SemaphoreID;

// Private Variables
// *****************
static DCC_LocoSlot_t loco_slots[DCC_MAX_LOCO_SLOTS];

static uint8_t  packet[DCC_MAX_PACKET_LENGTH];
static uint32_t	byte_count;
static uint32_t bit_count;
static uint32_t packet_length;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the UART.
 *  @return
 *      None
 */
void DCC_init(void) {
	DCC_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(DCC_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());

	// creation of DCC_Thread
	DCC_ThreadId = osThreadNew(DCC_Thread, NULL, &DCC_ThreadAttr);
	ASSERT_fatal(DCC_ThreadId != NULL, ASSERT_THREAD_CREATION, __get_PC());

	HAL_TIM_Base_Start_IT(&htim16);
}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the DCC thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void DCC_Thread(void *argument) {
	osStatus_t os_status;

	// Infinite loop
	for(;;) {
		// blocked till packet is sent
		os_status = osSemaphoreAcquire(DCC_SemaphoreID, 1000);
		if (os_status != osOK) {
			;
		}
		// create data for packet

	}
}


// Callbacks
// *********

/**
  * @brief
  * 	Period elapsed ISR for TIM16
  *
  * 	Ignore the shared TIM1 UP.
  * @retval
  * 	None
  */
void DCC_TIM16_PeriodElapsedIRQHandler() {
    __HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);

	HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, 0);
	HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, 0);
	if (bit_count) {
		HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, 1);
		bit_count = 0;
	} else {
		HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, 1);
		bit_count = 1;
	}
}
