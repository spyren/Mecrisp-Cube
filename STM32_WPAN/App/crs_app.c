/**
 *  @brief
 *      Cable Replacement Server CRS (for GAP Peripheral Role).
 *
 *      Using CMSIS-RTOS queues as buffers.
 *  @file
 *      crs_app.c
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
#include <stdio.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "crs_app.h"

#include "dbg_trace.h"
#include "ble.h"
#include "crs_stm.h"
#include "assert.h"


// Rx/Tx Buffer Length
// *******************
#define CRS_TX_BUFFER_LENGTH	1024
#define CRS_RX_BUFFER_LENGTH	1024

// max. Tx tries
#define MAX_TRIES				50

// Private function prototypes
// ***************************
static void CRS_Thread(void *argument);

// Global Variables
// ****************

// RTOS resources
// **************

// Definitions for CRS thread
osThreadId_t CRS_ThreadId;
static const osThreadAttr_t crs_ThreadAttr = {
		.name = "BLE_CRS",
		.priority = (osPriority_t) osPriorityHigh,
		.stack_size = 128 * 5
};

static osMutexId_t CRS_MutexID;
const osMutexAttr_t CRS_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

// Definitions for TxQueue
static osMessageQueueId_t CRS_TxQueueId;
static const osMessageQueueAttr_t crs_TxQueue_attributes = {
		.name = "CRS_TxQueue"
};

// Definitions for RxQueue
static osMessageQueueId_t CRS_RxQueueId;
static const osMessageQueueAttr_t crs_RxQueue_attributes = {
		.name = "CRS_RxQueue"
};


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the CRS.
 *  @return
 *      None
 */
void CRSAPP_Init(void) {
	// Create the queue(s)
	// creation of TxQueue
	CRS_TxQueueId = osMessageQueueNew(CRS_TX_BUFFER_LENGTH, sizeof(uint8_t),
			&crs_TxQueue_attributes);
	if (CRS_TxQueueId == NULL) {
		Error_Handler();
	}
	// creation of RxQueue
	CRS_RxQueueId = osMessageQueueNew(CRS_RX_BUFFER_LENGTH, sizeof(uint8_t),
			&crs_RxQueue_attributes);
	if (CRS_RxQueueId == NULL) {
		Error_Handler();
	}

	CRS_MutexID = osMutexNew(&CRS_MutexAttr);
	if (CRS_MutexID == NULL) {
		Error_Handler();
	}

	// creation of CRS_TxThread
	CRS_ThreadId = osThreadNew(CRS_Thread, NULL, &crs_ThreadAttr);
	if (CRS_ThreadId == NULL) {
		Error_Handler();
	}
}


/**
 *  @brief
 *		Reads a char from the CRS Rx (serial in). Blocking until char is
 *      ready.
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int CRSAPP_getc(void) {
	uint8_t c;
	if (osMessageQueueGet(CRS_RxQueueId, &c, NULL, osWaitForever) == osOK) {
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
int CRSAPP_RxReady(void) {
	if (osMessageQueueGetCount(CRS_RxQueueId) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/**
 *  @brief
 *      Writes a char to the CRS Tx (serial out). Blocking until char can be
 *      written into the queue.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int CRSAPP_putc(int c) {
	if (osMessageQueuePut(CRS_TxQueueId, &c, 0, osWaitForever) == osOK) {
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
int CRSAPP_TxReady(void) {
	if (osMessageQueueGetSpace(CRS_TxQueueId) > 0) {
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
int CRSAPP_putkey(const char c) {
	osStatus_t status;

	if (c == '\r') {
		// eat CR
		return 0;
	}
	status = osMessageQueuePut(CRS_RxQueueId, &c, 0, osWaitForever);
	if (status == osOK) {
		return 0;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**
 *  @brief
 *      Notification from CRS_Event_Handler.
 *
 *      Callback routine from crs_stm.c
 *  @return
 *      none
 */
void CRSAPP_Notification(CRSAPP_Notification_evt_t *pNotification) {
	uint8_t buffer;
	uint32_t i;
	osStatus_t status;

	switch(pNotification->CRS_Evt_Opcode) {
	case CRS_WRITE_EVT:
		APP_DBG_MSG("CRS_WRITE_EVT: Data received: %s \n", pNotification->DataTransfered.pPayload);
		pNotification->DataTransfered.pPayload[pNotification->DataTransfered.Length] = '\0';

		for (i=0; i<pNotification->DataTransfered.Length; i++) {
			buffer = pNotification->DataTransfered.pPayload[i];
			ASSERT_nonfatal(buffer != 0x03, ASSERT_CRS_SIGINT, 0) // ^C character abort
			status = osMessageQueuePut(CRS_RxQueueId, &buffer, 0, 0);
			if (status != osOK) {
				// can't put char into queue
				Error_Handler();
				break;
			}
		}
		break;

	case CRS_NOTIFY_ENABLED_EVT:
		APP_DBG_MSG("CRS_NOTIFY_ENABLED_EVT\n");
		break;

	case CRS_NOTIFY_DISABLED_EVT:
		APP_DBG_MSG("CRS_NOTIFY_DISABLED_EVT\n");
		break;

	default:
		break;
	}
}


/**
  * @brief
  * 	Function implementing the CRS thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void CRS_Thread(void *argument) {
	uint8_t buffer[CRS_MAX_DATA_LEN];
	uint32_t count;
	uint8_t i;
	tBleStatus status;
	uint8_t tries;

	// blocked till connected
	osThreadFlagsWait(CRSAPP_CONNECTED, osFlagsWaitAny, osWaitForever);
	osDelay(3000);

	// Infinite loop
	for(;;) {
		// blocked till a character is in the Tx queue
		if (osMessageQueueGet(CRS_TxQueueId, &buffer[0], 0, osWaitForever) == osOK) {
			count = osMessageQueueGetCount(CRS_TxQueueId);
			if (count > CRS_MAX_DATA_LEN-2) {
				count = CRS_MAX_DATA_LEN-2;
			}
			for (i=1; i<count+1; i++) {
				osMessageQueueGet(CRS_TxQueueId, &buffer[i], 0, osWaitForever);
			}
			buffer[count+1] = '\0';
			// send the characters
			status = BLE_STATUS_TIMEOUT;
			tries = 0;
			while (status != BLE_STATUS_SUCCESS) {
				// not elegant but it works
				status = CRSAPP_Update_Char(CRS_RX_CHAR_UUID, (uint8_t *)&buffer[0]);
				if (status != BLE_STATUS_SUCCESS && tries++ > MAX_TRIES) {
					Error_Handler();
					break;
				}
				osDelay(10);
			}
		} else {
			// can't write to the queue
			Error_Handler();
		}
	}
}

