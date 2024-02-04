: clock ( -- )
  oledclr
  2 oledfont
  >oled 
  -1 -1 -1 alarm!  \ set an alarm every second
  begin
    wait-alarm     \ wait a second
    0 0 oledpos!
    .time
  key? until
  key drop
  >term
;
