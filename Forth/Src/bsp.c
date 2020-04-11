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


// digital port pins D0 to D15 (Arduino numbering)
// ***********************************************

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} PortPin_t;

static const PortPin_t PortPin_a[16] = {
		{ D0_GPIO_Port, D0_Pin } ,
		{ D1_GPIO_Port, D1_Pin } ,
		{ D2_GPIO_Port, D2_Pin } ,
		{ D3_GPIO_Port, D3_Pin } ,
		{ D4_GPIO_Port, D4_Pin } ,
		{ D5_GPIO_Port, D5_Pin } ,
		{ D6_GPIO_Port, D6_Pin } ,
		{ D7_GPIO_Port, D7_Pin } ,
		{ D8_GPIO_Port, D9_Pin } ,
		{ D9_GPIO_Port, D9_Pin } ,
		{ D10_GPIO_Port, D10_Pin } ,
		{ D11_GPIO_Port, D11_Pin } ,
		{ D12_GPIO_Port, D12_Pin } ,
		{ D13_GPIO_Port, D13_Pin } ,
		{ D14_GPIO_Port, D14_Pin } ,
		{ D15_GPIO_Port, D15_Pin }
};

/**
 *  @brief
 *	    Sets the digital output port pins (D0 .. D15).
 *
 *	@param[in]
 *      state    lower word sets the pins.
 *  @return
 *      none
 *
 */
void BSP_setDigitalPort(int state) {
	uint8_t i;
	for (i=0; i<16; i++) {
		HAL_GPIO_WritePin(PortPin_a[i].port, PortPin_a[i].pin, state & 0x01);
		state = state >> 1;
	}
}


/**
 *  @brief
 *	    Gets the digital output port pins (D0 .. D15).
 *
 *  @return
 *      state port pins
 *
 */
int BSP_getDigitalPort(void) {
	int i;
	int return_value = 0;
	for (i=15; i>-1; i--) {
		return_value = return_value << 1;
		return_value |= HAL_GPIO_ReadPin(PortPin_a[i].port, PortPin_a[i].pin);
	}
	return return_value;
}


/**
 *  @brief
 *	    Sets the digital output port pin (D0 .. D15).
 *
 *	@param[in]
 *      pin_number    0 to 15.
 *	@param[in]
 *      state         0/1
 *  @return
 *      none
 *
 */
void BSP_setDigitalPin(int pin_number, int state) {
	HAL_GPIO_WritePin(PortPin_a[pin_number].port, PortPin_a[pin_number].pin, state);
}


/**
 *  @brief
 *	    Gets the digital output port pin (D0 .. D15).
 *
 *	@param[in]
 *      pin_number    0 to 15.
 *  @return
 *      state         0/1
 *
 */
int BSP_getDigitalPin(int pin_number) {
	return HAL_GPIO_ReadPin(PortPin_a[pin_number].port, PortPin_a[pin_number].pin);
}


// analog port pins A0 to A5 (Arduino numbering)
// *********************************************

// Private Functions
// *****************

