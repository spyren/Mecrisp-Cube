/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_common.h"
#include "uart.h"
#include "flash.h"
#include "usb_cdc.h"
#include "bsp.h"
#include "sd.h"
#include "fd_spi.h"
#include "rt_spi.h"
#include "fd.h"
#include "block.h"
#include "fatfs.h"
#include "fs.h"
#include "vi.h"
#include "clock.h"
#include "iic.h"
#include "plex.h"
#include "watchdog.h"
#include "myassert.h"
#if OLED == 1
#include "oled.h"
#endif
#if MIP == 1
#include "mip.h"
#endif

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for FORTH_ConThread */
osThreadId_t FORTH_ConThreadHandle;
const osThreadAttr_t FORTH_ConThread_attributes = {
  .name = "FORTH_Console",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Forth(void);

/* USER CODE END FunctionPrototypes */

void MainThread(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	WATCHDOG_init();
	BSP_init();
	RTC_init();
	UART_init();
	IIC_init();
	CDC_init();
	FLASH_init();
	RTSPI_init();
	BLOCK_init();
	VI_init();
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of FORTH_ConThread */
  FORTH_ConThreadHandle = osThreadNew(MainThread, NULL, &FORTH_ConThread_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_MainThread */
/**
  * @brief  Function implementing the FORTH_ConThread thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MainThread */
void MainThread(void *argument)
{
  /* USER CODE BEGIN MainThread */
	ASSERT_init();
	SD_init();
	FD_init();
	FS_init();
#if OLED == 1
	OLED_init();
#endif
#if MIP == 1
	MIP_init();
#endif
#if PLEX == 1
	PLEX_init();
#endif
#if EPD == 1
	EPD_init();
#endif

	osDelay(10);

	osDelay(10);
	FDSPI_init();
	FD_init();
	SD_init();
	BLOCK_init();
	FS_init();

	BSP_setLED1(FALSE); // switch off power on LED

	Forth();

	ASSERT_fatal(0, ASSERT_FORTH_UNEXPECTED_EXIT, 0);

  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END MainThread */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

