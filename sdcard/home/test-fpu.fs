: test-fpu ( -- n ) \ test 1000 times sin return n in ms
  osKernelGetTickCount  cr
  pi 2e f* 1000e f/  \ 2*pi/1000
  cr
  1000 0 do
\   dup i s>f f*      drop
    dup i s>f f* fsin drop
\   i .  dup i s>f f* fsin fs.   cr
\   i .  dup i s>f f* fsin hex.  cr
  loop
  drop
  osKernelGetTickCount swap -
;
