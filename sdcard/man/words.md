Mecrisp-Stellaris Forth Words
=============================

## Stack-Effect Comments

From [GForth Manual](https://gforth.org/manual/Stack_002dEffect-Comments-Tutorial.html)

By convention the comment after the name of a definition describes the stack effect: 
The part in front of the ‘--’ describes the state of the stack before the execution 
of the definition, i.e., the parameters that are passed into the colon definition; 
the part behind the ‘--’ is the state of the stack after the execution of the definition, 
i.e., the results of the definition. The stack comment only shows the top stack 
items that the definition accesses and/or changes.

You should put a correct stack effect on every definition, even if it is just ( -- ). 
You should also add some descriptive comment to more complicated words 
(I usually do this in the lines following :). 
If you don’t do this, your code becomes unreadable (because you have to work 
through every definition before you can understand any).

>  Assignment: The stack effect of swap can be written like this:
>  x1 x2 -- x2 x1. Describe the stack effect of -, drop, dup, over, rot, nip, and tuck.
>  Hint: When you are done, you can compare your stack effects to those in this
>  manual (see Word Index). 

Sometimes programmers put comments at various places in colon definitions 
that describe the contents of the stack at that place (stack comments); i.e., 
they are like the first part of a stack-effect comment. E.g.,

```forth
: cubed ( n -- n^3 )
   dup squared  ( n n^2 ) * ;
```

In this case the stack comment is pretty superfluous, because the word is 
simple enough. If you think it would be a good idea to add such a comment 
to increase readability, you should also consider factoring the word into 
several simpler words (see Factoring), which typically eliminates the need 
for the stack comment; however, if you decide not to refactor it, 
then having such a comment is better than not having it.

The names of the stack items in stack-effect and stack comments in the 
standard, in this manual, and in many programs specify the type through a 
type prefix, similar to Fortran and Hungarian notation. The most frequent 
prefixes are:

```
n          signed integer 
u          unsigned integer 
c          character 
f          Boolean flags, i.e. false or true. 
a-addr, a- Cell-aligned address 
c-addr, c- Char-aligned address (note that a Char may have two bytes in Windows NT)
f-addr, f- Float-aligned address 
xt         Execution token, same size as Cell 
w,x        Cell, can contain an integer or an address. It takes 32 bits
           A cell is more commonly known as machine word, but the term word already means something different in Forth. 
d          signed double-cell integer 
ud         unsigned double-cell integer 
r          Float (single precision
"          string in the input stream (not on the stack). The terminating character is a blank by default. If it is not a blank, it is shown in <> quotes. 
```

You can find a more complete list in [Notation](https://gforth.org/manual/Notation.html). 


Terminal-IO
-----------

Exactly ANS, some logical extension. See also [TerminalIO](TerminalIO.md)
```
Words           Stack Comment   Description

emit?           ( -- f )        Ready to send a character ?
key?            ( -- f )        Checks if a key is waiting
key             ( -- c )        Waits for and fetches the pressed key
emit            ( c -- )        Emits a character
hook-emit?      ( -- a- )       Hooks for redirecting terminal IO on the fly
hook-key?       ( -- a- )
hook-key        ( -- a- )
hook-emit       ( -- a- )
serial-emit?    ( -- f )        Serial interface terminal routines as default communications
serial-key?     ( -- f ) 
serial-key      ( -- c )        Waits for and fetches the pressed key
serial-emit     ( c -- )        Emits a character
hook-pause      ( -- a- )       Hook for a multitasker
pause           ( -- ) 	    Task switch, none for default
```

### Character Manipulation

How to convert numbers to characters and convert characters to numbers.
```
char            ( "name" -- c ) Skip leading space delimiters. Parse name delimited by a space. Put the value of its first character onto the stack 
[char]          ( "name" -- c ) like char but also in compile mode  
emit            ( c -- )        Emits a character
```

Stack Jugglers
--------------

### Single-Jugglers

Exactly ANS, some logical extension.
```
depth           ( -- +n )               Gives number of single-cell stack items. 	 
nip             ( x1 x2 -- x2 )         Drop the first item below the top of the stack 	 
drop            ( x -- ) 	  	 
rot             ( x1 x2 x3 -- x2 x3 x1 ) 	  	 
-rot            ( x1 x2 x3 -- x3 x1 x2 ) 	  	 
swap            ( x1 x2 -- x2 x1 ) 	  	 
tuck            ( x1 x2 -- x2 x1 x2 ) 	  	 
over            ( x1 x2 -- x1 x2 x1 ) 	  	 
?dup            ( x -- 0 | x x ) 	  	 
dup             ( x -- x x ) 	  	 
>r              ( x -- ) (R: -- x )     Push to Return Stack (the Return Stack must be balanced) 	 
r>              ( -- x ) (R: x -- )     Pull from Return Stack 	 
r@              ( -- x ) (R: x -- x )   Copy from Return Stack 	 
rdrop           ( -- ) (R: x -- ) 	  	 
rdepth          ( -- +n )               Gives number of return stack items. 

roll            ( xu xu-1 ... x0 u -- xu-1 ... x0 xu )            Remove u. Rotate u+1 items on the top of the stack.
pick            ( ... xi+1 xi ... x1 x0 i -- ... x1 x0 xi )       Copy a element X levels down the stack to the top of the stack 	 
rpick           ( i -- xi ) R: ( ... xi ... x0 -- ... xi ... x0 ) 	  	 
-roll           ( xu-1 ... x0 xu u – xu xu-1 ... x0 u ) 
```

### Double-Jugglers

They perform the same for double numbers
```
2nip            ( x1 x2 x3 x4 --x3 x4 )
2drop           ( x1 x2 -- )
2rot            ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )
2-rot           ( x1 x2 x3 x4 x5 x6 -- x5 x6 x1 x2 x3 x4 )
2swap           ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
2tuck           ( x1 x2 x3 x4 -- x3 x4 x1 x2 x3 x4 )
2over           ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 )
2dup            ( x1 x2 -- x1 x2 x1 x2 )
2>r             ( x1 x2 -- ) (R: --x1 x2 )
2r>             ( -- x1 x2 ) (R: x1 x2 -- )
2r@             ( -- x1 x2 ) (R: x1 x2 --x1 x2 )
2rdrop          ( -- ) (R: x1 x2 -- )
```

### Stack pointers

```
sp@             ( S:... -- a- )  Fetch data stack pointer
sp!             ( a- -- S:... )  Store data stack pointer
rp@             ( -- a- )        Fetch return stack pointer
rp!             ( a- -- )        Store return stack pointer
```

Logic
-----

exactly ANS, some logical extensions.
```
arshift         ( x1 u -- x2 )  Arithmetric right-shift of u bit-places
rshift          ( x1 u -- x2 ) 	Logical right-shift of u bit-places
lshift          ( x1 u -- x2 ) 	Logical left-shift of u bit-places
shr             ( x1 -- x2 ) 	Logical right-shift of one bit-place
shl             ( x1 -- x2 ) 	Logical left-shift of one bit-place
ror             ( x1 -- x2 ) 	Logical right-rotation of one bit-place
rol             ( x1 -- x2 ) 	Logical left-rotation of one bit-place
bic             ( x1 x2 -- x3 ) Bit clear, identical to “not and”
not             ( x1 -- x2 ) 	Invert all bits
xor             ( x1 x2 -- x3 ) Bitwise Exclusive-OR
or              ( x1 x2 -- x3 ) Bitwise OR
and             ( x1 x2 -- x3 ) Bitwise AND
false           ( -- 0 )        False-Flag
true            ( ---1 )        True-Flag
clz             ( x1 -- u )     Count leading zeros
```

Single Number Math
------------------

Exactly ANS, some logical extensions.
```
u/mod           ( u1 u2 -- u3 u4 )  32/32 = 32 rem 32 Division u1 / u2 = u4 remainder u3
/mod            ( n1 n2 -- n3 n4 )  n1 / n2 = n4 rem n3
mod             ( n1 n2 -- n3 )     n1 / n2 = remainder n3
/               ( n1 n2 -- n3 )     n1 / n2 = n3
\*              ( u1 n1 u2 n2 -- u3 n3 ) 	32*32 = 32 Multiplication
min             ( n1 n2 -- n1 n2 )  Keeps smaller of top two items
max             ( n1 n2 -- n1 n2 )  Keeps greater of top two items
umin            ( u1 u2 -- u1 u2 )  Keeps unsigned smaller
umax            ( u1 u2 -- u1 u2 )  Keeps unsigned greater
2-              ( u1 n1 -- u2 n2 )  Subtracts two, optimized
1-              ( u1 n1 -- u2 n2 )  Subtracts one, optimized
2+              ( u1 n1 -- u2 n2 )  Adds two, optimized
1+              ( u1 n1 -- u2 n2 )  Adds one, optimized
even            ( u1 n1 -- u2 n2 )  Makes even. Adds one if uneven.
2*              ( n1 -- n2 )        Arithmetric left-shift
2/              ( n1 -- n2 )        Arithmetric right-shift
abs             ( n -- u )          Absolute value
negate          ( n1 -- n2 )        Negate
-               ( u1|n1 u2|n2 -- u3|n3 )    Subtraction
+               ( u1|n1 u2|n2 -- u3|n3 )    Addition
```

Double Number Math
------------------

exactly ANS, some logical extensions.
```
um*             ( u1 u2 -- ud )                 32*32 = 64 Multiplication
ud*             ( ud1 ud2 -- ud3 )              64*64 = 64 Multiplication
udm*            ( ud1 ud2 -- ud3-Low ud4-High ) 64*64=128 Multiplication
um/mod          ( ud u1 -- u2 u3 )              ud / u1 = u3 remainder u2
ud/mod          ( ud1 ud2 -- ud3 ud4 )          64/64 = 64 rem 64 Division ud1 / ud2 = ud4 remainder ud3
m*              ( n1 n2 -- d ) 	        n1 * n2 = d
m/mod           ( d n1 -- n2 n3 )       d / n1 = n3 remainder r2
d/mod           ( d1 d2 -- d3 d4 )      d1 / d2 = d4 remainder d3
d/              ( d1 d2 -- d3 )         d1 / d2 = d3
*/              ( n1 n2 n3 -- n4 )      n1 * n2 / n3 = n4
u*/             ( u1 u2 u3 -- u4 )      u1 * u2 / u3 = u4
*/mod           ( n1 n2 n3 -- n4 n5 )   n1 * n2 / n3 = n5 remainder n4
u*/mod          ( u1 u2 u3 -- u4 u5 )   u1 * u2 / u3 = u5 remainder u4
d2*             ( d1 -- d2 ) 	        Arithmetric left-shift
d2/             ( d1 -- d2 ) 	        Arithmetric right-shift
dshl            ( ud1 -- ud2 ) 	        Logical left-shift, same as d2*
dshr            ( ud1 -- ud2 ) 	        Logical right-shift
dabs            ( d -- ud ) 	        Absolute value
dnegate         ( d1 -- d2 )            Negate
d-              ( ud1|d1 ud2|d2 -- ud3|d3 )     Subtraction
```

Decision Making
---------------

### Single-Comparisons

Exactly ANS, some logical extensions.

#### Unsigned Comparisons

```
u<=             ( u1 u2 -- f )   u1 less-or-equal u2
u>=             ( u1 u2 -- f )   u1 greater-or-equal u2
u>              ( u1 u2 -- f )   u1 greater-than u2
u<              ( u1 u2 -- f )   u1 less-than u2
```

#### Signed Comparisons

Exactly ANS, some logical extensions.
```
<=              ( n1 n2 -- f )   n1 less-or-equal n2
>=              ( n1 n2 -- f )   n1 greater-or-equal n2
>               ( n1 n2 -- f )   n1 greater-than n2
<               ( n1 n2 -- f )   n1 less-than n2
0<              ( n - f )        n less than zero
0<>             ( n -- f )       n not equal to zero
0=              ( n -- f )       n equals zero
<>              ( n1 n2 -- f )   n1 not-equals n2
=               ( n1 n2 -- f )   n1 equals n2
```

### Double-Comparisons

They perform the same for double numbers.
```
du>             ( ud1 ud2 -- f )
du<             ( ud1 ud2 -- f )
d>              ( d1 d2 -- f )
d<              ( d1 d2 -- f )
d0<             ( d -- f )
d0=             ( d -- f )
d<>             ( d1 d2 -- f )
d=              ( d1 d2 -- f )
```

### Bits: Test, Toggle, Clear and Set

Note: TRUE = 1, FALSE = 0. If a Bit is HIGH it leaves TRUE on the stack, otherwise FALSE.
```
cbit@           ( u c- -- f )    Test bit (u mask) in byte-location
hbit@           ( u h- -- f )    Test bit in halfword-location
bit@            ( u a- -- f )    Test bit in word-location
cxor!           ( u c- -- )      Toggle bit in byte-location
hxor!           ( u h- -- )      Toggle bit in halfword-location
xor!            ( u a- -- )      Toggle bit in word-location
cbic!           ( u c- -- )      Clear bit in byte-location
hbic!           ( u h- -- )      Clear bit in halfword-location
bic!            ( u a- -- )      Clear bit in word-location
cbis!           ( u c- -- )      Set bit in byte-location
hbis!           ( u h- -- )      Set bit in halfword-location
bis!            ( u a- -- )      Set bit in word-location
```

Control Structures
------------------

Internally, they have complicated compile-time stack effects.

### Summary

```
do ... loop                 Finite loop incrementing by 1
do ... +loop                Finite loop incrementing by X
begin ... until             Indefinite loop terminating when is ‘true’
begin ... while ... repeat  Indefinite loop terminating when is ‘false’
begin ... again             Infinite loop
if ... else ... then        Two-branch conditional; performs words following IF it is ‘true’ and words following ELSE if it is ‘false’. THEN marks the point at which the paths merge.
if ... then                 Like the two-branch conditional, but with only a ‘true’ clause.
case of ... endof endcase   (switch) case function
```

### Decisions

Exactly ANS.
```
if              ( f -- )          structure.
else            ( -- )            flag if ... [else ...] then
then            ( -- )            This is the common flag if ... [else ...] then structure.

case            ( -- )            Begins case structure
of              ( x1 x2 -- | x1 ) If the two values on the stack are not equal, discard the top value and continue execution
                                  at the location specified by the consumer of of-sys, e.g., following the next ENDOF.
                                  Otherwise, discard both values and continue execution in line. 
?of             ( f -- )          Flag-of, for custom comparisions
endof           ( -- )            End of one possibility
endcase         ( x -- )          Ends case structure, discards x
```

"IF that’s the case, do this, ELSE do that ... and THEN continue with ..." 
```forth
f if 
  ... 
then

f if 
  ...
else 
  ... 
then
```

```forth
n case
    m1    of  ... endof
    m2    of  ... endof
    flag  ?of ... endof
    all others
endcase
```

### Definite Loops

A loop structure in which the words contained within the loop repeat a definite number of times. 
In Forth, this number depends on the starting and ending counts (index and limit) which are 
placed on the stack prior to the execution of the word DO.

Exactly ANS.

```
i               ( -- u|n )      Gives innermost loop index
j               ( -- u|n )      Gives second loop index
k               ( -- u|n )      Gives third loop index
exit            ( -- )          Returns from current definition. Compiles a ret opcode.

do              ( n1 n2 -- )    limit n1, index n2, return stack (R: -- old-limit old-index ) 
loop            ( -- )          Increments current loop index register by one and checks whether to continue or not, (R: unchanged | old-limit old-index -- ) 
+loop           ( u|n -- )      If you want the index to go up by some number other than one each time around, (R: unchanged | old-limit old-index -- )
unloop          ( -- )          Drops innermost loop structure, pops back old loop structures to loop registers, (R: old-limit old-index -- )
leave           ( -- )          Leaves current innermost loop promptly, (R: old-limit old-index -- )   
?do             ( n1 n2 -- )    Begins a loop if limit n1 and index n2 are not equal, (R: unchanged | -- old-limit old-index ) 
```

### Indefinite Loops

A loop structure in which the words contained within the loop continue to repeat until some truth condition changes state.
```
repeat          ( -- )          Finish of a middle-flag-checking loop.
while           ( f -- )        Check a flag in the middle of a loop
until           ( f -- )        begin ... flag until loops as long flag is true
again           ( -- )          begin ... again is an endless loop
begin           ( -- ) 	 
```

Memory
------

### Memory Status


```
unused          ( -- u )        Displays memory depending on compile mode (Ram or Flash)
```

### Memory Access

Subtle differences to ANS, special cpu-specific extensions. Fetch-Modify-Store operations like +! or bic! are NOT ATOMIC.
```
move            ( c-1 c-2 u -- )         Moves u Bytes in Memory
fill            ( c- u c ) 	         Fill u Bytes of Memory with value c

constant        ( u|n "name"  -- )       Makes a single constant. i.e. “$1024 constant one-kb”
  name          ( -- u|n )
variable        ( u|n "name"  -- )       Makes an initialized single variable. i.e. “0 variable one-kb”
  name          ( -- a- )
nvariable       ( n1*u|n n1 "name"  -- ) Makes an initialized variable with specified size of n1 words Maximum is 15 words
buffer:         ( u "name" -- )          Creates a buffer in RAM u bytes in length
@               ( a- -- u|n )            Fetches single number from memory
!               ( u|n a- -- )            Stores single number in memory
+!              ( u|n a- -- )            Plus Store, use to increment a variable, register etc

2constant       ( ud|d "name" -- )       Makes a double constant.
2variable       ( ud|d "name" -- )       Makes an initialized double variable
2@              ( a- -- ud|d )           Fetches double number from memory
2!              ( ud|d a- -- )           Stores double number in memory

h@              ( c- -- c )              Fetches halfword from memory
h!              ( c c- -- )              Stores halfword in memory
h+!             ( u|n a- -- )            Add to halfword memory location

c@              ( c- -- c )              Fetches byte from memory
c!              ( c c- )                 Stores byte in memory
c+!             ( u|n a- -- )            Add to byte memory location
```

ANS (see [ansification.fs](/sdcard/fsr/ansification.fs)). Default is Mecrisp style.
```
variable        ( "name" -- )           Makes an uninitialized single variable
2variable       ( "name" -- )           Makes an uninitialized double variable
```

String Formatting
-----------------

### String Routines

Exactly ANS, some logical extensions.
```
type            ( c- u -- )           Prints a string.
s"              ( "ccc<quote>" -- c- u )  Compiles a string ccc and gives back its address c- and length u when executed
."              ( "ccc<quote>" -- )   Compiles a string ccc and prints it when executed.
.(              ( "ccc<paren>" -- )   Parse and display ccc delimited by )
(               ( "ccc<paren>" -- )   Ignore comment ccc 
\               ( "ccc<eol>" -- )     Comment to end of line 	 
cr              ( -- )                Emits line feed 	 
bl              ( -- c )              ASCII (32) for space 	 
space           ( -- )                Emits space 	 
spaces          ( n -- )              Emits n spaces if n is positive
compare         ( c-1 u-1 c-2 u-2 -- f )    Compares two strings
accept          ( c- u1 -- u2 )       Read input into a string.
```

### Counted String Routines

Exactly ANS, some logical extensions.
```
ctype           ( c- -- )             Prints a counted string.
c"              ( "ccc<quote>" -- c- ) Compiles a counted string ccc and gives back its address c- when executed.
cexpect         ( c- n -- )           read input into a counted string c-, n max length.
count           ( c- -- c- u ) 	      Convert counted string into addr-length string
skipstring      ( c- -- a- )          Increases the pointer to the aligned end of the string.
```

### Pictured Numerical Output

#### Pictured Numerical output

```
.digit 	        ( u -- c )                  Converts a digit to a char
digit           ( c -- u f )                Converts a char to a digit
char            ( "name" -- c )             Skip leading space delimiters. Parse name delimited by a space. Put the value of its first character onto the stack 
[char]          ( "name" -- c )             like char but compiles
hold            ( c -- )                    Adds character to pictured number output buffer from the front.
hold<           ( c -- )                    Adds character to pictured number output buffer from behind
sign            ( n -- )                    Add a minus sign to pictured number output buffer, if n is negative
#S              ( ud1|d1 -- 0 0 )           Add all remaining digits from the double length number to output buffer
f#S             ( n-comma1 -- n-comma2 )    Adds 32 comma-digits to number output
#               ( ud1|d1 -- ud2|d2 )        Add one digit from the double length number to output buffer
f#              ( n-comma1 -- n-comma2 )    Adds one comma-digit to number output
#>              ( ud|d -- c- n )            Drops double-length number and finishes pictured numeric output ready for type
<#              ( -- )                      Prepare pictured number output buffer
u.              ( u -- )                    Print unsigned single number
.               ( n -- )                    Print single number
ud.             ( ud -- )                   Print unsigned double number
d.              ( d -- )                    Print double number
```

Deep Insights
-------------

```
words           ( -- )        Prints list of defined words and properties
list            ( -- )        Prints all defined words. From; dissasembler-mx.txt
.s              ( -- )        Prints stack contents, signed
u.s             ( -- )        Prints stack contents, unsigned
h.s             ( -- )        Prints stack contents, unsigned, hex
hex.            ( u -- )      Prints 32 bit unsigned in hex base, needs emit only. This is independent of number subsystem.
```

User Input and Interpretation
-----------------------------

Exactly ANS, some logical extensions.
```
query           ( -- )                     Fetches user input to input buffer
tib             ( -- c- )                  Input buffer
current-source  ( -- a- )                  Double-Variable which contains source
tsource         ( c- n -- )                Change source
source          ( -- c- u )                Current source
>in             ( -- a- )                  Variable with current offset into source
token           ( -- c- u )                Cuts one token out of input buffer
parse           ( c "ccc<char>" -- c- u )  Cuts anything delimited by char out of input buffer
evaluate        ( i*x c- u -- j*x  )       Interpret given string
interpret       ( i*x -- j*x )             Execute, compile, fold, optimize…
quit            ( i*x -- ) (R: j*x -- )    Resets Stacks
hook-quit       ( -- a- )                  Hook for changing the inner quit loop
```

Dictionary Expansion
--------------------

Exactly ANS, some logical extension.
```
align           ( -- )          Aligns dictionary pointer
aligned         ( c- -- a- )    Advances to next aligned address
cell+           ( a-1 -- a-2 )  Add size of one cell
cells           ( n1 -- n2 )    Calculate size of n cells
allot           ( n -- )        tries to advance Dictionary Pointer by n bytes, aborts, if not enough space available
here            ( -- a-|c- )    Gives current position in Dictionary
,               ( u|n -- )      appends a single number to dictionary
><,             ( u|n -- )      reverses high and low-halfword, then appends it to dictionary
h,              ( u|n -- )      appends a halfword to dictionary

compiletoram? 	( -- f )        currently compiling into ram?
compiletoram 	( -- )          makes ram the target for compiling
compiletoflash  ( -- )          makes flash memory the target for compiling
forgetram       ( -- )          Forget definitions in ram without a reset
```

Speciality!
-----------

```
string,         ( c- u -- )     Inserts a string of maximum 255 characters without runtime
literal,        ( u|n -- )      Compiles a literal with runtime
inline,         ( a- -- )       Inlines the choosen subroutine
call,           ( a- -- )       Compiles a call to a subroutine
ret,            ( -- )          compiles a ret opcode
flashvar-here   ( -- a- )       Gives current RAM management pointer
dictionarystart ( -- a- )       Current entry point for dictionary search
dictionarynext  ( a-1 -- a-2 f ) Scans dictionary chain and returns true if end is reached.
	

Scans dictionary chain and returns true if end is reached.

jump,           ( Hole-for-Opcode Destination )         Writes an unconditional Jump to a-addr-Destination with the given Bitmask as Opcode into the halfword sized h-addr-Hole
cjump, 	        ( Hole-for-Opcode Destination Bitmask ) Writes a conditional Jump to a-addr-Destination with the given Bitmask as Opcode into the halfword sized h-addr-Hole
```

### Special Words Depending on MCU Capabilities

```
c,              ( c -- )         Appends a byte to dictionary. NOT AVAILABLE ON ALL MCU’S. Check with ‘ c, or just look in ‘words’.
halign          ( -- ) 	         Makes Dictionary Pointer even, if uneven.

eraseflash      ( -- )           Erases everything after — Flash Dictionary — Clears Ram, Restarts Forth. Does not erase Mecrisp-Stellaris
eraseflashfrom  ( a- -- )        Starts erasing at this address. Clears Ram. Restarts Forth.
flashpageerase  ( a- -- )        Erase one 4 KiB flash page only. Take care: No Reset, no dictionary reinitialisation.
hflash!         ( u|n a- -- )    Writes halfword to flash
   
movwmovt,       ( x u -- )       Generate a movw/movt-Sequence to get x into any given Register u. M3/M4 only
registerliteral,  ( x u -- ) 	 Generate shortest possible sequenceto get x into given low Register u. On M0: A movs-lsls-adds… sequence M3/M4: movs / movs-mvns / movw / movw-movt
12bitencoding   ( x -- x false | bitmask true )     Can x be encoded as 12-bit immediate ?
```

### Flags and Inventory

Speciality!
```
smudge          ( -- )                Makes current definition visible, burns collected flags to flash and takes care of proper ending
inline          ( -- )                Takes the code of a word, and puts it in place of a call to this word
immediate       ( -- )                Will not compile this word into the dictionary but execute the word immediately
compileonly     ( -- )                Makes current definition compileonly
setflags        ( c -- )              Sets Flags with a mask. This isn’t immediate,
find            ( c- u -- a- f )      Searches for a String in Dictionary. Gives back flags, which are different to ANS!
```

### Folding

Speciality!

```
0-foldable      ( -- )          Current word becomes foldable with 0 constants 	constants variables [‘] [char]
1-foldable      ( -- )          1 constant      ?dup drop negate
2-foldable      ( -- )          2 constants     + - * swap nip
3-foldable      ( -- )          3 constants     rot 
4-foldable      ( -- )          4 constants     d+ d- 2swap
5-foldable      ( -- )          5 constants 	 
6-foldable      ( -- )          6 constants 	 
7-foldable      ( -- )          7 constants 	 
```

Compiler Essentials
-------------------

The true POWER of Forth resides in the Words listed in this table.
Subtle differences to ANS.

```
execute         ( a- -- )                 Calls subroutine
recurse         ( -- )                    Lets the current definition call itself
'               ( "<spaces>name" -- xt )  Tries to find name in dictionary gives back executable address
[']             ( "<spaces>name" -- xt)   Tick that compiles the executable address of found word as literal
postpone        ( -- )                    See Glossary
does>           ( -- )                    executes: ( --a-addr ) Gives address to where you have stored data.
<builds         ( -- )                    Makes Dictionary header and reserves space for special call.
create          ( "<spaces>name" -- )     Names a location; space may be allocated at this location, or it can be set to contain a string or other initialized value.
  name          ( -- a- )                 Instance behavior returns the address of the beginning of this space. Use FIG-style <builds .. does> !
state           ( -- a- )                 Address of state variable
]               ( -- ) 	                  Switch to compile state
[               ( -- )                    Switch to execute state
:               ( "<spaces>name" --  )    Opens new definition
;               ( -- )                    Finishes new definition
```

Common Hardware Access
----------------------

For interrupt handling see [Interrupts and Forth](CmsisRtos.md#interrupts-and-forth).

```
reset           ( -- )          Reset on hardware level
dint            ( -- )          Disables Interrupts, do not use!
eint            ( -- )          Enables Interrupts
eint?           ( -- )          Are Interrupts enabled ?
nop             ( -- )          No Operation. Hook for unused handlers !
ipsr            ( -- u )        Interrupt Program Status Register
unhandled       ( -- )          Message for unhandled interrupts.
irq-systick     ( -- a- )       Memory locations for IRQ-Hooks
irq-fault       ( -- a- )       For all faults
irq-collection  ( -- a- )       Collection of all unhandled interrupts
```

The original of this document can be found at https://mecrisp-stellaris-folkdoc.sourceforge.io
Copyright 2016-2020, Terry Porter, released under the GPL V3.

