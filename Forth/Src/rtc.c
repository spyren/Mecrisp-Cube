/**
 *  @brief
 *      Real Time Clock (RTC).
 *
 *  @file
 *      rtc.c
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-06-03
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

// System include files
// ********************
#include "cmsis_os.h"
#include "time.h"

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "fs.h"
#include "rtc.h"

#define DATE_2000_01_01 (946684800u)
#define DATE_2099_12_31 (4102358400u)


// Private function prototypes
// ***************************

// Global Variables
// ****************

// RTOS resources
// **************

static osMutexId_t RTC_MutexID;
static const osMutexAttr_t RTC_MutexAttr = {
		NULL,				// no name required
		osMutexPrioInherit,	// attr_bits
		NULL,				// memory for control block
		0U					// size for control block
};

static osSemaphoreId_t RTC_SemaphoreID;


// Hardware resources
// ******************

extern RTC_HandleTypeDef hrtc;


// Public Functions
// ****************

/**
 *  @brief
 *      Initializes the RTC.
 *  @return
 *      None
 */
void RTC_init(void) {
	RTC_MutexID = osMutexNew(&RTC_MutexAttr);
	if (RTC_MutexID == NULL) {
		Error_Handler();
	}

	RTC_SemaphoreID = osSemaphoreNew(1, 0, NULL);
	if (RTC_SemaphoreID == NULL) {
		Error_Handler();
	}
}


/**
 *  @brief
 *      Returns the unix time stamp.
 *
 *      Epoch is 1.1.1970. But valid dates are from 1.1.2000 to 31.12.2099
 *      because of the STM32WB RTC peripheral.
 *  @return
 *      Unix time stamp (unsigned).
 */
uint32_t RTC_getTime(void){
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	time_t timestamp;
	struct tm tm_s;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	tm_s.tm_year = sDate.Year + 100;
	tm_s.tm_mon = sDate.Month - 1;
	tm_s.tm_mday = sDate.Date;

	tm_s.tm_hour = sTime.Hours;
	tm_s.tm_min = sTime.Minutes;
	tm_s.tm_sec = sTime.Seconds;

	timestamp = mktime(&tm_s);

	return timestamp;
}


/**
 *  @brief
 *      Initializes the RTC.
 *  @return
 *      None
 */
int RTC_setTime(uint32_t timestamp){
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	time_t time;
	struct tm tm_s;

	time = (time_t)timestamp;
	if (timestamp < DATE_2000_01_01 || timestamp > DATE_2099_12_31) {
		// the RTC peripheral can handle dates from
		// 2000-01-01T00:00:00+00:00 to 2099-12-31T00:00:00+00:00
		return -1;
	}

	gmtime_r(&time, &tm_s);

	sDate.Year = tm_s.tm_year - 100;
	sDate.Month = tm_s.tm_mon + 1;
	sDate.Date = tm_s.tm_mday;
	sDate.WeekDay = tm_s.tm_wday;

	sTime.Hours = tm_s.tm_hour;
	sTime.Minutes = tm_s.tm_min;
	sTime.Seconds = tm_s.tm_sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sTime.SubSeconds = 0;

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	return 0;
}


/**
 *  @brief
 *      Prints the current date and time.
 *
 *      YYYY-MM-DDTHH:MM:SS (ISO 8601, UTC)
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @return
 *      TOS (lower word) and SPS (higher word)
 */
uint64_t RTC_typeTime(uint64_t forth_stack) {
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;
	char line[22];

	uint64_t stack;
	stack = forth_stack;

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	snprintf(line, sizeof(line), "%4u-%02u-%02uT%02u:%02u:%02u",
			sDate.Year + 2000,  sDate.Month,  sDate.Date,
			sTime.Hours, sTime.Minutes, sTime.Seconds);
	stack = FS_type(stack, (uint8_t*)line, strlen(line));

	return stack;
}


// Callbacks
// *********

