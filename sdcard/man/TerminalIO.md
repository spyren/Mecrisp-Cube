![](img/mecrisp-cube-4th-logo-red-h.svg)

Intro
=====

To work interactively with a Forth system, you need some kind of
[console](https://en.wikipedia.org/wiki/System_console). A text
[terminal](https://en.wikipedia.org/wiki/Computer_terminal#Text_terminals)
with [RS-232](https://en.wikipedia.org/wiki/RS-232) port has been used
for this purpose in the past. Today's PCs have software to emulate a
terminal, but usually no longer have serial ports (RS-232 interfaces).
Instead, there are usually USB interfaces.

The
[USB-CDC](https://en.wikipedia.org/wiki/USB_communications_device_class)
appears as a traditional RS-232 port in your operating system (Linux
`/dev/ttyACMx`, Windoze `COMx`). No need to bother with baud rates,
handshaking, parity and other awkward things.

There is also a wireless console for PCs and mobile devices. The
proprietary STM BLE Cable Replacement Service is used as communication
protocol. 


Terminal-IO
===========

`emit` and `key` can block the calling thread, if the buffer is full
(`emit`) or empty (`key`). The other threads are not affected (no
busy-wait). The incoming characters (Rx) are buffered in a RTOS message
queue, outgoing characters are also buffered.

Default console is USB-CDC, but if you press button SW2 on reset, the
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

USB-CDC Serial Communication (API)
==================================

Buffered USB-CDC serial communication. The
[USB-CDC](https://en.wikipedia.org/wiki/USB_communications_device_class)
appears as a traditional RS-232 port in your operating system (Linux
`/dev/ttyACMx`, Windoze `COMx`). No need to bother with baud rates,
handshaking, parity and other awkward things. Under Linux I recommend to
disable the ModemManager, otherwise the ModemManager tries to configure
the ACM with some escape sequences.

After the first connect to the USB host the green LED is switched on.

USB_USER CN1 Micro USB connector on the MB1355 Nucleo Board.
USB_USER CN3 USB A plug on the MB1293 dongle.
```
cdc-emit   ( c -- ) Emits one character to the USB-CDC interface. Blocking if the Buffer is full.
cdc-key    ( -- c ) Waits and gets one character from the USB-CDC interface. Blocking if the buffer is empty.
cdc-emit?  ( -- f ) Ready to send a character. Buffer is not full.
cdc-key?   ( -- f ) Checks if a character is in the buffer.
```

UART Serial Communication (API)
===============================

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

BLE Cable Replacement Serial Communication (API)
================================================

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


Terminal Emulators
==================

Terminal emulator applications for PCs, e.g.:

-   [PuTTY](http://www.putty.org/) - Windows and Linux
-   [Tera Term](http://en.sourceforge.jp/projects/ttssh2/) - Windows
-   [Realterm](http://realterm.sourceforge.net/) - Windows
-   minicom, miniterm, microcom, CuteCom, screen - Linux

Terminal emulator for Android devices:

-   [Serial USB
    Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_usb_terminal)
-   [Serial Bluetooth
    Terminal](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal)

Settings

-   Terminal: Display mode Terminal
-   Receive: Newline CR+LF
-   Send: Newline CR, Local echo off



\-- [PeterSchmid - 2020-04-20]

