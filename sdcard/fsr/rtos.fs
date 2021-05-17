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
\ Report a snapshot of the current threads
CR .( rtos.fs loading ... )

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

