/**
 *  @brief
 *      USB CDC terminal console
 *
 *      Buffered serial communication.
 *  @file
 *      usb_cdc.c
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
#include "app_common.h"
#include "main.h"
#include "usb_cdc.h"
#include "myassert.h"
#include "tusb.h"


#define CDC_TX_SENT	0x01


// Private function prototypes
// ***************************
static void cdc_thread(void *argument);

// Global Variables
// ****************

// RTOS resources
// **************

// Definitions for CDC thread
osThreadId_t CDC_ThreadID;
const osThreadAttr_t cdc_thread_attributes = {
		.name = "USB_CDC",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 128*8
};

// Definitions for TxQueue
osMessageQueueId_t CDC_TxQueueId;
static const osMessageQueueAttr_t cdc_TxQueue_attributes = {
		.name = "CDC_TxQueue"
};

// Definitions for RxQueue
osMessageQueueId_t CDC_RxQueueId;
static const osMessageQueueAttr_t cdc_RxQueue_attributes = {
		.name = "CDC_RxQueue"
};

osEventFlagsId_t CDC_EvtFlagsID;


// Private Variables
// *****************
static uint8_t tx_buffer[64];

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the CDC.
 *  @return
 *      None
 */
void CDC_init(void) {
	// Create the queue(s)
	// creation of TxQueue
	CDC_TxQueueId = osMessageQueueNew(512, sizeof(uint8_t), &cdc_TxQueue_attributes);
	ASSERT_fatal(CDC_TxQueueId != NULL, ASSERT_QUEUE_CREATION, __get_PC());
	// creation of RxQueue
	CDC_RxQueueId = osMessageQueueNew(2048, sizeof(uint8_t), &cdc_RxQueue_attributes);
	ASSERT_fatal(CDC_RxQueueId != NULL, ASSERT_QUEUE_CREATION, __get_PC());

	// creation of CDC_Thread
	CDC_ThreadID = osThreadNew(cdc_thread, NULL, &cdc_thread_attributes);
	ASSERT_fatal(CDC_ThreadID != NULL, ASSERT_THREAD_CREATION, __get_PC());
}


/**
 *  @brief
 *		Reads a char from the CDC Rx (serial in). Blocking until char is
 *      ready.
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int CDC_getc(void) {
	uint8_t c;
	if (osMessageQueueGet(CDC_RxQueueId, &c, NULL, osWaitForever) == osOK) {
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
int CDC_RxReady(void) {
	if (osMessageQueueGetCount(CDC_RxQueueId) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/**
 *  @brief
 *      Writes a char to the USB CDC Tx (serial out). Blocking until char can be
 *      written into the queue.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int CDC_putc(int c) {
	if (osMessageQueuePut(CDC_TxQueueId, &c, 0, osWaitForever) == osOK) {
		return 0;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**
 *  @brief
 *      Tx queue ready for next char.
 *  @return
 *      FALSE if the buffer is full.
 */
int CDC_TxReady(void) {
	if (osMessageQueueGetSpace(CDC_TxQueueId) > 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}


/**
 *  @brief
 *      Writes a char direct into the key queue.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int CDC_putkey(const char c) {
	osStatus_t status;

	if (c == '\r') {
		// eat CR
		return 0;
	}
	status = osMessageQueuePut(CDC_RxQueueId, &c, 0, osWaitForever);
	if (status == osOK) {
		return 0;
	} else {
		Error_Handler();
		return EOF;
	}
}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the USB-CDC thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void cdc_thread(void *argument) {
	uint8_t c;
	int count, avail, i;

	// blocked till USB_CDC is connected
	osThreadFlagsWait(CDC_CONNECTED, osFlagsWaitAny, osWaitForever);
	osDelay(200);

	// Infinite loop
	for(;;) {
		// blocked till a character is in the Tx queue
		if (osMessageQueueGet(CDC_TxQueueId, &c, 0, osWaitForever) == osOK) {
			// send the characters
			tx_buffer[0] = c;
			count = osMessageQueueGetCount(CDC_TxQueueId) + 1;
			if (count > 64) {
				// CDC buffer is 64 bytes
				count = 64;
			}
			avail = tud_cdc_write_available();
			if (avail == 0) {
				// CDC buffer is full, wait till at least 1 element is available
				do {
					osDelay(2);
					avail = tud_cdc_write_available();
				} while (avail == 0);
			}
			if (avail < count) {
				// send only available elements
				count = avail;
			}
			for (i=1; i < count; i++) {
				osMessageQueueGet(CDC_TxQueueId, &c, 0, 0);
				tx_buffer[i] = c;
			}
			tud_cdc_write(tx_buffer, count);
	        tud_cdc_write_flush();
		} else {
			// can't write to the queue
			Error_Handler();
		}
	}
}


