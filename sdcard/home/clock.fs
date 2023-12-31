: clock ( -- )
  lcdclr
  2 lcdfont
  >lcd 
  -1 -1 -1 alarm!  \ set an alarm every second
  begin
    wait-alarm     \ wait a second
    0 0 lcdpos!
    .time
  key? until
  key drop
  >term
;
