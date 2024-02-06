1 plexshutdown

200 buffer: message
message .str" 4th"
message strlen 4th
lcdclr  0 lcdfont 
>lcd 2swap type >term  \ write string to lcd

plexclr
\ 4
1  0 lcdpos! lcdcolumn@  0 swap 10 plexcolumn!
2  0 lcdpos! lcdcolumn@  1 swap 10 plexcolumn!
3  0 lcdpos! lcdcolumn@  2 swap 10 plexcolumn!
4  0 lcdpos! lcdcolumn@  3 swap 10 plexcolumn!
5  0 lcdpos! lcdcolumn@  4 swap 10 plexcolumn!
                         5 0    10 plexcolumn!
\ t
7  0 lcdpos! lcdcolumn@  6 swap 10 plexcolumn!
8  0 lcdpos! lcdcolumn@  7 swap 10 plexcolumn!
9  0 lcdpos! lcdcolumn@  8 swap 10 plexcolumn!
10 0 lcdpos! lcdcolumn@  9 swap 10 plexcolumn!
                        10 0    10 plexcolumn!
\ h
13 0 lcdpos! lcdcolumn@ 11 swap 10 plexcolumn!
14 0 lcdpos! lcdcolumn@ 12 swap 10 plexcolumn!
15 0 lcdpos! lcdcolumn@ 13 swap 10 plexcolumn!
16 0 lcdpos! lcdcolumn@ 14 swap 10 plexcolumn!

