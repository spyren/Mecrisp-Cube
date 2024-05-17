\   Filename: wordlists.fs
\    Purpose: Adds WORDLISTs to Mecrisp-Stellaris RA
\        MCU: 
\      Board: * , tested with TI StellarisLaunchPad 
\       Core: Mecrisp-Stellaris by Matthias Koch.
\   Required: Mecrisp-Stellaris RA 2.3.8 or later
\     Author: Manfred Mahlow          manfred.mahlow@forth-ev.de
\   Based on: vocs-0.7.0
\    Licence: GPLv3
\  Changelog: 2020-04-19 wordlists-0.8.2.txt --> wordlists-0.8.3.fs
\             2020-05-20 smudge? changed
\             2020-05-22 wordlist changed  wid? added

\ Wordlists for Mecrisp-Stellaris                                      MM-170504
\ ------------------------------------------------------------------------------

\             Copyright (C) 2017-2020 Manfred Mahlow @ forth-ev.de

\        This is free software under the GNU General Public License v3.
\ ------------------------------------------------------------------------------
\ This is an implementation of a subset of words from the Forth Search-Order
\ word set.

\ ** This file must be loaded only once after a RESET (the dictionary in RAM 
\    must be empty) and before any new defining word is added to Mecrisp-
\    Stellaris. It is and needs to be compiled to FLASH.

\ ** Requires 

\    Mecrisp-Stellaris  2.3.6-hook-find  or  2.3.8-ra  or a later version with
\    hook-find.

     ' hook-find drop

\ ** Recommends

\    Terminalprogram: e4thcom -t mecrisp-st -b B115200


\ * The Forth Search-Order and three wordlists are added:
\
\   FORTH-WORDLIST
\
\       \WORDS          ( -- )
\       FORTH-WORDLIST  ( -- wid )
\       INSIDE-WORDLIST ( -- wid )
\       ROOT-WORDLIST   ( -- wid )
\       WORDLIST        ( -- wid )
\       SHOW-WORDLIST   ( wid -- )
\       GET-ORDER       ( -- wid1 ... widn n )  
\       SET-ORDER       ( wid1 ... widn n | -1 -- )
\       SET-CURRENT     ( wid -- )
\       GET-CURRENT     ( -- wid )
\
\   ROOT-WORDLIST
\
\       INIT            ( -- )
\       WORDS           ( -- )
\       ORDER           ( -- )
\
\   INSIDE-WORDLIST
\   holds words needed for the implementation but normally not required for
\   applications.
\
\
\ * The default search order is FORTH-WORDLIST FORTH-WORDLIST ROOT-WORDLIST.
\
\ * The search order can be changed with GET-ORDER and SET-ORDER.
\
\ * Dictionary searching is done by the new word FIND-IN-DICTIONARY (defined in
\   the INSIDE-WORDLIST). It is called via HOOK-FIND by the now vectored Mecrisp
\   word FIND .
\
\ * New words are added to the FORTH-WORDLIST by default. This can be changed
\   by setting a new compilation context with <wordlist> SET-CURRENT.
\
\ * Compiling to FLASH and RAM is supported. 
\
\ * The curious may take a look at the implementation notes at the end of this
\   file.
\
\ Some usage examples:
\
\   WORDLIST constant <name>  Creates an empty wordlist and assigns its wid to
\                             a constant.
\
\   <name> SHOW-WORDLIST      Lists all words of the wordlist <name>.
\
\   GET-ORDER NIP <name> SWAP SET-ORDER
\
\                             Overwrites the top of the search order.
\
\   <name> SET-CURRENT        Overwrites the compilation wordlist.
\
\   WORDS                     Lists all words of the top of the search order.
\                             ( initially this is the FORTH-WORDLIST )
\
\   \WORDS                    Alias for the word WORDS defined in the Mecrisp
\                             core. Ignores all wordlist related information. 
\                             Might be useful in special debuging situations.
\
\   INIT                      Initialisation of the wordlists extension.
\
\ ------------------------------------------------------------------------------

  compiletoflash      \ This extension must be compiled in flash.

  hex

\ An alias for WORD in the Mecrisp Stellaris core.
: \words ( -- ) words ;


\ Three wordlists are implemented now, all as members of the forth-wordlist:

   align 0 , here cell+ ,     here constant forth-wordlist

   align 0 , forth-wordlist , here constant inside-wordlist

   align 0 , forth-wordlist , here constant root-wordlist


  forth-wordlist ,
\ Return a wordlist identifier for a new empty wordlist.
: wordlist ( -- wid )
\ align here 0 ,                               MM-200522
  align here [ here @ not literal, ] ,
;

  inside-wordlist ,
\ Return true if lfa|wid is the wid of a wordlist.     MM-200522
: wid? ( lfa|wid -- f )
  @ [ here @ not literal, ] =
;

  inside-wordlist ,
\ lfa of the first word in flash
  dictionarystart constant _sof_


\ We need two buffers for the FORTH search order:

  inside-wordlist ,
  #6 constant #vocs 

\ A buffer for the search order in compiletoflash mode.
  inside-wordlist ,
  #vocs 1+ cells buffer: c2f-context


\ A buffer for the search order in compiletoram mode.
  inside-wordlist ,
  #vocs 1+ cells buffer: c2r-context

  inside-wordlist ,
\ Return the addr of the actual search order depending on the compile mode.
: context ( -- a-addr )
  compiletoram? if c2r-context else c2f-context then
;


  inside-wordlist ,
\ Current for the compiletoflash mode.
  forth-wordlist variable c2f-current

  inside-wordlist ,
\ Current for the compiletoram mode.
  forth-wordlist variable c2r-current

  inside-wordlist ,
\ Current depending on the compile mode.
: current ( -- a-addr )
  compiletoram? if c2r-current else c2f-current then
;


  inside-wordlist ,
\ A buffer to register a context switching request.
  0 variable _csr_

  inside-wordlist ,
\ A buffer for some flags in a wordlist tag (wtag).
  0 variable wflags


  inside-wordlist ,
\ A flag, true for searching the dictionary with context switching support,
\ false for searching the compilation context only without context switching.
  -1 variable _indic_ 


  inside-wordlist ,
\ Compile a wordlist tag ( wtag).
: wtag, ( -- )
  align current @ wflags @ or ,  0 wflags !  0 _indic_ !
;


  inside-wordlist ,
: lfa>flags ( a-addr1 -- a-addr2 ) cell+ ;

  inside-wordlist ,
: lfa>nfa ( a-addr -- cstr-addr ) lfa>flags 2+ ;

  inside-wordlist ,
: lfa>xt ( a-addr -- xt ) lfa>nfa skipstring ;

  inside-wordlist ,
: lfa>wtag ( a-addr -- wtag ) [ 1 cells literal, ] - @ ;

  inside-wordlist ,
: lfa>ctag ( a-addr -- ctag ) [ 2 cells literal, ] - @ ;

  inside-wordlist ,
: tag>wid ( wtag -- wid ) [ 1 cells 1- not literal, ] and ;

  inside-wordlist ,
: lfa>xt,flags ( a-addr -- xt|0 flags )
  dup if dup lfa>xt swap lfa>flags h@ else dup then
;


\ Tools to display wordlists:

  inside-wordlist ,
: .id ( lfa -- )
  lfa>nfa count type space
;

  inside-wordlist ,
: .wid ( lfa|wid -- )
\ dup @ if ( wid = lfa ) .id else u. then
  dup wid? if u. else .id then
;


  inside-wordlist ,
\ Print some word header information.
: .header ( lfa -- )
  ." lfa: " dup hex. dup ." xt: " lfa>xt hex.    \ print lfa and xt
  ." name: " lfa>nfa count type space            \ print name
;


  inside-wordlist ,
: .wtag ( wtag -- ) ." wtag: " hex. ;

  inside-wordlist ,
: .ctag ( ctag -- ) ." ctag: " hex. ;


  inside-wordlist ,
\ Return true if the word at lfa is smudged.
: smudged? ( lfa -- flag )
\ cell+ h@ FFFF <>
  cell+ h@ [ here @ FFFFFFFF = FFFF and literal, ] <>   \ MM-200520
;


  inside-wordlist ,
\ Show the word at lfa if its a member of the wordlist wid.
: show-wordlist-item ( lfa wid -- )
  >r dup forth-wordlist >=   \ tagged word ?
  if ( lfa )
    dup lfa>wtag tag>wid r@ =     \ wid(lfa) = wid
    if ( lfa )
     \ long version
       cr dup lfa>wtag dup 1 and if over lfa>ctag .ctag else #15 spaces then
       .wtag .header
     \ short
       \ .id space
    else
      drop
    then
  else ( lfa )
   \ long
     cr #30 spaces .header
   \ short
     \ .id space
  then
  r> drop
;


  inside-wordlist ,
: show-wordlist-item ( lfa wid -- )                        \ MM-200520
  over smudged? if show-wordlist-item else 2drop then
;
  


  inside-wordlist ,
: show-wordlist-in-ram ( wid lfa -- )
  drop ( wid ) >r
  dictionarystart ( lfa )
  begin
    dup _sof_ <>
  while
    dup r@ show-wordlist-item
    dictionarynext if drop _sof_ then
  repeat
  r> 2drop ;


  inside-wordlist ,
\ : show-wordlist-in-flash ( wid lfa -- )
: show-wordlist-in-flash ( wid lfa -- )
\ List all words of wordlist wid (defined in flash) starting with word at lfa.
\ lfa must be in flash
\ forth-wordlist dictionarystart c2f-... lists all forth-wordlist words
\ forth-wordlist dup             c2f-... lists forth-wordlist words starting
\                                        with lfa(forth-wordlist)
\ wid dup                        c2f-... lists all words of wordlist wid
  swap >r  ( lfa ) ( R: wid )
  begin ( lfa )
    dup r@ show-wordlist-item
    dictionarynext 
  until
  r> 2drop
;


  forth-wordlist ,
\ Show all words of the wordlist wid.
: show-wordlist ( wid -- )
  dup forth-wordlist =
  if dup _sof_ else dup forth-wordlist then show-wordlist-in-flash
  compiletoram? if 0 show-wordlist-in-ram else drop then
;

\ End of Tools to display wordlists.


  inside-wordlist ,
\ Return true if name of word at lfa equals c-addr,u and word is smudged.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: name? ( c-addr u lfa -- c-addr u lfa flag )
  >r 2dup r@ lfa>nfa count compare r> swap
  if  ( c-addr u lfa )  \ name = string c-addr,u
      dup smudged?
  else  \ name <> string c-addr,u
    false
  then
;


  inside-wordlist ,
\ If the word with name c-addr,u is a member of wordlist wid, return its lfa.
\ Otherwise return zero.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: search-wordlist-in-ram ( c-addr u wid -- lfa|0 )
  >r dictionarystart 
  begin ( c-addr u lfa )
    dup _sof_ <> 
  while ( c-addr u lfa )
    dup lfa>wtag tag>wid r@ =        \ wid(lfa) = wid ?
    if
      name?   ( c-addr u lfa flag )
      if
        nip nip r> drop exit
      then
    then
    dictionarynext drop
  repeat 
  ( c-addr u lfa ) 2drop r> ( c-addr wid ) 2drop 0
;


  inside-wordlist ,
\ If the word with name c-addr,u is a member of wordlist wid, return its lfa.
\ Otherwise return zero.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: search-wordlist-in-flash ( c-addr u wid -- lfa|0 )
  dup 0 >r >r forth-wordlist = if _sof_ else forth-wordlist @  then
   begin
    dup forth-wordlist >           \ tagged word ?
    if
      dup lfa>wtag tag>wid r@ =    \ wid(lfa) = wid ?
      if
        name?
        if
          r> r> drop over >r >r         \ R: wid lfa.found
          \ for debugginng only :
          \ cr dup lfa>wtag .wtag dup .header  \ print wtag(lfa) and name(lfa)
          \
        then
      then
    else ( c-addr u lfa )
      name?
      if 
        r> r> drop over >r >r           \ R: wid lfa.found
        \ for debugginng only :
        \ cr 9 spaces dup .header
        \
      then
    then
    dictionarynext
  until
  ( c-addr u lfa ) 2drop r> ( c-addr wid ) 2drop
  r> ( lfa|0 )
;


  inside-wordlist , 
\ If the word with name c-addr,u is a member of wordlist wid, return its lfa.
\ Otherwise return zero.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: search-in-wordlist ( c-addr u wid -- lfa|0 )
  >r compiletoram?
  if
    2dup r@ search-wordlist-in-ram ?dup if nip nip r> drop exit then
  then
  r> search-wordlist-in-flash
;


  inside-wordlist ,
\ Search the word with name c-addr,u in the search order at a-addr. If found
\ return the words lfa otherwise retun 0.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: search-in-order ( c-addr u a-addr -- lfa|0 )
  dup >r ( c-addr u a-addr )  \ a-addr = top of the search order
  begin
    @ ( c-addr u wid|0 ) dup
  while
    >r 2dup r> search-in-wordlist dup
    if nip nip r> drop exit then drop
    r> cell+ dup >r
  repeat
  r> 2drop 2drop 0
;


  inside-wordlist ,
\ Search the dictionary for the word with the name c-addr,u. Return xt and flags
\ if found, 0 and invalid flags otherwise.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: find-in-dictionary ( c-addr u -- xt|0 flags )
  context search-in-order ( lfa ) lfa>xt,flags
;


  inside-wordlist ,
\ Return the number of wordlists (wids) in the search order.
: w/o ( -- wid1 ... widn n )
  0 context begin dup @ while swap 1+ swap cell+ repeat drop
; 

  forth-wordlist ,
: get-order ( -- wid1 ... widn n )
  w/o dup 0= if exit then 
  dup >r cells context +
  begin 1 cells - dup @ swap dup context = until drop r>
;

  forth-wordlist ,
: set-order ( wid1 ... widn n | -1 )
   dup #vocs > if ." order overflow" cr quit then
   dup -1 = if drop root-wordlist forth-wordlist dup 3 then
   dup >r 0 ?do i cells context + ! loop
   0 r> cells context + !  \ zero terminated order
;


  forth-wordlist ,
: set-current ( wid -- ) current ! ;

  forth-wordlist ,
: get-current ( -- wid ) current @ ;


  inside-wordlist ,
: ?words ( wid f -- )
\ If f = -1 do not list the mecrisp core words.
\  context @ 
\  swap if
   if ( wid )
    \ Show words in flash starting with FORTH-WORDLIST.
    dup forth-wordlist
  else
    \ Show words in flash starting with the first word in flash.
\   dup forth-wordlist over = if _sof_ else forth-wordlist then  \ fails MM-200102
    dup dup forth-wordlist = if _sof_ else forth-wordlist then   \ works
  then
  show-wordlist-in-flash
  dup cr ." << FLASH: " .wid
  compiletoram?
  if
    cr dup ." >> RAM:   " .wid 
    0 show-wordlist-in-ram
  else
     drop
  then
;

  forth-wordlist ,
: words ( -- ) context @ 0 ?words ;

  root-wordlist ,
  : words ( -- ) words ;

  root-wordlist ,
\ Display the search order and the compilation context.
: order ( -- ) 
  cr ." context: " get-order 0 ?do .wid space loop
  cr ." current: " current @ .wid
  ."  compileto" compiletoram? if ." ram" else ." flash" then
;


\ We have to redefine all defining words of the Mecrisp Core to make them add
\ a wordlist tag when creating a new word:
\ ------------------------------------------------------------------------------
  forth-wordlist ,
  : : ( "name" -- ) wtag, : ;

  forth-wordlist set-current

: constant  wtag, constant ;
: 2constant wtag, 2constant ;

: variable  wtag, variable ;
: 2variable wtag, 2variable ;
: nvariable wtag, nvariable ;

: buffer: wtag, buffer: ;

: (create) wtag, (create) ;
: create   wtag, create ;
: <builds  wtag, <builds ;
\ ------------------------------------------------------------------------------

: abort ( -- ) cr quit ;  \ required for e4thcom error detection


inside-wordlist set-current  \ MM-200419

: wlst-init ( -- )
  compiletoflash -1 set-order  compiletoram -1 set-order  \ init both orders
  ['] find-in-dictionary hook-find !
;

root-wordlist set-current   \ MM-191228

\ MM-200419   (C) message only on reset

\ Finally we have to redefine INIT to set HOOK-FIND to call FIND-IN-DICTIONARY.
: init ( -- )
  hook-find @ ['] find-in-dictionary <
  if
    ."   * Wordlist Extension 0.8.3 for Mecrisp-Stellaris by Manfred Mahlow" cr
  then
  wlst-init
; 

decimal
init   \ Init the wordlist extension.

\ ------------------------------------------------------------------------------
\ Last Revision: MM-200522 0.8.4 : wordlist changed  wid? added  .wid changed
\                MM-200520 smudge? changed
\                MM-200419 0.8.3
\                          init changed to only display (C) message on reset
\                MM-200122 Final review of release 0.8.2.
\
\                MM-170604 First test implementation (feasibilty test)

\ ------------------------------------------------------------------------------
\ Implementation Notes:
\ ------------------------------------------------------------------------------
\ The code was created with Mecrisp-Stellaris 2.3.6 lm4f120 and tm4c1294 and
\ finally tested with Mecrisp-Stellaris 2.5.0 lm4f120-ra, msp432p401r-ra and
\ tm4c1294-ra. 

\ Wordlists are not implemented as separate linked lists but by tagging words
\ with a wordlist identifier (wid). The tags are evaluated to find a word in a 
\ specific wordlist. This idea was taken from noForth V. 

\ The main difference to noForth is, that not all words are tagged but only 
\ those, created after loading this extension. So only one minor change of the
\ Mecrisp-Stellaris Core was required: FIND had to be vectored (via HOOK-FIND).

\ A look at the Mecrisp-Stellaris dictionary structure shows, that a list entry
\ (a word) can be prefixed with the wid of the wordlist, the word belongs to.
\ This is what is done in this implementation.
\ ------------------------------------------------------------------------------

\ Address: 00004000 Link: 00004020 Flags: 00000081 Code: 0000400E Name: current
\ Address: 00004020 Link: 0000404C Flags: 00000000 Code: 00004030 Name: variable
\ Address: 0000404C Link: FFFFFFFF Flags: 00000000 Code: 0000405A Name: xt>nfa

\ 0404C         | Address (lfa) , holds the address of the next word or -1
\               |
\               |
\               |
\ cell+ = 04050 | Flags, 2 bytes    = lfa>flags
\         04051 |
\         04052 : 06     Name (nfa) = lfa>nfa
\         04053 : x
\               : t
\               : >
\               : n
\               : f
\               : a
\         04059 : 0    alignment
\ 405A          : Code (xt)         = lfa>xt = lfa>nfa skipstring

\ ------------------------------------------------------------------------------

\ After loading wordlists.txt all new words are prefixed/tagged with a wordlist-
\ tag ( wtag ).

\ wtag = wid || wflags

\  wid = identifier of the wordlist, the word belongs to

\  wflags = the 1 cells 2 / lowest bits of a wtag

\  we are only using Bit0 here (to be 16 Bit compatibel)
