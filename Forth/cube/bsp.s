/**
 *  @brief
 *      Board Support Package.
 *
 *		LEDs and switches.
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
		Wortbirne Flag_visible, "blinkThread"
blinkThread:
		@ (  --  )
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BSP_blinkThread
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led1!"
set_led1:
		@ ( n --  ) set LED1 (blue)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		BSP_setLED1
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led1@"
get_led1:
		@ (  -- n ) get LED1 (blue)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getLED1
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led2!"
set_led2:
		@ ( n --  ) set LED2 (green)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		BSP_setLED2
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led2@"
get_led2:
		@ (  -- n ) get LED2 (green)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getLED2
	movs	tos, r0
	pop		{r0-r3, pc}

	@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led3!"
set_led3:
		@ ( n --  ) set LED3 (red)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		BSP_setLED3
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led3@"
get_led3:
		@ (  -- n ) get LED3 (red)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getLED3
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch1?"
get_switch1:
		@ (  -- n ) get switch1
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getSwitch1
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch2?"
get_switch2:
		@ (  -- n ) get switch2
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getSwitch2
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch3?"
get_switch3:
		@ (  -- n ) get switch3
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getSwitch3
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dport!"
set_dport:
		@ ( n --  ) Sets the digital output port (D0 .. D15).
// void BSP_setDigitalPort(int state);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// state
	drop
	bl		BSP_setDigitalPort
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dport@"
get_dport:
		@ (  -- n ) Gets the digital input port (D0 .. D15).
// int BSP_getDigitalPort(void);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getDigitalPort
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dpin!"
set_dpin:
		@ ( u a --  ) Sets the digital output port pin (D0 .. D15).
// void BSP_setDigitalPin(int pin_number, int state);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// state
	drop
	bl		BSP_setDigitalPin
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dpin@"
get_dpin:
		@ ( a -- u ) Gets the digital input port pin (D0 .. D15).
// int BSP_getDigitalPin(int pin_number);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	bl		BSP_getDigitalPin
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "apin@"
get_apin:
		@ ( a -- u ) Gets the analog input port pin (A0 .. A5).
// int BSP_getAnalogPin(int pin_number)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	bl		BSP_getAnalogPin
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dmod"
set_dmod:
		@ ( u a --  ) sets the pin mode: 0 in, 1 in pullup, 2 in pulldown, 3 out pushpull, 4 out open drain.
// int BSP_setDigitalPinMode(int pin_number, int mode)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// mode
	drop
	bl		BSP_setDigitalPinMode
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwmpin!"
set_pwmpin:
		@ ( u a --  ) sets the digital output port pin (D3=3, D6=6, D9=9) to a PWM value (0..1000). Frequency is 1 kHz
// int BSP_setPwmPin(int pin_number, int value)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// value
	drop
	bl		BSP_setPwmPin
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwmprescale"
pwmprescale:
		@ ( u --  ) Sets the PWM prescale for TIMER1 (D3=3, D6=6, D9=9)
// void BSP_setPwmPrescale(int16_t value)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// value
	drop
	bl		BSP_setPwmPrescale
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "sdinit"
sdinit:
		@ ( -- ) Initializes the sd, sets the block count
// void SD_getSize(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		SD_getSize
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "sdblocks"
sdblocks:
		@ ( -- u ) Gets the block count
// int SD_getBlocks(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		SD_getBlocks
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "empty-buffers"
empty_buffers:
		@ ( -- ) Marks all block buffers as empty
// void BLOCK_emptyBuffers(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BLOCK_emptyBuffers
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "update"
update:
		@ ( -- ) Mark most recent block as updated
// void BLOCK_update(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BLOCK_update
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "block"
block:
		@ ( n -- addr ) Return address of buffer for block n
// uint8_t *BLOCK_get(int block_number)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// n
	bl		BLOCK_get
	movs	tos, r0		// addr
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "buffer"
buffer:
		@ ( n -- addr ) Return address of buffer for block n
// uint8_t *BLOCK_assign(int block_number)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// n
	bl		BLOCK_assign
	movs	tos, r0		// addr
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "save-buffers"
save_buffers:
		@ ( -- ) Transfer the contents of each updated block buffer
// void BLOCK_saveBuffers(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BLOCK_saveBuffers
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "flush"
flush:
		@ ( -- ) save-buffers empty-buffers
// void BLOCK_flushBuffers(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BLOCK_flushBuffers
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "include"
include:
		@ filename ( -- ) Interprets the content of the file.
// void FS_include(uint8_t *str, int count)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		token		@ ( -- c-addr len )
	movs	r1, tos		// len -> count
	drop
	movs	r0, tos		// c-addr -> str
	drop
	movs	r8, psp		// psp
	bl		FS_include
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cat"
cat:
		@ filename ( -- ) Types the content of the file.
// void FS_cat(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r8, psp		// psp
	bl		FS_cat
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ls"
ls:
		@ filename ( -- ) Types the content of the file.
// void FS_ls(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r8, psp		// psp
	bl		FS_ls
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cd"
cd:
		@ filename ( -- ) Change the working directory.
// void FS_cd(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r8, psp		// psp
	bl		FS_cd
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwd"
pwd:
		@ ( -- ) Types the content of the file.
// void FS_pwd(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r8, psp		// psp
	bl		FS_pwd
	pop		{r0-r3, pc}



