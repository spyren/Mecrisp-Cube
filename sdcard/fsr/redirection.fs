\  @brief
\      Redirects emit and key to files and other devices
\
\      There are many Forth words to format a string e.g. emit, type or print
\      something to the console. It is also possible to fetch user input like 
\      key, accept, etc. All these words can be used on files and other devices. 
\      Nothing new for a UNIX user (stdin/stdout). Mecrisp use hooks for 
\      redirection terminal IO, these hooks can also be used for file 
\      redirection. 
\  @file
\      redirection.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2021-05-16
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\
\      This project Mecrsip-Cube is free software: you can redistribute it
\      and/or modify it under the terms of the GNU General Public License
\      as published by the Free Software Foundation, either version 3 of
\      the License, or (at your option) any later version.
\
\      Mecrsip-Cube is distributed in the hope that it will be useful, but
\      WITHOUT ANY WARRANTY; without even the implied warranty of
\      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
\      General Public License for more details.
\
\      You should have received a copy of the GNU General Public License
\      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.

CR .( redirection.fs loading ... )

\ file redirection
\ ****************

: >f_open ( addr c-addr -- n )  \ open a file to redirect to (emit, type, ...)
  swap dup stdout ! swap
  FA_WRITE FA_OPEN_ALWAYS + f_open
;

: >>f_open ( addr c-addr -- n )  \ open a file to redirect to (emit, type, ...). Append to file
  swap dup stdout ! swap
  FA_WRITE FA_OPEN_APPEND + f_open
;

: <f_open ( addr1 c-addr -- n )  \ open a file to redirect from (key, accept, ...)
  swap dup stdin ! swap
  FA_READ f_open
;

: >f_close ( -- n )
  stdout @ f_close
  0 stdout !
;

: <f_close ( -- n )
  stdin @ f_close
  0 stdin !
;

: >file ( -- addr1 addr2 )  \ redirect to a file (emit, type, ...)
  hook-emit @
  hook-emit? @
  ['] fs-emit hook-emit !
  ['] fs-emit? hook-emit? !
;


: <file ( -- addr1 addr2 )  \ redirection from a file (key, accept, ...)
  hook-key @
  hook-key? @
  ['] fs-key hook-key !
  ['] fs-key? hook-key? !
;


\ uart (serial)
\ *************

: >uart ( -- addr1 addr2 )  \ redirection to uart
  hook-emit @
  hook-emit? @
  ['] serial-emit hook-emit !
  ['] serial-emit? hook-emit? !
;

: <uart ( -- addr1 addr2 )  \ redirection from uart (key, accept, ...)
  hook-key @
  hook-key? @
  ['] serial-key hook-key !
  ['] serial-key? hook-key? !
;

: <>uart ( -- addr1 addr2 addr3 addr4 )  \ redirection from and to uart
  <uart
  >uart
;

\ cdc (USB serial)
\ ****************

: >cdc ( -- addr1 addr2 )  \ redirection to cdc
  hook-emit @
  hook-emit? @
  ['] cdc-emit hook-emit !
  ['] cdc-emit? hook-emit? !
;

: <cdc ( -- addr1 addr2 )  \ redirection from cdc (key, accept, ...)
  hook-key @
  hook-key? @
  ['] cdc-key hook-key !
  ['] cdc-key? hook-key? !
;

: <>cdc ( -- addr1 addr2 addr3 addr4 )  \ redirection from and to cdc
  <cdc
  >cdc
;


\ crs (BLE serial)
\ ****************

[IFDEF] crs-emit : >crs  hook-emit @  hook-emit? @  ['] crs-emit hook-emit !   ['] crs-emit? hook-emit? ! ; [THEN]
[IFDEF] crs-emit : <crs  hook-key @   hook-key? @   ['] crs-key hook-key !     ['] crs-key? hook-key? ! ;   [THEN]
[IFDEF] crs-emit : <>crs <crs >crs ; [THEN]

\ oled display
\ ************

: >oled ( -- addr1 addr2 )  \ redirection to oled
  hook-emit @
  hook-emit? @
  ['] oled-emit hook-emit !
  ['] oled-emit? hook-emit? !
;

\ plex display
\ ************

: >plex ( -- addr1 addr2 )  \ redirection to cplexrs
  hook-emit @
  hook-emit? @
  ['] plex-emit hook-emit !
  ['] plex-emit? hook-emit? !
;

\ terminate redirection
\ ********************

: <term ( addr1 addr2 -- )   \ terminate from redirection
  hook-key? !
  hook-key !
;

: >term ( addr1 addr2 -- )   \ terminate to redirection
  hook-emit? !
  hook-emit !
;

: <>term ( addr1 addr2 addr3 addr4 -- )  \ terminat redirection
  <term
  >term
;

