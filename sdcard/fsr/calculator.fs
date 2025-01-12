\  @brief
\      4TH Calculator
\
\      Interpret the words from the calculator keyboard (buttons).
\      For details see peripherals/button.c.
\  @file
\      calculator.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2024-05-06
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\
\      This project Mecrsip-Cube is free software: you can redistribute it
\      and/or modify it under the terms of the GNU General Public License
\      as published by the Free Software Foundation, either version 3 of
\      the License, or (at your option) any later version.
\
\      Mecrsip-Cube is distributed in the hope that it will be useful, but
\      WITHOUT ANY WARRANTY; without even the implied warranty of
\      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
\      General Public License for more details.
\
\      You should have received a copy of the GNU General Public License
\      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.

CR .( calculator.fs loading ... )

false variable integer? \ default is float number
true  variable sign?    \ default with sign 
1 variable notation     \ 0 fix, 1 eng, 2 sci
' null-emit variable std-emit    \ standard emit
' null-emit? variable std-emit?   \ standard emit

0 variable register 9 cells allot \ 10 registers
register 10 cells 0 fill

: calc ( -- ) \ activate calculator, no echo on terminal
  ['] null-emit dup   hook-emit !   std-emit !
  ['] null-emit? dup  hook-emit? !  std-emit? !
  
;

: term ( -- ) \ activate terminal, but calculator ist still working
  ['] cdc-emit dup   hook-emit !   std-emit !
  ['] cdc-emit? dup  hook-emit? !  std-emit? !
  .greet 
;

: clock ( -- ) \ activate clock
  ['] null-emit dup   hook-emit !   std-emit !
  ['] null-emit? dup  hook-emit? !  std-emit? !
;

: float ( -- ) \ use float numbers
  false integer? !
;

: int ( -- ) \ use integer numbers
  true integer? !
;

: sgn ( -- ) \ print with sign
  true sign? !
;

: unsgn ( -- ) \ print without sign
  false sign? !
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

: sto ( n u -- n ) \ store n into register u (0 .. 9)
  dup dup 0 >=  swap 9 <=  and if 
    cells register + 2dup ! drop
  else
    drop
  then
;

: rcl ( u -- n ) \ recall n from register u (0 .. 9)
  dup dup 0 >=  swap 9 <=  and if 
    cells register + @
  else
    drop
  then
;

: r/s ( -- ) \ not used yet
;

: ffct ( -- ) \ user defined float function
;

: fct ( -- ) \ user defined integer function
;

$ae01 variable display_off
$af01 variable display_on

: off ( -- ) \ not used yet
  display_off oledcmd
  \ wait for key
  display_on oledcmd
;

: octal ( -- )
  8 base !
;

: .element ( n -- )
  integer? @ if
    sign? @ if
      .
    else
      u.
    then    
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
  integer? @ if 
    base @ case
     16 of ." HEX" endof
     10 of ." DEC" endof
      8 of ." OCT" endof
      2 of ." BIN" endof
    endcase
    94 1 oledpos!
    sign? @ if
      ." SIGN"
    else
      ." USGN"
    then  
  else
    ." FLT"
  then
  0 3 oledpos!
  std-emit @ hook-emit ! \ redirect terminal to cdc
;

\ extended prompt to update the OLED
: init ;

: prompt ( -- ) 
  begin 
    query interpret 
    ." ok." cr 
    .stack
  again
;

     
: init init ['] prompt hook-quit ! ; \ make new prompt 
     
init quit


