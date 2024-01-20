<table>
  <tr>
    <td><img src="img/C_Programming_Language.svg"  ></td>
    <td><img src="img/forth-logo.png"  ></td>
  </tr>
</table> 

Calling C Functions from Forth and Vice Versa
=============================================

Register Usage
--------------

Procedure Call Standard for the ARM Architecture
[AAPCS](https://developer.arm.com/documentation/ihi0042/j)

<https://community.arm.com/developer/ip-products/processors/b/processors-ip-blog/posts/on-the-aapcs-with-an-application-to-efficient-parameter-passing>

| Register | AAPCS Purpose                                        | Restore Contents | MECRISP Forth Purpose  | ISR Stacking |
|----------|------------------------------------------------------|------------------|------------------------|--------------|
| xPSR     | Processor Status Register, Flags                     | N                | Flags                  | 1            |
| r1       | Argument, result, scratch register 2                 | N                | Scratch                | 3            |
| r2       | Argument, scratch register 3                         | N                | Scratch                | 4            |
| r3       | Argument, scratch register 4                         | Y                | Scratch                | 5            |
| r4       | Variable register 1                                  | Y                | DO Index               |              |
| r5       | Variable register 2                                  | Y                | DO Limit               |              |
| r6       | Variable register 3                                  | Y                | Top Of Stack TOS       |              |
| r7       | Variable register 4                                  | Y                | Data stack pointer SPS |              |
| r8       | Variable register 5                                  | Y                | unused                 |              |
| r9       | Platform register (usage defined by platform in use) | Y                | unused                 |              |
| r10      | Variable register 7                                  | Y                | unused                 |              |
| r11      | Variable register 8                                  | Y                | unused                 |              |
| r12      | Intra-procedure-call scratch register                | N                | unused                 | 6            |
| r13      | Stack pointer (SP)                                   | y                | Stack pointer (SP)     |              |
| r14      | Link register (LR)                                   | N                | Link register (LR)     | 7            |
| r15      | Program counter (PC)                                 | N                | Program counter (PC)   | 0            |

VFP Register Usage
------------------

The VFP-v2 co-processor has 32 single-precision registers, s0-s31, which
may also be accessed as 16 double-precision registers, d0-d15 (with d0
overlapping s0, s1; d1 overlapping s2, s3; etc).

Calling C Functions from Forth
------------------------------

Word `osDelay` from
[rtos.s](https://github.com/spyren/Mecrisp-Cube/blob/master/Forth/cube/rtos.s)
```assembly
// -----------------------------------------------------------------------------
    Wortbirne Flag_visible, "osDelay"
    @ ( u -- n ) Wait for Timeout (Time Delay).
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value
/// \return status code that indicates the execution status of the function.
// osStatus_t osDelay (uint32_t ticks);
// -----------------------------------------------------------------------------
rtos_osDelay:
    push    {r0-r3, lr}
    movs    r0, tos     // ticks
    bl      osDelay
    movs    tos, r0
    pop     {r0-r3, pc}
```

If the C function itself calls Forth words the Data Stack Pointer SPS
and Top Of Stack TOS registers have to be passed to the Forth words. See
below.

Calling Forth Words from C Functions
------------------------------------

Data Stack Pointer SPS is in R1 and Top Of Stack TOS is in R0 (first
parameter).

C function `FS_include()` from
[fs.c](https://github.com/spyren/Mecrisp-Cube/blob/master/Forth/Src/fs.c)
calls the Forth words `FS_type` and `FS_evaluate`.
```C
/**
 *  @brief
 *      Interprets the content of the file.
 *  @param[in]
 *      forth_stack   TOS (lower word) and SPS (higher word)
 *  @param[in]
 *      str   filename (w/ or w/o null termination)
 *  @param[in]
 *      count string length
 *  @return
 *      TOS (lower word) and SPS (higer word)
 */
uint64_t FS_include(uint64_t forth_stack, uint8_t *str, int count) {
    FIL fil;        /* File object */
    FRESULT fr;     /* FatFs return code */

    uint64_t stack;
    stack = forth_stack;

    memcpy(path, str, count);
    line[count] = 0;

    /* Open a text file */
    fr = f_open(&fil, path, FA_READ);
    if (fr) {
        // open failed
        strcpy(line, "Err: file not found");
        stack = FS_type(stack, (uint8_t*)line, strlen(line));
    }

    /* Read every line and interprets it */
    while (f_gets(line, sizeof line, &fil)) {
        // line without \n
        stack = FS_evaluate(stack, (uint8_t*)line, strlen(line)-1);
    }

    /* Close the file */
    f_close(&fil);

    return stack;
}
```

Word `include` from
[fs.s](https://github.com/spyren/Mecrisp-Cube/blob/master/Forth/cube/fs.s)
calls the C function `FS_include()`.
```assembly
@ -----------------------------------------------------------------------------
        Wortbirne Flag_visible, "include"
        @  ( any "filename" -- any ) Interprets the content of the file.
// uint64_t FS_include  (uint64_t forth_stack, uint8_t *str, int count);
@ -----------------------------------------------------------------------------
include:
    push    {lr}
    bl  token       @ ( -- c-addr len )
incl:
    movs    r3, tos     // len -> count
    drop
    movs    r2, tos     // c-addr -> str
    drop
    movs    r0, tos     // get tos
    movs    r1, psp     // get psp
    bl  FS_include
    movs    tos, r0     // update tos
    movs    psp, r1     // update psp
    pop {pc}
```

The C function `FS_include()` from
[fs.c](https://github.com/spyren/Mecrisp-Cube/blob/master/Forth/Src/fs.c)
calls the Forth word `evaluate` by the `FS_evaluate()` function.
```assembly
// uint64_t FS_evaluate(uint64_t forth_stack, uint8_t* str, int count);
.global     FS_evaluate
FS_evaluate:
    push    {r4-r7, lr}
    movs    tos, r0     // get tos
    movs    psp, r1     // get psp
    pushdatos
    movs    tos, r2     // str
    pushdatos
    movs    tos, r3     // count
    bl      evaluate
    movs    r0, tos     // update tos
    movs    r1, psp     // update psp
    pop {r4-r7, pc}
```

\-- [PeterSchmid - 2020-07-13]
