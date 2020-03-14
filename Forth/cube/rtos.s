/**
 *  @brief
 *      Interface to the CMSIS-RTOSv2 functions.

 *  @file
 *      rtos.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-11
 *  @remark
 *      Language: ARM Assembler, STM32CubeIDE GCC
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

 @ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osdelay"
rtos_osdelay:
		@ ( u -- n ) waits for a time period specified in kernel ticks
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	bl	osDelay
	movs	tos, r0
	pop	{pc}

@ -----------------------------------------------------------------------------
