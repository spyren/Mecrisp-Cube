\ list and load for blocks
CR .( blocks.fs loading ... )

: list ( n -- )
  dup cr ." Block# " . cr
  block
  16 0 do
    i 3 .r dup 64 type cr
    64 +
  loop
  drop
;

: load ( n -- )
  block
  16 0 do
    dup 64 evaluate 
    64 +
  loop
  drop
;

