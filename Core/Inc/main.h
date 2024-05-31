/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbxx_hal.h"

#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define D7_Pin GPIO_PIN_13
#define D7_GPIO_Port GPIOC
#define D15_Pin GPIO_PIN_8
#define D15_GPIO_Port GPIOB
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOB
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOC
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOC
#define A5_Pin GPIO_PIN_2
#define A5_GPIO_Port GPIOC
#define A4_Pin GPIO_PIN_3
#define A4_GPIO_Port GPIOC
#define A3_Pin GPIO_PIN_0
#define A3_GPIO_Port GPIOA
#define A2_Pin GPIO_PIN_1
#define A2_GPIO_Port GPIOA
#define D1_Pin GPIO_PIN_2
#define D1_GPIO_Port GPIOA
#define D0_Pin GPIO_PIN_3
#define D0_GPIO_Port GPIOA
#define D10_Pin GPIO_PIN_4
#define D10_GPIO_Port GPIOA
#define D13_Pin GPIO_PIN_5
#define D13_GPIO_Port GPIOA
#define D12_Pin GPIO_PIN_6
#define D12_GPIO_Port GPIOA
#define D11_Pin GPIO_PIN_7
#define D11_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_8
#define D6_GPIO_Port GPIOA
#define D9_Pin GPIO_PIN_9
#define D9_GPIO_Port GPIOA
#define B1_Pin GPIO_PIN_4
#define B1_GPIO_Port GPIOC
#define DONGLE_SPI_CS_Pin GPIO_PIN_2
#define DONGLE_SPI_CS_GPIO_Port GPIOB
#define LD2_Pin GPIO_PIN_0
#define LD2_GPIO_Port GPIOB
#define LD3_Pin GPIO_PIN_1
#define LD3_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_6
#define D2_GPIO_Port GPIOC
#define D3_Pin GPIO_PIN_10
#define D3_GPIO_Port GPIOA
#define JTMS_Pin GPIO_PIN_13
#define JTMS_GPIO_Port GPIOA
#define JTCK_Pin GPIO_PIN_14
#define JTCK_GPIO_Port GPIOA
#define D5_Pin GPIO_PIN_15
#define D5_GPIO_Port GPIOA
#define D4_Pin GPIO_PIN_10
#define D4_GPIO_Port GPIOC
#define D8_Pin GPIO_PIN_12
#define D8_GPIO_Port GPIOC
#define B2_Pin GPIO_PIN_0
#define B2_GPIO_Port GPIOD
#define B3_Pin GPIO_PIN_1
#define B3_GPIO_Port GPIOD
#define JTDO_Pin GPIO_PIN_3
#define JTDO_GPIO_Port GPIOB
#define LD1_Pin GPIO_PIN_5
#define LD1_GPIO_Port GPIOB
#define STLINK_RX_Pin GPIO_PIN_6
#define STLINK_RX_GPIO_Port GPIOB
#define STLINK_TX_Pin GPIO_PIN_7
#define STLINK_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define B1_DONGLE_Pin GPIO_PIN_10
#define B1_DONGLE_GPIO_Port GPIOA
#define LD1_DONGLE_Pin GPIO_PIN_4
#define LD1_DONGLE_GPIO_Port GPIOA

#define COL0_Pin 		A3_Pin
#define COL0_GPIO_Port 	A3_GPIO_Port
#define COL1_Pin 		A2_Pin
#define COL1_GPIO_Port 	A2_GPIO_Port
#define COL2_Pin 		D1_Pin
#define COL2_GPIO_Port	D1_GPIO_Port
#define COL3_Pin 		D0_Pin
#define COL3_GPIO_Port	D0_GPIO_Port
#define COL4_Pin 		D13_Pin
#define COL4_GPIO_Port	D13_GPIO_Port
#define ROW0_Pin 		JTDO_Pin
#define ROW0_GPIO_Port	JTDO_GPIO_Port
#define ROW1_Pin 		DONGLE_SPI_CS_Pin
#define ROW1_GPIO_Port	DONGLE_SPI_CS_GPIO_Port
#define ROW2_Pin 		D12_Pin
#define ROW2_GPIO_Port	D12_GPIO_Port
#define ROW3_Pin		D11_Pin
#define ROW3_GPIO_Port	D11_GPIO_Port
#define ROW4_Pin		STLINK_RX_Pin
#define ROW4_GPIO_Port	STLINK_RX_GPIO_Port
#define ROW5_Pin		D9_Pin
#define ROW5_GPIO_Port	D9_GPIO_Port
#define ROW6_Pin		D6_Pin
#define ROW6_GPIO_Port	D6_GPIO_Port


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
