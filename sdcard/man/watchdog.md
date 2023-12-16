# Watchdog

For serious real time systems you need a [watchdog](https://en.wikipedia.org/wiki/Watchdog_timer)
to detect and recover from malfunctions e.g. deadlocks.  STM32 MCUs have two of them: independent 
watchdog (IWDG) and window watchdog (WWDG). 
I prefer the WWDG because it has a time window (as the name implies) and an interrupt just before 
the watchdog is going to bite. Window means that the feeding time has to be in a time 
window (interval), to feed to early is harmful too. 
The early wakeup interrupt can be used to bring the system into a save state or log something 
before the CPU is going into reset.

There is a thread only for kicking (regularly feed) the watchdog. This thread has the second 
lowest priority (just higher than the idle thread), if any task does not want to give up control, 
the watchdog thread does not get any more CPU time and cannot feed the watchdog and the watchdog 
will bite after the timeout.

## How to use

```
watchdog     ( -- )        Activate watchdog
watchdog?    ( -- flag )   Has the WATCHDOG bitten?
watchdog#    ( -- u )      How many times has the watchdog bitten since cold startup?
watchdog@    ( -- addr )   Address where the watchdog bit
```

If a thread takes longer than about 500 ms the watchdog will bite and a reset occurs. 
As long the thread waits for an event e.g. file operation, keyboard (the thread is blocked), 
nothing happens.
<pre>
<b>: forever begin again ; [RET]</b> ok.
<b>watchdog? .[RET]</b> 0  ok.
<b>watchdog[RET]</b>  ok.
<b>forever[RET]</b> BOOM
Mecrisp-Stellaris RA 2.5.4 by Matthias Koch.

Mecrisp-Cube 1.4.4 for STM32WB55, 63/128 KiB RAM/FLASH dictionary (C) 2021 peter@spyr.ch
  * Firmware Package STM32Cube FW_WB V1.11.0, USB-CDC, BLE Stack 5.0 (C) 2021 STMicroelectronics
  * CMSIS-RTOS V2 FreeRTOS wrapper, FreeRTOS Kernel V10.3.1 (C) 2020 Amazon.com
  * FatFs for internal flash and microSD - Generic FAT fs module  R0.12c (C) 2017 ChaN
  * tiny vi - part of BusyBox (C) 2000, 2001 Sterling Huxley

<b>watchdog? .[RET]</b> -1  ok.
<b>watchdog# .[RET]</b> 1  ok.

</pre>

Do not activate the watchdog in debugging mode.

<pre>
watchdog? [IF] .( Watchdog has bitten at ) watchdog@ hex. .( , # bites: ) watchdog# . cr [THEN]
</pre>

## Implementation

For !STM32WB55:
<pre>
WWDG clock (Hz) = PCLK1 / (4096 * Prescaler)

PCLK1 is 32 MHz, Prescaler is 64, 
clock = 32 MHz / (4096 * 64) = 122 Hz

WWDG timeout (mS) = 1000 * (T[5;0] + 1) / WWDG clock (Hz)

timeout = 1000 * 64 / 122 = 524 ms
window  = 1000 * 48 / 122 = 393 ms
</pre>

I choose 450 ms period to feed the watchdog.

RTC registers are used for accounting the watchdog bites. Those registers are not affected by the reset.

For implementation details see:
   * [watchdog.c](/peripherals/watchdog.c)
   * [watchdog.h](/peripherals/watchdog.h)
   * [wwdg.c](/Core/Src/wwdg.c)
   * [clock.h](/peripherals/clock.h)
   * 
