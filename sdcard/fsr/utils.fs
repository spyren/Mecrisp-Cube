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

: ms ( u -- ) \ Wait at least u milliseconds
  osDelay drop
;

: time&date ( -- u1 u2 u3 u4 u5 u6 )  \ u1 s, u2 Min, u3 h, u4 day, u5 Month, u6 Year
  clktime 
  dup 100 mod swap
  dup 10000 mod 100 / swap
  10000 /
  clkdate
  dup 100 mod swap
  dup 10000 mod 100 / swap
  10000 /
;

