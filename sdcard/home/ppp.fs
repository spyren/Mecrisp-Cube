CR .( ppp.fs loading ... )

5 constant pwm_mode

pwm_mode 0 dmod   \ set D0 to pwm
pwm_mode 1 dmod   \ set D1 to pwm
0 0 pwmpin!
0 1 pwmpin!

0 variable direction \ 1 forward, 0 reverse
0 variable brake     \ 1 brake
0 variable speed     \ 0 off, 1000 max

: calc_speed ( -- u ) \  get speed n 0 .. 1000
    0 apin@ 4 / dup
    1000 > if  drop 1000 then dup speed !
;

: %pwm ( -- u ) \ 
  speed @ 10 /
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
  key? until
  key drop
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

  

