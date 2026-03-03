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

: tolower ( C -- c ) \ only 7 bit ASCII
  dup dup [char] A >=  swap [char] Z <= and if
    32 or 
  then
;

: toupper ( c -- C ) \ only 7 bit ASCII
  dup dup [char] a >=  swap [char] z <= and if
    32 not and 
  then
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

: ms ( u -- ) \ Wait at least u milliseconds
  osDelay drop
;

