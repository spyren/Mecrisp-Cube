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
#define B1_Pin GPIO_PIN_3
#define B1_GPIO_Port GPIOH
#define QSPI_IO1_Pin GPIO_PIN_8
#define QSPI_IO1_GPIO_Port GPIOB
#define QSPI_IO0_Pin GPIO_PIN_9
#define QSPI_IO0_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_0
#define D2_GPIO_Port GPIOA
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOA
#define QSPI_CS_Pin GPIO_PIN_2
#define QSPI_CS_GPIO_Port GPIOA
#define QSPI_CLK_Pin GPIO_PIN_3
#define QSPI_CLK_GPIO_Port GPIOA
#define D10_Pin GPIO_PIN_4
#define D10_GPIO_Port GPIOA
#define A0_Pin GPIO_PIN_5
#define A0_GPIO_Port GPIOA
#define QSPI_IO3_Pin GPIO_PIN_6
#define QSPI_IO3_GPIO_Port GPIOA
#define QSPI_IO2_Pin GPIO_PIN_7
#define QSPI_IO2_GPIO_Port GPIOA
#define A2_Pin GPIO_PIN_8
#define A2_GPIO_Port GPIOA
#define D1_Pin GPIO_PIN_9
#define D1_GPIO_Port GPIOA
#define D4_Pin GPIO_PIN_2
#define D4_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_1
#define D5_GPIO_Port GPIOB
#define LD1_Pin GPIO_PIN_4
#define LD1_GPIO_Port GPIOE
#define D0_Pin GPIO_PIN_10
#define D0_GPIO_Port GPIOA
#define D3_Pin GPIO_PIN_15
#define D3_GPIO_Port GPIOA
#define D13_Pin GPIO_PIN_3
#define D13_GPIO_Port GPIOB
#define D12_Pin GPIO_PIN_4
#define D12_GPIO_Port GPIOB
#define D11_Pin GPIO_PIN_5
#define D11_GPIO_Port GPIOB
#define D14_Pin GPIO_PIN_6
#define D14_GPIO_Port GPIOB
#define D15_Pin GPIO_PIN_7
#define D15_GPIO_Port GPIOB
#define VBAT_EN_Pin GPIO_PIN_0
#define VBAT_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
