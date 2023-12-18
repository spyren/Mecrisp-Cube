# Assertation and Logging

For severe failures e.g. stack overflow the only way to recover for embedded systems is a 
reset (warm restart, abort). Other asserts (unknown events) can be ignored because they 
do not have fatal consequences. But for debugging during the development cycle, all asserts 
should be observed. Therefore Mecrisp-Cube has two types of asserts: fatal asserts and 
non-fatal asserts. The non-fatal asserts can be ignored in production systems.

After a fatal assert nothing can be considered as working, even the return from a 
subroutine can be not possible anymore. Therefore the file system cannot be used and 
the log can only be written after a warm restart.

RTC registers are used for accounting the asserts. Those registers are not affected 
by the reset. After the restart the registers are evaluated and the information is 
written to the log.

## How to use

<pre>
fatalassert  ( f n u -- )     abort if flag is false, log n and u after restart       
assert       ( f n u -- )     abort if flag is false and assertion is activated, log n and u after restart
assert?      ( -- f )         Was there an assert?
assert#      ( -- u )         How many asserts occurred since cold startup?
assert@      ( -- u1 u2 )     Assert number u1 and parameter u2 e.g. address where the assert occurred
.assert      ( u -- )         Print assert message
</pre>

If logging is activated (`CFG_LOG_MSG` defined) the logs are written to
`0:/var/log/messages` (not implemented yet)

A cold restart is a CPU reset with RTC domain reset (power cycle, Power On Reset POR). 
A warm restart is a CPU reset without RTC domain reset. 

You can add following line to the `/etc/rc.local`
<pre>
assert? [IF] assert# . .( Assertations since cold startup, last one: ) assert@ drop .assert [THEN]
</pre>

After ^C following message will appear after the greeting:
<pre>
3 Assertations since cold startup, last one: ASSERT_CDC_SIGINT
</pre>


## Implementation

https://en.wikipedia.org/wiki/Assertion_(software_development)
> [!NOTE]
> During development cycle, the programmer will typically run the program with assertions enabled. 
> When an assertion failure occurs, the programmer is immediately notified of the problem. 
>   
> Many assertion implementations will also halt the program's execution: this is useful, since 
> if the program continued to run after an assertion violation occurred, it might corrupt its 
> state and make the cause of the problem more difficult to locate. Using the information provided 
> by the assertion failure (such as the location of the failure and perhaps a stack trace, 
> or even the full program state if the environment supports core dumps or if the program is 
> running in a debugger), the programmer can usually fix the problem. Thus assertions provide 
> a very powerful tool in debugging. 
> 
> When a program is deployed to production, assertions are typically turned off, to avoid any 
> overhead or side effects they may have.

   * https://en.wikipedia.org/wiki/Assert.h
   * https://interrupt.memfault.com/blog/asserts-in-embedded-systems
   * [myassert.c](/peripherals/myassert.c)


## Hardfaults

<pre>
  /* activate divide by zero trap (usage fault) */
  SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
  /* enable Usage-/Bus-/MPU Fault */
  SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk  
             |  SCB_SHCSR_BUSFAULTENA_Msk 
             |  SCB_SHCSR_MEMFAULTENA_Msk; 
</pre>


https://interrupt.memfault.com/blog/cortex-m-fault-debug

