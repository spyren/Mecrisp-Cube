/**
 *  @brief
 *      Serial Peripheral Interface (SPI) for external devices.
 *
 *		The spi2 interface is used for the external devices.
 *  @file
 *      ext_spi.c
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
#include "ext_spi.h"
#include "myassert.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************
volatile uint8_t EXTSPI_Error = FALSE;


// RTOS resources
// **************

static osMutexId_t EXTSPI_MutexID;
static const osMutexAttr_t EXTSPI_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

osSemaphoreId_t EXTSPI_SemaphoreID;


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
void EXTSPI_init(void) {
	EXTSPI_MutexID = osMutexNew(&EXTSPI_MutexAttr);
	ASSERT_fatal(EXTSPI_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	EXTSPI_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(EXTSPI_SemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
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
void EXTSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;

	// only one thread is allowed to use the SPI
	osMutexAcquire(EXTSPI_MutexID, osWaitForever);

	EXTSPI_Error = FALSE;
	hal_status = HAL_SPI_TransmitReceive_DMA(&hspi2, (uint8_t*) DataIn, DataOut, DataLength);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(EXTSPI_SemaphoreID, 1000);
		if (EXTSPI_Error || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	osMutexRelease(EXTSPI_MutexID);
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
void EXTSPI_Write(uint8_t Value) {
	HAL_StatusTypeDef hal_status = HAL_OK;
	osStatus_t os_status = osOK;
	uint8_t data;

	// only one thread is allowed to use the SPI
	osMutexAcquire(EXTSPI_MutexID, osWaitForever);

	EXTSPI_Error = FALSE;
	hal_status = HAL_SPI_TransmitReceive_DMA(&hspi2, (uint8_t*) &Value, &data, 1);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(EXTSPI_SemaphoreID, 1000);
		if (EXTSPI_Error || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	osMutexRelease(EXTSPI_MutexID);
}



// Callbacks
// *********

// the callbacks are handled by the fd_spi.c
