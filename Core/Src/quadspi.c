/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    quadspi.c
  * @brief   This file provides code for the configuration
  *          of the QUADSPI instances.
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
#include "quadspi.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

QSPI_HandleTypeDef hqspi;
DMA_HandleTypeDef hdma_quadspi;

/* QUADSPI init function */
void MX_QUADSPI_Init(void)
{

  /* USER CODE BEGIN QUADSPI_Init 0 */

  /* USER CODE END QUADSPI_Init 0 */

  /* USER CODE BEGIN QUADSPI_Init 1 */

  /* USER CODE END QUADSPI_Init 1 */
  hqspi.Instance = QUADSPI;
  hqspi.Init.ClockPrescaler = 1;
  hqspi.Init.FifoThreshold = 4;
  hqspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
  hqspi.Init.FlashSize = 23;
  hqspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_6_CYCLE;
  hqspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
  if (HAL_QSPI_Init(&hqspi) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN QUADSPI_Init 2 */

  /* USER CODE END QUADSPI_Init 2 */

}

void HAL_QSPI_MspInit(QSPI_HandleTypeDef* qspiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspInit 0 */

  /* USER CODE END QUADSPI_MspInit 0 */
    /* QUADSPI clock enable */
    __HAL_RCC_QSPI_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**QUADSPI GPIO Configuration
    PD7     ------> QUADSPI_BK1_IO3
    PD5     ------> QUADSPI_BK1_IO1
    PD3     ------> QUADSPI_BK1_NCS
    PD6     ------> QUADSPI_BK1_IO2
    PB9     ------> QUADSPI_BK1_IO0
    PA3     ------> QUADSPI_CLK
    */
    GPIO_InitStruct.Pin = FLASH_IO3_Pin|FLASH_IO1_Pin|FLASH_NCS_Pin|FLASH_IO2_Pin
                          |FLASH_IO0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = FLASH_CLK_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(FLASH_CLK_GPIO_Port, &GPIO_InitStruct);

    /* QUADSPI DMA Init */
    /* QUADSPI Init */
    hdma_quadspi.Instance = DMA1_Channel3;
    hdma_quadspi.Init.Request = DMA_REQUEST_QUADSPI;
    hdma_quadspi.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_quadspi.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_quadspi.Init.MemInc = DMA_MINC_ENABLE;
    hdma_quadspi.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_quadspi.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_quadspi.Init.Mode = DMA_NORMAL;
    hdma_quadspi.Init.Priority = DMA_PRIORITY_LOW;
    if (HAL_DMA_Init(&hdma_quadspi) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(qspiHandle,hdma,hdma_quadspi);

    /* QUADSPI interrupt Init */
    HAL_NVIC_SetPriority(QUADSPI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
  /* USER CODE BEGIN QUADSPI_MspInit 1 */

  /* USER CODE END QUADSPI_MspInit 1 */
  }
}

void HAL_QSPI_MspDeInit(QSPI_HandleTypeDef* qspiHandle)
{

  if(qspiHandle->Instance==QUADSPI)
  {
  /* USER CODE BEGIN QUADSPI_MspDeInit 0 */

  /* USER CODE END QUADSPI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_QSPI_CLK_DISABLE();

    /**QUADSPI GPIO Configuration
    PD7     ------> QUADSPI_BK1_IO3
    PD5     ------> QUADSPI_BK1_IO1
    PD3     ------> QUADSPI_BK1_NCS
    PD6     ------> QUADSPI_BK1_IO2
    PD4     ------> QUADSPI_BK1_IO0
    PA3     ------> QUADSPI_CLK
    */
    HAL_GPIO_DeInit(GPIOD, FLASH_IO3_Pin|FLASH_IO1_Pin|FLASH_NCS_Pin|FLASH_IO2_Pin
                          |FLASH_IO0_Pin);

    HAL_GPIO_DeInit(FLASH_CLK_GPIO_Port, FLASH_CLK_Pin);

    /* QUADSPI DMA DeInit */
    HAL_DMA_DeInit(qspiHandle->hdma);

    /* QUADSPI interrupt Deinit */
    HAL_NVIC_DisableIRQ(QUADSPI_IRQn);
  /* USER CODE BEGIN QUADSPI_MspDeInit 1 */

  /* USER CODE END QUADSPI_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
