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

@ Terminal redirection hooks.

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "hook-emit" @ ( -- addr )
  // user variable hook_emit
@------------------------------------------------------------------------------
hook_emit:
	push	{lr}
	pushdatos
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	adds	tos, r0, #user_hook_emit
	pop		{pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "hook-key" @ ( -- addr )
  // user variable hook_key
@------------------------------------------------------------------------------
hook_key:
	push	{lr}
	pushdatos
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	adds	tos, r0, #user_hook_key
	pop		{pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "hook-emit?" @ ( -- addr )
  // user variable hook_qemit
@------------------------------------------------------------------------------
hook_qemit:
	push	{lr}
	pushdatos
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	adds	tos, r0, #user_hook_qemit
	pop		{pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "hook-key?" @ ( -- addr )
  // user variable hook_qkey
@------------------------------------------------------------------------------
hook_qkey:
	push	{lr}
	pushdatos
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	adds	tos, r0, #user_hook_qkey
	pop		{pc}


@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "emit" @ ( c -- )
emit:
@------------------------------------------------------------------------------
	push	{r0-r3, lr} @ Used in core, registers have to be saved !
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	ldr		r0, [r0, #user_hook_emit]
	adds	r0, r0, #1		// thumb instruction set
	blx		r0
	pop		{r0-r3, pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "key" @ ( -- c )
key:
@------------------------------------------------------------------------------
	push	{r0-r3, lr} @ Used in core, registers have to be saved !
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	ldr		r0, [r0, #user_hook_key]
	adds	r0, r0, #1		// thumb instruction set
	blx		r0
	pop		{r0-r3, pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "emit?" @ ( -- ? )
qemit:
@------------------------------------------------------------------------------
	push	{r0-r3, lr} @ Used in core, registers have to be saved !
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	ldr		r0, [r0, #user_hook_qemit]
	adds	r0, r0, #1		// thumb instruction set
	blx		r0
	pop		{r0-r3, pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "key?" @ ( -- ? )
qkey:
@------------------------------------------------------------------------------
	push	{r0-r3, lr} @ Used in core, registers have to be saved !
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer
	ldr		r0, [r0, #user_hook_qkey]
	adds	r0, r0, #1		// thumb instruction set
	blx		r0
	pop		{r0-r3, pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible|Flag_variable, "hook-pause" @ ( -- addr )
  CoreVariable hook_pause
@------------------------------------------------------------------------------
	pushdatos
	ldr		tos, =hook_pause
	bx		lr

	.word	osDelay		// CMSIS-RTOS


@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "pause" @ ( -- ? )
pause:
@------------------------------------------------------------------------------
	ldr		r0, =1			// parameter for osDelay
 	ldr		r1, =hook_pause
	ldr		r1, [r1]
	mov		pc, r1


@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "uart" @ ( -- )
@------------------------------------------------------------------------------
.global		uart_terminal
uart_terminal:
	push	{lr}
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer

	ldr		r1, =serial_emit
	str		r1, [r0, #user_hook_emit]

	ldr		r1, =serial_qemit
	str		r1, [r0, #user_hook_qemit]

	ldr		r1, =serial_key
	str		r1, [r0, #user_hook_key]

	ldr		r1, =serial_qkey
	str		r1, [r0, #user_hook_qkey]

	pop		{pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "cdc" @ ( -- )
@------------------------------------------------------------------------------
.global		cdc_terminal
cdc_terminal:
	push	{lr}
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer

	ldr		r1, =cdc_emit
	str		r1, [r0, #user_hook_emit]

	ldr		r1, =cdc_qemit
	str		r1, [r0, #user_hook_qemit]

	ldr		r1, =cdc_key
	str		r1, [r0, #user_hook_key]

	ldr		r1, =cdc_qkey
	str		r1, [r0, #user_hook_qkey]

	pop		{pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "crs" @ ( -- )
@------------------------------------------------------------------------------
.global		crs_terminal
crs_terminal:
	push	{lr}
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	bl		pvTaskGetThreadLocalStoragePointer

	ldr		r1, =crs_emit
	str		r1, [r0, #user_hook_emit]

	ldr		r1, =crs_qemit
	str		r1, [r0, #user_hook_qemit]

	ldr		r1, =crs_key
	str		r1, [r0, #user_hook_key]

	ldr		r1, =crs_qkey
	str		r1, [r0, #user_hook_qkey]

	pop		{pc}


.ltorg

// Redirect emit to key
//*********************

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "redirect" @ ( -- )
@------------------------------------------------------------------------------
.global 	TERMINAL_redirect
TERMINAL_redirect:
	push	{lr}
	ldr		r0, =0		// current task xTaskToQuery = 0
	mov		r1, r0		// index
	bl		pvTaskGetThreadLocalStoragePointer
	adds	r0, r0, #user_hook_emit	// r0 points to hook_emit
	ldr		r2, [r0]				// r2 contains current emit

	ldr		r1, =RedirectStore
	str		r2, [r1]				// store old hook

	ldr		r1, =cdc_emit
	cmp		r2, r1
	bne		1f
	ldr		r1, =cdc_emit2key
	b		3f
1:
	ldr		r1, =serial_emit
	cmp		r2, r1
	bne		2f
	ldr		r1, =serial_emit2key
	b		3f
2:
//	ldr		r1, =crs_emit
	ldr		r1, =crs_emit2key
3:
	str		r1, [r0]
	pop		{pc}

// status = osMessageQueuePut(UART_RxQueueId, &UART_RxBuffer, 0, 100);

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "unredirect" @ ( -- )
@------------------------------------------------------------------------------
.global		TERMINAL_unredirect
TERMINAL_unredirect:
	push	{lr}
	ldr		r0, =0		// current task xTaskToQuery = 0
	mov		r1, r0		// index
	bl		pvTaskGetThreadLocalStoragePointer
	adds	r0, r0, #user_hook_emit	// r0 points to hook_emit

	ldr		r1, =RedirectStore
	ldr		r2, [r1]
	str		r2, [r0]				// restore old hook
	pop		{pc}


cdc_emit2key:
	push	{lr}
	movs	r0, tos		// c
	drop
	bl		CDC_putkey
	pop		{pc}

serial_emit2key:
	push	{lr}
	movs	r0, tos		// c
	drop
	bl		UART_putkey
	pop		{pc}

crs_emit2key:
	push	{lr}
	movs	r0, tos		// c
	drop
	bl		CRSAPP_putkey
	pop		{pc}


