/**
 *  @brief
 *
 *  @file
 *      assert.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2021-12-31
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



#ifndef INC_ASSERT_H_
#define INC_ASSERT_H_

#include "app_conf.h"
#include "clock.h"

#define ASSERT_HARD_FAULT				1
#define ASSERT_MEM_MANAGE_FAULT			2
#define ASSERT_BUS_FAULT				3
#define ASSERT_USAGE_FAULT				4
#define ASSERT_CPU2_HARD_FAULT			5
#define ASSERT_FORTH_UNEXPECTED_EXIT	6


void ASSERT_init(void);
int ASSERT_occurred(void);
int ASSERT_getCount(void);
int ASSERT_getId(void);
int ASSERT_getParam(void);

/**
 *  @brief
 *      Abort if cond is false and assertion is activated, log n and u after restart
 *
 *  @param[in]
 *  	cond
 *  @param[in]
 *  	id
 *  @param[in]
 *  	param	e.g. address where the assert occurred
 *  @return
 *      None
 */
#ifdef CFG_ASSERT_ON
#define ASSERT_nonfatal(cond, id, param)                \
  if (!(cond)) {                                        \
	RTC_Backup.assert = RTC_MAGIC_COOKIE;               \
	RTC_Backup.assert_id = id;                          \
	RTC_Backup.assert_param = param;                    \
	NVIC_SystemReset();                                 \
  }
#else
#define ASSERT_nonfatal(cond, id, param)
#endif

/**
 *  @brief
 *      Abort if cond is false, log n and u after restart
 *
 *  @param[in]
 *  	cond
 *  @param[in]
 *  	id
 *  @param[in]
 *  	param	e.g. address where the assert occurred
 *  @return
 *      None
 */
#define ASSERT_fatal(cond, id, param)                   \
  if (!(cond)) {                                        \
	RTC_Backup.assert = RTC_MAGIC_COOKIE;               \
	RTC_Backup.assert_id = id;                          \
	RTC_Backup.assert_param = param;                    \
	NVIC_SystemReset();                                 \
  }


#endif /* INC_ASSERT_H_ */
