/**
 *  @brief
 *      Board Support Package.
 *
 *		Forth TRUE is -1, C TRUE is 1.
 *      LEDs and switches.
 *  @file
 *      bsp.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-26
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

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "bsp.h"


// Private function prototypes
// ***************************

// Global Variables
// ****************

// Hardware resources
// ******************

// RTOS resources
// **************



// Private Variables
// *****************

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the BSP.
 *  @return
 *      None
 */
void BSP_init(void) {
}


// only for testing purposes
void BSP_blinkThread(void *argument) {
/*
	: blink-thread  ( -- )
	  begin
	    led1@ 0= led1!   \ toggle blue LED
	    200 osDelay drop
	    switch1?
	  until
	  0 led1!
	;
*/
	while (! BSP_getSwitch1()) {
		BSP_setLED1(!BSP_getLED1());
		osDelay(200);
	}
	BSP_setLED1(FALSE);
	osThreadExit();
}


/**
 *  @brief
 *	    Sets the LED1 (blue).
 *
 *	@param[in]
 *      state    FALSE for dark LED, TRUE for bright LED.
 *  @return
 *      none
 *
 */
void BSP_setLED1(int state) {
	if (state) {
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	}
}


/**
 *  @brief
 *      Gets the LED1 (blue) state
 *
 *  @return
 *      FALSE for dark LED, TRUE for bright LED.
 */
int BSP_getLED1(void) {
	if (HAL_GPIO_ReadPin(LD1_GPIO_Port, LD1_Pin) == GPIO_PIN_SET) {
		return -1;
	} else {
		return FALSE;
	}
}


/**
 *  @brief
 *	    Sets the LED2 (green).
 *
 *	@param[in]
 *      state    FALSE for dark LED, TRUE for bright LED.
 *  @return
 *      none
 *
 */
void BSP_setLED2(int state) {
	if (state) {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
	}
}


/**
 *  @brief
 *      Gets the LED2 (green) state
 *
 *  @return
 *      FALSE for dark LED, TRUE for bright LED.
 */
int BSP_getLED2(void) {
	if (HAL_GPIO_ReadPin(LD2_GPIO_Port, LD2_Pin) == GPIO_PIN_SET) {
		return -1;
	} else {
		return FALSE;
	}
}

/**
 *  @brief
 *	    Sets the LED3 (red).
 *
 *	@param[in]
 *      state    FALSE for dark LED, TRUE for bright LED.
 *  @return
 *      none
 *
 */
void BSP_setLED3(int state) {
	if (state) {
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	}
}


/**
 *  @brief
 *      Gets the LED3 (red) state
 *
 *  @return
 *      FALSE for dark LED, TRUE for bright LED.
 */
int BSP_getLED3(void) {
	if (HAL_GPIO_ReadPin(LD3_GPIO_Port, LD3_Pin) == GPIO_PIN_SET) {
		return -1;
	} else {
		return FALSE;
	}
}


/**
 *  @brief
 *      Gets the switch1 state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch1(void) {
	if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) {
		return -1;
	} else {
		return FALSE;
	}
}

/**
 *  @brief
 *      Gets the switch1 state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch2(void) {
	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_RESET) {
		return -1;
	} else {
		return FALSE;
	}
}

/**
 *  @brief
 *      Gets the switch1 state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch3(void) {
	if (HAL_GPIO_ReadPin(B3_GPIO_Port, B3_Pin) == GPIO_PIN_RESET) {
		return -1;
	} else {
		return FALSE;
	}
}



// Private Functions
// *****************

