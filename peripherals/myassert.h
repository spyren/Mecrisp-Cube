/**
 *  @brief
 *
 *  @file
 *      myassert.h
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

#ifndef MYASSERT_H
#define MYASSERT_H

#include "app_conf.h"
#include "clock.h"

#define ASSERT_HARD_FAULT				1
#define ASSERT_MEM_MANAGE_FAULT			2
#define ASSERT_BUS_FAULT				3
#define ASSERT_USAGE_FAULT				4

#define ASSERT_CPU2_HARD_FAULT			5

#define ASSERT_FORTH_UNEXPECTED_EXIT	6
#define ASSERT_STACK_OVERFLOW			7
#define ASSERT_MALLOC_FAILED			8
#define ASSERT_MUTEX_CREATION			9
#define ASSERT_SEMAPHORE_CREATION		10
#define ASSERT_QUEUE_CREATION			11
#define ASSERT_THREAD_CREATION			12
#define ASSERT_EVENT_FLAGS_CREATION		13

#define ASSERT_CDC_SIGINT				14
#define ASSERT_UART_SIGINT				15
#define ASSERT_UART_ERROR_CALLBACK		16
#define ASSERT_UART_FIFO				17

#define ASSERT_FLASH_UNLOCK				18
#define ASSERT_FLASH_LOCK				19

#define ASSERT_FREERTOS					20

#define ASSERT_CRS_SIGINT				21

void ASSERT_init(void);
int ASSERT_occurred(void);
int ASSERT_getCount(void);
int ASSERT_getId(void);
int ASSERT_getParam(void);
char* ASSERT_getMsg(int index);

/**
 *  @brief
 *      Get program counter value
 *
 *  @return
 *      PC
 */
__attribute__( ( always_inline ) ) static inline uint32_t __get_PC(void)
{
  register uint32_t result;

  __asm volatile ("mov %0, pc\n" : "=r" (result) );
  return(result);
}

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
// #if defined(CFG_ASSERT_ON)
#if 1
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


#endif /* MYASSERT_H */

