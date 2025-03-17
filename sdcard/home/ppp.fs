CR .( ppp.fs loading ... )

5 constant pwm_mode

pwm_mode 0 dmod   \ set D0 to pwm
pwm_mode 1 dmod   \ set D1 to pwm
0 0 pwmpin!
0 1 pwmpin!

0 variable direction \ 0 forward, 1 reverse
0 variable brake     \ 1 brake
0 variable speed     \ 0 off, 1000 max
0 variable pwmselect \ 0 1 kHz, 1 2 kHz, 2 4 kHz, 3 8 kHz

: calc_speed ( -- u ) \  get speed n 0 .. 1000
    0 apin@ 4 / dup
    1000 > if  drop 1000 then dup speed !
;

: %pwm ( -- u ) \ 
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

: Vlipo (  -- r ) \ LiPo voltage 0 apin@ 4 / .
  vref@ s>f 1k f/              \ Vref
  1 apin@ s>f 1e 4096e f/ f* f* \ V measure
  2e f*                          \ 1/2 voltage divider
;

: Irail ( -- r ) \ rail current
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
  0 2 oledpos! 3 set-precision
  ." I " Irail f. ." A"
;

: %pwm. ( -- )
  3 oledfont
  0 4 oledpos!
  
  \ 0123456789
  \ < 100 % B 
  direction @ if 
    ." < " %pwm . ." % "
    brake @ if 
      ." B" 
    else 
      ."    " 
    then
    ."   "
  else 
    ."   " %pwm . ." % " 
    brake @ if 
      ." B" 
    else 
      ."  " 
    then
    ." > " 
  then
  
;

: pwm-menu.
  0 oledfont
  0 6 oledpos!
          \ 0123456789012345
         ." PWM Menu [kHz]"
  0 7 oledpos!
  pwmselect @ case
    0 of ." [1] 2  4  8 " endof
    1 of ."  1 [2] 4  8 " endof
    2 of ."  1  2 [4] 8 " endof
    3 of ."  1  2  4 [8]" endof
  endcase
;

: throttle ( -- ) 
  begin
    10 osdelay drop
    calc_speed
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

: ppp-display ( -- )  \ display throttle infos till button is pressed
  >oled
  begin
     Vrail. Vlipo. 
     Irail. 
     %pwm.
     pwm-menu.
     200 osDelay drop
  button? until
  >term
;

: ppp-menu ( -- )
  begin
    ppp-display
    button case
      [char] a of 1 direction ! endof      \ reverse
      [char] b of brake @ 0= brake ! endof \ brake
      [char] c of 0 direction ! endof      \ forward
      [char] d of 32 PWMprescale ! 0 pwmselect ! endof   \ 1 kHz
      [char] e of 16 PWMprescale ! 1 pwmselect ! endof   \ 2 kHz
      [char] f of  8 PWMprescale ! 2 pwmselect ! endof   \ 4 kHz
      [char] g of  4 PWMprescale ! 3 pwmselect ! endof   \ 8 kHz
    endcase
  again
;

oledclr  

task throttle&
throttle& construct
' throttle throttle& start-task

task ppp-menu&
ppp-menu& construct
' ppp-menu ppp-menu& start-task



