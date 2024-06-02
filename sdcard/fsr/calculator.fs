\ 4th calculator
CR .( calculator.fs loading ... )

false variable integer	\ default is float number
0 variable notation 	\ 0 fix, 1 eng, 2 sci
0 variable register 9 cells allot \ 10 registers
register 10 cells 0 fill

: float ( -- ) 
  false integer !
;

: int ( -- )
  true integer !
;

: fix ( -- )
  0 notation !
;

: eng ( -- )
  1 notation !
;

: sci ( -- )
  2 notation !
;

: sto ( n u -- ) \ store n into register u (0 .. 9)
  cells register + !
;

: rcl ( u -- n ) \ recall n from register u (0 .. 9)
  cells register + @
;


: .element ( n -- )
  integer @ if
    .
  else
    notation @ case
      0 of f. endof
      1 of fm. endof
      2 of fs. endof
    endcase
  then
;

: .stack ( -- ) \ print the 3 first stack elements to the OLED
  oledclr
  1 oledfont \ 8x8 font
  ['] oled-emit hook-emit ! \ redirect terminal to oled
  depth 
  dup 0<> if 
    dup 3 > if drop 3 then
    0 do
      i case
        0 of dup endof
        1 of 1 pick endof
        2 of 2 pick endof
      endcase
      0 2 i - oledpos! .element 
    loop  
  then
  102 0 oledpos!
  integer @ if 
    base @ case
      10 of ." DEC" endof
      16 of ." HEX" endof
      2  of ." BIN" endof
    endcase  
  else
    ." FLT"
  then
  0 3 oledpos!
  ['] cdc-emit hook-emit ! \ redirect terminal to cdc
;

\ extended prompt update the OLED
: init ;

: prompt ( -- ) 
  begin 
    query interpret 
    ."  ok." cr 
    .stack
  again
;
     
: init init ['] prompt hook-quit ! ; \ make new prompt 
     
init quit


