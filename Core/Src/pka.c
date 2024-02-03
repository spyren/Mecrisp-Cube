/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    pka.c
  * @brief   This file provides code for the configuration
  *          of the PKA instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "pka.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

PKA_HandleTypeDef hpka;

/* PKA init function */
void MX_PKA_Init(void)
{

  /* USER CODE BEGIN PKA_Init 0 */

  /* USER CODE END PKA_Init 0 */

  /* USER CODE BEGIN PKA_Init 1 */

  /* USER CODE END PKA_Init 1 */
  hpka.Instance = PKA;
  if (HAL_PKA_Init(&hpka) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN PKA_Init 2 */

  /* USER CODE END PKA_Init 2 */

}

void HAL_PKA_MspInit(PKA_HandleTypeDef* pkaHandle)
{

  if(pkaHandle->Instance==PKA)
  {
  /* USER CODE BEGIN PKA_MspInit 0 */

  /* USER CODE END PKA_MspInit 0 */
    /* PKA clock enable */
    __HAL_RCC_PKA_CLK_ENABLE();
  /* USER CODE BEGIN PKA_MspInit 1 */

  /* USER CODE END PKA_MspInit 1 */
  }
}

void HAL_PKA_MspDeInit(PKA_HandleTypeDef* pkaHandle)
{

  if(pkaHandle->Instance==PKA)
  {
  /* USER CODE BEGIN PKA_MspDeInit 0 */

  /* USER CODE END PKA_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_PKA_CLK_DISABLE();
  /* USER CODE BEGIN PKA_MspDeInit 1 */

  /* USER CODE END PKA_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
