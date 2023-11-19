/**
 *  @brief
 *      Serial Peripheral Interface (SPI1, SPI_R) for NFC, RFID and Sub-GHz
 *
 *  @file
 *      r_spi.c
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
#include "r_spi.h"
#include "myassert.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************

osMutexId_t RSPI_MutexID;
volatile int RSPI_SpiStatus = 0;

// RDOS resources
// **************

static const osMutexAttr_t RSPI_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

osSemaphoreId_t RSPI_SemaphoreID;


// Hardware resources
// ******************

extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;


// Private Variables
// *****************


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the RSPI.
 *  @return
 *      None
 */
void RSPI_init(void) {
	RSPI_MutexID = osMutexNew(&RSPI_MutexAttr);
	ASSERT_fatal(RSPI_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	RSPI_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(RSPI_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
}

/**
 *  @brief
 *      Get the RSPI mutex address.
 *  @return
 *      Mutex address
 */
uint32_t* RSPI_getMutex(void) {
	return (uint32_t*) RSPI_MutexID;
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
int RSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	RSPI_SpiStatus = 0;
	hal_status = HAL_SPI_TransmitReceive_DMA(&hspi1, (uint8_t*) DataIn, DataOut, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			RSPI_SpiStatus = -4;
		}
	} else {
		RSPI_SpiStatus = -3;
	}

	if (RSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return RSPI_SpiStatus;
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
int RSPI_WriteData(const uint8_t *Data, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	RSPI_SpiStatus = FALSE;
	hal_status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*) Data, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			RSPI_SpiStatus = -4;
		}
	} else {
		RSPI_SpiStatus = -3;
	}

	if (RSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return RSPI_SpiStatus;
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
int RSPI_ReadData(const uint8_t *Data, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	RSPI_SpiStatus = FALSE;
	hal_status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*) Data, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			RSPI_SpiStatus = -4;
		}
	} else {
		RSPI_SpiStatus = -3;
	}

	if (RSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return RSPI_SpiStatus;
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
int RSPI_Write(uint8_t Value) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;
	uint8_t data = Value;

	RSPI_SpiStatus = FALSE;
	hal_status = HAL_SPI_Transmit_DMA(&hspi1, &data, 1);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(RSPI_SemaphoreID, 1000);
		if (os_status != osOK) {
			RSPI_SpiStatus = -4;
		}
	} else {
		RSPI_SpiStatus = -3;
	}

	if (RSPI_SpiStatus != 0) {
		Error_Handler();
	}
	return RSPI_SpiStatus;
}


// Callbacks
// *********

// callback are handled by DSPI module.
