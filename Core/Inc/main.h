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
#include "stm32f4xx_hal.h"

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
#define D8_Pin GPIO_PIN_0
#define D8_GPIO_Port GPIOC
#define D13_Pin GPIO_PIN_1
#define D13_GPIO_Port GPIOC
#define D12_Pin GPIO_PIN_2
#define D12_GPIO_Port GPIOC
#define D11_Pin GPIO_PIN_3
#define D11_GPIO_Port GPIOC
#define A6_Pin GPIO_PIN_3
#define A6_GPIO_Port GPIOA
#define A0_Pin GPIO_PIN_4
#define A0_GPIO_Port GPIOA
#define A1_Pin GPIO_PIN_5
#define A1_GPIO_Port GPIOA
#define A2_Pin GPIO_PIN_6
#define A2_GPIO_Port GPIOA
#define A3_Pin GPIO_PIN_7
#define A3_GPIO_Port GPIOA
#define A4_Pin GPIO_PIN_4
#define A4_GPIO_Port GPIOC
#define A5_Pin GPIO_PIN_5
#define A5_GPIO_Port GPIOC
#define D1_Pin GPIO_PIN_10
#define D1_GPIO_Port GPIOB
#define D0_Pin GPIO_PIN_11
#define D0_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_13
#define SCK_GPIO_Port GPIOB
#define MISO_Pin GPIO_PIN_14
#define MISO_GPIO_Port GPIOB
#define MOSI_Pin GPIO_PIN_15
#define MOSI_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_6
#define D6_GPIO_Port GPIOC
#define D5_Pin GPIO_PIN_7
#define D5_GPIO_Port GPIOC
#define FLASH_SCK_Pin GPIO_PIN_3
#define FLASH_SCK_GPIO_Port GPIOB
#define FLASH_MISO_Pin GPIO_PIN_4
#define FLASH_MISO_GPIO_Port GPIOB
#define FLASH_MOSI_Pin GPIO_PIN_5
#define FLASH_MOSI_GPIO_Port GPIOB
#define D15_Pin GPIO_PIN_6
#define D15_GPIO_Port GPIOB
#define D14_Pin GPIO_PIN_7
#define D14_GPIO_Port GPIOB
#define D9_Pin GPIO_PIN_8
#define D9_GPIO_Port GPIOB
#define D10_Pin GPIO_PIN_9
#define D10_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define D2_Pin SCK_Pin
#define D2_GPIO_Port SCK_GPIO_Port
#define D3_Pin MISO_Pin
#define D3_GPIO_Port MISO_GPIO_Port
#define D4_Pin MOSI_Pin
#define D4_GPIO_Port MOSI_GPIO_Port
#define D7_Pin GPIO_PIN_13
#define D7_GPIO_Port GPIOC

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
