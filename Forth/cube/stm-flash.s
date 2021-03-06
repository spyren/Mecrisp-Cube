/**
 *  @brief
 *      Interface to the STM HAL flash functions.
 *
 *  @file
 *      flash-wb.s
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


@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "8flash!" @ Writes 8 Bytes at once into Flash.
eightflashstore: @ ( x1 x2 addr -- ) x1 contains LSB of those 64 bits.
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// set Address
	drop
	movs	r2, tos		// set word2
	drop
	movs	r1, tos		// set word1
	drop
	bl		FLASH_programDouble
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "flashpageerase" @ ( Addr -- )
	@ Deletes one 4 KiB Flash page (sector!)
	// there are only 12 sectors for 1024 KiB
	// 16 KiB sectors
	// 0x08000000  0x08003FFF  0
	// 0x08004000  0x08007FFF  1
	// 0x08008000  0x0800BFFF  2
	// 0x0800C000  0x0800FFFF  3
    // 64 KiB sector
	// 0x08010000  0x0801FFFF  4
    // 128 KiB sectors
	// 0x08020000  0x0803FFFF  5
	// 0x08040000  0x0805FFFF  6  here starts the flash dictionary
	// 0x08060000  0x0807FFFF  7
	// 0x08080000  0x0809FFFF  8
	// 0x080A0000  0x080BFFFF  9
	// 0x080C0000  0x080DFFFF  10
	// 0x080E0000  0x080FFFFF  11
flashpageerase:
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// set Address
	drop
	bl		FLASH_erasePage
	pop		{r0-r3, pc}



.ltorg

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "eraseflash" @ ( -- )
	@ Löscht den gesamten Inhalt des Flashdictionaries.
	// 6 sectors from 0x08040000
@ -----------------------------------------------------------------------------
	ldr		r0, =FlashDictionaryAnfang
eraseflash_intern:
//	cpsid	i
	ldr		r1, =FlashDictionaryEnde
	ldr		r2, =0xFFFF

1:	ldrh	r3, [r0]
	cmp		r3, r2
	beq		2f
	pushda	r0
	dup
	write	"Erase block at  "
	bl	hexdot
	writeln " from Flash"
	bl		flashpageerase
2:	adds	r0, r0, #2
	cmp		r0, r1
	bne		1b
	writeln	"Finished. Reset !"
	pushdatos
	ldr		tos, =500			// wait 500 ms to give some time for writeln
	bl		rtos_osDelay
	bl		Restart

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "eraseflashfrom" @ ( Addr -- )
	@ Beginnt an der angegebenen Adresse mit dem Löschen des Dictionaries.
@ -----------------------------------------------------------------------------
	popda	r0
	b.n		eraseflash_intern
.ltorg
