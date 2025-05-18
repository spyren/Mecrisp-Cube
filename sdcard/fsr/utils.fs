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

100 buffer: word-buffer
50  buffer: match-buffer
0 variable word-len
0 variable match-len
 
: mwords ( "str" -- )  \ list all words that match a parsed string str
  bl word count dup match-len ! 
  match-buffer swap cmove 
  match-buffer match-len @ upper
  cr
  dictionarystart
  begin
    dup 6 +  ( dict-adr counted-word ) 
    count dup word-len !  word-buffer swap cmove
    word-buffer word-len @ 2dup upper 
    match-buffer match-len @ 
    search nip nip if
      \ print word
      dup 6 + ctype cr
    then
    dictionarynext
  until
  drop
;

: ms ( u -- ) \ Wait at least u milliseconds
  osDelay drop
;

