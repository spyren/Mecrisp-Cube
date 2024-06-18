<table>
  <tr>
    <td><img src="/sdcard/man/img/nucleo-header.jpg">
 </td>
    <td><img src="/sdcard/man/img/mecrisp-cube-logo-red-h.svg"></td>
  </tr>
</table> 

# 4TH Calculator (Mecrisp-Cube calc branch)

Mecrisp-Stellaris Forth for the STM32 Cube ecosystem. 

Forth is an interactive and extensible language, with built-in lexical analysis 
(tokenizer, parser) and interpreter/compiler, needs less than 20 KiB Flash and 
4 KiB RAM, unbelievable for a self-contained (self-hosted) system. 

Forth is perfect for embedded systems where some sort of user interactivity 
like CLI and extensibility (at runtime) are needed. 

C & Forth in the 21st Century. C and Forth are both about 50 years old. 
The combination of the strength of this two worlds results in a powerful 
system that outperforms other much newer systems like Python. 
Good design withstands the test of time.

The goal of Mecrisp-Cube is to be a complete Forth programming environment 
for STM32 MCUs. There are three flavors (branches) available:

  * [STM32WB55](../master) (`master` branch) for the _STM32WB55 Nucleo Board_ and _dongle_ and variants for the
    [Firefly Dev Board](../firefly) (`firefly`), [Katydid Dev Board](../katydid) (`katydid`),
    [WB5M Discovery](../WB5M) (`WB55M`) and the [STM32WB Feather](../WBfeather) (`WBfeather`).
    In work [Flipper Zero](../Flipper) (`Flipper`):
  * [STM32F405](../F405) (`F405`) for Adafruit's Feather STM32F405.
  * [STM32H74x](../H743) (`H743`) for STM NUCLEO-H743ZI and Arduino Portenta H7 Development Board.

May the Forth Be With You!


## Intro for the Nucleo STM32WB55 Nucleo Board and Dongle

If you buy a [P-NUCLEO-WB55](https://www.st.com/en/evaluation-tools/p-nucleo-wb55.html) pack
you get a Nucleo68 Board and a USB Dongle. Both are supported by Mecrisp-Cube.

The Nucleo board has a ST-LINK on board. There are also an Arduino UNO R3 pin header 
and LEDs and switches (buttons).

STM32WB Series has two CPUs. An ARM Cortex M4 for the application (CPU1) and a 
Cortex M0+ (CPU2) for the BLE protocol stack. This Forth system runs on the CPU1. 
Developed with the same technology as the ultra-low power STM32L4 microcontrollers, 
the STM32WB MCU series provides the same digital and analog peripherals suitable 
for applications requiring an extended battery life and complex functionalities. 
If you do not need wireless connectivity, thanks to CubeMX you can easily adapt 
Mecrisp-Cube WB for STM32L4, and probably for STM32L5, and STM32U5.

![](/sdcard/man/img/stm32wb-nucleo.jpg)


## Features

### Standard Mecrisp-Cube features
  * 63 KiB RAM dictionary 
  * 128 KiB Flash Forth dictionary 
  * 50 KiB for C code 
  * Serial console UART / USB CDC / BLE [Terminal-IO](/sdcard/man/TerminalIO.md)
  * [Filesystem](/sdcard/man/FileSystem.md) (FAT)
    * Internal Flash drive 0:, 384 KiB
    * microSD drive 1: (optional, external SD adaptor)
  * Integration in STM32 Cube Ecosystem. 
    * Create C code from CubeMX for internal peripherals and use it in Forth
    * [Calling C Functions](/sdcard/man/CallingCFunction.md) from Forth and vice versa 
  * [RTOS](/sdcard/man/CmsisRtos.md)
    * Forth as CMSIS-RTOS thread.
    * CMSIS-RTOS API to use FreeRTOS from Forth.
    * Buffered terminal I/O (5 KiB buffer for UART Rx). Interrupt driven and RTOS aware,
      key and emit block the calling thread. 
  * USB
    * [USB-CDC](/sdcard/man/TerminalIO.md#usb-cdc-serial-communication-api) for
      serial communication via USB
    * Redirect console I/O like cdc-emit, cdc-key
  * BLE 5.0 GAP Peripheral Role (STM32WB)
    * DIS Device Information Service
    * HRS Heart Rate Service (heart rate depends on A0 for Nucleo and A2 for Dongle)
    * [CRS](/sdcard/man/TerminalIO.md#ble-cable-replacement-serial-communication-api)
      Cable Replacement Server service (proprietary service from STM, similar to
      Classic Bluetooth SPP).
      Redirect console I/O like crs-emit, crs-key. 
  * Floating-Point Unit [FPU](/sdcard/man/fpu.md)
    * Support for the floating-point unit FPU, single precision for M4F MPUs
      and double precision for M7 MPUs
    * CMSIS-DSP 
  * [vi](/sdcard/man/EditorVi.md) editor
  * Real Time Clock [RTC](/sdcard/man/RealTimeClock.md)
  * [Watchdog](/sdcard/man/watchdog.md)
  * [Assertion and Logging](/sdcard/man/assert.md)


### Board Support Package [BSP](/sdcard/man/BoardSupportPackage.md)

  * 3 push buttons
  * 3 LEDs
  * 16 GPIO pins, D0 .. D15
  * 6 ADC pins, A0 .. A5
  * I2C
  * SPI
  * 3 PWM pins
  * Input Capture / Output Compare
  * 4 external interrupts (EXTI)

For more BSP details see [BoardSupportPackage](/sdcard/man/BoardSupportPackage.md).


### External Peripherals (e.g. Feather Wings) 

  * OLED Display 128x32, 128x64, I2C
  * E-Ink FeatherWing 250x122 SPI
  * NeoPixel
  * CharlieWing
  * NeoPixelWing
  * DotStarWing 


## Getting Started

These instructions will get you a copy of the project up and running on your local 
machine (WB55 Nucleo or Dongle) for development and testing purposes. 


### Prerequisites

  * One of these boards
    * [STM32WB Nucleo Board](https://www.st.com/en/evaluation-tools/p-nucleo-wb55.html)
      Arduino™ Uno V3 and ST morpho connectors
    * [STM32WB USB Dongle](https://www.st.com/en/evaluation-tools/p-nucleo-wb55.html)
  * Terminal emulator application for PC, e.g.: 
    * [PuTTY](http://www.putty.org/) - Windows and Linux
    * [Tera Term](http://en.sourceforge.jp/projects/ttssh2/) - Windows
    * [Realterm](http://realterm.sourceforge.net/) - Windows
    * minicom, microcom, screen - Linux
    * Use the built in Eclipse console (but no LF)
    * for details see [TerminalIO]
  * STM32CubeProgrammer (optional)


### Flash the Mecrisp-Cube Firmware

Flash the Mecrisp-Cube [binary](/Release/MecrispCube.bin) `MecrispCube.bin` or better the [fs-binary](/sdcard/boot/MecrispCubeFS.bin) 
`MecrispCubeFS.bin` to the WB55 Nucleo. Using the built-in USB DFU bootloader.

 1. Connect the Nucleo Board USB ST-LINK to the PC
 1. Copy binary (MecrispCube.bin or better the MecrispCubeFS.bin) to the USB mass storage NODE_WB55RG 


### Use the Terminal (USB CDC)

Connect the WB55 Nucleo USB to the PC. Start the terminal emulator application on the PC. 
Check for the serial communication port (e.g. for Linux `/dev/ttyACM0`).
I set the putty terminal configuration to 

  * Implicit CR in every LF 
  * Local echo: off
  * Local line editing: off
  * Keyboard Backspace: Control-H
  * Keyboard Function: Keys VT100
  * Remote character set: CP850

The greeting screen should apear after startup:
```
Mecrisp-Stellaris RA 2.5.4 by Matthias Koch.

Mecrisp-Cube 1.5.0 for STM32WB Flipper, 63/128  KiB RAM/FLASH dictionary (C) 2023 peter@spyr.ch
  * Firmware Package STM32Cube FW_WB V1.17.3, USB-CDC, BLE Stack 5.3 (C) 2023 STMicroelectronics
  * CMSIS-RTOS V2 FreeRTOS wrapper, FreeRTOS Kernel V10.3.1 (C) 2020 Amazon.com
  * FatFs for internal flash and microSD - Generic FAT fs module  R0.12c (C) 2017 ChaN
include 0:/etc/rc.local
```
Use the interpreter ([reverse polnish notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation), like HP calculators):
```
23 5 / .
```
This looks like this on your terminal (**bold** is the Forth answer):
<pre>
23 5 / .[CR] <b>4  ok.</b>
</pre>
The `ok.` is the Forth prompt and apears at the end of the line (Forth does it differently, like most things ;-). 
If you don't like it, [change it](/sdcard/man/FileSystem.md#shell-prompt). 
`[CR]` is the Enter-key.

Type in your first Forth program (create a word in the RAM dictionray):
```
: hello ." World" ;
```
and execute the the program
```
hello 
```
again the terminal output:
<pre>
: hello ." World" ;[CR]  <b>ok.</b>
hello[CR] <b>World ok.</b>
</pre>
The program `hello` ist not persistent, after power cycle or even reset the RAM dictionray is erased. 
But it is easy to add a word to the Flash dictionray:
```
compiletoflash
: hello ." World" ;
```
The program source is not saved, only the executable machine code is compiled into the flash. 
You can use the built-in editor [vi](/sdcard/man/EditorVi.md) and save your source to the 
[filesystem](/sdcard/man/FileSystem.md) either on internal flash drive `0:` or on the microSD card drive `1:`.

The following part is only for people who are interested how Forth works and have knowledge about 
the ARM Assembler.
There is a built-in disassembler (consider the machine code `B500` is 16 bit hex number, but it is stored 
as `00` `B5`):
```
see hello
08043558: B500  push { lr }
0804355A: F7BF  bl  08002BE4  -->  .' World'
0804355C: FB43
0804355E: 5705
08043560: 726F
08043562: 646C
08043564: BD00  pop { pc }
```
The dictionray entry looks like this (you can see the 'hello' and the string constant 'World'):
```
$08043558 10 dump
08043550 :  00 00 05 68 65 6C 6C 6F   00 B5 BF F7 43 FB 05 57  | ...hello  ....C..W |
08043560 :  6F 72 6C 64 00 BD 00 00   FF FF FF FF FF FF FF FF  | orld....  ........ |
```
The compiled word `hello` needs only 14 bytes in the dictionary.

### Buttons, LED

#### Special Functions on Startup 

USB-CDC is the default console.
  * *SW1* [CRS](TerminalIO) (Bluetooth Cable Replacement Service) is standard console
  * *SW2* [UART](TerminalIO#UART_Serial_Communication_API) is standard console
  * *SW3* do not include `0:/etc/rc.local`


#### LEDs as Status Indicator

The RGB LED  displays the status
  * *Green* USB enumerated
  * *Red* Error occured
  * *Blue* BLE connected
  * *flashing Red* "disk" (serial flash or SD) write operation
  * *flashing Yellow* "disk" (serial flash or SD) read operation


## Installing Development Environment 

A step by step series of examples that tell you how to get a development env running

Install the IDE [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), 
it is Eclipse and GCC based. 
[STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
is included in the IDE, you need a stand alone installation only if 
you do not want to use the STM32CubeIDE.  

Get the sources from github:

```
psi@homer:~> git clone https://github.com/spyren/Mecrisp-Cube
Cloning into 'Mecrisp-Cube'...
remote: Enumerating objects: 8334, done.
remote: Counting objects: 100% (2220/2220), done.
remote: Compressing objects: 100% (860/860), done.
remote: Total 8334 (delta 1458), reused 2059 (delta 1335), pack-reused 6114
Receiving objects: 100% (8334/8334), 108.93 MiB | 13.49 MiB/s, done.
Resolving deltas: 100% (5788/5788), done.
```

Import the project into the IDE:

```
File -> Import -> General -> Existing Projects into Workspace -> Select root directory
Copy project into workspace
Browse to Mecrisp-Cube directory
```

Generate code from the STM32CubeMX `MecrispCube.ioc` file:

```
Project -> Generate Code 
```

Restore changed source files
```
$ git status
{list of changed files}
$ git restore {files to restore} 
```
alternative
```
$ git stash
```

Select the Build Configuration (Debug if you want to debug the project) and Build the project:

```
Project -> Build Configurations -> Set Active -> Debug/Release 
Project -> Build Project
```


## Built With

* [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) - 
  STM32CubeIDE is an all-in-one multi-OS development tool, 
  which is part of the STM32Cube software ecosystem. The IDE is used for 
  development, GCC tools are included.
* [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) - 
  STM32CubeMX is a graphical tool that allows a very easy configuration of 
  STM32 microcontrollers and microprocessors, as well as the generation of 
  the corresponding initialization C code for the Arm® Cortex®-M core or a 
  partial Linux® Device Tree for Arm® Cortex®-A core), through a step-by-step process. 
* [STM32CubeProg](https://www.st.com/en/development-tools/stm32cubeprog.html) - 
  STM32CubeProgrammer (STM32CubeProg) is an all-in-one multi-OS software 
  tool for programming STM32 products. 

The STM tools work on Linux, Windows, and Mac.

Tab size is 4 for C and assembler source files. 
Append `?ts=4` to the URL to change the tab-size.


## Flash Mecrisp-Cube to the Target

Alternate ways to flash Mecrisp-Cube to the target. For the easy way see 
MecrispCubeWB#Prerequisites. 


### Nucleo Board

Connect Nucleo's ST-LINK USB to you PC. Erase the flash e.g. by `openOCD`, 
we use 768 KiB %ICON{right}% 192 sectors. 
<pre>
$ <b>telnet localhost 4444</b>
Trying ::1...
telnet: connect to address ::1: Connection refused
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Open On-Chip Debugger
> <b>reset init</b>
Unable to match requested speed 500 kHz, using 480 kHz
Unable to match requested speed 500 kHz, using 480 kHz
target halted due to debug-request, current mode: Thread 
xPSR: 0x01000000 pc: 0x08003aae msp: 0x20000430
> <b>flash erase_sector 0 0 192</b>
erased sectors 0 through 192 on flash bank 0 in 4.583453s
> <b>flash write_image mecrisp-stellaris-stm32wb55.hex</b>
device idcode = 0x20016495 (STM32WB5x - Rev: 2.1)
flash size = 1024kbytes
flash mode : single-bank
Padding 4 bytes to keep 8-byte write size
block write succeeded
wrote 15404 bytes from file mecrisp-stellaris-stm32wb55.hex in 0.329747s (45.620 KiB/s)

> <b>shutdown</b>
shutdown command invoked
Connection closed by foreign host.
</pre>


### USB Dongle

The USB Dongle does not have a ST-Link interface, but the STM32WB has a built-in 
boot-loader. This bootloader works via USB. As programming tool I use the 
CLI from the [STM32CubeProg](https://www.st.com/en/development-tools/stm32cubeprog.html) package. 

<pre>
$ <b>alias cubepgmcli='/opt/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI'</b>
$ <b>cubepgmcli -c port=USB1 -e [0 191]</b>
$ <b>cubepgmcli -c port=USB1 -d Release/MecrispCube.bin 0x8000000 </b>
$ <b>cubepgmcli -c port=USB1 -ob displ</b>
</pre>

BTW you can flash the Nucleo Board in the same way. 


### Update BLE Stack

You can find the BLE Stack and FUS in 
[STM32CubeWB](https://www.st.com/en/embedded-software/stm32cubewb.html)
or from [GitHub](https://github.com/STMicroelectronics/STM32CubeWB), 
in the directory `Projects/STM32WB_Copro_Wireless_Binaries`. 
At time of writing the FUS is Version 1.2.0, the stack v1.17.3. The STM32CubeProgrammer is v2.14.0.

Nucleo board: Using USB_USER interface and the built-in bootloader (activate with jumper between CN7.5 and CN7.7)
<pre>
$ <b>alias cubepgmcli='/opt/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI'</b>
$ <b>cd STM32Cube_FW_WB_V1.14.1/Projects/STM32WB_Copro_Wireless_Binaries/STM32WB5x</b>
$ <b>cubepgmcli -c port=USB1 -fwdelete</b>
$ <b>cubepgmcli -c port=USB1 -fwupgrade stm32wb5x_FUS_fw.bin 0x080EC000 firstinstall=0</b>
$ <b>cubepgmcli -c port=USB1 -fwupgrade stm32wb5x_FUS_fw_for_fus_0_5_3.bin 0x080EC000 firstinstall=0</b>
$ <b>cubepgmcli -c port=USB1 -fwupgrade stm32wb5x_BLE_Stack_full_fw.bin 0x080CB000 firstinstall=1</b>
</pre>

Alternate way to erase the flash memory (change the Memory Read Protection from Level 1 to level 0):
<pre>
$ <b>cubepgmcli -c port=swd -ob rdp=0xbb</b>
$ <b>cubepgmcli -c port=swd -rdu</b>
</pre>


## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/spyren/Mecrisp-Cube/tags). 

### v1.0.0 Initial Release
* USART0 default terminal console, 115200 Baud, 20 KiB Rx buffer
* USB-CDC secondary terminal console, type `cdc` to change console, back with `uart`.
* 512 KiB Flash dictionary, 60 KiB RAM dictionary
### v1.2.2 FatFs/BLE/RTOS Release
* CMSIS-RTOS, FreeRTOS Kernel V10.2.1
* FatFs R0.12c, some Unix like command line tools
* BLE Cable Replacement Service.
### v1.3.3 FatFs/BLE/RTOS/vi Release
* CMSIS-RTOS, FreeRTOS Kernel V10.2.1
* FatFs R0.12c, some Unix like command line tools
* BLE Cable Replacement Service.
* tiny vi - part of BusyBox 
### v1.4.1 OLED/FlashDrive Release
* Mecrisp-Stellaris RA 
* Flash Drive
* OLED driver, I2C, timer, RTC
* Markdown docs
### v1.4.3 F405 Release
* Own Tags for F405 and WB55 chips
* `0:/etc/rc.local` startup script
* 2 MiB flash drive for F405
### v1.5.0 FPU Release
* Support for Floating-Point Unit
* EPD driver
* Watchdog and Asserts
### v1.5.1 Flipper Release
* LCD driver (Flipper)
* Docu on GitHub


## Authors

* **Peter Schmid** - *Initial work* - [Mecrisp Cube](https://spyr.ch/twiki/bin/view/MecrispCube)
* **Matthias Koch** - *Forth on ARM Cortex M0/M4* - [Mecrisp Stellaris](http://mecrisp.sourceforge.net/)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.


## License

This project *Mecrsip-Cube* is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

*Mecrsip-Cube* is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with *Mecrsip-Cube*. If not, see http://www.gnu.org/licenses/.


## Acknowledgments

* http://mecrisp.sourceforge.net/ Original Mecrisp project, **Matthias Koch**
* https://mecrisp-stellaris-folkdoc.sourceforge.io/ Mecrisp Stellaris Unofficial UserDoc, **Terry Porter**


## Documentation

### Local (Mecrisp-Cube specific)

 * [Words](/sdcard/man/words.md), built-in words
 * [Board Support Package](/sdcard/man/BoardSupportPackage.md), board specific
 * [RTOS](/sdcard/man/CmsisRtos.md), CMSIS real-time OS
 * [Terminal IO](/sdcard/man/TerminalIO.md)
 * [Filesystem](/sdcard/man/FileSystem.md)
 * [VI Editor](/sdcard/man/EditorVi.md)
 * Error Handling ([Assertion](/sdcard/man/assert.md) and [Watchdog](/sdcard/man/watchdog.md))
 * [Real Time Clock](/sdcard/man/RealTimeClock.md)
 * [Floating Point Unit](/sdcard/man/fpu.md)
 * [C API](/sdcard/man/CallingCFunction.md)
 * [Display](/sdcard/man/display.md)


### Elsewhere

* [Leo Brodie, Starting Forth](https://www.forth.com/starting-forth/) 
* [Leo Brodie, Thinking Forth](https://thinking-forth.sourceforge.net/)
* https://jeelabs.org/article/1612b/ Forth in 7 easy steps, **Jean-Claude Wippler**
* https://forth-ev.de/ Forth-Gesellschaft e.V.
* https://forth-ev.de/wiki/res/lib/exe/fetch.php/vd-archiv:4d2015-arm.pdf ARM-Sonderheft

* https://forth-standard.org/standard/words, Forth 2012 Standard Words
* https://gforth.org/manual/, GForth Manual


