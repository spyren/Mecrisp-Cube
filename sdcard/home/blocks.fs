\ list and load for blocks

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


: list ( n -- )
  dup cr ." Block# " . cr
  block
  16 0 do
    i 3 .r dup 64 type cr
    64 +
  loop
  drop
;

: load ( n -- )
  block
  16 0 do
    dup 64 evaluate 
    64 +
  loop
  drop
;

