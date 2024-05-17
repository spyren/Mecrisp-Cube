\   Filename: vis-init.fs
\    Purpose: Adds the VIS dictionary browser to Mecrisp-Stellaris
\        MCU: *
\      Board: * , tested with TI StellarisLaunchPad 
\       Core: Mecrisp-Stellaris by Matthias Koch.
\   Required: vis-0.8.4-core.fs for Mecrisp-Stellaris
\     Author: Manfred Mahlow          manfred.mahlow@forth-ev.de
\   Based on: vocs-0.7.0
\    Licence: GPLv3
\  Changelog: 2020-04-19 vis-0.8.2-??.txt --> vis-0.8.2-??.fs
\             200522 vis-0.8.4-??.fs  vid redefined  ?wid changed
\                                     .nid and core? new

\ Source Code Library for Mecrisp-Stellaris                           MM-190718
\ ------------------------------------------------------------------------------

  compiletoflash

  inside first definitions  decimal

: ?wid ( wid1 -- wid1|wid2 )
  dup root-wordlist   = if drop [ (' root   literal, ] exit then
  dup forth-wordlist  = if drop [ (' forth  literal, ] exit then
  dup inside-wordlist = if drop [ (' inside literal, ] exit then

;

: .wid ( wid -- ) ?wid .wid ;

: core? ( lfa -- f ) \ true if lfa is in the mecrisp core
  _sof_ @ u>= over forth-wordlist u< and
;

\ Given a words lfa print its name with allVOCabulary prefixes. If it's  a
\ word from the Stellaris core do not print the prefix forth.
: .nid ( lfa -- )  \ MM-200522
\  dup _sof_ @ u>= over forth-wordlist u< and if .id exit then
  dup core? if .id exit then
  0 swap
  begin
    dup lfa>wtag tag>wid
    dup forth-wordlist =
    if
      over dup forth-wordlist u>= swap root-wordlist u<= and if drop then -1
    else
      dup wid?
    then
  until
  over if dup forth-wordlist = if drop then then  \ no forth prefix  MM-200522
  begin
    dup
  while
    .wid     
  repeat
  drop
;

\ Given a wid of a VOCabulary print the VOCabulary name, given a wid of a
\ wordlist print the address.
: .vid ( wid -- )  \ MM-200522
  tag>wid
  dup wid? if .wid exit then
  .nid
;

root definitions

\ Display the search order, the compilation contex and the compilation mode.
: order ( -- ) 
  cr ." context: "
  _sop_ @ context =
  if  \ default search order
    context
    begin
      dup @ dup
    while
      .vid space cell+
    repeat
    2drop
  else  \ voc search order
    voc-context @
    begin
      dup .vid space vocnext dup 0=         \ MM-200102
    until
    drop root-wordlist .wid space
  then
  cr ." current: " current @ .vid space
  ." compileto" compiletoram? if ." ram" else ." flash" then
;


 inside definitions

: dashes ( +n -- ) 0 ?do [char] - emit loop ;

: ?words ( wid f -- )
\ If f = -1 do not list the mecrisp core words.
  if  ( wid )
    \ Show words in flash starting with FORTH-WORDLIST.
    dup forth-wordlist
  else  ( wid )
    \ Show words in flash starting with the first word in flash.
\   dup forth-wordlist over = if _sof_ else forth-wordlist then  \ fails MM-200102
    dup dup forth-wordlist = if _sof_ else forth-wordlist then   \ works
  then
  show-wordlist-in-flash 
  dup cr ." << FLASH: " .vid
  compiletoram?
  if
    cr dup ." >> RAM:   " .vid
    0 show-wordlist-in-ram
    cr 15 dashes
  else
     drop
  then
;

: \?? ( f -- ) 
    cr 15 dashes
    >r _sop_ @ @ ( lfa|wid ) 
    begin
      dup r@ ?words \ cr
      vocnext dup 0=
    until
    r> 2drop
;

  forth definitions

  : words ( -- ) 0 \?? cr ;

  root definitions

  : words ( -- ) words ;

  sticky : ?? ( -- )
  -1 \?? order ."   base: " base @ dup decimal u. base !  cr 2 spaces .s ;

  forth first definitions  decimal

\ Last Revision: MM-200522 vis-0.8.4-??.fs  vid redefined  ?wid changed 
\                          ?vid removed  .nid new
\                MM-200108 ?voc-words --> ?words

\ vis-0.8.2-also.txt    Source Code Library for Mecrisp-Stellaris      MM-191228
\ ------------------------------------------------------------------------------
  compiletoflash

  root definitions  decimal  inside first

  : only ( -- )
    [ root .. voc-context @ literal, forth .. voc-context @ literal, ] 
    dup 3 set-order  immediate ;

  : also ( -- ) 
    get-order dup #vocs = if ."  ? search order overflow " abort then
    over swap 1+ set-order  postpone first  immediate ;
\   over swap 1+ set-order [ ' first call, ] immediate ;

  : previous ( -- )
    get-order dup 1 = if ."  ? search order underflow " abort then
    nip 1- set-order immediate ;

  forth definitions  forth first

\ Last Revision: MM-200122

\ vis-0.8.2-vocs.fs     Source Code Library for Mecrisp-Stellaris     MM-191228
\ ------------------------------------------------------------------------------

inside first definitions  decimal

: ?voc ( lfa --) dup lfa>wtag 3 and 2 =  if space space .vid else drop then ;

forth definitions

\ Show all the VOCs that are actually defined in the dictionary.
: vocs ( -- )
  cr ." FLASH: "   \ show VOCs defined in FLASH
  forth-wordlist
  begin
    dup ?voc dictionarynext
  until
  drop
  compiletoram?
  if
    cr ."   RAM: "   \ show VOCs define in RAM
    dictionarystart ( lfa )
    begin
      dup _sof_ <>
    while
      dup ?voc dictionarynext if drop _sof_ then
    repeat
    drop
  then
  space
;

forth first

\ Last Revision: MM-200522 minor edit

\ vis-0.8.3-items.fs    Source Code Library for Mecrisp-Stellaris     MM-191228
\ ------------------------------------------------------------------------------

  compiletoflash

  inside first definitions  decimal

: ?item ( lfa --) dup lfa>wtag 3 and 1 = 
   if
      3 spaces
      dup lfa>ctag dup 1 = if drop ." sticky " else tag>wid .vid then 
      $08 emit ." : " .vid
   else
     drop
   then ;


  forth definitions

\ Show all context switching items which are actually defined in the dictionary.
: items ( -- )
  cr ." FLASH:"  \ show items defined in FLASH
  forth-wordlist
  begin
    dup ?item
    dictionarynext
  until
  drop
  compiletoram?
  if
    cr ."   RAM:"  \ show items defined in RAM
    dictionarystart ( lfa )
    begin
      dup _sof_ <>
    while
      dup ?item
      dictionarynext if drop _sof_ then
    repeat
    drop 
  then
  space
;

  forth first

\ Last Revision: MM-200522 ?vid --> .vid

compiletoram  \ EOF vis-0.8.4-mecrisp-stellaris.fs 
 
