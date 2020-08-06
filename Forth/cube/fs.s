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
		@ ( -- ) Initializes the sd, sets the block count
// void SD_getSize(void)
@ -----------------------------------------------------------------------------
sdinit:
	push	{r0-r3, lr}
	bl		SD_getSize
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "sdblocks"
		@ ( -- u ) Gets the block count
// int SD_getBlocks(void)
@ -----------------------------------------------------------------------------
sdblocks:
	push	{r0-r3, lr}
	pushdatos
	bl		SD_getBlocks
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "empty-buffers"
		@ ( -- ) Marks all block buffers as empty
// void BLOCK_emptyBuffers(void)
@ -----------------------------------------------------------------------------
empty_buffers:
	push	{r0-r3, lr}
	bl		BLOCK_emptyBuffers
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "update"
		@ ( -- ) Mark most recent block as updated
// void BLOCK_update(void)
@ -----------------------------------------------------------------------------
update:
	push	{r0-r3, lr}
	bl		BLOCK_update
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "block"
		@ ( n -- addr ) Return address of buffer for block n
// uint8_t *BLOCK_get(int block_number)
@ -----------------------------------------------------------------------------
block:
	push	{r0-r3, lr}
	movs	r0, tos		// n
	bl		BLOCK_get
	movs	tos, r0		// addr
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "buffer"
		@ ( n -- addr ) Return address of buffer for block n
// uint8_t *BLOCK_assign(int block_number)
@ -----------------------------------------------------------------------------
buffer:
	push	{r0-r3, lr}
	movs	r0, tos		// n
	bl		BLOCK_assign
	movs	tos, r0		// addr
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "save-buffers"
		@ ( -- ) Transfer the contents of each updated block buffer
// void BLOCK_saveBuffers(void)
@ -----------------------------------------------------------------------------
save_buffers:
	push	{r0-r3, lr}
	bl		BLOCK_saveBuffers
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "flush"
		@ ( -- ) save-buffers empty-buffers
// void BLOCK_flushBuffers(void)
@ -----------------------------------------------------------------------------
flush:
	push	{r0-r3, lr}
	bl		BLOCK_flushBuffers
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "include"
		@  ( "filename" any -- any ) Interprets the content of the file.
// void FS_include(uint8_t *str, int count)
@ -----------------------------------------------------------------------------
include:
	push	{lr}
	bl		token		@ ( -- c-addr len )
incl:
	movs	r1, tos		// len -> count
	drop
	movs	r0, tos		// c-addr -> str
	drop
	movs	r9, psp		// get psp
	movs	r8, tos		// get tos
	bl		FS_include
	movs	psp, r9		// update psp
	movs	tos, r8		// update tos
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "included"
		@  ( c-addr len -- ) Interprets the content of the file.
// void FS_include(uint8_t *str, int count)
@ -----------------------------------------------------------------------------
included:
	push	{lr}
	b		incl


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cat"
		@ cat ( "line<EOF>" -- ) Types the content of the file.
// void FS_cat(void)
@ -----------------------------------------------------------------------------
cat:
	push	{lr}
	movs	r9, psp		// get psp
	movs	r8, tos		// get tos
	bl		FS_cat
	movs	psp, r9		// update psp
	movs	tos, r8		// update tos
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ls"
		@ ( "line<EOF>" -- ) Types the content of the file.
// void FS_ls(void)
@ -----------------------------------------------------------------------------
ls:
	push	{lr}
	movs	r9, psp		// get psp
	movs	r8, tos		// get tos
	bl		FS_ls
	movs	psp, r9		// update psp
	movs	tos, r8		// update tos
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cd"
		@  ( "line<EOF>" -- ) Change the working directory.
// void FS_cd(void)
@ -----------------------------------------------------------------------------
cd:
	push	{lr}
	movs	r9, psp		// get psp
	movs	r8, tos		// get tos
	bl		FS_cd
	movs	psp, r9		// update psp
	movs	tos, r8		// update tos
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwd"
		@ ( -- ) Types the content of the file.
// void FS_pwd(void)
@ -----------------------------------------------------------------------------
pwd:
	push	{lr}
	movs	r9, psp		// get psp
	movs	r8, tos		// get tos
	bl		FS_pwd
	movs	psp, r9		// update psp
	movs	tos, r8		// update tos
	pop		{pc}


// C Interface to some Forth Words
//********************************

// void FS_evaluate(uint8_t* str, int count);
.global		FS_evaluate
FS_evaluate:
	push 	{r4-r7, lr}
	movs	psp, r9		// get psp
	movs	tos, r8		// get tos
	pushdatos
	movs	tos, r0		// str
	pushdatos
	movs	tos, r1		// count
	bl		evaluate
	movs	r9, psp		// update psp
	movs	r8, tos		// update tos
	pop		{r4-r7, pc}


// void FS_cr(void);
.global		FS_cr
FS_cr:
	push 	{r4-r7, lr}
	movs	psp, r9		// get psp
	movs	tos, r8		// get tos
	bl		cr
	movs	r9, psp		// update psp
	movs	r8, tos		// update tos
	pop		{r4-r7, pc}


// void FS_type(uint8_t* str, int count);
.global		FS_type
FS_type:
	push 	{r4-r7, lr}
	movs	psp, r9		// get psp
	movs	tos, r8		// get tos
	pushdatos
	movs	tos, r0		// str
	pushdatos
	movs	tos, r1		// count
	bl		stype
	movs	r9, psp		// update psp
	movs	r8, tos		// update tos
	pop		{r4-r7, pc}


// void  FS_token(uint8_t **str);
.global	FS_token
FS_token:
	push 	{r4-r7, lr}
	movs	psp, r9		// get psp
	movs	tos, r8		// get tos
	push 	{r0}		// push str argument (pointer to string)
	bl		token
	pop		{r1}
	movs	r0, tos		// len -> count (RETURN)
	drop
	movs	r2, tos		// c-addr -> str
	drop
	str		r2, [r1]
	movs	r9, psp		// update psp
	movs	r8, tos		// update tos
	pop		{r4-r7, pc}
