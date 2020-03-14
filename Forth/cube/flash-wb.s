/**
 *  @brief
 *      Interface to the HAL flash functions.
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
octflashstore: @ ( x1 x2 addr -- ) x1 contains LSB of those 64 bits.
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// set Address
	drop
	movs	r2, tos		// set word2
	drop
	movs	r1, tos		// set word1
	drop
	bl		FLASH_programDouble
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "16flash!" @ Writes 16 Bytes at once into Flash.
hexflashstore: @ ( x1 x2 x3 x4 addr -- ) x1 contains LSB of those 128 bits.
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// set Address
	drop
	push	{r0}		// save Address
	drop
	push	{tos}		// save x4
	drop
	push	{tos}		// save x3
	drop
	movs	r2, tos		// set word2
	drop
	movs	r1, tos		// set word1
	bl		FLASH_programDouble
	pop		{r1}		// get x3 -> word1
	pop		{r2}		// get x4 -> word2
	pop		{r0}		// get Address
	add		r0, r0, #4
	bl		FLASH_programDouble
	pop		{pc}


@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "flashpageerase" @ ( Addr -- )
	@ Löscht einen 4 KiB großen Flashblock  Deletes one 4 KiB Flash page
flashpageerase:
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// set Address
	drop
	bl		FLASH_erasePage
	pop		{pc}



.ltorg

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "eraseflash" @ ( -- )
	@ Löscht den gesamten Inhalt des Flashdictionaries.
@ -----------------------------------------------------------------------------
	ldr	r0, =FlashDictionaryAnfang
eraseflash_intern:
	cpsid	i
	ldr	r1, =FlashDictionaryEnde
	ldr	r2, =0xFFFF

1:      ldrh	r3, [r0]
	cmp	r3, r2
	beq	2f
	pushda	r0
	dup
	write	"Erase block at  "
	bl	hexdot
	writeln " from Flash"
	bl	flashpageerase
2:      adds	r0, r0, #2
	cmp	r0, r1
	bne	1b
	writeln	"Finished. Reset !"
	bl	Restart

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "eraseflashfrom" @ ( Addr -- )
	@ Beginnt an der angegebenen Adresse mit dem Löschen des Dictionaries.
@ -----------------------------------------------------------------------------
	popda	r0
	b.n	eraseflash_intern
.ltorg
