\  @brief
\      Start Pocket Power Pack application
\      
\      Model railroad power pack (throttle).
\      HW based on STM32WB55 Firefly.
\  @file
\      start.fs
\  @author
\      Peter Schmid, peter@spyr.ch
\  @date
\      2026-04-01
\  @remark
\      Language: Mecrisp-Stellaris Forth
\  @copyright
\      Peter Schmid, Switzerland
\      For details see copyright.txt

2000 osDelay drop \ time to display flash screen

slots dup
   3 ( default address)          swap ! cell+ dup 
  56 ( BL2 Bangor & Aroostook )  swap ! cell+ dup
  45 ( Faun Diesel )             swap ! cell+ dup
\ 6775 ( 2-10-0 Pennsy )           swap ! drop
 460 ( 4-4-2 Pennsy )            swap ! drop

user-functions dup
     ( Soundtraxx )
   3 ( Short Airhorn )           swap ! cell+ dup \ first row
   4 ( Dynamic Brake )           swap ! cell+ dup
   5 ( RPM+ )                    swap ! cell+ dup
   6 ( RPM- )                    swap ! cell+ dup
   7 ( Dimmer )                  swap ! cell+ dup \ second row
   9 ( Grade-Crossing Signal )   swap ! cell+ dup
  10 ( Straight-to-Eight )       swap ! cell+ dup
  11 ( Independent/Train Brake ) swap ! drop

switches dup
  37 ( 10 0 )                    swap ! cell+ dup \ first row
  41 ( 11 0 )                    swap ! cell+ dup
  45 ( 12 0 )                    swap ! cell+ dup
  49 ( 13 0 )                    swap ! cell+ dup
  53 ( 14 0 )                    swap ! cell+ dup \ second row
  57 ( 15 0 )                    swap ! cell+ dup
  61 ( 16 0 )                    swap ! cell+ dup
  65 ( 17 0 )                    swap ! drop

switch-states #SWITCH cells 0 fill \ init switch-states (all closed)

default-switch-names switch-names #SWITCH cells move \ init switch-names
default-slot-names slot-names #SLOT cells move        \ init slot-names
default-slot-functions slot-functions #SLOT cells move \ init slot-functions

oledclr  
true slot# @ DCCstate!

\ start throttle task
throttle& construct
' throttle throttle& start-task

\ start menu task
ppp-menu& construct
' ppp-menu ppp-menu& start-task

