/**
 *  @brief
 *      FAT filesystem for Secure Digital Memory Card. *
 *
 *      Block words.
 *      FS words like include.
 *      Some file tools like GNU tools e.g. ls, pwd, cat. For details see fs.c.
 *      Forth API to the FAT FS functions.
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


// block words
// ***********

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


// Forth words which call C functions and which themselves call Forth words
// ************************************************************************

// The C functions must therefore have PLC and TOS as parameters.

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "include"
		@  ( any "filename" -- any ) Interprets the content of the file.
// uint64_t FS_include  (uint64_t forth_stack, uint8_t *str, int count);
@ -----------------------------------------------------------------------------
include:
	push	{lr}
	bl		token		@ ( -- c-addr len )
incl:
	movs	r3, tos		// len -> count
	drop
	movs	r2, tos		// c-addr -> str
	drop
	movs	r0, tos		// get tos
	movs	r1, psp		// get psp
	bl		FS_include
	movs	tos, r0		// update tos
	movs	psp, r1		// update psp
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "included"
		@  ( any c-addr len -- any ) Interprets the content of the file.
// uint64_t FS_include (uint64_t forth_stack, uint8_t *str, int count);
@ -----------------------------------------------------------------------------
included:
	push	{lr}
	b		incl


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cat"
		@ cat ( "line<EOF>" -- ) Types the content of the file.
// uint64_t FS_cat (uint64_t forth_stack);
@ -----------------------------------------------------------------------------
cat:
	push	{lr}
	movs	r0, tos		// get tos
	movs	r1, psp		// get psp
	bl		FS_cat
	movs	tos, r0		// update tos
	movs	psp, r1		// update psp
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ls"
		@ ( "line<EOF>" -- ) Types the content of the file.
// uint64_t FS_ls (uint64_t forth_stack);
@ -----------------------------------------------------------------------------
ls:
	push	{lr}
	movs	r0, tos		// get tos
	movs	r1, psp		// get psp
	bl		FS_ls
	movs	tos, r0		// update tos
	movs	psp, r1		// update psp
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "cd"
		@  ( "line<EOF>" -- ) Change the working directory.
// uint64_t FS_cd (uint64_t forth_stack);
@ -----------------------------------------------------------------------------
cd:
	push	{lr}
	movs	r0, tos		// get tos
	movs	r1, psp		// get psp
	bl		FS_cd
	movs	tos, r0		// update tos
	movs	psp, r1		// update psp
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwd"
		@ ( -- ) Types the content of the file.
// uint64_t FS_pwd (uint64_t forth_stack);
@ -----------------------------------------------------------------------------
pwd:
	push	{lr}
	movs	r0, tos		// get tos
	movs	r1, psp		// get psp
	bl		FS_pwd
	movs	tos, r0		// update tos
	movs	psp, r1		// update psp
	pop		{pc}


// C Interface to some Forth Words
//********************************

// These functions call Forth words. They need a data stack SPS and
// top of stack (TOS).

// uint64_t FS_evaluate(uint64_t forth_stack, uint8_t* str, int count);
.global		FS_evaluate
FS_evaluate:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	pushdatos
	movs	tos, r2		// str
	pushdatos
	movs	tos, r3		// count
	bl		evaluate
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}


// uint64_t FS_cr(uint64_t forth_stack);
.global		FS_cr
FS_cr:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	bl		cr
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}


// uint64_t FS_type(uint64_t forth_stack, uint8_t* str, int count);
.global		FS_type
FS_type:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	pushdatos
	movs	tos, r2		// str
	pushdatos
	movs	tos, r3		// count
	bl		stype
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}


// uint64_t  FS_token(uint64_t forth_stack, uint8_t **str, int *count);
.global	FS_token
FS_token:
	push 	{r4-r7, lr}
	movs	tos, r0		// get tos
	movs	psp, r1		// get psp
	push 	{r2-r3}		// push str argument (pointer to string)
	bl		token
	pop		{r2-r3}
	movs	r1, tos		// len -> count (RETURN)
	drop
	movs	r0, tos		// c-addr -> str
	drop
	str		r0, [r2]
	str		r1, [r3]
	movs	r0, tos		// update tos
	movs	r1, psp		// update psp
	pop		{r4-r7, pc}


// FAT FS datastructures
// *********************

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#FIL"
		@ ( -- u ) Gets the FIL structure size
// int FS_FIL_size(void)
@ -----------------------------------------------------------------------------
zifferFIL:
	push	{lr}
	pushdatos
	bl		FS_FIL_size
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#FATFS"
		@ ( -- u ) Gets the FATFS structure size
// int FS_FATFS_size(void)
@ -----------------------------------------------------------------------------
zifferFATFS:
	push	{lr}
	pushdatos
	bl		FS_FATFS_size
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#DIR"
		@ ( -- u ) Gets the DIR structure size
// int FS_FIL_size(void)
@ -----------------------------------------------------------------------------
zifferDIR:
	push	{lr}
	pushdatos
	bl		FS_DIR_size
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#FILINFO"
		@ ( -- u ) Gets the FIL structure size
// int FS_FILINFO_size(void)
@ -----------------------------------------------------------------------------
zifferFILINFO:
	push	{lr}
	pushdatos
	bl		FS_FILINFO_size
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "fsize+"
		@ ( -- u ) Gets the FILINFO structure fsize offset
// int FS_FILINFO_fsize(void)
@ -----------------------------------------------------------------------------
fsizeplus:
	push	{lr}
	pushdatos
	bl		FS_FILINFO_fsize
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "fdate+"
		@ ( -- u ) Gets the FILINFO structure fdate offset
// int FS_FILINFO_fdate(void)
@ -----------------------------------------------------------------------------
fdateplus:
	push	{lr}
	pushdatos
	bl		FS_FILINFO_fdate
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ftime+"
		@ ( -- u ) Gets the FILINFO structure ftime offset
// int FS_FILINFO_ftime(void)
@ -----------------------------------------------------------------------------
ftimeplus:
	push	{lr}
	pushdatos
	bl		FS_FILINFO_ftime
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "fattrib+"
		@ ( -- u ) Gets the FILINFO structure fattrib offset
// int FS_FILINFO_fattrib(void)
@ -----------------------------------------------------------------------------
fattribplus:
	push	{lr}
	pushdatos
	bl		FS_FILINFO_fattrib
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "fname+"
		@ ( -- u ) Gets the FILINFO structure fname offset
// int FS_FILINFO_fname(void)
@ -----------------------------------------------------------------------------
fnameplus:
	push	{lr}
	pushdatos
	bl		FS_FILINFO_fname
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "altname+"
		@ ( -- u ) Gets the FILINFO structure altname offset
// int FS_FILINFO_altname(void)
@ -----------------------------------------------------------------------------
altnameplus:
	push	{lr}
	pushdatos
	bl		FS_FILINFO_altname
	movs	tos, r0
	pop		{pc}


// Forth API to the FAT FS functions
// *********************************

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "f_open"
		@  ( adr cadr u -- u )  opens a file.
// FRESULT f_open (
//  FIL* fp,           /* [OUT] Pointer to the file object structure */
//  const TCHAR* path, /* [IN] File name */
//  BYTE mode          /* [IN] Mode flags */
// );
@ -----------------------------------------------------------------------------
fs_f_open:
	push	{lr}
	movs	r2, tos		// mode
	drop
	movs	r1, tos		// path
	drop
	movs	r0, tos		// fp
	bl		f_open
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "f_close"
		@  ( adr -- u )  closes a file.
// FRESULT f_close (
//  FIL* fp     /* [IN] Pointer to the file object */
// );
@ -----------------------------------------------------------------------------
fs_f_close:
	push	{lr}
	movs	r0, tos		// fp
	bl		f_open
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "f_gets"
		@  ( adr len adr -- u )  reads a string from the file.
// TCHAR* f_gets (
//   TCHAR* buff, /* [OUT] Read buffer */
//   int len,     /* [IN] Size of the read buffer */
//   FIL* fp      /* [IN] File object */
// );
@ -----------------------------------------------------------------------------
fs_f_gets:
	push	{lr}
	movs	r2, tos		// fp
	drop
	movs	r1, tos		// len
	drop
	movs	r0, tos		// buff
	bl		f_open
	movs	tos, r0
	pop		{pc}



