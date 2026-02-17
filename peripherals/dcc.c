/**
 *  @brief
 *      Digital Command Control Driver
 *
 *		Using TIM16, resolution 1 us, interrupt every 58 us (see module tim.c).
 *		max. 2*116 us for one 0-bit -> min. 4 kBit/s
 *		129 bit in 30 ms, about min. 12 bytes, average 18 bytes
 *		Max. 4 active locomotive slots.
 *		Each slot has an assigned locomotive address.
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

#include "bsp.h"
#include "dcc.h"

#define TIME_0BIT_HALF_PERIOD	(100-1)		// 0 bit is 100 us
#define TIME_1BIT_HALF_PERIOD	(58-1)		// 1 bit is 58 us

// Types
// *****

typedef struct accessory_t {
	int 	adr;
	int 	D;
	int		R;
} accessory_t;

// Private function prototypes
// ***************************
static void DCC_Thread(void *argument);
static void prepare_packet(int delay);

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
static uint32_t	half_bit = FALSE;
static uint32_t bit_count;
static uint32_t byte_count = 0;
static uint32_t packet_len = 0;
static int len;

static 	accessory_t accessory;


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

	loco_slots[0].direction = 0x80;
	loco_slots[1].direction = 0x80;
	loco_slots[2].direction = 0x80;
	loco_slots[3].direction = 0x80;
}


/**
 *  @brief
 *      Start the DCC
 *
 *  @return
 *      None
 */
void DCC_start(void) {
	BSP_setDigitalPinMode(0, 3);  // D0 pushpull	int len;

	BSP_setDigitalPinMode(1, 3);  // D1 pushpull
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
	BSP_setDigitalPin(0, 0);
	BSP_setDigitalPin(1, 0);

}


/**
 *  @brief
 *      Set the state of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *	@param[in]
 *		state 0 deactivated, 1 activated
 *  @return
 *      None
 */
void DCC_setState(int slot, int state) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].state = state;
	if ((packet_len == 0) & loco_slots[slot].state) {
		osSemaphoreRelease(DCC_SemaphoreID);
	}
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the state of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *  @return	int len;
 *
 *      state
 */
int DCC_getState(int slot) {
	return loco_slots[slot].state;
}


/**
 *  @brief
 *      Assign an address for a slot
 *
 *	@param[in]
 *	 	slot  0..3
 *	@param[in]
 *		address
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
 *	@param[in]	int len;
 *
 *	 	slot  0..3
 *  @return
 *      address
 */
int DCC_getAddress(int slot) {
	return loco_slots[slot].address;
}


/**
 *  @brief
 *      Set the speed of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *	@param[in]
 *		speed 0 stop, 126 max. speed
 *  @return
 *      None
 */
void DCC_setSpeed(int slot, int speed) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].speed = speed & 0x7F;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the speed of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *  @return
 *      speed	int len;
 *
 */
int DCC_getSpeed(int slot) {
	return loco_slots[slot].speed;
}


/**
 *  @brief
 *      Set the direction of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *	@param[in]
 *		0 reverse
 *  @return
 *      None
 */
void DCC_setDirection(int slot, int direction) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	if (direction) {
		loco_slots[slot].direction = 0x80;
	} else {
		loco_slots[slot].direction = 0x00;
	}
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the direction of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
  *  @return
 *      direction
 */
int DCC_getDirection(int slot) {
	return loco_slots[slot].direction;
}


/**
 *  @brief
 *      Set the function of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *	@param[in]
 *		function F0 to F23, F0 is 2^0, F1 is 2^1 ..
 *  @return
 *      None
 */
void DCC_setFunction(int slot, int function) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].function_new |= function;
	loco_slots[slot].function_repetition = DCC_FUNCTION_REPETITION;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Reset the function of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *	@param[in]
 *		function F0 to F23, F0 is 2^0, F1 is 2^1 ..
 *  @return
 *      None
 */
void DCC_resetFunction(int slot, int function) {
	// only one thread is allowed to use DCC
	osMutexAcquire(DCC_MutexID, osWaitForever);
	loco_slots[slot].function_new &= ~function;
	loco_slots[slot].function_repetition = DCC_FUNCTION_REPETITION;
	osMutexRelease(DCC_MutexID);
}


/**
 *  @brief
 *      Get the functions of a DCC slot
 *
 *	@param[in]
 *	 	slot  0..3
 *  @return
 *      function bits
 */
int DCC_getFunction(int slot) {
	return loco_slots[slot].function;
}


/**
 *  @brief
 *      Control basic accessory.
 *
 *		LinearAddress = (DecoderAddress - 1) × 4 + Subaddress + 1
 *		DecoderAddress=10, Subaddress=0: (10-1)*4 + 0 + 1 = 37
 *	@param[in]
 *	 	address  1..2048 (Linear Address)
 *	@param[in]
 *	 	activate_D  0 deactivate, <>0 activate
 *	@param[in]
 *	 	direction_R  0 diverging, <>0 normal
 *  @return
 *      function bits
 */
void DCC_controlAccessory(int address, int activate_D, int direction_R) {
	osMutexAcquire(DCC_MutexID, osWaitForever);
	if (address <= 2044) {
		accessory.adr = address+3;
	} else {
		accessory.adr = address-2045;
	}
	accessory.D = activate_D;
	accessory.R = direction_R;
	osMutexRelease(DCC_MutexID);
}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the DCC thread
  *
  * 	Data for the packets are prepared here.
  * 	Synchronized with DCC_TIM16_PeriodElapsedIRQHandler by DCC_SemaphoreID.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void DCC_Thread(void *argument) {
	int i;
	int tmp;

	// Infinite loop
	for(;;) {
		// create data for packet
		for (i=0; i<DCC_MAX_LOCO_SLOTS; i++) {
			if (! loco_slots[i].state) {
				// ignore not active slots
				continue;
			}

			// blocked till packet is sent
			osSemaphoreAcquire(DCC_SemaphoreID, 1000);

			if (loco_slots[i].state) {
				// only one thread is allowed to use DCC
				osMutexAcquire(DCC_MutexID, osWaitForever);

				// slot is active
				len = 0;
				if (loco_slots[i].address <= 127) {
					packet[len++] = loco_slots[i].address;
				} else {
					packet[len++] = DCC_COMMAND_ADDRESS | (loco_slots[i].address >> 8);
					packet[len++] = loco_slots[i].address & 0xFF;
				}
				// speed and direction
				packet[len++] = DCC_COMMAND_SPEED_128;
				packet[len++] = loco_slots[i].speed | loco_slots[i].direction;

				prepare_packet(3);

				if (loco_slots[i].function_repetition) {
					loco_slots[i].function_repetition--;

					// blocked till packet is sent
					osSemaphoreAcquire(DCC_SemaphoreID, 1000);
					// only one thread is allowed to use DCC
					osMutexAcquire(DCC_MutexID, osWaitForever);

					len = 0;
					if (loco_slots[i].address <= 127) {
						packet[len++] = loco_slots[i].address;
					} else {
						packet[len++] = DCC_COMMAND_ADDRESS | (loco_slots[i].address >> 8);
						packet[len++] = loco_slots[i].address & 0xFF;
					}
					if ((loco_slots[i].function_new & 0x0000001f) != (loco_slots[i].function & 0x0000001f)) {
						// function F0 to F4
						packet[len++] = DCC_COMMAND_F0_F4 |
								(loco_slots[i].function_new & 0x00000001) << 4 |
								(loco_slots[i].function_new & 0x0000001e) >> 1;
					}
					if ((loco_slots[i].function_new & 0x000001e0) != (loco_slots[i].function & 0x000001e0)) {
						// function F5 to F8
						packet[len++] = DCC_COMMAND_F5_F8 |
								(loco_slots[i].function_new & 0x000001e0) >> 5;
					}
					if ((loco_slots[i].function_new & 0x00001e00) != (loco_slots[i].function & 0x00001e00)) {
						// function F9 to F12
						packet[len++] = DCC_COMMAND_F9_F12 |
								(loco_slots[i].function_new & 0x00001e00) >> 9 ;
					}
					if ((loco_slots[i].function_new & 0x001fe000) != (loco_slots[i].function & 0x001fe000)) {
						// function F13 to F20
						packet[len++] = DCC_COMMAND_F13_F20 |
								(loco_slots[i].function_new & 0x001fe000) >> 17;
					}
					if ((loco_slots[i].function_new & 0x1fe00000) != (loco_slots[i].function & 0x1fe00000)) {
						// function F21 to F28
						packet[len++] = DCC_COMMAND_F21_F28 |
								(loco_slots[i].function_new & 0x1fe00000) >> 21;
					}

				    if (! loco_slots[i].function_repetition) {
				    	loco_slots[i].function = loco_slots[i].function_new;
				    }

				    prepare_packet(3);
				}

			}
		}
		// accessory
		if (accessory.adr > 0) {
			// blocked till packet is sent
			osSemaphoreAcquire(DCC_SemaphoreID, 1000);
			// only one thread is allowed to use DCC
			osMutexAcquire(DCC_MutexID, osWaitForever);
			len = 0;
			tmp = 0x80;
			tmp |= ((accessory.adr >> 2) & 0x3F);
			packet[len++] = tmp;
			tmp = 0x80;
			tmp |= ((  accessory.adr  & 0x003) << 1);
			tmp |= ((~(accessory.adr) & 0x700) >> 4);
			tmp |= accessory.D?0x08:0x00;
			tmp |= accessory.R?0x01:0x00;
			packet[len++] = tmp;
			accessory.adr = -1;
			prepare_packet(3);
		}

	}
}


static void prepare_packet(int delay) {
	int j;

	// calculate checksum
	packet[len] = 0;
	for (j=0; j<len; j++) {
		packet[len] ^= packet[j];
	}

	osMutexRelease(DCC_MutexID);

	// min. time to the next packet 18*2*58 us = 2 ms
	osDelay(delay);
	HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
	packet_len = len+1;
	byte_count = 0;
	bit_count  = 9;
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
}


// Callbacks
// *********

/**
  * @brief
  * 	Period elapsed ISR for TIM16
  *
  *	 	The waveform for the packet is created here.
  *	 	The bit pattern is in the packet array.
  *	 	byte_count, packet_len, bit_count are global variables.
  *	 	Startbit is 0, highest bit first.
  * 	Ignore the shared TIM1 UP.
  * @retval
  * 	None
  */
void DCC_TIM16_PeriodElapsedIRQHandler() {
    __HAL_TIM_CLEAR_IT(&htim16, TIM_IT_UPDATE);

    if (half_bit) {
    	half_bit = FALSE;
    	HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, 0);
    	HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, 1);
    	return;
    } else {
    	half_bit = TRUE;
    	HAL_GPIO_WritePin(D0_GPIO_Port, D0_Pin, 0);
    	HAL_GPIO_WritePin(D1_GPIO_Port, D1_Pin, 1);
    }

    if (byte_count < packet_len) {
		// packet not finished
		if (bit_count-- == 9) {
			// start bit always 0
			__HAL_TIM_SET_AUTORELOAD(&htim16, TIME_0BIT_HALF_PERIOD);
		} else {
			// get bit
			if ((packet[byte_count] >> bit_count) & 0x01) {
				// 1
				__HAL_TIM_SET_AUTORELOAD(&htim16, TIME_1BIT_HALF_PERIOD);
			} else {
				// 0
				__HAL_TIM_SET_AUTORELOAD(&htim16, TIME_0BIT_HALF_PERIOD);
			}
		}
		if (bit_count == 0) {
			// byte finished
			bit_count = 9;
			byte_count++;
			if (byte_count >= packet_len) {
				// last byte -> ready for next packet
				osSemaphoreRelease(DCC_SemaphoreID);
			}
		}
	} else {
		// packet finished -> write only 1s (preamble)
		__HAL_TIM_SET_AUTORELOAD(&htim16, TIME_1BIT_HALF_PERIOD);
	}

}
