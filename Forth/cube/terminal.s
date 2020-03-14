/**
 *  @brief
 *      Interface to the buffered terminal (UART, USB-CDC) functions.
 *
 *  @file
 *      terminal.s
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
uart_init:
        @ ( -- ) Nothing todo here
@ -----------------------------------------------------------------------------

	bx	lr


.include "terminalhooks.s"

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-emit"
serial_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl	UART_putc
	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key"
serial_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl	UART_getc
	movs	tos, r0
	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-emit?"
serial_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl	UART_TxReady
	movs	tos, r0
	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key?"
serial_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl	UART_RxReady
	movs	tos, r0
	pop	{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-emit"
cdc_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl	CDC_putc
	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-key"
cdc_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl	CDC_getc
	movs	tos, r0
	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-emit?"
cdc_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl	CDC_TxReady
	movs	tos, r0
	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-key?"
cdc_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl	CDC_RxReady
	movs	tos, r0
	pop	{pc}

.ltorg @ Hier werden viele spezielle Hardwarestellenkonstanten gebraucht, schreibe sie gleich !
