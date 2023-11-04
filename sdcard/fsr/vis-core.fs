\   Filename: vis-core.fs
\    Purpose: Adds VOCs, ITEMs and STICKY Words to Mecrisp-Stellaris
\        MCU: *
\      Board: * , tested with TI StellarisLaunchPad 
\       Core: Mecrisp-Stellaris by Matthias Koch.
\   Required: wordlists-0.8.4.fs for Mecrisp-Stellaris
\     Author: Manfred Mahlow          manfred.mahlow@forth-ev.de
\   Based on: vocs-0.7.0
\    Licence: GPLv3
\  Changelog: 2020-04-19 vis-0.8.2-core.txt --> vis-0.8.3-core.fs
\             2020-05-22 vis-0.8.4-core.fs  minor changes

\ Source Code Library for Mecrisp-Stellaris                           MM-170628
\ ------------------------------------------------------------------------------
\              Vocabulary Prefixes ( VOCs ) for Mecrisp-Stellaris
\
\              Copyright (C) 2017-2020 Manfred Mahlow @ forth-ev.de
\
\        This is free software under the GNU General Public License v3.
\ ------------------------------------------------------------------------------
\ Vocabulary prefixes ( VOCs ) help to structure the dictionary, make it more 
\ readable and can reduce the code size because of shorter word names.
\
\ Like VOCABULARYs VOCs are context switching words. While a vocabulary changes
\ the search order permanently, a VOC changes it only temporarily until the next
\ word from the input stream is interpreted. VOCs are immediate words.
\
\ VOCABULARYs and VOCs are words for explicit context switching.
\
\ This extension also supports implicit context switching ( see the words ITEM
\ and STICKY ) and (single) inheritanc for VOCs.

\ Implicit Context Switching:

\ Implicit context switching means that a "normal" Forth word is tagged with
\ the wordlist identfier (wid) of a VOC. When Forths outer interpreter FINDs
\ such a word, it is executed or compiled as normal (depending on STATE) and
\ the VOCs search order is set as the new search context. The next word from 
\ the imput stream is then found in this context and afterwards the search
\ context is reset to the "normal" Forth search order.

\ Inheritance:

\ Inheritance means that a new VOC can inherit from (can extend) an existing
\ one. The search order of the new VOC is then the VOCs wordlist plus the 
\ inherited VOCs search order.

\ So VOCs can be used to create libraries, register identifiers, data types
\ and to define classes for objects with early binding methods and (single)
\ inheritance.

\ Give it a try and you will find that VOCs are an easy to use and powerful
\ tool to write well factored code and code modules.

\ Glossary:

\ voc ( "name" -- )  Create a vocabulary prefix that extends the voc root.

\ <voc> voc ( "name" -- )  Create a vocabulary prefix that extends (inherits 
\                          from) the given voc.

\ <voc> ?? ( -- )   Show all words of the actual VOC search order and stay in
\                   that VOC context.

\ .. ( -- )  Switch back from a VOC search order to the default Forth search
\            order.

\ <voc> definitions ( -- )  Make <voc> the current compilation context.

\ <voc> item ( -- )  Make the next created word a context switching one, i.e. 
\                    #123 int item variable int1  \ int1 ( -- a ; NS: int )

\ sticky ( -- )  Make the next created word a sticky one.

\ @voc ( -- )  Make the current compilation context the actual search context.

\ init ( -- )  Initialize the VOC extension.

\ ------------------------------------------------------------------------------

\ This extension must be compiled in FLASH.

compiletoflash

root-wordlist set-current  hex

: vis ( -- ) ." 0.8.4" ;

\ inside-wordlist first
get-order nip inside-wordlist swap set-order

inside-wordlist set-current


\ VOC context pointer for the compiletoflash mode.
  root-wordlist variable c2f-voc-context


\ VOC context pointer for the compiletoram mode.
  root-wordlist variable c2r-voc-context


\ VOC context pointer
: voc-context ( -- a-addr )
  compiletoram? if c2r-voc-context else c2f-voc-context then ;


\ Now that we have two context pointers ( context and voc-context ) representing
\ two search orders in two compile modes , we need a global search order pointer
\ too.


\ Search order pointer for the compiletoflash mode.
  c2f-context variable _c2f-sop_


\ Search order pointer for the compiletoram mode.
  c2r-context variable _c2r-sop_


\ Return the addr of the search oder pointer depending on the compile mode.  
  : _sop_ ( -- a-addr ) compiletoram? if _c2r-sop_ else _c2f-sop_ then ; 


\ Save the context switching request of the word with address lfa.
: _!csr_ ( lfa -- )
    0 _csr_ ! dup
    if ( lfa ) \ found
      \ *** for context switching debugging only
      \ ."  found: " dup .header 
      ( lfa ) dup forth-wordlist >   \ tagged word ?
      if ( lfa ) 
        dup lfa>wtag ( lfa wtag ) 1 and   \ word with ctag ?
        if dup lfa>ctag ( csr ) _csr_ ! then ( lfa )
      then ( lfa )
    then ( lfa )
    drop 
;


\ Process the last saved context switching request. 
: _?csr_ ( -- )
    \ *** for context switching debugging only
    \ cr _csr_ @ ." _csr_=" .
    \ postponed context switch request ? reset postpone flag csr.0 and exit
    _csr_ @ dup 1 and if -2 and _csr_ ! exit then ( csr|0 ) dup
    if ( csr )  \ context switching requested
       voc-context !  0 _csr_ !  voc-context
    else ( 0 )
      drop context
    then
    _sop_ !
;


\ Given a VOCs wid return the wid of the parent voc or zero if no voc was
\ inherited.
\ : vocnext ( wid1 -- wid2|0 ) [ 2 cells literal, ] - @ ;

\ MM-200103 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1
\ Given a vocs wid return the wid of the parent voc or zero if no voc was
\ inherited. Return 0 if wid is the wid of a wordlist.
  : vocnext ( wid1 -- wid2|0 )
    dup @ if [ 2 cells literal, ] - @ else dup - then ;


\ Search the VOCs search order (voc-context) at a-addr for a match with the
\ string c-addr,len. If found return the address (lfa) of the dictionary entry,
\ otherwise return 0.
: search-in-vocs ( c-addr len a-addr -- lfa|0 )
  @
  begin
   \ dup .
   >r 2dup r@ search-in-wordlist dup if nip nip r> drop exit then drop
   r> vocnext dup 0= 
  until
  drop root-wordlist search-in-wordlist
;

\ ------- MM-200110 -----------

\ The search-in-order defined in wordlists only searches the top wordlist of
\ every search order entry. To seach all wordlists except root of every entry
\ search-in-order must be overwriten as follows:

: search-in-vocs-without-root ( c-addr len a-addr -- lfa|0 )
\ @     MM-200102  !!!!!!!!!!!!
  begin
   \ dup .
   >r 2dup r@ search-in-wordlist dup if nip nip r> drop exit then drop
   r> vocnext dup 0= 
  until
  nip nip
;

\ from wordlists.txt

\ Search the word with name c-addr,u in the search order at a-addr. If found
\ return the words lfa otherwise retun 0.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: search-in-order ( c-addr u a-addr -- lfa|0 )
  dup >r ( c-addr u a-addr )  \ a-addr = top of the search order
  begin
    @ ( c-addr u wid|0 ) dup
  while
\    >r 2dup r> search-in-wordlist dup      \ old search  MM-200101
\ -- new search
    \ dup .id                                                     
    >r 2dup r> dup root-wordlist =
    if \ ."  swl "
      search-in-wordlist
    else \ ."  svoc "
      search-in-vocs-without-root
    then
    dup 
\ --
    if nip nip r> drop exit then drop
    r> cell+ dup >r
  repeat
  r> 2drop 2drop 0
;

\ ------------------------------

\ Search the dictionary for a match with the given string. If found return the
\ lfa of the dictionary entry, otherwise return 0.
\ Note: Based on mecrisps case insensitive non-ANS compare  !!!!!
: search-in-dictionary ( c-addr len -- lfa|0 )
  _sop_ @ dup context =
  if   \ ."  in order "  dup @ .id    \ MM-191226
    search-in-order
  else  \ ."  in voc "  dup @ .id
    search-in-vocs
  then
;

\ Search the dictionary for a match with the given string. If found return the 
\ xt and the flags of the dictionary entry, otherwise return 0 and flags.
\ Note: Based on mecrisps case insensitive non-ANS compare.
: vocs-find ( c-addr len -- xt|0 flags )
  _indic_ @ 
  if
    _?csr_
    \  ." voc-find,_indic_ = -1 "              \ *** for debugging only
    search-in-dictionary ( lfa )
    dup _!csr_
    \  ."  csr=" _csr_ @ . cr     \ *** for debugging only
  else ( lfa )
    \ ." voc-find,_indic_ = 0 "              \ *** for debugging only
    -1 _indic_ ! current @ search-in-wordlist
  then
  lfa>xt,flags
;

: (dovoc ( wid -- )
  _csr_ ! _?csr_ 1 _csr_ !   \ 190724
;

: dovoc ( a -- )
  @ (dovoc                   \ 190724
;


\ MM-190721-24
\ Create a VOC that extends the VOC wid (inherits from VOC wid).
: voc-extend ( "name" wid -- )
  \ compile the VOCs itag ( the wid of the inherited VOC )
    align ,
  \ create the VOC as an immediate word
   2 wflags !  \ set voc-flag in wtag
   here ( addr of names wtag ) cell+ ( lfa of name )
\  <builds , postpone immediate    \ MM-191226    
   <builds , [ ' immediate call, ] 
    does> dovoc 
;


: vocs-quit ( -- )
  0 _csr_ !  context _sop_ !
  \ ." reset "  \ only for debugging
  [ hook-quit @ literal, ] execute
;


root-wordlist set-current   \ Some tools needed in VOC contexts


\ Switch back from a VOC search order (voc-context) to the FORTH search order.
: .. ( -- )
  0 _csr_ !  _?csr_  immediate
;


: definitions ( -- ) 
\ _SOP_ @ @ set-current postpone .. immediate ;      \ MM-191226
  _SOP_ @ @ set-current [ ' .. call, ] immediate ;


\ Create a VOC that extends (inherits from) the actual VOC context.
 : voc ( "name" -- )
  _sop_ @ CONTEXT = if 0 else VOC-context @ then voc-extend
;


: first ( -- )
\ <voc> first overwrite the top of the permanent search order with the top wid
\ of the current temporary search order.
\  _sop_ @ @ context ! postpone .. immediate ;       MM-191226
\  _sop_ @ @ context ! \.. immediate ;               MM-191227
  _sop_ @ @ context !  [ ' .. call, ] immediate ;


\ Make the current compilation context the new search context.
: @voc ( -- )
  get-current (dovoc immediate
;


\ inside-wordlist first

get-order nip inside-wordlist swap set-order


 2 wflags !
 : root ( -- )   root-wordlist   (dovoc  immediate ;
 2 wflags !
 : inside ( -- ) inside-wordlist (dovoc  immediate ;
 2 wflags !
 : forth ( -- )  forth-wordlist  (dovoc  immediate ;


\ Make the next created word a context switching one (assign a ctag).
\ Usage: <voc> item <defining word> ...   i.e.:  123 item variable i1
: item ( -- )
  \ compile the actual VOCs wid as the next created words ctag
    align voc-context @ ,
  \ set bit 0 of the wflags in the next created words wtag
    1 wflags !
\    [ ' .. call, ]  \ MM-200105  MM-200106
;


\ Make the next created word a sticky one.
  : sticky ( -- ) align 1 , 1 wflags ! ;


\ Print the data stack and stay in the current context.
\  sticky   MM-191228
  : .s ( -- ) .s ;


inside-wordlist set-current

\ MM-200419  (C) message only on reset

\ Initialize Mecrisp with wordlist and voc extension.
: voc-init ( -- )
  hook-find @ ['] vocs-find <
  if
    ." VIS " vis
    ." : VOCs ITEMs and STICKY Words for Mecrisp-Stellaris by Manfred Mahlow"
    cr
  then
  wlst-init  ['] vocs-quit hook-quit !  ['] vocs-find hook-find !
;

\ inside-wordlist first
get-order nip inside-wordlist swap set-order

root-wordlist set-current

: init ( -- ) voc-init ;

\ forth-wordlist first
get-order nip forth-wordlist swap set-order

forth-wordlist set-current

init  \ now vocs can be used.


compiletoflash

: find ( a u -- xt|0 flags )  \ MM-200522
  inside first  search-in-dictionary lfa>xt,flags  forth first ;

\ MM-200521

root definitions  inside first

: (' ( "name" -- lfa )
  token search-in-dictionary ?dup if exit then ."  not found." abort
;  

forth definitions

: ' ( "name" -- xt ) (' lfa>xt ;  \ MM-200522

: ['] ( "name" -- ) ' literal, immediate ;

\ postpone needs to be redefined because the core word has an internal tick
\ dependency, that is not fullfilled, when the new find is used.

: postpone ( "name" -- )   \ MM-191227
  (' inside lfa>xt,flags dup $10 and if drop call, exit then  \ MM-200519
  drop literal, ['] call, call,  immediate
;


root definitions

: ' ( "name" -- ) ' ;

: ['] ( "name" -- ) postpone ['] immediate ;

: postpone ( "name" -- ) postpone postpone immediate ;

  forth first definitions  decimal

compiletoram

\ ------------------------------------------------------------------------------
\ Last Revision: MM-200522 0.8.3 : voc-init changed to only display (C) message
\                          on reset  find and (' added  ' and postpone changed  
\                MM-200122 0.8.2 revision
\ ------------------------------------------------------------------------------
\ Implementation Notes:
\ ------------------------------------------------------------------------------
\ After loading wordlists.txt all new words are prefixed/tagged with a wordlist-
\ tag ( wtag ).

\ wtag = wid || wflags

\  wid = identifier of the wordlist, the word belongs to

\  wflags = the 1 cells 2 / lowest bits of a wtag

\  we are only using Bit0 here (to be 16 Bit compatibel)


\ To make a word a context switching one, it's additionally prefixed with a 
\ context-tag ( ctag ) and bit wflags.0 is set.

\ ctag = wid || cflags

\ wid = identifier of the wordlist, to be set as top of the actual search order
\       after interpreting the word

\ cflags = the 1 cells 2 / lowest bits of a ctag ( not yet used )


\ Context switching is done by FIND-IN-Dictionary which is hooked to HOOK-FIND :

\ * Before searching the dictionary, it is checked ( by ?cps ), if the last 
\   interpreted word requested to change the search context. Then it's done.

\ * After a successful dictionary search it is recorded ( by !cps ) if a context
\   switch is requested. Then it will then be done ( by ?csp ) before the next
\   search.

\ * If an error occures, the search context is reset to the systems default
\   search order.


