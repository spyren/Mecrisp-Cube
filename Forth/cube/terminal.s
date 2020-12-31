/**
 *  @brief
 *      Interface to the buffered terminal (UART, USB-CDC, BLE CRS) functions.
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


.include "terminalhooks.s"

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-emit"
serial_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		UART_putc
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key"
serial_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		UART_getc
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-emit?"
serial_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		UART_TxReady
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key?"
serial_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		UART_RxReady
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-emit"
cdc_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		CDC_putc
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-key"
cdc_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		CDC_getc
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-emit?"
cdc_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		CDC_TxReady
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-key?"
cdc_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		CDC_RxReady
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-emit"
crs_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		CRSAPP_putc
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-key"
crs_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		CRSAPP_getc
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-emit?"
crs_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		CRSAPP_TxReady
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-key?"
crs_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		CRSAPP_RxReady
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "baudrate!"
set_baudrate:
		@ ( u --  ) sets baud rate (e.g. 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200)
// void UART_setBaudrate(const int baudrate)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setBaudrate
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "wordlength!"
set_wordlength:
		@ ( u --  ) sets word length 7, 8, 9 (including parity)
// void UART_setWordLength(const int wordlength)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setWordLength
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "paritybit!"
set_paritybit:
		@ ( u --  ) sets parity bit 0 none, 1 odd, 2 even
// void UART_setParityBit(const int paritybit)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setParityBit
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "stopbits!"
set_stopbits:
		@ ( u --  ) sets stop bits 0 1 bit, 1 1.5 bit, 2 2 bit
// void UART_setStopBits(const int stopbits)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setStopBits
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oled-emit"
oled_emit:
        @ ( c -- ) Emit one character
// void OLED_sendChar(char ch)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		OLED_sendChar
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oled-pos"
oled_pos:
        @ ( x y -- ) Set OLED position
// void OLED_setPos(uint8_t x, uint8_t y)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r1, tos		// y
	drop
	movs	r0, tos		// x
	drop
	bl		OLED_setPos
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oled-cmd"
oled_cmd:
        @ ( a u -- ) send command to OLED
// void OLED_sendCommand(static uint8_t *command, size)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r1, tos		// size
	drop
	movs	r0, tos		// command
	drop
	bl		OLED_sendCommand
	pop		{r0-r3, pc}


// C Interface to some Forth Words
//********************************

// These functions call Forth words. They need a data stack SPS and
// top of stack (TOS).

// uint64_t TERMINAL_emit(uint64_t forth_stack, uint8_t c);
.global		TERMINAL_emit
TERMINAL_emit:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	pushdatos
	movs	tos, r2		// c
	bl		emit
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}


// uint64_t TERMINAL_key(uint64_t forth_stack, uint8_t *c);
.global		TERMINAL_key
TERMINAL_key:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	push	{r2}
	bl		key
	pop		{r2}
	str		tos, [r2]
	drop
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}


// uint64_t TERMINAL_qemit(uint64_t forth_stack, uint8_t *c);
.global		TERMINAL_qemit
TERMINAL_qemit:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	push	{r2}
	bl		qemit
	pop		{r2}
	str		tos, [r2]
	drop
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}


// uint64_t TERMINAL_qkey(uint64_t forth_stack, uint8_t *c);
.global		TERMINAL_qkey
TERMINAL_qkey:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	push	{r2}
	bl		qkey
	pop		{r2}
	str		tos, [r2]
	drop
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}




.ltorg @ Hier werden viele spezielle Hardwarestellenkonstanten gebraucht, schreibe sie gleich !
