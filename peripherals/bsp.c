/**
 *  @brief
 *      Board Support Package for STM32WB5M Discovery Board.
 *
 *        - LEDs (LD4 RGB-LED)
 *        - Switches (SW1, SW2)
 *        - Digital port pins D0 to D15
 *        - Analog port pins A0 to A5
 *        - PWM: D3 TIM1CH1, D6 TIM16CH1, D9 TIM1CH2
 *        - SPI: D11 MOSI, D12 MISO, D13 SCK (display, memory)
 *        - Timer Capture/Compare
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
#include "rt_spi.h"

// Private function prototypes
// ***************************
uint32_t easy_set(uint8_t in_pattern);
uint32_t swap_uint32(uint32_t val);

// Global Variables
// ****************
const char BSP_Version[] =
	    "  * Firmware Package STM32Cube FW_WB V1.14.1, USB-CDC, BLE Stack 5.3 (C) 2022 STMicroelectronics \n";
extern TIM_HandleTypeDef htim2;

// Hardware resources
// ******************
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim16;

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

static osSemaphoreId_t EXTI_0_SemaphoreID;
static osSemaphoreId_t EXTI_9_5_SemaphoreID;
static osSemaphoreId_t EXTI_15_10_SemaphoreID;
static osSemaphoreId_t EXTI_3_SemaphoreID;
static osSemaphoreId_t EXTI_4_SemaphoreID;

// Private Variables
// *****************
ADC_ChannelConfTypeDef sConfig = { 0 };
extern ADC_HandleTypeDef hadc1;
uint32_t neo_pixel = 0;
uint32_t rgb_led = 0;
uint32_t rgb_buffer[16];
static uint32_t adc_calibration;

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

	EXTI_4_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_4_SemaphoreID == NULL) {
		Error_Handler();
	}
	EXTI_9_5_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_9_5_SemaphoreID == NULL) {
		Error_Handler();
	}
	EXTI_15_10_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (EXTI_15_10_SemaphoreID == NULL) {
		Error_Handler();
	}

	// ADC calibration
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
//	adc_calibration = HAL_ADCEx_Calibration_GetValue(&hadc1, ADC_SINGLE_ENDED);
//	HAL_ADCEx_Calibration_SetValue(&hadc1, ADC_SINGLE_ENDED, adc_calibration);

	// Configure Regular Channel
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_92CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}

	// start PWM
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);

}

// LEDs
// ****

// RGB LED
// ************

/**
 *  @brief
 *	    Sets the RGB LED.
 *
 *	    The SPI baudrate has to be lower than 2.4 MHz
 *	    For 2 MHz it takes about 20 * 0.5 us = 10 us.
 *	@param[in]
 *      rgb    Lowest (1st) byte blue, 2nd byte green, 3th byte red
 *  @return
 *      none
 *
 */
void BSP_setRgbLED(uint32_t rgb) {
	// only one thread is allowed to use the digital port
	osMutexAcquire(DigitalPort_MutexID, osWaitForever);
	osMutexAcquire(RTSPI_MutexID, osWaitForever);

	rgb_led = rgb;
	RTSPI_Write(0b00000000); // give some time & to be sure the MOSI is low
	HAL_GPIO_WritePin(RGB_SELECT_GPIO_Port, RGB_SELECT_Pin, GPIO_PIN_SET);

	rgb_buffer[0] = easy_set(0b00111010); 			// "start" byte
	rgb_buffer[1] = easy_set(rgb >> 16);			// red
	rgb_buffer[2] = easy_set((rgb >> 8) & 0xFF);	// green
	rgb_buffer[3] = easy_set(rgb & 0xFF);			// blue
	RTSPI_WriteReadData((uint8_t *)rgb_buffer, (uint8_t *)rgb_buffer, 16);
	RTSPI_Write(0b10001000); // GSLAT

	HAL_GPIO_WritePin(RGB_SELECT_GPIO_Port, RGB_SELECT_Pin, GPIO_PIN_RESET);

	osMutexRelease(RTSPI_MutexID);
	osMutexRelease(DigitalPort_MutexID);
	osDelay(1);
}

/**
 *  @brief
 *	    Get the RGB LED.
 *
 *  @return
 *      Lowest (1st) byte blue, 2nd byte green, 3th byte red
 *
 */
int BSP_getRgbLED(void) {
	return rgb_led;
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

	if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

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

	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_RESET) {
		return_value = -1;
	} else {
		return_value = FALSE;
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

// digital port pins D0 to D15 (Arduino numbering)
// ***********************************************

typedef struct {
	GPIO_TypeDef *port;
	uint16_t pin;
} PortPin_t;

static const PortPin_t PortPin_a[22] = { { D0_GPIO_Port, D0_Pin }, {
		D1_GPIO_Port, D1_Pin }, { D2_GPIO_Port, D2_Pin },
		{ D3_GPIO_Port, D3_Pin }, { D4_GPIO_Port, D4_Pin }, { D5_GPIO_Port,
				D5_Pin }, { D6_GPIO_Port, D6_Pin }, { D7_GPIO_Port, D7_Pin }, {
				D8_GPIO_Port, D9_Pin }, { D9_GPIO_Port, D9_Pin }, {
				D10_GPIO_Port, D10_Pin }, { D11_GPIO_Port, D11_Pin }, {
				D12_GPIO_Port, D12_Pin }, { D13_GPIO_Port, D13_Pin }, {
				D14_GPIO_Port, D14_Pin }, { D15_GPIO_Port, D15_Pin }, {
				A0_GPIO_Port, A0_Pin }, { A1_GPIO_Port, A1_Pin }, {
				A2_GPIO_Port, A2_Pin }, { A3_GPIO_Port, A3_Pin }, {
				A4_GPIO_Port, A4_Pin }, { A5_GPIO_Port, A5_Pin } };

/**
 *  @brief
 *	    Sets the digital output port pins (0 D0 .. 15 D15) .
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

	for (i = 0; i < 16; i++) {
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

	for (i = 15; i > -1; i--) {
		return_value = return_value << 1;
		return_value |= HAL_GPIO_ReadPin(PortPin_a[i].port, PortPin_a[i].pin);
	}

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

/**
 *  @brief
 *	    Sets the digital output port pin (0 D0 .. 15 D15) and analog pins (16 A1 .. 21 A2)..
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

	HAL_GPIO_WritePin(PortPin_a[pin_number].port, PortPin_a[pin_number].pin,
			state);

	osMutexRelease(DigitalPort_MutexID);
}

/**
 *  @brief
 *	    Gets the digital input port pin (0 D0 .. 15 D15) and analog pins (16 A1 .. 21 A2)..
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

	int return_value = HAL_GPIO_ReadPin(PortPin_a[pin_number].port,
			PortPin_a[pin_number].pin);

	osMutexRelease(DigitalPort_MutexID);
	return return_value;
}

// analog port pins A0 to A5 (Arduino numbering)
// *********************************************
static const uint32_t AnalogPortPin_a[6] = {
ADC_CHANNEL_4,  // A0 PC3
		ADC_CHANNEL_7,  // A1 PA2
		ADC_CHANNEL_10, // A2 PA5
		ADC_CHANNEL_2,  // A3 PC1
		ADC_CHANNEL_13, // A4 PC4
		ADC_CHANNEL_14  // A5 PC5
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
	HAL_ADC_Stop(&hadc1);

	osMutexRelease(Adc_MutexID);
	return return_value;
}

/**
 *  @brief
 *	    Get the Vref
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
	HAL_ADC_Stop(&hadc1);

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
	HAL_ADC_Stop(&hadc1);

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
	HAL_ADC_Stop(&hadc1);

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

static const PortPinMode_t DigitalPortPinMode_a[] = { { GPIO_MODE_INPUT,
		GPIO_NOPULL, 0 },				// 0 in
		{ GPIO_MODE_INPUT, GPIO_PULLUP, 0 },				// 1 pullup
		{ GPIO_MODE_INPUT, GPIO_PULLDOWN, 0 },				// 2 pulldow
		{ GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0 },				// 3 pushpull
		{ GPIO_MODE_OUTPUT_OD, GPIO_NOPULL, 0 },				// 4 opendrain
		{ GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM1 },	// 5 pwm pushpull
		{ GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2 },// 6 input capture in
		{ GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM2 },// 7 output compare pushpull
		{ GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_AF4_I2C1 },// 8 I2C opendrain pullup
		{ GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_AF7_USART1 },	// 9 USART pullup
		{ GPIO_MODE_ANALOG, GPIO_NOPULL, 0 } 				// 10 analog in
};
/**
 *  @brief
 *	    Sets the digital port pin mode (D0 .. D15).
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
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

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
 *	    Sets the digital output port pin (D3=3, D6=6, D9=9) to a PWM value (0..1000).
 *
 *	@param[in]
 *      pin_number    D3=3, D6=6, D9=9
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
	case 3:
		// D3 TIM1CH1
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, value);
		break;
	case 6:
		// D6 TIM16CH1
		__HAL_TIM_SET_COMPARE(&htim16, TIM_CHANNEL_1, value);
		break;
	case 9:
		// D9 TIM1CH2
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, value);
		break;
	}

	osMutexRelease(DigitalPort_MutexID);
}

/**
 *  @brief
 *	    Sets the PWM prescale for TIMER1 (D3=3, D9=9) and TIMER16 (D6=6)
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
	__HAL_TIM_SET_PRESCALER(&htim16, ++value);

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
 *	    pin_number
 *	@param[in]
 *      mode  0 frozen, 1 active level on match, 2 inactive level on match, 3 toggle on match
 *	          4 forced active, 5 forced inactive
 *  @return
 *      none
 */
void BSP_setModeOC(int pin_number, uint32_t mode) {
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	uint32_t ch;

	switch (pin_number) {
	case 5:
		ch = TIM_CHANNEL_3;
		break;
	case 13:
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
 *	    pin_number
 *	@param[in]
 *	    pulse
 *  @return
 *      none
 */
void BSP_startOC(int pin_number, uint32_t pulse) {
	switch (pin_number) {
	case 5:
		osSemaphoreAcquire(ICOC_CH3_SemaphoreID, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, pulse);
		HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_3);
		break;
	case 13:
		osSemaphoreAcquire(ICOC_CH2_SemaphoreID, 0);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, pulse);
		HAL_TIM_OC_Start_IT(&htim2, TIM_CHANNEL_2);
		break;
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
	case 5:
		HAL_TIM_OC_Stop_IT(&htim2, TIM_CHANNEL_3);
		break;
	case 13:
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
	switch (mode) {
	case 0:
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1,
				TIM_INPUTCHANNELPOLARITY_RISING);
		break;
	case 1:
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1,
				TIM_INPUTCHANNELPOLARITY_FALLING);
		break;
	case 2:
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim2, TIM_CHANNEL_1,
				TIM_INPUTCHANNELPOLARITY_BOTHEDGE);
		break;
	default:
		return;

	}
	osSemaphoreAcquire(ICOC_CH1_SemaphoreID, 0);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}

/**
 *  @brief
 *	    Stops Input Capture
 *  @return
 *      none
 */
void BSP_stopIC(void) {
	HAL_TIM_IC_Stop_IT(&htim2, TIM_CHANNEL_1);

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
	if (osSemaphoreAcquire(ICOC_CH1_SemaphoreID, timeout) == osErrorTimeout) {
		return 0;
	}
	return (HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1));
}

/**
 *  @brief
 *      Waits for Output Compare.
 *	@param[in]
 *      pin_number  port pin 5 D5, 13 D13
 *  @return
 *      none
 */
void BSP_waitOC(int pin_number) {
	switch (pin_number) {
	case 5:
		osSemaphoreAcquire(ICOC_CH3_SemaphoreID, osWaitForever);
		break;
	case 13:
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
 *	    pin_number port pin 0 D0, 1 D1, 2 D2, 4 D4, 10 D10
 *	@param[in]
 *      mode  0 rising edge, 1 falling edge, 2 both edges, 3 none
 *  @return
 *      none
 */
void BSP_setModeEXTI(int pin_number, uint32_t mode) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	GPIO_TypeDef *port;

	GPIO_InitStruct.Pull = GPIO_NOPULL;

	switch (pin_number) {
	case 0:
		GPIO_InitStruct.Pin = D0_Pin;
		port = D0_GPIO_Port;
		break;
	case 1:
		GPIO_InitStruct.Pin = D1_Pin;
		port = D1_GPIO_Port;
		break;
	case 2:
		GPIO_InitStruct.Pin = D2_Pin;
		port = D2_GPIO_Port;
		break;
	case 4:
		GPIO_InitStruct.Pin = D4_Pin;
		port = D4_GPIO_Port;
		break;
	case 10:
		GPIO_InitStruct.Pin = D10_Pin;
		port = D10_GPIO_Port;
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
 *      pin_number port pin 0 D0, 1 D1, 2 D2, 4 D4, 10 D10
 *	@param[in]
 *      timeout  in ms
 *  @return
 *      none
 */
int BSP_waitEXTI(int pin_number, int32_t timeout) {
	osStatus_t status;
	switch (pin_number) {
	case 0:
		status = osSemaphoreAcquire(EXTI_0_SemaphoreID, timeout);
		break;
	case 1:
		status = osSemaphoreAcquire(EXTI_9_5_SemaphoreID, timeout);
		break;
	case 2:
		status = osSemaphoreAcquire(EXTI_15_10_SemaphoreID, timeout);
		break;
	case 4:
		status = osSemaphoreAcquire(EXTI_3_SemaphoreID, timeout);
		break;
	case 10:
		status = osSemaphoreAcquire(EXTI_4_SemaphoreID, timeout);
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
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
			// D5, PA15
			osSemaphoreRelease(ICOC_CH3_SemaphoreID);
		}
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
			// D1, PA2
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
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
			// A2, PA1
			osSemaphoreRelease(ICOC_CH1_SemaphoreID);
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
	case GPIO_PIN_0:  // D0
		osSemaphoreRelease(EXTI_0_SemaphoreID);
		break;
	case GPIO_PIN_5:  // D1
		osSemaphoreRelease(EXTI_9_5_SemaphoreID);
		break;
	case GPIO_PIN_12: // D2
		osSemaphoreRelease(EXTI_15_10_SemaphoreID);
		break;
	case GPIO_PIN_3: // D4
		osSemaphoreRelease(EXTI_3_SemaphoreID);
		break;
	case GPIO_PIN_4: // D10
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
 *	    TI's Single-Wire Interface.
 *
 *	@param[in]
 *      byte    bits to convert to bit stuffed.
 *  @return
 *      single-wire pattern, 32 bit
 *
 */
uint32_t easy_set(uint8_t in_pattern) {
	uint32_t out_pattern = 0;
	int i;

	for (i = 0; i < 8; i++) {
		out_pattern = out_pattern << 4;
		if (in_pattern & 0b10000000) {
			out_pattern |= 0b00001010;
		} else {
			out_pattern |= 0b00001000;
		}
		in_pattern = in_pattern << 1;
	}
	return swap_uint32(out_pattern);
}

//! Byte swap unsigned int
uint32_t swap_uint32(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}
