CR .( ppp.fs loading ... )

5 constant pwm_mode

pwm_mode 0 dmod   \ set D0 to pwm
pwm_mode 1 dmod   \ set D1 to pwm
0 0 pwmpin!
0 1 pwmpin!

0 variable direction \ 0 forward, 1 reverse
0 variable brake     \ 1 brake
0 variable speed     \ 0 off, 1000 max

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
  0 0 oledpos! 1 set-precision
  ." Vr " Vrail f. ." V"
;

: Vlipo. ( -- )
  0 2 oledpos! 2 set-precision
  ." Vb " Vlipo f. ." V"
;

: Irail. ( -- )
  0 4 oledpos! 3 set-precision
  ." I  " Irail f. ." A"
;

: %pwm. ( -- )
  0 6 oledpos!
  %pwm . ." % "
  direction @ if ." < " else ." > " then
  brake @ if ." B " else ."   " then
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
  oledclr  3 oledfont
  begin
     Vrail. Vlipo. Irail. %pwm.
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
    endcase
  again
;

task throttle&
throttle& construct
' throttle throttle& start-task

task ppp-menu&
ppp-menu& construct
' ppp-menu ppp-menu& start-task



