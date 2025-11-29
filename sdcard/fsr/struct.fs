\ data structures (like C structs)

\ This file is in the public domain. NO WARRANTY.

\ This program uses the following words
\ from CORE :
\ : 1- + swap invert and ; DOES> @ immediate drop Create rot dup , >r
\ r> IF ELSE THEN over chars aligned cells 2* here - allot
\ from CORE-EXT :
\ tuck pick nip 
\ from BLOCK-EXT :
\ \ 
\ from DOUBLE :
\ 2Constant 
\ from FILE :
\ ( 
\ from MEMORY :
\ allocate 

: naligned ( addr1 n -- addr2 )
    \ addr2 is the aligned version of addr1 wrt the alignment size n
    1- tuck +  swap invert and ;

: +field ( n1 n2 "name" -- n3 ) \ Forth-2012
    create over , +	
  does> ( name execution: addr1 -- addr2 )
    @ + ;

: 0field ( "name" -- )
    \ "name" does nothing and compiles nothing (as a field with 0 offset should)
    create immediate
  does> ( name execution: -- )
    drop ;
  
: field ( align1 offset1 align size "name" --  align2 offset2 )
    \ name execution: addr1 -- addr2
    >r tuck naligned r> +field ( align1 align offset2 )
    >r naligned r> ;

: end-struct ( align size "name" -- )
    over naligned \ pad size to full alignment
    2constant ;

\ an empty struct
1 chars 0 end-struct struct

\ type descriptors, all ( -- align size )
1 aligned   1 cells   2constant cell%
1 chars     1 chars   2constant char%
cell% 2*              2constant double%

\ memory allocation words
: %alignment ( align size -- align )
    drop ;

: %size ( align size -- size )
    nip ;

: %align ( align size -- )
    drop here swap nalign here - allot ;

: %allot ( align size -- addr )
    tuck %align
    here swap allot ;

: %allocate ( align size -- addr ior )
    nip allocate ;


