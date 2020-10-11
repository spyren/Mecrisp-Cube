\ Conditional compilation
\ Idea similar to http://lars.nocrew.org/dpans/dpansa15.htm#A.15.6.2.2532
CR .( conditional.fs loading ... )

: [IF*]? ( addr len -- flag )
    2dup s" [IF]"     compare
 >r 2dup s" [IFDEF]"  compare r> or
 >r      s" [IFNDEF]" compare r> or
;

: [ENDIF*]? ( addr len -- flag )
    2dup s" [THEN]"   compare
 >r      s" [ENDIF]"  compare r> or
;

: [ELSE] ( -- )
  1 \ Initial level of nesting
  begin
    nexttoken ( level addr len )
    2dup upper

    2dup [IF*]?
    if
      2drop 1+  \ One more level of nesting
    else
      2dup s" [ELSE]" compare
      if
        2drop 1- dup if 1+ then  \ Finished if [ELSE] is reached in level 1. Skip [ELSE] branch otherwise.
      else
        [ENDIF*]? if 1- then  \ Level completed.
      then
    then

    ?dup 0=
  until

  immediate 0-foldable
;

: [THEN]  ( -- ) immediate 0-foldable ;

: [ENDIF]  ( -- ) immediate 0-foldable ;

: [IF]  ( flag | flag “〈spaces〉name . . . ” –– )   
  0= if 
    postpone [ELSE] 
  then 
  immediate 1-foldable 
;

: [IFDEF]  ( –– )
  token find drop 
  0= if 
    postpone [ELSE] 
  then 
  immediate 0-foldable 
;

: [IFNDEF] ( -- ) 
  token find drop 
  0<> if 
    postpone [ELSE] 
  then 
  immediate 0-foldable 
;

: [UNDEFINED] ( "<spaces>name ..." -- flag ) 
  token find drop 
  0= 
  immediate 
;

: [DEFINED]   ( "<spaces>name ..." -- flag ) 
  token find drop 
  0<> 
  immediate 
;
