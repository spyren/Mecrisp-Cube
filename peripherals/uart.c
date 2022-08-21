/**
 *  @brief
 *      Buffered serial communication.
 *
 *      Using interrupt for USART1 peripheral. Separate threads for transmitting
 *      and receiving data. CMSIS-RTOS Mutex for mutual-exclusion UART resource.
 *      CMSIS-RTOS queues as buffers.
 *      CR is end of line for Rx.
 *      LF is end of line for Tx.
 *  @file
 *      uart.c
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
#include "uart.h"
#include "myassert.h"

// Rx/Tx Buffer Length
// *******************
#define UART_TX_BUFFER_LENGTH	1024
#define UART_RX_BUFFER_LENGTH	(5 * 1024)

#define UART_CHAR_SENT		0x01
#define UART_CHAR_RECEIVED	0x01

// Private function prototypes
// ***************************
static void UART_TxThread(void *argument);
static void UART_RxThread(void *argument);

// Global Variables
// ****************

// Hardware resources
// ******************
extern UART_HandleTypeDef huart1;

// RTOS resources
// **************

// Definitions for UART Tx thread
static osThreadId_t UART_TxThreadId;
static const osThreadAttr_t UART_TxThreadAttr = {
		.name = "UART_Tx",
		.priority = (osPriority_t) osPriorityHigh,
		.stack_size = 128 * 5
};

// Definitions for UART Rx thread
static osThreadId_t UART_RxThreadId;
static const osThreadAttr_t UART_RxThreadAttr = {
		.name = "UART_Rx",
		.priority = (osPriority_t) osPriorityHigh,
		.stack_size = 128 * 5
};

osMutexId_t UART_MutexID;
const osMutexAttr_t UART_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

// Definitions for TxQueue
static osMessageQueueId_t UART_TxQueueId;
static const osMessageQueueAttr_t uart_TxQueue_attributes = {
		.name = "UART_TxQueue"
};

// Definitions for RxQueue
static osMessageQueueId_t UART_RxQueueId;
static const osMessageQueueAttr_t uart_RxQueue_attributes = {
		.name = "UART_RxQueue"
};

// Private Variables
// *****************
static uint8_t UART_TxBuffer;
static uint8_t UART_RxBuffer;

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the UART.
 *  @return
 *      None
 */
void UART_init(void) {
	// Create the queue(s)
	// creation of TxQueue
	UART_TxQueueId = osMessageQueueNew(UART_TX_BUFFER_LENGTH, sizeof(uint8_t),
			&uart_TxQueue_attributes);
	ASSERT_fatal(UART_TxQueueId != NULL, ASSERT_QUEUE_CREATION, __get_PC());
	// creation of RxQueue
	UART_RxQueueId = osMessageQueueNew(UART_RX_BUFFER_LENGTH, sizeof(uint8_t),
			&uart_RxQueue_attributes);
	ASSERT_fatal(UART_RxQueueId != NULL, ASSERT_QUEUE_CREATION, __get_PC());

	UART_MutexID = osMutexNew(&UART_MutexAttr);
	ASSERT_fatal(UART_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	// creation of UART_TxThread
	UART_TxThreadId = osThreadNew(UART_TxThread, NULL, &UART_TxThreadAttr);
	ASSERT_fatal(UART_TxThreadId != NULL, ASSERT_THREAD_CREATION, __get_PC());

	// creation of UART_RxThread
	UART_RxThreadId = osThreadNew(UART_RxThread, NULL, &UART_RxThreadAttr);
	ASSERT_fatal(UART_RxThreadId != NULL, ASSERT_THREAD_CREATION, __get_PC());
}

/**
 *  @brief
 *      Resets the UART queues.
 *  @return
 *      None
 */
void UART_reset(void) {
	osMessageQueueReset(UART_RxQueueId);
	osMessageQueueReset(UART_TxQueueId);
}


/**
 *  @brief
 *		Reads a char from the UART Rx (serial in). Blocking until char is
 *      ready.
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int UART_getc(void) {
	uint8_t c;
	if (osMessageQueueGet(UART_RxQueueId, &c, NULL, osWaitForever) == osOK) {
		return c;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**
 *  @brief
 *      Reads a line from the UART Rx (serial in). Blocking until line is
 *      ready (newline character CR is read) or max length is reached.
 *
 *      Does not work in ISRs.
 *  @param[out]
 *  	str This is the pointer to an array of chars where the C string is
 *  		stored. C style string.
 *  @param[in]
 *      length  max. string length
 *  @return
 *      Return 0 if successful, EOF on error
 */
int UART_gets(char *str, int length) {
	int i = 0;
	uint8_t c;

	for (i=0; i<length; i++) {
		if (osMessageQueueGet(UART_RxQueueId, &c, NULL, osWaitForever) == osOK) {
			str[i] = c;
			if (c == '\r' || c == '\n') {
				str[i+1] = 0;
				return 0;
			}
		} else {
			Error_Handler();
			str[i] = EOF;
			str[i+1] = 0;
			return EOF;
		}
	}
	return 0;
}


/**
 *  @brief
 *		There is a character in the queue (key pressed).
 *  @return
 *		TRUE if a character has been received.
 */
int UART_RxReady(void) {
	if (osMessageQueueGetCount(UART_RxQueueId) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}


/**
 *  @brief
 *      Writes a char to the UART Tx (serial out). Blocking until char can be
 *      written into the queue.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int UART_putc(int c) {
	osStatus_t status;

	status = osMessageQueuePut(UART_TxQueueId, &c, 0, osWaitForever);
	if (status == osOK) {
		return 0;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**
 *  @brief
 *      Writes a line (string) to the UART Tx (serial out). Blocking until
 *      string can be written into the queue.
 *
 *      Does not work in ISRs.
 *  @param
 *      s  string to write. C style string.
 *  @return
 *      Return EOF on error, 0 on success.
 */
int UART_puts(const char *s) {
	int i=0;
	uint8_t buffer;

	while (s[i] != 0) {
		buffer = (uint8_t) s[i];
		if (osMessageQueuePut(UART_TxQueueId, &buffer, 0, osWaitForever) != osOK) {
			Error_Handler();
			return EOF;
		}
		i++;
	}
	return 0;
}


/**
 *  @brief
 *      Tx queue ready for next char.
 *  @return
 *      FALSE if the buffer is full.
 */
int UART_TxReady(void) {
	if (osMessageQueueGetSpace(UART_TxQueueId) > 0) {
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
int UART_putkey(const char c) {
	osStatus_t status;

	if (c == '\r') {
		// eat CR
		return 0;
	}
	status = osMessageQueuePut(UART_RxQueueId, &c, 0, osWaitForever);
	if (status == osOK) {
		return 0;
	} else {
		Error_Handler();
		return EOF;
	}
}


/**
 *  @brief
 *      Sets the serial port's baud rate.
 *
 *      No checks whatsoever
 *	@param[in]
 *      baudrate    0 to 15.
 *  @return
 *      none
 *
 */
void UART_setBaudrate(const int baudrate) {
	// only one thread is allowed to use the UART
	osMutexAcquire(UART_MutexID, osWaitForever);

	huart1.Init.BaudRate = baudrate;
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	osMutexRelease(UART_MutexID);
}


/**
 *  @brief
 *      Sets the serial port's word length.
 *
 *	@param[in]
 *      wordlength    7, 8, 9 (including parity).
 *  @return
 *      none
 *
 */
void UART_setWordLength(const int wordlength) {
	// only one thread is allowed to use the UART
	osMutexAcquire(UART_MutexID, osWaitForever);

	switch (wordlength) {
	case 7:
		huart1.Init.WordLength = UART_WORDLENGTH_7B;
		break;
	case 8:
		huart1.Init.WordLength = UART_WORDLENGTH_8B;
		break;
	case 9:
		huart1.Init.WordLength = UART_WORDLENGTH_9B;
		break;
	default:
		huart1.Init.WordLength = UART_WORDLENGTH_8B;
		break;
	}
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}
	osMutexRelease(UART_MutexID);
}


/**  @brief
*      Sets the serial port's parity bit.
*
*	@param[in]
*      paritybit    0 none, 1 odd, 2 even.
*  @return
*      none
*
*/
void UART_setParityBit(const int paritybit) {
	// only one thread is allowed to use the UART
	osMutexAcquire(UART_MutexID, osWaitForever);

	switch (paritybit) {
	case 0:
		huart1.Init.Parity = UART_PARITY_NONE;
		break;
	case 1:
		huart1.Init.Parity = UART_PARITY_ODD;
		break;
	case 2:
		huart1.Init.Parity = UART_PARITY_EVEN;
		break;
	default:
		huart1.Init.Parity = UART_PARITY_NONE;
		break;
	}
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}

	osMutexRelease(UART_MutexID);
}

/**
 *  @brief
 *	    Sets the serial port's stop bits.
 *
 *	@param[in]
 *      stopbits    0 1 bit, 1 1.5 bit, 2 2 bit.
 *  @return
 *      none
 *
 */
void UART_setStopBits(const int stopbits) {
	// only one thread is allowed to use the UART
	osMutexAcquire(UART_MutexID, osWaitForever);

	switch (stopbits) {
	case 0:
		huart1.Init.StopBits = UART_STOPBITS_1;
		break;
	case 1:
		huart1.Init.StopBits = UART_STOPBITS_1_5;
		break;
	case 2:
		huart1.Init.StopBits = UART_STOPBITS_2;
		break;
	default:
		huart1.Init.StopBits = UART_STOPBITS_1;
		break;
	}
	if (HAL_UART_Init(&huart1) != HAL_OK) {
		Error_Handler();
	}

	osMutexRelease(UART_MutexID);
}


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the UART Tx thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void UART_TxThread(void *argument) {
	osStatus_t status;

	// Infinite loop
	for(;;) {
		// blocked till a character is in the Tx queue
		status = osMessageQueueGet(UART_TxQueueId, &UART_TxBuffer, 0, osWaitForever);
		if (status == osOK) {
			// only one thread is allowed to use the UART
			osMutexAcquire(UART_MutexID, osWaitForever);
			// send the character
			if (HAL_UART_Transmit_IT(&huart1, &UART_TxBuffer, 1) == HAL_ERROR) {
				// can't send char
				Error_Handler();
			}
			osMutexRelease(UART_MutexID);

			// blocked till character is sent
			status = osThreadFlagsWait(UART_CHAR_SENT, osFlagsWaitAny, 5);
		} else {
			// can't read the queue
			Error_Handler();
		}
	}
}


/**
  * @brief
  * 	Function implementing the UART Rx thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void UART_RxThread(void *argument) {
	osStatus_t status;

	osMutexAcquire(UART_MutexID, osWaitForever);
	// wait for the first Rx character
	if (HAL_UART_Receive_IT(&huart1, &UART_RxBuffer, 1) != HAL_OK) {
		// something went wrong
		Error_Handler();
	}
	osMutexRelease(UART_MutexID);

	// Infinite loop
	for(;;) {
		// blocked till a character is received
		status = osThreadFlagsWait(UART_CHAR_RECEIVED, osFlagsWaitAny, osWaitForever);
		ASSERT_nonfatal(UART_RxBuffer != 0x03, ASSERT_UART_SIGINT, 0) // ^C character abort
		// put the received character into the queue
		status = osMessageQueuePut(UART_RxQueueId, &UART_RxBuffer, 0, 100);
		if (status != osOK) {
			// can't put char into queue
			Error_Handler();
		}
		// receive the next character
		osMutexAcquire(UART_MutexID, osWaitForever);
		status = HAL_UART_Receive_IT(&huart1, &UART_RxBuffer, 1);
		osMutexRelease(UART_MutexID);
		if (status != osOK) {
			// can't receive char
			Error_Handler();
		}
	}
}


// Callbacks
// *********

/**
  * @brief Tx Transfer completed callback.
  * @param huart UART handle.
  * @retval None
  */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	osThreadFlagsSet(UART_TxThreadId, UART_CHAR_SENT);
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	osThreadFlagsSet(UART_RxThreadId, UART_CHAR_RECEIVED);
}

/**
  * @brief  UART error callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	ASSERT_nonfatal(0, ASSERT_UART_ERROR_CALLBACK, 0);
}


/**
  * @brief  UART RX Fifo full callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	ASSERT_nonfatal(0, ASSERT_UART_FIFO, 0);
}

