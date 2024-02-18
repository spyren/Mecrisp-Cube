/**
 *  @brief
 *  	On-board MEMS sensors.
 *
 *  	 - Temperature sensor TTS22H
 *  	 - Accelerometer ISM330DHCX
 *  	 - Time‑of‑Flight sensor VL53L0CXV0DH
 *
 *  @file
 *      mems.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-02-17
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
#include "mems.h"
#include "iic3.h"

// Macros
// ******


// Private function prototypes
// ***************************


// Global Variables
// ****************

// Hardware resources
// ******************
extern I2C_HandleTypeDef hi2c3;

// RTOS resources
// **************
osSemaphoreId_t MEMS_DRDY_SemaphoreID;

// Private Variables
// *****************
static uint8_t tts22h_ready = FALSE;
static uint8_t ism330_ready = FALSE;

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the mems controller.
 *  @return
 *      None
 */
void MEMS_init(void) {
	// semaphore not used yet
	MEMS_DRDY_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (MEMS_DRDY_SemaphoreID == NULL) {
		Error_Handler();
	}

	if (MEMS_TTS22H_getRegister(MEMS_TTS22H_WHOAMI) == MEMS_TTS22H_ID) {
		tts22h_ready = TRUE;
	}

	if (MEMS_ISM330_getRegister(MEMS_ISM330DHCX_WHOAMI) == MEMS_ISM330DHCX_ID) {
		ism330_ready = TRUE;
	}
}


/**
 *  @brief
 *      Get temperature from temperature sensor TTS22H
 *  @return
 *      temperature * 100
 */
int MEMS_getTemperature(void) {
	int i = 0;

	MEMS_TTS22H_setRegister(MEMS_TTS22H_CTRL, MEMS_TTS22H_ONE_SHOT);
	while ((i++ < 12) && (MEMS_TTS22H_getRegister(MEMS_TTS22H_STATUS) & MEMS_TTS22H_BUSY)) {
		osDelay(100);
	}
	return MEMS_TTS22H_getRegister(MEMS_TTS22H_TEMP_H_OUT) << 8 |
		   MEMS_TTS22H_getRegister(MEMS_TTS22H_TEMP_L_OUT);
}


/**
 *  @brief
 *      Get register from temperature sensor TTS22H
 *  @param[in]
 *  	reg   register/command
 *  @return
 *      data
 */
int MEMS_TTS22H_getRegister(uint8_t reg) {
	uint8_t buf[2];

	buf[0] = reg;
	IIC3_putGetMessage(buf, 1, 1, MEMS_TTS22H_I2C_ADR);
	return buf[0];
}


/**
 *  @brief
 *      Set register content of the temperature sensor TTS22H
 *  @param[in]
 *  	reg   register/command
 *  @param[in]
 *  	data
 *  @return
 *      none
 */
void MEMS_TTS22H_setRegister(uint8_t reg, uint8_t data) {
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = data;
	IIC3_putMessage(buf, 2, MEMS_TTS22H_I2C_ADR);
}


/**
 *  @brief
 *      Get register from accelerometer ISM330DHCX
 *  @param[in]
 *  	reg   register/command
 *  @return
 *      data
 */
int MEMS_ISM330_getRegister(uint8_t reg) {
	uint8_t buf[2];

	buf[0] = reg;
	IIC3_putGetMessage(buf, 1, 1, MEMS_ISM330DHCX_I2C_ADR);
	return buf[0];
}


/**
 *  @brief
 *      Set register content of the accelerometer ISM330DHCX
 *  @param[in]
 *  	reg   register/command
 *  @param[in]
 *  	data
 *  @return
 *      none
 */
void MEMS_ISM330_setRegister(uint8_t reg, uint8_t data) {
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = data;
	IIC3_putMessage(buf, 2, MEMS_ISM330DHCX_I2C_ADR);
}

