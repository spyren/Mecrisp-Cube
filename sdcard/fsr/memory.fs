\ The optional Memory-Allocation word set
\ Based on FreeRTOS heap memory management
\ Thread safe, Mecrisp-Cube is using FreeRTOS heap4 scheme
CR .( memory.fs loading ... )

\ Allocate u address units of contiguous data space. 
\ The data-space pointer is unaffected by this operation. 
\ The initial content of the allocated space is undefined.
\ If the allocation succeeds, a-addr is the aligned starting address of the allocated space and ior is zero.
\ If the operation fails, a-addr does not represent a valid address and 
\ ior is the implementation-defined I/O result code.
: allocate ( u -- a-addr ior ) 
  pvPortMalloc
  dup if 0= -1 else 0 then
;

\ Return the contiguous region of data space indicated by a-addr to the system for later allocation. 
\ a-addr shall indicate a region of data space that was previously obtained by ALLOCATE or RESIZE. 
\ The data-space pointer is unaffected by this operation.
\ If the operation succeeds, ior is zero. 
\ If the operation fails, ior is the implementation-defined I/O result code.
: free ( a-addr -- ior ) 
  vPortFree 0
;

