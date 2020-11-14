Mecrisp-Stellaris Forth Words
=============================

Terminal-IO
-----------

Exactly ANS, some logical extension

    Words 	        Stack Comment 	Description

    emit? 	        ( -- Flag )     Ready to send a character ?
    key?            ( -- Flag )     Checks if a key is waiting
    key             ( -- Char )     Waits for and fetches the pressed key
    emit            ( Char -- )     Emits a character
    hook-emit?      ( -- a-addr )   Hooks for redirecting terminal IO on the fly
    hook-key?
    hook-key
    hook-emit
    serial-emit?    ( -- Flag )     Serial interface terminal routines as default communications
    serial-key?
    serial-key      ( -- Char )     Waits for and fetches the pressed key
    serial-emit     ( Char -- )     Emits a character
    hook-pause      ( -- a-addr )   Hook for a multitasker
    pause           ( -- ) 	        Task switch, none for default


### Character Manipulation

How to convert numbers to characters and convert characters to numbers.

    [char] *        ( -- char )     Compiles code of following char when executed 
    char *          ( -- char )     Gives code of following char 
    emit            ( char -- )     Emits a character


Stack Jugglers
--------------

### Single-Jugglers

Exactly ANS, some logical extension.

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
    r> 	            ( -- x ) (R: x -- )     Pull from Return Stack 	 
    r@              ( -- x ) (R: x -- x )   Copy from Return Stack 	 
    rdrop           ( -- ) (R: x -- ) 	  	 
    rdepth          ( -- +n )               Gives number of return stack items. 

    roll            ( xu xu-1 ... x0 u -- xu-1 ... x0 xu ) 	        Remove u. Rotate u+1 items on the top of the stack.
    pick            ( ... xi+1 xi ... x1 x0 i -- ... x1 x0 xi )     Copy a element X levels down the stack to the top of the stack 	 
    rpick           ( i -- xi ) R: ( ... xi ... x0 -- ... xi ... x0 ) 	  	 
    -roll 	        ( xu-1 ... x0 xu u – xu xu-1 ... x0 u ) 

### Double-Jugglers

They perform the same for double numbers

    2nip 	        ( x1 x2 x3 x4 --x3 x4 )
    2drop 	        ( x1 x2 -- )
    2rot 	        ( x1 x2 x3 x4 x5 x6 -- x3 x4 x5 x6 x1 x2 )
    2-rot 	        ( x1 x2 x3 x4 x5 x6 -- x5 x6 x1 x2 x3 x4 )
    2swap 	        ( x1 x2 x3 x4 -- x3 x4 x1 x2 )
    2tuck 	        ( x1 x2 x3 x4 -- x3 x4 x1 x2 x3 x4 )
    2over 	        ( x1 x2 x3 x4 -- x1 x2 x3 x4 x1 x2 )
    2dup 	        ( x1 x2 -- x1 x2 x1 x2 )
    2>r 	        ( x1 x2 -- ) (R: --x1 x2 )
    2r> 	        ( -- x1 x2 ) (R: x1 x2 -- )
    2r@ 	        ( -- x1 x2 ) (R: x1 x2 --x1 x2 )
    2rdrop 	        ( -- ) (R: x1 x2 -- )

### Stack pointers

    sp@ 	        ( -- a-addr )   Fetch data stack pointer
    sp! 	        ( a-addr -- ) 	Store data stack pointer
    rp@ 	        ( -- a-addr ) 	Fetch return stack pointer
    rp! 	        ( a-addr -- ) 	Store return stack pointer


Logic
-----

exactly ANS, some logical extensions.

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


Single Number Math
------------------

Exactly ANS, some logical extensions.

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


Double Number Math
------------------

exactly ANS, some logical extensions.

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


Decision Making
---------------

### Single-Comparisons

Exactly ANS, some logical extensions.

#### Unsigned Comparisons

    u<=             ( u1 u2 -- flag )   u1 less-or-equal u2
    u>=             ( u1 u2 -- flag )   u1 greater-or-equal u2
    u>              ( u1 u2 -- flag )   u1 greater-than u2
    u<              ( u1 u2 -- flag )   u1 less-than u2

#### Signed Comparisons

Exactly ANS, some logical extensions.
 
    <=              ( n1 n2 -- flag )   n1 less-or-equal n2
    >=              ( n1 n2 -- flag )   n1 greater-or-equal n2
    >               ( n1 n2 -- flag )   n1 greater-than n2
    <               ( n1 n2 -- flag )   n1 less-than n2
    0<              ( n - flag )        n less than zero
    0<>             ( n -- flag )       n not equal to zero
    0=              ( n -- flag )       n equals zero
    <>              ( n1 n2 -- flag )   n1 not-equals n2
    =               ( n1 n2 -- flag )   n1 equals n2

### Double-Comparisons

They perform the same for double numbers.

    du>             ( ud1 ud2 -- flag )
    du<             ( ud1 ud2 -- flag )
    d>              ( d1 d2 -- flag )
    d<              ( d1 d2 -- flag )
    d0<             ( d -- flag )
    d0=             ( d -- flag )
    d<>             ( d1 d2 -- flag )
    d=              ( d1 d2 -- flag )

### Bits: Test, Toggle, Clear and Set

Note: TRUE = 1, FALSE = 0. If a Bit is HIGH it leaves TRUE on the stack, otherwise FALSE.

    cbit@           ( mask c-addr -- flag ) Test bit in byte-location
    hbit@           ( mask h-addr -- flag ) Test bit in halfword-location
    bit@            ( mask a-addr -- flag ) Test bit in word-location
    cxor!           ( mask c-addr -- )      Toggle bit in byte-location
    hxor!           ( mask h-addr -- )      Toggle bit in halfword-location
    xor!            ( mask a-addr -- )      Toggle bit in word-location
    cbic!           ( mask c-addr -- )      Clear bit in byte-location
    hbic!           ( mask h-addr -- )      Clear bit in halfword-location
    bic!            ( mask a-addr -- )      Clear bit in word-location
    cbis!           ( mask c-addr -- )      Set bit in byte-location
    hbis!           ( mask h-addr -- )      Set bit in halfword-location
    bis!            ( mask a-addr -- )      Set bit in word-location

Memory Status
------------

    unused          ( -- u )        Displays memory depending on compile mode (Ram or Flash)

Memory Access
-------------

Subtle differences to ANS, special cpu-specific extensions. Fetch-Modify-Store operations like +! or bic! are NOT ATOMIC.

    move            ( c-addr1 c-addr2 u -- )    Moves u Bytes in Memory
    fill            ( c-addr u c ) 	    Fill u Bytes of Memory with value c
    2constant name  ( ud|d -- ) 	    Makes a double constant.
    constant name   ( u|n -- )          Makes a single constant. i.e. “$1024 constant one-kb”
    2variable name  ( ud|d -- )         Makes an initialized double variable
    variable name   ( n|n -- )          Makes an initialized single variable. i.e. “0 variable one-kb”
    nvariable name  ( n1*u|n n1 -- )    Makes an initialized variable with specified size of n1 words Maximum is 15 words
    buffer: name    ( u -- )            Creates a buffer in RAM u bytes in length
    2@              ( a-addr -- ud|d )  Fetches double number from memory
    2!              ( ud|d a-addr -- )  Stores double number in memory
    @               ( a-addr -- u|n )   Fetches single number from memory
    !               ( u|n a-addr -- )   Stores single number in memory
    +!              ( u|n a-addr -- )   Plus Store, use to increment a variable, register etc
    h@              ( c-addr -- char )  Fetches halfword from memory
    h!              ( char c-addr )     Stores halfword in memory
    h+!             ( u|n a-addr -- )   Add to halfword memory location
    c@              ( c-addr -- char )  Fetches byte from memory
    c!              ( char c-addr )     Stores byte in memory
    c+!             ( u|n a-addr -- )   Add to byte memory location


String Formatting
-----------------

### String Routines

Warning: ” may not render properly on your browser, hence copying and pasting commands that use ” may fail. Click on “Show Source” to see the correct character.

Exactly ANS, some logical extensions.

    type 	        ( c-addr length -- )    Prints a string.
    s” Hello”       ( -- c-addr length )    Compiles a string and gives back its address and length when executed
    .” Hello”       ( -- ) 	                Compiles a string and prints it when executed.
    ( Comment ) 	                        Ignore Comment 	 
    \ Comment                               Comment to end of line 	 
    cr              ( -- )                  Emits line feed 	 
    bl              ( -- 32 )               ASCII for Space 	 
    space           ( -- )                  Emits space 	 
    spaces          ( n -- )                Emits n spaces if n is positive
    compare         ( caddr-1 len-1 c-addr-2 len-2 -- flag )    Compares two strings
    accept          ( c-addr maxlength -- length )              Read input into a string.

### Counted String Routines

Warning: ” may not render properly on your browser, hence copying and pasting commands that use ” may fail. Click on “Show Source” to see the correct character.

Exactly ANS, some logical extensions.

    ctype           ( cstr-addr -- )                Prints a counted string.
    c” Hello”       ( -- cstr-addr )                Compiles a counted string and gives back its address when executed.
    cexpect         ( cstr-addr maxlength -- )      Read input into a counted string.
    count           ( cstr-addr -- c-addr length ) 	Convert counted string into addr-length string
    skipstring      ( cstr-addr -- a-addr )         Increases the pointer to the aligned end of the string.

### Pictured Numerical Output

#### Pictured Numerical output

    .digit 	        ( u -- char )               Converts a digit to a char
    digit           ( char -- u true | false )  Converts a char to a digit
    [char] *        ( -- char )                 Compiles code of following char when executed
    char *          ( -- char )                 gives code of following char
    hold            ( char -- )                 Adds character to pictured number output buffer from the front.
    hold<           ( char -- )                 Adds character to pictured number output buffer from behind
    sign            ( n -- )                    Add a minus sign to pictured number output buffer, if n is negative
    #S              ( ud1|d1 -- 0 0 )           Add all remaining digits from the double length number to output buffer
    f#S             ( n-comma1 -- n-comma2 )    Adds 32 comma-digits to number output
    #               ( ud1|d1 -- ud2|d2 )        Add one digit from the double length number to output buffer
    f#              ( n-comma1 -- n-comma2 )    Adds one comma-digit to number output
    #>              ( ud|d -- c-addr len )      Drops double-length number and finishes pictured numeric output ready for type
    <#              ( -- )                      Prepare pictured number output buffer
    u.              ( u -- )                    Print unsigned single number
    .               ( n -- )                    Print single number
    ud.             ( ud -- )                   Print unsigned double number
	d.              ( d -- )                    Print double number

Deep Insights
-------------

    words           ( -- )              Prints list of defined words and properties
    list            ( -- )              Prints all defined words. From; dissasembler-mx.txt
    .s              ( many -- many )    Prints stack contents, signed
    u.s             ( many -- many )    Prints stack contents, unsigned
    h.s             ( many -- many )    Prints stack contents, unsigned, hex
    hex.            ( u -- )            Prints 32 bit unsigned in hex base, needs emit only. This is independent of number subsystem.

User Input and Interpretation
-----------------------------

Exactly ANS, some logical extensions.

    query           ( -- )                  Fetches user input to input buffer
    tib             ( -- cstr-addr )        Input buffer
    current-source  ( -- addr )             Double-Variable which contains source
    tsource         ( c-addr len -- )       Change source
    source          ( -- c-addr len )       Current source
    >in             ( -- addr )             Variable with current offset into source
    token           ( -- c-addr len )       Cuts one token out of input buffer
    parse           ( char -- c-addr len )  Cuts anything delimited by char out of input buffer
    evaluate        ( any addr len -- any ) Interpret given string
    interpret       ( any -- any )          Execute, compile, fold, optimize…
    quit            ( many -- ) (R: many -- ) Resets Stacks
    hook-quit       ( -- a-addr )           Hook for changing the inner quit loop

Dictionary Expansion
--------------------

Exactly ANS, some logical extension.

    align           ( -- )                  Aligns dictionary pointer
    aligned         ( c-addr -- a-addr )    Advances to next aligned address
    cell+           ( x -- x+4 )            Add size of one cell
    cells           ( n -- 4*n )            Calculate size of n cells
    allot           ( n -- )                tries to advance Dictionary Pointer by n bytes, aborts, if not enough space available
    here            ( -- a-addr|c-addr )    Gives current position in Dictionary
    ,               ( u|n -- )              appends a single number to dictionary
    ><,             ( u|n -- )              reverses high and low-halfword, then appends it to dictionary
    h,              ( u|n -- )              appends a halfword to dictionary

    compiletoram? 	( -- ? )    currently compiling into ram ?
    compiletoram 	( -- )      makes ram the target for compiling
    compiletoflash 	( -- )      makes flash memory the target for compiling
    forgetram       ( -- )      Forget definitions in ram without a reset

Speciality!
-----------

    string,         ( c-addr len -- )   Inserts a string of maximum 255 characters without runtime
    literal,        ( u|n -- )          Compiles a literal with runtime
    inline,         ( a-addr -- )       Inlines the choosen subroutine
    call,           ( a-addr -- )       Compiles a call to a subroutine
    ret,            ( -- )              compiles a ret opcode
    flashvar-here   ( -- a-addr )       Gives current RAM management pointer
    dictionarystart ( -- a-addr )       Current entry point for dictionary search
    dictionarynext

    jump,           ( Hole-for-Opcode Destination )         Writes an unconditional Jump to a-addr-Destination with the given Bitmask as Opcode into the halfword sized h-addr-Hole
    cjump, 	        ( Hole-for-Opcode Destination Bitmask ) Writes a conditional Jump to a-addr-Destination with the given Bitmask as Opcode into the halfword sized h-addr-Hole


Special Words Depending on MCU Capabilities
-------------------------------------------

    c,              ( char -- )         Appends a byte to dictionary. NOT AVAILABLE ON ALL MCU’S. Check with ‘ c, or just look in ‘words’.
    halign          ( -- ) 	            Makes Dictionary Pointer even, if uneven.

    eraseflash      ( -- )              Erases everything after — Flash Dictionary — Clears Ram, Restarts Forth. Does not erase Mecrisp-Stellaris
    eraseflashfrom  ( a-addr – )        Starts erasing at this address. Clears Ram. Restarts Forth.
    flashpageerase  ( a-addr – )        Erase one 1k flash page only. Take care: No Reset, no dictionary reinitialisation.
    hflash!         ( u|n a-addr – )    Writes halfword to flash
   
    movwmovt,       ( x Register -- )   Generate a movw/movt-Sequence to get x into any given Register. M3/M4 only
    registerliteral,    ( x Register -- ) 	Generate shortest possible sequenceto get x into given low Register. On M0: A movs-lsls-adds… sequence M3/M4: movs / movs-mvns / movw / movw-movt
    12bitencoding   ( x -- x false | bitmask true )     Can x be encoded as 12-bit immediate ?


Flags and Inventory
-------------------

Speciality!

    smudge          ( -- )          Makes current definition visible, burns collected flags to flash and takes care of proper ending
    inline          ( -- )          Takes the code of a word, and puts it in place of a call to this word
    immediate       ( -- )          Will not compile this word into the dictionary but execute the word immediately
    compileonly     ( -- )          Makes current definition compileonly
    setflags        ( char -- )     Sets Flags with a mask. This isn’t immediate,
    (create) name   ( -- )          Names a location; space may be allocated at this location, or it can be set to contain a string or other initialized value. Instance behavior returns the address of the beginning of this space. Use FIG-style <builds .. does> !
    find            ( c-addr len -- a-addr flags) 	Searches for a String in Dictionary. Gives back flags, which are different to ANS!


Folding
-------
Speciality!

    0-foldable      ( -- )          Current word becomes foldable with 0 constants 	constants variables [‘] [char]
    1-foldable      ( -- )          1 constant      ?dup drop negate
    2-foldable      ( -- )          2 constants     + - * swap nip
    3-foldable      ( -- )          3 constants     rot 
    4-foldable      ( -- )          4 constants     d+ d- 2swap
    5-foldable      ( -- )          5 constants 	 
    6-foldable      ( -- )          6 constants 	 
    7-foldable      ( -- )          7 constants 	 


Compiler Essentials
-------------------

The true POWER of Forth resides in the Words listed in this table.
Subtle differences to ANS.

    execute         ( a-addr -- )   Calls subroutine
    recurse         ( -- )          Lets the current definition call itself
    ‘ name          ( -- a-addr )   Tries to find name in dictionary gives back executable address
    [‘] name        ( -- a-addr)    Tick that compiles the executable address of found word as literal
    postpone        ( -- )          See Glossary
    does>           ( -- )          executes: ( --a-addr ) Gives address to where you have stored data.
    <builds         ( -- )          Makes Dictionary header and reserves space for special call.
    create name     ( -- )          Create a definition with default action which cannot be changed later. Use <builds does> instead. Equivalent to : create <builds does> ;
    state           ( -- a-addr )   Address of state variable
    ]               ( -- ) 	        Switch to compile state
    [               ( -- )          Switch to execute state
    ;               ( -- )          Finishes new definition
    : name          ( -- )          Opens new definition


Control Structures
------------------

Internally, they have complicated compile-time stack effects.

### Summary

    do ... loop                 Finite loop incrementing by 1
    do ... +loop                Finite loop incrementing by X
    begin ... until             Indefinite loop terminating when is ‘true’
    begin ... while ... repeat  Indefinite loop terminating when is ‘false’
    begin ... again             Infinite loop
    if ... else ... then        Two-branch conditional; performs words following IF it is ‘true’ and words following ELSE if it is ‘false’. THEN marks the point at which the paths merge.
    if ... then                 Like the two-branch conditional, but with only a ‘true’ clause.

### Decisions

Exactly ANS.

    if              ( flag -- )     structure.
    else            ( -- )          flag if ... [else ...] then
    then            ( -- )          This is the common flag if ... [else ...] then structure.

“IF that’s the case, do this, ELSE do that ... and THEN continue with ...” 

    flag if 
        ... 
    then

    flag if 
        ...
    else 
        ... 
    then

    case            ( n --n )       Begins case structure
    of              ( m -- )        Compares m with n, choose this if n=m
    ?of             ( flag -- )     Flag-of, for custom comparisions
    endof           ( -- )          End of one possibility
    endcase         ( n -- )        Ends case structure, discards n

    n case
        m1    of  ... endof
        m2    of  ... endof
        flag  ?of ... endof
        all others
    endcase


### Definite Loops

A loop structure in which the words contained within the loop repeat a definite number of times. In Forth, this number depends on the starting and ending counts (index and limit) which are placed on the stack prior to the execution of the word DO.
Exactly ANS.

    i               ( -- u|n )      Gives innermost loop index
    j               ( -- u|n )      Gives second loop index
    k               ( -- u|n )      Gives third loop index
    exit            ( -- )          Returns from current definition. Compiles a ret opcode.

    do              ( Limit Index -- ) (R: -- old-limit old-index )     Begins a loop
    loop            ( -- ) (R: unchanged | old-limit old-index -- )     Increments current loop index register by one and checks whether to continue or not.
    +loop           ( u|n -- ) (R: unchanged | old-limit old-index -- ) If you want the index to go up by some number other than one each time around.
    unloop          (R: old-limit old-index -- ) 	                    Drops innermost loop structure, pops back old loop structures to loop registers
    leave           ( -- ) (R: old-limit old-index -- )                 Leaves current innermost loop promptly
    ?do             ( Limit Index -- ) (R: unchanged | -- old-limit old-index )     Begins a loop if limit and index are not equal


### Indefinite Loops

A loop structure in which the words contained within the loop continue to repeat until some truth condition changes state.

    repeat          ( -- )          Finish of a middle-flag-checking loop.
    while           ( flag -- )     Check a flag in the middle of a loop
    until           ( flag -- )     begin ... flag until loops as long flag is true
    again           ( -- )          begin ... again is an endless loop
    begin           ( -- ) 	 


Common Hardware Access
----------------------
 
    reset           ( -- )          Reset on hardware level
    dint            ( -- )          Disables Interrupts
    eint            ( -- )          Enables Interrupts
    eint?           ( -- )          Are Interrupts enabled ?
    nop             ( -- )          No Operation. Hook for unused handlers !
    ipsr            ( -- ipsr )     Interrupt Program Status Register
    unhandled       ( -- )          Message for unhandled interrupts.
    irq-systick     ( -- a-addr )   Memory locations for IRQ-Hooks
    irq-fault       ( -- a-addr )   For all faults
    irq-collection  ( -- a-addr )   Collection of all unhandled interrupts


\-- [PeterSchmid - 2020-10-11]
