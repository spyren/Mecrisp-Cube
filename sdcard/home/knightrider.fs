CR .( knightrider.fs loading ... )

3 0 dmod   \ set D0 to Output
3 1 dmod   \ set D1 to Output
3 2 dmod   \ set D2 to Output
3 3 dmod   \ set D3 to Output
3 4 dmod   \ set D4 to Output
3 5 dmod   \ set D5 to Output
3 6 dmod   \ set D6 to Output
3 7 dmod   \ set D7 to Output

: left ( -- ) 
  7 0 do
    1 i dpin! 
    0 apin@ 10 / osDelay drop  \ delay depends on A0
    0 i dpin!
  loop 
;

: right ( -- )
  8 1 do  
    1 8 i - dpin! 
    0 apin@ 10 / osDelay drop  \ delay depends on A0
    0 8 i - dpin!
  loop 
;

: knigthrider ( -- )
  begin 
    left right 
    switch1? 
  until 
  0 0 dpin!
;

: knightrider-thread ( -- )
  osNewDataStack
  knigthrider
  osThreadExit
;

' knightrider-thread 0 0 osThreadNew
CR .( thread started with ID ) .

