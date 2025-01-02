\  @brief
\      Egg timer with 4 buttons
\      
\      Button 'c' minutes, 'b' seconds, 'd' start/stop, 'a' on/off
\  @file
\      egg-timer.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2024-12-27
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

0 variable seconds
0 variable minutes
0 variable counter

: .60 ( n -- ) \ display minutes/seconds on quad
  dup 
  10 /   [char] 0 + quad-emit
  10 mod [char] 0 + quad-emit
;
 
: .minutes ( -- ) \ display minutes on quad
  0 quadpos!
  minutes @ .60
;

: .seconds ( -- ) \ display seconds on quad
  2 quadpos!
  seconds @ .60
;

: 2counter ( -- ) \ set counter from minutes/seconds
  seconds @ minutes @ 60 * + counter !
;

: 2MinSec ( -- ) \ set minutes/seconds from counter
 counter @ dup
 60 mod seconds !
 60 /   minutes !
;

: beep ( -- )
  quadclr  2 quadblink
  [char] E quad-emit [char] L quad-emit [char] K quad-emit [char] E quad-emit
  8 pwmprescale \ 4 kHz buzzer
  5 6 dmod       \ set D6 to PWM
  20 0 do
    500 6 pwmpin! \ 50 % PWM
    300 osDelay drop
    button? if leave then
    0 6 pwmpin! \ PWM off
    500 osDelay drop 
    button? if leave then
  loop
  button drop
  0 6 dmod   \ set D6 to input
  0 quadblink
;

: count-down ( -- )
 -1 -1 -1 alarm!  \ an alarm every second
  begin
    wait-alarm  
    counter @ 1- dup counter !
    2MinSec .minutes .seconds
    0= button? or
  until
  button? 0= if beep then
  button drop
 ;

: count-up ( -- )
 -1 -1 -1 alarm!  \ an alarm every second
  begin
    wait-alarm  
    counter @ 1+ dup counter !
    2MinSec .minutes .seconds
    99 60 * 59 + >= button? or
  until
  button drop
 ;

: egg-timer ( -- )
  2counter
  begin button? while
    button drop \ empty the buffer
  repeat
  begin
    .minutes .seconds
    button case
    [char] a of  \ power
      shutdown
    endof
    [char] b of  \ second
      seconds @ 1+ dup
      60 >= if drop 0 then seconds !
      2counter
    endof
    [char] c of  \ minute
      minutes @ 1+ dup
      100 >= if drop 0 then minutes !
      2counter
    endof
    [char] d of  \ start/stop
      counter @ 0= if count-up else count-down then
    endof
    endcase
  again
;

task egg-timer&
egg-timer& construct
' egg-timer egg-timer& start-task



