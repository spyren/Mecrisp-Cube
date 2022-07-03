/**
 *  @brief
 *      Quad Serial Peripheral Interface (QSPI) for the serial flash.
 *
 *		The hqspi interface is used for the serial NOR flash.
 *		External NOR Flash 16 Mb, e.g.:
 *		  - Winbond W25Q128JV
 *		  - Micron N25Q128A
 *		  - Cypress S25FL128S
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
#include "myassert.h"
#include "n25q128a.h"
#include "quadspi.h"


// Private function prototypes
// ***************************
static uint8_t reset_chip();
static uint8_t config_chip(void);
static uint8_t write_enable(void);
static uint8_t wait_mem_ready(void);

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

static osSemaphoreId_t FDSPI_DataSemaphoreID;
static osSemaphoreId_t FDSPI_StatusSemaphoreID;
static osSemaphoreId_t FDSPI_CommandSemaphoreID;



// Hardware resources
// ******************

extern QSPI_HandleTypeDef hqspi;
extern MDMA_HandleTypeDef hmdma_quadspi_fifo_th;


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

	FDSPI_DataSemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(FDSPI_DataSemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
	FDSPI_CommandSemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(FDSPI_CommandSemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());
	FDSPI_StatusSemaphoreID = osSemaphoreNew(1, 0, NULL);
	ASSERT_fatal(FDSPI_StatusSemaphoreID != NULL, ASSERT_SEMAPHORE_CREATION, __get_PC());

    reset_chip();
    osDelay(1);
    write_enable();
    config_chip();
}


/**
  * @brief
  *     QSPI Write byte(s) to the flash device
  *
  *     Using DMA. RTOS blocking till finished.
  *     Page size for 16 Mb devices is 256 bytes.
  * @param[in]
  *     pData: Pointer to data buffer to write
  * @param[in]
  * 	WriteAddr: flash memory address
  * @param[in]
  *     DataLength: number of bytes to write
  * @retval
  *     None
  */
int FDSPI_writeData(uint8_t* pData, uint32_t WriteAddr, uint32_t Size) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
	QSPI_CommandTypeDef s_command;
	uint32_t end_addr, current_size, current_addr;

	// only one thread is allowed to use the QSPI
	osMutexAcquire(FDSPI_MutexID, osWaitForever);

	/* Calculation of the size between the write address and the end of the page */
	current_size = N25Q128A_PAGE_SIZE - (WriteAddr % N25Q128A_PAGE_SIZE);

	/* Check if the size of the data is less than the remaining place in the page */
	if (current_size > Size) {
		current_size = Size;
	}

	/* Initialize the address variables */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;

	/* Initialize the program command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = EXT_QUAD_IN_FAST_PROG_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* Perform the write page by page */
	do {
		s_command.Address = current_addr;
		s_command.NbData  = current_size;

		/* Enable write operations */
		write_enable();

		/* Configure the command */
		SpiError = FALSE;
		hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
		if (hal_status != HAL_OK) {
			Error_Handler();
		}

//		if (hal_status == HAL_OK) {
//			// blocked till command is finished
//			os_status = osSemaphoreAcquire(FDSPI_CommandSemaphoreID, 1000);
//			if (SpiError || (os_status != osOK)) {
//				Error_Handler();
//			}
//		} else {
//			Error_Handler();
//		}

		/* Transmission of the data */
		SpiError = FALSE;
		hal_status = HAL_QSPI_Transmit_DMA(&hqspi, pData);
		if (hal_status == HAL_OK) {
			// blocked till command write is finished
			os_status = osSemaphoreAcquire(FDSPI_DataSemaphoreID, 1000);
			if (SpiError || (os_status != osOK)) {
				Error_Handler();
			}
		} else {
			Error_Handler();
		}

		// wait till page is written
		wait_mem_ready();

		/* Update the address and size variables for next page programming */
		current_addr += current_size;
		pData += current_size;
		current_size = ((current_addr + N25Q128A_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : N25Q128A_PAGE_SIZE;
	} while (current_addr < end_addr);

	osMutexRelease(FDSPI_MutexID);
	return HAL_OK;
}


/**
  * @brief
  *     QSPI read byte(s) from the flash device
  *
  *     Using DMA. RTOS blocking till finished.
  * @param[in]
  *     pData: Pointer to data buffer to write
  * @param[in]
  * 	WriteAddr: flash memory address
  * @param[in]
  *     DataLength: number of bytes to write
  * @retval
  *     None
  */
int FDSPI_readData(uint8_t* pData, uint32_t ReadAddr, uint32_t Size) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
	QSPI_CommandTypeDef s_command;

	// only one thread is allowed to use the QSPI
	osMutexAcquire(FDSPI_MutexID, osWaitForever);

	/* Initialize the read command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = QUAD_INOUT_FAST_READ_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_4_LINES;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = ReadAddr;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_4_LINES;
	s_command.DummyCycles       = N25Q128A_DUMMY_CYCLES_READ_QUAD;
	s_command.NbData            = Size;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* Configure the command */
	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status != HAL_OK) {
		Error_Handler();
	}

	/* Set S# timing for Read command */
	MODIFY_REG(hqspi.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_3_CYCLE);

	/* Reception of the data */
	SpiError = FALSE;
	hal_status = HAL_QSPI_Receive_DMA(&hqspi, pData);
	if (hal_status == HAL_OK) {
		// blocked till read is finished
		os_status = osSemaphoreAcquire(FDSPI_DataSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	/* Restore S# timing for nonRead commands */
	MODIFY_REG(hqspi.Instance->DCR, QUADSPI_DCR_CSHT, QSPI_CS_HIGH_TIME_6_CYCLE);

	osMutexRelease(FDSPI_MutexID);
	return HAL_OK;
}


int FDSPI_eraseChip(void) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
	QSPI_CommandTypeDef s_command;

	// only one thread is allowed to use the QSPI
	osMutexAcquire(FDSPI_MutexID, osWaitForever);

	write_enable();

	/* Erasing Sequence --------------------------------- */
	s_command.Instruction = BULK_ERASE_CMD;
	s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
	s_command.AddressSize = QSPI_ADDRESS_24_BITS;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
	s_command.AddressMode = QSPI_ADDRESS_NONE;
	s_command.Address = 0;
	s_command.DataMode = QSPI_DATA_NONE;
	s_command.DummyCycles = 0;

	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status == HAL_OK) {
		// blocked till command is finished
		os_status = osSemaphoreAcquire(FDSPI_CommandSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	wait_mem_ready();

	osMutexRelease(FDSPI_MutexID);
	return HAL_OK;
}


int FDSPI_eraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
    QSPI_CommandTypeDef s_command;

	// only one thread is allowed to use the QSPI
	osMutexAcquire(FDSPI_MutexID, osWaitForever);

    EraseStartAddress = EraseStartAddress
                        - EraseStartAddress % N25Q128A_SECTOR_SIZE;

    /* Erasing Sequence -------------------------------------------------- */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.Instruction = SECTOR_ERASE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_1_LINE;

    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;

    while (EraseEndAddress >= EraseStartAddress)
    {
        s_command.Address = (EraseStartAddress & 0x0FFFFFFF);

        write_enable();

    	SpiError = FALSE;
    	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
    	if (hal_status == HAL_OK) {
    		// blocked till command is finished
    		os_status = osSemaphoreAcquire(FDSPI_CommandSemaphoreID, 1000);
    		if (SpiError || (os_status != osOK)) {
    			Error_Handler();
    		}
    	} else {
    		Error_Handler();
    	}

        EraseStartAddress += N25Q128A_SECTOR_SIZE;

        wait_mem_ready();
    }

	osMutexRelease(FDSPI_MutexID);
    return HAL_OK;
}


int FDSPI_eraseBlock(uint32_t BlockAddress) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
	QSPI_CommandTypeDef s_command;

	// only one thread is allowed to use the QSPI
	osMutexAcquire(FDSPI_MutexID, osWaitForever);

	/* Initialize the erase command */
	s_command.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
	s_command.Instruction       = SUBSECTOR_ERASE_CMD;
	s_command.AddressMode       = QSPI_ADDRESS_1_LINE;
	s_command.AddressSize       = QSPI_ADDRESS_24_BITS;
	s_command.Address           = BlockAddress;
	s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
	s_command.DataMode          = QSPI_DATA_NONE;
	s_command.DummyCycles       = 0;
	s_command.DdrMode           = QSPI_DDR_MODE_DISABLE;
	s_command.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
	s_command.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

	/* Enable write operations */
	if (write_enable() != HAL_OK) {
		return HAL_ERROR;
	}

	/* Send the command */
	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status == HAL_OK) {
		// blocked till command is finished
		os_status = osSemaphoreAcquire(FDSPI_CommandSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	/* Configure automatic polling mode to wait for end of erase */
	wait_mem_ready();

	osMutexRelease(FDSPI_MutexID);
	return HAL_OK;
}


// Private Functions
// *****************

static uint8_t reset_chip() {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
    QSPI_CommandTypeDef s_command;

    /* Erasing Sequence -------------------------------------------------- */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.Instruction = RESET_ENABLE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.Address = 0;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;

	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status == HAL_OK) {
		// blocked till command is finished
		os_status = osSemaphoreAcquire(FDSPI_CommandSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

    osDelay(1);

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AddressSize = QSPI_ADDRESS_24_BITS;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.Instruction = RESET_MEMORY_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.Address = 0;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;

	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status == HAL_OK) {
		// blocked till command is finished
		os_status = osSemaphoreAcquire(FDSPI_CommandSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	return HAL_OK;
}


/*Enable quad mode and set dummy cycles count*/
static uint8_t config_chip(void) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
    QSPI_CommandTypeDef s_command;
    uint8_t reg;

    /*read configuration register*/
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = READ_VOL_CFG_REG_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.NbData = 1;

	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status != HAL_OK) {
		Error_Handler();
	}

	SpiError = FALSE;
	hal_status = HAL_QSPI_Receive_DMA(&hqspi, &reg);
	if (hal_status == HAL_OK) {
		// blocked till read is finished
		os_status = osSemaphoreAcquire(FDSPI_DataSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

	write_enable();

    /*set dummy cycles*/
    MODIFY_REG(reg, 0xF0, (N25Q128A_DUMMY_CYCLES_READ_QUAD << POSITION_VAL(0xF0)));

    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    s_command.Instruction = WRITE_VOL_CFG_REG_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.NbData = 1;

	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status != HAL_OK) {
		Error_Handler();
	}

	SpiError = FALSE;
	hal_status = HAL_QSPI_Transmit_DMA(&hqspi, &reg);
	if (hal_status == HAL_OK) {
		// blocked till command write is finished
		os_status = osSemaphoreAcquire(FDSPI_DataSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

    return HAL_OK;
}


static uint8_t write_enable(void) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Enable write operations ------------------------------------------ */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = WRITE_ENABLE_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_NONE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

	SpiError = FALSE;
	hal_status = HAL_QSPI_Command_IT(&hqspi, &s_command);
	if (hal_status == HAL_OK) {
		// blocked till command is finished
		os_status = osSemaphoreAcquire(FDSPI_CommandSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

    /* Configure automatic polling mode to wait for write enabling ---- */
    s_config.Match = 0x02;
    s_config.Mask = 0x02;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    s_command.Instruction = READ_STATUS_REG_CMD;
    s_command.DataMode = QSPI_DATA_1_LINE;

    SpiError = FALSE;
    hal_status = HAL_QSPI_AutoPolling_IT(&hqspi, &s_command, &s_config);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(FDSPI_StatusSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

    return HAL_OK;
}


static uint8_t wait_mem_ready(void) {
	HAL_StatusTypeDef hal_status;
	osStatus_t os_status;
    QSPI_CommandTypeDef s_command;
    QSPI_AutoPollingTypeDef s_config;

    /* Configure automatic polling mode to wait for memory ready ------ */
    s_command.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    s_command.Instruction = READ_STATUS_REG_CMD;
    s_command.AddressMode = QSPI_ADDRESS_NONE;
    s_command.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    s_command.DataMode = QSPI_DATA_1_LINE;
    s_command.DummyCycles = 0;
    s_command.DdrMode = QSPI_DDR_MODE_DISABLE;
    s_command.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    s_command.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    s_config.Match = 0x00;
    s_config.Mask = N25Q128A_SR_WIP;
    s_config.MatchMode = QSPI_MATCH_MODE_AND;
    s_config.StatusBytesSize = 1;
    s_config.Interval = 0x10;
    s_config.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;

    SpiError = FALSE;
    hal_status = HAL_QSPI_AutoPolling_IT(&hqspi, &s_command, &s_config);
	if (hal_status == HAL_OK) {
		// blocked till read/write is finished
		os_status = osSemaphoreAcquire(FDSPI_StatusSemaphoreID, 1000);
		if (SpiError || (os_status != osOK)) {
			Error_Handler();
		}
	} else {
		Error_Handler();
	}

    return HAL_OK;
}


// Callbacks
// *********

/**
  * @brief  Rx Transfer completed callback.
  * @param  hqspi QSPI handle
  * @retval None
  */
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi) {
	SpiError = FALSE;
	osSemaphoreRelease(FDSPI_DataSemaphoreID);
}


/**
  * @brief  Rx Transfer completed callback.
  * @param  hqspi QSPI handle
  * @retval None
  */
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi) {
	SpiError = FALSE;
	osSemaphoreRelease(FDSPI_DataSemaphoreID);
}


/**
  * @brief  Command completed callback.
  * @param  hqspi QSPI handle
  * @retval None
  */
void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi) {
	SpiError = FALSE;
	osSemaphoreRelease(FDSPI_CommandSemaphoreID);
}


/**
  * @brief  Status Match callback.
  * @param  hqspi QSPI handle
  * @retval None
  */
void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi) {
	SpiError = FALSE;
	osSemaphoreRelease(FDSPI_StatusSemaphoreID);
}


/**
  * @brief  QSPI transfer error callback.
  * @param  hqspi QSPI handle
  * @retval None
  */
void HAL_QSPI_ErrorCallback(QSPI_HandleTypeDef *hqspi) {
	SpiError = TRUE;
	osSemaphoreRelease(FDSPI_DataSemaphoreID);
}


