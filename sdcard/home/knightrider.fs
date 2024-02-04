CR .( knightrider.fs loading ... )

: init-port ( -- )
  11 16 dmod   \ set A0/D16 to analog
  8 0 do
    0 i dpin!
    3 i dmod \ port is output
  loop
;

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
  switch1? until 
  0 0 dpin!
;

: knightrider-thread ( -- )
  osNewDataStack
  knigthrider
  osThreadExit
;

' knightrider-thread 0 0 osThreadNew
CR .( thread started with ID ) .

