\ Report a snapshot of the current threads
CR .( threads.fs loading ... )

32 constant MAX_THREAD_COUNT 
MAX_THREAD_COUNT buffer: threadList

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
  cr ." Name                State    Priority   Stack Space" cr
  threadList MAX_THREAD_COUNT osThreadEnumerate
  0 do
    threadList I cells + @  \ get thread ID
    dup osThreadGetName          strlen tuck type 20 swap - spaces
    dup osThreadGetState        .osState 
    dup osThreadGetPriority     u.2 10 spaces
        osThreadGetStackSpace   u.4  
    cr
  loop
;
