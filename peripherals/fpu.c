/**
 *  @brief
 *      Support for the FPU (Floating Point Unit)
 *
 *		Most Cortex M4 CPUs have a single precission FPU,
 *		Cortex M7 CPUs have a double precission FPU.
 *		IEEE 754 binary32
 *		https://forth-standard.org/standard/float
 *  @file
 *      fpu.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2022-11-01
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
#include <float.h>
#include <math.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "fpu.h"

// Private function prototypes
// ***************************

// Global Variables
// ****************

// Hardware resources
// ******************

// RTOS resources
// **************
static osMutexId_t fpu_MutexID;
static const osMutexAttr_t fpu_MutexAttr = {
NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
		};

// Private Variables
// *****************
static char fpu_string[FPU_MAX_STR];

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the FPU.
 *  @return
 *      None
 */
void FPU_init(void) {
	fpu_MutexID = osMutexNew(&fpu_MutexAttr);
	if (fpu_MutexID == NULL) {
		Error_Handler();
	}
//	__set_FPSCR();
}

/**
 *  @brief
 *      Macros can't be called from assembler
 *  @return
 *      None
 */
void FPU_setFPSCR(uint32_t fpscr) {
	__set_FPSCR(fpscr);
}

/**
 *  @brief
 *      Macros can't be called from assembler
 *  @return
 *      None
 */
uint32_t FPU_getFPSCR(void) {
	return __get_FPSCR();
}

/**
 *  @brief
 *      Convert ASCII string to floating-point number
 *  @return
 *      float
 */
float FPU_str2f(char *str, int len) {
	memcpy(fpu_string, str, len);
	return strtof(fpu_string, NULL);
}


/**
 *  @brief
 *      Convert ASCII string to floating-point number
 *  @return
 *      float
 */
float FPU_sin(float arg) {
	return sinf(arg);
}

///**
// *  @brief
// *      Display, with a trailing space, the top number on the floating-point stack using fixed-point notation
// *  @param[in]
// *      forth_stack   TOS (lower word) and SPS (higher word)
// *  @param[in]
// *      number
// *  @return
// *      TOS (lower word) and SPS (higher word)
// */
//uint64_t FPU_fdot(uint64_t forth_stack, float number) {
//	uint64_t stack;
//	stack = forth_stack;
//
//	stack = FS_type(stack, (uint8_t*)fpu_string, strlen(fpu_string));
//
//	// only one thread is allowed to use fpu string
//	osMutexAcquire(fpu_MutexID, osWaitForever);
//
//	sprintf(fpu_string, "%f ", number);
//
//	osMutexRelease(fpu_MutexID);
//
//	return stack;
//}

