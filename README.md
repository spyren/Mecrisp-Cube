# Mecrisp-Cube

Mecrisp-Stellaris Forth for the STM32 Cube ecosystem. 

Forth is an interactive and extensible language, with built-in lexical analysis (tokenizer, parser) and interpreter/compiler, needs less than 20 KiB Flash and 4 KiB RAM, unbelievable for a self-contained (self-hosted) system. 

Forth is perfect for embedded systems where some sort of user interactivity like CLI and extensibility (at runtime) are needed. 

C & Forth in the 21st Century. C and Forth are both about 50 years old. To combine the strength of this two worlds results in a powerful system that outperforms other much newer systems like Python. Good design withstands the test of time.

The goal of Mecrisp-Cube is to be a complete Forth programming environment for STM32 MCUs. There are three flavors available:

  * MecrispCubeWB (BLE) for the STM32WB55 Nucleo Board and dongle and variants for the [Firefly](../firefly), [Katydid](../katydid), [Discovery](../WB5M) and the [STM32WB Feather](../WBfeater) Dev Board. In work [STM32WB Flipper Zero](../Flipper)
  * [F405](../F405) for Adafruit's Feather STM32F405.
  * [H74x](../H74x3) for STM NUCLEO-H743ZI and Arduino Portenta H7 Development Board.


## Intro for the Flipper Zero

Instant real-time programming with Forth. 
The [Flipper Zero](https://flipperzero.one/) is an excellent portable tool for interaction with access control systems. 
But the display, the buttons, LIPO (2.1 Ah), GPIO, BLE, SD-Card, RTC, etc. make it also an ideal tool for programming on the go.

## Features

### Standard Mecrisp-Cube features
  * 63 !KiB RAM dictionary 
  * 128 !KiB Flash Forth dictionary 
  * 50 !KiB for C code 
  * Serial console UART / USB CDC / BLE [Terminal-IO](sdcard/man/TerminalIO.md)
  * [Filesystem](sdcard/man/FileSystem.md) (FAT)
    * Internal Flash drive 0:, 384 !KiB
    * microSD drive 1: 
  * Integration in STM32 Cube Ecosystem. 
    * Create C code from CubeMX for internal peripherals and use it in Forth
    * [Calling C Functions](sdcard/man/CallingCFunction.md) from Forth and vice versa 
  * [RTOS](sdcard/man/CmsisRtos.md)
    * Forth as CMSIS-RTOS thread.
    * CMSIS-RTOS API to use FreeRTOS from Forth.
    * Buffered terminal I/O (5 KiB buffer for UART Rx). Interrupt driven and RTOS aware, key and emit block the calling thread. 
  * USB
    * [USB-CDC](sdcard/man/TerminalIO.md#usb-cdc-serial-communication-api) for serial communication via USB
    * Redirect console I/O like cdc-emit, cdc-key
  * BLE 5.0 GAP Peripheral Role (STM32WB)
    * DIS Device Information Service
    * HRS Heart Rate Service (heart rate depends on A0 for Nucleo and A2 for Dongle)
    * [CRS](sdcard/man/TerminalIO.md#ble-cable-replacement-serial-communication-api) Cable Replacement Server service (proprietary service from STM, similar to Classic Bluetooth SPP). Redirect console I/O like crs-emit, crs-key. 
  * Floating-Point Unit [FPU](sdcard/man/fpu.md)
    * Support for the floating-point unit FPU, single precision for M4F MPUs and double precision for M7 MPUs
    * CMSIS-DSP 
  * [vi](sdcard/man/EditorVi.md) editor
  * Real Time Clock [RTC](sdcard/man/RealTimeClock.md)
  * Watchdog
  * Assertion and Logging

### Board Support Package [BSP](sdcard/man/BoardSupportPackage.md)
  * Control (5-button joystick, Back button, Reboot)
  * LCD display 128x64 pixel
  * RGB LED
  * Power (LIPO charger and fuel gauge)
  * Vibration Motor
  * GPIO

Not supported yet:
  * Sub-1 GHz Transceiver
  * 125 kHz RFID
  * NFC
  * Infrared Transceiver
  * iButton
  * Buzzer/Speaker

For more BSP details see [BoardSupportPackage](sdcard/man/BoardSupportPackage.md).

### External Peripherals (e.g. Feather Wings) 

  * OLED Display 128x32, 128x64, I2C
  * E-Ink FeatherWing 250x122 SPI
  * NeoPixel
  * CharlieWing
  * NeoPixelWing
  * DotStarWing 

## Getting Started

These instructions will get you a copy of the project up and running on your local 
machine (Flipper Zero) for development and testing purposes. 

### Prerequisites

  * [Flipper Zero](https://flipperzero.one/) with STM32WB55 MCU (Cortex ARM M4) runs at a 32 MHz (the Bluetooth stack runs on a Cortex ARM M0+ core). 
  * Optional: [ST-Link V3 Developer Board](https://docs.flipper.net/development/hardware/devboard-stlinkv3) or you can build your own e.g. with a [STLINK-V3MINI](https://www.st.com/en/development-tools/stlink-v3mini.html) and some cables, see BoardSupportPackageFlipper#JTAG_SWD_Adaptor
  * Terminal emulator application for PC, e.g.: 
    * [PuTTY](http://www.putty.org/) - Windows and Linux
    * [Tera Term](http://en.sourceforge.jp/projects/ttssh2/) - Windows
    * [Realterm](http://realterm.sourceforge.net/) - Windows
    * minicom, microcom, screen - Linux
    * Use the built in Eclipse console (but no LF)
    * for details see [TerminalIO]
  * STM32CubeProgrammer or [qFlipper](https://docs.flipper.net/qflipper)

### Flash the Mecrisp-Cube Firmware

Flash the Mecrisp-Cube [binary](https://github.com/spyren/Mecrisp-Cube/raw/Flipper/Release/MecrispCubeFlipper.bin) `MecrispCubeFlipper.bin` or better the [fs-util-binary](https://github.com/spyren/Mecrisp-Cube/raw/Flipper/sdcard/boot/MecrispCubeFlipperFS.bin) (`MecrispCubeFlipperFS.bin`) to the Flipper Zero. Using the built-in USB DFU bootloader, see also [firmware recovery](https://docs.flipper.net/basics/firmware-update/firmware-recovery).

   1. Press and hold the OK and the back buttons for 30 s (you should see a blank screen)
   1. Connect the Flipper Zero USB to the PC
   1. Program the binary (`MecrispCubeFlipper.bin` or `MecrispCubeFlipperFS.bin`) with 
      1. the STMCubeProgrammer (select USB Device), for Linux 
         `sudo /usr/local/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32CubeProgrammer`
      1. or the [qFlipper](https://docs.flipper.net/qflipper) with _install from file_
   1. Reboot your device by pressing and holding the left and back buttons 


### Use the Terminal (USB CDC)

Connect the Flipper Zero USB to the PC. Start the terminal emulator application on the PC. 
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
23 5 /
```
This looks like this on your terminal:
```
23 5 / .[CR] 4  ok.
```
The `ok.` is the Forth prompt and apears at the end of the line (Forth does it differently). If you don't like it, [change it](sdcard/man/FileSystem.md#shell-prompt).
Type in your first Forth program (create a word in the RAM dictionray):
```
: hello ." World" ;
```
and execute the the program
```
hello 
```
again the terminal output:
```
: hello ." World" ;[CR]  ok.
hello[CR] World ok.
```
The program `hello` ist not persistent, after power cycle or even reset the RAM dictionray is erased. 
But it is easy to add a word to the Flash dictionray:
```
compiletoflash
: hello ." World" ;
```
The program source is not saved, only the executable machine code is compiled into the flash. 

The following part is only for people who are interested how Forth works and have knowledge about 
the ARM Assembler.
There is a built-in disassembler (consider the machine code `B500` is 16 bit hex number, but i is stored 
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

### Switch On/Off, Buttons, LED

  * *Switch Off* press and hold the BACK-Button for 5 s or type in the command =halt=
  * *Switch On* press the BACK-Button 
  * *Reset* press and hold the BACK- and LEFT-Button


#### Special Functions on Startup 
USB-CDC is the default console.
  * *Button UP* [CRS](TerminalIO) (Bluetooth Cable Replacement Service) is standard console
  * *Button DOWN* [UART](TerminalIO#UART_Serial_Communication_API) is standard console
  * *Button RIGHT* do not include `0:/etc/rc.local`

#### RGB LED as Status Indicator
The RGB LED  displays the status
  * *dimmed Green* LIPO fully charged
  * *dimmed Red* LIPO charging
  * *dimmed Blue* BLE connected
  * *flashing Red* "disk" (serial flash or SD) write operation
  * *flashing Yellow* "disk" (serial flash or SD) read operation


### Flash the original Flipper Firmware

If you want to go back to the original firmware, do the following

   1. Press and hold the OK and the back buttons for 30 s (you should see a blank screen)
   1. Connect the Flipper Zero USB to the PC
   1. Flash the Flipper firmware with [qFlipper](https://docs.flipper.net/qflipper) _REPAIR_
   1. Reboot your device by pressing and holding the left and back buttons 


## Installing Development Environment 

A step by step series of examples that tell you how to get a development env running

Install the IDE [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), 
it is Eclipse and GCC based. 
[STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
is included in the IDE, you need a stand alone installation only if 
you do not want to use the STM32CubeIDE.  

Get the sources from github:

```
psi@homer:~> git clone --branch Flipper https://github.com/spyren/Mecrisp-Cube
Klone nach 'Mecrisp-Cube' ...
remote: Enumerating objects: 106, done.
remote: Counting objects: 100% (106/106), done.
remote: Compressing objects: 100% (71/71), done.
remote: Total 106 (delta 33), reused 106 (delta 33), pack-reused 0
Empfange Objekte: 100% (106/106), 938.96 KiB | 2.39 MiB/s, Fertig.
Löse Unterschiede auf: 100% (33/33), Fertig.
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
Append ?ts=4 to the URL to change the tab-size.

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
* https://jeelabs.org/article/1612b/ Forth in 7 easy steps, **Jean-Claude Wippler**
* https://forth-ev.de/ Forth-Gesellschaft e.V.
* https://forth-ev.de/wiki/res/lib/exe/fetch.php/vd-archiv:4d2015-arm.pdf ARM-Sonderheft


