/**
 *  @brief
 *      Secure Digital Memory Card block read and write.
 *
 *      SDIO is used as interface.
 *  @file
 *      sd.c
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
#include "stm32h7xx_hal_sd.h"
#include "sd.h"
#include "myassert.h"


// Defines
// *******

// Private typedefs
// ****************


// Private function prototypes
// ***************************

// Global Variables
// ****************

// RTOS resources
// **************

static osMutexId_t SD_MutexID;
static const osMutexAttr_t SD_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t SD_RxSemaphoreID;
static osSemaphoreId_t SD_TxSemaphoreID;


// Hardware resources
// ******************


// Private Variables
// *****************
static volatile uint8_t SdError = FALSE;
static HAL_SD_CardInfoTypeDef HalCardInfo;
static SD_CardInfo CardInfo;

__IO uint8_t SdStatus = SD_NOT_PRESENT;

/* flag_SDHC :
      0 :  Standard capacity
      1 : High capacity
*/
uint16_t flag_SDHC = 0;

int sd_size = 0; // number of blocks

//uint8_t scratch_block[SD_BLOCK_SIZE];

extern SD_HandleTypeDef hsd1;

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the Secure Digital Memory Card.
 *  @return
 *      None
 */
void SD_init(void) {
	SD_MutexID = osMutexNew(&SD_MutexAttr);
	ASSERT_fatal(SD_MutexID != NULL, ASSERT_MUTEX_CREATION, __get_PC());

	SD_RxSemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(SD_RxSemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());

	SD_TxSemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(SD_TxSemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());

//	MX_SDIO_SD_Init();
}


/**
 *  @brief
 *      Initializes the Secure Digital Memory Card and get the size in KiB.
 *  @return
 *      None
 */
void SD_getSize(void) {
	if (SD_GetCardInfo(&CardInfo) != SD_ERROR) {
		sd_size = CardInfo.CardCapacity / 1024;
	}
}

/**
 *  @brief
 *      Get the SD size in KiB.
 *  @return
 *      size in KiB
 */
int SD_getBlocks(void) {
	return sd_size;
}

/**
  * @brief
  *     Returns information about specific card.
  * @param
  *     pCardInfo: Pointer to a SD_CardInfo structure that contains all SD
  *     card information.
  * @retval
  *     The SD Response:
  *         - MSD_ERROR: Sequence failed
  *         - MSD_OK: Sequence succeed
  */
uint8_t SD_GetCardInfo(SD_CardInfo *pCardInfo) {
	/* Get SD card Information */

	if (HAL_SD_GetCardInfo(&hsd1, &HalCardInfo) == HAL_OK) {
	//	pCardInfo->Cid.ManufacturerID 			= HalCardInfo.CardType;		// card Type
	//	pCardInfo->Csd 			= HalCardInfo.CardVersion;	// card version
	//	pCardInfo->Csd 			= HalCardInfo.Class;		// class of the card class
	//	pCardInfo-> 			= HalCardInfo.RelCardAdd;	// Relative Card Address
		pCardInfo->CardCapacity	= HalCardInfo.BlockNbr;  	// Card Capacity in blocks
		pCardInfo->CardBlockSize= HalCardInfo.BlockSize; 	// one block size in bytes
		pCardInfo->LogBlockNbr	= HalCardInfo.LogBlockNbr;	// Card logical Capacity in blocks
		pCardInfo->LogBlockSize	= HalCardInfo.LogBlockSize; // logical block size in bytes

		return SD_OK;
	} else {
		return  SD_ERROR;
	}
}


/**
  * @brief
  *     Reads block(s) from a specified address in the SD card.
  * @param
  *     pData: Pointer to the buffer that will contain the data to transmit
  * @param
  *     ReadAddr: Address from where data is to be read. The address is counted
  *                   in blocks of 512bytes
  * @param
  *     NumOfBlocks: Number of SD blocks to read
  * @retval
  *     SD status
  */
uint8_t SD_ReadBlocks(uint8_t *pData, uint32_t ReadAddr, uint32_t NumOfBlocks) {
	uint8_t retr = SD_ERROR;
	osStatus_t os_status = osOK;

	// only one thread is allowed to use the SD
	osMutexAcquire(SD_MutexID, osWaitForever);
	SdError = FALSE;
	if (osSemaphoreGetCount(SD_RxSemaphoreID) > 0) {
		// reset semaphore
		osSemaphoreAcquire(SD_RxSemaphoreID, 0);
	}
	if (HAL_SD_ReadBlocks_DMA(&hsd1, pData, ReadAddr, NumOfBlocks) == HAL_OK) {
		// blocked till read is finished or timeout
		os_status = osSemaphoreAcquire(SD_RxSemaphoreID, 5000);
		if (SdError || (os_status != osOK)) {
			Error_Handler();
		} else {
			osDelay(2);
			retr = SD_OK;
		}
	} else {
		Error_Handler();
	}
	osMutexRelease(SD_MutexID);

	return retr;
}


/**
  * @brief
  *     Writes block(s) to a specified address in the SD card.
  * @param
  *     pData: Pointer to the buffer that will contain the data to transmit
  * @param
  *     WriteAddr: Address from where data is to be written. The address is counted
  *                   in blocks of 512bytes
  * @param
  *     NumOfBlocks: Number of SD blocks to write
  * @retval
  *     SD status
  */
uint8_t SD_WriteBlocks(uint8_t *pData, uint32_t WriteAddr, uint32_t NumOfBlocks) {
	uint8_t retr = SD_ERROR;
	osStatus_t os_status = osOK;

	// only one thread is allowed to use the SD
	osMutexAcquire(SD_MutexID, osWaitForever);
	SdError = FALSE;
	if (osSemaphoreGetCount(SD_TxSemaphoreID) > 0) {
		// reset semaphore
		osSemaphoreAcquire(SD_TxSemaphoreID, 0);
	}
	if (HAL_SD_WriteBlocks_DMA(&hsd1, pData, WriteAddr, NumOfBlocks) == HAL_OK) {
		// blocked till read is finished or timeout
		os_status = osSemaphoreAcquire(SD_TxSemaphoreID, 5000);
		if (SdError || (os_status != osOK)) {
			Error_Handler();
		} else {
			osDelay(10);
			while ( HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER ) {
				osDelay(5);
			}
			retr = SD_OK;
		}
	} else {
		Error_Handler();
	}
	osMutexRelease(SD_MutexID);

	return retr;
}


/**
  * @brief
  *     Erases the specified memory area of the given SD card.
  * @param
  *     StartAddr: Start address in Blocks (Size of a block is 512bytes)
  * @param
  *     EndAddr: End address in Blocks (Size of a block is 512bytes)
  * @retval
  *     SD status
  */
uint8_t SD_Erase(uint32_t StartAddr, uint32_t EndAddr) {
	uint8_t retr = SD_ERROR;

	// only one thread is allowed to use the SD
	osMutexAcquire(SD_MutexID, osWaitForever);
	if (HAL_SD_Erase(&hsd1, StartAddr, EndAddr) == HAL_OK) {
		osDelay(10);
		while (HAL_SD_GetCardState(&hsd1) != HAL_SD_CARD_TRANSFER ) {
			osDelay(5);
		}
		retr = SD_OK;
	} else {
		Error_Handler();
	}
	osMutexRelease(SD_MutexID);

	return retr;
}


/**
  * @brief  Gets the current SD card data status.
  * @retval Data transfer state.
  *          This value can be one of the following values:
  *            @arg  SD_TRANSFER_OK: No data transfer is acting
  *            @arg  SD_TRANSFER_BUSY: Data transfer is acting
  */
//uint8_t SD_GetCardState(void) {
//  return((HAL_SD_GetCardState(&hsd1) == HAL_SD_CARD_TRANSFER ) ? SD_OK : SD_ERROR);
//}


// Private Functions
// *****************



// Callbacks
// *********

/**
  * @brief Tx Transfer completed callbacks
  * @param hsd: Pointer to SD handle
  * @retval None
  */
void HAL_SD_TxCpltCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);

  osSemaphoreRelease(SD_TxSemaphoreID);
}

/**
  * @brief Rx Transfer completed callbacks
  * @param hsd: Pointer SD handle
  * @retval None
  */
void HAL_SD_RxCpltCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);

  osSemaphoreRelease(SD_RxSemaphoreID);
}

/**
  * @brief SD error callbacks
  * @param hsd: Pointer SD handle
  * @retval None
  */
void HAL_SD_ErrorCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);

  SdError = TRUE;
  osSemaphoreRelease(SD_RxSemaphoreID);
  osSemaphoreRelease(SD_TxSemaphoreID);
}

/**
  * @brief SD Abort callbacks
  * @param hsd: Pointer SD handle
  * @retval None
  */
void HAL_SD_AbortCallback(SD_HandleTypeDef *hsd)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hsd);

  SdError = TRUE;
  osSemaphoreRelease(SD_RxSemaphoreID);
  osSemaphoreRelease(SD_TxSemaphoreID);
}

