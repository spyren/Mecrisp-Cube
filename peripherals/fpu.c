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


