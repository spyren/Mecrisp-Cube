/**
 *  @brief
 *      Serial Peripheral Interface (SPI) for MIP, dotstar, and EPD
 *
 *		Default is SD. SD is using 2EDGE (CPHA=1) and polarity high (CPOL=1), MODE3
 *		The application is responsible for chip select and mode. To protect
 *		for race condition use the RTSPI_MutexID.
 *		F405: spi2 is used. FD is using spi1.
 *  @file
 *      rt_spi.c
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
#include "rt_spi.h"
#include "fd_spi.h"
#include "myassert.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************

osMutexId_t RTSPI_MutexID;

// RTOS resources
// **************

static const osMutexAttr_t RTSPI_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t RTSPI_SemaphoreID;


// Hardware resources
// ******************

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;


// Private Variables
// *****************
static volatile int RTSPI_Status = 0;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the RTSPI.
 *  @return
 *      None
 */
void RTSPI_init(void) {
	RTSPI_MutexID = osMutexNew(&RTSPI_MutexAttr);
	ASSERT_fatal(RTSPI_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	RTSPI_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(RTSPI_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
}

/**
 *  @brief
 *      Get the RTSPI mutex address.
 *  @return
 *      Mutex address
 */
uint32_t* RTSPI_getMutex(void) {
	return (uint32_t*) RTSPI_MutexID;
}

/**
  * @brief
  *     SPI write byte(s) to and read from device
  *
  *     Using DMA. RTOS blocking till finished.
  * @param[in]
  *     DataIn: Pointer to data buffer to write
  * @param[out]
  *     DataOut: Pointer to data buffer for read data
  * @param[in]
  *     DataLength: number of bytes to write
  * @retval
  *      Return 0 for success, -1 SPI error, -2 abort, -3 HAL, -4 RTOS
  */
int RTSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	RTSPI_Status = 0;
	hal_status = HAL_SPI_TransmitReceive_DMA(&hspi2, (uint8_t*) DataIn, DataOut, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RTSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			RTSPI_Status = -4;
		}
	} else {
		RTSPI_Status = -3;
	}

	if (RTSPI_Status != 0) {
		Error_Handler();
	}
	return RTSPI_Status;
}


/**
  * @brief
  *     SPI write byte(s) to device
  *
  *     Using DMA. RTOS blocking till finished.
  * @param[in]
  *     Data: Pointer to data buffer to write
  * @param[in]
  *     DataLength: number of bytes to write
  * @retval
  *      Return 0 for success, -1 SPI error, -2 abort, -3 HAL
  */
int RTSPI_WriteData(const uint8_t *Data, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	RTSPI_Status = 0;
	hal_status = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*) Data, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RTSPI_SemaphoreID, osWaitForever);
		if (os_status != osOK) {
			RTSPI_Status = -4;
		}
	} else {
		RTSPI_Status = -3;
	}

	if (RTSPI_Status != 0) {
		Error_Handler();
	}
	return RTSPI_Status;
}


/**
  * @brief
  *     SPI read byte(s) from device
  *
  *     Using DMA. RTOS blocking till finished.
  * @param[in]
  *     Data: Pointer to data buffer to write
  * @param[in]
  *     DataLength: number of bytes to write
  * @retval
  *      Return 0 for success, -1 SPI error, -2 abort, -3 HAL
  */
int RTSPI_ReadData(const uint8_t *Data, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	RTSPI_Status = 0;
	hal_status = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*) Data, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RTSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			RTSPI_Status = -4;
		}
	} else {
		RTSPI_Status = -3;
	}

	if (RTSPI_Status != 0) {
		Error_Handler();
	}
	return RTSPI_Status;
}


/**
  * @brief
  *     SPI Write a byte to device
  *
  *     Using DMA. RTOS blocking till finished.
  * @param[in]
  *     Value: value to be written
  * @retval
  *      Return 0 for success, -1 SPI error, -2 abort, -3 HAL
  */
int RTSPI_Write(uint8_t Value) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;
	uint8_t data = Value;

	RTSPI_Status = 0;
	hal_status = HAL_SPI_Transmit_DMA(&hspi2, &data, 1);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RTSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			RTSPI_Status = -4;
		}
	} else {
		RTSPI_Status = -3;
	}

	if (RTSPI_Status != 0) {
		Error_Handler();
	}
	return RTSPI_Status;
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
		osSemaphoreRelease(RTSPI_SemaphoreID);
	}
}


/**
  * @brief  Tx Transfer completed callback.
  * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
  *               the configuration information for SPI module.
  * @retval None
  */
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == hspi1.Instance) {
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}

	if (hspi->Instance == hspi2.Instance) {
		osSemaphoreRelease(RTSPI_SemaphoreID);
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
		FDSPI_Error = -1;
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}

	if (hspi->Instance == hspi2.Instance) {
		RTSPI_Status = -1;
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}
}

/**
  * @brief  SPI Abort Complete callback.
  * @param  hspi SPI handle.
  * @retval None
  */
void HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef *hspi) {
	if (hspi->Instance == hspi1.Instance) {
		FDSPI_Error = -2;
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}

	if (hspi->Instance == hspi2.Instance) {
		RTSPI_Status = -2;
		osSemaphoreRelease(FDSPI_SemaphoreID);
	}
}
