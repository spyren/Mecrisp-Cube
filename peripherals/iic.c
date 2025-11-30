/**
 *  @brief
 *      Buffered I2C (or IIC) communication.
 *
 *      Using interrupt and DMA for I2C1 peripheral.
 *      CMSIS-RTOS Mutex for mutual-exclusion I2C resource.
 *  @file
 *      iic.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-12-29
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
#include "iic.h"
#include "iic3.h"
#include "myassert.h"
#include "stm32_lpm.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************
static volatile int IIC_Status;

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;
extern I2C_HandleTypeDef hi2c3;

// RTOS resources
// **************

osMutexId_t IIC_MutexID;
const osMutexAttr_t IIC_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t IIC_SemaphoreID;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the IIC.
 *  @return
 *      None
 */
void IIC_init(void) {
	IIC_MutexID = osMutexNew(&IIC_MutexAttr);
	ASSERT_fatal(IIC_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	IIC_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(IIC_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
}


/**
 *  @brief
 *		Is I2C ready?
 *  @return
 *      TRUE: ready, FALSE: transfer ongoing
 *      error.
 */
int IIC_ready(void) {
	if (osMutexGetOwner(IIC_MutexID) == NULL) {
		// ready for the next transfer
		return TRUE;
	} else {
		// transfer ongoing
		return FALSE;
	}
}


/**
 *  @brief
 *		Get a message from the IIC. Blocking until message is received.
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
int IIC_getMessage(uint8_t *RxBuffer, uint32_t RxSize, uint16_t dev) {
	HAL_StatusTypeDef hal_status = HAL_OK;

	UTIL_LPM_SetStopMode(1U << CFG_LPM_IIC, UTIL_LPM_DISABLE);
	// only one thread is allowed to use the IIC
	osMutexAcquire(IIC_MutexID, osWaitForever);
	IIC_Status = 0;
	// get the Message
	hal_status = HAL_I2C_Master_Receive_DMA(&hi2c1, dev<<1, RxBuffer, RxSize);
	if (hal_status == HAL_OK) {
		// blocked till message is received
		if (osSemaphoreAcquire(IIC_SemaphoreID, IIC_TIMEOUT) == osErrorTimeout) {
			hal_status = -4;
		}
	} else {
		// can't get Message
		IIC_Status = -3;
	}
	osMutexRelease(IIC_MutexID);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_IIC, UTIL_LPM_ENABLE);
	if (IIC_Status != 0) {
		ASSERT_nonfatal(0, ASSERT_I2C, 0);
	}

	return IIC_Status;
}


/**
 *  @brief
 *      Put a message to the IIC. Blocking until message is sent.
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
int IIC_putMessage(uint8_t *TxBuffer, uint32_t TxSize, uint16_t dev) {
	HAL_StatusTypeDef hal_status = HAL_OK;

	UTIL_LPM_SetStopMode(1U << CFG_LPM_IIC, UTIL_LPM_DISABLE);
	// only one thread is allowed to use the IIC
	osMutexAcquire(IIC_MutexID, osWaitForever);
	IIC_Status = 0;
	// send the Message
	hal_status = HAL_I2C_Master_Transmit_DMA(&hi2c1, dev<<1, TxBuffer, TxSize);
	if (hal_status == HAL_OK) {
		// blocked till message is sent
		if (osSemaphoreAcquire(IIC_SemaphoreID, IIC_TIMEOUT) == osErrorTimeout) {
			hal_status = -4;
		}
	} else {
		// can't get Message
		IIC_Status = -3;
	}
	osMutexRelease(IIC_MutexID);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_IIC, UTIL_LPM_ENABLE);
	if (IIC_Status != 0) {
		ASSERT_nonfatal(0, ASSERT_I2C, 0);
	}

	return IIC_Status;
}


/**
 *  @brief
 *      Writes a char to the IIC. Blocking until char can be written into the queue.
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
int IIC_putGetMessage(uint8_t *TxRxBuffer, uint32_t TxSize, uint32_t RxSize, uint16_t dev) {
	HAL_StatusTypeDef hal_status = HAL_OK;

	UTIL_LPM_SetStopMode(1U << CFG_LPM_IIC, UTIL_LPM_DISABLE);
	// only one thread is allowed to use the IIC
	osMutexAcquire(IIC_MutexID, osWaitForever);
	IIC_Status = 0;
	// send the Message
	hal_status = HAL_I2C_Master_Sequential_Transmit_DMA(&hi2c1, dev<<1, TxRxBuffer, TxSize, I2C_FIRST_FRAME);
	if (hal_status == HAL_OK) {
		// blocked till message is sent
		if (osSemaphoreAcquire(IIC_SemaphoreID, IIC_TIMEOUT) == osErrorTimeout) {
			hal_status = -4;
		}
	} else {
		// can't transmit Message
		IIC_Status = -3;
	}
	if (IIC_Status != 0) {
		ASSERT_nonfatal(0, ASSERT_I2C, 0);
		osMutexRelease(IIC_MutexID);
		UTIL_LPM_SetStopMode(1U << CFG_LPM_IIC, UTIL_LPM_ENABLE);
		return IIC_Status;
	}

	// get the Message
	hal_status = HAL_I2C_Master_Sequential_Receive_DMA(&hi2c1, dev<<1, TxRxBuffer, RxSize, I2C_LAST_FRAME);
	if (hal_status == HAL_OK) {
		// blocked till message is received
		if (osSemaphoreAcquire(IIC_SemaphoreID, IIC_TIMEOUT) == osErrorTimeout) {
			hal_status = -4;
		}
	} else {
		// can't get Message
		IIC_Status = -3;
	}
	osMutexRelease(IIC_MutexID);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_IIC, UTIL_LPM_ENABLE);
	if (IIC_Status != 0) {
		ASSERT_nonfatal(0, ASSERT_I2C, 0);
	}

	return IIC_Status;
}


// Private Functions
// *****************



// Callbacks
// *********

/**
  * @brief  Master Tx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		osSemaphoreRelease(IIC_SemaphoreID);
	} else if (hi2c->Instance == I2C3) {
		osSemaphoreRelease(IIC3_SemaphoreID);
	}
}

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		osSemaphoreRelease(IIC_SemaphoreID);
	} else if (hi2c->Instance == I2C3) {
		osSemaphoreRelease(IIC3_SemaphoreID);
	}
}

/**
  * @brief  I2C error callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		IIC_Status = -1;
		osSemaphoreRelease(IIC_SemaphoreID);
	} else if (hi2c->Instance == I2C3) {
		IIC3_Status = -1;
		osSemaphoreRelease(IIC3_SemaphoreID);
	}
}


/**
  * @brief  I2C abort callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c->Instance == I2C1) {
		IIC_Status = -2;
		osSemaphoreRelease(IIC_SemaphoreID);
	} else if (hi2c->Instance == I2C3) {
		IIC3_Status = -2;
		osSemaphoreRelease(IIC3_SemaphoreID);
	}
}

