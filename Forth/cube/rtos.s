@ -----------------------------------------------------------------------------
		Wortbirne Flag_visible, "osdelay"
rtos_osdelay:
		@ ( u -- n ) waits for a time period specified in kernel ticks
@ -----------------------------------------------------------------------------
	movs	r0, tos
	push	{r4, r5, r7, lr}
	bl	osDelay
	movs	tos, r0
	pop	{r4, r5, r7, pc}

@ -----------------------------------------------------------------------------
