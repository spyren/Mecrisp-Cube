/**
 *  @brief
 *      Mecrisp-Stellaris Forth tailored for using the STM Cube ecosystem.
 *
 *      Forth is a CMSIS-RTOS Thread (FreeRTOS Task) MainThread.
 *      This file is derived from Mecrisp-Stellaris forth-core.s
 *      All the assembler files from common directory are included here.
 *  @file
 *      mecrisp.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-12
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

.syntax unified
.cpu cortex-m4
//.thumb


@ -----------------------------------------------------------------------------
@ Swiches for capabilities of this chip
@ -----------------------------------------------------------------------------

.equ	registerallocator, 1
@.equ	flash16bytesblockwrite, 1
.equ	flash8bytesblockwrite, 1
@.equ	charkommaavailable, 1  Not available.

// console redirection
.equ	UART_TERMINAL, 		1
.equ	CDC_TERMINAL, 		2
.equ	CRS_TERMINAL,		3

.equ	DEFAULT_TERMINAL, CDC_TERMINAL
//.equ	DEFAULT_TERMINAL, UART_TERMINAL

.equ	TERMINAL_AUTO,		1

.equ	OLED,				1
.equ	MIP,				0
.equ	PLEX,				1
.equ	EPD,				1

@ -----------------------------------------------------------------------------
@ Start with some essential macro definitions
@ -----------------------------------------------------------------------------

.include "datastackandmacros.s"

@ -----------------------------------------------------------------------------
@ Speicherkarte für Flash und RAM
@ Memory map for Flash and RAM
@ -----------------------------------------------------------------------------

@ Konstanten für die Größe des Ram-Speichers

// see stm32wb55xx_flash_cm4.ld
/*
MEMORY
MEMORY
{
FLASH (rx)                 : ORIGIN = 0x08000000, LENGTH = 256K
FLASH_FORTH (rx)           : ORIGIN = 0x08040000, LENGTH = 128K
FLASH_DRIVE (rx)           : ORIGIN = 0x08060000, LENGTH = 384K
FLASH_BLESTACK (rx)        : ORIGIN = 0x080C0000, LENGTH = 256K
RAM_FORTH (xrw)            : ORIGIN = 0X20000000, LENGTH = 64K
RAM1 (xrw)                 : ORIGIN = 0x20010000, LENGTH = 128K
RAM_SHARED (xrw)           : ORIGIN = 0x20030000, LENGTH = 10K
}
*/

.equ	RamAnfang,				0x20000000	@ Start of RAM
.equ	RamEnde,				0x20010000	@ End   of RAM. (64 KiB RAM dictionary)

@ Konstanten für die Größe und Aufteilung des Flash-Speichers

.equ	Kernschutzadresse,		0x08040000	@ Mecrisp core never writes flash below this address.
.equ	FlashDictionaryAnfang,	0x08040000	@ 256 KiB Flash reserved for core and C.
.equ	FlashDictionaryEnde,	0x08060000	@ 128 KiB Flash available, 386 KiB for drive, 256 KiB for BLE Stack
.equ	Backlinkgrenze,			RamAnfang	@ Ab dem Ram-Start.



@ Forth Core. This file sets up dictionary structure and includes all definitions.
@ forth-core.s

@ -----------------------------------------------------------------------------
@ Festverdrahtete Kernvariablen, Puffer und Stacks zu Begin des RAMs
@ Hardwired core variables, buffers and stacks at the begin of RAM
@ -----------------------------------------------------------------------------

.set	rampointer,	RamAnfang  @ Ram-Anfang setzen  Set location for core variables.

@ Variablen des Kerns  Variables of core that are not visible

	ramallot	Dictionarypointer, 4
	ramallot	Fadenende, 4
	ramallot	konstantenfaltungszeiger, 4
	ramallot	leavepointer, 4
	ramallot	Datenstacksicherung, 4
	ramallot	Einsprungpunkt, 4

@ Variablen für das Flashdictionary  Variables for Flash management

	ramallot	ZweitDictionaryPointer, 4
	ramallot	ZweitFadenende, 4
	ramallot	FlashFlags, 4
	ramallot	VariablenPointer, 4

	ramallot	EvaluateSP,	4
	ramallot	EvaluateState, 4
	ramallot	RedirectStore, 4

	ramallot	DriveNumber, 4

.global		Dictionarypointer
.global		Fadenende
.global		ZweitDictionaryPointer
.global		ZweitFadenende
.global		EvaluateState
.global		DriveNumber


.ifdef registerallocator

@ Variablen für den Registerallokator

	ramallot	state_tos, 4
	ramallot 	constant_tos, 4

	ramallot 	state_nos, 4
	ramallot 	constant_nos, 4

	ramallot 	state_3os, 4
	ramallot 	constant_3os, 4

	ramallot 	state_4os, 4
	ramallot 	constant_4os, 4

	ramallot 	state_5os, 4
	ramallot 	constant_5os, 4

	ramallot 	sprungtrampolin, 4

	ramallot 	state_r0, 4
	ramallot 	constant_r0, 4

	ramallot 	inline_cache_count, 4


.equ allocator_base, state_tos

.equ	offset_state_tos,		0 * 4
.equ	offset_constant_tos,	1 * 4
.equ	offset_state_nos,		2 * 4
.equ	offset_constant_nos,	3 * 4
.equ	offset_state_3os,		4 * 4
.equ	offset_constant_3os,	5 * 4
.equ	offset_state_4os,		6 * 4
.equ	offset_constant_4os,	7 * 4
.equ	offset_state_5os,		8 * 4
.equ	offset_constant_5os,	9 * 4

.equ	offset_sprungtrampolin,	10 * 4

.equ	offset_state_r0,		11 * 4
.equ	offset_constant_r0,		12 * 4
.equ	offset_inline_cache_count,	13 * 4


.equ	rawinlinelength,		10 @ How many opcodes long may definitions be for direct inlining ?
.equ	inline_cache_length,	6 @ For optimisation across inlined definitions, how many compilation steps should be buffered at most ?

	ramallot	inline_cache, 6 * inline_cache_length

.endif // registerallocator

@ Jetzt kommen Puffer und Stacks:  Buffers and Stacks

.equ	Zahlenpufferlaenge,		63 			@ Number buffer (Length+1 mod 4 = 0)
	ramallot	Zahlenpuffer, Zahlenpufferlaenge+1 @ Reserviere mal großzügig 64 Bytes RAM für den Zahlenpuffer

.equ	Maximaleeingabe,		200			@ Input buffer for an Address-Length string
	ramallot	Eingabepuffer, Maximaleeingabe  @ Eingabepuffer wird einen Adresse-Länge String enthalten

.equ	stack_canary,			68295045

	ramallot	datenstackende, 256		@ Data stack
	ramallot	datenstackanfang, 0

	ramallot	returnstackende, 4		@ Return stack address end
	ramallot	returnstackanfang, 4	@ Return stack address start


.ifdef emulated16bitflashwrites
	.equ		Sammelstellen, 32					@ 32 * 6 = 192 Bytes.
	ramallot	Sammeltabelle, Sammelstellen * 6	@ 16-Bit Flash write emulation collection buffer
.endif

.ifdef flash8bytesblockwrite
	.equ		Sammelstellen, 32					@ 32 * (8 + 4) = 384 Bytes
	ramallot	Sammeltabelle, Sammelstellen * 12	@ Buffer 32 blocks of 8 bytes each for ECC constrained Flash write
.endif

.ifdef flash16bytesblockwrite
	.equ		Sammelstellen, 32 					@ 32 * (16 + 4) = 640 Bytes
	ramallot	Sammeltabelle, Sammelstellen * 20	@ Buffer 32 blocks of 16 bytes each for ECC constrained Flash write

	ramallot	iap_command, 5*4
	ramallot	iap_reply, 4*4
.endif

.equ	user_size,				128

	ramallot	uservariables, user_size
.equ	user_threadid,			0
.equ	user_argument,			4
.equ	user_attr,				8
.equ	user_XT,				12
.equ	user_R0,				16
.equ	user_S0,				20
.equ	user_base,				24
.equ	user_hook_emit,			28
.equ	user_hook_key,			32
.equ	user_hook_qemit,		36
.equ	user_hook_qkey,			40
.equ	user_stdin,				44
.equ	user_stdout,			48
.equ	user_stderr,			52
.equ	user_free,				56


.equ	RamDictionaryAnfang,	rampointer	@ Start of RAM dictionary
.equ	RamDictionaryEnde,		RamEnde		@ End of RAM dictionary = End of RAM


@ -----------------------------------------------------------------------------
@ Vorbereitung der Dictionarystruktur
@ Preparations for dictionary structure
@ -----------------------------------------------------------------------------

CoreDictionaryAnfang: @ Dictionary-Einsprungpunkt setzen
                      @ Set entry point for Dictionary

.set CoreVariablenPointer, RamDictionaryEnde @ Im Flash definierte Variablen kommen ans RAM-Ende
                                             @ Variables defined in Flash are placed at the end of RAM

  Dictionary_Welcome

@ -----------------------------------------------------------------------------
@ Include the complete Mecrisp-Stellaris core
@ -----------------------------------------------------------------------------

.ifdef registerallocator
.include "ra/ra-infrastructure.s"
.include "ra/ra-tools.s"
.ltorg

.include "ra/double.s"
.include "ra/stackjugglers.s"
.include "ra/logic.s"
.include "ra/ra-logic.s"
.include "ra/comparisions.s"
.ltorg
.include "ra/memory.s"
.include "stm-flash.s"
.ltorg

.ifdef emulated16bitflashwrites
.include "flash4bytesblockwrite.s"
.ltorg
.endif

.ifdef flash8bytesblockwrite
.include "flash8bytesblockwrite.s"
.ltorg
.endif

.ifdef flash16bytesblockwrite
.include "flash16bytesblockwrite.s"
.ltorg
.endif

.include "ra/calculations.s"
.include "terminal.s"
.include "query.s"
.ltorg
.include "strings.s"
.include "deepinsight.s"
.ltorg
.include "compiler.s"
.include "compiler-flash.s"
.ltorg
.include "ra/controlstructures.s"
.ltorg
.include "ra/doloop.s"
.include "ra/case.s"
.include "token.s"
.ltorg
.include "numberstrings.s"
.ltorg

.include "ra/ra-kompilator.s"
.include "ra/ra-inline.s"
.include "ra/interpreter.s"
.ltorg
.include "rtos.s"
.include "bsp.s"
.include "wings.s"
.include "fs.s"
.ltorg

.include "interrupts-common.s"
.include "interrupts.s" @ You have to change interrupt handlers for Porting !

.else  // registerallocator

.include "double.s"
.include "stackjugglers.s"
.include "logic.s"
.include "comparisions.s"
.ltorg
.include "memory.s"
.include "stm-flash.s"
.ltorg

.ifdef emulated16bitflashwrites
	.include "flash4bytesblockwrite.s"
	.ltorg
.endif

.ifdef flash8bytesblockwrite
	.include "flash8bytesblockwrite.s"
	.ltorg
.endif

.ifdef flash16bytesblockwrite
	.include "flash16bytesblockwrite.s"
	.ltorg
.endif

.include "calculations.s"
.include "terminal.s"
.include "query.s"
.ltorg
.include "strings.s"
.include "deepinsight.s"
.ltorg
.include "compiler.s"
.include "compiler-flash.s"
.include "controlstructures.s"
.ltorg
.include "doloop.s"
.include "case.s"
.include "token.s"
.ltorg
.include "numberstrings.s"
.ltorg
.include "interpreter.s"
.ltorg
.include "rtos.s"
.include "bsp.s"
.include "fs.s"
.ltorg

.include "interrupts-common.s"
.include "interrupts.s" @ You have to change interrupt handlers for Porting !

.endif // registerallocator

@ -----------------------------------------------------------------------------
@ Schließen der Dictionarystruktur und Zeiger ins Flash-Dictionary
@ Finalize the dictionary structure and put a pointer into changeable Flash-Dictionary
@ -----------------------------------------------------------------------------

  Wortbirne_Kernende Flag_invisible, "--- Flash Dictionary ---"

@ -----------------------------------------------------------------------------
@  End of Dictionary
@ -----------------------------------------------------------------------------

.global		Forth

@ -----------------------------------------------------------------------------
Forth:
@ -----------------------------------------------------------------------------

// Stack already set in the main thread
//	ldr		r0, =returnstackanfang
//	str		sp, [r0]

// set the local storage pointer to the user variables
	ldr		r0, =0	// current task xTaskToQuery = 0
	mov		r1, r0	// index
	ldr		r2, =uservariables
	bl		vTaskSetThreadLocalStoragePointer

// store terminal task thread ID to user variable threadid
	bl		osThreadGetId
	ldr		r2, =uservariables
	str		r0, [r2]

// store return stack bottom address to user variable R0
	str		r13, [r2, #user_R0]

// store data stack bottom address to user variable S0
	ldr		r0, =datenstackanfang
	str		r0, [r2, #user_S0]

// set user variables attr and argument to 0
	ldr		r0, =0
	str		r0, [r2, #user_attr]
	str		r0, [r2, #user_argument]

// set user variables stdin, stdout, and stderr
	str		r0, [r2, #user_stdin]
	str		r0, [r2, #user_stdout]
	str		r0, [r2, #user_stderr]

// set default hooks for terminal
.if	DEFAULT_TERMINAL == UART_TERMINAL
	ldr		r0,	=serial_emit
	str		r0, [r2, #user_hook_emit]
	ldr		r0,	=serial_key
	str		r0, [r2, #user_hook_key]
	ldr		r0,	=serial_qemit
	str		r0, [r2, #user_hook_qemit]
	ldr		r0,	=serial_qkey
	str		r0, [r2, #user_hook_qkey]
.else
.if DEFAULT_TERMINAL == CDC_TERMINAL
	ldr		r0,	=cdc_emit
	str		r0, [r2, #user_hook_emit]
	ldr		r0,	=cdc_key
	str		r0, [r2, #user_hook_key]
	ldr		r0,	=cdc_qemit
	str		r0, [r2, #user_hook_qemit]
	ldr		r0,	=cdc_qkey
	str		r0, [r2, #user_hook_qkey]
.else
.if	DEFAULT_TERMINAL == CRS_TERMINAL
	ldr		r0,	=crs_emit
	str		r0, [r2, #user_hook_emit]
	ldr		r0,	=crs_key
	str		r0, [r2, #user_hook_key]
	ldr		r0,	=crs_qemit
	str		r0, [r2, #user_hook_qemit]
	ldr		r0,	=crs_qkey
	str		r0, [r2, #user_hook_qkey]
.endif
.endif
.endif

	@ Catch the pointers for Flash dictionary
.include "catchflashpointers.s"

	ldr		r1, =DriveNumber	// set invalid drive number
	ldr		r0, =2
	str		r0, [r1]

	bl		BSP_getSwitch1
	cmp		r0, #0
	beq		1f
	bl		crs_terminal		// button1 pressed on reset -> crs terminal
1:	bl		BSP_getSwitch2
	cmp		r0, #0
	beq		2f
	bl		uart_terminal		// button2 pressed on reset -> uart terminal
2:
	welcome " by Matthias Koch. "

	bl		cr
	pushdatos
	ldr		tos, =MecrispCubeVersion	// print Mecrisp-Cube version
	bl		fs_strlen
	bl		stype
	pushdatos
	ldr		tos, =BSP_Version	// print Cube and BLE version
	bl		fs_strlen
	bl		stype
	pushdatos
	ldr		tos, =RTOS_Version	// print RTOS version
	bl		fs_strlen
	bl		stype
	pushdatos
	ldr		tos, =FS_Version	// print file system version
	bl		fs_strlen
	bl		stype
	pushdatos
	ldr		tos, =VI_Version	// print vi system version
	bl		fs_strlen
	bl		stype

	bl		BSP_getSwitch3		// button1 pressed on reset -> no include
	cmp		r0, #0
	bne		3f
    // include 0:/etc/rc.local
    pushdatos
   	ldr		tos, =rc_local
    bl		fs_strlen
    bl		included
3:
	@ Ready to fly !
.include "boot.s"

