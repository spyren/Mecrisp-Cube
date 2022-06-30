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
		@ ( n --  ) set LED1 (green)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		BSP_setLED1
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led1@"
get_led1:
		@ (  -- n ) get LED1 (green)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getLED1
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led2!"
set_led2:
		@ ( n --  ) set LED2 (yellow)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		BSP_setLED2
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led2@"
get_led2:
		@ (  -- n ) get LED2 (yellow)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getLED2
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led3!"
set_led3:
		@ ( n --  ) set LED3 (red)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		BSP_setLED3
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "led3@"
get_led3:
		@ (  -- n ) get LED3 (red)
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getLED3
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch1?"
get_switch1:
		@ (  -- n ) get switch1
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getSwitch1
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch2?"
get_switch2:
		@ (  -- n ) get switch2
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getSwitch2
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "switch3?"
get_switch3:
		@ (  -- n ) get switch3
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getSwitch3
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dport!"
set_dport:
		@ ( n --  ) Sets the digital output port (D0 .. D15).
// void BSP_setDigitalPort(int state);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// state
	drop
	bl		BSP_setDigitalPort
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dport@"
get_dport:
		@ (  -- n ) Gets the digital input port (D0 .. D15).
// int BSP_getDigitalPort(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getDigitalPort
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dpin!"
set_dpin:
		@ ( u a --  ) Sets the digital output port pin (D0 .. D15).
// void BSP_setDigitalPin(int pin_number, int state);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// state
	drop
	bl		BSP_setDigitalPin
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dpin@"
get_dpin:
		@ ( a -- u ) Gets the digital input port pin (D0 .. D15).
// int BSP_getDigitalPin(int pin_number);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	bl		BSP_getDigitalPin
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "apin@"
get_apin:
		@ ( a -- u ) Gets the analog input port pin (A0 .. A5).
// int BSP_getAnalogPin(int pin_number)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	bl		BSP_getAnalogPin
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "dmod"
set_dmod:
		@ ( u a --  ) sets the pin mode: 0 in, 1 in pullup, 2 in pulldown, 3 out pushpull, 4 out open drain.
		@                                5 out pwm, 6 input capture, 7 output compare, 8 I2C
// int BSP_setDigitalPinMode(int pin_number, int mode)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// mode
	drop
	bl		BSP_setDigitalPinMode
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwmpin!"
set_pwmpin:
		@ ( u a --  ) sets the digital output port pin (D3=3, D6=6, D9=9) to a PWM value (0..1000). Frequency is 1 kHz
// int BSP_setPwmPin(int pin_number, int value)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// value
	drop
	bl		BSP_setPwmPin
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "pwmprescale"
pwmprescale:
		@ ( u --  ) Sets the PWM prescale for TIMER1 (D3=3, D6=6, D9=9)
// void BSP_setPwmPrescale(int16_t value)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// value
	drop
	bl		BSP_setPwmPrescale
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "wait-alarm"
wait_alarm:		@ ( --  ) Wait for alarm A
// void RTC_waitAlarmA()
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		RTC_waitAlarmA
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "alarm!"
set_alarm:
		@ ( n1 n2 n3 --  ) sets the alarm
// void RTC_setAlarmA(int hour, int minute, int second)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r2, tos		// n3 second
	drop
	movs	r1, tos		// n2 minute
	drop
	movs	r0, tos		// n3 hour
	drop
	bl		RTC_setAlarmA
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCprescale"
ICOCprescale:
		@ ( u -- )   Sets the input capture / output compare prescale for TIMER2.
		@            default 32 -> 32 MHz / 32 = 1 MHz, timer resolution 1 us
// void BSP_setPrescaleICOC(uint32_t prescale);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// prescale
	drop
	bl		BSP_setPrescaleICOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCperiod!"
set_ICOCperiod:
		@ ( u -- )    Sets the input capture / output compare (TIMER2) period.
		@             default $FFFFFFFF (4'294'967'295).
        @             When the up counter reaches the period, the counter is set to 0.
        @              For prescale 32 the maximum time is about 1 h 11 m
// void BSP_setPeriodICOC(uint32_t period);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// period
	drop
	bl		BSP_setPeriodICOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCcount!"
set_ICOCcount:
		@ ( u -- )    Sets the input capture / output compare counter for TIMER2
// void BSP_setCounterICOC(uint32_t count);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// count
	drop
	bl		BSP_setCounterICOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCcount@"
get_ICOCcount:
		@ ( -- u )    Gets the input capture / output compare counter for TIMER2
// uint32_t BSP_getCounterICOC(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		BSP_getCounterICOC
	movs	tos, r0		// return value
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCstart"
ICOCstart:
		@ ( -- )      Starts the ICOC period
// void BSP_startPeriodICOC(void) {
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		BSP_startPeriodICOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICOCstop"
ICOCstop:
		@ ( -- )      Stops the ICOC period
// void BSP_stopPeriodICOC(void) {
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		BSP_stopPeriodICOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCmod"
OCmod:
		@ ( u a -- )  Sets for pin a the OC mode u  0 frozen, 1 active level on match,
		@             inactive level on match, 3 toggle on match
        @             4 forced active, 5 forced inactive
// void BSP_setModeOC(int pin_number, uint32_t mode);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// mode
	drop
	bl		BSP_setModeOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCstart"
OCstart:
		@ ( u a -- )  Starts the output compare mode for pin a with pulse u
// void BSP_startOC(int pin_number, uint32_t pulse);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// pulse
	drop
	bl		BSP_startOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCstop"
OCstop:
		@ ( a -- )    Stops output compare for pin a
// void BSP_stopOC(int pin_number);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	bl		BSP_stopOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICstart"
ICstart:
		@ ( u -- )    Starts input capture u: 0 rising edge, 1 falling edge, 2 both edges
// void BSP_startIC(uint32_t mode);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// mode
	drop
	bl		BSP_startIC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICstop"
ICstop:
		@ ( -- )      Stops input capture
// void BSP_stopIC(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		BSP_stopIC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "waitperiod"
waitperiod:
		@ ( -- )      wait for the end of the TIMER2 period
// void BSP_waitPeriod(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		BSP_waitPeriod
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCwait"
OCwait:
		@ ( a -- )    wait for the end of output capture pin a
// void BSP_waitOC(int pin_number);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	bl		BSP_waitOC
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "ICwait"
		@ ( u -- u )    wait for the end of input capture, returns counter u
ICwait:
// uint32_t BSP_waitIC(uint32_t timeout);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// timeout
	bl		BSP_waitIC
	movs	tos, r0		// return value
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "EXTImod"
		@ ( u a --  )    Sets for pin a the EXTI mode u: 0 rising edge, 1 falling edge, 2 both edges, 3 none
EXTImod:
// void BSP_setModeEXTI(int pin_number, uint32_t mode)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// mode
	drop
	bl		BSP_setModeEXTI
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "EXTIwait"
		@ ( u a --  u )    Waits for EXTI line (interrupt driven by port pin edge)
EXTIwait:
// void BSP_waitEXTI(int pin_number, int32_t timeout)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos		// pin_number
	drop
	movs	r1, tos		// timeout
	bl		BSP_waitEXTI
	movs	tos, r0		// return value
	pop		{pc}


// I2C
// ***

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "I2Cdev"
I2dev:
        @ ( c -- ) Sets the I2C slave device
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos
	drop
	bl		IIC_setDevice
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "I2Cget"
I2Cget:
        @ ( a size -- ) Get a message
// int IIC_getMessage(uint8_t *RxBuffer, uint32_t RxSize)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r1, tos			// RxSize
	drop
	movs	r0, tos			// *RxBuffer
	drop
	bl		IIC_getMessage
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "I2Cput"
I2Cput:
        @ ( a size --  ) Put a message
// int IIC_putMessage(uint8_t *TxBuffer, uint32_t TxSize)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r1, tos			// TxSize
	drop
	movs	r0, tos			// *TxBuffer
	drop
	bl		IIC_putMessage
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "I2Cputget"
I2Cputget:
        @ ( a1 size1 a2 size2 --  ) Put and get a message
// int IIC_putGetMessage(uint8_t *TxBuffer, uint32_t TxSize, uint8_t *RxBuffer, uint32_t RxSize)
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r3, tos			// RxSize
	drop
	movs	r2, tos			// *RxBuffer
	drop
	movs	r1, tos			// TxSize
	drop
	movs	r0, tos			// *TxBuffer
	drop
	bl		IIC_putGetMessage
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "SPIputc"
        @ ( c --  ) Put a message
// void EXTSPI_Write(uint8_t Value)
@ -----------------------------------------------------------------------------
SPIputc:
	push	{lr}
	movs	r0, tos			// Value
	drop
	bl		EXTSPI_Write
	pop		{pc}

@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "SPIputget"
       @ ( a1 a2 size2 --  ) Put and get a message
// void EXTSPI_WriteReadData(const uint8_t *DataIn, uint8_t *DataOut, uint16_t DataLength)
@ -----------------------------------------------------------------------------
SPIputget:
 	push	{lr}
	movs	r2, tos			// DataLength
	drop
	movs	r1, tos			// *DataOut
	drop
	movs	r0, tos			// *DataIn
	drop
	bl		EXTSPI_WriteReadData
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "watchdog"
watchdog:
	@ ( -- )      activate watchdog
// void WATCHDOG_activate(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	bl		WATCHDOG_activate
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "watchdog?"
get_watchdog_bitten:
	@ ( -- flag )      Has the WATCHDOG bitten?
// int WATCHDOG_bitten(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		WATCHDOG_bitten
	cmp		r0, #0
	beq		1f
	ldr		r0, =-1	// true
1:
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "watchdog#"
get_watchdog_bites:
	@ ( -- u )      How many times has the watchdog bitten since cold startup?
// int WATCHDOG_bites(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		WATCHDOG_bites
	movs	tos, r0			// bites
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "watchdog@"
get_watchdog_adr:
	@ ( -- a )      How many times has the watchdog bitten since cold startup?
// int WATCHDOG_adr(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		WATCHDOG_adr
	movs	tos, r0			// address
	pop		{pc}


@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "assert"
assert:
	@ ( -- u )      How many asserts occurred since cold startup?
// int ASSERT_getCount(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		ASSERT_getCount
	movs	tos, r0			// count
	pop		{pc}



@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "assert?"
get_assert:
	@ ( -- flag )      ASSERT occurred since cold startup?
// int ASSERT_occurred(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		ASSERT_occurred
	cmp		r0, #0
	beq		1f
	ldr		r0, =-1	// true
1:
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "assert#"
get_assert_count:
	@ ( -- u )      How many asserts occurred since cold startup?
// int ASSERT_getCount(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		ASSERT_getCount
	movs	tos, r0			// count
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, "assert@"
get_assert_adr:
	@ ( -- u1 u2 )      Assert number u1 and parameter u2 e.g. address where the assert occurred
// int ASSERT_getId(void);  int ASSERT_getParam(void);
@ -----------------------------------------------------------------------------
	push	{lr}
	pushdatos
	bl		ASSERT_getId
	movs	tos, r0			// ID
	pushdatos
	bl		ASSERT_getParam
	movs	tos, r0			// Parameter
	pop		{pc}

@ -----------------------------------------------------------------------------
	Wortbirne Flag_visible, ".assert"
print_assert:
	@ ( u --  )      Print assert message
// char* ASSERT_getMsg(int index);
@ -----------------------------------------------------------------------------
	push	{lr}
	movs	r0, tos			// u
	bl		ASSERT_getMsg
	movs	tos, r0			// string
	bl		fs_strlen
	bl		stype
	pop		{pc}


