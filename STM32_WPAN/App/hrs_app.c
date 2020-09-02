/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : App/hrs_app.c
  * Description        : Heart Rate Service Application
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
#include "app_common.h"

#include "ble.h"
#include "hrs_app.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  HRS_BodySensorLocation_t BodySensorLocationChar;
  HRS_MeasVal_t MeasurementvalueChar;
  uint8_t ResetEnergyExpended;
  uint8_t TimerMeasurement_Id;

} HRSAPP_Context_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private defines ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macros ------------------------------------------------------------*/

/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
 * START of Section BLE_APP_CONTEXT
 */

PLACE_IN_SECTION("BLE_APP_CONTEXT") static HRSAPP_Context_t HRSAPP_Context;

/**
 * END of Section BLE_APP_CONTEXT
 */

osThreadId_t HrsThreadId;

const osThreadAttr_t HrsThread_attr = {
		.name = CFG_HRS_PROCESS_NAME,
		.attr_bits = CFG_HRS_PROCESS_ATTR_BITS,
		.cb_mem = CFG_HRS_PROCESS_CB_MEM,
		.cb_size = CFG_HRS_PROCESS_CB_SIZE,
		.stack_mem = CFG_HRS_PROCESS_STACK_MEM,
		.priority = CFG_HRS_PROCESS_PRIORITY,
		.stack_size = CFG_HRS_PROCESS_STACK_SIZE
};

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private functions prototypes-----------------------------------------------*/
static void HrsThread(void *argument);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void HRS_Notification(HRS_App_Notification_evt_t *pNotification) {
	switch(pNotification->HRS_Evt_Opcode)
	{
#if (BLE_CFG_HRS_ENERGY_EXPENDED_INFO_FLAG != 0)
	case HRS_RESET_ENERGY_EXPENDED_EVT:
		break;
#endif

	case HRS_NOTIFICATION_ENABLED:
		break;

	case HRS_NOTIFICATION_DISABLED:
		break;

#if (BLE_CFG_OTA_REBOOT_CHAR != 0)
	case HRS_STM_BOOT_REQUEST_EVT:
		break;
#endif

	default:
		break;
	}
	return;
}

void HRSAPP_Init(void) {
	HrsThreadId = osThreadNew(HrsThread, NULL, &HrsThread_attr);

	/**
	 * Set Body Sensor Location
	 */
	HRSAPP_Context.ResetEnergyExpended = 0;
	HRSAPP_Context.BodySensorLocationChar = HRS_BODY_SENSOR_LOCATION_HAND;
	HRS_UpdateChar(SENSOR_LOCATION_UUID, (uint8_t *)&HRSAPP_Context.BodySensorLocationChar);


	/**
	 * Set Flags for measurement value
	 */

	HRSAPP_Context.MeasurementvalueChar.Flags = ( HRS_HRM_VALUE_FORMAT_UINT16      |
			HRS_HRM_SENSOR_CONTACTS_PRESENT   |
			HRS_HRM_SENSOR_CONTACTS_SUPPORTED |
			HRS_HRM_ENERGY_EXPENDED_PRESENT  |
			HRS_HRM_RR_INTERVAL_PRESENT );

#if (BLE_CFG_HRS_ENERGY_EXPENDED_INFO_FLAG != 0)
	if(HRSAPP_Context.MeasurementvalueChar.Flags & HRS_HRM_ENERGY_EXPENDED_PRESENT)
		HRSAPP_Context.MeasurementvalueChar.EnergyExpended = 10;
#endif

#if (BLE_CFG_HRS_ENERGY_RR_INTERVAL_FLAG != 0)
	if(HRSAPP_Context.MeasurementvalueChar.Flags & HRS_HRM_RR_INTERVAL_PRESENT)
	{
		uint8_t i;

		HRSAPP_Context.MeasurementvalueChar.NbreOfValidRRIntervalValues = BLE_CFG_HRS_ENERGY_RR_INTERVAL_FLAG;
		for(i = 0; i < BLE_CFG_HRS_ENERGY_RR_INTERVAL_FLAG; i++)
			HRSAPP_Context.MeasurementvalueChar.aRRIntervalValues[i] = 1024;
	}
#endif

	return;
}


static void HrsThread(void *argument) {
	UNUSED(argument);
	int measurement;

	for (;;) {
		// update every 500 ms
		osDelay(500);

		measurement = 30 + BSP_getAnalogPin(0) / 20;

		HRSAPP_Context.MeasurementvalueChar.MeasurementValue = measurement;
#if (BLE_CFG_HRS_ENERGY_EXPENDED_INFO_FLAG != 0)
		if((HRSAPP_Context.MeasurementvalueChar.Flags & HRS_HRM_ENERGY_EXPENDED_PRESENT) &&
				(HRSAPP_Context.ResetEnergyExpended == 0))
			HRSAPP_Context.MeasurementvalueChar.EnergyExpended += 5;
		else if(HRSAPP_Context.ResetEnergyExpended == 1)
			HRSAPP_Context.ResetEnergyExpended = 0;
#endif

		// danger
		HRS_UpdateChar(HEART_RATE_MEASURMENT_UUID, (uint8_t *)&HRSAPP_Context.MeasurementvalueChar);

	}

}


