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
#define BUTTON_BACK_Pin GPIO_PIN_13
#define BUTTON_BACK_GPIO_Port GPIOC
#define BUTTON_OK_Pin GPIO_PIN_3
#define BUTTON_OK_GPIO_Port GPIOH
#define SPEAKER_Pin GPIO_PIN_8
#define SPEAKER_GPIO_Port GPIOB
#define IR_TX_Pin GPIO_PIN_9
#define IR_TX_GPIO_Port GPIOB
#define A0_Pin GPIO_PIN_0
#define A0_GPIO_Port GPIOC
#define A1_Pin GPIO_PIN_1
#define A1_GPIO_Port GPIOC
#define SPI_D_MISO_Pin GPIO_PIN_2
#define SPI_D_MISO_GPIO_Port GPIOC
#define A2_Pin GPIO_PIN_3
#define A2_GPIO_Port GPIOC
#define IR_RX_Pin GPIO_PIN_0
#define IR_RX_GPIO_Port GPIOA
#define CC1101_G0_Pin GPIO_PIN_1
#define CC1101_G0_GPIO_Port GPIOA
#define RFID_PULL_Pin GPIO_PIN_2
#define RFID_PULL_GPIO_Port GPIOA
#define PERIPH_POWER_Pin GPIO_PIN_3
#define PERIPH_POWER_GPIO_Port GPIOA
#define D10_Pin GPIO_PIN_4
#define D10_GPIO_Port GPIOA
#define SPI_R_SCK_Pin GPIO_PIN_5
#define SPI_R_SCK_GPIO_Port GPIOA
#define D12_Pin GPIO_PIN_6
#define D12_GPIO_Port GPIOA
#define D11_Pin GPIO_PIN_7
#define D11_GPIO_Port GPIOA
#define VIBRO_Pin GPIO_PIN_8
#define VIBRO_GPIO_Port GPIOA
#define I2C_SCL_Pin GPIO_PIN_9
#define I2C_SCL_GPIO_Port GPIOA
#define RF_SW_O_Pin GPIO_PIN_4
#define RF_SW_O_GPIO_Port GPIOC
#define RFID_RF_IN_Pin GPIO_PIN_5
#define RFID_RF_IN_GPIO_Port GPIOC
#define D9_Pin GPIO_PIN_2
#define D9_GPIO_Port GPIOB
#define BUTTON_UP_Pin GPIO_PIN_10
#define BUTTON_UP_GPIO_Port GPIOB
#define BUTTON_LEFT_Pin GPIO_PIN_11
#define BUTTON_LEFT_GPIO_Port GPIOB
#define DISPLAY_RST_Pin GPIO_PIN_0
#define DISPLAY_RST_GPIO_Port GPIOB
#define DISPLAY_DI_Pin GPIO_PIN_1
#define DISPLAY_DI_GPIO_Port GPIOB
#define NFC_CS_Pin GPIO_PIN_4
#define NFC_CS_GPIO_Port GPIOE
#define BUTTON_RIGHT_Pin GPIO_PIN_12
#define BUTTON_RIGHT_GPIO_Port GPIOB
#define RFID_OUT_Pin GPIO_PIN_13
#define RFID_OUT_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_14
#define D6_GPIO_Port GPIOB
#define SPI_D_MOSI_Pin GPIO_PIN_15
#define SPI_D_MOSI_GPIO_Port GPIOB
#define BUTTON_DOWN_Pin GPIO_PIN_6
#define BUTTON_DOWN_GPIO_Port GPIOC
#define I2C_SDA_Pin GPIO_PIN_10
#define I2C_SDA_GPIO_Port GPIOA
#define D2_Pin GPIO_PIN_13
#define D2_GPIO_Port GPIOA
#define D3_Pin GPIO_PIN_14
#define D3_GPIO_Port GPIOA
#define RFID_CARRIER_Pin GPIO_PIN_15
#define RFID_CARRIER_GPIO_Port GPIOA
#define SD_CD_Pin GPIO_PIN_10
#define SD_CD_GPIO_Port GPIOC
#define DISPLAY_CS_Pin GPIO_PIN_11
#define DISPLAY_CS_GPIO_Port GPIOC
#define SD_CS_Pin GPIO_PIN_12
#define SD_CS_GPIO_Port GPIOC
#define CC1101_CS_Pin GPIO_PIN_0
#define CC1101_CS_GPIO_Port GPIOD
#define SPI_D_SCK_Pin GPIO_PIN_1
#define SPI_D_SCK_GPIO_Port GPIOD
#define D13_Pin GPIO_PIN_3
#define D13_GPIO_Port GPIOB
#define SPI_R_MISO_Pin GPIO_PIN_4
#define SPI_R_MISO_GPIO_Port GPIOB
#define SPI_R_MOSI_Pin GPIO_PIN_5
#define SPI_R_MOSI_GPIO_Port GPIOB
#define D1_Pin GPIO_PIN_6
#define D1_GPIO_Port GPIOB
#define D0_Pin GPIO_PIN_7
#define D0_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define B1_DONGLE_Pin GPIO_PIN_10
#define B1_DONGLE_GPIO_Port GPIOA
#define LD1_DONGLE_Pin GPIO_PIN_4
#define LD1_DONGLE_GPIO_Port GPIOA


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
