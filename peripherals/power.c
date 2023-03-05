/**
 *  @brief
 *      Power management e.g. low power, shutdown and the like.
 *
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
#include "oled.h"
#include "power.h"

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
			__HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
			RTC_Backup.power_param &= ~POWER_SHUTDOWN;

		    // CPU2 is not started
		    // Set the lowest low-power mode for CPU2: shutdown mode */
		    LL_C2_PWR_SetPowerMode(LL_PWR_MODE_SHUTDOWN);

		    // shutdown, exit into POR, wake up on falling edge (PWR_WAKEUP_PINx_LOW)
		    HAL_PWREx_EnablePullUpPullDownConfig();
			if (RTC_Backup.power_param & POWER_SWITCH1) {
				// enable wakeup switch1 (PC12, WKUP3)
			    HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_C, PWR_GPIO_BIT_12); // wakeup is PC12
			    HAL_PWREx_ClearWakeupFlag(PWR_FLAG_WUF3);
			    HAL_PWREx_EnableWakeUpPin(PWR_WAKEUP_PIN3_LOW, PWR_CORE_CPU1);
			}
			if (RTC_Backup.power_param & POWER_SWITCH2) {
				// enable wakeup switch2 (PC13, WKUP2)
			    HAL_PWREx_EnableGPIOPullUp(PWR_GPIO_C, PWR_GPIO_BIT_13); // wakeup is PC13
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
		RTC_Backup.power_param = POWER_SWITCH2;
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
		// RTC is up and running and power is activated

		// switch off peripherals
		OLED_switchOff();

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


// Private Functions
// *****************

void update_sw(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	if (RTC_Backup.power_param & POWER_SWITCH1) {
		// enable halt switch1
		// halt on switch button 1 release (rising edge)
		GPIO_InitStruct.Pin = B1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
	} else {
		// disable halt switch1
		GPIO_InitStruct.Pin = B1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);
	}
	if (RTC_Backup.power_param & POWER_SWITCH2) {
		// enable halt switch2
		// halt on switch button 2 release (rising edge)
		GPIO_InitStruct.Pin = B2_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(B2_GPIO_Port, &GPIO_InitStruct);
	} else {
		// disable halt switch2
		GPIO_InitStruct.Pin = B2_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(B2_GPIO_Port, &GPIO_InitStruct);
	}
}


// Callbacks
// *********


