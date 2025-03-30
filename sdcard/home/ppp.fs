\  @brief
\      Pocket Power Pack
\      
\      Model railroad power pack (throttle).
\      HW based on STM32WB55 Firefly.
\  @file
\      ppp.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2025-01-21
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
CR .( ppp.fs loading ... )

5 constant PWM_MODE

0 variable menu      \ 0 mode, 1 pwm1, 2 pwm2
2 constant maxmenu
0 variable dcc       \ 0 DC, 1 DCC
0 variable direction \ 0 forward, 1 reverse
0 variable brake     \ 1 brake
0 variable speed     \ 0 off, 1000 max
5 variable pwmselect \ 0 0.25 kHz, 1 0.5 kHz, 2 1 kHz, 
                     \ 3 2 kHz, 4 4 kHz, 5 8 kHz, 6 16 kHz
0 variable display-off

: speed@ ( -- u ) \  get speed u 0 .. 1000
    0 apin@ 4 / dup
    1000 > if  drop 1000 then dup speed !
;

: %pwm ( -- u ) \ get PWM u 0 .. 100 %
  speed @ 10 /
; 

: Vusb (  -- r ) \ USB voltage 
  vref@ s>f 1k f/              \ Vref
  4 apin@ s>f 1e 4096e f/ f* f* \ V measure
  2e f*                          \ 1/2 voltage divider
;

: Vrail (  -- r ) \ rail voltage 
  vref@ s>f 1k f/              \ Vref
  2 apin@ s>f 1e 4096e f/ f* f* \ V measure
  10k f/ 68k 10k f+ f*           \ 68 k 10 k voltage divider
;

: Vlipo (  -- r ) \ LiPo voltage 0
  vref@ s>f 1k f/              \ Vref
  1 apin@ s>f 1e 4096e f/ f* f* \ V measure
  2e f*                          \ 1/2 voltage divider
;

: Irail ( -- r ) \ rail current [A]
  vref@ s>f 1k f/              \ Vref
  3 apin@ s>f 1e 4096e f/ f* f* \ V measure -> A
;

: Vrail. ( -- )
  2 oledfont
  0 0 oledpos! 1 set-precision
  ." Vr " Vrail f. 
;

: Vlipo. ( -- )
  2 oledfont
  64 0 oledpos! 2 set-precision
  ." Vb " Vlipo f. 
;

: Irail. ( -- )
  3 oledfont
  0 4 oledpos! 3 set-precision
  ." I " Irail f. ." A"
;

: %pwm. ( -- )
  3 oledfont
  0 2 oledpos!
  
  \ 0123456789
  \ < 100 %  > 
  direction @ if 
    ." < " %pwm . ." %    "
  else 
    ."   " %pwm . ." %  > " 
  then
;

: mode.
  0 oledfont
  0 6 oledpos!
   \ 012345678901234567890
  ." Display, Mode        " 
  0 7 oledpos!
  ."  OFF  "
  dcc @ if ." [DCC]        " else ." [DC]C         " then 
;

: BRK. ( -- )
  brake @ if ." [BRK]" else ."  BRK " then
;

: pwm-menu.
  0 oledfont
  0 6 oledpos!
          \ 012345678901234567890
         ." PWM Frequency [kHz] 1"
  0 7 oledpos!
  pwmselect @ case
          \ 012345678901234567890
          \ [BRK]
    0 of BRK. ." [.25] .5    1  " endof
    1 of BRK. ."  .25 [.5]   1  " endof
    2 of BRK. ."  .25  .5   [1] " endof
         BRK. ."  .25  .5    1  "
  endcase
;

: pwm-menu1.
  0 oledfont
  0 6 oledpos!
          \ 012345678901234567890
         ." PWM Frequency [kHz] 2"
  0 7 oledpos!
  pwmselect @ case
          \ 012345678901234567890
    3 of ."  [2]   4    8    16 " endof
    4 of ."   2   [4]   8    16 " endof
    5 of ."   2    4   [8]   16 " endof
    6 of ."   2    4    8   [16]" endof
         ."   2    4    8    16 "
  endcase
;

: menu. ( -- ) 
  menu @ case
    0 of mode. endof
    1 of pwm-menu. endof
    2 of pwm-menu1. endof
  endcase
;

: throttle ( -- ) \ control the PWM pins, update every 10 ms
  0 0 pwmpin!
  0 1 pwmpin!
  4 pwmprescale     \ 8 kHz
  PWM_MODE 0 dmod   \ set D0 to pwm
  PWM_MODE 1 dmod   \ set D1 to pwm

  begin
    10 osdelay drop
    speed@
    direction @ if
      \ forward
      brake @ if
        1000            0 pwmpin!
        1000 swap -     1 pwmpin!
      else
        0               1 pwmpin!
                        0 pwmpin!
      then
    else
      \ reverse
      brake @ if
        1000            1 pwmpin!
        1000 swap -     0 pwmpin!
      else
        0               0 pwmpin!
                        1 pwmpin!
      then
    then
  again
;

: ppp-display ( -- )  \ display throttle infos every 200 ms till button is pressed
  >oled
  begin
     Vrail. Vlipo. 
     %pwm.
     Irail. 
     menu.
     200 osDelay drop
  button? until
  >term
;

: mode-button ( u -- ) \ mode buttons
  case
    [char] d of 1 display-off ! oledclr endof \ switch off display
    [char] e of dcc @ 0= dcc ! endof          \ toggle DCC
  endcase
;

: pwm-button ( u -- ) \ Brake and PWM1 buttons
  case
    [char] d of brake @ 0= brake ! endof            \ toggle brake
    [char] e of 0 pwmselect ! 128 pwmprescale endof \ 250 Hz
    [char] f of 1 pwmselect !  64 pwmprescale endof \ 500 Hz
    [char] g of 2 pwmselect !  32 pwmprescale endof \ 1 kHz
  endcase
;

: pwm-button1 ( u -- ) \ PWM2 buttons
  case
    [char] d of 3 pwmselect !  16 pwmprescale endof \ 2 kHz
    [char] e of 4 pwmselect !   8 pwmprescale endof \ 4 kHz
    [char] f of 5 pwmselect !   4 pwmprescale endof \ 8 kHz
    [char] g of 6 pwmselect !   2 pwmprescale endof \ 16  kHz
  endcase
;

: menu-button ( u -- ) \ menu buttons
  display-off @ if
    drop 0 display-off !
  else
    menu @ case
      0 of mode-button endof 
      1 of pwm-button  endof
      2 of pwm-button1 endof
    endcase
  then
;

: ppp-menu ( -- ) \ display throttle infos till a button is pressed
  begin
    display-off @ 0= if ppp-display then
    button case
      [char] a of 1 direction ! endof      \ reverse
      [char] b of menu @ 1+ dup maxmenu > if drop 0 then menu ! endof \ menu
      [char] c of 0 direction ! endof      \ forward
      menu-button
    endcase
  again
;

task throttle&
task ppp-menu&


\ this should part of /etc/rc.local to start the threads
\
\ 2000 osDelay drop \ time to display flash screen
\ oledclr  
\ throttle& construct
\ ' throttle throttle& start-task
\ ppp-menu& construct
\ ' ppp-menu ppp-menu& start-task



