\ Large letter F

: star ( -- )
  [char] * emit 
;

: stars ( u -- )  
  0 do  
    star  
  loop
;

: margin ( -- )
  cr 30 spaces
;

: blip ( -- )
  margin star 
;

: bar ( -- )
  margin 5 stars
;

: F ( -- )
  bar
  blip 
  bar 
  blip 
  blip
  cr 
;

F
