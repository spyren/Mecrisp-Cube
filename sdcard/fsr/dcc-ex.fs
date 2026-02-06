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


\ Power Management
\ ****************

\ Turn power on or off to the MAIN and PROG tracks

false variable main-track
false variable prog-track
false variable dc-inverse
false variable main-inverse


: <1> ( -- ) \  <1> - power on
  \ switch on
  dcc @ if
    \ DCC
    DCCstart
    true slotselect @ DCCstate!
  else
    DCCstop
    \ DC -> PWM
    0 0 pwmpin!
    0 1 pwmpin!
    2 pwmprescale     \ 16 kHz
    PWM_MODE 0 dmod   \ set D0 to pwm
    PWM_MODE 1 dmod   \ set D1 to pwm
  then
  true power !
  true main-track ! false prog-track !
  ." <p1>"
;

: main? ( c- u -- f )  s" MAIN" compare ;
: prog? ( c- u -- f )  s" PROG" compare ;
: join? ( c- u -- f )  s" JOIN" compare ;

: <1  ( "ccc"<greaterthan> -- ) \  <1 MAIN|PROG|JOIN> - power on track 
  [char] > parse
  case 
    2dup main? ?of true  main-track ! false prog-track ! ." <p1 MAIN>" <1> endof
    2dup prog? ?of false main-track ! true  prog-track ! ." <p1 PROG>" <1> endof
    2dup join? ?of true  main-track ! true  prog-track ! ." <p1 JOIN>" <1> endof
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
  ." <p0>"
;

: <0  ( "ccc"<greaterthan> -- ) \  <0 MAIN|PROG> - power off track 
  [char] > parse
  case 
    2dup main? ?of false main-track ! ." <p0 MAIN>" endof
    2dup prog? ?of false prog-track ! ." <p0 PROG>" endof
  endcase
  drop
;

: reset?  ( c- u -- f )  s" RESET" compare ;

: <D ( "ccc"<greaterthan> -- ) \ <D RESET|> - Re-boot the command Station
  [char] > parse
  case 
    2dup reset? ?of reset endof
  endcase
  drop
;


\ Track Manager aka DC-District
\ *****************************

0 variable dc-cab
0 variable dc-speed
0 variable dc-direction

: <=> ( -- ) \ <=> - Request the current Track Manager configuration
;

: main_inv?  ( c- u -- f )  s" MAIN_INV" compare ;
: main_auto? ( c- u -- f )  s" MAIN_AUTO" compare ;
: dc?        ( c- u -- f )  s" DC" compare ;
: dc_inv?    ( c- u -- f )  s" DC_INV" compare ;
: dcx?       ( c- u -- f )  s" DCX" compare ;
: none?      ( c- u -- f )  s" NONE" compare ;

: dc-track ( f -- )   \ inverse
  dc-inverse !  false dcc !  token evaluate dc-cab !  <1>
;

: dcc-track ( f -- )  \ inverse
  main-inverse !  true dcc !  <1>
;


: trackA
  ." TrackA "
  token 2dup cr type case 
    2dup main?      ?of false dcc-track endof
    2dup main_inv?  ?of true  dcc-track endof
    2dup main_auto? ?of false dcc-track endof
    2dup dc?        ?of false dc-track endof
    2dup dc_inv?    ?of true  dc-track endof
    2dup dcx?       ?of true  dc-track endof
    2dup none?      ?of endof
  endcase
  drop
;

: <= ( "ccc"<greaterthan> -- ) \ <= trackletter mode [cab]> - configure track manager 
  source \ save source
  [char] > parse setsource \ set new source
  >in @  \ save old source offset
  0 >in ! \ set new source offset
  token s" A" compare if 
    trackA
  then
  >in !    \ restore source offset
  setsource \ restore source
;


\ Cab (Loco) Commands
\ *******************

: cab2slot ( n1 -- n2 ) \ find a slot for the cab, if there is no slot available -1
  #SLOT 0 do
    dup i DCCaddress@ = if
      \ slot found
      drop i unloop exit
    then
  loop
  \ look for a free slot
  #SLOT 0 do
    dup 0 i DCCaddress@ = if
      \ free slot found
      drop i unloop exit
    then
  loop
  drop
  -1
;

: <t ( "ccc"<greaterthan> -- ) \ <t cab speed dir> - Set Cab (Loco) speed 
  [char] > parse
  evaluate ( -- cab speed dir)
  rot cab2slot dup ( -- speed dir slot slot)
  rot DCCdirection!
  DCCspeed!
;

: <!> ( -- ) \ Emergency stop
;

: <F ( "ccc"<greaterthan> -- ) \ <F cab funct state> - Turn loco decoder functions ON or OFF
  [char] > parse
  evaluate ( -- cab func state)
  rot cab2slot swap ( -- func slot state )
  if DCCfunction! else -DCCfunction! then
;

: <-> ( -- ) \ <-> - Remove all locos from reminders
;

: <- ( "ccc"<greaterthan> -- ) \ <- cab> - Remove one loco from reminders
  [char] > parse
  evaluate ( -- cab )
  drop \ not supported yet
;

: <D ( "ccc"<greaterthan> -- ) \ <D speedsteps> - Switch between 28 and 128 speed steps 
  [char] > parse 
  evaluate ( -- speedsteps )
  drop \ not supported
;

: <m ( "ccc"<greaterthan> -- ) \ <m [type] | [cab acceleration [deceleration]]> - set the momentum of a loco
  [char] > parse
  2drop \ not supported
;

: <c> ( -- ) \ <c> - Request Current on the Track(s)
;

: <s> ( -- ) \ <s> - Request the DCC-EX version and hardware info, along with listing defined turnouts
;

: <#> ( -- ) \ <#> - Request the number of supported cabs(locos)
;


\ Roster Commands
\ ***************

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


\ Other
\ *****

\ Other Commands
\ **************

