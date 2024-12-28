<table>
  <tr>
    <td><img src="/sdcard/man/img/nucleo-header.jpg">
 </td>
    <td><img src="/sdcard/man/img/mecrisp-cube-logo-red-h.svg"></td>
  </tr>
</table> 

# Egg Timer (Mecrisp-Cube egg branch)

An altoid tin box project. 

https://en.wikipedia.org/wiki/Egg_timer

An egg timer or kitchen timer is a device whose primary function is to assist in timing during cooking; 
the name comes from the first timers initially being used for the timing of cooking eggs. 
Early designs simply counted down for a specific period of time.

## Features

  * Count down max. 99:59 time period
  * Alarm
  * Start/stop

    
### Board Support Package [BSP](/sdcard/man/BoardSupportPackage.md)

  * 4 push buttons (GPIO)
  * Piezo Buzzer (PWM)
  * I2C
    * Quad Alphanumeric display 

For more BSP details see [BoardSupportPackage](/sdcard/man/BoardSupportPackage.md).


## Getting Started

These instructions will get you a copy of the project up and running on your egg timer 
for development and testing purposes. 


### Prerequisites
 
  * [STM32WB USB Dongle](https://www.st.com/en/evaluation-tools/p-nucleo-wb55.html) part of P-NUCLEO-WB55, DigiKey 497-18384-ND
  * Altoid tin box
  * Perfboard square grid of 0.1 inches, pads on both sides (plate-through holes), 2" x 3.5", $3
  * Display , [Adafruit Quad Alphanumeric FeatherWing Display](https://www.adafruit.com/product/3131) $10
  * 4 pcs push buttons, e.g. [STEMMA Wired Tactile Push-Button Pack](https://www.adafruit.com/product/4431), $7
  * Adafruit Micro-Lipo Charger for LiPo/LiIon Batt w/USB Type C Jack, $7
  * LiPo battery (optional) e.g. Renata ICP422339PR
  * Hot glue or double sided mounting tape e.g. Tesa 55750
  * Laminating machine (optional) 
  * STM32CubeProgrammer

### Build the Hardware

![schematic](/sdcard/man/img/schematic.png)

#### Cut the perfboard
Cut the perfboard to 2" x 3.5" (50 x 89 mm) dimension or 20 x 35 pads.

#### Glue or solder push buttons
<img src="/sdcard/man/img/buttons.jpg" width="200">
Glue or solder the 4 push buttons to the perfboard top layer. 

#### Mount the dongle
Remove the USB connector from the dongle. 
Glue (hot glue, mounting tape) or solder the dongle to the perfboard. 
If you do not want to use the BLE, cut off the BLE antenna.

#### Mount USB Breakout
Glue (hot glue, mounting tape) or solder the USB Breakout or the 
Adafruit Micro-Lipo Charger to the perfboard.

#### Wire the buttons to the dongle
Wire the buttons to the dongle. Pullups are integrated in the MCU.

|*Description*|*Dongle*|*Function* | *Egg Timer * |
|-------------|--------|-----------|--------------|
| PA0         | CN2.3  | A3        | a on/off     |
| PA1         | CN2.10 | A2        | b sec        |
| PA2         | CN2.4  | D1        | c min        |
| PA3         | CN2.5  | D0        | d start/stop |

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

I use a BAT54C double schottky diode in SOT23 package. 
Solder the diode to the perfboard. 
You can also use two 1N4148 diodes.

#### Wire the USB connector
<img src="/sdcard/man/img/calculator-bottom-cut.jpg" width="500">

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

#### Mount the Quad Alphanumeric Display
Solder the Quad Alphanumeric Display to the perfboard top layer.

#### Wire the Quad Alphanumeric Display
|*Description*|*Dongle*|*Function* | *Quad*       |
|-------------|--------|-----------|--------------|
| GND         | CN1.1  | GND       | GND          |
| 3V3         | CN1.6  | 3V3       | VCC          |
| PB8         | CN2.1  | D15 SCL   | SCL          |
| PB9         | CN2.2  | D14 SDA   | SDA          |

#### Mount the Battery
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
5. Cut holes for the push buttons

### Flash the Mecrisp-Cube Firmware

Flash the Mecrisp-Cube [binary](/Release/MecrispCube.bin) `MecrispCube.bin` or better the [fs-binary](/sdcard/boot/MecrispCubeFS.bin) 
`MecrispCubeFS.bin` to the WB55 Nucleo. Using the built-in USB DFU bootloader.

 1. Connect the Nucleo Board USB ST-LINK to the PC
 1. Copy binary (MecrispCube.bin or better the MecrispCubeFS.bin) to the USB mass storage NODE_WB55RG 


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
### v1.6.0 Calculator Release
* tinyUSB
* 4th Calculator


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


