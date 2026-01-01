\  @brief
\      Extend listing words
\  @file
\      words.fs
\  @author
\      t.porter <terry@tjporter.com.au>: words4
\      Peter Schmid, peter@spyr.ch
\  @date
\      2025-05-19
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\      For details see copyright.txt

CR .( words.fs loading ... )

: words4 ( -- )  \ A columnar Word list printer. Width = 20 characters, handles overlength Words neatly
  cr
  0                         \ column counter
  dictionarystart
    begin
      dup 6 + dup
      ctype \ dup before 6 is for dictionarynext input
      count nip   \ get number of characters in the word and drop the address of the word
      20 swap - dup 0 > if  \ if Word is less than 20 chars
        spaces swap         \ pad with spaces to equal 20 chars
      else drop cr          \ issue immediate carriage return and drop negative number
        nip -1              \ and reset to column -1
      then
      dup 3 = if
        3 - cr              \ if at 4th column, zero column counter
      else
        1 +
      then
      swap
      dictionarynext       \ ( a-addr - - a-addr flag )
    until
  2drop
;

100 buffer: word-buffer
50  buffer: match-buffer
0 variable word-len
0 variable match-len
 
: mwords ( "str" -- )  \ list all words that match a parsed string str
  bl word count dup match-len ! 
  match-buffer swap cmove 
  match-buffer match-len @ upper
  cr
  dictionarystart
  begin
    dup 6 +  ( dict-adr counted-word ) 
    count dup word-len !  word-buffer swap cmove
    word-buffer word-len @ 2dup upper 
    match-buffer match-len @ 
    search nip nip if
      \ print word
      dup 6 + ctype cr
    then
    dictionarynext
  until
  drop
;


