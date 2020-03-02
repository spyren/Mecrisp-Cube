@
@    Mecrisp-Stellaris - A native code Forth implementation for ARM-Cortex M microcontrollers
@    Copyright (C) 2013  Matthias Koch
@
@    This program is free software: you can redistribute it and/or modify
@    it under the terms of the GNU General Public License as published by
@    the Free Software Foundation, either version 3 of the License, or
@    (at your option) any later version.
@
@    This program is distributed in the hope that it will be useful,
@    but WITHOUT ANY WARRANTY; without even the implied warranty of
@    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
@    GNU General Public License for more details.
@
@    You should have received a copy of the GNU General Public License
@    along with this program.  If not, see <http://www.gnu.org/licenses/>.
@

@ Terminalroutinen
@ Terminal code and initialisations.
@ Porting: Rewrite this !

@ stm32wb55 nucleo board uses PB6 and PB7 on USART1

@ -----------------------------------------------------------------------------
uart_init:
        @ ( -- ) A few bits are different
@ -----------------------------------------------------------------------------

	bx	lr


@ Following code is the same as for STM32F051
.include "terminalhooks.s"

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-emit"
serial_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{lr}

    ldr r0, tos
	bl	UART_putc

	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key"
serial_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{lr}

1:	bl	serial_qkey
	cmp	tos, #0
	drop
	beq	1b

	pushdatos
	ldr	r2, =USART1_RDR
	ldrb	tos, [r2]			@ Fetch the character

	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-emit?"
serial_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	bl	pause

	pushdaconst 0				@ False Flag
	ldr	r0, =USART1_ISR
	ldr	r1, [r0]			@ Fetch status
	ldr	r0, =1<<USART1_TXE_Shift
	ands	r1, r1, r0
	beq	1f
	mvns	tos, tos 			@ True Flag
1:	pop	{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key?"
serial_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{lr}
	bl	pause

	pushdaconst 0				@ False Flag
	ldr	r0, =USART1_ISR
	ldr	r1, [r0]			@ Fetch status
	ldr	r0, =1<<USART1_RXNE_Shift
	ands	r1, r1, r0
	beq	1f
	mvns	tos, tos			@ True Flag
1:	pop	{pc}

.ltorg @ Hier werden viele spezielle Hardwarestellenkonstanten gebraucht, schreibe sie gleich !
