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
