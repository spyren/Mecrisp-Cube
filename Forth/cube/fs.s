/**
 *  @brief
 *      FAT filesystem for Secure Digital Memory Card. *
 *
 *  @file
 *      fs.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-07-17
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
		@  ( "filename" -- ) Interprets the content of the file.
// void FS_include(uint32_t psp, uint32_t tos, uint8_t *str, int count)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		token		@ ( -- c-addr len )
	movs	r3, tos		// len -> count
	drop
	movs	r2, tos		// c-addr -> str
	drop
	movs	r0, psp		// psp
	movs	r1, tos		// tos
	bl		FS_include
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cat"
cat:
		@ cat ( "line<EOF>" -- ) Types the content of the file.
// void FS_cat(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, psp		// psp
	movs	r1, tos		// tos
	bl		FS_cat
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ls"
ls:
		@ ( "line<EOF>" -- ) Types the content of the file.
// void FS_ls(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, psp		// psp
	movs	r1, tos		// tos
	bl		FS_ls
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cd"
cd:
		@  ( "line<EOF>" -- ) Change the working directory.
// void FS_cd(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, psp		// psp
	movs	r1, tos		// tos
	bl		FS_cd
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwd"
pwd:
		@ ( -- ) Types the content of the file.
// void FS_pwd(void)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, psp		// psp
	movs	r1, tos		// tos
	bl		FS_pwd
	pop		{r0-r3, pc}


// C Interface to some Forth Words
//********************************

// void FS_evaluate(uint32_t psp, uint32_t tos, uint8_t* str, int count);
.global		FS_evaluate
FS_evaluate:
	push 	{r4-r7, lr}	// save registers used by Forth
	movs	psp, r0		// psp
	movs	tos, r1		// tos
	pushdatos
	movs	tos, r2		// str
	pushdatos
	movs	tos, r3		// count
	bl		evaluate
	pop		{r4-r7, pc}


// void FS_cr(uint32_t psp, uint32_t tos);
.global		FS_cr
FS_cr:
	push 	{r4-r7, lr}	// save registers used by Forth
	movs	psp, r0		// psp
	movs	tos, r1		// tos
	bl		cr
	pop		{r4-r7, pc}


// void FS_type(uint32_t psp, uint32_t tos, uint8_t* str, int count);
.global		FS_type
FS_type:
	push	{r4-r7, lr}	// save registers used by Forth
	movs	psp, r0		// psp
	movs	tos, r1		// tos
	pushdatos
	movs	tos, r2		// str
	pushdatos
	movs	tos, r3		// count
	bl		stype
	pop		{r4-r7, pc}


// int  FS_token(uint32_t psp, uint32_t tos, uint8_t **str);
.global	FS_token
FS_token:
	push 	{r4-r7, lr}	// save registers used by Forth
	movs	psp, r0		// psp
	movs	tos, r1		// tos
	push 	{r2}		// push str argument (pointer to string)
	bl		token
	pop		{r2}
	movs	r0, tos		// len -> count (RETURN)
	drop
	movs	r1, tos		// c-addr -> str
	drop
	str		r1, [r2]
	pop		{r4-r7, pc}
