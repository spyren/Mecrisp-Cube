/**
 *  @brief
 *      Board Support Package for STM32WB Flipper Zero
 *
 *        - LEDs see rbgw.c module
 *        - Switches (SW1, SW2, SW3, SW4, SW5, S6)
 *        - Analog port pins A0 to A2
 *        - Digital port pins D0 to D4 and D9 to D13
 *          - PWM: D11 TIM1CH1, D4 TIM1CH2
 *          - SPI: D11 MOSI, D12 MISO, D13 SCK
 *          - I2C: A0 I2C3_SCL, A1 I2C3_SDA
 *          - Timer Capture/Compare D9 TIM2_CH2
 *        - Vibro
 *        - NeoPixel D4
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
#include "bsp.h"
#include "power.h"
#include "rgbw.h"


// Private function prototypes
// ***************************
static void update_sysled(void);

// Global Variables
// ****************
const char BSP_Version[] = "  * Firmware Package STM32Cube FW_WB V1.17.3, USB-CDC, BLE Stack 5.3 (C) 2023 STMicroelectronics \n";
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

static osSemaphoreId_t EXTI_2_SemaphoreID;
static osSemaphoreId_t EXTI_3_SemaphoreID;
static osSemaphoreId_t EXTI_4_SemaphoreID;
static osSemaphoreId_t EXTI_9_5_SemaphoreID;


// Private Variables
// *****************
ADC_ChannelConfTypeDef sConfig = {0};
extern ADC_HandleTypeDef hadc1;
uint32_t neo_pixel = 0;

static uint32_t adc_calibration;

static int sys_led_status = SYSLED_ACTIVATE;

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
	EXTI_9_5_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_9_5_SemaphoreID == NULL) {
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



// Switches
// ********

/**
 *  @brief
 *      Gets the switch1 (OK button) state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch1(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// Flipper OK button
	if (HAL_GPIO_ReadPin(BUTTON_OK_GPIO_Port, BUTTON_OK_Pin) == GPIO_PIN_SET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

/**
 *  @brief
 *      Gets the switch2 (BACK button) state
 *
 *      No debouncing. There is a pulldown resistor.
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch2(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// Flipper BACK button
	if (HAL_GPIO_ReadPin(BUTTON_BACK_GPIO_Port, BUTTON_BACK_Pin) == GPIO_PIN_RESET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

/**
 *  @brief
 *      Gets the switch3 (UP button) state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch3(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// Flipper UP button
	if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin) == GPIO_PIN_RESET) {
		return_value =  -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}


/**
 *  @brief
 *      Gets the switch4 (DOWN) state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch4(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// Flipper DOWN button
	if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin) == GPIO_PIN_RESET) {
		return_value =  -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}


/**
 *  @brief
 *      Gets the switch5 (LEFT button) state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch5(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// Flipper LEFT button
	if (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin) == GPIO_PIN_RESET) {
		return_value =  -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}


/**
 *  @brief
 *      Gets the switch5 (RIGHT button) state
 *
 *      No debouncing
 *  @return
 *      FALSE for open switch, TRUE for closed (pressed) switch.
 */
int BSP_getSwitch6(void) {
	int return_value;

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	// Flipper RIGHT button
	if (HAL_GPIO_ReadPin(BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin) == GPIO_PIN_RESET) {
		return_value =  -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
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
		{ D0_GPIO_Port, D0_Pin } ,
		{ D0_GPIO_Port, D0_Pin } ,
		{ D0_GPIO_Port, D0_Pin } ,
		{ D0_GPIO_Port, D0_Pin } ,
		{ D9_GPIO_Port, D9_Pin } ,
		{ D10_GPIO_Port, D10_Pin } ,
		{ D11_GPIO_Port, D11_Pin } ,
		{ D12_GPIO_Port, D12_Pin } ,
		{ D13_GPIO_Port, D13_Pin } ,
		{ D0_GPIO_Port, D0_Pin } ,
		{ D0_GPIO_Port, D0_Pin }
};

/**
 *  @brief
 *	    Sets the digital output port pins (D0 .. D4, D9 .. D13).
 *
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
 *	    Gets the digital output port pins (D0 .. D4, D9 .. D13).
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
 *	    Sets the digital output port pin (D0 .. D4, D9 .. D13).
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
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

	HAL_GPIO_WritePin(PortPin_a[pin_number].port, PortPin_a[pin_number].pin, state);

	osMutexRelease(DigitalPort_MutexID);
}


/**
 *  @brief
 *	    Gets the digital input port pin (D0 .. D4, D9 .. D13).
 *
 *	@param[in]
 *      pin_number    0 to 15.
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


// analog port pins A0 to A2 (Arduino numbering)
// *********************************************
static const uint32_t AnalogPortPin_a[3] = {
		ADC_CHANNEL_1, // A0 PC0
		ADC_CHANNEL_2, // A1 PC1
		ADC_CHANNEL_4, // A2 PC3
};

/**
 *  @brief
 *	    Gets the analog input port pin (A0 .. A2) ADC value.
 *
 *	@param[in]
 *      pin_number    0 to 2.
 *  @return
 *      12 bit ADC value *
 */
int BSP_getAnalogPin(int pin_number) {
	int return_value;
	HAL_StatusTypeDef status;

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

	// only one thread is allowed to use the ADC
	osMutexAcquire(Adc_MutexID, osWaitForever);

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

	osMutexRelease(Adc_MutexID);

	return 3 * __HAL_ADC_CALC_DATA_TO_VOLTAGE(ref_voltage_mv, value, ADC_RESOLUTION_12B) ;
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
	{ GPIO_MODE_INPUT,     GPIO_NOPULL,   0 } ,				// 0 in
	{ GPIO_MODE_INPUT,     GPIO_PULLUP,   0 } ,				// 1 pullup
	{ GPIO_MODE_INPUT,     GPIO_PULLDOWN, 0 } ,				// 2 pulldow
	{ GPIO_MODE_OUTPUT_PP, GPIO_NOPULL,   0 } ,				// 3 pushpull
	{ GPIO_MODE_OUTPUT_OD, GPIO_NOPULL,   0 } ,				// 4 opendrain
	{ GPIO_MODE_AF_PP,     GPIO_NOPULL,   GPIO_AF1_TIM1 } ,	// 5 pwm pushpull
	{ GPIO_MODE_AF_PP,     GPIO_NOPULL,   GPIO_AF1_TIM2 } ,	// 6 input capture in
	{ GPIO_MODE_AF_PP,     GPIO_NOPULL,   GPIO_AF1_TIM2 } ,	// 7 output compare pushpull
	{ GPIO_MODE_OUTPUT_OD, GPIO_PULLUP,   GPIO_AF4_I2C1 }  	// 8 I2C opendrain pullup
};
/**
 *  @brief
 *	    Sets the digital port pin mode (D0 .. D4, D9 .. D13).
 *
 *      0 in, 1 in pullup, 2 in pulldown, 3 out pushpull, 4 out open drain,
 *      5 out pwm, 6 input capture, 7 output compare, 8 I2C
 *	@param[in]
 *      pin_number    0 to 15.
 *	@param[in]
 *      mode          0 to 8
 *  @return
 *      none
 *
 */
void BSP_setDigitalPinMode(int pin_number, int mode) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);

    GPIO_InitStruct.Pin = PortPin_a[pin_number].pin;
    GPIO_InitStruct.Mode = DigitalPortPinMode_a[mode].mode;
    GPIO_InitStruct.Pull = DigitalPortPinMode_a[mode].pull;
    GPIO_InitStruct.Alternate = DigitalPortPinMode_a[mode].alternate;
//    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(PortPin_a[pin_number].port, &GPIO_InitStruct);

	osMutexRelease(DigitalPort_MutexID);
}


// PWM
// ***

/**
 *  @brief
 *	    Sets the digital output port pin (D11=11, D4=4) to a PWM value (0..1000).
 *
 *	@param[in]
 *      pin_number    D11=11, D4=4
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
	case 11:
		// D11 TIM1CH1 N
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 1000 - value);
		break;
	case 4:
		// D4 TIM1CH2 N
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 1000 - value);
		break;
	}

	osMutexRelease(DigitalPort_MutexID);
}


/**
 *  @brief
 *	    Sets the PWM prescale for TIMER1 (D3=3, D6=6, D9=9)
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
 *	    pin_number D9=9
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
	case 9:
		ch = TIM_CHANNEL_2;
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
 *	    pin_number D9
 *	@param[in]
 *	    pulse
 *  @return
 *      none
 */
void BSP_startOC(int pin_number, uint32_t pulse) {
	switch (pin_number) {
	case 9:
		osSemaphoreAcquire(ICOC_CH2_SemaphoreID, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
		HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_2);
		break;
	}

}


/**
 *  @brief
 *	    Stops Output Compare
 *	@param[in]
 *	    pin_number
 *  @return
 *      none
 */
void BSP_stopOC(int pin_number) {
	switch (pin_number) {
	case 9:
		HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_2);
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
 *      pin_number  port pin 9 D9
 *  @return
 *      none
 */
void BSP_waitOC(int pin_number) {
	switch (pin_number) {
	case 9:
		osSemaphoreAcquire(ICOC_CH2_SemaphoreID, osWaitForever);
		break;
	}
}


// EXTI
// ****

/**
 *  @brief
 *	    Sets the EXTI port pin mode
 *	@param[in]
 *	    pin_number port pin 9 D9, 10 D10, 11 D11, 12 D12, D11 and D12 share the same EXTI line
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
	case 9:
		GPIO_InitStruct.Pin = D9_Pin;
		port = D9_GPIO_Port;
		break;
	case 10:
		GPIO_InitStruct.Pin = D10_Pin;
		port = D10_GPIO_Port;
		break;
	case 11:
		GPIO_InitStruct.Pin = D11_Pin;
		port = D11_GPIO_Port;
		break;
	case 12:
		GPIO_InitStruct.Pin = D12_Pin;
		port = D12_GPIO_Port;
		break;
	case 13:
		GPIO_InitStruct.Pin = D13_Pin;
		port = D13_GPIO_Port;
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
 *	    pin_number port pin 9 D9, 10 D10, 11 D11, 12 D12, D11 and D12 share the same EXTI line
 *	@param[in]
 *      timeout  in ms
 *  @return
 *      none
 */
int BSP_waitEXTI(int pin_number, int32_t timeout) {
	osStatus_t status;
	switch (pin_number) {
	case 9:
		status = osSemaphoreAcquire(EXTI_2_SemaphoreID, timeout);
		break;
	case 10:
		status = osSemaphoreAcquire(EXTI_4_SemaphoreID, timeout);
		break;
	case 11:
		status = osSemaphoreAcquire(EXTI_9_5_SemaphoreID, timeout);
		break;
	case 12:
		status = osSemaphoreAcquire(EXTI_9_5_SemaphoreID, timeout);
		break;
	case 13:
		status = osSemaphoreAcquire(EXTI_3_SemaphoreID, timeout);
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
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
			// D9, PB2
			osSemaphoreRelease(ICOC_CH2_SemaphoreID);
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
	switch (GPIO_Pin) {
	case GPIO_PIN_2:  // D9
		osSemaphoreRelease(EXTI_2_SemaphoreID);
		break;
	case GPIO_PIN_3:  // D13
		osSemaphoreRelease(EXTI_3_SemaphoreID);
		break;
	case GPIO_PIN_4:  // D10
		osSemaphoreRelease(EXTI_4_SemaphoreID);
		break;
	case GPIO_PIN_6:  // D2
		osSemaphoreRelease(EXTI_9_5_SemaphoreID);
		break;
	case GPIO_PIN_13: // B2 (switch2)
		POWER_switchEvent(POWER_SWITCH1);
	default:
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

	BSP_neopixelBufferTx(D4_GPIO_Port, D4_Pin, buffer, len);

	RESTORE_PRIMASK();

	osMutexRelease(DigitalPort_MutexID);
	osDelay(1);
}


/**
 *  @brief
 *	    Set vibro.
 *
 *	@param[in]
 *      status    0 off, 1 on
 *  @return
 *      none
 *
 */
void BSP_setVibro(int status) {
	if (status) {
		HAL_GPIO_WritePin(VIBRO_GPIO_Port, VIBRO_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(VIBRO_GPIO_Port, VIBRO_Pin, GPIO_PIN_RESET);
	}
}


/**
 *  @brief
 *	    Get vibro status.
 *
 *  @return
 *      0 off, 1 on
 *
 */
int BSP_getVibro(void) {
	if (HAL_GPIO_ReadPin(VIBRO_GPIO_Port, VIBRO_Pin) == GPIO_PIN_SET) {
		return 1;
	} else {
		return 0;
	}
}


/**
 *  @brief
 *	    Set the system LED.
 *
 *	@param[in]
 *      status
 *  @return
 *      none
 *
 */
void BSP_setSysLED(int status) {
	sys_led_status |= status;
	update_sysled();
}

void BSP_clearSysLED(int status) {
	sys_led_status &= ~status;
	update_sysled();
}

static void update_sysled(void) {
	uint32_t rgb = 0;
	if (sys_led_status & SYSLED_ACTIVATE) {
		if (sys_led_status & SYSLED_DISK_READ_OPERATION) {
			// bright yellow
			rgb = 0xFFFF00;
		} else if (sys_led_status & SYSLED_DISK_WRITE_OPERATION) {
			// bright red
			rgb = 0xFF0000;
		} else if (sys_led_status & SYSLED_CHARGING) {
			// red
			rgb = 0x200000;
		} else if (sys_led_status & SYSLED_FULLY_CHARGED) {
			// green
			rgb = 0x002000;
		}
		if (sys_led_status & SYSLED_BLE_CONNECTED) {
			// add some blue
			rgb |= 0x000020;
		}
		RGBW_setRGB(rgb);
	}
}
