/**
 *  @brief
 *      Serial Peripheral Interface (SPI2) for SD card
 *
 *  @file
 *      sd_spi.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-03-30
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
 *      MERCHANTABILITY or FITNESS FOR A PAD_ICULAR PURPOSE. See the GNU
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
#include "sd_spi.h"
#include "rt_spi.h"
#include "myassert.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************

osMutexId_t SDSPI_MutexID;
volatile int SDSPI_SpiStatus = 0;

// RDOS resources
// **************

static const osMutexAttr_t SDSPI_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

osSemaphoreId_t SDSPI_SemaphoreID;


// Hardware resources
// ******************

extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi2_tx;


// Private Variables
// *****************


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the SDSPI.
 *  @return
 *      None
 */
void SDSPI_init(void) {
	SDSPI_MutexID = osMutexNew(&SDSPI_MutexAttr);
	ASSERT_fatal(SDSPI_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	SDSPI_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(SDSPI_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
}

/**
 *  @brief
 *      Get the SDSPI mutex address.
 *  @return
 *      Mutex address
 */
uint32_t* SDSPI_getMutex(void) {
	return (uint32_t*) SDSPI_MutexID;
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
int SDSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	SDSPI_SpiStatus = 0;
	hal_status = HAL_SPI_TransmitReceive_DMA(&hspi2, (uint8_t*) DataIn, DataOut, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(SDSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			SDSPI_SpiStatus = -4;
		}
	} else {
		SDSPI_SpiStatus = -3;
	}

	if (SDSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return SDSPI_SpiStatus;
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
int SDSPI_WriteData(const uint8_t *Data, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	SDSPI_SpiStatus = FALSE;
	hal_status = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*) Data, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(SDSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			SDSPI_SpiStatus = -4;
		}
	} else {
		SDSPI_SpiStatus = -3;
	}

	if (SDSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return SDSPI_SpiStatus;
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
int SDSPI_ReadData(const uint8_t *Data, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	SDSPI_SpiStatus = FALSE;
	hal_status = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*) Data, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(SDSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			SDSPI_SpiStatus = -4;
		}
	} else {
		SDSPI_SpiStatus = -3;
	}

	if (SDSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return SDSPI_SpiStatus;
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
int SDSPI_Write(uint8_t Value) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;
	uint8_t data = Value;

	SDSPI_SpiStatus = FALSE;
	hal_status = HAL_SPI_Transmit_DMA(&hspi2, &data, 1);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(SDSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			SDSPI_SpiStatus = -4;
		}
	} else {
		SDSPI_SpiStatus = -3;
	}

	if (SDSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return SDSPI_SpiStatus;
}


// Callbacks
// *********

// callback are handled by RTSPI module.
