<link rel="shortcut icon" type="image/x-icon" href="favicon.ico">
<table>
  <tr>
    <td><img src="img/CMSIS_Logo_Final.png" ></td>
    <td><img src="img/FreeRTOS.jpg"  ></td>
    <td><img src="img/mecrisp-cube-4th-logo-red-half.svg"  ></td>
  </tr>
</table> 

Why a Preemptive Real Time Operating System?
============================================

Forth systems traditionally make use of cooperative multitasking. 
It is very simple and clever. But it has its limits. 
If you write all your software by yourself, each software part can be cooperative. 
But if you want to benefit from middleware written by somebody else 
(and most probably not written in Forth), you can be sure that software is 
not cooperative (in the context of multitasking). Forth wants to rule your system. 
I would like to have a Forth system that is cooperative. 
It should extend the system, to make it interactive and easy to use.

The Forth interpreter (called terminal task in Forth jargon) itself 
is only a thread and can be used as some sort of CLI for testing purposes 
or could be the main part of the application. 


# Forth Multitasking

Andrew Haley wrote "Forth has been multi-tasking for almost 50 years. 
It's time to standardize it" and he is right. I will implement his 
proposed API for Mecrisp-Cube described in 
[A multi-tasking wordset for Standard Forth](http://www.complang.tuwien.ac.at/anton/euroforth/ef17/papers/haley-slides.pdf). 
The multitasker wordset is very similar to the one in SwiftForth / PolyForth.

I use the term task here because it is well known in the Forth world, 
although Mecrisp-Cube make use of threads. Mecrisp-Cube tasks are CMSIS-RTOS 
threads with user variables. The Mecrisp-Cube (CMSIS-RTOS / FreeRTOS) 
scheduler is pre-emptive and not round robin (cooperative). Mecrisp-Cube 
is always multi tasked, you can not switch off the scheduler and 
therefore there is no `MULTI`, `SINGLE`, or `INIT-MULTI`.


## Terminal Task

Mecrisp-Cube has only one terminal task. Soleley this task is 
allowed to define words for the dictionary. 

Following variables / buffers are exclusively for the terminal task:
   * User Input and Interpretation
      * `Eingabepuffer` 200 Bytes (TIB)
      * `Pufferstand` (>in)
      * `current_source` double user variable
   * Pictured Numerical Output
      * `Zahlenpuffer` 64 Bytes 
      * `Zahlenpufferlaenge`
   * `PAD` Scratch storage e.g. for strings. 100 bytes


## Background Task

Background tasks have their own user variables (see below). 


## User Variables

One of a set of variables provided by Forth, whose values are 
unique for each task. The defining word =user= behaves in the same
way as `variable`. The difference is that it reserves space in 
user (data) space rather than normal data space. In a Forth system 
that has a multi-tasker, each task has its own set of user variables.


### Words

Dictionary commands like `user` and `+user` can be used only in the terminal task.
<pre>
user   ( n "name" -- )         Define a user variable at offset n in the user area
#user  (  -- n )               Return the number of bytes currently allocated in a user area. 
/user  (  -- n )               user variable area size
his    ( addr1 n -- addr2 )    Given a task address addr1 (TCB) and user variable offset n, returns the address of 
                               the referenced user variable in that task's user area. 
not implemented:
+user  ( n1 n2 "name" -- n3 )  Define a user variable at offset n1 in the user area, and increment the offset 
                               by the size n2 to give a new offset n3.
</pre>


### Predefined User Variables

`hook-*` user variables are for terminal redirection, eg. to write 
to a display or another serial channel. The user variables `stdin` 
and `stdout` are used for file redirection, see 
MicroSdBlocks#File_Redirection.

<pre>
Offset Variable
0      threadid    CMSIS-RTOS thread ID
4      argument    Argument for the CMSIS-RTOS thread creation
8      attr        Attributes for CMSIS-RTOS thread creation 
                      osThreadNew(osThreadFunc_t func, void <b>*argument</b>, const osThreadAttr_t <b>*attr</b>);
12     XT          execution token for the task word
16     R0          (R-zero) is the address of the bottom of the return stack
20     S0          (S-zero) is the address of the bottom of the data stack
24     base
28     hook-emit
32     hook-key
36     hook-emit?
40     hook-key?
44     stdin
48     stdout
52     stderr
56     user area, 17 cells (#user returns the offset of the user area)

not implemented yet:
pad tib >in in> blk hld dpl
</pre>


### Implementation

CMSIS-RTOS does not support Thread Local Storage, but FreeRTOS does, 
details see https://www.freertos.org/thread-local-storage-pointers.html 
e.g `vTaskSetThreadLocalStoragePointer()` and `pvTaskGetThreadLocalStoragePointer()`. 
Thread Flag 15 ($8000) is used for STOP/AWAKEN.


## Task Management

<pre>
task       ( "name" -- )        Creates a task control block TCB. Invoking "name" returns the address of the task's Task Control Block (TCB).
/task      ( -- n )             n is the size of a Task Control block. 
                                This word allows arrays of tasks to be created without having to name each one.
construct  ( addr -- )          Instantiate the task whose TCB is at addr. 
                                This creates the TCB and initialize the user variables
                                After this, user variables may be changed before the task is started
start-task ( xt addr -- )       Start the task at addr asynchronously executing the word whose execution token is xt

stop       ( -- )               blocks the current task unless or until AWAKEN has been issued, waits for thread flag 15
awaken     ( addr -- )          wake up the task, sets the thread flag 15

mutex-init ( addr -- )          Initialize a mutex. Set its state to released.
/mutex     ( -– n)              n is the number of bytes in a mutex.

get        ( addr -- )          Obtain control of the mutex at addr. If the mutex is owned by another task, 
                                the task executing GET will wait until the mutex is available.
release    ( addr –- )          Relinquish the mutex at addr


[C         ( -- )               Begin a critical section. Other tasks cannot execute during a critical section, but interrupts can. !osKernelLock
C]         ( -- )               Terminate a critical section. !osKernelRestoreLock 

terminate  ( -- )               Causes the task executing this word to cease operation !osThreadTerminate
suspend    ( addr -- ior)       Force the task whose TCB is at addr to suspend operation indefinitely. !osThreadSuspend
resume     ( addr -- ior)       Cause the task whose TCB is at addr to resume operation at the point at which it !osThreadResume 
                                was SUSPENDed (or where the task called STOP).
halt       ( addr -- )          Caution! not working yet.
				Cause the task whose TCB is at addr to cease operation permanently, but to remain instantiated. 
                                The task may be reactivated (through start-task).
kill       ( addr -- )          Cause the task whose TCB is at addr to cease operation and release all its TCB memory. 

pause      ( -- )

skeleton   ( -- )               skeleton for tasks (creates the stacks for the task)
</pre>

See also:
   * FORTH MULTITASKING IN A NUTSHELL https://www.bradrodriguez.com/papers/mtasking.html
   * https://theforth.net/package/multi-tasking


# How to use Tasks

Create a Task Control Block (TCB) with the name `blinker&` (the ampersand at 
the end of the word is UNIX style, it means to start the process in the background 
and the shell job control does not wait till the process terminates).
<pre>
task blinker&
</pre>

Initialize the TCB (user variables)
<pre>
blinker& construct
</pre>

Start the task 
<pre>
' blinker blinker& start-task
</pre>

Another task to write date and time every second to the OLED:
```forth
: clock (  -- )
  >oled            \ redirect terminal to oled-emit
  oledclr
  3 oledfont
  -1 -1 -1 alarm!  \ set an alarm every second
  begin
    wait-alarm     \ wait a second
    0 0 oledpos!
    .time
  again
;

task clock&
clock& construct
' clock clock& start-task
```

For implementation details see:
   * [rtos.s](/Forth/cube/rtos.s)
   * [rtos.c](/peripherals/rtos.c)


Show XT (execution token) for the task:
<pre>
clock& xt threadid - + @ hex. 200004B0  ok.
</pre>

no suprise, it's the XT from the clock word:
<pre>
' clock hex. 200004B0
</pre>

look for the word in the dictionary
<pre>
$200004a0 10 dump
200004A0 :  00 0B 06 00 80 01 00 08   00 00 <b>05 63 6C 6F 63 6B</b>  | ........  ...<b>clock</b> |
</pre>

>body 4780


How to Create a Thread
======================

You can use RTOS threads direct without task management. 

A very simple thread could be like this one, a boring blinker:
```forth
: blinker  ( -- )
  $01 -sysled \ gain control over the RGB LED
  $000000 rgbled!
  begin 
    rgbled@ 0= if
      $0000ff
    else
      $000000
    then
    rgbled!
    200 osDelay drop  \ wait 200 ms
  switch1? until 
  $01 +sysled \ giving up control over the RGB LED
;
```

If you type the word `blinker`, the blue LED blinks, after push the
button SW1, the blinking stops an the `ok.` apears. But if you try to
start the thread with
```forth
' blinker 0 0 osThreadNew
```

Nothing happens and probably the Forth system hangs. Restart the Forth
system with the Reset button *SW4*.

If you create a new RTOS Thread, CMSIS-RTOS (FreeRTOS) allocate some
memory from the heap for the stack and the thread control block. But
Forth thread needs another stack, the data stack. The blink-thread runs
concurrent to the Forth interpreter and use the same data stack. This
cannot work. Each thread must have its own data stack, the thread
function can get one with `osNewDataStack` (see below for the assembler
source).
```forth
: blink-thread  ( -- )
  osNewDataStack
  blinker
  osThreadExit
;

' blink-thread 0 0 osThreadNew
```

`osThreadExit` is needed to exit the thread, otherwise the Forth system
hangs after leaving the thread. These threads are very similar to the
*control tasks* described in Starting Forth, Leo Brodie. But without
*user variables*. If a thread wants to use variables and share these
variables with other threads, the variables have to be protected by a
mutex or a semaphore. Anyway variables have to be created by the main
Forth thread (terminal task) before.

Now you can interactively play with the words `osThreadGetId`,
`osThreadGetState`, `osThreadSuspend`, and `osThreadResume` without the
tedious edit-compile-download-run-abort.
```assembly
// -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "osNewDataStack"
        @ (  --  ) Creates an new data stack for a Forth thread.
// -----------------------------------------------------------------------------
rtos_osNewDataStack:
    push    {r0-r3, lr}
    ldr     r0, =256    // 64 levels should be more than enough
    bl      pvPortMalloc
    adds    r7, r0, #256    // stack grows down
    movs    tos, 42
    pop     {r0-r3, pc}
```

# Interrupts and Forth

Mecrisp uses CPU register R7 as data stack pointer and R6 as TOS. 
If Forth rules the system R7 is always the data stack pointer 
and you can use the data stack pointer within a interrupt service routine. 
But in a mixed system, where C routines are used, there is no guarantee 
that the register R7 remains unchanged. When the interrupt occurs 
while a C routine is executed, the data stack pointer is invalid and 
Forth words can't be used in interrupt service routines. 
A possible solution would be a separate data stack for the ISRs. 
I don't do that and use C for the ISRs. 

The Forth threads are synchronized by RTOS IPCs like semaphores, 
e.g. the ISR Release a semaphore and the Forth thread aquire the 
same semaphore, like the sample below.

For details see [bsp.c](/Forth/Src/bsp.c).
```C
/**
  * @brief  Output Compare callback in non-blocking mode
  * @param  htim TIM OC handle
  * @retval None
  */
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM2) {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
			// D5, PA15
			osSemaphoreRelease(ICOC_CH1_SemaphoreID);
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3) {
			// D1, PA2
			osSemaphoreRelease(ICOC_CH3_SemaphoreID);
		}
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {
			// D0, PA3
			osSemaphoreRelease(ICOC_CH4_SemaphoreID);
		}
	}
}


/**
 *  @brief
 *      Waits for Output Compare.
 *	@param[in]
 *      pin_number  port pin 0 D0, 1 D1, or 5 D5
 *  @return
 *      none
 */
void BSP_waitOC(int pin_number) {
	switch (pin_number) {
	case 0:
		osSemaphoreAcquire(ICOC_CH4_SemaphoreID, osWaitForever);
		break;
	case 1:
		osSemaphoreAcquire(ICOC_CH3_SemaphoreID, osWaitForever);
		break;
	case 5:
		osSemaphoreAcquire(ICOC_CH1_SemaphoreID, osWaitForever);
		break;
	}
}
```

`OCwait` Forth word waits for the event Output Compare, timer 
interrupt assigned to a port pin. Details [bsp.s]](/Forth/cube/bsp.s).
<pre>
@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "OCwait"
OCwait:
		@ ( a -- )    wait for the end of output capture pin a
// void BSP_waitOC(int pin_number);
@ -----------------------------------------------------------------------------
	push	{r0-r3, lr}
	movs	r0, tos		// pin_number
	drop
	bl	BSP_waitOC
	pop	{r0-r3, pc}
</pre>



CMSIS-RTOS API
==============

The C function prototype for `osThreadNew` looks like this:
```
osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);

param[in]     func          thread function.
param[in]     argument      pointer that is passed to the thread function as start argument.
param[in]     attr          thread attributes; NULL: default values.

return        thread ID for reference by other functions or NULL in case of error.
```

The parameter order for the Forth Word is the same: addr1 is func, addr2
is argument, and addr3 is attr.
```
osThreadNew  ( addr1 addr2 addr3 -- u )   Create a thread and add it to Active Threads.
```

Start the knightrider thread with default parameters and print the
thread ID:
```
' knightrider-thread 0 0 osThreadNew .[RET] 536871016 ok.
```

Stop the thread with pressing button SW1 or
```
536871016 osThreadTerminate drop[RET] ok.
```

Start the Knightrider thread with name=\"Knightrider\" priority=48,
stack_size=256:
```
\ buffer for thread attributes
/osThreadAttr buffer: threadAttr[RET] ok.      
\ clear the buffer
threadAttr /osThreadAttr 0 fill[RET] ok.
\ set the thread name
16 buffer: threadString[RET] ok.
threadString .str" Knightrider"[RET] ok.
\ set the thread parameters
threadString threadAttr thName+ + ![RET] ok.
256 threadAttr thStackSize+ + ![RET] ok.
 48 threadAttr thPriority+  + ![RET] ok.
\ start the thread
' knightrider-thread 0 threadAttr osThreadNew .[RET] 0 ok.
\ print all threads
.threads[RET]
Name                State    Priority   Stack Space
MainThread          Running     24          0754
CDC_Thread          Blocked     24          0087
IDLE                Ready       00          0107
HRS_THREAD          Blocked     24          0380
Knightrider         Blocked     48          0023
UART_TxThread       Blocked     40          0217
HCI_USER_EVT_TH     Blocked     24          0217
ADV_UPDATE_THRE     Blocked     24          0217
CRS_Thread          Blocked     40          0207
SHCI_USER_EVT_T     Blocked     24          0071
Tmr Svc             Ready       02          0209
UART_RxThread       Blocked     40          0213
 ok.
```

See also
[osThreadNew](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__ThreadMgmt.html#ga48d68b8666d99d28fa646ee1d2182b8f)
and MicroSdBlocks\#C_String_Helpers

RTOS Support Functions
----------------------

```
osNewDataStack       ( --   )       Creates an new data stack for a Forth thread.
xPortGetFreeHeapSize ( -- u )       returns the total amount of heap space that remains
pvPortMalloc         ( u -- addr )  allocate dynamic memory (thread safe)
vPortFree            ( addr -- )    free dynamic memory (thread safe)

/osThreadAttr        ( -- u ) Gets the osThreadAttr_t structure size
thName+              ( -- u ) Gets the osThreadAttr_t structure name attribut offset
thAttrBits+          ( -- u ) Gets the osThreadAttr_t structure attr_bits attribut offset
thCbMem+             ( -- u ) Gets the osThreadAttr_t structure size attribut offset
thCbSize+            ( -- u ) Gets the osThreadAttr_t structure cb_size attribut offset
thStackMem+          ( -- u ) Gets the osThreadAttr_t structure stack_mem attribut offset
thStackSize+         ( -- u ) Gets the osThreadAttr_t structure stack_size attribut offset
thPriority+          ( -- u ) Gets the osThreadAttr_t structure priority attribut offset
thTzModule+          ( -- u ) Gets the osThreadAttr_t structure tz_module attribut offset
/osEventFlagsAttr    ( -- u ) Gets the osEventFlagsAttr_t structure size
/osMessageQueueAttr  ( -- u ) Gets the osMessageQueueAttr_t structure size
/osMutexAttr         ( -- u ) Gets the osMutexAttr_t structure size
/osSemaphoreAttr     ( -- u ) Gets the osSemaphoreAttr_t structure size
```

Kernel Management Functions
---------------------------

[Kernel Information and Control](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__KernelCtrl.html)

-   osKernelGetTickCount
-   osKernelGetTickFreq
-   osKernelGetSysTimerCount
-   osKernelGetSysTimerFreq

Generic Wait Functions
----------------------

[Generic Wait Functions](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__Wait.html)

-   osDelay
-   osDelayUntil

Thread Management
-----------------

![](img/threadstatus.png)

[Thread Management](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__ThreadMgmt.html)

-   osThreadNew default Attributes if attr==NULL: name=\"\", priority=24
    (osPriorityNormal), stack_size=128
-   osThreadGetId
-   osThreadGetName
-   osThreadGetState
-   osThreadSetPriority
-   osThreadGetPriority
-   osThreadYield
-   osThreadSuspend
-   osThreadResume
-   osThreadExit
-   osThreadTerminate
-   osThreadGetStackSpace
-   osThreadGetCount
-   osThreadEnumerate

Timer Management Functions
--------------------------

![](img/Timer.png)

[Timer Management](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__TimerMgmt.html)

-   osTimerNew
-   osTimerGetName
-   osTimerStart
-   osTimerStop
-   osTimerIsRunning
-   osTimerDelete

Event Flags Management Functions
--------------------------------

![](img/simple_signal.png)

[Event Flags](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__EventFlags.html)

-   osEventFlagsNew
-   osEventFlagsSet
-   osEventFlagsClear
-   osEventFlagsGet
-   osEventFlagsWait
-   osEventFlagsDelete

Mutex Management Functions
--------------------------

![](img/mutex.png)

[Mutex Management](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__MutexMgmt.html)

-   osMutexNew
-   osMutexAcquire
-   osMutexRelease
-   osMutexGetOwner
-   osMutexDelete

Semaphore Management Functions
------------------------------

![](img/Semaphore.png)

[Semaphores](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__SemaphoreMgmt.html)

-   osSemaphoreNew
-   osSemaphoreAcquire
-   osSemaphoreRelease
-   osSemaphoreGetCount
-   osSemaphoreDelete

Message Queue Management Functions
----------------------------------

![](img/messagequeue.png)

[Message Queue](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__Message.html)

-   osMessageQueueNew
-   osMessageQueuePut
-   osMessageQueueGet
-   osMessageQueueGetCapacity
-   osMessageQueueGetMsgSize
-   osMessageQueueGetCount
-   osMessageQueueGetSpace
-   osMessageQueueReset
-   osMessageQueueDelete

A multi-tasking wordset for Standard Forth, Andrew Haley
<http://www.complang.tuwien.ac.at/anton/euroforth/ef17/papers/haley-slides.pdf>

