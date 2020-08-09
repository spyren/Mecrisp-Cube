/**
 *  @brief
 *      Forth API to the CMSIS-RTOSv2 functions.

 *  @file
 *      rtos.s
 *  @author
 *      Peter Schmid, peter@spyr.ch
 *  @date
 *      2020-03-11
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


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osNewDataStack"
		@ (  --  ) Creates an new data stack for a Forth thread.
// -----------------------------------------------------------------------------
rtos_osNewDataStack:
	push	{r0-r3, lr}
	ldr		r0, =256		// 64 levels should be more than enough
	bl		pvPortMalloc
	adds	r7, r0, #256	// stack grows down
	movs	tos, 42
	pop		{r0-r3, pc}


// RTOS datastructures
// *******************

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#osThreadAttr"
		@ ( -- u ) Gets the osThreadAttr_t structure size
// int RTOS_osThreadAttr_size(void)
@ -----------------------------------------------------------------------------
zifferosThreadAttr:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_size
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thName+"
		@ ( -- u ) Gets the osThreadAttr_t structure name attribut offset
// int RTOS_osThreadAttr_name(void)
@ -----------------------------------------------------------------------------
thNameplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_name
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thAttrBits+"
		@ ( -- u ) Gets the osThreadAttr_t structure attr_bits attribut offset
// int RTOS_osThreadAttr_attr_bits(void)
@ -----------------------------------------------------------------------------
thAttrBitsplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_attr_bits
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thCbMem+"
		@ ( -- u ) Gets the osThreadAttr_t structure size attribut offset
// int RTOS_osThreadAttr_cb_mem(void)
@ -----------------------------------------------------------------------------
thCbMemplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_cb_mem
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thCbSize+"
		@ ( -- u ) Gets the osThreadAttr_t structure cb_size attribut offset
// int RTOS_osThreadAttr_cb_size(void)
@ -----------------------------------------------------------------------------
thCbSizeplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_cb_size
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thStackMem+"
		@ ( -- u ) Gets the osThreadAttr_t structure stack_mem attribut offset
// int RTOS_osThreadAttr_stack_mem(void)
@ -----------------------------------------------------------------------------
thStackMemplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_stack_mem
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thStackSize+"
		@ ( -- u ) Gets the osThreadAttr_t structure stack_size attribut offset
// int RTOS_osThreadAttr_stack_size(void)
@ -----------------------------------------------------------------------------
thStackSizeplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_stack_size
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thPriority+"
		@ ( -- u ) Gets the osThreadAttr_t structure priority attribut offset
// int RTOS_osThreadAttr_priority(void)
@ -----------------------------------------------------------------------------
thPriorityplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_priority
	movs	tos, r0
	pop		{pc}

@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "thTzModule+"
		@ ( -- u ) Gets the osThreadAttr_t structure tz_module attribut offset
// int RTOS_osThreadAttr_tz_module(void)
@ -----------------------------------------------------------------------------
thTzModuleplus:
	push	{lr}
	pushdatos
	bl		RTOS_osThreadAttr_tz_module
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#osEventFlagsAttr"
		@ ( -- u ) Gets the osEventFlagsAttr_t structure size
// int RTOS_EventFlagsAttr_size(void)
@ -----------------------------------------------------------------------------
zifferosEventFlagsAttr:
	push	{lr}
	pushdatos
	bl		RTOS_osEventFlagsAttr_size
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#osMessageQueueAttr"
		@ ( -- u ) Gets the osMessageQueueAttr_t structure size
// int RTOS_osMessageQueueAttr_size(void)
@ -----------------------------------------------------------------------------
zifferosMessageQueueAttr:
	push	{lr}
	pushdatos
	bl		RTOS_osMessageQueueAttr_size
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#osMutexAttr"
		@ ( -- u ) Gets the osMutexAttr_t structure size
// int RTOS_osMutexAttr_size(void)
@ -----------------------------------------------------------------------------
zifferosMutexAttr:
	push	{lr}
	pushdatos
	bl		RTOS_osMutexAttr_size
	movs	tos, r0
	pop		{pc}


@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "#osSemaphoreAttr"
		@ ( -- u ) Gets the osSemaphoreAttr_t structure size
// int RTOS_osSemaphoreAttr_size(void)
@ -----------------------------------------------------------------------------
zifferosSemaphoreAttr:
	push	{lr}
	pushdatos
	bl		RTOS_osSemaphoreAttr_size
	movs	tos, r0
	pop		{pc}




//  ==== Kernel Management Functions ====

/// Initialize the RTOS Kernel.
/// \return status code that indicates the execution status of the function.
//osStatus_t osKernelInitialize (void);

///  Get RTOS Kernel Information.
/// \param[out]    version       pointer to buffer for retrieving version information.
/// \param[out]    id_buf        pointer to buffer for retrieving kernel identification string.
/// \param[in]     id_size       size of buffer for kernel identification string.
/// \return status code that indicates the execution status of the function.
//osStatus_t osKernelGetInfo (osVersion_t *version, char *id_buf, uint32_t id_size);

/// Get the current RTOS Kernel state.
/// \return current RTOS Kernel state.
//osKernelState_t osKernelGetState (void);

/// Start the RTOS Kernel scheduler.
/// \return status code that indicates the execution status of the function.
//osStatus_t osKernelStart (void);

/// Lock the RTOS Kernel scheduler.
/// \return previous lock state (1 - locked, 0 - not locked, error code if negative).
//int32_t osKernelLock (void);

/// Unlock the RTOS Kernel scheduler.
/// \return previous lock state (1 - locked, 0 - not locked, error code if negative).
//int32_t osKernelUnlock (void);

/// Restore the RTOS Kernel scheduler lock state.
/// \param[in]     lock          lock state obtained by \ref osKernelLock or \ref osKernelUnlock.
/// \return new lock state (1 - locked, 0 - not locked, error code if negative).
//int32_t osKernelRestoreLock (int32_t lock);

/// Suspend the RTOS Kernel scheduler.
/// \return time in ticks, for how long the system can sleep or power-down.
//uint32_t osKernelSuspend (void);

/// Resume the RTOS Kernel scheduler.
/// \param[in]     sleep_ticks   time in ticks for how long the system was in sleep or power-down mode.
//void osKernelResume (uint32_t sleep_ticks);

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osKernelGetTickCount"
		@ (  -- u ) Get the RTOS kernel tick count.
/// \return RTOS kernel current tick count.
//uint32_t osKernelGetTickCount (void);
// -----------------------------------------------------------------------------
rtos_osKernelGetTickCount:
	push	{r0-r3, lr}
	pushdatos
	bl		osKernelGetTickCount
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osKernelGetTickFreq"
		@ (  -- u ) Get the RTOS kernel tick frequency.
/// \return frequency of the kernel tick in hertz, i.e. kernel ticks per second.
// uint32_t osKernelGetTickFreq (void);
// -----------------------------------------------------------------------------
rtos_osKernelGetTickFreq:
	push	{r0-r3, lr}
	pushdatos
	bl		osKernelGetTickFreq
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osKernelGetSysTimerCount"
		@ (  -- u ) Get the RTOS kernel system timer count.
/// \return RTOS kernel current system timer count as 32-bit value.
// uint32_t osKernelGetSysTimerCount (void);
// -----------------------------------------------------------------------------
rtos_osKernelGetSysTimerCount:
	push	{r0-r3, lr}
	pushdatos
	bl		osKernelGetSysTimerCount
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osKernelGetSysTimerFreq"
		@ (  -- u ) Get the RTOS kernel system timer frequency.
/// \return frequency of the system timer in hertz, i.e. timer ticks per second.
// uint32_t osKernelGetSysTimerFreq (void);
// -----------------------------------------------------------------------------
rtos_osKernelGetSysTimerFreq:
	push	{r0-r3, lr}
	pushdatos
	bl		osKernelGetSysTimerFreq
	movs	tos, r0
	pop		{r0-r3, pc}


//  ==== Generic Wait Functions ====

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osDelay"
		@ ( u -- n ) Wait for Timeout (Time Delay).
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value
/// \return status code that indicates the execution status of the function.
// osStatus_t osDelay (uint32_t ticks);
// -----------------------------------------------------------------------------
rtos_osDelay:
	push	{r0-r3, lr}
	movs	r0, tos		// ticks
	bl		osDelay
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osDelayUntil"
		@ ( u -- n ) Wait until specified time.
/// \param[in]     ticks         absolute time in ticks
/// \return status code that indicates the execution status of the function.
// osStatus_t osDelayUntil (uint32_t ticks);
// -----------------------------------------------------------------------------
rtos_osDelayUntil:
	push	{r0-r3, lr}
	movs	r0, tos		// ticks
	bl		osDelay
	movs	tos, r0
	pop		{r0-r3, pc}


// Thread Management
// *****************

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadNew"

		@ ( a a a -- u ) Create a thread and add it to Active Threads.
// Create a thread and add it to Active Threads.
// The new thread needs its own data stack!
// \param[in]     func          thread function.
// \param[in]     argument      pointer that is passed to the thread function as start argument.
// \param[in]     attr          thread attributes; NULL: default values.
// \return thread ID for reference by other functions or NULL in case of error.
// osThreadId_t osThreadNew (osThreadFunc_t func, void *argument, const osThreadAttr_t *attr);
// -----------------------------------------------------------------------------
rtos_osThreadNew:
	push	{r0-r3, lr}
	movs	r2, tos		// attr
	drop
	movs	r1, tos		// argument
	drop
	movs	r0, tos		// func
	bl		osThreadNew
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadGetId"
		@ ( -- u ) Return the thread ID of the current running thread.
// \return		the thread object ID of the currently running thread or NULL in case of an error.
// -----------------------------------------------------------------------------
.global		rtos_osThreadGetId
rtos_osThreadGetId:
	push	{r0-r3, lr}
	pushdatos
	bl		osThreadGetId
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadGetState"
		@ ( u -- u ) Get current thread state of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return current thread state of the specified thread.
// osThreadState_t 	osThreadGetState (osThreadId_t thread_id)
// -----------------------------------------------------------------------------
.global		rtos_osThreadGetState
rtos_osThreadGetState:
	push	{r0-r3, lr}
	movs	r0, tos		// set Thread ID
	bl		osThreadGetState
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadSetPriority"
		@ ( u u -- u ) Change priority of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \param[in]     priority      new priority value for the thread function.
// osStatus_t 	osThreadSetPriority (osThreadId_t thread_id, osPriority_t priority)
// -----------------------------------------------------------------------------
.global		rtos_osThreadSetPriority
rtos_osThreadSetPriority:
	push	{r0-r3, lr}
	movs	r1, tos		// priority
	drop
	movs	r0, tos		// set Thread ID
	bl		osThreadSetPriority
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadGetPriority"
		@ ( u  -- u ) Get current priority of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return current priority value of the specified thread.
// osPriority_t 	osThreadGetPriority (osThreadId_t thread_id)
// -----------------------------------------------------------------------------
.global		rtos_osThreadGetPriority
rtos_osThreadGetPriority:
	push	{r0-r3, lr}
	movs	r0, tos		// set Thread ID
	bl		osThreadGetPriority
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadYield"
		@ (  -- u ) Pass control to next thread that is in state READY.
/// \return status code that indicates the execution status of the function.
// osPriority_t 	osThreadGetPriority (osThreadId_t thread_id)
// -----------------------------------------------------------------------------
.global		rtos_osThreadYield
rtos_osThreadYield:
	push	{r0-r3, lr}
	pushdatos
	bl		osThreadYield
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadSuspend"
		@ ( u -- u ) Suspend execution of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
// osStatus_t 	osThreadSuspend (osThreadId_t thread_id)
// -----------------------------------------------------------------------------
.global		rtos_osThreadSuspend
rtos_osThreadSuspend:
	push	{r0-r3, lr}
	movs	r0, tos		// set Thread ID
	bl		osThreadSuspend
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadResume"
		@ (  --  ) Resume execution of a thread.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
// osStatus_t 	osThreadResume (osThreadId_t thread_id)
// -----------------------------------------------------------------------------
.global		rtos_osThreadResume
rtos_osThreadResume:
	push	{r0-r3, lr}
	movs	r0, tos		// set Thread ID
	bl		osThreadResume
	movs	tos, r0
	pop		{r0-r3, pc}


// not implemented under FreeRTOS
//// -----------------------------------------------------------------------------
//		Wortbirne Flag_visible, "osThreadDetach"
//		@ ( u -- u ) Detach a thread (thread storage can be reclaimed when thread terminates).
//// osStatus_t 	osThreadDetach (osThreadId_t thread_id)
//// -----------------------------------------------------------------------------
//.global		rtos_osThreadDetach
//rtos_osThreadDetach:
//	push	{r0-r3, lr}
//	movs	r0, tos		// set Thread ID
//	bl		osThreadDetach
//	movs	tos, r0		// return state
//	pop		{r0-r3, pc}


// not implemented under FreeRTOS
//// -----------------------------------------------------------------------------
//		Wortbirne Flag_visible, "osThreadJoin"
//		@ (  --  ) Wait for specified thread to terminate.
//// osStatus_t 	osThreadJoin (osThreadId_t thread_id)
//// -----------------------------------------------------------------------------
//.global		rtos_osThreadJoin
//rtos_osThreadJoin:
//	push	{r0-r3, lr}
//	movs	r0, tos		// set Thread ID
//	bl		osThreadJoin
//	movs	tos, r0
//	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadExit"
		@ (  --  ) Terminate execution of current running thread.
// __NO_RETURN void 	osThreadExit (void)
// -----------------------------------------------------------------------------
.global		rtos_osThreadExit
rtos_osThreadExit:
	push	{r0-r3, lr}
	bl		osThreadExit
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadTerminate"
		@ ( u -- n ) Pass control to next thread that is in state READY.
/// \param[in]     thread_id     thread ID obtained by \ref osThreadNew or \ref osThreadGetId.
/// \return status code that indicates the execution status of the function.
// osStatus_t 	osThreadTerminate (osThreadId_t thread_id)
// -----------------------------------------------------------------------------
.global		rtos_osThreadTerminate
rtos_osThreadTerminate:
	push	{r0-r3, lr}
	movs	r0, tos		// set Thread ID
	bl		osThreadTerminate
	movs	tos, r0
	pop		{r0-r3, pc}


// not implemented under FreeRTOS
//// -----------------------------------------------------------------------------
//		Wortbirne Flag_visible, "osThreadGetStackSize"
//		@ ( u -- u ) Get stack size of a thread.
// uint32_t 	osThreadGetStackSize (osThreadId_t thread_id)
//// -----------------------------------------------------------------------------
//.global		rtos_osThreadGetStackSize
//rtos_osThreadGetStackSize:
//	push	{r0-r3, lr}
//	movs	r0, tos		// set Thread ID
//	bl		osThreadGetStackSize
//	movs	tos, r0
//	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadGetStackSpace"
		@ ( u -- u ) returns the size of unused stack space of the specified thread
// \param[in]	thread_id	thread ID obtained by osThreadNew or osThreadGetId.
// \return		unused stack size in bytes
// uint32_t osThreadGetStackSpace(osThreadId_t thread_id)
// -----------------------------------------------------------------------------
.global		rtos_osThreadGetStackSpace
rtos_osThreadGetStackSpace:
	push	{r0-r3, lr}
	movs	r0, tos		// set Thread ID
	bl		osThreadGetStackSpace
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadGetCount"
		@ (  -- u ) Get number of active threads.
/// \return number of active threads.
// uint32_t 	osThreadGetCount (void)
// -----------------------------------------------------------------------------
.global		rtos_osThreadGetCount
rtos_osThreadGetCount:
	push	{r0-r3, lr}
	pushdatos
	bl		osThreadGetCount
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osThreadEnumerate"
		@ ( a u  -- u ) Enumerate active threads.
/// \param[out]    thread_array  pointer to array for retrieving thread IDs.
/// \param[in]     array_items   maximum number of items in array for retrieving thread IDs.
/// \return number of enumerated threads.
// uint32_t 	osThreadEnumerate (osThreadId_t *thread_array, uint32_t array_items)
// -----------------------------------------------------------------------------
.global		rtos_osThreadEnumerate
rtos_osThreadEnumerate:
	push	{r0-r3, lr}
	movs	r1, tos		// array items
	drop
	movs	r0, tos		// addr thread array
	bl		osThreadEnumerate
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "xPortGetFreeHeapSize"
		@ (  -- u ) returns the total amount of heap space that remains
		@			unallocated when the function is called
// -----------------------------------------------------------------------------
.global		rtos_xPortGetFreeHeapSize
rtos_xPortGetFreeHeapSize:
	push	{r0-r3, lr}
	pushdatos
	bl		xPortGetFreeHeapSize
	movs	tos, r0
	pop		{r0-r3, pc}


//  ==== Timer Management Functions ====

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osTimerNew"
		@ ( a u a a -- u ) Create and Initialize a timer.
/// \param[in]     func          function pointer to callback function.
/// \param[in]     type          \ref osTimerOnce for one-shot or \ref osTimerPeriodic for periodic behavior.
/// \param[in]     argument      argument to the timer callback function.
/// \param[in]     attr          timer attributes; NULL: default values.
/// \return timer ID for reference by other functions or NULL in case of error.
// osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr);
// -----------------------------------------------------------------------------
rtos_osTimerNew:
	push	{r0-r3, lr}
	movs	r3, tos		// attr
	drop
	movs	r2, tos		// argument
	drop
	movs	r1, tos		// type
	drop
	movs	r0, tos		// func
	bl		osTimerNew
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osTimerGetName"
		@ ( u -- u ) Get name of a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return name as NULL terminated string.
// const char *osTimerGetName (osTimerId_t timer_id);
// -----------------------------------------------------------------------------
rtos_osTimerGetName:
	push	{r0-r3, lr}
	movs	r0, tos		// timer_id
	bl		osTimerGetName
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osTimerStart"
		@ ( u u -- u ) Start or restart a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
/// \return status code that indicates the execution status of the function.
// osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks);
// -----------------------------------------------------------------------------
rtos_osTimerStart:
	push	{r0-r3, lr}
	movs	r1, tos		// ticks
	drop
	movs	r0, tos		// timer_id
	bl		osTimerStart
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osTimerStop"
		@ ( u -- u ) Stop a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osTimerStop (osTimerId_t timer_id);
// -----------------------------------------------------------------------------
rtos_osTimerStop:
	movs	r0, tos		// timer_id
	bl		osTimerStop
	movs	tos, r0
	pop		{r0-r3, pc}

///
// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osTimerIsRunning"
		@ ( u -- u ) Check if a timer is running.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return 0 not running, 1 running.
// uint32_t osTimerIsRunning (osTimerId_t timer_id);
// -----------------------------------------------------------------------------
rtos_osTimerIsRunning:
	push	{r0-r3, lr}
	movs	r0, tos		// timer_id
	bl		osTimerIsRunning
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osTimerDelete"
		@ ( u -- u ) Delete a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osTimerDelete (osTimerId_t timer_id);
// -----------------------------------------------------------------------------
rtos_osTimerDelete:
	push	{r0-r3, lr}
	movs	r0, tos		// timer_id
	bl		osTimerDelete
	movs	tos, r0
	pop		{r0-r3, pc}


//  ==== Event Flags Management Functions ====

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osEventFlagsNew"
		@ ( a -- u ) Create and Initialize an Event Flags object.
/// \param[in]     attr          event flags attributes; NULL: default values.
/// \return event flags ID for reference by other functions or NULL in case of error.
// osEventFlagsId_t osEventFlagsNew (const osEventFlagsAttr_t *attr);
// -----------------------------------------------------------------------------
rtos_osEventFlagsNew:
	push	{r0-r3, lr}
	movs	r0, tos		// attr
	bl		osEventFlagsNew
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osEventFlagsSet"
		@ ( u u -- u ) Set the specified Event Flags.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \param[in]     flags         specifies the flags that shall be set.
/// \return event flags after setting or error code if highest bit set.
// uint32_t osEventFlagsSet (osEventFlagsId_t ef_id, uint32_t flags);
// -----------------------------------------------------------------------------
rtos_osEventFlagsSet:
	movs	r1, tos		// flags
	drop
	movs	r0, tos		// ef_id
	bl		osEventFlagsSet
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osEventFlagsClear"
		@ ( a u a a -- u ) Clear the specified Event Flags.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \param[in]     flags         specifies the flags that shall be cleared.
/// \return event flags before clearing or error code if highest bit set.
// uint32_t osEventFlagsClear (osEventFlagsId_t ef_id, uint32_t flags);
// -----------------------------------------------------------------------------
rtos_osEventFlagsClear:
	push	{r0-r3, lr}
	movs	r1, tos		// flags
	drop
	movs	r0, tos		// ef_id
	bl		osEventFlagsClear
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osEventFlagsGet"
		@ ( u -- u ) Get the current Event Flags.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \return current event flags.
// uint32_t osEventFlagsGet (osEventFlagsId_t ef_id);
// -----------------------------------------------------------------------------
rtos_osEventFlagsGet:
	push	{r0-r3, lr}
	movs	r0, tos		// ef_id
	bl		osEventFlagsGet
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osEventFlagsWait"
		@ ( a u a a -- u ) Wait for one or more Event Flags to become signaled.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \param[in]     flags         specifies the flags to wait for.
/// \param[in]     options       specifies flags options (osFlagsXxxx).
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return event flags before clearing or error code if highest bit set.
// uint32_t osEventFlagsWait (osEventFlagsId_t ef_id, uint32_t flags, uint32_t options, uint32_t timeout);
// -----------------------------------------------------------------------------
rtos_osEventFlagsWait:
	push	{r0-r3, lr}
	movs	r3, tos		// timeout
	drop
	movs	r2, tos		// options
	drop
	movs	r1, tos		// flags
	drop
	movs	r0, tos		// ef_id
	bl		osEventFlagsWait
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osEventFlagsDelete"
		@ ( u  -- u ) Delete an Event Flags object.
/// \param[in]     ef_id         event flags ID obtained by \ref osEventFlagsNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osEventFlagsDelete (osEventFlagsId_t ef_id);
// -----------------------------------------------------------------------------
rtos_osEventFlagsDelete:
	push	{r0-r3, lr}
	movs	r0, tos		// ef_id
	bl		osEventFlagsDelete
	movs	tos, r0
	pop		{r0-r3, pc}


//  ==== Mutex Management Functions ====

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMutexNew"
		@ ( a -- u ) Create and Initialize a Mutex object.
/// \param[in]     attr          mutex attributes; NULL: default values.
/// \return mutex ID for reference by other functions or NULL in case of error.
// osMutexId_t osMutexNew (const osMutexAttr_t *attr);
// -----------------------------------------------------------------------------
rtos_osMutexNew:
	push	{r0-r3, lr}
	movs	r0, tos		// attr
	bl		osMutexNew
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMutexAcquire"
		@ ( u u -- u ) Acquire a Mutex or timeout if it is locked.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
// osStatus_t osMutexAcquire (osMutexId_t mutex_id, uint32_t timeout);
// -----------------------------------------------------------------------------
rtos_osMutexAcquire:
	push	{r0-r3, lr}
	movs	r1, tos		// timeout
	drop
	movs	r0, tos		// mutex_id
	bl		osMutexAcquire
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMutexRelease"
		@ ( u -- u ) Release a Mutex that was acquired by \ref osMutexAcquire.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osMutexRelease (osMutexId_t mutex_id);
// -----------------------------------------------------------------------------
rtos_osMutexRelease:
	push	{r0-r3, lr}
	movs	r0, tos		// mutex_id
	bl		osMutexRelease
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMutexGetOwner"
		@ ( u -- u ) Get Thread which owns a Mutex object.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \return thread ID of owner thread or NULL when mutex was not acquired.
// osThreadId_t osMutexGetOwner (osMutexId_t mutex_id);
// -----------------------------------------------------------------------------
rtos_osMutexGetOwner:
	push	{r0-r3, lr}
	movs	r0, tos		// mutex_id
	bl		osMutexGetOwner
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMutexDelete"
		@ ( u -- u ) Delete a Mutex object.
/// \param[in]     mutex_id      mutex ID obtained by \ref osMutexNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osMutexDelete (osMutexId_t mutex_id);
// -----------------------------------------------------------------------------
rtos_osMutexDelete:
	push	{r0-r3, lr}
	movs	r0, tos		// mutex_id
	bl		osMutexDelete
	movs	tos, r0
	pop		{r0-r3, pc}


//  ==== Semaphore Management Functions ====

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osSemaphoreNew"
		@ ( u u a -- u ) Create and Initialize a Semaphore object.
/// \param[in]     max_count     maximum number of available tokens.
/// \param[in]     initial_count initial number of available tokens.
/// \param[in]     attr          semaphore attributes; NULL: default values.
/// \return semaphore ID for reference by other functions or NULL in case of error.
// osSemaphoreId_t osSemaphoreNew (uint32_t max_count, uint32_t initial_count, const osSemaphoreAttr_t *attr);
// -----------------------------------------------------------------------------
rtos_osSemaphoreNew:
	push	{r0-r3, lr}
	movs	r2, tos		// attr
	drop
	movs	r1, tos		// initial_count
	drop
	movs	r0, tos		// max_count
	bl		osSemaphoreNew
	movs	tos, r0
	pop		{r0-r3, pc}


// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osSemaphoreAcquire"
		@ ( u u -- u ) Acquire a Semaphore token or timeout if no tokens are available.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
// osStatus_t osSemaphoreAcquire (osSemaphoreId_t semaphore_id, uint32_t timeout);
// -----------------------------------------------------------------------------
rtos_osSemaphoreAcquire:
	push	{r0-r3, lr}
	movs	r1, tos		// timeout
	drop
	movs	r0, tos		// semaphore_id
	bl		osSemaphoreAcquire
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osSemaphoreRelease"
		@ ( u -- u ) Release a Semaphore token up to the initial maximum count.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osSemaphoreRelease (osSemaphoreId_t semaphore_id);
// -----------------------------------------------------------------------------
rtos_osSemaphoreRelease:
	push	{r0-r3, lr}
	movs	r0, tos		// semaphore_id
	bl		osSemaphoreRelease
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osSemaphoreGetCount"
		@ ( u -- u ) Get current Semaphore token count.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \return number of tokens available.
// uint32_t osSemaphoreGetCount (osSemaphoreId_t semaphore_id);
// -----------------------------------------------------------------------------
rtos_osSemaphoreGetCount:
	push	{r0-r3, lr}
	movs	r0, tos		// semaphore_id
	bl		osSemaphoreGetCount
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osSemaphoreDelete"
		@ ( u -- u ) Delete a Semaphore object.
/// \param[in]     semaphore_id  semaphore ID obtained by \ref osSemaphoreNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osSemaphoreDelete (osSemaphoreId_t semaphore_id);
// -----------------------------------------------------------------------------
rtos_osSemaphoreDelete:
	push	{r0-r3, lr}
	movs	r0, tos		// semaphore_id
	bl		osSemaphoreDelete
	movs	tos, r0
	pop		{r0-r3, pc}



//  ==== Message Queue Management Functions ====

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueNew"
		@ ( u u a -- u ) Create and Initialize a Message Queue object.
/// \param[in]     msg_count     maximum number of messages in queue.
/// \param[in]     msg_size      maximum message size in bytes.
/// \param[in]     attr          message queue attributes; NULL: default values.
/// \return message queue ID for reference by other functions or NULL in case of error.
// osMessageQueueId_t osMessageQueueNew (uint32_t msg_count, uint32_t msg_size, const osMessageQueueAttr_t *attr);
// -----------------------------------------------------------------------------
rtos_osMessageQueueNew:
	push	{r0-r3, lr}
	movs	r2, tos		// attr
	drop
	movs	r1, tos		// msg_size
	drop
	movs	r0, tos		// msg_count
	bl		osMessageQueueNew
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueuePut"
		@ ( u a a u -- u ) Put a Message into a Queue or timeout if Queue is full.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \param[in]     msg_ptr       pointer to buffer with message to put into a queue.
/// \param[in]     msg_prio      message priority.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
// osStatus_t osMessageQueuePut (osMessageQueueId_t mq_id, const void *msg_ptr, uint8_t msg_prio, uint32_t timeout);
// -----------------------------------------------------------------------------
rtos_osMessageQueuePut:
	push	{r0-r3, lr}
	movs	r3, tos		// timeout
	drop
	movs	r2, tos		// msg_prio
	drop
	movs	r1, tos		// msg_ptr
	drop
	movs	r0, tos		// mq_id
	bl		osMessageQueuePut
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueGet"
		@ ( u a a u -- u ) Get a Message from a Queue or timeout if Queue is empty.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \param[out]    msg_ptr       pointer to buffer for message to get from a queue.
/// \param[out]    msg_prio      pointer to buffer for message priority or NULL.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
// osStatus_t osMessageQueueGet (osMessageQueueId_t mq_id, void *msg_ptr, uint8_t *msg_prio, uint32_t timeout);
// -----------------------------------------------------------------------------
rtos_osMessageQueueGet:
	push	{r0-r3, lr}
	movs	r3, tos		// timeout
	drop
	movs	r2, tos		// msg_prio
	drop
	movs	r1, tos		// msg_ptr
	drop
	movs	r0, tos		// mq_id
	bl		osMessageQueueGet
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueGetCapacity"
		@ ( u -- u ) Get maximum number of messages in a Message Queue.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return maximum number of messages.
// uint32_t osMessageQueueGetCapacity (osMessageQueueId_t mq_id);
// -----------------------------------------------------------------------------
rtos_osMessageQueueGetCapacity:
	push	{r0-r3, lr}
	movs	r0, tos		// mq_id
	bl		osMessageQueueGetCapacity
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueGetMsgSize"
		@ ( u -- u ) Get maximum message size in a Memory Pool.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return maximum message size in bytes.
// uint32_t osMessageQueueGetMsgSize (osMessageQueueId_t mq_id);
// -----------------------------------------------------------------------------
rtos_osMessageQueueGetMsgSize:
	push	{r0-r3, lr}
	movs	r0, tos		// mq_id
	bl		osMessageQueueGetMsgSize
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueGetCount"
		@ ( u -- u ) Get number of queued messages in a Message Queue.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return number of queued messages.
// uint32_t osMessageQueueGetCount (osMessageQueueId_t mq_id);
// -----------------------------------------------------------------------------
rtos_osMessageQueueGetCount:
	push	{r0-r3, lr}
	movs	r0, tos		// mq_id
	bl		osMessageQueueGetCount
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueGetSpace"
		@ ( u -- u ) Get number of available slots for messages in a Message Queue.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return number of available slots for messages.
// uint32_t osMessageQueueGetSpace (osMessageQueueId_t mq_id);
// -----------------------------------------------------------------------------
rtos_osMessageQueueGetSpace:
	push	{r0-r3, lr}
	movs	r0, tos		// mq_id
	bl		osMessageQueueGetSpace
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueReset"
		@ ( u -- u ) Reset a Message Queue to initial empty state.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osMessageQueueReset (osMessageQueueId_t mq_id);
// -----------------------------------------------------------------------------
rtos_osMessageQueueReset:
	movs	r0, tos		// mq_id
	bl		osMessageQueueReset
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osMessageQueueDelete"
		@ ( u -- u ) Delete a Message Queue object.
/// \param[in]     mq_id         message queue ID obtained by \ref osMessageQueueNew.
/// \return status code that indicates the execution status of the function.
// osStatus_t osMessageQueueDelete (osMessageQueueId_t mq_id);
// -----------------------------------------------------------------------------
rtos_osMessageQueueDelete:
	push	{r0-r3, lr}
	movs	r0, tos		// mq_id
	bl		osMessageQueueDelete
	movs	tos, r0
	pop		{r0-r3, pc}

// -----------------------------------------------------------------------------
