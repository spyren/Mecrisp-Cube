/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "app_common.h"
#include "time.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum {
  APPLICATION_IDLE = 0,
  APPLICATION_INIT,
  APPLICATION_RUNNING,
  APPLICATION_SD_UNPLUGGED,
}FS_FileOperationsTypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
FATFS USERFatFs;    /* File system object for USER logical drive */
FIL USERFile;       /* File  object for USER */
char USERPathSD[4];   /* USER logical drive path */
char USERPathFD[4];   /* USER logical drive path */
/* USER CODE BEGIN PV */
FS_FileOperationsTypeDef Appli_state = APPLICATION_IDLE;

// Hardware resources
// ******************

extern RTC_HandleTypeDef hrtc;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  FatFs initialization
  * @param  None
  * @retval Initialization result
  */
int32_t MX_FATFS_Init(void) {
	/*## FatFS: Link the disk I/O driver(s)  ###########################*/

	// init flash drive (0:)
	if (FATFS_LinkDriver(&USER_FD_Driver, USERPathFD) != 0) {
		return APP_ERROR;
	}

	// init SD drive (1:)
	if (FATFS_LinkDriver(&USER_SD_Driver, USERPathSD) != 0) {
		return APP_ERROR;
	} else {
		Appli_state = APPLICATION_INIT;
		return APP_OK;
	}
	/* USER CODE END FATFS_Init */
}

/**
  * @brief  FatFs application main process
  * @param  None
  * @retval Process result
  */
int32_t MX_FATFS_Process(void) {
	/* USER CODE BEGIN FATFS_Process */
	int32_t process_res = APP_OK;

	return process_res;
	/* USER CODE END FATFS_Process */
}

/**
  * @brief  Gets Time from RTC (generated when FS_NORTC==0; see ff.c)
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

	DWORD value = 0;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	value = (sDate.Year + 20) << 25;
	value |= sDate.Month << 21;
	value |= sDate.Date << 16;

	value |= sTime.Hours << 11;
	value |= sTime.Minutes << 5 ;
	value |= sTime.Seconds;

	return value;
  /* USER CODE END get_fattime */
}


/* USER CODE BEGIN Application */

/* USER CODE END Application */
