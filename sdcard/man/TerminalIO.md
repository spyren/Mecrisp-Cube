![](img/mecrisp-cube-4th-logo-red-h.svg)
![](img/ch3-flush.gif)

# Terminal-IO

## Intro

To work interactively with a Forth system, you need some kind of
[console](https://en.wikipedia.org/wiki/System_console). A text
[terminal](https://en.wikipedia.org/wiki/Computer_terminal#Text_terminals)
with [RS-232](https://en.wikipedia.org/wiki/RS-232) port has been used
for this purpose in the past. Today's PCs have software to emulate a
terminal, but usually no longer have serial ports (RS-232 interfaces).
Instead, there are usually USB interfaces.

What says Leo Brodie to this topic? 
[I/O and You](https://www.forth.com/starting-forth/10-input-output-operators/)

The
[USB-CDC](https://en.wikipedia.org/wiki/USB_communications_device_class)
appears as a traditional RS-232 port in your operating system (Linux
`/dev/ttyACMx`, Windoze `COMx`). No need to bother with baud rates,
handshaking, parity and other awkward things.

There is also a wireless console for PCs and mobile devices. The
proprietary STM BLE Cable Replacement Service is used as communication
protocol. 

`emit` and `key` can block the calling thread, if the buffer is full
(`emit`) or empty (`key`). The other threads are not affected (no
busy-wait). The incoming characters (Rx) are buffered in a RTOS message
queue, outgoing characters are also buffered.

Default console is USB-CDC, but if you press button SW2 
(it depends on the used board) on reset, the
console is redirected to the UART
[RS-232](https://en.wikipedia.org/wiki/RS-232) serial device (for
details see
[mecrisp.s](https://github.com/spyren/Mecrisp-Cube/blob/master/Forth/cube/mecrisp.s)).
If you press button SW1 on reset, the console is redirected to the
Bluetooth LE Cable Replacement Service.

```
emit?        ( -- f )       Ready to send a character?
key?         ( -- f )       Checks if a key is waiting
key          ( -- c )       Waits for and fetches the pressed key
emit         ( c -- )       Emits a character

hook-emit?   ( -- a-addr )  Hooks for redirecting terminal IO on the fly
hook-key?
hook-key
hook-emit

uart         ( -- )         redirect console to serial interface (UART)
cdc          ( -- )         redirect console to USB-CDC
crs          ( -- )         redirect console to BLE CRS
```

```forth
: ascii ( -- ) 
  127 32 do 
    i emit 
  loop 
;

: crs-ascii ( -- ) 
  127 32 do 
    i crs-emit 
  loop 
  10 crs-emit / LF
;
```


## USB-CDC Serial Communication (API)

Buffered USB-CDC serial communication. The
[USB-CDC](https://en.wikipedia.org/wiki/USB_communications_device_class)
appears as a traditional RS-232 port in your operating system (Linux
`/dev/ttyACMx`, Windoze `COMx`). No need to bother with baud rates,
handshaking, parity and other awkward things. Under Linux I recommend to
disable the ModemManager, otherwise the ModemManager tries to configure
the ACM with some escape sequences.

USB_USER CN1 Micro USB connector on the MB1355 Nucleo Board.
USB_USER CN3 USB A plug on the MB1293 dongle.
```
cdc-emit   ( c -- ) Emits one character to the USB-CDC interface. Blocking if the Buffer is full.
cdc-key    ( -- c ) Waits and gets one character from the USB-CDC interface. Blocking if the buffer is empty.
cdc-emit?  ( -- f ) Ready to send a character. Buffer is not full.
cdc-key?   ( -- f ) Checks if a character is in the buffer.
```


## UART Serial Communication (API)

The Rx buffer is 5 k, the Tx buffer 1 k. Copy and past source code into
the terminal is possible without buffer overrun.

USB-CDC on the ST-LINK Micro USB connector on the MB1355 Nucleo Board.
Serial port UART1 on PB6/PB7 (remove jumper SB2, close jumper SB6) on
the MB1293 dongle. Do not forget to set the baud rate. The default baud
rate for the MecrispCube is 115200 baud, e.g.
`microcom -D /dev/ttyACM1 -s 115200` .
```
serial-emit  ( c -- ) Emits one character to the UART interface. Blocking if the Buffer is full.
serial-key   ( -- c ) Waits and gets one character from the UART interface. Blocking if the buffer is empty.
serial-emit? ( -- f ) Ready to send a character. Buffer is not full.
serial-key?  ( -- f ) Checks if a character is in the buffer.

baudrate!    ( u -- ) sets baud rate (e.g. 300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200)
paritybit!   ( u -- ) sets parity bit 0 none, 1 odd, 2 even
wordlength!  ( u -- ) sets word length 7, 8, 9 (including parity)
stopbits!    ( u -- ) sets stop bits 0 1 bit, 1 1.5 bit, 2 2 bit
```


## BLE Cable Replacement Serial Communication (API)

Classic Bluetooth has a Serial Port Profile (SPP). It emulates a serial
cable to provide a simple substitute for existing RS-232, including the
familiar control signals. But for BLE there is no official/standard
service for serial communication. Many chip manufactures have their own
proprietary service.

-   LAIRD vSP, Rx and Tx FIFO characteristics. For details see [Using VSP
with
smartBASIC](http://cdn.lairdtech.com/home/brandworld/files/Application%20Note%20-%20Using%20VSP%20with%20smartBASIC.pdf)
-   TI SPS, Rx and Tx use the same characteristic for read and write. For
details see [TI Serial Port Service
(SPS)](http://www.ti.com/lit/ug/tidua63/tidua63.pdf?ts=1590183153410) \*
Silicon Labs [AN1033 LE Serial Cable
Replacement](https://www.silabs.com/documents/public/application-notes/AN1033-LE-Serial-Cable-Replacement.pdf)
-   STM CRS Cable Replacement Service.

My own CRS Terminal is derived from [Danila
Loginov\'s](https://medium.com/@loginov_rocks/how-to-make-a-web-app-for-your-own-bluetooth-low-energy-device-arduino-2af8d16fdbe8)
Progressive Web App
[Web-Bluetooth-Terminal](https://github.com/loginov-rocks/Web-Bluetooth-Terminal)
and is based on Web Bluetooth.

<https://spyr.ch/crs-terminal/>

This Progressive Web App works in the Chrome browser on Linux, OSX, and
Windows operating systems. It also works on Android devices, you can
download the App and use it offline.

[Serial Bluetooth Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal)
works great. You have to configure the the STM CRS to Custom, choose the
default UUIDs (Service `0000fe60-cc7a-482a-984a-7f2ed5b3e58f`, Read
Characteristic `0000fe62-8e22-4541-9d4c-21edae82ed19`, Write
Characteristic `0000fe61-8e22-4541-9d4c-21edae82ed19`).
```
crs-emit   ( c -- ) Emits one character to the BLE Cable Replacement Service. Blocking if the Buffer is full.
crs-key    ( -- c ) Waits and gets one character from the BLE Cable Replacement Service. Blocking if the buffer is empty.
crs-emit?  ( -- f ) Ready to send a character. Buffer is not full.
crs-key?   ( -- f ) Checks if a character is in the buffer.
```

The blue LED indicates a connection to a central device.


## Terminal Emulators

Terminal emulator applications for PCs, e.g.:
 - [tio](https://github.com/tio/tio) my favourite, find your device ID with `tio -L` and connect to it `tio --map ICRNL,INLCRNL /dev/serial/by-id/usb-TinyUSB_TinyUSB_Device_550039001850503542363220-if00` 
 - [PuTTY](http://www.putty.org/) - Windows and Linux
 - [Tera Term](http://en.sourceforge.jp/projects/ttssh2/) - Windows
 - [Realterm](http://realterm.sourceforge.net/) - Windows
 - microcom, minicom, miniterm, CuteCom, screen - Linux

Terminal emulator for Android devices:

-   [Serial USB
    Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal)
-   [Serial Bluetooth
    Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal)

Settings

-   Terminal: Display mode Terminal
-   Receive: Newline CR+LF
-   Send: Newline CR, Local echo off


## Some Sort of SIGINT

https://en.wikipedia.org/wiki/Signal_(IPC)#SIGINT
<blockquote>
The SIGINT signal is sent to a process by its controlling terminal when a user wishes to 
interrupt the process. This is typically initiated by pressing Ctrl+C, 
but on some systems, the "delete" character or "break" key can be used.
</blockquote>

You do not have always a reset button to restart a hanging console, power cycle 
is not convenient in most cases. There is now an assert for ^C, 
UART errors (FIFO, interruption) throw an assert too.

If you want something similar to an SIGINT, replace the assert `ASSERT_nonfatal()`
with `osThreadFlagsSet()` or `osEventFlagsSet()` to inform your application thread. 

[uart.c](https://github.com/spyren/Mecrisp-Cube/blob/master/peripherals/uart.c)
<pre>
/**
  * @brief
  * 	Function implementing the UART Rx thread.
  * @param
  * 	argument: Not used
  * @retval
  * 	None
  */
static void UART_RxThread(void *argument) {
	osStatus_t status;

	osMutexAcquire(UART_MutexID, osWaitForever);
	// wait for the first Rx character
	if (HAL_UART_Receive_IT(&huart1, &UART_RxBuffer, 1) != HAL_OK) {
		// something went wrong
		Error_Handler();
	}
	osMutexRelease(UART_MutexID);

	// Infinite loop
	for(;;) {
		// blocked till a character is received
		status = osThreadFlagsWait(UART_CHAR_RECEIVED, osFlagsWaitAny, osWaitForever);
		<b>ASSERT_nonfatal(UART_RxBuffer != 0x03, ASSERT_UART_SIGINT, 0) // ^C character abort</b>
		// put the received character into the queue
		status = osMessageQueuePut(UART_RxQueueId, &UART_RxBuffer, 0, 100);
		if (status != osOK) {
			// can't put char into queue
			Error_Handler();
		}
		// receive the next character
		osMutexAcquire(UART_MutexID, osWaitForever);
		status = HAL_UART_Receive_IT(&huart1, &UART_RxBuffer, 1);
		osMutexRelease(UART_MutexID);
		if (status != osOK) {
			// can't receive char
			Error_Handler();
		}
	}
}
</pre>
