/* USER CODE BEGIN Header */
/**
 *  @brief
 *      Mecrisp-Cube, Forth for the STM32 Cube ecosystem.
 *
 *      Forth is a CMSIS-RTOS Thread (FreeRTOS Task) MainThread.
 *      Console I/O (UART, USB-CDC, BLE) is buffered and RTOS aware.
 *
 *      STM32WB55C: 1 MiB FLASH
 *      For details see STM32WB55RGVX_FLASH.ld
 *
 *      FLASH (rx)                 : ORIGIN = 0x08000000, LENGTH = 256K
 *       20 KiB Forth Core
 *      186 KiB Middleware (debug 187 KiB, only 128 font chars)
 *
 *      FLASH_FORTH (rx)           : ORIGIN = 0x08040000, LENGTH = 128K
 *      128 KiB Flash Dictionary
 *
 *      FLASH_DRIVE (rx)           : ORIGIN = 0x08060000, LENGTH = 384K
 *      384 KiB future use for built in flash drive
 *
 *      FLASH_BLESTACK (rx)        : ORIGIN = 0x080C0000, LENGTH = 256K
 *
 *      STM32WB55C: 256 KiB RAM
 *
 *      RAM_FORTH (xrw)            : ORIGIN = 0X20000000, LENGTH = 64K
 *       1 KiB Core
 *      63 KiB RAM Dictionary
 *
 *      RAM1 (xrw)                 : ORIGIN = 0x20010000, LENGTH = 128K
 *       1 KiB Stack         (for startup and ISRs, MSP)
 *       2 KiB Heap          (maybe not needed)
 *       4 KiB Flash erase Buffer
 *       4 KiB Block Buffer
 *      10 KiB global variables
 *      90 KiB RTOS Heap (about 17 KiB free)
 *         Thread Stack size
 *              4 KiB Forth (main)
 *              1 KiB UART_Tx
 *              5 KiB UART_Rx
 *              2 KiB CDC_Rx
 *            0.5 KiB CDC_Tx
 *              1 KiB CRS
 *              1 KiB HRS
 *              1 KiB HCI_USER_EVT
 *              1 KiB ADV_UPDATE
 *              1 KiB SHCI_USER_EVT
 *
 *             40 KiB vi text buffer
 *     107 KiB
 *
 *      RAM_SHARED (xrw)           : ORIGIN = 0x20030000, LENGTH = 10K
 *       10 KiB communication between CPU1 and CPU2 (part of RAM2a)
 *
 *     (RAM2a                      : ORIGIN = 0x20030000, LENGTH = 32K)
 *       10 KiB shared between CPU1 and CPU2
 *       22 KiB secure RAM for CPU2
 *
 *     (RAM2b                      : ORIGIN = 0x20038000, LENGTH = 32K)
 *       15 KiB shared between CPU1 and CPU2
 *       17 KiB secure RAM for CPU2
 *
 *
 *  @file
 *      main.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-12
 *  @remark
 *      Language: C, STM32CubeIDE GCC
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This project Mecrsip-Cube is free software: you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation, either version 3 of
 *      the License, or (at your option) any later version.
 *
 *      Mecrsip-Cube is distributed in the hope that it will be useful, but
 *      WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *      General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "aes.h"
#include "crc.h"
#include "dma.h"
#include "app_fatfs.h"
#include "i2c.h"
#include "ipcc.h"
#include "pka.h"
#include "rf.h"
#include "rng.h"
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb.h"
#include "wwdg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_common.h"
#include "uart.h"
#include "flash.h"
#include "usb_cdc.h"
#include "bsp.h"
#include "spi.h"
#include "sd.h"
#include "block.h"
#include "app_fatfs.h"
#include "fs.h"
#include "clock.h"
#include "myassert.h"
#if POWER == 1
#include "power.h"
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

/* USER CODE BEGIN PV */
const char MecrispCubeVersion[] = MECRISP_CUBE_VERSION;
const char MecrispVersion[] = "  * ";
const char rc_local[] = RC_LOCAL;

const char ForthCalculator[] = FORTH_CALCULATOR;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
#if POWER == 1
	POWER_startup();
#endif
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Config code for STM32_WPAN (HSE Tuning must be done before system clock configuration) */
  MX_APPE_Config();

  /* USER CODE BEGIN Init */

  /* activate divide by zero trap (usage fault) */
  SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
  /* enable Usage-/Bus-/MPU Fault */
  SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk
             |  SCB_SHCSR_BUSFAULTENA_Msk
             |  SCB_SHCSR_MEMFAULTENA_Msk;

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* IPCC initialisation */
//  MX_IPCC_Init();

  /* USER CODE BEGIN SysInit */
 
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_RTC_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_SPI1_Init();
  if (MX_FATFS_Init() != APP_OK) {
    Error_Handler();
  }
  MX_TIM2_Init();
  MX_I2C1_Init();
//  MX_WWDG_Init();
  MX_AES1_Init();
  MX_RNG_Init();
  MX_PKA_Init();
  MX_CRC_Init();
//  MX_USB_PCD_Init();
  MX_RF_Init();
  /* USER CODE BEGIN 2 */
#if CFG_DEBUGGER_SUPPORTED == 1
  // test for SWO debug trace
  printf("Hallo Velo\n");
#endif
  /**
   * This prevents the CPU2 to disable the HSI48 oscillator when
   * it does not use anymore the RNG IP
   */
  int i = 100;
  while (LL_HSEM_1StepLock(HSEM, CFG_HW_CLK48_CONFIG_SEMID) && i > 0) {
	  // lock failed
	  i--;
  }

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();  /* Init code for STM32_WPAN */

  /* Init code for STM32_WPAN */
  MX_APPE_Init();

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSI
          	                  |RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the SYSCFG APB clock
  */
  __HAL_RCC_CRS_CLK_ENABLE();

  /** Configures CRS
  */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,1000);
  RCC_CRSInitStruct.ErrorLimitValue = 34;
  RCC_CRSInitStruct.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS|RCC_PERIPHCLK_RFWAKEUP;
  PeriphClkInitStruct.RFWakeUpClockSelection = RCC_RFWKPCLKSOURCE_LSE;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSI;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE1;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */
  // USB clock configuration
  RCC_CRSInitTypeDef RCC_CRSInitStruct= {0};
  __HAL_RCC_CRS_CLK_ENABLE();

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInitStruct.UsbClockSelection    = RCC_USBCLKSOURCE_HSI48;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);

  /*Enable CRS Clock*/
  __HAL_RCC_CRS_CLK_ENABLE();

  /* Default Synchro Signal division factor (not divided) */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;

  /* Set the SYNCSRC[1:0] bits according to CRS_Source value */
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;

  /* HSI48 is synchronized with USB SOF at 1KHz rate */
  RCC_CRSInitStruct.ReloadValue =  __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000, 1000);
  RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;

  /* Set the TRIM[5:0] to the default value */
  RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;

  /* Start automatic synchronization */
  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

  /* USER CODE END Smps */
}

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName ) {
	ASSERT_fatal(0, ASSERT_STACK_OVERFLOW, (uint32_t) pcTaskName);
}


void vApplicationMallocFailedHook(void) {
	ASSERT_fatal(0, ASSERT_MALLOC_FAILED, 0);
} // vApplicationMallocFailedHook


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM17 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	  if (htim->Instance == TIM2) {
	    BSP_TIM2_PeriodElapsedCallback();
	  }

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM17) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	BSP_setLED3(TRUE);
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
