\ tools for fat filesystem
CR .( fat.fs loading ... )

: 0: (  --  )
  s0" 0:" drop f_chdrive if
    ." invalid drive"
  then
;


: 1: (  --  )
  s0" 1:" drop f_chdrive if
    ." invalid drive"
  then
;

