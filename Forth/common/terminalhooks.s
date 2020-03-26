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
  Wortbirne Flag_visible|Flag_variable, "hook-emit" @ ( -- addr )
  CoreVariable hook_emit
@------------------------------------------------------------------------------
	pushdatos
	ldr 	tos, =hook_emit
	bx lr
.ifdef	UART_TERMINAL
	.word	serial_emit		// Serial (UART) for terminal
.endif
.ifdef	CDC_TERMNAL
	.word	cdc_emit		// USB CDC for terminal
.endif

@------------------------------------------------------------------------------
  Wortbirne Flag_visible|Flag_variable, "hook-key" @ ( -- addr )
  CoreVariable hook_key
@------------------------------------------------------------------------------
	pushdatos
	ldr		tos, =hook_key
	bx		lr
.ifdef	UART_TERMINAL
	.word	serial_key		// Serial (UART) for terminal
.endif
.ifdef	CDC_TERMINAL
	.word	cdc_key			// USB CDC for terminal
.endif

@------------------------------------------------------------------------------
  Wortbirne Flag_visible|Flag_variable, "hook-emit?" @ ( -- addr )
  CoreVariable hook_qemit
@------------------------------------------------------------------------------
	pushdatos
	ldr		tos, =hook_qemit
	bx		lr
.ifdef	UART_TERMINAL
	.word	serial_qemit	// Serial (UART) for terminal
.endif
.ifdef	CDC_TERMINAL
	.word	cdc_qemit		// USB CDC for terminal
.endif

@------------------------------------------------------------------------------
  Wortbirne Flag_visible|Flag_variable, "hook-key?" @ ( -- addr )
  CoreVariable hook_qkey
@------------------------------------------------------------------------------
	pushdatos
	ldr		tos, =hook_qkey
	bx		lr
.ifdef	UART_TERMINAL
	.word	serial_qkey		// Serial (UART) for terminal
.endif
.ifdef	CDC_TERMINAL
	.word	cdc_qkey		// USB CDC for terminal
.endif

@------------------------------------------------------------------------------
  Wortbirne Flag_visible|Flag_variable, "hook-pause" @ ( -- addr )
  CoreVariable hook_pause
@------------------------------------------------------------------------------
	pushdatos
	ldr		tos, =hook_pause
	bx		lr
  .word nop_vektor  @ No Pause defined for default
//	.word	odelay		// CMSIS-RTOS

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "emit" @ ( c -- )
emit:
@------------------------------------------------------------------------------
	push	{r0, r1, r2, r3, lr} @ Used in core, registers have to be saved !
	ldr		r0, =hook_emit
	bl		hook_intern
	pop		{r0, r1, r2, r3, pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "key" @ ( -- c )
key:
@------------------------------------------------------------------------------
	push	{r0, r1, r2, r3, lr} @ Used in core, registers have to be saved !
	ldr		r0, =hook_key
	bl		hook_intern
	pop		{r0, r1, r2, r3, pc}

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "emit?" @ ( -- ? )
@------------------------------------------------------------------------------
	ldr		r0, =hook_qemit
	ldr		r0, [r0]
	mov		pc, r0

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "key?" @ ( -- ? )
@------------------------------------------------------------------------------
	ldr		r0, =hook_qkey
	ldr		r0, [r0]
	mov		pc, r0

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "pause" @ ( -- ? )
pause:
@------------------------------------------------------------------------------
 	ldr		r0, =hook_pause
	ldr		r0, [r0]
	mov		pc, r0


@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "uart" @ ( -- )
@------------------------------------------------------------------------------
uart_terminal:
	ldr		r1, =serial_emit
	ldr		r0, =hook_emit
	str		r1, [r0]

	ldr		r1, =serial_qemit
	ldr		r0, =hook_qemit
	str		r1, [r0]

	ldr		r1, =serial_key
	ldr		r0, =hook_key
	str		r1, [r0]

	ldr		r1, =serial_qkey
	ldr		r0, =hook_qkey
	str		r1, [r0]
	bx		lr

@------------------------------------------------------------------------------
  Wortbirne Flag_visible, "cdc" @ ( -- )
@------------------------------------------------------------------------------
cdc_terminal:
	ldr		r1, =cdc_emit
	ldr		r0, =hook_emit
	str		r1, [r0]

	ldr		r1, =cdc_qemit
	ldr		r0, =hook_qemit
	str		r1, [r0]

	ldr		r1, =cdc_key
	ldr		r0, =hook_key
	str		r1, [r0]

	ldr		r1, =cdc_qkey
	ldr		r0, =hook_qkey
	str		r1, [r0]
	bx		lr


@------------------------------------------------------------------------------
hook_intern:
 	ldr		r0, [r0]
	mov		pc, r0



.macro Debug_Terminal_Init

  @ A special initialisation sequence intended for debugging
  @ Necessary when you wish to use the terminal before running catchflashpointers.

  @ Kurzschluss-Initialisierung für die Terminalvariablen

   @ Return stack pointer already set up. Time to set data stack pointer !
   @ Normaler Stackpointer bereits gesetzt. Setze den Datenstackpointer:
   ldr psp, =datenstackanfang

   @ TOS setzen, um Pufferunterläufe gut erkennen zu können
   @ TOS magic number to see spurious stack underflows in .s
   @ ldr tos, =0xAFFEBEEF
   movs tos, #42

   ldr r1, =serial_emit
   ldr r0, =hook_emit
   str r1, [r0]

   ldr r1, =serial_qemit
   ldr r0, =hook_qemit
   str r1, [r0]

   ldr r1, =serial_key
   ldr r0, =hook_key
   str r1, [r0]

   ldr r1, =serial_qkey
   ldr r0, =hook_qkey
   str r1, [r0]

   ldr r1, =nop_vektor
   ldr r0, =hook_pause
   str r1, [r0]

.endm
