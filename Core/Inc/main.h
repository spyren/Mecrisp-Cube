/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

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
#define QS_D2_Pin GPIO_PIN_2
#define QS_D2_GPIO_Port GPIOE
#define D7_Pin GPIO_PIN_13
#define D7_GPIO_Port GPIOC
#define D8_Pin GPIO_PIN_3
#define D8_GPIO_Port GPIOF
#define A5_Pin GPIO_PIN_10
#define A5_GPIO_Port GPIOF
#define A1_Pin GPIO_PIN_0
#define A1_GPIO_Port GPIOC
#define A4_Pin GPIO_PIN_2
#define A4_GPIO_Port GPIOC
#define A2_Pin GPIO_PIN_3
#define A2_GPIO_Port GPIOC
#define A0_Pin GPIO_PIN_3
#define A0_GPIO_Port GPIOA
#define D13_Pin GPIO_PIN_5
#define D13_GPIO_Port GPIOA
#define D12_Pin GPIO_PIN_6
#define D12_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_0
#define LED1_GPIO_Port GPIOB
#define A3_Pin GPIO_PIN_1
#define A3_GPIO_Port GPIOB
#define QS_CLK_Pin GPIO_PIN_2
#define QS_CLK_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_9
#define D6_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_11
#define D5_GPIO_Port GPIOE
#define D3_Pin GPIO_PIN_13
#define D3_GPIO_Port GPIOE
#define D4_Pin GPIO_PIN_14
#define D4_GPIO_Port GPIOE
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOB
#define STLINK_RX_Pin GPIO_PIN_8
#define STLINK_RX_GPIO_Port GPIOD
#define STLINK_TX_Pin GPIO_PIN_9
#define STLINK_TX_GPIO_Port GPIOD
#define USB_OTG_FS_PWR_EN_Pin GPIO_PIN_10
#define USB_OTG_FS_PWR_EN_GPIO_Port GPIOD
#define QS_D0_Pin GPIO_PIN_11
#define QS_D0_GPIO_Port GPIOD
#define QS_D1_Pin GPIO_PIN_12
#define QS_D1_GPIO_Port GPIOD
#define QS_D3_Pin GPIO_PIN_13
#define QS_D3_GPIO_Port GPIOD
#define D10_Pin GPIO_PIN_14
#define D10_GPIO_Port GPIOD
#define D9_Pin GPIO_PIN_15
#define D9_GPIO_Port GPIOD
#define QS_CS_Pin GPIO_PIN_6
#define QS_CS_GPIO_Port GPIOG
#define SDIO_D0_Pin GPIO_PIN_8
#define SDIO_D0_GPIO_Port GPIOC
#define SDIO_D1_Pin GPIO_PIN_9
#define SDIO_D1_GPIO_Port GPIOC
#define SDIO_D2_Pin GPIO_PIN_10
#define SDIO_D2_GPIO_Port GPIOC
#define SDIOD3_Pin GPIO_PIN_11
#define SDIOD3_GPIO_Port GPIOC
#define SDIO_CLK_Pin GPIO_PIN_12
#define SDIO_CLK_GPIO_Port GPIOC
#define SDIO_CMD_Pin GPIO_PIN_2
#define SDIO_CMD_GPIO_Port GPIOD
#define D2_Pin GPIO_PIN_14
#define D2_GPIO_Port GPIOG
#define D11_Pin GPIO_PIN_5
#define D11_GPIO_Port GPIOB
#define D1_Pin GPIO_PIN_6
#define D1_GPIO_Port GPIOB
#define D0_Pin GPIO_PIN_7
#define D0_GPIO_Port GPIOB
#define D15_Pin GPIO_PIN_8
#define D15_GPIO_Port GPIOB
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_1
#define LED2_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
