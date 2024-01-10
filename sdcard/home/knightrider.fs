CR .( knightrider.fs loading ... )

\ Flipper Zero portmap
create port-map 6 , 0 , 1 , 9 , 13 , 10 , 12 , 11 ,

: pin ( n -- n )  \ gets the Dx pin number
  cells port-map + @
;

: init-port ( -- )
  11 16 dmod   \ set A0/D16 to analog
  8 0 do
    0 i pin dpin!
    3 i pin dmod \ port is output
  loop
;

: delay ( -- )
  0 apin@ 10 / 5 + osDelay drop  \ delay depends on A0
;

: left ( -- ) 
  7 0 do
    1 i pin dpin! 
    delay
    0 i pin dpin!
  loop 
;

: right ( -- )
  8 1 do  
    1 8 i - pin dpin! 
    delay
    0 8 i - pin dpin!
  loop 
;

: knigthrider ( -- )
  init-port
  begin 
    left right 
  switch1? until 
  0 0 pin dpin!
;


: knightrider-thread ( -- )
  osNewDataStack
  knigthrider
  osThreadExit
;

' knightrider-thread 0 0 osThreadNew
CR .( thread started with ID ) .

