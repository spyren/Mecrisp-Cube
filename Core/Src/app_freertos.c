/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_common.h"
#include "app_entry.h"
#include "uart.h"
#include "flash.h"
#include "usb_cdc.h"
#include "bsp.h"
#include "rt_spi.h"
#include "sd.h"
#include "fd.h"
#include "block.h"
#include "app_fatfs.h"
#include "fs.h"
#include "vi.h"
#include "shci.h"
#include "clock.h"
#include "iic.h"
#include "iic3.h"
#include "plex.h"
#include "mems.h"
#include "watchdog.h"
#include "myassert.h"
#if OLED == 1
#include "oled.h"
#endif
#if MIP == 1
#include "mip.h"
#endif
#if EPD == 1
#include "epd.h"
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
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 20
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Forth(void);
   
/* USER CODE END FunctionPrototypes */

void MainThread(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	MX_APPE_Init();
	WATCHDOG_init();
	BSP_init();
	RTC_init();
	UART_init();
	IIC_init();
	IIC3_init();
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
//	MX_APPE_Init();
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_MainThread */
/**
  * @brief  Function implementing the Main thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_MainThread */
void MainThread(void *argument)
{
  /* USER CODE BEGIN MainThread */
	BSP_setRgbLED(0x008000); // Set RGB green LED to 50 %
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
	MEMS_init();

	// wait till BLE is ready
	if (osThreadFlagsWait(BLE_IS_READY, osFlagsWaitAny, 2000) == BLE_IS_READY) {
		// sem7 is used by CPU2 to prevent CPU1 from writing/erasing data in Flash memory
		SHCI_C2_SetFlashActivityControl(FLASH_ACTIVITY_CONTROL_SEM7);
		BSP_setRgbLED(BSP_getRgbLED() & 0xFF7FFF); // // switch off power on LED
	} else {
		// timeout -> BLE (CPU2) not started, flash operations not possible
		BSP_setRgbLED(0x800000); // Set RGB red LED to 50 %
	}

	Forth();

	ASSERT_fatal(0, ASSERT_FORTH_UNEXPECTED_EXIT, 0);

  /* USER CODE END MainThread */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

