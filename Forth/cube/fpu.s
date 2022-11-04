/**
 *  @brief
 *      Interface to the FPU (Flosating Poibnt Unit)
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



@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fflags@"
get_fflags:
        @ ( -- u ) get the current value of the Floating Point Status/Control register FPSCR
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl FPU_getFPSCR
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fflags!"
set_fflags:
        @ ( u --  ) assign the given value to the Floating Point Status/Control register FPSCR
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	bl FPU_setFPSCR
	drop
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f+"
f_add:
        @ ( r1 r2 -- r3 ) Add r1 to r2 giving the sum r3.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vadd.f32 s0, s1
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f-"
f_sub:
        @ ( r1 r2 -- r3 ) Subtract r2 from r1, giving r3.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vsub.f32 s0, s1
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f*"
f_star:
        @ ( r1 r2 -- r3 ) Multiply r1 by r2 giving r3.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vmul.f32 s0, s1
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f/"
f_slash:
        @ ( r1 r2 -- r3 ) Divide r1 by r2, giving the quotient r3.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s1, tos
	drop
	vmov 	s0, tos
	vdiv.f32 s0, s0, s1
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fsqrt"
fsqrt:
        @ ( r1 -- r2 ) r2 is the square root of r1.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s0, tos
	vsqrt.f32 s0, s0
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fabs"
fabs:
        @ ( r1 -- r2 ) r2 is the absolute value of r1.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s0, tos
	vabs.f32 s0, s0
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fnegate"
fnegate:
        @ ( r1 -- r2 ) r2 is the negation of r1.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s0, tos
	vneg.f32 s0, s0
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fround"
fround:
        @ ( r1 -- r2 ) round r1 to an integral value using the "round to nearest" rule, giving r2
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s0, tos
	vcvtr.s32.f32 s0, s0
	vcvt.f32.s32 s0, s0
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f>s"
f_to_s:
        @ ( r -- n ) n is the single-cell signed-integer equivalent of the integer portion of r.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s0, tos
	vcvt.s32.f32 s0, s0
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "s>f"
s_to_f:
        @ ( n -- r ) r is the floating-point equivalent of the single-cell value n.
@ -----------------------------------------------------------------------------
	push	{lr}
	vmov 	s0, tos
	vcvt.f32.s32 s0, s0
	vmov 	tos, s0
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "pi"
pi:
        @ (  -- r ) r is pi approx. 3.14159274101257324
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	mov 	tos, #0x0fdb
	movt 	tos, #0x4049
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "e"
euler:
        @ (  -- r ) r is e approx. 2.718281828459045235360287
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	mov 	tos, #0xf854
	movt 	tos, #0x402d
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fnumber"
fnumber:
        @ (a # -- r u ) convert the numbered string to float r, on success u is 1, fail 0
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	mov 	r1, tos		// #
	drop
	mov		r0, tos		// a
	bl		FPU_str2f
	mov		tos, r0		// float
	pushdatos
	mov		tos, #1
	cmp		r0, #0
	bne		1f			// success
	mov		tos, #0		// fail
1:
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
//   s31.32 <-> Float conversions by Ivan Dimitrov
// -----------------------------------------------------------------------------

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f>fx"
f_to_fx:
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
	push	{lr}
	movs	r1, #0
							// dup 1 31 lshift and
    movs	r0, #0x80000000
    and		r1, r0, tos		// tos v, r1 S
	vmov 	s0, tos			// s0 v
	vabs.f32 s0, s0			// s0 v
	vcvt.s32.f32 s1, s0		// v>n s1 h
	vmov	r0, s1			// r0 h
	vcvt.f32.s32 s3, s1		// n>v s3 h'
	movs	r2, #0x4f800000
	vmov	s2, r2			// s2 0x4f800000
	vsub.f32 s0, s0, s3
	vmul.f32 s0, s0, s2		// s0 l
	vcvt.u32.f32 s0, s0
	vmov	tos, s0			// tos l
	cmp		r1, #0
  	beq 	2f 				// Zero? Nothing to do!
  	movs 	r2, #0			// dnegate
	mvns 	tos, tos
	mvns 	r0, r0
 	adds 	tos, #1
	adcs 	r0, r2
2:
	pushdatos
	movs	tos, r0
 	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "fx>f"
fx_to_f:
        @ ( d -- r ) r is the floating-point equivalent of the fixed-point d.
	// : f>v ( df -- v ) L H -- v
    //   dup 1 31 lshift and -rot ( S L H )
    //   dabs u>v  swap u>v       ( S vh vl )
    //   $2f800000 vmul vadd      ( S v )
    //   or                       \ apply the sign
    // ; 2-foldable
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r1, tos			// r1  H
	drop					// tos L
    movs	r0, #0x80000000
    and		r0, r0, r1		// r0 S
    						// dabs d
  	cmp 	r0, #0   		// check sign in H
  	bpl 	1f 				// positive? Nothing to do!
  	movs 	r2, #0			// dnegate r1 tos
	mvns 	tos, tos
	mvns 	r1, r1
 	adds 	tos, #1
	adcs 	r1, r2
1:
	vmov 	s1, r1			// H u>v, vh
	vcvt.f32.s32 s1, s1
	vmov 	s0, tos			// L u>v, vl
	vcvt.f32.s32 s0, s0
    movs	r3, #0x2f800000
    vmov	s2, r3
	vmul.f32 s0, s2
	vadd.f32 s0, s1

	vmov 	tos, s0
  	orr		tos, r0			// set sign
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "f."
f_dot:
        @ ( r --  ) Display, with a trailing space, the top number using fixed-point notation
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		f_to_fx
	pushdatos
	mov		tos, #4
	bl		fdotn
	pop		{pc}




.ltorg @ Hier werden viele spezielle Hardwarestellenkonstanten gebraucht, schreibe sie gleich !
