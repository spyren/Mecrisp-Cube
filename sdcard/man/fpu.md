Floating-Point Unit FPU
=======================

Why Floating-Point?
-------------------

[STM AN4044](https://www.st.com/resource/en/application_note/an4044-floating-point-unit-demonstration-on-stm32-microcontrollers-stmicroelectronics.pdf):
One alternative to floating-point is [fixed-point](https://mecrisp-stellaris-folkdoc.sourceforge.io/fixed-point.html), where the exponent field is fixed. But if
fixed-point is giving better calculation speed on FPU-less processors, the range of numbers
and their dynamic is low. As a consequence, a developer using the fixed-point technique will
have to check carefully any scaling/saturation issues in the algorithm. 

| *Coding*         | *Dynamic [dB]* |
|------------------|----------------|
| Int32            | 192            |
| Int64            | 385            |
| Single precision | 1529           |
| Double precision | 12318          |

   * [fpu.s](https://github.com/spyren/Mecrisp-Cube/blob/master/Forth/cube/fpu.s) on GitHub
   * [fpu.c](https://github.com/spyren/Mecrisp-Cube/blob/master/peripherals/fpu.c) on GitHub


Floating-Point Words
--------------------

No separate floating-point stack. A single precision floating-point number is one cell. The 32-bit base-2 format is officially referred to as binary32 [IEEE 754-2008](https://en.wikipedia.org/wiki/IEEE_754-2008_revision).


Bare FPU Words (Without C Math Library)

    f+      ( r1 r2 -- r3 )     Add r1 to r2 giving the sum r3
    f-      ( r1 r2 -- r3 )     Subtract r2 from r1, giving r3
    f*      ( r1 r2 -- r3 )     Multiply r1 by r2 giving r3
    f/      ( r1 r2 -- r3 )     Divide r1 by r2, giving the quotient r3
    fsqrt   ( r1 -- r2 )        r2 is the square root of r1

    fabs    ( r1 -- r2 )        r2 is the absolute value of r1
    fnegate ( r1 -- r2 )        r2 is the negation of r1
    fround  ( r1 -- r2 )        round r1 to an integral value using the "round to nearest" rule, giving r2

    10**>f  ( n -- r )          raise 10 to the power n, giving product r
    flog>n  ( r -- n )          n is the base-ten logarithm of r

    fflags@ ( -- u )            get the current value of the Floating Point Status/Control register FPSCR
    fflags! ( u -- )            assign the given value to the Floating Point Status/Control register FPSCR

    f0=     ( r -- ? )          flag is true if r is equal to zero
    f0<     ( r -- ? )          flag is true if r is less than zero
    f<      ( r1 r2 -- ? )      flag is true if r1 is less than r2
    f~      ( r1 r2 r3 -- ? )   If r3 is positive, flag is true if the absolute value of (r1 minus r2) is less than r3
                                If r3 is zero, flag is true if the implementation-dependent encoding of r1 and r2 are exactly identical 
                                (positive and negative zero are unequal if they have distinct encodings).
                                If r3 is negative, flag is true if the absolute value of (r1 minus r2) is less than the absolute value 
                                of r3 times the sum of the absolute values of r1 and r2. 

    f>s     ( r -- n )          n is the single-cell signed-integer equivalent of the integer portion of r
    s>f     ( n -- r )          r is the floating-point equivalent of the single-cell value n
    f>x     ( r -- x )          x is the fixed-point equivalent of the floating-point r
    x>f     ( x -- r )          r is the floating-point equivalent of the fixed-point x

    pi      (  -- r )           r is pi, approx. 3.14159274101257324
    e       (  -- r )           r is e, approx. 2.7182818

    fnumber (a # -- r u )       convert the specified string by a and # to float r, on success u is 1, otherwise 0
    >float  (a # -- r ? )       convert the specified string by a and # to float r, on success flag is true

    f.      ( r --  )           display, with a trailing space, the floating-point number r in fixed-point notation
    fs.     ( r --  )           display, with a trailing space, the floating-point number r in scientific notation
    fe.     ( r --  )           display, with a trailing space, the floating-point number r in engineering notation
    fm.     ( r --  )           display, with a trailing space, the floating-point number r in metric unit prefix notation
    precision     ( -- u )      return the number of significant digits currently used by f., fs., fe., or fm. as u
    set-precision ( u -- )      set the number of significant digits currently used by f., fs., fe., or fm. to u



Words Using the C Math Library
------------------------------

[C mathematical functions](https://en.wikipedia.org/wiki/C_mathematical_functions) @ Wikipedia

    fsin    ( r1 -- r2 )       r2 is the sine of the radian angle r1
    fcos    ( r1 -- r2 )       r2 is the cosine of the radian angle r1
    ftan    ( r1 -- r2 )       r2 is the principal radian angle whose tangent is r1
    fasin   ( r1 -- r2 )       r2 is the principal radian angle whose sine is r1
    facos   ( r1 -- r2 )       r2 is the principal radian angle whose cosine is r1
    fatan   ( r1 -- r2 )       r2 is the principal radian angle whose tangent is r1

    fsinh   ( r1 -- r2 )       r2 is the hyperbolic sine of r1
    fcosh   ( r1 -- r2 )       r2 is the hyperbolic cosine of r1
    ftanh   ( r1 -- r2 )       r2 is the hyperbolic tangent of r1
    fasinh  ( r1 -- r2 )       r2 is the floating-point value whose hyperbolic sine is r1
    facosh  ( r1 -- r2 )       r2 is the floating-point value whose hyperbolic cosine is r1
    fatanh  ( r1 -- r2 )       r2 is the floating-point value whose hyperbolic tangent is r1

    fceil   ( r1 -- r2 )       return the smallest integral value that is not less than r1
    ffloor  ( r1 -- r2 )       Round r1 to an integral value using the "round toward negative infinity" rule, giving r2

    fexp    ( r1 -- r2 )       raise e to the power r1, giving r2.
    f**     ( r1 r2 -- r3 )    raise r1 to the power r2, giving the product r3

    fln     ( r1 -- r2 )       r2 is the natural logarithm of r1
    flog    ( r1 -- r2 )       r2 is the base-ten logarithm of r1


How to Use
----------

Calculation of two [parallel resistors](https://en.wikipedia.org/wiki/Resistor#Series_and_parallel_resistors):

    : f|| ( r1 r2 -- r3) 
      2dup f* -rot f+ f/ 
    ;

    27k 100k f|| fm.  21.3k  ok.


[RC time constant](https://en.wikipedia.org/wiki/RC_time_constant)

    2.2n 47k f* fm. 103u  ok.


Mecrisp-Cube has the word =f.= defined as an assembler routine in [fpu.s](https://github.com/spyren/Mecrisp-Cube/blob/master/Forth/cube/fpu.s), but the example here is written in Forth. I use a dot for the [decimal separator](https://en.wikipedia.org/wiki/Decimal_separator). 
[Terry Porter](https://mecrisp-stellaris-folkdoc.sourceforge.io/fixed-point.html) "because those crazy Europeans use a comma instead of a decimal point". Not all europeans are crazy, at least the Swiss are an exception ;-), they use sometimes decimal points.

    : f. ( r -- )  \ display, with a trailing space, the floating-point number r in fixed-point notation
      dup  f0< if
        45 emit 
        fabs
      then
      dup
      $3F000000 \ .5
      precision 0 do
        $41200000 f/ \ 10.0 / 
      loop
      f+            \ round
      f>x
      <# 
        0 #s 2drop    \ integer part
        46 hold<       \ decimal point
        precision 0 do
          x#             \ fract digit
        loop
        dup
      #> 
      type space
    ; 


\-- [PeterSchmid - 2022-11-22]
