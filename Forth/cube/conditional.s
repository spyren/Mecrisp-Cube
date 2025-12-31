/**
 *  @brief
 *      Conditional compilation/execution
 *
 *      Idea similar to http://lars.nocrew.org/dpans/dpansa15.htm#A.15.6.2.2532.
 *      In included files it works only on the same line:
 *      [IFNDEF] .( : .( ( -- ) [char] ) ['] ." $1E + call, parse string, immediate 0-foldable ; [THEN]
 *
 *		Or use line continuation with a backslash at the EOL.
 *		Be aware: the max. length of the concatenated string is 255 chars.
 *      [IFNDEF] .( \
 *      : .( ( -- ) \
 *        [char] ) ['] ." $1E + call, parse string, \
 *        immediate 0-foldable ; \
 *      [THEN]
 *  @file
 *      conditional.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2025-12-20
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
		Wortbirne Flag_visible, "tolower"
/*
: tolower ( C -- c ) \ only 7 bit ASCII
  dup dup [char] A >=  swap [char] Z <= and if
    32 or
  then
;
*/
tolower:
@ -----------------------------------------------------------------------------
	cmp		tos, #'A'
	blo.n	1f
	cmp		tos, #'Z'
	bhi.n	1f
	orrs	tos, #32
1:
	bx		lr


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "toupper"
/*
: toupper ( c -- C ) \ only 7 bit ASCII
  dup dup [char] a >=  swap [char] z <= and if
    32 not and
  then
;
*/
toupper:
@ -----------------------------------------------------------------------------
	cmp		tos, #'a'
	blo.n	1f
	cmp		tos, #'z'
	bhi.n	1f
	ands	tos, #0x5f
1:
	bx		lr


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "lower"
/*
: lower ( addr len -- )
  over + swap
  do
    i c@ tolower i c!
  loop
;
*/
lower:
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// len
	drop
	movs	r1, tos		// addr
1:
	cmp		r0, #0
	ble.n	2f
	ldrb	tos, [r1]
	bl		tolower		// does not change r0 and r1
	strb	tos, [r1]
	adds	r1, #1
	subs	r0, #1
	b.n		1b
2:
	drop
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "upper"
/*
: upper ( addr len -- )
  over + swap
  do
    i c@ toupper i c!
  loop
;
*/
upper:
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// len
	drop
	movs	r1, tos		// addr
1:
	cmp		r0, #0
	ble.n	2f
	ldrb	tos, [r1]
	bl		toupper		// does not change r0 and r1
	strb	tos, [r1]
	adds	r1, #1
	subs	r0, #1
	b.n		1b
2:
	drop
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "nexttoken"
/*
: nexttoken ( -- addr len )
  begin
    token          \ fetch new token.
  dup 0= while      \ if length of token is zero, end of line is reached.
    2drop cr query   \ fetch new line, this will not work in evaluate
  repeat
;
*/
nexttoken:
@ -----------------------------------------------------------------------------
	push	{lr}
1:	bl 		token
	cmp		tos, #0
	bne 	2f
	ddrop
	bl		cr
	bl 		query
	b.n		1b
2:	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "[IF*]?"
/*
: [IF*]? ( addr len -- flag ) \ true if string is "[IF]", "[IFDEF]", or "[IFNDEF]"
    2dup s" [IF]"     compare
 >r 2dup s" [IFDEF]"  compare r> or
 >r      s" [IFNDEF]" compare r> or
;
*/
bracket_if_star:
@ -----------------------------------------------------------------------------
	push	{lr}

	ddup
	bl 		dotsfuesschen
	.byte 	8f - 7f   // length of name field
7:  .ascii 	"[IF]"
8:  .p2align 1
	bl		compare

	to_r
	ddup
	bl 		dotsfuesschen
	.byte 	8f - 7f   // length of name field
7:  .ascii 	"[IFDEF]"
8:  .p2align 1
	bl		compare
	r_from
	ldm 	psp!, {r0}
	orrs 	tos, r0

	to_r
	bl 		dotsfuesschen
	.byte 	8f - 7f   // length of name field
7:  .ascii 	"[IFNDEF]"
8:  .p2align 1
	bl		compare
	r_from
	ldm 	psp!, {r0}
	orrs 	tos, r0

	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "[ENDIF*]?"
/*
: [ENDIF*]? ( addr len -- flag ) \ true if string is "[THEN]", or "[ENDIF]"
    2dup s" [THEN]"   compare
 >r      s" [ENDIF]"  compare r> or
;
*/
bracket_endif_star:
@ -----------------------------------------------------------------------------
	push	{lr}

	ddup
	bl 		dotsfuesschen
	.byte 	8f - 7f   // length of name field
7:  .ascii 	"[THEN]"
8:  .p2align 1
	bl		compare

	to_r
	bl 		dotsfuesschen
	.byte 	8f - 7f   // length of name field
7:  .ascii 	"[ENDIF]"
8:  .p2align 1
	bl		compare
	r_from
	ldm 	psp!, {r0}
	orrs 	tos, r0

	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_0, "[ELSE]"
/*
: [ELSE] ( -- )
  1 \ Initial level of nesting
  begin
    nexttoken ( level addr len )
    2dup [IF*]? if
      2drop 1+  \ One more level of nesting
    else
      2dup s" [ELSE]" compare if
        2drop 1- dup if
          1+   	\ Finished if [ELSE] is reached in level 1. Skip [ELSE] branch otherwise.
        then
      else
\ 4:
        [ENDIF*]? if
          1-  	\ Level completed.
        then
      then
    then
\ 3:
    ?dup 0=
  until

  immediate 0-foldable
;
*/
bracket_else:
@ -----------------------------------------------------------------------------

	push	{lr}
	pushdaconst 1		//  Initial level of nesting
1:
	bl		nexttoken
	ddup
	bl		bracket_if_star
	cmp		tos, #-1
	drop
	bne		2f
	ddrop
	adds	tos, #1		// One more level of nesting
	b.n		3f
2:
	ddup
	bl 		dotsfuesschen
	.byte 	8f - 7f  	// length of name field
7:  .ascii 	"[ELSE]"
8:  .p2align 1
	bl		compare
	cmp		tos, #-1
	drop
	bne		4f
	ddrop
	subs	tos, #1
	beq		3f
	adds	tos, #1		// Finished if [ELSE] is reached in level 1. Skip [ELSE] branch otherwise.
	b.n		3f
4:
	bl		bracket_endif_star
	cmp		tos, #-1
	drop
	bne		3f
	subs	tos, #1		// level completed
3:
	cmp		tos, #0
	bne		1b
	drop
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_0, "[THEN]"
/*
: [THEN]  ( -- ) immediate 0-foldable ;
*/
bracket_then:
@ -----------------------------------------------------------------------------
	bx lr


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_0, "[ENDIF]"
/*
: [ENDIF]  ( -- ) immediate 0-foldable ;
*/
bracket_endif:
@ -----------------------------------------------------------------------------
	bx lr


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_1, "[IF]"
/*
: [IF]  ( flag  --  )
  0= if
    postpone [ELSE]
  then
  immediate 1-foldable
;
*/
bracket_if:
@ -----------------------------------------------------------------------------
	push	{lr}
	cmp		tos, #0
	drop
	bne		1f
//	bl 		postpone
	bl		bracket_else
1:
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_0, "[IFDEF]"
/*
: [IFDEF]  ( "<spaces>name" -- )
  token find drop
  0= if
    postpone [ELSE]
  then
  immediate 0-foldable
;
*/
bracket_ifdef:
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		token
	bl		find
	drop
	cmp		tos, #0
	drop
	bne		1f
//	bl 		postpone
	bl		bracket_else
1:
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_0, "[IFNDEF]"
/*
: [IFNDEF] ( "<spaces>name" -- )
  token find drop
  0<> if
    postpone [ELSE]
  then
  immediate 0-foldable
;
*/
bracket_ifndef:
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		token
	bl		find
	drop
	cmp		tos, #0
	drop
	beq		1f
//	bl 		postpone
	bl		bracket_else
1:
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_0, "[UNDEFINED]"
/*
: [UNDEFINED] ( "<spaces>name" -- flag )
  token find drop
  0=
  immediate
;
*/
bracket_undefined:
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		token
	bl		find
	subs 	tos, #1		// 0=
	sbcs 	tos, tos
	mvns	tos, tos
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_immediate|Flag_foldable_0, "[DEFINED]"
/*
: [DEFINED]   ( "<spaces>name" -- flag )
  token find drop
  0<>
  immediate
;
*/
bracket_defined:
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		token
	bl		find
	subs	tos, #1		// 0<>
	sbcs 	tos, tos
	pop		{pc}


.ltorg
