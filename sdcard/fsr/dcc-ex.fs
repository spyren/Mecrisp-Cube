\  @brief
\      DCC-EX commands for the Pocket Power Pack
\      
\      https://dcc-ex.com/reference/software/command-summary-consolidated.html
\      There is only one output. 
\        - MAIN PROG JOIN
\        - Trackletter A.
\  @file
\      dcc-ex.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2025-09-24
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\      For details see copyright.txt

CR .( dcc-ex.fs loading ... )

10 constant #SLOT

true dcc !

: crlf ( -- ) 
  13 emit 10 emit
;

\ Power Management
\ ****************

\ Turn power on or off to the MAIN and PROG tracks

true  variable main-track
false variable prog-track
false variable dc-inverse
false variable main-inverse


: <1> ( -- ) \  <1> - power on
  \ switch on
  dcc @ if
    \ DCC
    DCCstart
    ." <p1 A>" crlf
    ." <p1>" crlf
    main-track @ if 
      ." <p1 MAIN>" crlf 
    else 
      prog-track @ if ." <p1 PROG>" crlf then 
    then
    true slotselect @ DCCstate!
  else
    DCCstop
    \ DC -> PWM
    0 0 pwmpin!
    0 1 pwmpin!
    2 pwmprescale     \ 16 kHz
    PWM_MODE 0 dmod   \ set D0 to pwm
    PWM_MODE 1 dmod   \ set D1 to pwm
    ." <p1 A>" crlf
    ." <p1>" crlf
    ." <p1 DC>" crlf 
  then
  true power !
;

: main? ( c- u -- f )  s" MAIN" compare ;
: prog? ( c- u -- f )  s" PROG" compare ;
: join? ( c- u -- f )  s" JOIN" compare ;

: <1  ( "ccc"<greaterthan> -- ) \  <1 MAIN|PROG|JOIN> - power on track 
  [char] > parse
  case 
    2dup main? ?of true  main-track ! false prog-track ! ." <p1 MAIN>" crlf endof
    2dup prog? ?of false main-track ! true  prog-track ! ." <p1 PROG>" crlf endof
    2dup join? ?of true  main-track ! true  prog-track ! ." <p1 JOIN>" crlf endof
  endcase
  drop
;

: <0> ( -- ) \  <0> - power off
  \ switch off rails
  DCCstop
  0 0 pwmpin!
  0 1 pwmpin!
  OUTPUT_MODE 0 dmod   \ set D0 to output
  OUTPUT_MODE 1 dmod   \ set D1 to output
  false power !
  ." <p0 A>" crlf
  ." <p0>" crlf
;

: <0  ( "ccc"<greaterthan> -- ) \  <0 MAIN|PROG> - power off track 
  [char] > parse
  case 
    2dup main? ?of true  main-track ! ." <p0 MAIN>" crlf endof
    2dup prog? ?of false prog-track ! ." <p0 PROG>" crlf endof
  endcase
  drop
;

: reset?    ( c- u -- f )  s" RESET" compare ;
: ack?      ( c- u -- f )  s" ACK" compare ;
: cabs?     ( c- u -- f )  s" CABS" compare ;
: speed28?  ( c- u -- f )  s" SPEED28" compare ;
: speed128? ( c- u -- f )  s" SPEED128" compare ;

: show-cabs ( -- )
  #SLOT 0 do
    i DCCstate@ if
      \ slot enabled
      ." <cab " i DCCaddress@ . i DCCspeed@ . i DCCdirection@ 
      if ." forward>" else ." reverse>" then crlf
    then 
  loop
;

: <D ( "ccc"<greaterthan> -- ) \ <D RESET|> - Re-boot the command Station
  [char] > parse
  case 
    2dup reset?    ?of reset endof
    2dup ack?      ?of  endof      \ not supported yet
    2dup cabs?     ?of show-cabs endof
    2dup speed28?  ?of  endof      \ supports only 128 speed steps
    2dup speed128? ?of  endof       \ "
  endcase
  drop
;

: <JI>  ( -- ) \ <J I> <JI> - Request current values list
  \ <jI [cA cB cC ...]> c: Raw current value for each defined Track, in milliAmps 
  ." <jI " Irail 1000E f* f>s u-. ." A>" crlf
;

: <JG>  ( -- ) \ <J G> <JG> - Request max current list
  \ not supported yet
  ." <jG 1000A>" crlf
;


\ Track Manager aka DC-District
\ *****************************

99 variable dc-cab
0  variable dc-speed
1  variable dc-direction

: <=> ( -- ) \ <=> - Request the current Track Manager configuration
  \ for each track/channel supported by the Motor Shield <= trackletter state cab>
  ." <= A " 
  dcc @ if 
    main-track @ prog-track @ or if
      main-track @ if
        main-inverse @ if ." MAIN_INV>" else ." MAIN>" then 
      else
        main-inverse @ if ." PROG_INV>" else ." PROG>" then
      then
    else
      main-inverse @ if ." MAIN_INV>" else ." MAIN>" then
    then
  else
    dc-inverse @ if ." DCX " else ." DC " then dc-cab @ u-. ." >"
  then 
  crlf
;

: main_inv?  ( c- u -- f )  s" MAIN_INV" compare ;
: main_auto? ( c- u -- f )  s" MAIN_AUTO" compare ;
: dc?        ( c- u -- f )  s" DC" compare ;
: dc_inv?    ( c- u -- f )  s" DC_INV" compare ;
: dcx?       ( c- u -- f )  s" DCX" compare ;
: none?      ( c- u -- f )  s" NONE" compare ;

: dc-track ( f -- )   \ inverse
  dc-inverse !  false dcc !  token evaluate dc-cab !
;

: dcc-track ( f -- )  \ inverse
  main-inverse !  true dcc !
;

: trackA
  token case 
    2dup main?      ?of false dcc-track endof
    2dup main_inv?  ?of true  dcc-track endof
    2dup main_auto? ?of false dcc-track endof \ not supported yet
    2dup dc?        ?of false dc-track endof
    2dup dc_inv?    ?of true  dc-track endof
    2dup dcx?       ?of true  dc-track endof
    2dup none?      ?of 
      false main-track !    false prog-track ! 
      false main-inverse !  false dc-inverse !
    endof
  endcase
  drop
  <0> \ Whenever the track mode is changed, track power is automatically turned off.
;

: <= ( "ccc"<greaterthan> -- ) \ <= trackletter mode [cab]> - configure track manager 
  \ this word is already defined for comparison!
  source \ save source
  [char] > parse setsource \ set new source
  >in @  \ save old source offset
  0 >in ! \ set new source offset
  token s" A" compare if 
    trackA
  then
  >in !    \ restore source offset
  setsource \ restore source
  <=>
;


\ Cab (Loco) Commands
\ *******************

: cab2slot ( n1 -- n2 ) \ find a slot for the cab, if there is no slot available -1
  #SLOT 0 do
    dup i DCCaddress@ = if
      \ slot found
      drop 
      true i DCCstate! 
      i unloop exit
    then
  loop
  \ look for a free slot
  #SLOT 0 do
    dup 0 i DCCaddress@ = if
      \ free slot found -> fill in address and activate slot
      i DCCaddress!  true i DCCdirection!  true i DCCstate!
      i unloop exit
    then
  loop
  drop
  -1
;

: slot-info ( u -- ) \ slot info <l cab reg speedByte functMap>
  dup 0< if drop exit then
  ." <l " 
  dup DCCaddress@ .
  0 .
  dup dup DCCspeed@ swap DCCdirection@ + .
  DCCFunction@ u-.
  ." >" crlf
;

: <t ( "ccc"<greaterthan> -- ) \ <t cab speed dir> - Set Cab (Loco) speed 
                               \ <t cab> - Request a deliberate update on 
                                \ the cab (loco) speed/functions
  depth >r
  [char] > parse
  evaluate ( -- cab [speed] [dir])
  depth r> - \ calculate number of parameter
  case
    1 of 
      cab2slot slot-info
    endof
    3 of 
      rot cab2slot ( -- speed dir slot)
      dup 0< if 2drop drop exit then
      tuck ( -- speed slot dir slot)
      DCCdirection! ( -- speed slot)
      tuck ( -- slot speed slot) 
      DCCspeed! ( -- slot)
      dup true swap DCCstate!
      slot-info
    endof
    ( default) 
  endcase
;

: <!> ( -- ) \ Emergency stop
  #SLOT 0 do
    i DCCstate@ if
      \ slot enabled -> stop
      1 i DCCspeed!
      i slot-info
    then 
  loop
;

: <F ( "ccc"<greaterthan> -- ) \ <F cab funct state> - Turn loco decoder functions ON or OFF
  [char] > parse
  evaluate swap 1 swap lshift ( -- cab state funcbit) 
  swap rot ( -- funcbit state cab )
  cab2slot ( -- funcbit state slot) 
  dup 0< if 2drop drop exit then
  dup >r swap ( -- funcbit slot state ) ( R: -- slot )
  if DCCfunction! else -DCCfunction! then
  r> slot-info
;

: <-> ( -- ) \ <-> - Remove all locos from reminders
  #SLOT 0 do
    slotselect @ i <> if
      false i DCCstate!
    then 
  loop
;

: <- ( "ccc"<greaterthan> -- ) \ <- cab> - Remove one loco from reminders
  [char] > parse
  evaluate ( -- cab )
  cab2slot
  dup 0< if drop exit then
  false swap DCCstate!
;

: <m ( "ccc"<greaterthan> -- ) \ <m [type] | [cab acceleration [deceleration]]> - set the momentum of a loco
  [char] > parse
  2drop \ not supported
;

: <c> ( -- ) \ <c> - Request Current on the Track(s)
\ Irail ( -- r ) \ rail current [A]

;

: <s> ( -- ) \ <s> - Request the DCC-EX version and hardware info, along with listing defined turnouts
  \ <iDCCEX version / microprocessorType / MotorControllerType / buildNumber>
  ." <iDCCEX PPP1.0 STM32WB DRV8871 0>" crlf
;

: <#> ( -- ) \ <#> - Request the number of supported cabs(locos)
  ." <# " #SLOT u-.  ." >"
;


\ Roster Commands
\ ***************

: <JR> ( -- ) \ <J R> <JR> - Request the list defined Roster Entry IDs
  ." <jR>" \ roster not supported yet
;


\ Turnouts/Points
\ ***************

\ Turntables/Traversers
\ *********************

\ Routes/Automations
\ ******************

\ System Information
\ ******************

\ DCC Accessories
\ ***************

\ Sensors
\ *******

\ Signals
\ *******

\ WiFi Control
\ ************


\ Writing Configuration Variable (CVs)
\ ************************************

\ Reading/Writing Configuration Variables (CVs) - Programming track
\ *****************************************************************


\ Write direct DCC packet
\ ***********************

\ Programming track - Tuning
\ **************************


\ Configuring the EX-CommandStation
\ *********************************

\ Turnouts/Points (Configuring the EX-CommandStation)
\ ***************************************************

\ Turntables/Traversers (Configuring the EX-CommandStation)
\ *********************************************************

\ Sensors (Configuring the EX-CommandStation)
\ *******************************************

\ Outputs (Configuring the EX-CommandStation)
\ *******************************************

\ EEPROM Management (Configuring the EX-CommandStation)
\ *****************************************************

\ Diagnostic Programming Commands (Configuring the EX-CommandStation)
\ *******************************************************************

\ I/O (HAL) Diagnostics
\ *********************


\ Tools
\ *****

: decoder2linear ( u1 u2 -- u3 ) \ convert decoder address u1 (1..511) and sub address u2 (0..3) to linear address u3
  swap 1 - 4 *
  1 + +
; 

: linear2decoder ( u1 -- u2 u3 ) \ convert linear address (1..2044) u1 to decoder address u2 and sub address u3
  3 + dup 4 / swap
  4 mod
;

: close-switch ( u1 u2 -- ) \ close switch at decoder address u1, sub address u2
  decoder2linear
  1 0 rot DCCaccessory!
;

: throw-switch ( u1 u2 -- ) \ throw switch at decoder address u1, sub address u2
  decoder2linear
  1 1 rot DCCaccessory!
;
