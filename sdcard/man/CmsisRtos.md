Why a Preemptive Real Time Operating System?
============================================

Forth systems traditionally make use of cooperative multitasking. It is
very simple and clever. But it has its limits. If you write all your
software by yourself, each software part can be cooperative. But if you
want to benefit from middleware written by somebody else (and most
probably not written in Forth), you can be sure that software is not
cooperative (in the context of multitasking). Forth wants to rule your
system. I would like to have a Forth system that is cooperative. It
should extend the system, to make it interactive and easy to use.

The Forth itself is only a thread and can be used as some sort of CLI
for testing purposes or could be the main part of the application.


How to Create a Thread
======================

A very simple thread could be like this one, a boring blinker:

    : blink-thread  ( -- )
      begin 
        led1@ 0= led1!   \ toggle blue LED
        200 osDelay drop  \ wait 200 ms
        switch1? 
      until 
      0 led1! 
    ;

If you type the word `blink-thread`, the blue LED blinks, after push the
button SW1, the blinking stops an the `ok.` apears. But if you try to
start the thread with

    ' blink-thread 0 0 osThreadNew

Nothing happens and probably the Forth system hangs. Restart the Forth
system with the Reset button *SW4*.

If you create a new RTOS Thread, CMSIS-RTOS (FreeRTOS) allocate some
memory from the heap for the stack and the thread control block. But
Forth thread needs another stack, the data stack. The blink-thread runs
concurrent to the Forth interpreter and use the same data stack. This
cannot work. Each thread must have its own data stack, the thread
function can get one with `osNewDataStack` (see below for the assembler
source).

    : blink-thread  ( -- )
      <b>osNewDataStack</b>
      begin 
        led1@ 0= led1!   \ toggle blue LED
        200 osDelay drop  
        switch1? 
      until 
      0 led1! 
      <b>osThreadExit</b>
    ;

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

    // -----------------------------------------------------------------------------
            Wortbirne Flag_visible, "<b>osNewDataStack</b>"
            @ (  --  ) Creates an new data stack for a Forth thread.
    // -----------------------------------------------------------------------------
    rtos_osNewDataStack:
        push    {r0-r3, lr}
        ldr r0, =256    // 64 levels should be more than enough
        bl  pvPortMalloc
        adds    r7, r0, #256    // stack grows down
        movs    tos, 42
        pop {r0-r3, pc}


CMSIS-RTOS API
==============

The C function prototype for `osThreadNew` looks like this:

    osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);

    param[in]     func          thread function.
    param[in]     argument      pointer that is passed to the thread function as start argument.
    param[in]     attr          thread attributes; NULL: default values.

    return        thread ID for reference by other functions or NULL in case of error.

The parameter order for the Forth Word is the same: addr1 is func, addr2
is argument, and addr3 is attr.

    osThreadNew  ( addr1 addr2 addr3 -- u )   Create a thread and add it to Active Threads.

Start the knightrider thread with default parameters and print the
thread ID:

    <b>' knightrider-thread 0 0 osThreadNew .[RET]</b> 536871016 ok.

Stop the thread with pressing button SW1 or

    <b>536871016 osThreadTerminate drop[RET]</b> ok.

Start the Knightrider thread with name=\"Knightrider\" priority=48,
stack_size=256:

    \ buffer for thread attributes
    <b>/osThreadAttr buffer: threadAttr[RET]</b> ok.      
    \ clear the buffer
    <b>threadAttr /osThreadAttr 0 fill[RET]</b> ok.
    \ set the thread name
    <b>16 buffer: threadString[RET]</b> ok.
    <b>threadString .str" Knightrider"[RET]</b> ok.
    \ set the thread parameters
    <b>threadString threadAttr thName+ + ![RET]</b> ok.
    <b>256 threadAttr thStackSize+ + ![RET]</b> ok.
    <b> 48 threadAttr thPriority+  + ![RET]</b> ok.
    \ start the thread
    <b>' knightrider-thread 0 threadAttr osThreadNew .[RET]</b> 0 ok.
    \ print all threads
    <b>.threads[RET]</b>
    Name                State    Priority   Stack Space
    MainThread          Running     24          0754
    CDC_Thread          Blocked     24          0087
    IDLE                Ready       00          0107
    HRS_THREAD          Blocked     24          0380
    <i>Knightrider         Blocked     48          0023</i>
    UART_TxThread       Blocked     40          0217
    HCI_USER_EVT_TH     Blocked     24          0217
    ADV_UPDATE_THRE     Blocked     24          0217
    CRS_Thread          Blocked     40          0207
    SHCI_USER_EVT_T     Blocked     24          0071
    Tmr Svc             Ready       02          0209
    UART_RxThread       Blocked     40          0213
     ok.

See also
[osThreadNew](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__ThreadMgmt.html#ga48d68b8666d99d28fa646ee1d2182b8f)
and MicroSdBlocks\#C_String_Helpers

RTOS Support Functions
----------------------

    osNewDataStack       ( --   )       Creates an new data stack for a Forth thread.
    xPortGetFreeHeapSize ( -- u )       returns the total amount of heap space that remains
    pvPortMalloc         ( u -- addr )  allocate dynamic memory (thread save)
    vPortFree            ( addr -- )    free dynamic memory (thread save)

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

Kernel Management Functions
---------------------------

[Kernel Information and
Control](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__KernelCtrl.html)

-   osKernelGetTickCount
-   osKernelGetTickFreq
-   osKernelGetSysTimerCount
-   osKernelGetSysTimerFreq

Generic Wait Functions
----------------------

[Generic Wait
Functions](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__Wait.html)

-   osDelay
-   osDelayUntil

Thread Management
-----------------

[Thread
Management](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__ThreadMgmt.html)

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

[Timer
Management](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__TimerMgmt.html)

-   osTimerNew
-   osTimerGetName
-   osTimerStart
-   osTimerStop
-   osTimerIsRunning
-   osTimerDelete

Event Flags Management Functions
--------------------------------

[Event
Flags](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__EventFlags.html)

-   osEventFlagsNew
-   osEventFlagsSet
-   osEventFlagsClear
-   osEventFlagsGet
-   osEventFlagsWait
-   osEventFlagsDelete

Mutex Management Functions
--------------------------

[Mutex
Management](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__MutexMgmt.html)

-   osMutexNew
-   osMutexAcquire
-   osMutexRelease
-   osMutexGetOwner
-   osMutexDelete

Semaphore Management Functions
------------------------------

[Semaphores](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__SemaphoreMgmt.html)

-   osSemaphoreNew
-   osSemaphoreAcquire
-   osSemaphoreRelease
-   osSemaphoreGetCount
-   osSemaphoreDelete

Message Queue Management Functions
----------------------------------

[Message
Queue](https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS__Message.html)

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

\-- [PeterSchmid - 2020-04-07]

