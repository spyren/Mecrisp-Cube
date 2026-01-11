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

: main? ( c- u -- f ) 
  2dup upper s" MAIN" compare
;

: prog? ( c- u -- f ) 
  2dup upper s" PROG" compare
;
: join? ( c- u -- f ) 
  2dup upper s" JOIN" compare
;

: main_inv? ( c- u -- f ) 
  2dup upper s" MAIN_INV" compare
;

: main_auto? ( c- u -- f ) 
  2dup upper s" MAIN_AUTO" compare
;

: dc? ( c- u -- f ) 
  2dup upper s" DC" compare
;

: dc_inv? ( c- u -- f ) 
  2dup upper s" DC_INV" compare
;

: dcx? ( c- u -- f ) 
  2dup upper s" DCX" compare
;

\ Turn power on or off to the MAIN and PROG tracks

: <1> ( -- ) \  power on
  ." <p1>"
;

: <1  ( "ccc"<greaterthan> -- ) \  power on track <1 MAIN|PROG|JOIN>
  [char] > parse
  case 
    2dup main? ?of ." <pM1>" endof
    2dup prog? ?of ." <pP1>" endof
    2dup join? ?of ." <pJ1>" endof
  endcase
  drop
;

: <0> ( -- ) \  power off
  ." <p0>"
;

: <0  ( "ccc"<greaterthan> -- ) \  power off track <0 MAIN|PROG>
  [char] > parse
  case 
    2dup main? ?of ." <pM0>" endof
    2dup prog? ?of ." <pP0>" endof
  endcase
  drop
;


\ Track Manager aka DC-District

: <= ( "ccc"<greaterthan> -- ) \ configure track manager <= trackletter mode [cab]>
  [char] > parse
  case 
    2dup main?      ?of ." <pM0>" endof
    2dup main_inv?  ?of ." <pP0>" endof
    2dup main_auto? ?of ." <pP0>" endof
    2dup dc?        ?of ." <pP0>" endof
    2dup dc_inv?    ?of ." <pP0>" endof
    2dup dcx?       ?of ." <pP0>" endof
    2dup none?      ?of ." <pP0>" endof
  endcase
  drop
;


\ Cab (Loco) Commands

: <t ( "ccc"<greaterthan> -- ) \ Set Cab (Loco) speed <t 
  [char] > parse
;

: <!> ( -- ) \ Emergency stop
;
