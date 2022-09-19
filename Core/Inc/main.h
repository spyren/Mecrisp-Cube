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
#define FLASH_IO3_Pin GPIO_PIN_7
#define FLASH_IO3_GPIO_Port GPIOD
#define OLED_DC_Pin GPIO_PIN_9
#define OLED_DC_GPIO_Port GPIOC
#define D14_Pin GPIO_PIN_10
#define D14_GPIO_Port GPIOA
#define FLASH_IO1_Pin GPIO_PIN_5
#define FLASH_IO1_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_12
#define D2_GPIO_Port GPIOD
#define MEMS_SCK_Pin GPIO_PIN_13
#define MEMS_SCK_GPIO_Port GPIOB
#define FLASH_NCS_Pin GPIO_PIN_3
#define FLASH_NCS_GPIO_Port GPIOD
#define B1_Pin GPIO_PIN_12
#define B1_GPIO_Port GPIOC
#define FLASH_IO2_Pin GPIO_PIN_6
#define FLASH_IO2_GPIO_Port GPIOD
#define D12_Pin GPIO_PIN_4
#define D12_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_0
#define D6_GPIO_Port GPIOE
#define D8_Pin GPIO_PIN_13
#define D8_GPIO_Port GPIOD
#define FLASH_IO0_Pin GPIO_PIN_4
#define FLASH_IO0_GPIO_Port GPIOD
#define D1_Pin GPIO_PIN_5
#define D1_GPIO_Port GPIOB
#define STLINK_RX_Pin GPIO_PIN_7
#define STLINK_RX_GPIO_Port GPIOB
#define D3_Pin GPIO_PIN_14
#define D3_GPIO_Port GPIOD
#define D9_Pin GPIO_PIN_15
#define D9_GPIO_Port GPIOD
#define RGB_SELECT_Pin GPIO_PIN_1
#define RGB_SELECT_GPIO_Port GPIOH
#define OLED_CS_Pin GPIO_PIN_0
#define OLED_CS_GPIO_Port GPIOH
#define OLED_RST_Pin GPIO_PIN_8
#define OLED_RST_GPIO_Port GPIOC
#define STLINK_TX_Pin GPIO_PIN_6
#define STLINK_TX_GPIO_Port GPIOB
#define A1_Pin GPIO_PIN_2
#define A1_GPIO_Port GPIOA
#define D15_Pin GPIO_PIN_8
#define D15_GPIO_Port GPIOB
#define D0_Pin GPIO_PIN_0
#define D0_GPIO_Port GPIOC
#define D4_Pin GPIO_PIN_3
#define D4_GPIO_Port GPIOE
#define A4_Pin GPIO_PIN_4
#define A4_GPIO_Port GPIOC
#define B2_Pin GPIO_PIN_13
#define B2_GPIO_Port GPIOC
#define D13_Pin GPIO_PIN_1
#define D13_GPIO_Port GPIOA
#define A3_Pin GPIO_PIN_1
#define A3_GPIO_Port GPIOC
#define A0_Pin GPIO_PIN_3
#define A0_GPIO_Port GPIOC
#define A5_Pin GPIO_PIN_5
#define A5_GPIO_Port GPIOC
#define D11_Pin GPIO_PIN_7
#define D11_GPIO_Port GPIOA
#define MEMS_SDA_Pin GPIO_PIN_11
#define MEMS_SDA_GPIO_Port GPIOB
#define MICRO_CK_Pin GPIO_PIN_8
#define MICRO_CK_GPIO_Port GPIOA
#define FLASH_CLK_Pin GPIO_PIN_3
#define FLASH_CLK_GPIO_Port GPIOA
#define D5_Pin GPIO_PIN_10
#define D5_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_2
#define D7_GPIO_Port GPIOB
#define MICRO_D2_Pin GPIO_PIN_9
#define MICRO_D2_GPIO_Port GPIOA
#define A2_Pin GPIO_PIN_5
#define A2_GPIO_Port GPIOA
#define D10_Pin GPIO_PIN_4
#define D10_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
