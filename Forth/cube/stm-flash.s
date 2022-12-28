/**
 *  @brief
 *      Interface to the STM HAL flash functions.
 *
 *		The STM32H74x has 2 MiB flash in 2 banks.
 *		One page has 128 KiB, a sector has 32 bytes.
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
	Wortbirne Flag_visible, "32flash!" @ Writes 32 Bytes at once into Flash.
flashstore_32b: @ ( x1 x2 x3 x4 x5 x6 x7 x8 addr -- ) x1 contains LSB of those 256 bits.
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
  	movs 	r0, #31
  	ands 	r0, tos
  	beq 	1f
    Fehler_Quit "32flash! needs 32-aligned address"
1:
	movs	r0, tos				// addr
	drop
	ldr		r1, =flashbuffer
	str		tos, [r1, #28]		// x8
	drop
	str		tos, [r1, #24]		// x7
	drop
	str		tos, [r1, #20]		// x6
	drop
	str		tos, [r1, #16]		// x5
	drop
	str		tos, [r1, #12]		// x4
	drop
	str		tos, [r1, #8]		// x3
	drop
	str		tos, [r1, #4]		// x2
	drop
	str		tos, [r1]			// x1
	drop
	bl		FLASH_program32B	// int FLASH_program32B(uint32_t Address, uint32_t* buffer)
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "flashpageerase" @ ( Addr -- )
	@ Deletes one 128 KiB Flash page (sector!)
	// there are only 8 sectors for 1024 KiB
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
	// 8 sectors from 0x08100000
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
