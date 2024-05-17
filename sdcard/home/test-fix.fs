: test-fix ( -- n ) \ test 1000 times fixed-point sin return n in ms
  osKernelGetTickCount  cr
\ pi 2e f* 1000e f/  \ 2*pi/1000
  360,0 1000,0 x/
  cr
  1000 0 do
\   2dup i 0 swap x*      2drop
\   2dup i 0 swap x* sin  2drop
    2dup i 0 swap x* sqrt  2drop
\   i .  2dup i 0 swap x* sin x.   cr
\   i .  2dup i 0 swap x* sqrt x.   cr
\   i .  2dup i 0 swap x* sin hex. hex. cr
  loop
  2drop
  osKernelGetTickCount swap -
;
