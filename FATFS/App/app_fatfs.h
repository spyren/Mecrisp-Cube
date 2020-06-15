/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   app_fatfs.h
  * @brief  Header for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_FATFS_H
#define __APP_FATFS_H

/* Includes ------------------------------------------------------------------*/
#include "ff.h"
#include "ff_gen_drv.h"
#include "user_diskio.h" /* defines USER_Driver as external */

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
int32_t MX_FATFS_Init(void);
int32_t MX_FATFS_Process(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define APP_OK                      0
#define APP_ERROR                  -1
#define APP_SD_UNPLUGGED           -2
/* USER CODE END Private defines */

extern FATFS USERFatFs;    /* File system object for USER logical drive */
extern FIL USERFile;       /* File  object for USER */
extern char USERPath[4];   /* USER logical drive path */

#endif /*__APP_FATFS_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
