\ control-structure add-ons (ENDIF, ?DUP-IF etc.)

\ This file is in the public domain. NO WARRANTY.

\ Hmm, this would be a good application for ]] ... [[

\ The program uses the following words
\ from CORE :
\ : POSTPONE THEN ; immediate ?dup IF 0= 
\ from BLOCK-EXT :
\ \ 
\ from FILE :
\ ( 

: endif ( compilation orig -- ; run-time -- ) \ gforth
    POSTPONE then ; immediate

: ?dup-if ( compilation -- orig ; run-time n -- n| ) \ gforth	question-dupe-if
    POSTPONE ?dup POSTPONE if ; immediate

: ?dup-0=-if ( compilation -- orig ; run-time n -- n| ) \ gforth	question-dupe-zero-equals-if
    POSTPONE ?dup POSTPONE 0= POSTPONE if ; immediate
