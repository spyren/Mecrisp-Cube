\  @brief
\      Some tools for Mecrisp-Cube RTOS
\
\      For details see https://spyr.ch/twiki/bin/view/MecrispCube/CmsisRtos
\      and /man/CmsisRtos.md
\  @file
\      rtos.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2021-05-16
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\      For details see copyright.txt

\ Report a snapshot of the current threads
CR .( rtos.fs loading ... )

32 constant MAX_THREAD_COUNT
MAX_THREAD_COUNT cells buffer: threadList

: .osState ( n -- )
  case
    0  of ." Inactive    " endof
    1  of ." Ready       " endof
    2  of ." Running     " endof
    3  of ." Blocked     " endof
    4  of ." Terminated  " endof
   -1  of ." Error       " endof
          ." unknown     "
  endcase
;

: .threads ( -- )
  cr ." Name                ID         State       Priority  Stack Space" cr
     ." ----------------------------------------------------------------" cr
  threadList MAX_THREAD_COUNT osThreadEnumerate
  0 do
    threadList I cells + @  \ get thread ID
    dup osThreadGetName          strlen tuck type 20 swap - spaces
    dup                         hex. 2 spaces
    dup osThreadGetState        .osState
    dup osThreadGetPriority     u.2 8 spaces
        osThreadGetStackSpace   u.4
    cr
  loop
;


: user  ( "name" -- )  \ create user variable
  create , does> @
  0 dup pvTaskGetThreadLocalStoragePointer +
;

