/**
 *  @brief
 *      Power management e.g. low power, shutdown, charging and the like.
 *
 *		Flipper's power switch is the back button.
 *
 *		Fuel gauge BQ27220YZFR
 *		  - uses the Compensated End-of-Discharge Voltage (CEDV) algorithm
 *		    for fuel gauging
 *		  - remaining battery capacity (mAh),
 *		  - state-of-charge (%)
 *		  - runtime-to-empty (min)
 *		  - battery voltage (mV)
 *		  - temperature (°C)
 *		  - state-of-health (%)
 *
 *		Charger BQ25896RTWR
 *  @file
 *      power.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2023-03-02
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
#include "clock.h"
#include "lcd.h"
#include "rgbw.h"
#include "power.h"
#include "iic.h"

// Private function prototypes
// ***************************
void update_sw(void);

// Global Variables
// ****************
extern RTC_HandleTypeDef hrtc;

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
 *      Check for startup or halt/shutdown.
 *
 *      Called direct after reset.
 *  @return
 *      None
 */
void POWER_startup(void) {
	if (RTC_Backup.power == RTC_MAGIC_COOKIE) {
		if (RTC_Backup.power_param & POWER_SHUTDOWN) {
			// request for halt/shutdown

			// clear halt request
			hrtc.Instance = RTC;
		    HAL_PWR_EnableBkUpAccess();
			RTC_Backup.power_param &= ~POWER_SHUTDOWN;

		    // CPU2 is not started
		    // Set the lowest low-power mode for CPU2: shutdown mode */
		    LL_C2_PWR_SetPowerMode(LL_PWR_MODE_SHUTDOWN);

		    // shutdown, exit into POR, wake up on falling edge (PWR_WAKEUP_PINx_LOW)
		    // BACK button is the power switch
		    HAL_PWREx_EnablePullUpPullDownConfig();
			if (RTC_Backup.power_param & POWER_SWITCH1) {
				// enable wake up switch2 (PC13, WKUP2)
			    HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_C, PWR_GPIO_BIT_13); // wake up pin is PC13
			    HAL_PWREx_ClearWakeupFlag(PWR_FLAG_WUF2);
			    HAL_PWREx_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW, PWR_CORE_CPU1);
			}
#ifdef NDEBUG
			DBGMCU->CR = 0; // Disable debug, trace and IWDG in low-power modes
#endif
    		HAL_PWREx_EnterSHUTDOWNMode();

    		// should never reach this code, only for debugging
    		HAL_NVIC_SystemReset();
		}
	}
}


/**
 *  @brief
 *      Initialize the POWER module.
 *  @return
 *      None
 */
void POWER_init(void) {
	if (RTC_Backup.power != RTC_MAGIC_COOKIE) {
		// default power switch behavior
		RTC_Backup.power = RTC_MAGIC_COOKIE;
		RTC_Backup.power_param = POWER_SWITCH1;
	}
	update_sw();
}


/**
 *  @brief
 *      Halt (power off) the MCU and wait for wake up.
 *  @return
 *      None
 */
void POWER_halt(void) {
	if (RTC_Backup.power == RTC_MAGIC_COOKIE) {
		// RTC is up and running and low power shutdown is activated

		// switch off peripherals
		LCD_switchOff();
		RGBW_switchOff();
		POWER_setPeripheral(0);

		// request halt
		RTC_Backup.power_param |= POWER_SHUTDOWN;
		NVIC_SystemReset();
	}
}


/**
 *  @brief
 *      Set power switch
 *  @param[in]
 *  	sw	0x01 switch1, 0x02 switch2
 *  @return
 *      None
 */
void POWER_setSwitch(int sw) {
	RTC_Backup.power_param = sw;
	update_sw();
}


/**
 *  @brief
 *      Get power switch
 *  @return
 *      1 switch1, 2 switch2
 */
int POWER_getSwitch(void) {
	return RTC_Backup.power_param;
}


/**
 *  @brief
 *      EXTI from switch button occurred
 *  @param[in]
 *  	sw	1 switch1, 2 switch2
 *  @return
 *      None
 */
void POWER_switchEvent(int sw) {
	if (sw & RTC_Backup.power_param) {
		POWER_halt();
	} else {
		// no power switch enabled
		return;
	}
	// wakeup
}


/**
 *  @brief
 *      Set Peripheral power
 *  @return
 *      None
 */
void POWER_setPeripheral(int status) {
	if (status) {
		HAL_GPIO_WritePin(GPIOA, PERIPH_POWER_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(GPIOA, PERIPH_POWER_Pin, GPIO_PIN_RESET);
	}

}


/**
 *  @brief
 *      Get LIPO voltage
 *  @return
 *      voltage in mV
 */
int GAUGE_getVoltage(void) {
	return GAUGE_getRegister(GAUGE_CMD_VOLTAGE);
}


/**
 *  @brief
 *      Get LIPO current
 *  @return
 *      current in mA
 */
int GAUGE_getCurrent(void) {
	return GAUGE_getRegister(GAUGE_CMD_CURRENT);
}


/**
 *  @brief
 *      Get LIPO charge
 *  @return
 *      charge in %
 */
int GAUGE_getCharge(void) {
	return GAUGE_getRegister(GAUGE_CMD_STATE_OF_CHARGE);
}


/**
 *  @brief
 *      Get register from fuel gauge
 *  @param[in]
 *  	reg   register/command
 *  @return
 *      data
 */
int GAUGE_getRegister(uint8_t reg) {
	uint8_t buf[3];

	buf[0] = reg;
	IIC_putGetMessage(buf, 1, 2, GAUGE_I2C_ADR);
	return buf[0] + (buf[1] << 8);
}


/**
 *  @brief
 *      Set fuel gauge register
 *  @param[in]
 *  	reg   register/command
 *  @return
 *      data
 */
void GAUGE_setRegister(uint8_t reg, int data) {
	uint8_t buf[3];

	buf[0] = reg;
	buf[1] = data & 0xFF;
	buf[2] = (data >> 8) & 0xFF;
	IIC_putMessage(buf, 3, GAUGE_I2C_ADR);
}


/**
 *  @brief
 *      Get register from charger
 *  @param[in]
 *  	reg   register/command
 *  @return
 *      data
 */
int CHARGER_getRegister(uint8_t reg) {
	uint8_t buf[2];

	buf[0] = reg;
	IIC_putGetMessage(buf, 1, 1, CHARGER_I2C_ADR);
	return buf[0];
}


/**
 *  @brief
 *      Set register content of the charger
 *  @param[in]
 *  	reg   register/command
 *  @param[in]
 *  	data
 *  @return
 *      none
 */
void CHARGER_setRegister(uint8_t reg, uint8_t data) {
	uint8_t buf[2];

	buf[0] = reg;
	buf[1] = data;
	IIC_putMessage(buf, 2, CHARGER_I2C_ADR);
}


// Private Functions
// *****************

void update_sw(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	if (RTC_Backup.power_param & POWER_SWITCH1) {
		// enable halt switch1
		// halt on switch button 1 release (rising edge)
		GPIO_InitStruct.Pin = BUTTON_BACK_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(BUTTON_BACK_GPIO_Port, &GPIO_InitStruct);
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_13);
		HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
		HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	} else {
		// disable halt switch1
		GPIO_InitStruct.Pin = BUTTON_BACK_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(BUTTON_BACK_GPIO_Port, &GPIO_InitStruct);
	}
}


// Callbacks
// *********


