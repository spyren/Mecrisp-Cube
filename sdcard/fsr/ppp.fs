\  @brief
\      Pocket Power Pack
\      
\      Model railroad power pack (throttle).
\      HW based on STM32WB55 Firefly.
\  @file
\      ppp.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2025-01-21
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\      For details see copyright.txt

CR .( ppp.fs loading ... )

5  constant PWM_MODE
3  constant OUTPUT_MODE

3    variable slot0    \ default locomotive address for slot0
56   variable slot1    \ Bangor and Aroostook BL2
45   variable slot2    \ FAUR L45H
6775 variable slot3    \ PRR Mountain M1a

0 variable menu        \ 0 mode, 1 pwm1, 2 pwm2
2 constant maxmenu-dc
5 constant maxmenu-dcc
maxmenu-dc variable maxmenu
0 variable power       \ power off
0 variable dcc         \ 0 DC, 1 DCC
1 variable direction   \ 1 forward, 0 reverse
1 variable brake       \ 1 brake
0 variable speed       \ 0 off, 1000 max
0 variable dcc-speed   \ 0 off, 1 emergency off, 2 min, 126 max
5 variable pwmselect   \ 0 0.5 kHz, 1 1 kHz, 2 2 kHz, 
                       \ 3 4 kHz,   4 8 kHz, 5 16 kHz, 6 32 kHz
0 variable display-off
 
0 variable slotselect  \ active slot 0..3
0 variable funcselect  \ F0 .. F28, bitwise coding

create user-func  3 ,  4 ,  5 ,  6 , \ first user functions row
                  7 ,  9 , 10 , 11 , \ second row

: speed@ ( -- u ) \  get speed u (0 .. 1000) from potentiometer
  0 apin@ 4 / dup
  1000 > if  drop 1000 then dup speed !
;

: dcc-speed@ ( -- u ) \  get dcc-speed u (0, 2 .. 126) from potentiometer
  0 apin@ 32 / dup
  1 = if 
    drop 0 
  else 
    dup 127 >= if 
      drop 126
    then
  then 
  dup dcc-speed !
;

: %pwm ( -- u ) \ get PWM u 0 .. 100 %
  speed @ 10 /
; 

: Vusb (  -- r ) \ USB voltage 
  vref@ s>f 1k f/              \ Vref
  4 apin@ s>f 1e 4096e f/ f* f* \ V measure
  2e f*                          \ 1/2 voltage divider
;

: Vrail (  -- r ) \ rail voltage 
  vref@ s>f 1k f/              \ Vref
  2 apin@ s>f 1e 4096e f/ f* f* \ V measure
  10k f/ 68k 10k f+ f*           \ 68 k 10 k voltage divider
;

: Vlipo (  -- r ) \ LiPo voltage 0
  vref@ s>f 1k f/              \ Vref
  1 apin@ s>f 1e 4096e f/ f* f* \ V measure
  2e f*                          \ 1/2 voltage divider
;

: Irail ( -- r ) \ rail current [A]
  vref@ s>f 1k f/              \ Vref
  3 apin@ s>f 1e 4096e f/ f* f* \ V measure -> A
  1.25e f*                       \ correction factor
;

: .Vrail ( -- ) \ print rail voltage
  2 oledfont
  0 0 oledpos! 1 set-precision
  ." Vr " Vrail f. 
;

: .Vlipo ( -- ) \ print LiPo voltage
  2 oledfont
  64 0 oledpos! 2 set-precision
  ." Vb " Vlipo f. 
;

: .Irail ( -- ) \ print rail current [A]
  3 oledfont
  0 4 oledpos! 3 set-precision
  ." I " Irail f. ." A"
;

: u.pwm ( u -- ) \ print PWM
  dup 10 < if
    u.   ." %    "
  else
    dup 100 < if
      u.  ." %   "
    else
      u.   ." %  "
    then
  then
;

: u.speed ( u -- ) \ print speed step
  dup 10 < if
    u.   ." st   "
  else
    dup 100 < if
      u.  ." st  "
    else
      u.   ." st " 
    then
  then
;

: .speed ( -- ) \ print speed (PWM or DCC step)
  3 oledfont
  0 2 oledpos!
  
  dcc @ if
    direction @ if 
      ."   " dcc-speed @ u.speed ." >"
    else 
      ." < " dcc-speed @ u.speed ."  " 
    then
  else
    direction @ if 
     ."   " %pwm u.pwm           ." >" 
    else 
     ." < " %pwm u.pwm           ."  "
    then
 then
;

: .mode ( -- ) \ print mode menu
  0 oledfont
  0 6 oledpos!
   \ 012345678901234567890
  ." Display, Mode        " 
  0 7 oledpos!
  ."  ON  "
  dcc @ if 
       ." [DCC] " 
  else ." [DC]C " then 
              ."  -   DARK"
;

: .BRK ( -- )
  brake @ if ." [BRK]" else ."  BRK " then
;

: .pwm-menu ( -- ) \ print first PWM menu
  0 oledfont
  0 6 oledpos!
          \ 012345678901234567890
         ." PWM Frequency [kHz] 1"
  0 7 oledpos!
  pwmselect @ case
          \ 012345678901234567890
          \ [BRK]
    0 of .BRK ." [.5]   1    2   " endof
    1 of .BRK ."  .5   [1]   2   " endof
    2 of .BRK ."  .5    1   [2]  " endof
         .BRK ."  .5    1    2   "
  endcase
;

: .pwm-menu1 ( -- ) \ print second PWM menu
  0 oledfont
  0 6 oledpos!
          \ 012345678901234567890
         ." PWM Frequency [kHz] 2"
  0 7 oledpos!
  pwmselect @ case
          \ 012345678901234567890
    3 of ."  [4]   8    16   32 " endof
    4 of ."   4   [8]   16   32 " endof
    5 of ."   4    8   [16]  32 " endof
    6 of ."   4    8    16  [32]" endof
         ."   4    8    16   32 "
  endcase
;

: .dc-menu ( -- ) \ print DC menus
  menu @ case
    0 of .mode endof
    1 of .pwm-menu endof
    2 of .pwm-menu1 endof
  endcase
;

: u-. ( u -- ) \ like u. but without trailing space
  0 <# #s #> TYPE 
;

: [u-]. ( u -- ) \ like u-. but with brackets
  0 [char] [ emit <# #s #> TYPE [char] ] emit
;

: .dcc-slot-item ( u f -- ) \ print u item, f selected
  if
    dup 10 < if
      space [u-]. space
    else dup 100 <  if [u-]. space 
      else
        [u-].
      then
    then
  else
    dup 10 <  if
      space space  u-.  space space
    else dup 100 <  if
        space u-. space space
      else
        space u-. space
      then
    then
  then
; 

: .dcc-menu-slot ( -- ) \ print slots
  0 oledfont
  0 6 oledpos!
   \ 012345678901234567890
  ." Slots                " 
  slotselect @ 
  4 0 do
    dup i =  i DCCaddress@ swap .dcc-slot-item
  loop
  drop
;

: .dcc-menu-functions ( -- ) \ print functions
  0 oledfont
  0 6 oledpos!
   \ 012345678901234567890
  ." Functions            "
  slotselect @ DCCfunction@ funcselect ! funcselect
  dup 1 swap bit@ if ." [Lgt]" else ." Lght " then \ F0
  dup 2 swap bit@ if ." [Bll]" else ." Bell " then \ F1
  dup 4 swap bit@ if ." [Hrn]" else ." Horn " then \ F2
    256 swap bit@ if ." [Mte]" else ." Mute " then \ F8
;

: .function-bit ( u -- ) \ print function bit u from active slot, if set with [u]
    cells user-func + @  dup \ get the function number
    slotselect @ DCCfunction@ funcselect ! funcselect  \ get the function bits
    swap bit@ 
    swap dup 10 < if space then \ add a space 
    swap if [u-]. else space u-. space then space \ F0
;

: .dcc-menu-user-1 ( -- ) \ print user functions line 1
  0 oledfont
  0 6 oledpos!
   \ 012345678901234567890
  ." User Functions 1st   "
  4 0 do i .function-bit loop
;

: .dcc-menu-user-2 ( -- ) \ print user functions line 2
  0 oledfont
  0 6 oledpos!
   \ 012345678901234567890
  ." User Functions 2nd   "
  8 4 do i .function-bit loop
;

: .dcc-menu ( -- ) \ print DCC menus
  menu @ case
    0 of .mode endof
    1 of .dcc-menu-slot endof
    2 of .dcc-menu-functions endof
    3 of .dcc-menu-user-1 endof
    4 of .dcc-menu-user-2 endof
  endcase
;

: dc-throttle ( -- ) \ control the PWM pins
  speed@
  direction @ if
    \ forward
    brake @ if
      1000        0 pwmpin!
      1000 swap - 1 pwmpin!
    else
      0           1 pwmpin!
                  0 pwmpin!
    then
  else
    \ reverse
    brake @ if
      1000        1 pwmpin!
      1000 swap - 0 pwmpin!
    else
      0           0 pwmpin!
                  1 pwmpin!
    then
 then
;

: dcc-throttle ( -- ) \ set speed and direction of the selected slot
  dcc-speed@ slotselect @ DCCspeed!
  direction @  slotselect @ DCCdirection!
;

: throttle ( -- ) \ control the speed, update every 10 ms
  begin
    10 osdelay drop
    dcc @ if dcc-throttle else dc-throttle then
  again
;

: .menu ( -- ) \ main menu
  dcc @ if
    .dcc-menu
  else
    .dc-menu
  then
 ;

: ppp-display ( -- )  \ display throttle infos every 200 ms till button is pressed
  >oled
  begin
     .Vrail .Vlipo 
     .speed
     .Irail 
     .menu
     200 osDelay drop
  button? until
  >term
;

: mode-power ( -- ) \ set power mode
  power @ if
    \ switch on
    dcc @ if
      \ DCC
      DCCstart
    else
      DCCstop
      \ DC -> PWM
      0 0 pwmpin!
      0 1 pwmpin!
      2 pwmprescale     \ 16 kHz
      PWM_MODE 0 dmod   \ set D0 to pwm
      PWM_MODE 1 dmod   \ set D1 to pwm
    then
  else
    \ switch off rails
    0 0 pwmpin!
    0 1 pwmpin!
    OUTPUT_MODE 0 dmod   \ set D0 to output
    OUTPUT_MODE 1 dmod   \ set D1 to output
  endif
;

: mode-button ( u -- ) \ mode buttons
  case
    [char] d of  \ switch on/off
      power @ 0= power ! \ toggle power
      mode-power
    endof
    [char] e of  \ DC or DCC
      dcc @ 0= dcc ! \ toggle DCC
      dcc @ if maxmenu-dcc else maxmenu-dc then maxmenu !
      mode-power
    endof          
    [char] f of  \ not used yet
    [char] g of  \ switch off display (dark)
      1 display-off ! oledclr 
    endof 
  endcase
;

: pwm-button ( u -- ) \ Brake and PWM1 buttons
  case
    [char] d of brake @ 0= brake ! endof           \ toggle brake
    [char] e of 0 pwmselect ! 64 pwmprescale endof \ 500 Hz
    [char] f of 1 pwmselect ! 32 pwmprescale endof \ 1 kHz
    [char] g of 2 pwmselect ! 16 pwmprescale endof \ 2 kHz
  endcase
;

: pwm-button1 ( u -- ) \ PWM2 buttons
  case
    [char] d of 3 pwmselect !  8 pwmprescale endof \ 4 kHz
    [char] e of 4 pwmselect !  4 pwmprescale endof \ 8 kHz
    [char] f of 5 pwmselect !  2 pwmprescale endof \ 16 kHz
    [char] g of 6 pwmselect !  1 pwmprescale endof \ 32  kHz
  endcase
;

: slot-button ( u -- ) \ select DCC slot
  0 slotselect @ DCCstate!  \ disable old slot
  case
    [char] d of 0 endof
    [char] e of 1 endof
    [char] f of 2 endof
    [char] g of 3 endof
    ( default)  0
  endcase
  dup slotselect ! -1 swap DCCstate!
;

: functions-button ( u -- ) \ functions buttons
  case
    [char] d of 1   endof \ F0 Light
    [char] e of 2   endof \ F1 Bell
    [char] f of 4   endof \ F2 Horn
    [char] g of 256 endof \ F8 Mute
    ( default)  0
  endcase
  dup if 
    \ there is something to change
    slotselect @ DCCfunction@ funcselect !
    dup funcselect bit@ if
      \ bit already set
      slotselect @ -DCCfunction!
    else
      \ bit already reset
      slotselect @ DCCfunction!
    then
  else
    drop
  then
;

: user-button-1 ( u -- ) \ functions buttons
;

: user-button-2 ( u -- ) \ functions buttons
;

: menu-button ( u -- ) \ menu buttons
  display-off @ if
    drop 0 display-off !
  else
    menu @ case
      0 of mode-button endof 
      1 of dcc @ if slot-button else pwm-button then endof
      2 of dcc @ if functions-button else pwm-button1 then endof
      3 of user-button-1 endof
      4 of user-button-2 endof
   endcase
  then
;

: ppp-menu ( -- ) \ display throttle infos till a button is pressed
  \ default slots
  slot0 @ 0 DCCaddress!
  slot1 @ 1 DCCaddress!
  slot2 @ 2 DCCaddress!
  slot3 @ 3 DCCaddress!
  begin
    display-off @ 0= if ppp-display then
    button case
      [char] a of 0 direction ! endof      \ reverse
      [char] b of menu @ 1+ dup maxmenu @ > if drop 0 then menu ! endof \ menu down
      [char] h of menu @ 1- dup 0 < if drop maxmenu @ then menu ! endof \ menu up
      [char] c of 1 direction ! endof      \ forward
      menu-button
    endcase
  again
;

task throttle&
task ppp-menu&


\ this should part of /etc/rc.local to start the threads
\
\ 2000 osDelay drop \ time to display flash screen
\ oledclr  
\ throttle& construct
\ ' throttle throttle& start-task
\ ppp-menu& construct
\ ' ppp-menu ppp-menu& start-task



