/**
 *  @brief
 *      CMSIS-RTOS API for Mecrisp-Cube
 *
 *  @file
 *      rtos.c
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

// System include files
// ********************
#include "cmsis_os.h"
#include <string.h>

// Application include files
// *************************
#include "app_common.h"
#include "main.h"
#include "rtos.h"


// Defines
// *******

// Global Variables
// ****************
const char RTOS_Version[] = "  * CMSIS-RTOS V2 FreeRTOS wrapper, FreeRTOS Kernel V10.2.1 (C) 2017 Amazon.com\n";

// Public Functions
// ****************

int RTOS_osThreadAttr_size(void) {
	return(sizeof(osThreadAttr_t));
}

int RTOS_osThreadAttr_name(void) {
	return(0*4);
}

int RTOS_osThreadAttr_attr_bits(void) {
	return(1*4);
}

int RTOS_osThreadAttr_cb_mem(void) {
	return(2*4);
}

int RTOS_osThreadAttr_cb_size(void) {
	return(3*4);
}

int RTOS_osThreadAttr_stack_mem(void) {
	return(4*4);
}

int RTOS_osThreadAttr_stack_size(void) {
	return(5*4);
}

int RTOS_osThreadAttr_priority(void) {
	return(6*4);
}

int RTOS_osThreadAttr_tz_module(void) {
	return(7*4);
}

int RTOS_osThreadAttr_reserved(void) {
	return(8*4);
}


int RTOS_osEventFlagsAttr_size(void) {
	return(sizeof(osEventFlagsAttr_t));
}

int RTOS_osMessageQueueAttr_size(void) {
	return(sizeof(osMessageQueueAttr_t));
}


int RTOS_osMutexAttr_size(void) {
	return(sizeof(osMutexAttr_t));
}


int RTOS_osSemaphoreAttr_size(void) {
	return(sizeof(osSemaphoreAttr_t));
}


int RTOS_osTimerAttr_size(void) {
	return(sizeof(osTimerAttr_t));
}






