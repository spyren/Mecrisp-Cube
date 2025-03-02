CR .( ppp.fs loading ... )

5 constant pwm_mode

pwm_mode 0 dmod   \ set D0 to pwm
pwm_mode 1 dmod   \ set D1 to pwm
0 0 pwmpin!
0 1 pwmpin!

0 variable direction \ 0 forward, 1 reverse
0 variable brake     \ 1 brake
0 variable speed

: throttle ( -- )
  10 osdelay drop
  0 apin@ 4 / speed ! \ get speed 0 .. 1023
  direction @ if
    \ forward
    brake @ if
      1023 1 pwmpin!
    else
      0 1 pwmpin!
    then
    speed @ 0 pwmpin!
  then
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

: Vlipo (  -- r ) \ LiPo voltage 
  vref@ s>f 1k f/              \ Vref
  1 apin@ s>f 1e 4096e f/ f* f* \ V measure
  2e f*                          \ 1/2 voltage divider
;

: Irail ( -- r ) \ rail current
  vref@ s>f 1k f/              \ Vref
  3 apin@ s>f 1e 4096e f/ f* f* \ V measure -> A
;

: %pwm ( -- u ) \ 
  0 apin@ 40 / dup \ 4096 resolution
  100 > if
    drop 100 \ not more than 100 %
  then
; 
  

