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

@.equ	flash16bytesblockwrite, 1
.equ	flash8bytesblockwrite, 1
@.equ	charkommaavailable, 1  Not available.

// console redirection
.equ	UART_TERMINAL, 		1
.equ	CDC_TERMINAL, 		2
.equ	CRS_TERMINAL,		3

.equ	DEFAULT_TERMINAL, CDC_TERMINAL

.equ	TERMINAL_AUTO,		1

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
{
FLASH (rx)                 : ORIGIN = 0x08000000, LENGTH = 256K
FLASH_FORTH (rx)           : ORIGIN = 0x08040000, LENGTH = 512K
FLASH_BLESTACK (rx)        : ORIGIN = 0x080C0000, LENGTH = 256K
RAM_FORTH (xrw)            : ORIGIN = 0X20000000, LENGTH = 64K
RAM1 (xrw)                 : ORIGIN = 0x20010000, LENGTH = 192K
RAM_SHARED (xrw)           : ORIGIN = 0x20030000, LENGTH = 10K
}
*/

.equ	RamAnfang,				0x20000000	@ Start of RAM
.equ	RamEnde,				0x20010000	@ End   of RAM. (64 KiB RAM dictionary)

@ Konstanten für die Größe und Aufteilung des Flash-Speichers

.equ	Kernschutzadresse,		0x08040000	@ Mecrisp core never writes flash below this address.
.equ	FlashDictionaryAnfang,	0x08040000	@ 256 KiB Flash reserved for core and C.
.equ	FlashDictionaryEnde,	0x080C0000	@ 512 KiB Flash available C0000 - 40000, 256 KiB for BLE Stack
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

@ Jetzt kommen Puffer und Stacks:  Buffers and Stacks

.equ	Zahlenpufferlaenge,		63 			@ Number buffer (Length+1 mod 4 = 0)
	ramallot	Zahlenpuffer, Zahlenpufferlaenge+1 @ Reserviere mal großzügig 64 Bytes RAM für den Zahlenpuffer

.equ	Maximaleeingabe,		200			@ Input buffer for an Address-Length string
	ramallot	Eingabepuffer, Maximaleeingabe  @ Eingabepuffer wird einen Adresse-Länge String enthalten

.equ	stack_canary,			68295045

	ramallot	datenstackende, 256		@ Data stack
	ramallot	datenstackanfang, 0

@ not needed anymore, stack is set by RTOS
	ramallot	returnstackende, 0		@ Return stack
	ramallot	returnstackanfang, 0


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

.include "interrupts-common.s"
.include "interrupts.s" @ You have to change interrupt handlers for Porting !

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

	@ Catch the pointers for Flash dictionary
.include "catchflashpointers.s"

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

	@ Ready to fly !
.include "boot.s"

