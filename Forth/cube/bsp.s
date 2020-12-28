/**
 *  @brief
 *      Board Support Package.
 *
 *		LEDs and switches.
 *  @file
 *      bsp.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-26
 *  @remark
 *      Language: ARM Assembler, STM32CubeIDE GCC
 *  @copyright
 *      Peter Schmid, Switzerland
 *
 *      This project Mecrsip-Cube is free software: you can redistribute it
 *      and/or modify it under the terms of the GNU General Public License
 *      as published by the Free Software Foundation, either version 3 of
 *      the License, or (at your option) any later version.
 *
 *      Mecrsip-Cube is distributed in the hope that it will be useful, but
 *      WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 *      General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with Mecrsip-Cube. If not, see http://www.gnu.org/licenses/.
 */


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led1!"
set_led1:
		@ ( n --  ) set LED1 (blue)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		BSP_setLED1
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led1@"
get_led1:
		@ (  -- n ) get LED1 (blue)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getLED1
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led2!"
set_led2:
		@ ( n --  ) set LED2 (green)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		BSP_setLED2
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led2@"
get_led2:
		@ (  -- n ) get LED2 (green)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getLED2
	movs	tos, r0
	pop		{r0-r3, pc}

	@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led3!"
set_led3:
		@ ( n --  ) set LED3 (red)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos
	drop
	bl		BSP_setLED3
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led3@"
get_led3:
		@ (  -- n ) get LED3 (red)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getLED3
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch1?"
get_switch1:
		@ (  -- n ) get switch1
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getSwitch1
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch2?"
get_switch2:
		@ (  -- n ) get switch2
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getSwitch2
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch3?"
get_switch3:
		@ (  -- n ) get switch3
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getSwitch3
	movs	tos, r0
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dport!"
set_dport:
		@ ( n --  ) Sets the digital output port (D0 .. D15).
// void BSP_setDigitalPort(int state);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// state
	drop
	bl		BSP_setDigitalPort
	pop		{r0-r3, pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dport@"
get_dport:
		@ (  -- n ) Gets the digital input port (D0 .. D15).
// int BSP_getDigitalPort(void);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getDigitalPort
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dpin!"
set_dpin:
		@ ( u a --  ) Sets the digital output port pin (D0 .. D15).
// void BSP_setDigitalPin(int pin_number, int state);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// state
	drop
	bl		BSP_setDigitalPin
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dpin@"
get_dpin:
		@ ( a -- u ) Gets the digital input port pin (D0 .. D15).
// int BSP_getDigitalPin(int pin_number);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	bl		BSP_getDigitalPin
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "apin@"
get_apin:
		@ ( a -- u ) Gets the analog input port pin (A0 .. A5).
// int BSP_getAnalogPin(int pin_number)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	bl		BSP_getAnalogPin
	movs	tos, r0
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dmod"
set_dmod:
		@ ( u a --  ) sets the pin mode: 0 in, 1 in pullup, 2 in pulldown, 3 out pushpull, 4 out open drain.
// int BSP_setDigitalPinMode(int pin_number, int mode)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// mode
	drop
	bl		BSP_setDigitalPinMode
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwmpin!"
set_pwmpin:
		@ ( u a --  ) sets the digital output port pin (D3=3, D6=6, D9=9) to a PWM value (0..1000). Frequency is 1 kHz
// int BSP_setPwmPin(int pin_number, int value)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// value
	drop
	bl		BSP_setPwmPin
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwmprescale"
pwmprescale:
		@ ( u --  ) Sets the PWM prescale for TIMER1 (D3=3, D6=6, D9=9)
// void BSP_setPwmPrescale(int16_t value)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// value
	drop
	bl		BSP_setPwmPrescale
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "wait-alarm"
		@ ( --  ) Wait for alarm A
// void RTC_waitAlarmA()
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		RTC_waitAlarmA
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "alarm!"
		@ ( n1 n2 n3 --  ) sets the alarm
// void RTC_setAlarmA(int hour, int minute, int second)
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r2, tos		// n3 second
	drop
	movs	r1, tos		// n2 minute
	drop
	movs	r0, tos		// n3 hour
	drop
	bl		RTC_setAlarmA
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCprescale"
		@ ( u -- )   Sets the input capture / output compare prescale for TIMER2.
		@            default 32 -> 32 MHz / 32 = 1 MHz, timer resolution 1 us
// void BSP_setPrescaleICOC(uint32_t prescale);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// prescale
	drop
	bl		BSP_setPrescaleICOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCperiod!"
		@ ( u -- )    Sets the input capture / output compare (TIMER2) period.
		@             default $FFFFFFFF (4'294'967'295).
        @             When the up counter reaches the period, the counter is set to 0.
        @              For prescale 32 the maximum time is about 1 h 11 m
// void BSP_setPeriodICOC(uint32_t period);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// period
	drop
	bl		BSP_setPeriodICOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCcount!"
		@ ( u -- )    Sets the input capture / output compare counter for TIMER2
// void BSP_setCounterICOC(uint32_t count);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// count
	drop
	bl		BSP_setCounterICOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCcount@"
		@ ( -- u )    Gets the input capture / output compare counter for TIMER2
// uint32_t BSP_getCounterICOC(void);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	pushdatos
	bl		BSP_getCounterICOC
	movs	tos, r0		// return value
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCstart"
		@ ( -- )      Starts the ICOC period
// void BSP_startPeriodICOC(void) {
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BSP_startPeriodICOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCstop"
		@ ( -- )      Stops the ICOC period
// void BSP_stopPeriodICOC(void) {
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BSP_stopPeriodICOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCmod"
		@ ( u a -- )  Sets for pin a the OC mode u  0 frozen, 1 active level on match,
		@             inactive level on match, 3 toggle on match
        @             4 forced active, 5 forced inactive
// void BSP_setModeOC(int pin_number, uint32_t mode);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// mode
	drop
	bl		BSP_setModeOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCstart"
		@ ( u a -- )  Starts the output compare mode for pin a with pulse u
// void BSP_startOC(int pin_number, uint32_t pulse);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// pulse
	drop
	bl		BSP_startOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCstop"
		@ ( a -- )    Stops output compare for pin a
// void BSP_stopOC(int pin_number);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	bl		BSP_stopOC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICstart"
		@ ( u -- )    Starts input capture u: 0 rising edge, 1 falling edge, 2 both edges
// void BSP_startIC(uint32_t mode);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// mode
	drop
	bl		BSP_startIC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICstop"
		@ ( -- )      Stops input capture
// void BSP_stopIC(void);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BSP_stopIC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "waitperiod"
		@ ( -- )      wait for the end of the TIMER2 period
// void BSP_waitPeriod(void);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	bl		BSP_waitPeriod
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCwait"
		@ ( a -- )    wait for the end of output capture pin a
// void BSP_waitOC(int pin_number);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	bl		BSP_waitIC
	pop		{r0-r3, pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICwait"
		@ ( u -- u )    wait for the end of input capture, returns counter u
// uint32_t BSP_waitIC(uint32_t timeout);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// timeout
	bl		BSP_waitOC
	movs	tos, r0		// return value
	pop		{r0-r3, pc}


