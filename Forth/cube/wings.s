/**
 *  @brief
 *      Wings Support Package.
 *
 *		Adafruit wing boards.
 *  @file
 *      bsp.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-26
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
		Wortbirne Flag_visible, "neopixel!"
set_neopixel:
		@ ( n --  ) set neopixel (rgb)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		BSP_setNeoPixel
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "neopixel@"
get_neopixel:
		@ (  --  n ) get neopixel (rgb)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getNeoPixel
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "neopixels"
neopixels:
		@ ( addr len --  ) set neopixels
// void BSP_setNeoPixels(uint32_t *buffer, unit32_t len)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r1, tos		// len
	drop
	movs	r0, tos		// buffer
	drop
	bl		BSP_setNeoPixels
	pop		{pc}


/*
	void BSP_neopixelDataTx(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t rgb);
	                                      R0              R1        R2

	Cycle = 1 / 32 MHz = 31 ns
*/
.ltorg

.equ	T0H,		3	// 0.3 us
.equ	T1H,		8	// 0.8 us
.equ	T0L,		7	// 0.8 us
.equ	T1L,		2	// 0.3 us

//	Registers
.equ	GPIO_BSRR,	0x18	// GPIOx->BSRR bit set/reset


.global		BSP_neopixelDataTx
BSP_neopixelDataTx:
	push	{r4-r6, lr}
	lsl		r2, r2, #8			// r2 = rrggbb00
	mov		r3, r2				// r3 = rrggbb00
	bfc		r3, #16, #16		// r3 = 0000bb00
	rev16	r2, r2				// r2 = ggrr00bb
	bfc		r2, #0, #16			// r2 = ggrr0000
	add		r2, r2, r3			// r2 = ggrrbb00

	lsl		r3, r1, #16			// clear port pin for BSRR
	mov		r6, #24				// 24 bits

bit_loop:
	lsls	r2, r2, #1			// get the next bit -> set the carry bit
	ittee	cs
	movcs	r4, #T1H
	movcs	r5, #T1L
	movcc	r4, #T0H
	movcc	r5, #T0L


	// set DOUT pin high
	str		r1, [r0, #GPIO_BSRR]
1:	subs	r4, r4, #1
	bne		1b

	// set DOUT pin low
	str		r3, [r0, #GPIO_BSRR]
2:	subs	r5, r5, #1
	bne		2b

	subs	r6, r6, #1
	bne		bit_loop

	pop		{r4-r6, pc}


/*
	void BSP_neopixelBufferTx(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint32_t *buffer, uint32_t len);
	                                      R0              R1             R2 rgb           R3

	Cycle = 1 / 168 MHz = 6 ns
	R0 GPIOx
	R1 GPIO_Pin set
	R2 *buffer
	R3 pixels
	R4 high time
	R5 low time
	R6 bit count
	R7 bits
	R8 GPIO_Pin reset
*/


.global		BSP_neopixelBufferTx
BSP_neopixelBufferTx:
	push	{r4-r8, lr}
	lsl		r8, r1, #16			// clear port pin for BSRR

pixel_loop:
	ldr		r7, [r2]			// r7 = 00rrggbb
	lsl		r7, r7, #8			// r7 = rrggbb00
	mov		r6, r7				// r6 = rrggbb00
	bfc		r6, #16, #16		// r6 = 0000bb00
	rev16	r7, r7				// r7 = ggrr00bb
	bfc		r7, #0, #16			// r7 = ggrr0000
	add		r7, r7, r6			// r7 = ggrrbb00
	mov		r6, #24				// 24 bits

bit_loop2:
	lsls	r7, r7, #1			// get the next bit -> set the carry bit
	ittee	cs
	movcs	r4, #T1H
	movcs	r5, #T1L
	movcc	r4, #T0H
	movcc	r5, #T0L


	// set DOUT pin high
	str		r1, [r0, #GPIO_BSRR]
1:	subs	r4, r4, #1
	bne		1b

	// set DOUT pin low
	str		r8, [r0, #GPIO_BSRR]
2:	subs	r5, r5, #1
	bne		2b

	subs	r6, r6, #1
	bne		bit_loop2

	add		r2, r2, #4
	subs	r3, r3, #1
	bne		pixel_loop

	pop		{r4-r8, pc}


// PLEX words only if needed
.if PLEX == 1

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plex-emit"
plex_emit:
        @ ( c -- ) Emit one character
// void PLEX_sendChar(char ch)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		PLEX_putc
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plex-emit?"
plex_qemit:
        @ ( -- ? ) Ready to send a character ?
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		PLEX_Ready
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexpos!"
plex_get_pos:
        @ ( x -- ) Set PLEX position
// void PLEX_setPos(uint8_t x)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// x
	drop
	bl		PLEX_setPos
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexpos@"
plex_set_pos:
        @ (  -- x ) Get PLEX position
// void PLEX_getPos(uint8_t x)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		PLEX_getPosX
	movs	tos, r0		// x
	pushdatos
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexshutdown"
plexshutdown:
        @ ( u -- )
// void PLEX_shutdown(uint8_t status)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// leds
	drop
	bl		PLEX_shutdown
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexclr"
plexclr:
        @ ( -- )
// void PLEX_clear(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		PLEX_clear
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexfont"
plexfont:
        @ ( u -- ) select font for the Plex
// void PLEX_setFont(PLEX_FontT font);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// font
	drop
	bl		PLEX_setFont
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexpwm"
plexpwm:
        @ ( u -- ) select PWM (brightness) for the Plex emit
// void PLEX_setPWM(int PWM)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// font
	drop
	bl		PLEX_setPWM
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexcolumn!"
set_plexcolumn:
        @ ( u1 u2 u3 -- )
// void PLEX_setColumn(uint8_t col, uint8_t leds, int brightness)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r2, tos		// brightness
	drop
	movs	r1, tos		// leds
	drop
	movs	r0, tos		// col
	drop

	bl		PLEX_setColumn
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexcolumn@"
get_plexcolumn:
        @ ( u -- )
// int PLEX_getColumn(uint8_t col)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// col
	bl		PLEX_getColumn
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexpixel!"
set_plexpixel:
        @ ( u1 u2 u3 -- )
// void PLEX_setPixel(uint8_t x, uint8_t y, int brightness
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r2, tos		// brightness
	drop
	movs	r1, tos		// y
	drop
	movs	r0, tos		// x
	drop

	bl		PLEX_setPixel
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexpixel@"
get_plexpixel:
        @ ( u -- )
// int PLEX_getPixel(uint8_t col, uint8_t row)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// col
	bl		PLEX_getPixel
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexframe!"
set_plexframe:
        @ ( u -- )
// void PLEX_setFrame(uint8_t frame)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// frame
	drop
	bl		PLEX_setFrame
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexframe@"
get_plexframe:
        @ ( u -- )
// int PLEX_getFrame(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		PLEX_getFrame
	pushdatos
	movs	tos, r0		// frame
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexdisplay!"
set_plexdisplay:
        @ ( u -- )
// void PLEX_setDisplay(uint8_t frame)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// frame
	drop
	bl		PLEX_setDisplay
	pop		{pc}


@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "plexdisplay@"
get_plexdisplay:
        @ ( u -- )
// int PLEX_getDisplay(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		PLEX_getDisplay
	pushdatos
	movs	tos, r0		// frame
	pop		{pc}

.endif

