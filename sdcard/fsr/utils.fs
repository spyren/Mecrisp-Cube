\ some utilities
CR .( utils.fs loading ... )

: u.4 ( u -- ) 0 <# # # # # #> type ;
: u.2 ( u -- ) 0 <# # # #> type ;

: #digits ( n -- u )
  abs 1 swap 
  begin 
    10 / dup 1 >= while
    swap 1+ swap
  repeat
  drop
;

: .r ( n u -- )
  over #digits 
  over 0< if
    1+
  then
  - spaces
  .
;

: tolower ( C -- c ) 
  32 or 
;

: toupper ( c -- C ) 
  32 not and 
;

: lower ( addr len -- ) 
  over + swap  
  do 
    i c@ tolower i c!  
  loop 
;

: upper ( addr len -- ) 
  over + swap  
  do 
    i c@ toupper i c!  
  loop 
;

: nexttoken ( -- addr len )
  begin
    token          \ Fetch new token.
  dup 0= while      \ If length of token is zero, end of line is reached.
    2drop cr query   \ Fetch new line.
  repeat
;


\ change drives

: 0: (  --  )
  s0" 0:" drop f_chdrive if
    ." invalid drive"
  then
;


: 1: (  --  )
  s0" 1:" drop f_chdrive if
    ." invalid drive"
  then
;


\ file redirection

: >file ( addr c-addr -- addr )  \ redirect to a file (emit, type, ...)
  swap dup stdout ! swap
  FA_WRITE FA_OPEN_ALWAYS + f_open drop
  hook-emit @  
  ['] fs-emit hook-emit !
;

: >term ( addr -- )   \ terminate to redirection
  hook-emit ! 
  stdout @ f_close drop
;

: <file ( addr c-addr -- addr )  \ redirection for a file (key, accept, ...)
  swap dup stdin ! swap
  FA_READ f_open drop
  hook-key @  
  ['] fs-key hook-key !
;

: <term ( addr -- )   \ terminate from redirection
  hook-key ! 
  stdin @ f_close drop
;


\ create user variable

: user  ( "name" -- )
  create , does> @ 
  0 dup pvTaskGetThreadLocalStoragePointer + 
;