1 plexshutdown

: frame ( c- -- )
  15 0 do
    dup i + @  i swap 10 plexcolumn!
  loop
  drop
;

50 buffer: frame-buffer

0 plexpos!
char 4 plex-emit
1 plexcolumn@ frame-buffer !
2 plexcolumn@ frame-buffer 1 + !
3 plexcolumn@ frame-buffer 2 + !
4 plexcolumn@ frame-buffer 3 + !
5 plexcolumn@ frame-buffer 4 + !

0 plexpos!
char t plex-emit
1 plexcolumn@ frame-buffer 6 + !
2 plexcolumn@ frame-buffer 7 + !
3 plexcolumn@ frame-buffer 8 + !
4 plexcolumn@ frame-buffer 9 + !

0 plexpos!
char h plex-emit
1 plexcolumn@ frame-buffer 11 + !
2 plexcolumn@ frame-buffer 12 + !
3 plexcolumn@ frame-buffer 13 + !
4 plexcolumn@ frame-buffer 14 + !

plexclr
frame-buffer frame

