/**
 *  @brief
 *      Buffered serial communication.
 *
 *      Using interrupt for USART1 peripheral.
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

#define UART_TX_SENT	0x01


// Private function prototypes
// ***************************
static void uart_thread(void *argument);

// Global Variables
// ****************

// Hardware resources
// ******************
extern UART_HandleTypeDef huart1;

// RTOS resources
// **************

// Definitions for UART thread
osThreadId_t UART_ThreadId;
static const osThreadAttr_t uart_thread_attributes = {
		.name = "UART_Thread",
		.priority = (osPriority_t) osPriorityNormal,
		.stack_size = 512
};

// Definitions for TxQueue
osMessageQueueId_t UART_TxQueueId;
static const osMessageQueueAttr_t uart_TxQueue_attributes = {
		.name = "UART_TxQueue"
};

// Definitions for RxQueue
osMessageQueueId_t UART_RxQueueId;
static const osMessageQueueAttr_t uart_RxQueue_attributes = {
		.name = "UART_RxQueue"
};

// Private Variables
// *****************
uint8_t uart_rx_buffer;

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the UART.
 *  @return
 *      None
 */
void UART_init(void) {
	HAL_UARTEx_EnableFifoMode(&huart1);
	HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8);
	HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8);

	// Create the queue(s)
	// creation of TxQueue
	UART_TxQueueId = osMessageQueueNew(1024, sizeof(uint8_t), &uart_TxQueue_attributes);
	// creation of RxQueue
	UART_RxQueueId = osMessageQueueNew(2048, sizeof(uint8_t), &uart_RxQueue_attributes);

	// creation of UART_Thread
	UART_ThreadId = osThreadNew(uart_thread, NULL, &uart_thread_attributes);
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
 *  @param[out]
 *  	str This is the pointer to an array of chars where the C string is stored
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
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int UART_putc(int c) {
	if (osMessageQueuePut(UART_TxQueueId, &c, 0, osWaitForever) == osOK) {
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
 *  @param
 *      s  string to write
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


// Private Functions
// *****************

/**
  * @brief
  * 	Function implementing the UART thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void uart_thread(void *argument) {
	uint8_t buffer;
	osStatus_t status;

	// wait for the first Rx character
	if (HAL_UART_Receive_IT(&huart1, &uart_rx_buffer, 1) == HAL_ERROR) {
		// something went wrong
		Error_Handler();
	}

	// Infinite loop
	for(;;) {
		if (HAL_UART_GetState(&huart1) != 0) {
			HAL_UART_AbortReceive(&huart1);
			if (HAL_UART_Receive_IT(&huart1, &uart_rx_buffer, 1) == HAL_ERROR) {
				// something went wrong
				Error_Handler();
			}
//			Error_Handler();
		}
		// blocked till a character is in the Tx queue
		status = osMessageQueueGet(UART_TxQueueId, &buffer, 0, 100);
		if (status == osOK) {
			// send the character
			if (HAL_UART_Transmit_IT(&huart1, &buffer, 1) == HAL_ERROR) {
				// can't send char
				Error_Handler();
			}
			// blocked till character is sent
			osThreadFlagsWait(UART_TX_SENT, osFlagsWaitAny, 2);
		} else if (status == osErrorTimeout) {
			; // go ahead
		} else {
			// can't write to the queue
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

	osThreadFlagsSet(UART_ThreadId, UART_TX_SENT);
}

/**
  * @brief  Rx Transfer completed callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	// put the received character into the queue
	if (osMessageQueuePut(UART_RxQueueId, &uart_rx_buffer, 0, 0) != osOK) {
		// can't put char into queue
		Error_Handler();
	}
	// wait for the next character
	if (HAL_UART_Receive_IT(&huart1, &uart_rx_buffer, 1) == HAL_ERROR) {
		// can't receive char
		Error_Handler();
	};
}

/**
  * @brief  UART error callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	Error_Handler();
}


/**
  * @brief  UART RX Fifo full callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UARTEx_RxFifoFullCallback(UART_HandleTypeDef *huart) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	Error_Handler();
}

