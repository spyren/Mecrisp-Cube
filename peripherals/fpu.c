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
#include <ctype.h>


// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "fpu.h"

#if FPU_IP == 1

// Private function prototypes
// ***************************
float pow10_to_f(int exp);

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
 *      Convert ASCII string to single precision floating-point number
 *
 *      				:= 	<significand>[<exponent>]
 *		<significand> 	:= 	[<sign>]{<digits>[.<digits0>] | .<digits> }
 *			<exponent> 	:= 	{<marker><digits0> | <unit-prefix> }
 * 			<marker> 	:= 	{<e-form> | <sign-form>}
 *			<e-form> 	:= 	<e-char>[<sign-form>]
 *		<sign-form> 	:= 	{ + | - }
 *			<e-char> 	:= 	{ D | d | E | e }
 *		<unit-prefix> 	:=  { P | T | G | M | k | e | m | u | n | p | f | a | }
 *  @return
 *      float, NAN for invalid numbers
 */
float FPU_str2f(char *str, int len) {
	int exponent = 0;
	int sign = 1;
	int integer = 0;
	float fract = 0.0f;
	float number = 0.0f;
	int unit_prefix;
	int i, j, k;
	char *s;

	s = str;

	// eat white spaces
	// eat only at the beginning/end?
	j = 0;
	for (i=0; i<len; i++) {
		if (!isblank(s[i])) {
			fpu_string[j++] = s[i];
		}
	}
	fpu_string[j] = 0;

	// sign
	if (fpu_string[0] == '+') {
		// remove +
		s = fpu_string + 1;
	} else if (fpu_string[0] == '-') {
		// remove -
		s = fpu_string + 1;
		sign = -1;
	} else {
		s = fpu_string;
	}

	// check for unit-prefix at the end
	unit_prefix = TRUE;
	switch (s[strlen(s)-1]) {
	case 'P':
		exponent = 15;
		break;
	case 'T':
		exponent = 12;
		break;
	case 'G':
		exponent = 9;
		break;
	case 'M':
		exponent = 6;
		break;
	case 'k':
		exponent = 3;
		break;
	case 'e':
		// fall through
	case 'E':
		exponent = 0;
		break;
	case 'm':
		exponent = -3;
		break;
	case 'u':
		exponent = -6;
		break;
	case 'n':
		exponent = -9;
		break;
	case 'p':
		exponent = -12;
		break;
	case 'f':
		exponent = -15;
		break;
	case 'a':
		exponent = -18;
		break;
	default:
		unit_prefix = FALSE;
	}
	if (unit_prefix) {
		s[strlen(s)-1] = 0;
	} else {
		// check for exponent
		if (       (toupper(s[strlen(s)-2]) == 'D') ||
			       (toupper(s[strlen(s)-2]) == 'E') ) {
			// single digit exponent
			if (isdigit((unsigned char) s[strlen(s)-1])) {
				exponent = s[strlen(s)-1] - '0';
			} else {
				return NAN;
			}
			s[strlen(s)-2] = 0;
		} else if ( (toupper((unsigned char) s[strlen(s)-3]) == 'D') ||
				    (toupper(s[strlen(s)-3]) == 'E') ) {
			// double digit exponent, or single digit exponent with sign
			if (isdigit((unsigned char) s[strlen(s)-2])) {
				// double digit exponent
				if (isdigit((unsigned char) s[strlen(s)-1])) {
					exponent = (s[strlen(s)-2] - '0')*10 + s[strlen(s)-1] - '0';
				} else {
					return NAN;
				}

			} else {
				// single digit exponent with sign
				if (s[strlen(s)-2] == '-') {
					exponent = -1;
				} else if (s[strlen(s)-2] == '+')  {
					exponent = 1;
				} else {
					return NAN;
				}
				if (isdigit((unsigned char) s[strlen(s)-1])) {
					exponent = exponent * (s[strlen(s)-1] - '0');
				} else {
					return NAN;
				}
			}
			s[strlen(s)-3] = 0;
		} else if ( (toupper(s[strlen(s)-4]) == 'D') ||
				    (toupper(s[strlen(s)-4]) == 'E') ) {
			// double digit exponent with sign
			if (s[strlen(s)-3] == '-') {
				exponent = -1;
			} else if (s[strlen(s)-3] == '+')  {
				exponent = 1;
			} else {
				return NAN;
			}
			if (isdigit((unsigned char) s[strlen(s)-2]) && isdigit((unsigned char) s[strlen(s)-1])) {
				exponent = exponent * ((s[strlen(s)-2] - '0')*10 + s[strlen(s)-1] - '0');
			} else {
				return NAN;
			}
			s[strlen(s)-4] = 0;
		}
	}

	// decode significand
	for (i=0; i<strlen(s); i++) {
		// search for decimal point
		if (s[i] == '.') {
			break;
		}
	}
	// integer part
	k=1;
	for (j=i-1; j>=0; j--) {
		if (isdigit((unsigned char) s[j])) {
			integer += (s[j]-'0')*k;
			k=k*10;
		} else {
			return NAN;
		}
	}
	// fract part
	k=10;
	for (j=i+1; j<strlen(s); j++) {
		if (isdigit((unsigned char) s[j])) {
			fract += ((float)(s[j]-'0'))/((float)k);
			k=k*10;
		} else {
			return NAN;
		}
	}

	number = sign * ((float) integer+fract) * pow10_to_f(exponent);
	return number;
}


float pow10_to_f(int exp) {
	float value=1.0f;
	int i;

	if (exp != 0) {
		if (exp < 0) {
			for (i=0; i<38; i++) {
				value = value / 10.0f;
				if (++exp >= 0) {
					break;
				}
			}
		} else {
			for (i=0; i<38; i++) {
				value = 10.0f * value;
				if (--exp <= 0) {
					break;
				}
			}

		}
	}
	return value;
}


#endif // FPU_IP == 1

