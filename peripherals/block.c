/**
 *  @brief
 *      Blocks for Secure Digital Memory Card.
 *
 *  @file
 *      block.c
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
#include "block.h"
#include "sd.h"
#include "fd.h"
#include "assert.h"


// Defines
// *******



// Private function prototypes
// ***************************

// SD raw block functions
static int get_block(int block_number, int buffer_index);
static int save_buffer(int buffer_index);
static void init_block(int block_number, int buffer_index);


// Global Variables
// ****************

// RTOS resources
// **************

static osMutexId_t BLOCK_MutexID;
static const osMutexAttr_t BLOCK_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};


// Hardware resources
// ******************


// Private Variables
// *****************

block_buffer_t BLOCK_Buffers[BLOCK_BUFFER_COUNT];

extern uint32_t DriveNumber;



// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the Secure Digital Memory Card.
 *  @return
 *      None
 */
void BLOCK_init(void) {
	BLOCK_MutexID = osMutexNew(&BLOCK_MutexAttr);
	ASSERT_fatal(BLOCK_MutexID != NULL, ASSERT_MUTEX_CREATION, 0);

	BLOCK_emptyBuffers();
}


/**
 *  @brief
 *      Empties all buffers.
 *
 *      empty-buffers ( -- ) Marks all block buffers as empty
 *  @return
 *      None
 */
void BLOCK_emptyBuffers(void) {
	int i;

	// only one thread is allowed to use blocks
	osMutexAcquire(BLOCK_MutexID, osWaitForever);

	for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
		BLOCK_Buffers[i].BlockNumber = -1;
		BLOCK_Buffers[i].Current = FALSE;
		BLOCK_Buffers[i].Updated = FALSE;
	}

	osMutexRelease(BLOCK_MutexID);
}


/**
 *  @brief
 *      Makes the most recent block dirty (updated).
 *
 *      update	( -- )	Mark most recent block as updated
 *  @return
 *      none
 */
void BLOCK_update(void) {
	int i;

	// only one thread is allowed to use blocks
	osMutexAcquire(BLOCK_MutexID, osWaitForever);

	for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
		if (BLOCK_Buffers[i].Current) {
			BLOCK_Buffers[i].Updated = TRUE;
			break;
		}
	}

	osMutexRelease(BLOCK_MutexID);
}


/**
 *  @brief
 *      Gets a block from raw (unformatted) SD.
 *
 *      block ( n -- addr )	Return address of buffer for block n
 *      If a block buffer is assigned for block u, return its start address, a-addr.
 *      Otherwise, assign a block buffer for block u (if the assigned block buffer
 *      has been updated, transfer the contents to mass storage), read the block i
 *      to the block buffer and return its start address, a-addr.
 *  @param[in]
 *  	block_number
 *  @return
 *      Buffer Address
 */
uint8_t *BLOCK_get(int block_number) {
	int i;
	uint8_t* buffer_p = NULL;

	// only one thread is allowed to use blocks
	osMutexAcquire(BLOCK_MutexID, osWaitForever);

	// there will be a new current block
	for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
		BLOCK_Buffers[i].Current = FALSE;
	}

	// already assigned?
	for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
		if (BLOCK_Buffers[i].BlockNumber == block_number) {
			// the block is already in the buffer -> make current
			BLOCK_Buffers[i].Current = TRUE;
			buffer_p = &BLOCK_Buffers[i].Data[0];
			break;
		}
	}

	if (buffer_p == NULL) {
		// is there an unassigned (empty) buffer?
		for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
			if (BLOCK_Buffers[i].BlockNumber < 0) {
				// buffer is unassigned (empty)
				if (get_block(block_number, i) == SD_OK ) {
					buffer_p = &BLOCK_Buffers[i].Data[0];
				} else {
					buffer_p = NULL;
				}
				break;
			}
		}
	}

	if (buffer_p == NULL) {
		// take the first not current buffer
		for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
			if (! BLOCK_Buffers[i].Current) {
				// buffer is not current
				if (BLOCK_Buffers[i].Updated) {
					// Buffer is updated -> save buffer to SD
					save_buffer(i);
				}
				if (get_block(block_number, i) == SD_OK ) {
					buffer_p = &BLOCK_Buffers[i].Data[0];
				} else {
					buffer_p = NULL;
				}
				break;
			}
		}
	}

	osMutexRelease(BLOCK_MutexID);

	return buffer_p;
}


/**
 *  @brief
 *      Assigns a block. No file I/O.
 *
 *      buffer ( n -- addr )	Return address of buffer for block n
 *  @param[in]
 *  	block_number
 *  @return
 *      Buffer Address
 */
uint8_t *BLOCK_assign(int block_number) {
	int i;
	int j;
	uint8_t* buffer_p = NULL;

	// only one thread is allowed to use blocks
	osMutexAcquire(BLOCK_MutexID, osWaitForever);

	// already assigned?
	for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
		if (BLOCK_Buffers[i].BlockNumber == block_number) {
			// the block is already in the buffer -> make current
			for (j=0; j<BLOCK_BUFFER_COUNT; j++) {
				BLOCK_Buffers[i].Current = FALSE;
			}
			BLOCK_Buffers[i].Current = TRUE;
			buffer_p = &BLOCK_Buffers[i].Data[0];
			break;
		}
	}

	if (buffer_p == NULL) {
		// is there an unassigned (empty) buffer?
		for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
			if (BLOCK_Buffers[i].BlockNumber < 0) {
				// buffer is unassigned (empty)
				init_block(block_number, i);
				buffer_p = &BLOCK_Buffers[i].Data[0];
				break;
			}
		}
	}

	if (buffer_p == NULL) {
		// take the first not current buffer
		for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
			if (! BLOCK_Buffers[i].Current) {
				// buffer is not current
				if (BLOCK_Buffers[i].Updated) {
					// Buffer is updated -> save buffer to SD
					save_buffer(i);
				}
				// fill the block with spaces
				init_block(block_number, i);
				buffer_p = &BLOCK_Buffers[i].Data[0];
				break;
			}
		}
	}

	osMutexRelease(BLOCK_MutexID);

	return buffer_p;
}


/**
 *  @brief
 *      Saves all updated buffers to SD.
 *
 *      save-buffers ( -- ) Transfer the contents of each updated block buffer
 *      to mass storage, then mark all block buffers as assigned-clean.
 *  @return
 *      none
 */
void BLOCK_saveBuffers(void) {
	int i;

	// only one thread is allowed to use blocks
	osMutexAcquire(BLOCK_MutexID, osWaitForever);

	for (i=0; i<BLOCK_BUFFER_COUNT; i++) {
		if (BLOCK_Buffers[i].Updated) {
			if (save_buffer(i) == SD_OK) {
				BLOCK_Buffers[i].Updated = FALSE;
			}
		}
	}

	osMutexRelease(BLOCK_MutexID);
}


/**
 *  @brief
 *      Saves and empties all updated buffers to SD.
 *
 *      flush ( -- ) save-buffers empty-buffers
 *  @return
 *      none
 */
void BLOCK_flushBuffers(void) {
	BLOCK_saveBuffers();
	BLOCK_emptyBuffers();
}


// Private Functions
// *****************

/**
 *  @brief
 *      Gets a block from raw (unformatted) SD.
 *
 *      The block consists of 2 SD blocks.
 *  @param[in]
 *  	block_number	raw block number.
 *  @param[in]
 *  	buffer_index	Buffer array index.
 *  @return
 *      none
 */
static int get_block(int block_number, int buffer_index) {
	int ret_value;
	if (DriveNumber == 0) {
		ret_value= FD_ReadBlocks(&BLOCK_Buffers[buffer_index].Data[0], block_number*2, 2);
	} else if (DriveNumber == 1) {
		ret_value= SD_ReadBlocks(&BLOCK_Buffers[buffer_index].Data[0], block_number*2, 2);
	} else {
		ret_value = SD_ERROR;
	}
	if (ret_value == SD_OK) {
		BLOCK_Buffers[buffer_index].BlockNumber = block_number;
		BLOCK_Buffers[buffer_index].Current = TRUE;
		BLOCK_Buffers[buffer_index].Updated = FALSE;
	}
	return ret_value;
}


/**
 *  @brief
 *      Inits a block with spaces.
 *
 *  @param[in]
 *  	block_number	raw block number.
 *  @param[in]
 *  	buffer_index	Buffer array index.
 *  @return
 *      none
 */
static void init_block(int block_number, int buffer_index) {
	memset(&BLOCK_Buffers[buffer_index].Data[0], ' ', BLOCK_BUFFER_SIZE);
	BLOCK_Buffers[buffer_index].BlockNumber = block_number;
	BLOCK_Buffers[buffer_index].Current = TRUE;
	BLOCK_Buffers[buffer_index].Updated = FALSE;
}


/**
 *  @brief
 *      Saves a buffer to the SD.
 *
 *      The block consists of 2 SD blocks.
 *  @param[in]
 *  	buffer_index	Buffer array index.
 *  @return
 *      none
 */
static int save_buffer(int buffer_index) {
	int ret_value;
	if (DriveNumber == 0) {
		ret_value = FD_WriteBlocks(&BLOCK_Buffers[buffer_index].Data[0],
				BLOCK_Buffers[buffer_index].BlockNumber*2, 2);
	} else if (DriveNumber == 1) {
		ret_value = SD_WriteBlocks(&BLOCK_Buffers[buffer_index].Data[0],
				BLOCK_Buffers[buffer_index].BlockNumber*2, 2);
	} else {
		ret_value = SD_ERROR;
	}
	if (ret_value == SD_OK) {
		BLOCK_Buffers[buffer_index].Updated = FALSE;
	}
	return ret_value;
}



