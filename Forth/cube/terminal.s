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
	push	{lr}
	movs	r0, tos
	drop
	bl		UART_putc
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key"
serial_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		UART_getc
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-emit?"
serial_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		UART_TxReady
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "serial-key?"
serial_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		UART_RxReady
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-emit"
cdc_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		CDC_putc
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-key"
cdc_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		CDC_getc
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-emit?"
cdc_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		CDC_TxReady
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "cdc-key?"
cdc_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		CDC_RxReady
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-emit"
crs_emit:
        @ ( c -- ) Emit one character
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		CRSAPP_putc
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-key"
crs_key:
        @ ( -- c ) Receive one character
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		CRSAPP_getc
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-emit?"
crs_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		CRSAPP_TxReady
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "crs-key?"
crs_qkey:
        @ ( -- ? ) Is there a key press ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		CRSAPP_RxReady
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "baudrate!"
set_baudrate:
		@ ( u --  ) sets baud rate (e.g. 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200)
// void UART_setBaudrate(const int baudrate)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setBaudrate
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "wordlength!"
set_wordlength:
		@ ( u --  ) sets word length 7, 8, 9 (including parity)
// void UART_setWordLength(const int wordlength)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setWordLength
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "paritybit!"
set_paritybit:
		@ ( u --  ) sets parity bit 0 none, 1 odd, 2 even
// void UART_setParityBit(const int paritybit)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setParityBit
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "stopbits!"
set_stopbits:
		@ ( u --  ) sets stop bits 0 1 bit, 1 1.5 bit, 2 2 bit
// void UART_setStopBits(const int stopbits)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// baudrate
	drop
	bl		UART_setStopBits
	pop		{pc}


// OLED words only if needed
.if OLED == 1
@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oled-emit"
oled_emit:
        @ ( c -- ) Emit one character
// void OLED_sendChar(char ch)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		OLED_putc
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oled-emit?"
soled_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		OLED_Ready
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oledpos!"
oled_get_pos:
        @ ( x y -- ) Set OLED position
// void OLED_setPos(uint8_t x, uint8_t y)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r1, tos		// y
	drop
	movs	r0, tos		// x
	drop
	bl		OLED_setPos
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oledpos@"
oled_set_pos:
        @ (  -- x y ) Get OLED position
// void OLED_getPos(uint8_t x, uint8_t y)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		OLED_getPosX
	movs	tos, r0		// x
	pushdatos
	bl		OLED_getPosY
	movs	tos, r0		// y
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oledcmd"
oledcmd:
        @ ( c-addr -- ) send command to OLED
// void OLED_sendCommand(static uint8_t *command, size)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// command
	drop
	bl		OLED_sendCommand
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oledclr"
oledclr:
        @ ( --  ) Clears the OLED display
// void OLED_clear()
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		OLED_clear
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oledfont"
oledfont:
        @ ( u -- ) select font for the OLED
// void OLED_setFont(OLED_FontT font);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// font
	drop
	bl		OLED_setFont
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oledcolumn!"
        @ ( u -- ) Write a column (8 pixels) to the current position
// void OLED_writeColumn(uint8_t column)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// column
	drop
	bl		OLED_writeColumn
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "oledcolumn@"
        @ (  -- u ) Read a column (8 pixels) from the current position
// int OLED_readColumn(void)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		OLED_readColumn
	movs	tos, r0		// column
	pop		{pc}


@ -----------------------------------------------------------------------------
  Wortbirne Flag_visible, "oled-test" // (  -- )
  // writes every 100 ms a character to the OLED
  // for testing the redirection (hooks)
@ -----------------------------------------------------------------------------
	bl		hook_emit		// redirect emit to OLED
	ldr		r0, =oled_emit
	str		r0, [tos]
	drop
1:
	ldr		r0, =32			// ASCCI starts at 32 ' '
	ldr		r1, =128-32		// count
2:
	movs	tos, r0
	pushdatos
	bl		emit
	push	{r0-r1}
	ldr		r0, =100
	bl		osDelay			// wait 100ms
	pop		{r0-r1}
	add		r0, r0, #1
	subs	r1, r1, #1
	bne		2b
	b		1b

.endif  // OLED == 1


// MIP words only if needed
.if MIP == 1
@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "mip-emit"
mip_emit:
        @ ( c -- ) Emit one character
// void MIP_sendChar(char ch)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		MIP_putc
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "mip-emit?"
smip_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		MIP_Ready
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "mippos!"
mip_get_pos:
        @ ( x y -- ) Set MIP position
// void MIP_setPos(uint8_t x, uint8_t y)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r1, tos		// y
	drop
	movs	r0, tos		// x
	drop
	bl		MIP_setPos
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "mippos@"
mip_set_pos:
        @ (  -- x y ) Get MIP position
// void MIP_getPos(uint8_t x, uint8_t y)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		MIP_getPosX
	movs	tos, r0		// x
	pushdatos
	bl		MIP_getPosY
	movs	tos, r0		// y
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "mipcmd"
mipcmd:
        @ ( c-addr -- ) send command to MIP
// void MIP_sendCommand(static uint8_t *command, size)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// command
	drop
	bl		MIP_sendCommand
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "mipclr"
mipclr:
        @ ( --  ) Clears the MIP display
// void MIP_clear()
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		MIP_clear
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "mipfont"
mipfont:
        @ ( u -- ) select font for the MIP
// void MIP_setFont(MIP_FontT font);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// font
	drop
	bl		MIP_setFont
	pop		{pc}

.endif // MIP == 1


// EPD words only if needed
.if EPD == 1
@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epd-emit"
epd_emit:
        @ ( c -- ) Emit one character
// void EPD_sendChar(char ch)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		EPD_putc
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epd-emit?"
sepd_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		EPD_Ready
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdpos!"
epd_get_pos:
        @ ( x y -- ) Set EPD position
// void EPD_setPos(uint8_t x, uint8_t y)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r1, tos		// y
	drop
	movs	r0, tos		// x
	drop
	bl		EPD_setPos
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdpos@"
epd_set_pos:
        @ (  -- x y ) Get EPD position
// void EPD_getPos(uint8_t x, uint8_t y)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		EPD_getPosX
	movs	tos, r0		// x
	pushdatos
	bl		EPD_getPosY
	movs	tos, r0		// y
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdcmd"
epdcmd:
        @ ( c-addr -- ) send command to EPD
// void EPD_sendCommand(static uint8_t *command, size)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// command
	drop
	bl		EPD_sendCommand
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdclr"
epdclr:
        @ ( --  ) Clears the EPD display
// void EPD_clear()
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		EPD_clear
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdfont"
epdfont:
        @ ( u -- ) select font for the EPD
// void EPD_setFont(EPD_FontT font);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// font
	drop
	bl		EPD_setFont
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdcolumn!"
        @ ( u -- ) Write a column (8 pixels) to the current position
// void EPD_writeColumn(uint8_t column)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// column
	drop
	bl		EPD_writeColumn
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdcolumn@"
        @ (  -- u ) Read a column (8 pixels) from the current position
// int EPD_readColumn(void)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		EPD_readColumn
	movs	tos, r0		// column
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "epdupdate"
epdupdate:
        @ ( --  ) Update the EPD display
// void EPD_clear()
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		EPD_update
	pop		{pc}


.endif // EPD == 1


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
