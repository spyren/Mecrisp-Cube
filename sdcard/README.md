![calculator](/sdcard/man/img/calculator-header.jpg)

# 4TH Calculator (Mecrisp-Cube calc branch)
I am using HP calculators for more than 45 years and wrote my very first program on a HP-41. 
There are many excellent HP calculator emulators for PCs and smart phones and HP still sells pocket 
[calculators]([https://www.hp.com/us-en/calculators.html](https://hpofficesupply.com/product-category/calculators/)).
Why should I build one by myself? The HP-15 Collector’s Edition could be the right one and it even can emulate the HP-16, 
but it is landscape and not cheap. 

A small pocket calculator for programmers and engineers, a mix between HP-42 and HP-16 would be nice to have.
This projetc fill the bill, at least for me.

I had all the parts in my drawer, leftovers from other projects. But other parts can easily be used.
The display is a bit small for my eyes, I will build another calculator with a bigger display and a 128x64 resolution, 
but this calculator will exceed the business card dimension.

It is a good opportunity to show how good Forth works on modern hardware.

## Features

### Calculator
  * RPN
  * Keyboard layout similar to HP 42
  * Stack 64 elements
  * 10 registers
  * 3 top elements (x, y, and z) are displayed
  * Floating-point
    * Single precision floating-point IEEE.754
    * Elementary arithmetic
    * trigometric and exponential functions
    * Display modes: fixed, scientific, engineering (metric unit prefix)
  * Integer
    * Hex, decimal, octal, and binary number base
    * Signed (2's complement) and unsigned numbers
    * Elementary arithmetic
    * logical and shift operations
  * Forth programmable

<table>
  <tr>
    <td><img src="/sdcard/man/img/calculator-top.jpg" width="500">
 </td>
    <td><img src="/sdcard/man/img/calculator-bottom.jpg" width="450"></td>
  </tr>
</table> 

### Standard Mecrisp-Cube Forth Features
  * 63 KiB RAM dictionary 
  * 128 KiB Flash Forth dictionary 
  * 50 KiB for C code 
  * Serial console USB CDC / BLE [Terminal-IO](/sdcard/man/TerminalIO.md)
  * [Filesystem](/sdcard/man/FileSystem.md) (FAT), Internal Flash drive 0:, 384 KiB
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
      serial communication via USB, redirect console I/O like cdc-emit, cdc-key
    * USB-MSC Device: The USB mass storage device class (also known as USB MSC or UMS).
      You can use your Forth system as a flash drive (also thumb drive [US], memory stick [UK],
      and pen drive/pendrive elsewhere)
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

  * 1 push button
  * 3 LEDs
  * Keyboard wth 35 buttons
  * I2C
    * OLED display 128x32 pixel 

For more BSP details see [BoardSupportPackage](/sdcard/man/BoardSupportPackage.md).


## About Forth
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

For this project you only need the dongle.

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


## Getting Started

These instructions will get you a copy of the project up and running on your local 
machine (Calculator, WB55 Dongle) for development and testing purposes. 


### Prerequisites
 
  * [STM32WB USB Dongle](https://www.st.com/en/evaluation-tools/p-nucleo-wb55.html) part of P-NUCLEO-WB55, DigiKey 497-18384-ND
  * Perfboard square grid of 0.1 inches, pads on both sides (plate-through holes), 2" x 3.5", $3
  * OLED display 0.91" 128x32 pixels, $5
  * 35 pcs push buttons, fit in 0.4" grid
    * Würth 430473035826, DigiKey #732-7021-1-ND, data sheet, $0.5
    * TE Connectivity ALCOSWITCH Switches 1825910-6, $0.15
    * C&K PTS 647 SN50 SMTR2 LFS, DigiKey #PTS647SN50SMTR2LFSCT-ND, smaller 4.5 x 4.5 mm, $0.2
  * USB Connector e.g.
    * Adafruit USB Micro-B Breakout Board, DigiKey 1528-1383-ND, from $1 up
    * With Micro-Lipo Charger
      * Adafruit Micro-Lipo Charger for LiPo/LiIon Batt w/MicroUSB Jack, $7
      * Adafruit Micro-Lipo Charger for LiPo/LiIon Batt w/USB Type C Jack, $7
  * LiPo battery (optional) e.g. Renata ICP422339PR
  * Terminal emulator application for PC, e.g.:
    * [tio](https://github.com/tio/tio), my favourite - Windows and Linux 
    * [PuTTY](http://www.putty.org/) - Windows and Linux
    * [Tera Term](http://en.sourceforge.jp/projects/ttssh2/) - Windows
    * [Realterm](http://realterm.sourceforge.net/) - Windows
    * minicom, microcom, screen - Linux
    * for details see [Terminal-IO](/sdcard/man/TerminalIO.md)
  * Hot glue or double sided mounting tape e.g. Tesa 55750
  * Laminating machine (optional) 
  * STM32CubeProgrammer


### Build the Hardware

![schematic](/sdcard/man/img/schematic.png)

#### Cut the perfboard
Cut the perfboard to 2" x 3.5" (50 x 89 mm) dimension or 20 x 35 pads.

#### Solder push buttons
<img src="/sdcard/man/img/buttons.jpg" width="200">
Solder the 35 push buttons to the perfboard top layer. Each button is in the middle of 4 x 3 pads, the grid is 0.4".

#### Wire the buttons 
<img src="/sdcard/man/img/button-row-column.jpg" width="200">
Wire the buttons to columns and rows on the perfboard bottom layer. 
Build a [keyboard matrix](https://en.wikipedia.org/wiki/Keyboard_matrix_circuit) without diodes. Pullups are integrated in the MCU.

#### Mount the dongle
Remove the USB connector from the dongle. 
Glue (hot glue, mounting tape) the dongle to the perfboard bottom layer, upper left corner. 
If you do not want to use the BLE, cut off the BLE antenna.

#### Mount USB Breakout
Glue (hot glue, mounting tape) the USB Breakout or the Adafruit Micro-Lipo Charger to the perfboard bottom layer.

#### Wire the buttons to the dongle
Wire the button rows and columns to the dongle.

|*Description*|*Dongle*|*Function* | *Calculator* |
|-------------|--------|-----------|--------------|
| PB3         | CN1.5  | SWO       | ROW0         |
| PB2         | CN1.7  |           | ROW1         |
| PA6         | CN1.9  | D12       | ROW2         |
| PA7         | CN1.10 | D11       | ROW3         |
| PB6         | CN2.6  | UARTRX    | ROW4         |
| PA9         | CN2.7  | D9        | ROW5         |
| PA8         | CN2.8  | D6        | ROW6         |
| PA0         | CN2.3  | A3        | COL0         |
| PA1         | CN2.10 | A2        | COL1         |
| PA2         | CN2.4  | D1        | COL2         |
| PA3         | CN2.5  | D0        | COL3         |
| PA5         | CN1.8  | D13       | COL4         |

#### Wire the JTAG SWD (optional)
Only needed if you want to debug the board.

|*Description*|*Dongle*|*Function* | *JTAG 10pin* |
|-------------|--------|-----------|--------------|
| GND         | CN1.1  | GND       |  3, 5, 9     |
| NRST        | CN1.2  | RES       |  10          |
| PA13        | CN1.3  | SWDIO     |  2           |
| PA14        | CN1.4  | SWDCLK    |  4           |
| 3V3         | CN1.6  | 3V3       |  1           |


#### Place the Diodes (optional)
<img src="/sdcard/man/img/diode-bat54c.jpg" width="500">

I use a BAT54C double schottky diode in SOT23 package. Solder the diode to the perfboard bottom layer. 
You can also use two 1N4148 diodes.

#### Wire the USB connector
<img src="/sdcard/man/img/calculator-bottom-cut.jpg" width="500">

USB breakout without charger:

|*Description*|*Dongle*|*Function* | *USB Breakout* |
|-------------|--------|-----------|----------------|
| GND         | GND    | GND       | GND            |
| 5 V         | 5 V    | 5 V       | 5 V            |
| D+          | D+     |           | D+             |
| D-          | D-     |           | D-             |

or with charger

|*Description*|*Dongle*|*Function* | *USB Charger* |
|-------------|--------|-----------|---------------|
| GND         | GND    | GND       | GND           |
| 5 V         | 5 V    | 5 V       | Diode Cathode |
| D+          | D+     |           | D+            |
| D-          | D-     |           | D-            |

|*Description*|*USB Charger*|*Function* | *Diode* |
|-------------|-------------|-----------|---------|
| 5 V         | 5 V         |           | Anode 1 |
| BAT         | BAT         |           | Anode 2 |

#### Mount the OLED Display
Solder or glue the OLED Display to the perfboard top layer.

#### Wire the OLED
|*Description*|*Dongle*|*Function* | *OLED*       |
|-------------|--------|-----------|--------------|
| GND         | CN1.1  | GND       | GND          |
| 3V3         | CN1.6  | 3V3       | VCC          |
| PB8         | CN2.1  | D15 SCL   | SCL          |
| PB9         | CN2.2  | D14 SDA   | SDA          |

#### Mount the Battery (optional)
Glue (hot glue, mounting tape) the LiPo battery to the perfboard bottom layer. 

#### Wire the Battery (optional)
If the battery has a molex connector, no wiring/solderin is required.

#### Keyboard Overlay

<table>
  <tr>
    <td><img src="/sdcard/man/img/overlay.svg" width="200">
 </td>
    <td><img src="/sdcard/man/img/overlay.png" width="200"></td>
  </tr>
</table> 

1. Print the keyboard overlay on heavy weight white paper
2. Cut out the overlay
3. Laminate the overlay
4. Cut out the laminated overlay
5. Cut holes for the push buttons, I use a leather puncher (4 mm holes)


#### Flash the 4TH Calculator Firmware

Flash the 4TH Calculator [binary](/sdcard/boot/MecrispCubeCalcFS.bin) `MecrispCubeCalcFS.bin` to the WB55 Nucleo dongle. Using the built-in USB DFU bootloader.

The USB Dongle does not have a ST-Link interface, but the STM32WB has a built-in 
boot-loader. This bootloader works via USB. As programming tool I use the 
the [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) package. 

1. For programming the switch SW2 has to be in position BOOT0.
2. Connect the dongle USB with the computer.
3. Flash the firmware with STM32CubeProgrammer, either with the CLI (see below) or the GUI (open file .., Download)
4. Disconnect dongle from the computer.
5. Set the switch SW2 to position 0.

<pre>
$ <b>alias cubepgmcli='/opt/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI'</b>
$ <b>cubepgmcli -c port=USB1 -d MecrispCubeCalcFS.bin 0x8000000 </b>
</pre>

#### Update BLE Stack (optional)

You can find the BLE Stack and FUS in 
[STM32CubeWB](https://www.st.com/en/embedded-software/stm32cubewb.html)
or from [GitHub](https://github.com/STMicroelectronics/STM32CubeWB), 
in the directory `Projects/STM32WB_Copro_Wireless_Binaries`. 
At time of writing the FUS is Version 1.2.0, the stack v1.19.1. The STM32CubeProgrammer is v2.16.0.

1. For programming the switch SW2 has to be in position BOOT0.
2. Connect the dongle USB with the computer.
3. Flash the firmware with STM32CubeProgrammer, either with the CLI (see below) or the GUI (use the firmware upgrade service, start address usually 0x080CE000)
4. Disconnect dongle from the computer.
5. Set the switch SW2 to position 0.

<pre>
$ <b>alias cubepgmcli='/opt/STMicroelectronics/STM32Cube/STM32CubeProgrammer/bin/STM32_Programmer_CLI'</b>
$ <b>cd STM32Cube_FW_WB_V1.19.1/Projects/STM32WB_Copro_Wireless_Binaries/STM32WB5x</b>
$ <b>cubepgmcli -c port=USB1 -fwupgrade stm32wb5x_BLE_Stack_full_fw.bin 0x080CE000 firstinstall=0</b>
</pre>


### Use the Terminal (USB CDC)

Connect the WB55 Dongle USB to the PC. Start the terminal emulator application on the PC. 
Check for the serial communication port (e.g. for Linux `/dev/ttyACM0`).
With `tio` you can list available devices, look for 4TH devices
```
$ tio -l
Device            TID     Uptime [s] Driver           Description
----------------- ---- ------------- ---------------- --------------------------
/dev/ttyACM0      38E5      8567.913 cdc_acm          ST-Link VCP Ctrl
/dev/ttyACM1      PESx       873.742 cdc_acm          4TH CDC

By-id
--------------------------------------------------------------------------------
/dev/serial/by-id/usb-spyr.ch_4TH_Calculator_43003F0013504D5943373420-if00
/dev/serial/by-id/usb-STMicroelectronics_STLINK-V3_002700073156501320323443-if02

By-path
--------------------------------------------------------------------------------
/dev/serial/by-path/pci-0000:00:14.0-usb-0:2:1.0
/dev/serial/by-path/pci-0000:00:14.0-usb-0:1:1.2
```

I set the terminal configuration (e.g. putty) to 

  * Implicit CR in every LF 
  * Local echo: off
  * Local line editing: off
  * Keyboard Backspace: Control-H
  * Keyboard Function: Keys VT100
  * Remote character set: CP850

With `tio`
```
tio --map ICRNL,INLCRNL /dev/serial/by-id/usb-spyr.ch_4TH_Calculator_43003F0013504D5943373420-if00 
```

The greeting screen should apear after pushing the **TERM** button on the Calculator:
```
4TH Calculator
--------------
Mecrisp-Cube 1.6.0 deb for STM32WB Nucleo, 63/128 KiB RAM/FLASH dictionary (C) 2024 peter@spyr.ch
  * Mecrisp-Stellaris RA 2.5.4 by Matthias Koch. 
  * Firmware Package STM32Cube FW_WB V1.17.3, BLE Stack 5.3 (C) 2023 STMicroelectronics 
  * CMSIS-RTOS V2 FreeRTOS wrapper, FreeRTOS Kernel V10.3.1 (C) 2020 Amazon.com
  * FatFs for internal flash and microSD - Generic FAT fs module  R0.12c (C) 2017 ChaN
  * tiny vi - part of BusyBox (C) 2000, 2001 Sterling Huxley
  * TinyUSB CDC, MSC v0.16.0 (C) 2023, hathach (tinyusb.org)
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

The display on the calculator is also updated. The display number mode is probably FLOAT, to see decimal values, hit
the **DEC** button.

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

  * *SW1* do not include `0:/etc/rc.local` on startup


#### LEDs as Status Indicator

The LEDs on the dongle displays the status
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

## How it Works

The idea behind this is to use as much of the Forth interpreter as possible. 
There is already a data stack, but with 64 elements and not only x, y, z, and t registers. 
The calculator keyboard acts as an ordinary keyboard (terminal), but one key can send several characters (string), 
e.g. the key **COS** sends a string `" fcos\n"`. The interpreter executes the float cosinus on the top of the stack. 
After the successful execution of a line, the stack content is then shown on the display. 
Digits (**0..9**, **A..F**) are forwarded to the display and are interpreted after hitting the **ENTER** key. 
If you connect a terminal and hit the **TERM** key, you can see how the interpreter works. 

For implementation details see
* [button.c](peripherals/button.c)
* [calculator.fs](sdcard/fsr/calculator.fs)

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


## Authors

* **Peter Schmid** - *Initial work* - [Mecrisp Cube](https://spyr.ch/twiki/bin/view/MecrispCube)
* **Matthias Koch** - *Forth on ARM Cortex M0/M4* - [Mecrisp Stellaris](http://mecrisp.sourceforge.net/)


## License

This project *4TH Calculator* is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

*4TH Calculator* is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with *4TH Calculator*. If not, see http://www.gnu.org/licenses/.


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
* [Learn X in Y minutes](https://learnxinyminutes.com/docs/forth/) Where X=Forth
* https://forth-ev.de/ Forth-Gesellschaft e.V.
* https://forth-ev.de/wiki/res/lib/exe/fetch.php/vd-archiv:4d2015-arm.pdf ARM-Sonderheft

* https://forth-standard.org/standard/words, Forth 2012 Standard Words
* https://gforth.org/manual/, GForth Manual


