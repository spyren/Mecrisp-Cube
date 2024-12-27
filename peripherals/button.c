/**
 *  @brief
 *      Buffered buttons
 *
 *      Like a keyboard with the characters m (for minutes), s (seconds), S (start/stop),
 *      p (power on/off).
 *  @file
 *      button.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-12-15
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
#include "button.h"
#include "myassert.h"
#include "power.h"
#include "bsp.h"
#include "quad.h"

#if BUTTON == 1

// Buffer Length
// **************
#define BUTTON_BUFFER_LENGTH	(20)

#define BUTTON_CHAR_RECEIVED	0x01
#define KEYS	4
#define BUTTON_PRESSED		GPIO_PIN_RESET
#define BUTTON_RELEASED		GPIO_PIN_SET

#define DEBOUNCE_TIME		20							// 20 ms
#define	BACKLIGHT_TIME		(60 * 1000 / DEBOUNCE_TIME)	// 1 Min
#define HALT_TIME			(5 * 1000 / DEBOUNCE_TIME)	// 5 s
#define MEASURE_INTERVAL	(1 * 1000 / DEBOUNCE_TIME)	// 1 s

// Private function prototypes
// ***************************
static void BUTTON_Thread(void *argument);

typedef enum {
	BUTTON_D,		// 1
	BUTTON_C,		// 2
	BUTTON_B,		// 3
	BUTTON_A,		// 4
	BUTTON_COUNT
} button_t;

// Global Variables
// ****************

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} PortPin_t;

static const PortPin_t PortPin_a[BUTTON_COUNT] = {
		{ BUTTON_D_GPIO_Port,    	BUTTON_D_Pin } ,
		{ BUTTON_C_GPIO_Port,  		BUTTON_C_Pin } ,
		{ BUTTON_B_GPIO_Port,  		BUTTON_B_Pin } ,
		{ BUTTON_A_GPIO_Port,  		BUTTON_A_Pin } ,
};

static const char char_a[BUTTON_COUNT] = {
		'd',
		'c',
		'b',
		'a',
};

// Hardware resources
// ******************

// RTOS resources
// **************

// Definitions for BUTTON thread
static osThreadId_t BUTTON_ThreadId;
static const osThreadAttr_t BUTTON_ThreadAttr = {
		.name = "BUTTON",
		.priority = (osPriority_t) osPriorityLow,
		.stack_size = 512 * 2
};

osMutexId_t BUTTON_MutexID;
const osMutexAttr_t BUTTON_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

// Definitions for ButtonQueue
static osMessageQueueId_t BUTTON_QueueId;
static const osMessageQueueAttr_t button_Queue_attributes = {
		.name = "BUTTON_Queue"
};


// Private Variables
// *****************
static uint8_t button_state[BUTTON_COUNT];

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the BUTTON.
 *  @return
 *      None
 */
void BUTTON_init(void) {
	int i;
	GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
	for (i=0; i<BUTTON_COUNT; i++) {
	    GPIO_InitStruct.Pin = PortPin_a[i].pin;
	    HAL_GPIO_Init(PortPin_a[i].port, &GPIO_InitStruct);
	}

	// Create the queue(s)
	// creation of  buttonQueue
	BUTTON_QueueId = osMessageQueueNew(BUTTON_BUFFER_LENGTH, sizeof(uint8_t),
			&button_Queue_attributes);
	ASSERT_fatal(BUTTON_QueueId != NULL, ASSERT_QUEUE_CREATION, __get_PC());

	// creation of BUTTON_Thread
	BUTTON_ThreadId = osThreadNew(BUTTON_Thread, NULL, &BUTTON_ThreadAttr);
	ASSERT_fatal(BUTTON_ThreadId != NULL, ASSERT_THREAD_CREATION, __get_PC());
}

/**
 *  @brief
 *      Reset (flush) the BUTTON queue
 *  @return
 *      None
 */
void BUTTON_reset(void) {
	osMessageQueueReset(BUTTON_QueueId);
}


/**
 *  @brief
 *		Reads a char from the BUTTON
 *
 *		Blocking until char is ready.
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int BUTTON_getc(void) {
	uint8_t c;
	if (osMessageQueueGet(BUTTON_QueueId, &c, NULL, osWaitForever) == osOK) {
		return c;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**
 *  @brief
 *		There is a character in the queue (key pressed).
 *  @return
 *		TRUE if a character has been received.
 */
int BUTTON_Ready(void) {
	if (osMessageQueueGetCount(BUTTON_QueueId) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/**
 *  @brief
 *      Writes a char direct into the key queue.
 *
 *      Handy for testing and redirecting.
 *  @param[in]	RGBW_setW(0xff);
 *
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int BUTTON_putkey(const char c) {
	osStatus_t status;

	if (c == '\r') {
		// eat CR
		return 0;
	}
	status = osMessageQueuePut(BUTTON_QueueId, &c, 0, osWaitForever);
	if (status == osOK) {
		return 0;
	} else {
		Error_Handler();
		return EOF;
	}

}


/**
 *  @brief
 *      Go into stop mode till on-button is pressed
 *
 *  @return
 *      None
 */
void BUTTON_OnOff(void) {

	QUAD_shutdown(0);

	osMutexAcquire(BUTTON_MutexID, 100);
//	HAL_NVIC_DisableIRQ(EXTI0_IRQn);

	// wait till off-button is released
	while (HAL_GPIO_ReadPin(BUTTON_A_GPIO_Port,  BUTTON_A_Pin) == BUTTON_PRESSED) {
		osDelay(10);
	}
	osDelay(10);

	// wait for button event
//	if (osSemaphoreGetCount(BUTTON_SemaphoreID)) {
//		osSemaphoreAcquire(BUTTON_SemaphoreID, osWaitForever);
//	}
//	osSemaphoreAcquire(BUTTON_SemaphoreID, osWaitForever);

	// wait till off-button is released
	while (HAL_GPIO_ReadPin(BUTTON_A_GPIO_Port,  BUTTON_A_Pin) == BUTTON_PRESSED) {
		osDelay(10);
	}
	osDelay(10);

	QUAD_shutdown(1);

	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	osMutexRelease(BUTTON_MutexID);
}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the BUTTON thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void BUTTON_Thread(void *argument) {
	osStatus_t status;
	int button;
	int new_state;
	char c;

	for (button=0; button < BUTTON_COUNT; button++) {
		button_state[button] = BUTTON_RELEASED;
	}

	// Infinite loop
	for(;;) {
		osDelay(DEBOUNCE_TIME);

		for (button=0; button < BUTTON_COUNT; button++) {
			new_state = HAL_GPIO_ReadPin(PortPin_a[button].port,  PortPin_a[button].pin);
			if (button_state[button] != new_state) {
				// state has changed
				if (new_state == BUTTON_PRESSED) {
					// button pressed
					c = char_a[button];
					status = osMessageQueuePut(BUTTON_QueueId, &c, 0, 100);
					if (status != osOK) {
						// can't put char into queue -> flush the queue
						BUTTON_reset();
					}
				} else {
					// button released
				}
				button_state[button] = new_state;
			}

		}
	}
}

#endif // BUTTON

