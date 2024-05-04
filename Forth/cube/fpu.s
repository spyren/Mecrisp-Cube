/**
 *  @brief
 *      Interface to the FPU (Floating Point Unit)
 *
 *  @file
 *      fpu.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2022-11-01
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

// FPU words only if needed
.if FPU == 1

@ -----------------------------------------------------------------------------
        Wortbirne Flag_inline, "fflags@"
get_fflags:
        @ ( -- u ) get the current value of the Floating Point Status/Control register FPSCR
@ -----------------------------------------------------------------------------
	pushdatos
	vmrs	tos, FPSCR
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_inline, "fflags!"
set_fflags:
        @ ( u --  ) assign the given value to the Floating Point Status/Control register FPSCR
@ -----------------------------------------------------------------------------
	vmsr	FPSCR, tos
	drop
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_2|Flag_inline, "f+"
f_add:
        @ ( r1 r2 -- r3 ) Add r1 to r2 giving the sum r3.
@ -----------------------------------------------------------------------------
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vadd.f32 s0, s1
	vmov 	tos, s0
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_2|Flag_inline, "f-"
f_sub:
        @ ( r1 r2 -- r3 ) Subtract r2 from r1, giving r3.
@ -----------------------------------------------------------------------------
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vsub.f32 s0, s1
	vmov 	tos, s0
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_2|Flag_inline, "f*"
f_star:
        @ ( r1 r2 -- r3 ) Multiply r1 by r2 giving r3.
@ -----------------------------------------------------------------------------
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vmul.f32 s0, s1
	vmov 	tos, s0
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_2|Flag_inline, "f/"
f_slash:
        @ ( r1 r2 -- r3 ) Divide r1 by r2, giving the quotient r3.
@ -----------------------------------------------------------------------------
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vdiv.f32 s0, s0, s1
	vmov 	tos, s0
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_1|Flag_inline, "fsqrt"
fsqrt:
        @ ( r1 -- r2 ) r2 is the square root of r1.
@ -----------------------------------------------------------------------------
	vmov 	s0, tos
	vsqrt.f32 s0, s0
	vmov 	tos, s0
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_1|Flag_inline, "fabs"
fabs:
        @ ( r1 -- r2 ) r2 is the absolute value of r1.
@ -----------------------------------------------------------------------------
	vmov 	s0, tos
	vabs.f32 s0, s0
	vmov 	tos, s0
	bx 		lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "10**>f"
pow10_to_f:
        @ ( n -- r ) raise 10 to the power n, giving r.
@ -----------------------------------------------------------------------------
	mov			r0, tos
	cmp			r0, #0
	bne			1f
	vmov		s0, #1.0		// 10^0 = 1
	vmov		tos, s0
	bx			lr
1:
	bmi			4f				// exponent negative

	vmov 		s0, #10.0
	vmov		s1, s0
2:
	sub			r0, #1
	cmp			r0, #0
	beq			3f
	vmul.f32 	s0, s0, s1
	b			2b
4:
	vmov 		s0, #1.0
	vmov		s1, #10.0
5:
	vdiv.f32 	s0, s0, s1
	add			r0, #1
	cmp			r0, #0
	bpl			3f
	b			5b
3:
	vmov		tos, s0
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "flog>n"
flog_to_n:
        @ ( r -- n ) n is the base-ten logarithm of r.
@ -----------------------------------------------------------------------------
	mov			r0, #0
	vmov 		s0, tos
	vmov		s1, #10.0
	mov			tos, #0				// start with exponent 0
	vmov		s2, r0				// 0.0
	vcmp.f32	s0, s2
	vmrs 		APSR_nzcv, FPSCR
	bne			1f
	ldr			tos, =0x7fffffff	// -infinity
	bx			lr
1:
	bpl			2f
	ldr			tos, =0				// NaN
	bx			lr
2:
	vmov		s2, #1.0
	vcmp.f32	s0, s2
	vmrs 		APSR_nzcv, FPSCR
	blt			3f
	b			6f
4:
	add			tos, #1
6:
	vdiv.f32 	s0, s0, s1
	vmov		s2, #1.0
	vcmp.f32	s0, s2
	vmrs 		APSR_nzcv, FPSCR
	bpl			4b
	bx			lr

3:
	ldr			r0, =0x3F7FFF58     // 0.99999
	vmov		s2, r0
5:
	sub			tos, #1
	vmul.f32 	s0, s0, s1
	vcmp.f32	s0, s2
	vmrs 		APSR_nzcv, FPSCR
	ble			5b
6:
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_1|Flag_inline, "fnegate"
fnegate:
        @ ( r1 -- r2 ) r2 is the negation of r1.
@ -----------------------------------------------------------------------------
	vmov 		s0, tos
	vneg.f32 	s0, s0
	vmov 		tos, s0
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fround"
fround:
        @ ( r1 -- r2 ) round r1 to an integral value using the "round to nearest" rule, giving r2
@ -----------------------------------------------------------------------------
	vmov 		s0, tos
	ldr			r0, =0x3f000000	// 0.5
	tst			tos, #0x80000000
	beq			1f
	ldr			r0, =0xbf000000	// -0.5
1:
	vmov		s1, r0
	vadd.f32	s0, s1
	vcvt.s32.f32 s0, s0
	vcvt.f32.s32 s0, s0
	vmov 		tos, s0
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_1|Flag_inline, "f>s"
f_to_s:
        @ ( r -- n ) n is the single-cell signed-integer equivalent of the integer portion of r.
@ -----------------------------------------------------------------------------
	vmov 		s0, tos
	vcvt.s32.f32 s0, s0
	vmov 		tos, s0
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_foldable_1|Flag_inline, "s>f"
s_to_f:
        @ ( n -- r ) r is the floating-point equivalent of the single-cell value n.
@ -----------------------------------------------------------------------------
	vmov 		s0, tos
	vcvt.f32.s32 s0, s0
	vmov 		tos, s0
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_inline, "pi"
pi:
        @ (  -- r ) r is pi approx. 3.14159274101257324
@ -----------------------------------------------------------------------------
	pushdatos
	mov 		tos, #0x0fdb
	movt 		tos, #0x4049
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_inline, "e"
euler:
        @ (  -- r ) r is e approx. 2.718281828459045235360287
@ -----------------------------------------------------------------------------
	pushdatos
	mov 		tos, #0xf854
	movt 		tos, #0x402d
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "f0="
        @ ( r -- ? )          flag is true if r is equal to zero
@ -----------------------------------------------------------------------------
	vmov 		s0, tos
	vcmp.f32	s0, #0.0
	vmrs 		APSR_nzcv, FPSCR
	beq			1f
	mov			tos, #0
	bx			lr
1:
	mov			tos, #-1
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "f0<"
        @ ( r -- ? )          flag is true if r is less than zero
f0less:
@ -----------------------------------------------------------------------------
	tst			tos, #0x80000000
	beq			1f
	mov			tos, #-1
	bx			lr
1:
	mov			tos, #0
	bx			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_2, "f<"
        @ ( r1 r2 -- ? )      flag is true if r1 is less than r2
@ -----------------------------------------------------------------------------
	vmov 		s0, tos			// r2
	drop
	vmov 		s1, tos			// r1
	vcmp.f32	s1, s0
	vmrs 		APSR_nzcv, FPSCR
	blt			1f
	mov			tos, #0
	bx			lr
1:
	mov			tos, #-1
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_3, "f~"
        @ ( r1 r2 r3 -- ? )   If r3 is positive, flag is true if the absolute value of (r1 minus r2) is less than r3
        @				If r3 is zero, flag is true if the implementation-dependent encoding of r1 and r2 are exactly identical
        @                 (positive and negative zero are unequal if they have distinct encodings).
        @               If r3 is negative, flag is true if the absolute value of (r1 minus r2) is less than the absolute value
        @                 of r3 times the sum of the absolute values of r1 and r2.
@ -----------------------------------------------------------------------------
	vmov 		s2, tos		// r3
	drop
	vmov 		s1, tos		// r2
	drop
	vmov 		s0, tos		// r1
	vsub.f32	s0, s1
	vabs.f32 	s0, s0
	vcmp.f32	s0, s2
	vmrs 		APSR_nzcv, FPSCR
	ble			1f
	mov			tos, #0
	bx			lr
1:
	mov			tos, #-1
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "f>expo"
f2expo:
        @ ( r1 -- n )       get exponent
@ -----------------------------------------------------------------------------
	lsr			tos, tos, #23		// remove fraction
	and			tos, tos, #0xff		// remove sign
	sub			tos, tos, #127		// bias
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "f>fract"
f2fract:
        @ ( r1 -- u )       get fraction part of r1
@ -----------------------------------------------------------------------------
	lsl			tos, tos, 9		// remove sign and exponent
	lsr			tos, tos, 9
	orr			tos, tos, #0x00800000		// set 23. bit
	bx 			lr


@ -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//   s31.32 <-> Float conversions by Ivan Dimitrov
// -----------------------------------------------------------------------------

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "f>x"
f_to_x:
        @ ( r -- d ) d is the fixed-point equivalent of the floating-point r.
	// : v>f ( v -- df )
    //   dup 1 31 lshift and 0<> swap ( nf v )
    //   vabs                         ( nf v )
    //   dup v>n                      ( nf v h )
    //   swap over                    ( nf h v h )
    //   n>v                          ( nf h v h' )
    //   vsub $4f800000 vmul \ low part times 2^32 (v - h') * 4f800000
    //   v>u swap rot                 ( l h s )
    //   if dnegate then
	// ; 1-foldable

@ -----------------------------------------------------------------------------
	push		{lr}
	movs		r1, #0
								// dup 1 31 lshift and
    movs		r0, #0x80000000
    and			r1, r0, tos		// tos v, r1 S
	vmov 		s0, tos			// s0 v
	vabs.f32 	s0, s0			// s0 v
	vcvt.s32.f32 s1, s0			// v>n s1 h
	vmov		r0, s1			// r0 h
	vcvt.f32.s32 s3, s1			// n>v s3 h'
	movs		r2, #0x4f800000
	vmov		s2, r2			// s2 0x4f800000
	vsub.f32 	s0, s0, s3
	vmul.f32 	s0, s0, s2		// s0 l
	vcvt.u32.f32 s0, s0
	vmov		tos, s0			// tos l
	cmp			r1, #0
  	beq 		2f 				// Zero? Nothing to do!
  	movs 		r2, #0			// dnegate
	mvns 		tos, tos
	mvns 		r0, r0
 	adds 		tos, #1
	adcs 		r0, r2
2:
	pushdatos
	movs		tos, r0
 	pop			{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "x>f"
x_to_f:
        @ ( d -- r ) r is the floating-point equivalent of the fixed-point d.
	// : f>v ( df -- v ) L H -- v
    //   dup 1 31 lshift and -rot ( S L H )
    //   dabs u>v  swap u>v       ( S vh vl )
    //   $2f800000 vmul vadd      ( S v )
    //   or                       \ apply the sign
    // ; 2-foldable
@ -----------------------------------------------------------------------------
	push		{lr}
	movs		r1, tos			// r1  H
	drop						// tos L
    movs		r0, #0x80000000
    and			r0, r0, r1		// r0 S
    							// dabs d
  	cmp 		r0, #0   		// check sign in H
  	bpl 		1f 				// positive? Nothing to do!
  	movs 		r2, #0			// dnegate r1 tos
	mvns 		tos, tos
	mvns 		r1, r1
 	adds 		tos, #1
	adcs 		r1, r2
1:
	vmov 		s1, r1			// H u>v, vh
	vcvt.f32.s32 s1, s1
	vmov 		s0, tos			// L u>v, vl
	vcvt.f32.s32 s0, s0
    movs		r3, #0x2f800000
    vmov		s2, r3
	vmul.f32 	s0, s2
	vadd.f32 	s0, s1

	vmov 		tos, s0
  	orr			tos, r0			// set sign
	pop			{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f."
f_dot:
        @ ( r --  ) Display, with a trailing space, the rounded floating-point number r in fixed-point notation
@ -----------------------------------------------------------------------------
	push 		{r0-r3, lr}
	tst			tos, #0x80000000	// is r negative?
	beq			1f
	pushdaconst	'-'
	bl			emit				// sign
1:
	and			tos, #0x7fffffff	// abs
	ldr			r2, =Fprecision		// round to the precision
	ldr			r2, [r2]
	ldr			r0, =0x3F000000 	//  0.5
	vmov		s0, r0
	ldr			r1, =0x41200000  	//  10.0
	vmov		s1, r1
1:
	vdiv.f32	s0, s0, s1
	sub			r2, #1				// all digits?
	cmp			r2, #0
	bne			1b

	vmov		s1, tos
	vadd.f32	s0, s1				// round

	vmov		tos, s0
	bl			f_to_x				// f>x

	bl			zifferstringanfang	// <#
	pushdaconst	0
	bl			alleziffern			// #s
	drop
	mov			tos, #'.'
	bl			zahlanhaengen		// hold<
	ldr			r2, =Fprecision		// add <precision> fract digits
	ldr			r2, [r2]
2:
	bl			fziffer
	sub			r2, #1				// all digits?
	cmp			r2, #0
	bne			2b

	pushdatos
	bl			zifferstringende	// #>
	bl			stype				// type
	bl			space				// space
	pop			{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fs."
fs_type:
        @ ( r --  )     display, with a trailing space, the floating-point number r in scientific notation
@ -----------------------------------------------------------------------------
// s16 |r|
// s17 exponent decimal
// s18
// s19 divisor
// s20 digit

	push		{r0-r3, r8,r9, lr}
	mov			r8, #0				// scientific exponent
	bl			f_dot_special_values
	pushdatos
	vmov		tos, s16
	bl			flog_to_n			// get the integer part of exponent
	vmov 		s17, tos
	vcvt.f32.s32 s17, s17 			// exponent decimal -> s17
	vmov		s18, s17
	bl			f_dot_round
	bl			pow10_to_f			// exponent in tos
	vmov		s1, tos
	drop
	vmov		s19, s1				// save divisor
	vdiv.f32	s0, s16, s1
	vcvt.s32.f32 s0, s0 			// digit
	vcvt.f32.s32 s20, s0
	pushdatos
	vmov		tos, s0
	add			tos, tos, #'0'		// '0'+digit
	bl			emit

	pushdatos
	mov			tos, #'.'			// '.'
	bl			emit

	ldr			r0, =Fprecision
	ldr			r0, [r0]
2:
	vmul.f32	s0, s19, s20		// remove last digit
	vsub.f32	s16, s16, s0
	vmov		s1, #10.0
	vmul.f32	s16, s1
	vdiv.f32	s0, s16, s19
	vcvt.s32.f32 s0, s0 			// digit
	vcvt.f32.s32 s20, s0
	pushdatos
	vmov		tos, s0
	add			tos, tos, #'0'		// '0'+digit
	bl			emit

	sub			r0, #1				// all digits?
	cmp			r0, #0
	bne			2b

	b			f_dot_exponent

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fe."
fe_type:
        @ ( r --  )     display, with a trailing space, the floating-point number r in engineering notation
@ -----------------------------------------------------------------------------
// s16 |r|
// s17 exponent decimal (engineering)
// s18 exponent decimal
// s19 divisor
// s20 digit

	push		{r0-r3, r8, r9, lr} // -- r
	mov			r8, #1				// engineering exponent
fe_m_type:
	bl			f_dot_special_values //
	pushdatos
	vmov		tos, s16			// -- r |r|
	bl			flog_to_n			// get the integer part of exponent
	vmov 		s17, tos			// tos exponent
	vcvt.f32.s32 s17, s17 			// exponent decimal -> s17
	vmov		s18, s17            // old exponent
									// exponent mod 3: 0 > 0; 1 > -1; 2 > +1; -1 > +1; -2 > -1
	pushdatos						// -- r exp exp
	pushdatos
	mov			tos, #3				// -- r exp exp 3
	bl			divmod				// -- nexp remainder
	drop							// -- r exp remainder

	mov			r0, tos				// remainder
	drop
	mov			r1, tos				// exponent
	drop

	cmp			r0, #0
	bne			1f
	mov			r9, #1				// position decimal point
	vmov		s18, s17
	b			3f
1:
	cmp			r0, #1
	bne			1f
	sub			r1, #1
	mov			r9, #2				// position decimal point
	b			2f
1:
	cmp			r0, #2
	bne			1f
	sub			r1, #2
	mov			r9, #3				// position decimal point
	b			2f
1:
	cmp			r0, #-1
	bne			1f
	sub			r1, #2
	mov			r9, #3				// position decimal point
	b			2f
1:
	cmp			r0, #-2
	bne			3f
	sub			r1, #1
	mov			r9, #2				// position decimal point

2:
	vmov		s18, s17			// old exponent
	vmov		s17, r1				// save new exponent
	vcvt.f32.s32 s17, s17
3:
	pushdatos
	vcvt.s32.f32 s0, s18 			// old exponent
	vmov		tos, s0
	bl			f_dot_round
	bl			pow10_to_f			// exponent in tos
	vmov		s19, tos				// save divisor
	drop
	vdiv.f32	s0, s16, s19
	vcvt.s32.f32 s0, s0 			// digit
	vcvt.f32.s32 s20, s0
	pushdatos
	vmov		tos, s0
	add			tos, tos, #'0'		// '0'+digit
	bl			emit

	ldr			r0, =Fprecision
	ldr			r0, [r0]
2:
	sub			r9, #1
	cmp			r9, #0
	bne			3f
	pushdatos
	mov			tos, #'.'			// '.'
	bl			emit
3:
	vmul.f32	s0, s19, s20		// remove last digit
	vsub.f32	s16, s16, s0
	vmov		s1, #10.0
	vmul.f32	s16, s1
	vdiv.f32	s0, s16, s19
	vcvt.s32.f32 s0, s0 			// digit
	vcvt.f32.s32 s20, s0
	pushdatos
	vmov		tos, s0
	add			tos, tos, #'0'		// '0'+digit
	bl			emit

	sub			r0, #1				// all digits?
	cmp			r0, #0
	bne			2b

	b			f_dot_exponent

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fm."
fm_type:
        @ ( r --  )     display, with a trailing space, the floating-point number r in metric notation
@ -----------------------------------------------------------------------------
	push		{r0-r3, r8, r9, lr}
	mov			r8, #2				// metric unit exponent
	b			fe_m_type

f_dot_special_values:
	push		{lr}
	ldr			r2, =#0x7fc00000	// NaN
	cmp			tos, r2
	bne			1f
	write		"NaN "
	pop			{r0}				// remove return
	b			f_dot_exit
1:
	cmp			tos, #0x7f800000	// +infinity
	bne			1f
	write		"+infinity "
	pop			{r0}				// remove return
	b			f_dot_exit
1:
	cmp			tos, #0xff800000	// -infinity
	bne			1f
	write		"-infinity "
	pop			{r0}				// remove return
	b			f_dot_exit
1:									// zero?
	cmp			tos, #0
	bne			3f
									// special case zero
	write		"0."
	ldr			r0, =Fprecision
	ldr			r0, [r0]
1:
	pushdatos
	mov			tos, #'0'
	bl			emit
	sub			r0, #1				// all digits?
	cmp			r0, #0
	bne			1b
	cmp			r8, #2
	bne			1f
	write		"e "				// metric unit exponent
	b			2f
1:
	write		"E0 "
2:
	pop			{r0}				// remove return
	b			f_dot_exit
3:									// minus sign?
	vmov		s16, tos			// |r| -> s16
	vabs.f32 	s16, s16
	tst			tos, #0x80000000
	beq			3f					// positive number
	pushdatos
	mov			tos, #'-'			// '-'
	bl			emit
3:
	pop			{pc}

f_dot_round:
	push		{lr}
	ldr			r1, =Fprecision		// |r| = |r| + 0.5 * 10^(-precision+exp-1) for rounding
	ldr			r1, [r1]
	negs		r1, r1
	vcvt.s32.f32 s0, s18
	vmov		r0, s0
	add			r1, r1, r0
	vmov		s1, r1
	vcvt.f32.s32 s1, s1
	vmov		s0, #10.0
	vmov		r0, s0
	vmov		r1, s1
	bl			powf
	vmov		s0, r0
	vmov		s1, #0.5
	vmul.f32	s0, s0, s1
	vadd.f32	s16, s16, s0
	pop			{pc}

f_dot_exponent:
	cmp			r8, #2
	bne			1f
									// metric unit exponent
	vcvt.s32.f32 s0, s17 			// exponent decimal
	vmov		r0, s0
	cmp			r0, #0
	bne			1f
	mov			r0, #'e'
	b			2f
1:
	cmp			r0, #3
	bne			1f
	mov			r0, #'k'
	b			2f
1:
	cmp			r0, #6
	bne			1f
	mov			r0, #'M'
	b			2f
1:
	cmp			r0, #9
	bne			1f
	mov			r0, #'G'
	b			2f
1:
	cmp			r0, #12
	bne			1f
	mov			r0, #'T'
	b			2f
1:
	cmp			r0, #15
	bne			1f
	mov			r0, #'P'
	b			2f
1:
	cmp			r0, #-3
	bne			1f
	mov			r0, #'m'
	b			2f
1:
	cmp			r0, #-6
	bne			1f
	mov			r0, #'u'
	b			2f
1:
	cmp			r0, #-9
	bne			1f
	mov			r0, #'n'
	b			2f
1:
	cmp			r0, #-12
	bne			1f
	mov			r0, #'p'
	b			2f
1:
	cmp			r0, #-15
	bne			1f
	mov			r0, #'f'
	b			2f
1:
	b			1f					// no metric unit found
2:
	pushdatos
	mov			tos, r0
	bl			emit
	pushdatos
	mov			tos, ' '			// trailling space
	bl			emit
	b			f_dot_exit

1:
	pushdatos
	mov			tos, #'E'			// 'E'
	bl			emit
	pushdatos
	vcvt.s32.f32 s0, s17 			// exponent decimal
	vmov		tos, s0
	bl			dot					// type exponent
f_dot_exit:
	drop
 	pop			{r0-r3, r8, r9, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "precision"
precision:
        @ ( -- u )      return the number of significant digits currently used by F., FE., or FS. as u
@ -----------------------------------------------------------------------------

	pushdatos
	ldr			tos, =Fprecision
	ldr			tos, [tos]
	bx 			lr

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "set-precision"
set_precision:
        @ ( -- u )      ( u -- )      set the number of significant digits currently used by F., FE., or FS. to u
@ -----------------------------------------------------------------------------
	cmp			tos, #0
	blt			1f
	cmp			tos, #8
	bgt			1f
	ldr			r0, =Fprecision
	str			tos, [r0]
1:
	drop
	bx 			lr


// Words Using C Math Library
// **************************

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, ">float"
to_float:
        @ (a # -- r f ) convert the numbered string to float r, on success flag is true
@ -----------------------------------------------------------------------------
	push		{r0-r3, lr}
	mov 		r1, tos		// #
	drop
	mov			r0, tos		// a
	bl			FPU_str2f
	mov			tos, r0		// float
	pushdatos
	mov			tos, #-1
	ldr			r2, =0x7fc00000  // NaN
	cmp			r0, r2
	bne			1f			// success
	mov			tos, #0		// fail
1:
	pop			{r0-r3, pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fnumber"
fnumber:
        @ (a # -- r u ) convert the numbered string to float r, on success u is 1 or 2, fail 0
        //				single precision u = 1, double precision u = 2
@ -----------------------------------------------------------------------------
	push		{lr}
	bl			to_float
	cmp			tos, #0
	beq			1f			// fail
	mov			tos, #1		// success
1:
	pop			{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fsin"
fsin:
        @ ( r1 -- r2 )       r2 is the sine of the radian angle r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		sinf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fcos"
fcos:
        @ ( r1 -- r2 )       r2 is the cosine of the radian angle r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		cosf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "ftan"
ftan:
        @ ( r1 -- r2 )       r2 is the principal radian angle whose tangent is r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		tanf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fasin"
fasin:
        @ ( r1 -- r2 )       r2 is the principal radian angle whose sine is r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		fasin
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "facos"
facos:
        @ ( r1 -- r2 )       r2 is the principal radian angle whose cosine is r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		acosf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fatan"
fatan:
        @ ( r1 -- r2 )       r2 is the principal radian angle whose tangent is r1.
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		atanf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fsinh"
fsinh:
        @ ( r1 -- r2 )       r2 is the hyperbolic sine of r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		sinhf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fcosh"
fcosh:
        @ ( r1 -- r2 )       r2 is the hyperbolic cosine of r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		coshf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "ftanh"
ftanh:
        @ ( r1 -- r2 )       r2 is the hyperbolic tangent of r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		tanhf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fasinh"
fasinh:
        @ ( r1 -- r2 )       r2 is the floating-point value whose hyperbolic sine is r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		asinhf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "facosh"
facosh:
        @ ( r1 -- r2 )       r2 is the floating-point value whose hyperbolic cosine is r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		acoshf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fatanh"
fatanh:
        @ ( r1 -- r2 )       r2 is the floating-point value whose hyperbolic tangent is r1.
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		atanhf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fceil"
fceil:
        @ ( r1 -- r2 )       return the smallest integral value that is not less than r1
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		ceilf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "ffloor"
ffloor:
        @ ( r1 -- r2 )       Round r1 to an integral value using the "round toward negative infinity" rule, giving r2
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		floorf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fexp"
fexp:
        @ ( r1 -- r2 )       raise e to the power r1, giving r2.
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		expf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "fln"
fln:
        @ ( r1 -- r2 )       r2 is the natural logarithm of r1. An ambiguous condition exists if r1 is less than or equal to zero
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		logf
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_1, "flog"
flog:
        @ ( r1 -- r2 )       r2 is the base-ten logarithm of r1. An ambiguous condition exists if r1 is less than or equal to zero
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r0, tos
	bl		log10f
	mov 	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible|Flag_foldable_2, "f**"
fpow:
        @ ( r1 r2 -- r3 )       raise r1 to the power r2, giving the product r3
@ -----------------------------------------------------------------------------
	push	{lr}
	mov 	r1, tos	// r2
	drop
	mov		r0, tos // r1
	bl		powf
	mov 	tos, r0 // r3
	pop		{pc}

.ltorg @ Hier werden viele spezielle Hardwarestellenkonstanten gebraucht, schreibe sie gleich !

.endif // FPU == 1

