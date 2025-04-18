/**
 *  @brief
 *      Board Support Package for STM32WB Firefly.
 *
 *        - LEDs (LED1, blue)
 *        - Switches (SW1)
 *        - Digital port pins D0 to D15
 *        - Analog port pins A0 to A4
 *        - PWM:  D0 TIM1CH3, D1 TIM1CH2, A4 TIM1CH1
 *        - Timer
 *          - Input capture TIM2CH2 A1
 *          - Output compare TIM2CH1 D13, TIM2CH3 A2, TIM2CH4 A3
 *        - EXTI: D3, D5, D6, D7
 *        - SPI: D11 MOSI, D12 MISO, D13 SCK (display, memory), D10 CS for SD
 *        - NeoPixel D9
 *        - Buttons D2, D3, D4, D5, D6, D7, D8
 *
 *      Forth TRUE is -1, C TRUE is 1.
 *      No timeout (osWaitForever) for mutex ->
 *        could be problematic in real world applications.
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
#include "i2c.h"
#include "usart.h"
#include "spi.h"
#include "bsp.h"
#include "button.h"
#include "stm32_lpm.h"
#include "button.h"


// Private function prototypes
// ***************************
static void update_sysled(void);

// Global Variables
// ****************
const char BSP_Version[] = "  * Firmware Package STM32Cube FW_WB V1.17.3, BLE Stack 5.3 (C) 2023 STMicroelectronics \n";
extern TIM_HandleTypeDef htim2;

// Hardware resources
// ******************
extern TIM_HandleTypeDef htim1;

// RTOS resources
// **************

static osMutexId_t DigitalPort_MutexID;
static const osMutexAttr_t DigitalPort_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osMutexId_t Adc_MutexID;
static const osMutexAttr_t Adc_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t Adc_SemaphoreID;

static osSemaphoreId_t ICOC_period_SemaphoreID;
static osSemaphoreId_t ICOC_CH1_SemaphoreID;
static osSemaphoreId_t ICOC_CH2_SemaphoreID;
static osSemaphoreId_t ICOC_CH3_SemaphoreID;
static osSemaphoreId_t ICOC_CH4_SemaphoreID;

static osSemaphoreId_t EXTI_1_SemaphoreID;
static osSemaphoreId_t EXTI_2_SemaphoreID;
static osSemaphoreId_t EXTI_3_SemaphoreID;
static osSemaphoreId_t EXTI_4_SemaphoreID;


// Private Variables
// *****************
ADC_ChannelConfTypeDef sConfig = {0};
extern ADC_HandleTypeDef hadc1;
uint32_t neo_pixel = 0;
static uint32_t adc_calibration;

//static int sys_led_status = SYSLED_ACTIVATE;
static int sys_led_status = 0;

// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the BSP.
 *  @return
 *      None
 */
void BSP_init(void) {
	DigitalPort_MutexID = osMutexNew(&DigitalPort_MutexAttr);
	if (DigitalPort_MutexID == NULL) {
		Error_Handler();
	}
	Adc_MutexID = osMutexNew(&Adc_MutexAttr);
	if (Adc_MutexID == NULL) {
		Error_Handler();
	}
	Adc_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (Adc_SemaphoreID == NULL) {
		Error_Handler();
	}

	ICOC_period_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (ICOC_period_SemaphoreID == NULL) {
		Error_Handler();
	}
	ICOC_CH1_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (ICOC_CH1_SemaphoreID == NULL) {
		Error_Handler();
	}
	ICOC_CH2_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (ICOC_CH2_SemaphoreID == NULL) {
		Error_Handler();
	}
	ICOC_CH3_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (ICOC_CH3_SemaphoreID == NULL) {
		Error_Handler();
	}
	ICOC_CH4_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (ICOC_CH4_SemaphoreID == NULL) {
		Error_Handler();
	}

	EXTI_1_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_1_SemaphoreID == NULL) {
		Error_Handler();
	}
	EXTI_2_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_2_SemaphoreID == NULL) {
		Error_Handler();
	}
	EXTI_3_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_3_SemaphoreID == NULL) {
		Error_Handler();
	}
	EXTI_4_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_4_SemaphoreID == NULL) {
		Error_Handler();
	}

	// ADC calibration
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	adc_calibration = HAL_ADCEx_Calibration_GetValue(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_SetValue(&hadc1, ADC_SINGLE_ENDED, adc_calibration);

	// Configure Regular Channel
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_47CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	// start PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

}


// LEDs
// ****

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
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	if (state) {
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
	}

	osMutexRelease(DigitalPort_MutexID);
}


/**
 *  @brief
 *      Gets the LED1 (blue) state
 *
 *  @return
 *      FALSE for dark LED, TRUE for bright LED.
 */
int BSP_getLED1(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	if (HAL_GPIO_ReadPin(LD1_GPIO_Port, LD1_Pin) == GPIO_PIN_SET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}




// Switches
// ********

/**
 *  @brief
 *      Gets the switch1 state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch1(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

#if BUTTON == 1
#if BUTTON_MATRIX == 0
	if (HAL_GPIO_ReadPin(BUTTON_A_GPIO_Port, BUTTON_A_Pin) == GPIO_PIN_RESET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}
#else
	if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}
#endif
#endif

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}


#if BUTTON == 1
#if BUTTON_MATRIX == 0
/**
 *  @brief
 *      Gets the switch2 state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch2(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	if (HAL_GPIO_ReadPin(BUTTON_B_GPIO_Port, BUTTON_B_Pin) == GPIO_PIN_RESET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

/**
 *  @brief
 *      Gets the switch3 state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch3(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	if (HAL_GPIO_ReadPin(BUTTON_C_GPIO_Port, BUTTON_C_Pin) == GPIO_PIN_RESET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

#endif
#endif



// digital port pins D0 to D15 (Arduino numbering)
// ***********************************************

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
} PortPin_t;

static const PortPin_t PortPin_a[21] = {
		{ D0_GPIO_Port, D0_Pin } ,		// 0
		{ D1_GPIO_Port, D1_Pin } ,		// 1
		{ D2_GPIO_Port, D2_Pin } ,		// 2
		{ D3_GPIO_Port, D3_Pin } ,		// 3
		{ D4_GPIO_Port, D4_Pin } ,		// 4
		{ D5_GPIO_Port, D5_Pin } ,		// 5
		{ D6_GPIO_Port, D6_Pin } ,		// 6
		{ D7_GPIO_Port, D7_Pin } ,		// 7
		{ D8_GPIO_Port, D8_Pin } ,		// 8
		{ D9_GPIO_Port, D9_Pin } ,		// 9
		{ D10_GPIO_Port, D10_Pin } ,	// 10
		{ D11_GPIO_Port, D11_Pin } ,	// 11
		{ D12_GPIO_Port, D12_Pin } ,	// 12
		{ D13_GPIO_Port, D13_Pin } ,	// 13
		{ D14_GPIO_Port, D14_Pin } ,	// 14
		{ D15_GPIO_Port, D15_Pin } ,	// 15
		{ A0_GPIO_Port, A0_Pin } ,		// 16
		{ A1_GPIO_Port, A1_Pin } ,		// 17
		{ A2_GPIO_Port, A2_Pin } ,		// 18
		{ A3_GPIO_Port, A3_Pin } ,		// 19
		{ A4_GPIO_Port, A4_Pin }		// 20
};

/**
 *  @brief
 *	    Sets the digital output port pins (D0 .. D15).
 *
 *		The analog pins can also used as digital pins.
 *	@param[in]
 *      state    lower word sets the pins.
 *  @return
 *      none
 *
 */
void BSP_setDigitalPort(int state) {
	uint8_t i;
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	for (i=0; i<16; i++) {
		HAL_GPIO_WritePin(PortPin_a[i].port, PortPin_a[i].pin, state & 0x01);
		state = state >> 1;
	}

	osMutexRelease(DigitalPort_MutexID);
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
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	for (i=15; i>-1; i--) {
		return_value = return_value << 1;
		return_value |= HAL_GPIO_ReadPin(PortPin_a[i].port, PortPin_a[i].pin);
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}


/**
 *  @brief
 *	    Sets the digital output port pin (D0 .. D15, A0 .. A4).
 *
 *	@param[in]
 *      pin_number    0 to 20.
 *	@param[in]
 *      state         0/1
 *  @return
 *      none
 *
 */
void BSP_setDigitalPin(int pin_number, int state) {
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	HAL_GPIO_WritePin(PortPin_a[pin_number].port, PortPin_a[pin_number].pin, state);

	osMutexRelease(DigitalPort_MutexID);
}


/**
 *  @brief
 *	    Gets the digital input port pin (D0 .. D15, A0 .. A4).
 *
 *	@param[in]
 *      pin_number    0 to 20.
 *  @return
 *      state         0/1
 *
 */
int BSP_getDigitalPin(int pin_number) {
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	int return_value = HAL_GPIO_ReadPin(PortPin_a[pin_number].port, PortPin_a[pin_number].pin);

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}


// analog port pins A0 to A5 (Arduino numbering)
// *********************************************
static const uint32_t AnalogPortPin_a[6] = {
		ADC_CHANNEL_5,  // A0 PA0
		ADC_CHANNEL_6,  // A1 PA1
		ADC_CHANNEL_7,  // A2 PA2
		ADC_CHANNEL_8,  // A3 PA3
		ADC_CHANNEL_15, // A4 PA8
		ADC_CHANNEL_3   // A5 -
};

/**
 *  @brief
 *	    Gets the analog input port pin (A0 .. A5) ADC value.
 *
 *	@param[in]
 *      pin_number    0 to 5.
 *  @return
 *      12 bit ADC value *
 */
int BSP_getAnalogPin(int pin_number) {
	int return_value;
	HAL_StatusTypeDef status;

	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_DISABLE);
	// only one thread is allowed to use the ADC
	osMutexAcquire(Adc_MutexID, osWaitForever);

	sConfig.Channel = AnalogPortPin_a[pin_number];
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	status = HAL_ADC_Start_IT(&hadc1);
	if (status != HAL_OK) {
		Error_Handler();
	}
	// blocked till ADC conversion is finished
	status = osSemaphoreAcquire(Adc_SemaphoreID, osWaitForever);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_ENABLE);

	return_value = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop_IT(&hadc1);

	osMutexRelease(Adc_MutexID);
	return return_value;
}

/**
 *  @brief
 *	    Get the Vref intern
 *
 *		The Vref is actually the VDDA
 *  @return
 *      Voltage in mV
 */
int BSP_getVref(void) {
	int value;
	HAL_StatusTypeDef status;

	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_DISABLE);
	// only one thread is allowed to use the ADC
	osMutexAcquire(Adc_MutexID, osWaitForever);

	sConfig.Channel = ADC_CHANNEL_VREFINT;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	status = HAL_ADC_Start_IT(&hadc1);
	if (status != HAL_OK) {
		Error_Handler();
	}
	// blocked till ADC conversion is finished
	status = osSemaphoreAcquire(Adc_SemaphoreID, osWaitForever);
	value = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop_IT(&hadc1);

	osMutexRelease(Adc_MutexID);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_ENABLE);

	return __HAL_ADC_CALC_VREFANALOG_VOLTAGE(value, ADC_RESOLUTION_12B);
}


/**
 *  @brief
 *	    Get the Vbat
 *
 *  @return
 *      Voltage in mV
 */
int BSP_getVbat(void) {
	int value;
	int ref_voltage_mv;
	HAL_StatusTypeDef status;

	ref_voltage_mv = BSP_getVref();

	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_DISABLE);
	// only one thread is allowed to use the ADC
	osMutexAcquire(Adc_MutexID, osWaitForever);

	HAL_GPIO_WritePin(VBAT_EN_GPIO_Port, VBAT_EN_Pin, GPIO_PIN_SET);
	osDelay(5);

	sConfig.Channel = ADC_CHANNEL_VBAT;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	status = HAL_ADC_Start_IT(&hadc1);
	if (status != HAL_OK) {
		Error_Handler();
	}
	// blocked till ADC conversion is finished
	status = osSemaphoreAcquire(Adc_SemaphoreID, osWaitForever);
	value = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop_IT(&hadc1);
	HAL_GPIO_WritePin(VBAT_EN_GPIO_Port, VBAT_EN_Pin, GPIO_PIN_RESET);

	osMutexRelease(Adc_MutexID);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_ENABLE);

	// internal voltage divider 2/1, external 27k/100k
	return (3 * __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_voltage_mv, value, ADC_RESOLUTION_12B) * 127 ) / 100 ;
}


/**
 *  @brief
 *	    Gets the CPU temperature
 *
 *  @return
 *      temperature in degree Celsius
 */
int BSP_getCpuTemperature(void) {
	int value;
	int ref_voltage_mv;
	HAL_StatusTypeDef status;

	ref_voltage_mv = BSP_getVref();

	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_DISABLE);
	// only one thread is allowed to use the ADC
	osMutexAcquire(Adc_MutexID, osWaitForever);

	sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	status = HAL_ADC_Start_IT(&hadc1);
	if (status != HAL_OK) {
		Error_Handler();
	}
	// blocked till ADC conversion is finished
	status = osSemaphoreAcquire(Adc_SemaphoreID, osWaitForever);
	value = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop_IT(&hadc1);

	osMutexRelease(Adc_MutexID);
	UTIL_LPM_SetStopMode(1U << CFG_LPM_ADC, UTIL_LPM_ENABLE);

	return __HAL_ADC_CALC_TEMPERATURE(ref_voltage_mv, value, ADC_RESOLUTION_12B);
}

// digital port pin mode
// *********************
typedef struct {
	uint32_t mode;
	uint32_t pull;
	uint32_t alternate;
} PortPinMode_t;

static const PortPinMode_t DigitalPortPinMode_a[] = {
	{ GPIO_MODE_INPUT,     GPIO_NOPULL,   0 } ,				//  0 in
	{ GPIO_MODE_INPUT,     GPIO_PULLUP,   0 } ,				//  1 in pullup
	{ GPIO_MODE_INPUT,     GPIO_PULLDOWN, 0 } ,				//  2 in pulldown
	{ GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,   0 } ,				//  3 out pushpull
	{ GPIO_MODE_OUTPUT_OD, GPIO_NOPULL,   0 } ,				//  4 out opendrain
	{ GPIO_MODE_AF_PP,     GPIO_NOPULL,   GPIO_AF1_TIM1 } ,	//  5 pwm pushpull
	{ GPIO_MODE_AF_PP,     GPIO_NOPULL,   GPIO_AF1_TIM2 } ,	//  6 input capture in
	{ GPIO_MODE_AF_PP,     GPIO_NOPULL,   GPIO_AF1_TIM2 } ,	//  7 output compare pushpull
	{ GPIO_MODE_OUTPUT_OD, GPIO_PULLUP,   GPIO_AF4_I2C1 } ,	//  8 I2C opendrain pullup
	{ GPIO_MODE_AF_PP,     GPIO_PULLUP,   GPIO_AF7_USART1 },//  9 UART
    { GPIO_MODE_AF_PP,     GPIO_NOPULL,   GPIO_AF5_SPI1 },  // 10 SPI
    { GPIO_MODE_ANALOG,    GPIO_NOPULL,   0 }               // 11 analog
};
/**
 *  @brief
 *	    Sets the digital port pin mode (D0 .. D15, A0 .. A4).
 *
 *      0 in, 1 in pullup, 2 in pulldown, 3 out pushpull, 4 out open drain,
 *      5 out pwm, 6 input capture, 7 output compare, 8 I2C, 9 UART, 10 SPI, 11 analog
 *
 *      For I2C, UART, and SPI only one pin has to be activated.
 *	@param[in]
 *      pin_number    0 to 20.
 *	@param[in]
 *      mode          0 to 11
 *  @return
 *      none
 *
 */
void BSP_setDigitalPinMode(int pin_number, int mode) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// some pins have special (shared/stacked) functions
	switch (pin_number) {
	case 0:
	case 1:
		// UART: D0, D1
		if (mode != 9) {
			HAL_UART_MspDeInit(&huart1);
		}
		break;
	case 11:
	case 12:
	case 13:
		// SPI: D11, D12, and D13
		if (mode != 10) {
			HAL_SPI_MspDeInit(&hspi1);
		}
		break;
	case 14:
	case 15:
		// I2C: D14, D15
		if (mode != 8) {
			HAL_I2C_MspDeInit(&hi2c1);
		}
		break;
	}

    GPIO_InitStruct.Pin = PortPin_a[pin_number].pin;
    GPIO_InitStruct.Mode = DigitalPortPinMode_a[mode].mode;
    GPIO_InitStruct.Pull = DigitalPortPinMode_a[mode].pull;
    GPIO_InitStruct.Alternate = DigitalPortPinMode_a[mode].alternate;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PortPin_a[pin_number].port, &GPIO_InitStruct);

	// some pins have special (shared/stacked) functions
	switch (pin_number) {
	case 0:
	case 1:
		// UART: D0, D1
		if (mode == 9) {
			HAL_UART_MspInit(&huart1);
		}
		break;
	case 11:
	case 12:
	case 13:
		// SPI: D11, D12, and D13
		if (mode == 10) {
			HAL_SPI_MspInit(&hspi1);
		}
		break;
	case 14:
	case 15:
		// I2C: D14, D15
		if (mode == 8) {
			HAL_I2C_MspInit(&hi2c1);
		}
		break;
	}

	osMutexRelease(DigitalPort_MutexID);
}


// PWM
// ***

/**
 *  @brief
 *	    Sets the digital output port pin (D0=0, D1=1, A4=20) to a PWM value (0..1000).
 *
 *	@param[in]
 *      pin_number    D0=0, D1=1, D4=20
 *	@param[in]
 *      value         0 to 1000
 *  @return
 *      none
 *
 */
void BSP_setPwmPin(int pin_number, int value) {
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);


	switch (pin_number) {
	case 0:
		// D0 TIM1CH3
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, value);
		break;
	case 1:
		// D1 TIM1CH2
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, value);
		break;
	case 20:
		// A4 TIM1CH1
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, value);
		break;
	}

	osMutexRelease(DigitalPort_MutexID);
}


/**
 *  @brief
 *	    Sets the PWM prescale for TIMER1
 *
 *	@param[in]
 *      value         32 kHz / prescale, default 32 -> PWM frequency 1 kHz
 *  @return
 *      none
 *
 */
void BSP_setPwmPrescale(uint16_t value) {
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	__HAL_TIM_SET_PRESCALER(&htim1, ++value);

	osMutexRelease(DigitalPort_MutexID);
}


// Input Capture / Output Compare
// ******************************

/**
 *  @brief
 *	    Sets the input capture / output compare prescale for TIMER2.
 *
 *	    default 32 -> 32 MHz / 32 = 1 MHz, timer resolution 1 us
 *	@param[in]
 *      prescale         divider
 *  @return
 *      none
 */
void BSP_setPrescaleICOC(uint32_t prescale) {
	__HAL_TIM_SET_PRESCALER(&htim2, --prescale);
}


/**
 *  @brief
 *	    Sets the input capture / output compare (TIMER2) period. default $FFFFFFFF (4294967295)
 *
 *		When the up counter reaches the period, the counter is set to 0.
 *      For prescale 32 the maximum time is about 1 h 11 m
 *
 *	@param[in]
 *      period
 *  @return
 *      none
 */
void BSP_setPeriodICOC(uint32_t period) {
	__HAL_TIM_SET_AUTORELOAD(&htim2, period);
}


/**
 *  @brief
 *	    Sets the input capture / output compare counter for TIMER2
 *	@param[in]
 *      count
 *  @return
 *      none
 */
void BSP_setCounterICOC(uint32_t count) {
	__HAL_TIM_SET_COUNTER(&htim2, count);
}


/**
 *  @brief
 *	    Gets the input capture / output compare counter for TIMER2
 *  @return
 *      counter value
 */
uint32_t BSP_getCounterICOC(void) {
	return __HAL_TIM_GET_COUNTER(&htim2);
}


/**
 *  @brief
 *	    Starts the ICOC period
 *  @return
 *      none
 */
void BSP_startPeriodICOC(void) {
	HAL_TIM_Base_Start_IT(&htim2);
}


/**
 *  @brief
 *	    Stops the ICOC period
 *  @return
 *      none
 */
void BSP_stopPeriodICOC(void) {
	HAL_TIM_Base_Stop_IT(&htim2);
}


/**
 *  @brief
 *	    Sets the Output Compare mode
 *	@param[in]
 *	    pin_number D13=13 (TIM2CH1), A2=18 (TIM2CH3), A3=19 (TIM1CH4)
 *	@param[in]
 *      mode  0 frozen, 1 active level on match, 2 inactive level on match, 3 toggle on match
 *	          4 forced active, 5 forced inactive
 *  @return
 *      none
 */
void BSP_setModeOC(int pin_number, uint32_t mode) {
	TIM_OC_InitTypeDef sConfigOC = {0};
	uint32_t ch;

	switch (pin_number) {
	case 13:
		ch = TIM_CHANNEL_1;
		break;
	case 18:
		ch = TIM_CHANNEL_3;
		break;
	case 19:
		ch = TIM_CHANNEL_4;
		break;
	default:
		return;
	}

	sConfigOC.Pulse = __HAL_TIM_GET_COMPARE(&htim2, ch);
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	switch (mode) {
	case 0: // frozen
		sConfigOC.OCMode = TIM_OCMODE_TIMING;
		break;
	case 1: // active level on match
		sConfigOC.OCMode = TIM_OCMODE_ACTIVE;
		break;
	case 2: // inactive level on match
		sConfigOC.OCMode = TIM_OCMODE_INACTIVE;
		break;
	case 3: // toggle on match
		sConfigOC.OCMode = TIM_OCMODE_TOGGLE;
		break;
	case 4: // forced active
		sConfigOC.OCMode = TIM_OCMODE_FORCED_ACTIVE;
		break;
	case 5: // forced inactive
		sConfigOC.OCMode = TIM_OCMODE_FORCED_INACTIVE;
		break;
	default:
		Error_Handler();
		return;
	}
	if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, ch) != HAL_OK) {
		Error_Handler();
	}
}


/**
 *  @brief
 *	    Starts Output Compare
 *	@param[in]
 *	    pin_number D13=13 (TIM2CH1), A2=18 (TIM2CH3), A3=19 (TIM1CH4)
 *	@param[in]
 *	    pulse
 *  @return
 *      none
 */
void BSP_startOC(int pin_number, uint32_t pulse) {
	switch (pin_number) {
	case 13:
		osSemaphoreAcquire(ICOC_CH1_SemaphoreID, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, pulse);
		HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_1);
		break;
	case 18:
		osSemaphoreAcquire(ICOC_CH3_SemaphoreID, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);
		HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_3);
		break;
	case 19:
		osSemaphoreAcquire(ICOC_CH4_SemaphoreID, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, pulse);
		HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_4);
		break;
	}

}


/**
 *  @brief
 *	    Stops Output Compare
 *	@param[in]
 *	    pin_number D13=13 (TIM2CH1), A2=18 (TIM2CH3), A3=19 (TIM1CH4)
 *  @return
 *      none
 */
void BSP_stopOC(int pin_number) {
	switch (pin_number) {
	case 13:
		HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_1);
		break;
	case 18:
		HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_3);
		break;
	case 19:
		HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_4);
		break;
	}
}


/**
 *  @brief
 *	    Starts Input Capture
 *	@param[in]
 *      mode  0 rising edge, 1 falling edge, 2 both edges sets the output compare mode
 *  @return
 *      none
 */
void BSP_startIC(uint32_t mode) {
	switch(mode) {
	case 0:
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_RISING);
		break;
	case 1:
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_FALLING);
		break;
	case 2:
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_2, TIM_INPUTCHANNELPOLARITY_BOTHEDGE);
		break;
	default:
		return;

	}
	osSemaphoreAcquire(ICOC_CH2_SemaphoreID, 0);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2);
}


/**
 *  @brief
 *	    Stops Input Capture
 *  @return
 *      none
 */
void BSP_stopIC(void) {
	HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_2);

}


/**
 *  @brief
 *      Waits for Input Capture / Output Compare timer period.
 *  @return
 *      Captured value
 */
void BSP_waitPeriod(void) {
	osSemaphoreAcquire(ICOC_period_SemaphoreID, osWaitForever);
}


/**
 *  @brief
 *      Waits for Input Capture.
 *	@param[in]
 *      timeout  in ms
 *  @return
 *      Captured value
 */
uint32_t BSP_waitIC(uint32_t timeout) {
	if (osSemaphoreAcquire(ICOC_CH2_SemaphoreID, timeout) == osErrorTimeout) {
		return 0;
	}
	return (HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_2));
}


/**
 *  @brief
 *      Waits for Output Compare.
 *	@param[in]
 *	    pin_number D13=13 (TIM2CH1), A2=18 (TIM2CH3), A3=19 (TIM1CH4)
 *  @return
 *      none
 */
void BSP_waitOC(int pin_number) {
	switch (pin_number) {
	case 13:
		osSemaphoreAcquire(ICOC_CH1_SemaphoreID, osWaitForever);
		break;
	case 18:
		osSemaphoreAcquire(ICOC_CH3_SemaphoreID, osWaitForever);
		break;
	case 19:
		osSemaphoreAcquire(ICOC_CH4_SemaphoreID, osWaitForever);
		break;
	}
}


// EXTI
// ****

/**
 *  @brief
 *	    Sets the EXTI port pin mode
 *	@param[in]
 *	    pin_number D3=3 (PB4), D5=5 (PB3), D6=6 (PB2), D7=7 (PB1)
 *	@param[in]
 *      mode  0 rising edge, 1 falling edge, 2 both edges, 3 none
 *  @return
 *      none
 */
void BSP_setModeEXTI(int pin_number, uint32_t mode) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_TypeDef *port;

	GPIO_InitStruct.Pull = GPIO_NOPULL;

	switch (pin_number) {
	case 3:
		GPIO_InitStruct.Pin = D3_Pin;
		port = D3_GPIO_Port;
		break;
	case 5:
		GPIO_InitStruct.Pin = D5_Pin;
		port = D5_GPIO_Port;
		break;
	case 6:
		GPIO_InitStruct.Pin = D6_Pin;
		port = D6_GPIO_Port;
		break;
	case 7:
		GPIO_InitStruct.Pin = D7_Pin;
		port = D7_GPIO_Port;
		break;
	default:
		return;
	}

	switch (mode) {
	case 0: // rising edge
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		break;
	case 1: // falling edge
		GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
		break;
	case 2: // both edges
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
		break;
	case 3: // none
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		break;
	default:
		Error_Handler();
		return;
	}
	HAL_GPIO_Init(port, &GPIO_InitStruct);
}


/**
 *  @brief
 *      Waits for EXTI line (interrupt driven by port pin edge)
 *	@param[in]
 *	    pin_number D3=3 (PB4), D5=5 (PB3), D6=6 (PB2), D7=7 (PB1)
 *	@param[in]
 *      timeout  in ms
 *  @return
 *      none
 */
int BSP_waitEXTI(int pin_number, int32_t timeout) {
	osStatus_t status;
	switch (pin_number) {
	case 3:
		status = osSemaphoreAcquire(EXTI_4_SemaphoreID, timeout);
		break;
	case 5:
		status = osSemaphoreAcquire(EXTI_3_SemaphoreID, timeout);
		break;
	case 6:
		status = osSemaphoreAcquire(EXTI_2_SemaphoreID, timeout);
		break;
	case 7:
		status = osSemaphoreAcquire(EXTI_1_SemaphoreID, timeout);
		break;
	default:
		return -1;
	}

	if (status == osOK) {
		return 0;
	} else {
		// timeout or error
		return -2;
	}
}


// Private Functions
// *****************

// Callbacks
// *********

/**
  * @brief  Conversion complete callback in non-blocking mode.
  * @param hadc ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hadc);

	osSemaphoreRelease(Adc_SemaphoreID);
}


/**
  * @brief  ADC error callback in non-blocking mode
  *         (ADC conversion with interruption or transfer by DMA).
  * @note   In case of error due to overrun when using ADC with DMA transfer
  *         (HAL ADC handle parameter "ErrorCode" to state "HAL_ADC_ERROR_OVR"):
  *         - Reinitialize the DMA using function "HAL_ADC_Stop_DMA()".
  *         - If needed, restart a new ADC conversion using function
  *           "HAL_ADC_Start_DMA()"
  *           (this function is also clearing overrun flag)
  * @param hadc ADC handle
  * @retval None
  */
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
	/* Prevent unused argument(s) compilation warning */
	UNUSED(hadc);

	Error_Handler();
	osSemaphoreRelease(Adc_SemaphoreID);
}


// Input Capture / Output Compare Interrupt Callbacks
// **************************************************

/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void BSP_TIM2_PeriodElapsedCallback() {
	//		/* Get configured autoreload value */
	//		autoreload_value = __HAL_TIM_GET_AUTORELOAD(htim);
	osSemaphoreRelease(ICOC_period_SemaphoreID);
}


/**
  * @brief  Output Compare callback in non-blocking mode
  * @param  htim TIM OC handle
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
			// D5, PA15
			osSemaphoreRelease(ICOC_CH1_SemaphoreID);
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
			// D1, PA2
			osSemaphoreRelease(ICOC_CH3_SemaphoreID);
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
			// D0, PA3
			osSemaphoreRelease(ICOC_CH4_SemaphoreID);
		}
	}
}


/**
  * @brief  Input Capture callback in non-blocking mode
  * @param  htim TIM IC handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
			// A2, PA1
			osSemaphoreRelease(ICOC_CH2_SemaphoreID);
		}
	}

}


/**
  * @brief  EXTI line detection callback.
  * @param GPIO_Pin Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	//    pin_number D3=3 (PB4), D5=5 (PB3), D6=6 (PB2), D7=7 (PB1)
	switch (GPIO_Pin) {
	case GPIO_PIN_1:  // D7
		osSemaphoreRelease(EXTI_1_SemaphoreID);
		break;
	case GPIO_PIN_2:  // D6
		osSemaphoreRelease(EXTI_2_SemaphoreID);
		break;
	case GPIO_PIN_3:  // D5
		osSemaphoreRelease(EXTI_3_SemaphoreID);
		break;
	case GPIO_PIN_4:  // D3
		osSemaphoreRelease(EXTI_4_SemaphoreID);
		break;

	}
}


// NeoPixel LED
// ************

/**
 *  @brief
 *	    Sets the NeoPixel RGB LED.
 *
 *	@param[in]
 *      rgb    Lowest (1st) byte blue, 2nd byte green, 3th byte red
 *  @return
 *      none
 *
 */
void BSP_setNeoPixel(uint32_t rgb) {
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// do not disturb, it takes about 1.25 us * 24 = 30 us
	BACKUP_PRIMASK();
	DISABLE_IRQ();

	BSP_neopixelDataTx(D9_GPIO_Port, D9_Pin, rgb);

	RESTORE_PRIMASK();

	neo_pixel = rgb;
	osMutexRelease(DigitalPort_MutexID);
	osDelay(1);
}


/**
 *  @brief
 *	    Get the NeoPixel RGB LED.
 *
 *  @return
 *      Lowest (1st) byte blue, 2nd byte green, 3th byte red
 *
 */
int BSP_getNeoPixel(void) {
	return neo_pixel;
}


/**
 *  @brief
 *	    Sets the NeoPixel RGB LEDs.
 *
 *	@param[in]
 *      buffer    array of pixels
 *	@param[in]
 *      len       array length
 *  @return
 *      none
 *
 */
void BSP_setNeoPixels(uint32_t *buffer, uint32_t len) {

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// do not disturb, it takes about 1.25 us * 24 = 30 us
	BACKUP_PRIMASK();
	DISABLE_IRQ();

	BSP_neopixelBufferTx(D6_GPIO_Port, D6_Pin, buffer, len);

	RESTORE_PRIMASK();

	osMutexRelease(DigitalPort_MutexID);
	osDelay(1);
}


/**
 *  @brief
 *	    Set the system LED.
 *
 *		SYSLED_ACTIVATE 			= 1 << 0,
 *		SYSLED_DISK_READ_OPERATION 	= 1 << 1, yellow
 *		SYSLED_DISK_WRITE_OPERATION = 1 << 2, red
 *		SYSLED_CHARGING 			= 1 << 3, dimmed red
 *		SYSLED_FULLY_CHARGED 		= 1 << 4, dimmed green
 *		SYSLED_BLE_CONNECTED 		= 1 << 5, dimmed blue
 *	@param[in]
 *      status
 *  @return
 *      none
 *
 */
void BSP_setSysLED(BSP_sysled_t status) {
	sys_led_status |= status;
	update_sysled();
}

/**
 *  @brief
 *	    Clear the system LED.
 *
 *		SYSLED_ACTIVATE 			= 1 << 0,
 *		SYSLED_DISK_READ_OPERATION 	= 1 << 1,
 *		SYSLED_DISK_WRITE_OPERATION = 1 << 2,
 *		SYSLED_CHARGING 			= 1 << 3,
 *		SYSLED_FULLY_CHARGED 		= 1 << 4,
 *		SYSLED_BLE_CONNECTED 		= 1 << 5,
 *	@param[in]
 *      status
 *  @return
 *      none
 *
 */
void BSP_clearSysLED(BSP_sysled_t status) {
	sys_led_status &= ~status;
	update_sysled();
}

static void update_sysled(void) {
	uint32_t rgb = 0;
	if (sys_led_status & SYSLED_ACTIVATE) {
		if (sys_led_status & SYSLED_DISK_READ_OPERATION) {
			// bright green
			rgb = 0x00FF00;
		} else if (sys_led_status & SYSLED_DISK_WRITE_OPERATION) {
			// bright yellow
			rgb = 0xFFFF00;
		} else if (sys_led_status & SYSLED_CHARGING) {
			// red
			rgb = 0x200000;
		} else if (sys_led_status & SYSLED_FULLY_CHARGED) {
			// green
			rgb = 0x002000;
		} else if (sys_led_status & SYSLED_POWER_ON) {
			// white
			rgb = 0x404040;
		} else if (sys_led_status & SYSLED_ERROR) {
			// red
			rgb = 0x400000;
		}
		if (sys_led_status & SYSLED_BLE_CONNECTED) {
			// add some blue
			rgb |= 0x000020;
		}
		BSP_setNeoPixel(rgb);
	}
}

