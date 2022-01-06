/**
 *  @brief
 *      Serial Peripheral Interface (SPI) for the serial flash.
 *
 *		The spi1 interface is used for the serial flash.
 *  @file
 *      fd_spi.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-06-03
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
#include "fd_spi.h"
#include "ext_spi.h"
#include "assert.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************

// RTOS resources
// **************

static osMutexId_t FDSPI_MutexID;
static const osMutexAttr_t FDSPI_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t FDSPI_SemaphoreID;


// Hardware resources
// ******************

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;


// Private Variables
// *****************
static volatile uint8_t SpiError = FALSE;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the FDSPI.
 *  @return
 *      None
 */
void FDSPI_init(void) {
	FDSPI_MutexID = osMutexNew(&FDSPI_MutexAttr);
	ASSERT_fatal(FDSPI_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	FDSPI_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(FDSPI_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
}

/**
  * @brief
  *     SPI Write byte(s) to device
  *
  *     Using DMA. RTOS blocking till finished.
  * @param[in]
  *     DataIn: Pointer to data buffer to write
  * @param[out]
  *     DataOut: Pointer to data buffer for read data
  * @param[in]
  *     DataLength: number of bytes to write
  * @retval
  *     None
  */
void FDSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	// only one thread is allowed to use the SPI
	osMutexAcquire(FDSPI_MutexID, osWaitForever);

	SpiError = FALSE;
	hal_status = HAL_SPI_TransmitReceive_DMA(&hspi1, (uint8_t*) DataIn, DataOut, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(FDSPI_SemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	osMutexRelease(FDSPI_MutexID);
}


/**
  * @brief
  *     SPI Write a byte to device
  *
  *     Using DMA. RTOS blocking till finished.
  * @param[in]
  *     Value: value to be written
  * @retval
  *     None
  */
void FDSPI_Write(uint8_t Value) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;
	uint8_t data;

	// only one thread is allowed to use the SPI
	osMutexAcquire(FDSPI_MutexID, osWaitForever);

	SpiError = FALSE;
	hal_status = HAL_SPI_TransmitReceive_DMA(&hspi1, (uint8_t*) &Value, &data, 1);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(FDSPI_SemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	osMutexRelease(FDSPI_MutexID);
}



// Callbacks
// *********

/**
  * @brief  Tx and Rx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == hspi1.Instance) {
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}

	if (hspi->Instance == hspi2.Instance) {
		osSemaphoreRelease(EXTSPI_SemaphoreID);
	}
}


/**
  * @brief  SPI error callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == hspi1.Instance) {
		SpiError = TRUE;
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}

	if (hspi->Instance == hspi2.Instance) {
		EXTSPI_Error = TRUE;
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}
}

