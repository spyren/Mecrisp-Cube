/**
 *  @brief
 *      CMSIS-RTOS API for Mecrisp-Cube
 *
 *  @file
 *      rtos.h
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-08-09
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

#ifndef INC_RTOS_H_
#define INC_RTOS_H_

extern const char RTOS_Version[];

int RTOS_osThreadAttr_size(void);
int RTOS_osThreadAttr_name(void);
int RTOS_osThreadAttr_attr_bits(void);
int RTOS_osThreadAttr_cb_mem(void);
int RTOS_osThreadAttr_cb_size(void);
int RTOS_osThreadAttr_stack_mem(void);
int RTOS_osThreadAttr_stack_size(void);
int RTOS_osThreadAttr_priority(void);
int RTOS_osThreadAttr_tz_module(void);
int RTOS_osThreadAttr_reserved(void);
int RTOS_osEventFlagsAttr_size(void);
int RTOS_osMessageQueueAttr_size(void);
int RTOS_osMutexAttr_size(void);
int RTOS_osSemaphoreAttr_size(void);
int RTOS_osTimerAttr_size(void);

#endif /* INC_RTOS_H_ */
