1 plexshutdown
0 0 100 plexpixel!
1 1 200 plexpixel!

: count-down ( -- )
  plexclr
 -1 -1 -1 alarm!  \ an alarm every second
  wait-alarm  
  10 0 do
    1 plexpos!
    i 1 + 25 * plexpwm  \ set brightness
    i 0 = if 
      [char] 1 plex-emit
      [char] 0 plex-emit
    else
      [char] 0 plex-emit
      10 i - [char] 0 + plex-emit
    then
    wait-alarm  
  loop
   0 $ff -1 plexcolumn!
  14 $ff -1 plexcolumn!
  1 plexpos!
  [char] 0 dup plex-emit plex-emit 
  cr ." Launch!" cr
;

