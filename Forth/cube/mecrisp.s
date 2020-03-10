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
.thumb

	@ from CMSIS-SVD generated equates for STM32WB peripherals
.include "STM32WBxx_CM4.svd.equates.s"

@ -----------------------------------------------------------------------------
@ Swiches for capabilities of this chip
@ -----------------------------------------------------------------------------

.equ flash16bytesblockwrite, 1

@ .equ charkommaavailable, 1  Not available.

@ -----------------------------------------------------------------------------
@ Start with some essential macro definitions
@ -----------------------------------------------------------------------------

.include "datastackandmacros.s"

@ -----------------------------------------------------------------------------
@ Speicherkarte für Flash und RAM
@ Memory map for Flash and RAM
@ -----------------------------------------------------------------------------

@ Konstanten für die Größe des Ram-Speichers

.equ RamAnfang, 0x20000004 @ Start of RAM                   Porting: Change this !
.equ RamEnde,   0x20010000 @ End   of RAM.  64 KiB. on SRAM2 Porting: Change this !

@ Konstanten für die Größe und Aufteilung des Flash-Speichers

.equ Kernschutzadresse,     0x08020000 @ Darunter wird niemals etwas geschrieben ! Mecrisp core never writes flash below this address.
.equ FlashDictionaryAnfang, 0x08020000 @ 128 KiB für den Kern reserviert...            20 KiB Flash reserved for core.
.equ FlashDictionaryEnde,   0x080C0000 @ 1024 - 256 KiB Platz für das Flash-Dictionary    1024 KiB Flash available. Porting: Change this !
.equ Backlinkgrenze,        RamAnfang  @ Ab dem Ram-Start.


@ -----------------------------------------------------------------------------
@ Anfang im Flash - Interruptvektortabelle ganz zu Beginn
@ Flash start - Vector table has to be placed here
@ -----------------------------------------------------------------------------
.text    @ Hier beginnt das Vergnügen mit der Stackadresse und der Einsprungadresse
@.include "vectors.s" @ You have to change vectors for Porting !

@ -----------------------------------------------------------------------------
@ Include the Forth core of Mecrisp-Stellaris
@ -----------------------------------------------------------------------------

.include "forth-core.s"
@.include "rtos.s"

.global     Forth

@ -----------------------------------------------------------------------------
Forth:
Reset:	@ Einsprung zu Beginn
@ -----------------------------------------------------------------------------
	@ Initialisierungen der Hardware, habe und brauche noch keinen Datenstack dafür

	@ Initialisations for Terminal hardware, without Datastack.
	bl uart_init


	@ Catch the pointers for Flash dictionary
.include "catchflashpointers.s"

	welcome " for STM32WB55 by Matthias Koch "

	@ Ready to fly !
.include "boot.s"
