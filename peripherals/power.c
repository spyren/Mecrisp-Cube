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

// Global Variables
// ****************
uint8_t GAUGE_UpdateBatState = TRUE;


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

		    // wakeup in debug mode
		    // should also work on STOP2/Shutdown mode
		    GPIO_InitTypeDef GPIO_InitStruct = {0};
		    __disable_irq();
		    __HAL_RCC_GPIOC_CLK_ENABLE();
		    GPIO_InitStruct.Pin = BUTTON_BACK_Pin;
		    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    HAL_GPIO_Init(BUTTON_BACK_GPIO_Port, &GPIO_InitStruct);

		    while (HAL_GPIO_ReadPin(BUTTON_BACK_GPIO_Port, BUTTON_BACK_Pin) == GPIO_PIN_RESET) {
		    	; // wait till button release
		    }

		    __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_BACK_Pin);
		    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
		    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

		    LL_EXTI_DisableIT_0_31( (~0) );
		    LL_EXTI_DisableIT_32_63( (~0) );
		    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_13);
		    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_13);

		    // shutdown, exit into POR, wake up on falling edge (BUTTON_BACK_Pin, PWR_WAKEUP_PIN2_LOW)
		    // BACK button is the power switch
		    HAL_PWREx_ClearWakeupFlag(PWR_FLAG_WUF2);
		    HAL_PWREx_DisableInternalWakeUpLine();
		    HAL_PWREx_EnableWakeUpPin(PWR_WAKEUP_PIN2_LOW, PWR_CORE_CPU1);
#ifndef DEBUG
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
	return (int16_t) (buf[0] + (buf[1] << 8));
}


/**
 *  @brief
 *      Set fuel gauge register
 *  @param[in]
 *  	reg   register/command
 *  @return
 *      data
 */
void GAUGE_setRegister(uint8_t reg, int16_t data) {
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


// Callbacks
// *********


