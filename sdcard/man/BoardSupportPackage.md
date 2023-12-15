Board Support Package
=====================

The board support package for the STM32WB Nucleo Board is restricted to the
Arduino UNO R3 pin header and the onboard LEDs and switches (buttons).
The STM32 has much more capabilities than 14 digital I/O pins, 6 analog
input pins, UART, SPI, and I2C interfaces. But if you want to use the
more advanced features you can use the CubeMX to create source code for
the internal peripherals. This project wants to show how to use the Cube
Ecosystem for a Forth system (or vice versa) and can\'t implement all
features and possibilities the STM32WB has. It is a good starting point
for your project. 

Board Support Words
===================

Defaults: Digital port pins D0 to D7 are push pull outputs, D8 to D15
are inputs with pull-up resistors.

    led1!    ( n -- )      sets LED1 (blue)
    led2!    ( n -- )      sets LED2 (green)
    led3!    ( n -- )      sets LED3 (red)
    led1@    ( -- n )      gets LED1 (blue)
    led2@    ( -- n )      gets LED2 (green)
    led3@    ( -- n )      gets LED3 (red)

    switch1? ( -- n )      gets switch1, closed=TRUE
    switch2? ( -- n )      gets switch2, closed=TRUE
    switch3? ( -- n )      gets switch3, closed=TRUE

    dport!   ( n -- )      sets the digital output port (D0=bit0 .. D15=bit15).
    dport@   ( -- n )      gets the digital input/output port (D0=bit0 .. D15=bit15).
    dpin!    ( n a -- )    sets the digital output port pin (D0=0 .. D15=15)
    dpin@    ( a -- n )    gets the digital input/output port pin 
    dmod     ( u a -- )    sets the pin mode: 0 in, 1 in pull-up, 2 in pull-down, 3 out push pull, 4 out open drain, 5 out push pull PWM 

    pwmpin!     ( u a -- )  sets the digital output port pin (D3=3, D6=6, D9=9) to a PWM value (0..1000). Default frequency is 1 kHz, TIMER1
    pwmprescale ( u --  )   Sets the PWM prescale for TIMER1 (D3=3, D6=6, D9=9). 32 kHz / prescale, default 32 -> PWM frequency 1 kHz

    apin@    ( a -- u )    gets the analog input port pin (A0 .. A5). Returns a 12 bit value (0..4095) 

    neopixel! ( rgb -- )   sets the neopixel RGB LED ($ff0000 red, $00ff00 green, $0000ff blue)
    neopixel@ ( -- rgb )   gets the neopixel RGB LED
    neopixels ( a u -- )   sets u pixels at address a




Using the Digital Port Pins (Input and Output)
==============================================

This example is very similar to the Cosmac.McForth\#Knight_Rider
program. `dport!` and `dport@` set and get all 16 digital pins (D0 to
D15) at once. You have to press the *SW1* push button til D0 is set to
cancel the operation.

```forth
: left ( -- ) 
  7 0 do  
    dport@ shl dport!  
    100 osDelay drop  
  loop 
;

: right ( -- )
  7 0 do  
    dport@ shr dport!
    100 osDelay drop  
  loop 
;

: knightrider ( -- )
  1 dport! 
  begin 
    left right 
    switch1? \ or key?
  until 
  0 dport!
;
```


Single port pin variant (no side effects on port pins D8 to D15):

```forth
: left ( -- ) 
  7 0 do
    1 i dpin! 
    100 osDelay drop  
    0 i dpin!
  loop 
;
```

```forth
: right ( -- )
  8 1 do  
    1 8 i - dpin! 
    100 osDelay drop  
    0 8 i - dpin!
  loop 
;
```

```forth
: knigthrider ( -- )
  begin 
    left right 
    switch1? 
  until 
  0 0 dpin!
;
```

Using the ADC (Analog Input Pins)
=================================

`apin@ ( a -- u )` returns the ADC value (12 bit, 0 .. 4095) from one of
the analog pins A0 to A5 (0 .. 5). Here I use the A0 to control the
delay.

```forth
: left ( -- ) 
  7 0 do
    1 i dpin! 
    0 apin@ 10 / osDelay drop  \ delay depends on A0
    0 i dpin!
  loop 
;

: right ( -- )
  8 1 do  
    1 8 i - dpin! 
    0 apin@ 10 / osDelay drop  \ delay depends on A0
    0 8 i - dpin!
  loop 
;
```


To get an idea how fast the ADC, RTOS, and the Forth program are. The
`left` or `right` word takes about 125 us, the `knightrider` loop about
50 us (no osDelay). Pretty fast for my opinion.


Using the PWM (Analog Output Pins)
==================================

Only three port pins are supported so far. The TIMER1 is used for the
timebase, time resolution is 1 us (32 MHz SysClk divided by 32). The PWM
scale is from 0 (0 % duty cycle) to 1000 (100 % duty cycle), this
results in a PWM frequency of 1 kHz. If you need higher PWM frequencies,
decrease the divider and/or the scale.

PWM port pins: D6 (TIM1CH1), D9 (TIM1CH2), D3 (TIM1CH3)

Simple test program to set brightness of a LED on pin D3 with a
potentiometer on A0. Default PWM frequency is 1 kHz (prescaler set to
32). You can set the prescale with the word `pwmprescale` from 32 kHz
(value 1) down to 0.5 Hz (64000).

```forth
5 3 dmod   \ set D3 to PWM

: pwm ( -- )
  begin 
    0 apin@  4 /  3 pwmpin!
    10 osDelay drop
    switch1? 
  until 
;
```


\-- [PeterSchmid - 2020-04-11]
