: LCD>plex ( u -- ) \ copy LCD from column u to plex
  15 0 do \ write 15 charlie columns
    dup i + dup 126 mod swap 126 /  ( u -- u x y)
    lcdpos! lcdcolumn@  \ read lcd column
    i swap 50 plexcolumn! \ write PLEX column
  loop
  drop
;

: Marquee ( c- u -- ) \ marquee a string on charlie plex
  lcdclr  0 lcdfont 
  2dup >lcd 2swap type >term  \ write string to lcd
  nip ( c- u -- u )
  3 - \ remove trailing spaces
  begin
    dup 6 *  0 do \ all string columns, a char is 6 pixels wide
      i LCD>plex
      40 osDelay drop
      switch1? if leave then
    loop 
  switch1? until
  drop ( u -- )
;

1 plexshutdown

200 buffer: message
message .str"    MECRISP-CUBE REAL-TIME FORTH ON THE GO!   "
message strlen Marquee

