640 pwmprescale 
5 6 dmod   \ set D6 to PWM
11 16 dmod   \ set A0/D16 to analog

: servo ( -- ) 
  begin
    100 50 do
      i 6 pwmpin! 
      i neopixel! 
      i 50 = if 
        1000 \ give some more time to get back
      else
        200
      then 
      osDelay drop
    10 +loop
  key? until 
  key drop
;
