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
#include "stm32_lpm.h"

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

static osMutexId_t DCC_MutexID;
static const osMutexAttr_t DCC_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

// Private Variables
// *****************
static DCC_LocoSlot_t loco_slots[DCC_MAX_LOCO_SLOTS];

static uint8_t  packet[DCC_MAX_PACKET_LENGTH];
static uint32_t	byte_count;
static uint32_t bit_count;
static uint32_t packet_length = 0;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the DCC
 *
 *  @return
 *      None
 */
void DCC_init(void) {
	DCC_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(DCC_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());

	DCC_MutexID = osMutexNew(&DCC_MutexAttr);
	ASSERT_fatal(DCC_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	// creation of DCC_Thread
	DCC_ThreadId = osThreadNew(DCC_Thread, NULL, &DCC_ThreadAttr);
	ASSERT_fatal(DCC_ThreadId != NULL, ASSERT_THREAD_CREATION, __get_PC());
}


/**
 *  @brief
 *      Start the DCC
 *
 *  @return
 *      None
 */
void DCC_start(void) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_DCC, UTIL_LPM_DISABLE);
	HAL_TIM_Base_Start_IT(&htim16);
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Stop the DCC
 *
 *  @return
 *      None
 */
void DCC_stop(void) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	HAL_TIM_Base_Stop_IT(&htim16);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_DCC, UTIL_LPM_ENABLE);
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Set the state of a DCC slot
 *
 *  @return
 *      None
 */
void DCC_setState(int slot, int state) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].state = state;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the state of a DCC slot
 *
 *  @return
 *      None
 */
int DCC_getState(int slot) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	return loco_slots[slot].state;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Set the address of a DCC slot
 *
 *  @return
 *      None
 */
void DCC_setAddress(int slot, int address) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].address = address;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the address of a DCC slot
 *
 *  @return
 *      None
 */
int DCC_getAddress(int slot) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	return loco_slots[slot].address;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Set the speed of a DCC slot
 *
 *  @return
 *      None
 */
void DCC_setSpeed(int slot, int speed) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].speed = speed;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the speed of a DCC slot
 *
 *  @return
 *      None
 */
int DCC_getSpeed(int slot) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	return loco_slots[slot].speed;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Set the direction of a DCC slot
 *
 *  @return
 *      None
 */
void DCC_setDirection(int slot, int direction) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].direction = direction;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the direction of a DCC slot
 *
 *  @return
 *      None
 */
int DCC_getDirection(int slot) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	return loco_slots[slot].direction;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Set the function of a DCC slot
 *
 *  @return
 *      None
 */
void DCC_setFunction(int slot, int function) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].function |= function;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Reset the function of a DCC slot
 *
 *  @return
 *      None
 */
void DCC_resetFunction(int slot, int function) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].function &= !function;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the functions of a DCC slot
 *
 *  @return
 *      None
 */
int DCC_getFunction(int slot) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	return loco_slots[slot].function;
	osMutexRelease(DCC_MutexID);
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
		// only one thread is allowed to use DCC
		osMutexAcquire(DCC_MutexID, osWaitForever);
		osMutexRelease(DCC_MutexID);

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
