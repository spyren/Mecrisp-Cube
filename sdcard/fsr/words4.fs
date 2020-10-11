\ Program Name: words4.fs
\ This program may require preloaded support files
\ Date: Thu 25 Jul 2019 23:27:35 AEST
\ Copyright 2018  t.porter <terry@tjporter.com.au>, licensed under the GPL
\ For Mecrisp-Stellaris by Matthias Koch.
\ https://sourceforge.net/projects/mecrisp/
\ 140 bytes

CR .( words4.fs loading ... )

: words4 ( -- )  \ A columnar Word list printer. Width = 20 characters, handles overlength Words neatly 
  cr
  0                         \ column counter
  dictionarystart
    begin
      dup 6 + dup
      ctype \ dup before 6 is for dictionarynext input
      count nip		    \ get number of characters in the word and drop the address of the word
      20 swap - dup 0 > if  \ if Word is less than 20 chars
        spaces swap         \ pad with spaces to equal 20 chars
      else drop cr	    \ issue immediate carriage return and drop negative number
        nip -1              \ and reset to column -1
      then		       
      dup 3 = if
        3 - cr              \ if at 4th column, zero column counter			   
      else
        1 +
      then
      swap		       
      dictionarynext        \ ( a-addr - - a-addr flag )
    until
  2drop
;
