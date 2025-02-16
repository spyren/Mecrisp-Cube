CR .( ppp.fs loading ... )

5 0 dmod   \ set D0 to pwm
5 1 dmod   \ set D1 to pwm
0 0 pwmpin!
0 1 pwmpin!

0 variable direction \ 0 forward, 1 reverse
0 variable brake     \ 1 brake
0 variable speed

: throttle ( -- )
  10 osdelay drop
  0 apin@ 4 / speed ! \ get speed 0 .. 1023
  direction @ if
    \ reverse
    brake @ if
      1023 0 pwmpin!
    else
      0 @ 0 pwmpin!
    then
    speed @ 1 pwmpin!
  else
    \ forward
    brake @ if
      1023 1 pwmpin!
    else
      0 1 pwmpin!
    then
    speed @ 0 pwmpin!
  then
;

: vrail. 
  vref@ s>f 1k f/              \  vref
  2 apin@ s>f 1e 4096e f/ f* f* \ v measure
  10k f/ 68k 10k f+ f*        
  fm.
;


