/**
 *  @brief
 *      Buffered I2C (or IIC) communication.
 *
 *      Using interrupt and DMA for I2C3 peripheral.
 *      CMSIS-RTOS Mutex for mutual-exclusion I2C resource.
 *      For Flipper external (GPIO) devices-
 *  @file
 *      iic3.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-01-02
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
#include "i2c.h"
#include "iic3.h"
#include "myassert.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************
volatile int IIC3_Status;

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;

// RTOS resources
// **************

osMutexId_t IIC3_MutexID;
const osMutexAttr_t IIC3_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

osSemaphoreId_t IIC3_SemaphoreID;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the IIC3.
 *  @return
 *      None
 */
void IIC3_init(void) {
	IIC3_MutexID = osMutexNew(&IIC3_MutexAttr);
	ASSERT_fatal(IIC3_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	IIC3_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(IIC3_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
}


/**
 *  @brief
 *		Is I2C ready?
 *  @return
 *      TRUE: ready, FALSE: transfer ongoing
 *      error.
 */
int IIC3_ready(void) {
	if (osMutexGetOwner(IIC3_MutexID) == NULL) {
		// ready for the next transfer
		return TRUE;
	} else {
		// transfer ongoing
		return FALSE;
	}
}


/**
 *  @brief
 *		Get a message from the IIC3. Blocking until message is received.
 *
 *      Do not use in ISRs.
 * @param[in]
 *     RxBuffer: Pointer to data buffer for the message
 * @param[in]
 *     RxSize: number of bytes to receive
 * @param[in]
 *     dev: I2C device number
 *  @return
 *      Return 0 for success, -1 I2C error, -2 abort, -3 HAL
 */
int IIC3_getMessage(uint8_t *RxBuffer, uint32_t RxSize, uint16_t dev) {
	HAL_StatusTypeDef hal_status = HAL_OK;

	// only one thread is allowed to use the IIC3
	osMutexAcquire(IIC3_MutexID, osWaitForever);
	IIC3_Status = 0;
	// get the Message
	hal_status = HAL_I2C_Master_Receive_DMA(&hi2c1, dev<<1, RxBuffer, RxSize);
	if (hal_status == HAL_OK) {
		// blocked till message is received
		osSemaphoreAcquire(IIC3_SemaphoreID, osWaitForever);
	} else {
		// can't get Message
		IIC3_Status = -3;
	}
	osMutexRelease(IIC3_MutexID);
	if (IIC3_Status != 0) {
		Error_Handler();
	}

	return IIC3_Status;
}


/**
 *  @brief
 *      Put a message to the IIC3. Blocking until message is sent.
 *
 *      Do not use in ISRs.
 * @param[in]
 *     TxBuffer: Pointer to data buffer for the message
 * @param[in]
 *     TxSize: number of bytes to send
 * @param[in]
 *     dev: I2C device number
 *  @return
 *      Return 0 for success, -1 I2C error, -2 abort, -3 HAL
 */
int IIC3_putMessage(uint8_t *TxBuffer, uint32_t TxSize, uint16_t dev) {
	HAL_StatusTypeDef hal_status = HAL_OK;

	// only one thread is allowed to use the IIC3
	osMutexAcquire(IIC3_MutexID, osWaitForever);
	IIC3_Status = 0;
	// send the Message
	hal_status = HAL_I2C_Master_Transmit_DMA(&hi2c1, dev<<1, TxBuffer, TxSize);
	if (hal_status == HAL_OK) {
		// blocked till message is received
		osSemaphoreAcquire(IIC3_SemaphoreID, osWaitForever);
	} else {
		// can't get Message
		IIC3_Status = -3;
	}
	osMutexRelease(IIC3_MutexID);
	if (IIC3_Status != 0) {
		Error_Handler();
	}

	return IIC3_Status;
}


/**
 *  @brief
 *      Writes a char to the IIC3. Blocking until char can be written into the queue.
 *
 *		Transmit and receive in the same frame.
 *      Do not use in ISRs.
 * @param[in]
 *     TxRxBuffer: Pointer to data buffer for the message to send and receive
 * @param[in]
 *     TxSize: number of bytes to send
 * @param[in]
 *     RxSize: number of bytes to receive
 * @param[in]
 *     dev: I2C device number
 *  @return
 *      Return 0 for success, -1 I2C error, -2 abort, -3 HAL
 */
int IIC3_putGetMessage(uint8_t *TxRxBuffer, uint32_t TxSize, uint32_t RxSize, uint16_t dev) {
	HAL_StatusTypeDef hal_status = HAL_OK;

	// only one thread is allowed to use the IIC3
	osMutexAcquire(IIC3_MutexID, osWaitForever);
	IIC3_Status = 0;
	// send the Message
	hal_status = HAL_I2C_Master_Sequential_Transmit_DMA(&hi2c1, dev<<1, TxRxBuffer, TxSize, I2C_FIRST_FRAME);
	if (hal_status == HAL_OK) {
		// blocked till message is received
		osSemaphoreAcquire(IIC3_SemaphoreID, osWaitForever);
	} else {
		// can't transmit Message
		IIC3_Status = -3;
	}
	osMutexRelease(IIC3_MutexID);
	if (IIC3_Status != 0) {
		Error_Handler();
		return IIC3_Status;
	}

	// get the Message
	hal_status = HAL_I2C_Master_Sequential_Receive_DMA(&hi2c1, dev<<1, TxRxBuffer, RxSize, I2C_LAST_FRAME);
	if (hal_status == HAL_OK) {
		// blocked till message is received
		osSemaphoreAcquire(IIC3_SemaphoreID, osWaitForever);
	} else {
		// can't get Message
		IIC3_Status = -3;
	}
	osMutexRelease(IIC3_MutexID);
	if (IIC3_Status != 0) {
		Error_Handler();
	}

	return IIC3_Status;
}


// Private Functions
// *****************



// Callbacks
// *********

// callbacks are handled in iic module
