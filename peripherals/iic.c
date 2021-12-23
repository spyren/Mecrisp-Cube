/**
 *  @brief
 *      Buffered I2C (or IIC) communication.
 *
 *      Using interrupt for I2C1 peripheral. Separate threads for transmitting
 *      and receiving data. CMSIS-RTOS Mutex for mutual-exclusion I2C resource.
 *      CMSIS-RTOS queues as buffers.
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


// Private function prototypes
// ***************************

// Global Variables
// ****************
static volatile int IIC_Status;

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c1;

// RTOS resources
// **************

osMutexId_t IIC_MutexID;
const osMutexAttr_t IIC_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t II2_SemaphoreID;

// Private Variables
// *****************
static uint16_t DevAdr;

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
	if (IIC_MutexID == NULL) {
		Error_Handler();
	}

	II2_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (II2_SemaphoreID == NULL) {
		Error_Handler();
	}

}


/**
 *  @brief
 *      Sets the device address.
 *  @return
 *      None
 */
void IIC_setDevice(uint16_t dev) {
	DevAdr = dev << 1;
}


/**
 *  @brief
 *		Reads a char from the IIC Rx (serial in). Blocking until char is
 *      ready.
 *  @return
 *      Return the character read as an unsigned char cast to an int or EOF on
 *      error.
 */
int IIC_getMessage(uint8_t *RxBuffer, uint32_t RxSize) {
	// only one thread is allowed to use the IIC
	osMutexAcquire(IIC_MutexID, osWaitForever);
	IIC_Status = 0;
	// get the Message
	if (HAL_I2C_Master_Transmit_DMA(&hi2c1, DevAdr, RxBuffer, RxSize) == HAL_ERROR) {
		// can't get Message
		Error_Handler();
	}
	// blocked till message is sent
	osSemaphoreAcquire(II2_SemaphoreID, osWaitForever);
	osMutexRelease(IIC_MutexID);

	return IIC_Status;
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
 *      Writes a char to the IIC Tx (serial out). Blocking until char can be
 *      written into the queue.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int IIC_putMessage(uint8_t *TxBuffer, uint32_t TxSize) {
	// only one thread is allowed to use the IIC
	osMutexAcquire(IIC_MutexID, osWaitForever);
	IIC_Status = 0;
	// send the Message
	if (HAL_I2C_Master_Transmit_DMA(&hi2c1, DevAdr, TxBuffer, TxSize) == HAL_ERROR) {
		// can't send message
		Error_Handler();
	}
	// blocked till message is sent
	osSemaphoreAcquire(II2_SemaphoreID, osWaitForever);
	osMutexRelease(IIC_MutexID);

	return IIC_Status;
}


/**
 *  @brief
 *      Writes a char to the IIC Tx (serial out). Blocking until char can be
 *      written into the queue.
 *
 *      Does not work in ISRs.
 *  @param[in]
 *      c  char to write
 *  @return
 *      Return EOF on error, 0 on success.
 */
int IIC_putGetMessage(uint8_t *TxBuffer, uint32_t TxSize, uint8_t *RxBuffer, uint32_t RxSize) {
	// only one thread is allowed to use the IIC
	osMutexAcquire(IIC_MutexID, osWaitForever);
	IIC_Status = 0;
	// send the Message
	if (HAL_I2C_Master_Sequential_Transmit_DMA(&hi2c1, DevAdr, TxBuffer, TxSize, I2C_FIRST_FRAME) == HAL_ERROR) {
		// can't send message
		Error_Handler();
	}
	// blocked till message is sent
	osSemaphoreAcquire(II2_SemaphoreID, osWaitForever);
	// get the Message
	if (HAL_I2C_Master_Sequential_Receive_DMA(&hi2c1, DevAdr, RxBuffer, RxSize, I2C_LAST_FRAME) == HAL_ERROR) {
		// can't get Message
		Error_Handler();
	}
	// blocked till message is sent
	osSemaphoreAcquire(II2_SemaphoreID, osWaitForever);
	osMutexRelease(IIC_MutexID);

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
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hi2c);

	osSemaphoreRelease(II2_SemaphoreID);
}

/**
  * @brief  Master Rx Transfer completed callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {

	/* Prevent unused argument(s) compilation warning */
	UNUSED(hi2c);

	osSemaphoreRelease(II2_SemaphoreID);
}

/**
  * @brief  I2C error callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {

	/* Prevent unused argument(s) compilation warning */
	UNUSED(hi2c);

	IIC_Status = -1;
	osSemaphoreRelease(II2_SemaphoreID);
}


/**
  * @brief  I2C abort callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hi2c);

	IIC_Status = -2;
	osSemaphoreRelease(II2_SemaphoreID);
}

