/**
 *  @brief
 *      Power management e.g. low power, shutdown, charging and the like.
 *
 *		Needs BUTTON_A or calculator keyboard matrix.
 *  @file
 *      power.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2024-12-15
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
#include "power.h"
#include "button.h"
#if OLED == 1
#include "oled.h"
#endif
#if QUAD == 1
#include "quad.h"
#endif

#if POWER == 1

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
 *      Called direct after reset. Using WKUP1 (PA0) or WKUP2 (PC?)
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

		    // Set the lowest low-power mode for CPU2: shutdown mode */
		    LL_C2_PWR_SetPowerMode(LL_PWR_MODE_SHUTDOWN);
//			LL_C2_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);

		    // wakeup in debug mode
		    // should also work on STOP2/Shutdown mode
		    GPIO_InitTypeDef GPIO_InitStruct = {0};
		    __disable_irq();
//		    __HAL_RCC_GPIOC_CLK_ENABLE();
		    __HAL_RCC_GPIOA_CLK_ENABLE();

#if BUTTON == 1
#if BUTTON_MATRIX == 1
		    // calculator keyboard matrix
		    int i;

		    // using COL0 and ROW6 (on/off button)
		    GPIO_InitStruct.Pin = COL0_Pin;
		    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    HAL_GPIO_Init(COL0_GPIO_Port, &GPIO_InitStruct);
		    GPIO_InitStruct.Pin = ROW6_Pin;
		    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		    GPIO_InitStruct.Pull = GPIO_NOPULL;
		    HAL_GPIO_Init(ROW6_GPIO_Port, &GPIO_InitStruct);

		    // activate only last row
			HAL_GPIO_WritePin(ROW6_GPIO_Port, ROW6_Pin, GPIO_PIN_RESET);

			for (i=0; i<100000; i++) {
				; // wait for ports
			}

			while (HAL_GPIO_ReadPin(COL0_GPIO_Port, COL0_Pin) == GPIO_PIN_RESET) {
		    	; // wait till button release
			}

		    __HAL_GPIO_EXTI_CLEAR_IT(COL0_Pin);
		    __HAL_GPIO_EXTI_CLEAR_FLAG(COL0_Pin);
		    __NVIC_ClearPendingIRQ(EXTI0_IRQn);

		    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
		    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

		    LL_EXTI_DisableIT_0_31( (~0) );
		    LL_EXTI_DisableIT_32_63( (~0) );
		    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_0);
		    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_0);

		    // shutdown, exit into POR, wake up on falling edge (PA0, BUTTON_A_Pin, PWR_WAKEUP_PIN1_LOW)
		    // POWER button is the power switch
		    HAL_PWREx_ClearWakeupFlag(PWR_FLAG_WUF1);
		    HAL_PWREx_DisableInternalWakeUpLine();
		    HAL_PWREx_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW, PWR_CORE_CPU1);
#ifndef DEBUG
		    DBGMCU->CR = 0; // Disable debug, trace and IWDG in low-power modes
#endif
		    //HAL_PWREx_EnterSHUTDOWNMode();
		    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);

#else
		    // using BUTTON_A
		    GPIO_InitStruct.Pin = BUTTON_A_Pin;
		    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		    GPIO_InitStruct.Pull = GPIO_PULLUP;
		    HAL_GPIO_Init(BUTTON_A_GPIO_Port, &GPIO_InitStruct);

		    while (HAL_GPIO_ReadPin(BUTTON_A_GPIO_Port, BUTTON_A_Pin) == GPIO_PIN_RESET) {
		    	; // wait till button release
		    }

		    __HAL_GPIO_EXTI_CLEAR_IT(BUTTON_A_Pin);
		    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
		    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
		    __NVIC_ClearPendingIRQ(EXTI0_IRQn);

		    LL_EXTI_DisableIT_0_31( (~0) );
		    LL_EXTI_DisableIT_32_63( (~0) );
		    LL_EXTI_EnableFallingTrig_0_31(LL_EXTI_LINE_0);
		    LL_EXTI_EnableIT_0_31(LL_EXTI_LINE_0);

		    // shutdown, exit into POR, wake up on falling edge (PA0, BUTTON_A_Pin, PWR_WAKEUP_PIN1_LOW)
		    // POWER button is the power switch
		    HAL_PWREx_ClearWakeupFlag(PWR_FLAG_WUF1);
		    HAL_PWREx_DisableInternalWakeUpLine();
		    HAL_PWREx_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW, PWR_CORE_CPU1);
#ifndef DEBUG
		    DBGMCU->CR = 0; // Disable debug, trace and IWDG in low-power modes
#endif
		    //HAL_PWREx_EnterSHUTDOWNMode();
		    HAL_PWREx_EnterSTOP2Mode(PWR_STOPENTRY_WFI);
#endif
#endif

    		// should never reach this code (except exit from stop), only for debugging
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
#if OLED == 1
		OLED_off();
#endif
#if QUAD == 1
		QUAD_shutdown(0);
#endif

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


// Private Functions
// *****************


// Callbacks
// *********

#endif // POWER
