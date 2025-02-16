<table>
  <tr>
    <td><img src="/sdcard/man/img/firefly-header.jpg">
 </td>
    <td><img src="/sdcard/man/img/mecrisp-cube-logo-red-h.svg"></td>
  </tr>
</table> 

Board Support Package
=====================

The board support package for the !STM32WB Firefly Development Board is restricted to the 
Arduino UNO R3 pin header and the onboard LED and switch (button). 
The !STM32 has much more capabilities then 16 digital I/O pins, 5 analog input pins, 
UART, SPI, and !I2C interfaces. But if you want to use the more advanced features you 
can use the !CubeMX to create source code for the internal peripherals. 
This project wants to show how to use the Cube Ecosystem for a Forth system (or vice versa) 
and can't implement all features and possibilities the !STM32WB has. 
It is a good starting point for your project.


Board Support Words
===================

Defaults: Digital port pins D0 to D9 are inputs, except D9 is output for neopixel, D1 for UART Tx. 
D10, D11, D12, and D13 are for the SD card. You are free to use the GPIO pins for other purposes.

```
led1!        ( f -- )         set LED1 (blue)
led1@        ( -- f )         get LED1 (blue)

switch1?     ( -- f )         get switch1, closed=TRUE
dport!       ( n -- )         set the digital output port (D0=bit0 .. D15=bit15).
dport@       ( -- n )         get the digital input/output port (D0=bit0 .. D15=bit15).

dpin!        ( f u -- )       set the digital output port pin u (D0=0 .. D15=15, A0=16 .. A4=20)  to f (TRUE 1, FALSE 0)
dpin@        ( u -- f )       get the digital input/output port pin u
dmod         ( u1 u2 -- )     set the pin u2 to mode u1: 0 in, 1 in pull-up, 2 in pull-down, 3 out push pull, 4 out open drain, 5 out push pull PWM
                                                6 input capture, 7 output compare, 8 I2C, 9 UART, 10 SPI, 11 analog

pwmpin!      ( u1 u2 -- )     set the digital output port pin u2 (D0=0, D1=1, A4=20) to a PWM value u1 (0..1000). Default frequency is 1 kHz, TIMER1
pwmprescale  ( u --  )        set the PWM prescale for TIMER1. 32 kHz / prescale, default 32 -> PWM frequency 1 kHz

EXTImod      ( u1 u2 -- )     set for pin u2 (D3, D5, D6, D7) the EXTI mode u1: 0 rising, 1 falling, 2 both edges, 3 none
EXTIwait     ( u1 u2 -- )     wait for EXTI interrupt on pin u2 (D3, D5, D6, D7), timeout u2 in [ms]

ICOCprescale ( u -- )         set the input capture / output compare prescale for TIMER2. default 32 -> 32 MHz / 32 = 1 MHz, timer resolution 1 us
ICOCperiod!  ( u -- )         set the input capture / output compare (TIMER2) period. default $FFFFFFFF (4'294'967'295). 
                              When the up counter reaches the period, the counter is set to 0. 
                              For prescale 32 the maximum time is about 1 h 11 m
ICOCcount!   ( u -- )         set the input capture / output compare counter for TIMER2
ICOCcount@   ( -- u )         get the input capture / output compare counter for TIMER2
ICOCstart    ( -- )           start the ICOC period
ICOCstop     ( -- )           stop the ICOC period
OCmod        ( u1 u2 -- )     set for pin u2 (D13=13, A2=18, A3=19 the Output Compare mode u1: 0 frozen, 1 active level on match, 2 inactive level on match, 
                              3 toggle on match, 4 forced active, 5 forced inactive
    
OCstart      ( u1 u2 -- )     start the output compare mode for pin u2 with pulse u1
OCstop       ( u -- )         stop output compare for pin u
ICstart      ( u -- )         start input capture u: 0 rising edge, 1 falling edge, 2 both edges
ICstop       ( -- )           stop input capture

waitperiod   ( -- )           wait for the end of the TIMER2 period
OCwait       ( u -- )         wait for the end of output capture on pin u
ICwait       ( u1 -- u2 )     wait for the end of input capture with timeout u1, returns counter u2

apin@        ( u1 -- u2 )     get the analog input port pin u1 (A0 .. A4). Returns a 12 bit value u2 (0..4095) 
vref@        ( -- u )         get the Vref voltage in mV (rather the VDDA, about 3300 mV)
vbat@        ( -- u )         get the Vbat voltage in mV (about 3300 mV)
CPUtemp@     ( -- u )         get CPU temperature in degree Celsius

I2Cput       ( c- u1 u2-- )   put a message with length u (count in bytes) from buffer at a to the I2C slave device u
I2Cget       ( c- u1 u2 -- )  get a message c- with length u1 from I2C slave u2 device to buffer at c-
I2Cputget    ( c- u1 u2 u3 -- ) put a message with length u1 from buffer at c- to the I2C slave device u3
                              and get a message with length u2 from device to buffer at c-

SPIget       ( c- u -- )      get a message with length u from SPI slave device to buffer at c-
SPIput       ( c- u -- )      put a message with length u from buffer at c- to the SPI slave device 
SPIputget    ( c- u1 u2 -- )  put a message with length u1 from buffer at c- to the SPI slave device 
                              and get a message with length u2 from device to buffer at c-
SPImutex     ( -- a- )        get the SPI mutex address
```


Using the Digital Port Pins (Input and Output)
==============================================

This example is a very simple chase lighting program inspired by Knight Rider. 
You need 8 LEDs and 8 resistors.

```forth
: init-port ( -- )
  11 16 dmod   \ set A0/D16 to analog
  8 0 do
    0 i dpin!
    3 i dmod \ port is output
  loop
;

: delay ( -- )
  50 osDelay drop  \ wait 50 ms
;

: left ( -- ) 
  7 0 do
    1 i dpin! 
    delay
    0 i dpin!
  loop 
;

: right ( -- )
  8 1 do  
    1 8 i - dpin! 
    delay
    0 8 i - dpin!
  loop 
;

: knigthrider ( -- )
  init-port
  begin 
    left right 
  switch1? until 
  0 0 pin dpin!
;
```


Using the ADC (Analog Input Pins)
=================================

`apin@ ( a -- u )` returns the ADC value (12 bit, 0 .. 4095) from one of
the analog pins A0 to A2 (0 .. 2). Here I use the A0 to control the
delay.

```forth
: delay ( -- )
  0 apin@ 10 / osDelay drop  \ delay depends on A0
;
```

To get an idea how fast the ADC, RTOS, and the Forth program are. The
`left` or `right` word takes about 125 us, the `knightrider` loop about
50 us (no osDelay). Pretty fast for my opinion (STM32WB55 @ 32 MHz sysclock).

Create a task for the knigthrider (details see [How to Use Tasks](CmsisRtos.md#how-to-use-tasks)) 
to run it in the background:
```
task knigthrider&
knigthrider& construct
' knigthrider knigthrider& start-task
```

Using the PWM (Analog Output Pins)
==================================

Only three port pins are supported so far. The TIMER1 is used for the
timebase, time resolution is 1 us (32 MHz SysClk divided by 32). The PWM
scale is from 0 (0 % duty cycle) to 1000 (100 % duty cycle), this
results in a PWM frequency of 1 kHz. If you need higher PWM frequencies,
decrease the divider and/or the scale.

PWM port pins: D0 (!TIM1CH3), D1 (!TIM1CH2), A4 (!TIM1CH1) 

Simple test program to set brightness of a LED on pin D3 with a
potentiometer on A0. Default PWM frequency is 1 kHz (prescaler set to
32). You can set the prescale with the word `pwmprescale` from 32 kHz
(value 1) down to 0.5 Hz (64000).

```forth
5 0 dmod   \ set D0 to PWM
11 16 dmod   \ set A0/D16 to analog

: pwm ( -- )
  begin 
    0 apin@  4 /  0 pwmpin!
    10 osDelay drop
    switch1? 
  until 
;
```

## Control an RC Servo

https://en.wikipedia.org/wiki/Servo_(radio_control): 
The control signal is a digital PWM signal with a 50 Hz frame rate. Within each 20 ms timeframe, 
an active-high digital pulse controls the position. The pulse nominally ranges from 1.0 ms to 
2.0 ms with 1.5 ms always being center of range. Pulse widths outside this range can be used for 
"overtravel" - moving the servo beyond its normal range. 

A servo pulse of 1.5 ms width will typically set the servo to its "neutral" position (typically 
half of the specified full range), a pulse of 1.0 ms will set it to 0°, and a pulse of 2.0 ms 
to 90° (for a 90° servo). The physical limits and timings of the servo hardware varies between 
brands and models, but a general servo's full angular motion will travel somewhere in the range 
of 90° – 180° and the neutral position (45° or 90°) is almost always at 1.5 ms. This is the 
"standard pulse servo mode" used by all hobby analog servos. 

The BSPs default PWM frequency is 1 kHz, 50 Hz is 20 times slower. The divider is therefore 32 * 20 = 640. 

| angle | time   | n   | 
|-------|--------|-----|
|   0°  | 1 ms   | 50  | 
|  45°  | 1.5 ms | 75  | 
|  90°  | 2 ms   | 100 | 
| 135°  | 2.5 ms | 125 | 
| 180°  | 3 ms   | 150 | 
| 225°  | 3.5 ms | 175 | 
| 270°  | 4 ms   | 200 | 


```forth
640 pwmprescale 
5 0 dmod   \ set D0 to PWM
11 16 dmod   \ set A0/D16 to analog

: servo ( -- ) 
  begin
    100 50 do# Using Input Capture and Output Compare

## Time Base

Default timer resolution is 1 us. The 32 bit TIMER2 is used as time base 
for Input Capture / Output Compare. For a 5 s period 5'000'000 cycles are needed. 
All channels (input capture / output compare) use the same time base.

```forth
: period ( -- )
  5000000 ICOCperiod! \ 5 s period
  ICOCstart
  begin
     waitperiod
     cr .time
  key? until
  key drop 
;
```

## Output Compare

Output compare TIM2: D0 (TIM2CH4), D1 (TIM2CH3), D5 (TIM2CH1)
```forth
7 0 dmod \ output compare for D0
7 1 dmod \ output compare for D1
7 5 dmod \ output compare for D5

: oc-toggle ( -- )
  5000000 ICOCperiod! \ 5 s period
  ICOCstart
  3 0 OCmod  1000000 0 OCstart \ toggle D0 after 1 s
  3 1 OCmod  2000000 1 OCstart \ toggle D1 after 2 s
  3 5 OCmod  3000000 5 OCstart \ toggle D5 after 3 s 
  begin
     waitperiod
     cr .time
  key? until
  key drop 
;
```

When you abort (hit any key) the program, the timer still runs and controls 
the port pins. To stop the port pins:
<pre>
0 OCstop  1 OCstop  5 OCstop  
</pre>

Or change the prescale to make it faster or slower:
<pre>
1 ICOCprescale
</pre>


## Input Capture

This sample program measures the time between the edges on port A2. 
If no event occurs within 2 seconds, "timeout" is issued. 
Hit any key to abort program.
```forth
: ic-test ( -- )
  6 18 dmod \ input capture on A2
  ICOCstart
  2 ICstart  \ both edges
  ICOCcount@ ( -- count )
  begin
    2000 \ 2 s timeout
    ICwait ( -- old-capture capture ) 
    cr
    dup 0= if
      ." timeout" drop
    else 
      dup rot ( -- capture capture old-capture )
      - . ." us"
    then
  key? until
  key drop
  drop
  ICstop
;
```
If  you use a push button for D13, there could be several events on pressing the push button once.
This is called bouncing. 
Bouncing time is about 250 us for my push button.


# Using EXTI line

GPIO pins D2, D4, D7 and D10 can be used as an EXTI line, D4 and D7 share the same EXTI line. 
EXTIs are external interrupt lines, D2 uses EXTI_9_5 (EXTI Line 9..5 interrupt), 
D2 EXTI_9_5, D4 EXTI_15_10, D7 EXTI_15_10, and D10 EXTI_4. 

If  you use a push button for D2, there could be several events on pressing the push button once.
This is called bouncing. 
For details see [A Guide to Debouncing](https://my.eng.utah.edu/~cs5780/debouncing.pdf).

```forth
: exti-test ( -- )# Using Input Capture and Output Compare

## Time Base

Default timer resolution is 1 us. The 32 bit TIMER2 is used as time base 
for Input Capture / Output Compare. For a 5 s period 5'000'000 cycles are needed. 
All channels (input capture / output compare) use the same time base.

```forth
: period ( -- )
  5000000 ICOCperiod! \ 5 s period
  ICOCstart
  begin
     waitperiod
     cr .time
  key? until
  key drop 
;
```

## Output Compare

Output compare TIM2: D0 (TIM2CH4), D1 (TIM2CH3), D5 (TIM2CH1)
```forth
7 0 dmod \ output compare for D0
7 1 dmod \ output compare for D1
7 5 dmod \ output compare for D5

: oc-toggle ( -- )
  5000000 ICOCperiod! \ 5 s period
  ICOCstart
  3 0 OCmod  1000000 0 OCstart \ toggle D0 after 1 s
  3 1 OCmod  2000000 1 OCstart \ toggle D1 after 2 s
  3 5 OCmod  3000000 5 OCstart \ toggle D5 after 3 s 
  begin
     waitperiod
     cr .time
  key? until
  key drop 
;
```

When you abort (hit any key) the program, the timer still runs and controls 
the port pins. To stop the port pins:
<pre>
0 OCstop  1 OCstop  5 OCstop  
</pre>

Or change the prescale to make it faster or slower:
<pre>
1 ICOCprescale
</pre>


## Input Capture

This sample program measures the time between the edges on port A2. 
If no event occurs within 2 seconds, "timeout" is issued. 
Hit any key to abort program.
```forth
: ic-test ( -- )
  6 18 dmod \ input capture on A2
  ICOCstart
  2 ICstart  \ both edges
  ICOCcount@ ( -- count )
  begin
    2000 \ 2 s timeout
    ICwait ( -- old-capture capture ) 
    cr
    dup 0= if
      ." timeout" drop
    else 
      dup rot ( -- capture capture old-capture )
      - . ." us"
    then
  key? until
  key drop
  drop
  ICstop
;
```
If  you use a push button for D13, there could be several events on pressing the push button once.
This is called bouncing. 
Bouncing time is about 250 us for my push button.


# Using EXTI line

GPIO pins D2, D4, D7 and D10 can be used as an EXTI line, D4 and D7 share the same EXTI line. 
EXTIs are external interrupt lines, D2 uses EXTI_9_5 (EXTI Line 9..5 interrupt), 
D2 EXTI_9_5, D4 EXTI_15_10, D7 EXTI_15_10, and D10 EXTI_4. 

If  you use a push button for D2, there could be several events on pressing the push button once.
This is called bouncing. 
For details see [A Guide to Debouncing](https://my.eng.utah.edu/~cs5780/debouncing.pdf).

```forth
: exti-test ( -- )
  2 2 EXTImod \ both edges on D2
  begin
    2000 2 EXTIwait \ wait for edge on D2 with 2 s timeout
    cr
    0= if
      2 dpin@ if
        ." rising edge"
      else
        ." falling edge"
      then 
    else
      ." timeout"
    then
  key? until
  key drop
;
```

# Using Push Buttons

## Switches

Most development boards have at least a switch or a push button, 
the Nucleo has 3 switches and Dongle has 1 switche.

```
switch1? .
```
The result is _0_. But if you press and hold the OK Button, the result will be _-1_. 
There is no debouncing for the `switchx?` words.

  2 2 EXTImod \ both edges on D2
  begin
    2000 2 EXTIwait \ wait for edge on D2 with 2 s timeout
    cr
    0= if
      2 dpin@ if
        ." rising edge"
      else
        ." falling edge"
      then 
    else
      ." timeout"
    then
  key? until
  key drop
;
```

# Using Push Buttons

## Switches

Most development boards have at least a switch or a push button, 
the Nucleo has 3 switches and Dongle has 1 switche.

```
switch1? .
```
The result is _0_. But if you press and hold the OK Button, the result will be _-1_. 
There is no debouncing for the `switchx?` words.

      i 0 pwmpin! 
      i neopixel! 
      i 50 = if 
        1000 \ give some more time to get back
      else
        200
      then 
      osDelay drop
    10 +loop
  key? until 
  key drop
;
```

```forth
640 pwmprescale 
5 0 dmod   \ set D0 to PWM
11 16 dmod   \ set A0/D16 to analog

: slowservo ( -- ) 
  begin
    100 50 do
      i 0 pwmpin! 
      50 osDelay drop
    1 +loop
    50 100 do
      i 0 pwmpin! 
      50 osDelay drop
    -1 +loop
  key? until 
  key drop
;
```

# Using Input Capture and Output Compare

## Time Base

Default timer resolution is 1 us. The 32 bit TIMER2 is used as time base 
for Input Capture / Output Compare. For a 5 s period 5'000'000 cycles are needed. 
All channels (input capture / output compare) use the same time base.

```forth
: period ( -- )
  5000000 ICOCperiod! \ 5 s period
  ICOCstart
  begin
     waitperiod
     cr .time
  key? until
  key drop 
;
```

## Output Compare

Output compare TIM2: D0 (TIM2CH4), D1 (TIM2CH3), D5 (TIM2CH1)
```forth
7 0 dmod \ output compare for D0
7 1 dmod \ output compare for D1
7 5 dmod \ output compare for D5

: oc-toggle ( -- )
  5000000 ICOCperiod! \ 5 s period
  ICOCstart
  3 0 OCmod  1000000 0 OCstart \ toggle D0 after 1 s
  3 1 OCmod  2000000 1 OCstart \ toggle D1 after 2 s
  3 5 OCmod  3000000 5 OCstart \ toggle D5 after 3 s 
  begin
     waitperiod
     cr .time
  key? until
  key drop 
;
```

When you abort (hit any key) the program, the timer still runs and controls 
the port pins. To stop the port pins:
<pre>
0 OCstop  1 OCstop  5 OCstop  
</pre>

Or change the prescale to make it faster or slower:
<pre>
1 ICOCprescale
</pre>


## Input Capture

This sample program measures the time between the edges on port A2. 
If no event occurs within 2 seconds, "timeout" is issued. 
Hit any key to abort program.
```forth
: ic-test ( -- )
  6 18 dmod \ input capture on A2
  ICOCstart
  2 ICstart  \ both edges
  ICOCcount@ ( -- count )
  begin
    2000 \ 2 s timeout
    ICwait ( -- old-capture capture ) 
    cr
    dup 0= if
      ." timeout" drop
    else 
      dup rot ( -- capture capture old-capture )
      - . ." us"
    then
  key? until
  key drop
  drop
  ICstop
;
```
If  you use a push button for D13, there could be several events on pressing the push button once.
This is called bouncing. 
Bouncing time is about 250 us for my push button.


# Using EXTI line

GPIO pins D2, D4, D7 and D10 can be used as an EXTI line, D4 and D7 share the same EXTI line. 
EXTIs are external interrupt lines, D2 uses EXTI_9_5 (EXTI Line 9..5 interrupt), 
D2 EXTI_9_5, D4 EXTI_15_10, D7 EXTI_15_10, and D10 EXTI_4. 

If  you use a push button for D2, there could be several events on pressing the push button once.
This is called bouncing. 
For details see [A Guide to Debouncing](https://my.eng.utah.edu/~cs5780/debouncing.pdf).

```forth
: exti-test ( -- )
  2 2 EXTImod \ both edges on D2
  begin
    2000 2 EXTIwait \ wait for edge on D2 with 2 s timeout
    cr
    0= if
      2 dpin@ if
        ." rising edge"
      else
        ." falling edge"
      then 
    else
      ." timeout"
    then
  key? until
  key drop
;
```

# Using Push Buttons

## Switches

Most development boards have at least a switch or a push button, 
the Nucleo has 3 switches and Dongle has 1 switche.

```
switch1? .
```
The result is _0_. But if you press and hold the OK Button, the result will be _-1_. 
There is no debouncing for the `switchx?` words.


\-- [PeterSchmid - 2020-04-11]
