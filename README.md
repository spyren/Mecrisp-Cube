# Mecrisp-Cube

Mecrisp-Stellaris Forth for the STM32 Cube ecosystem. 
The STM32WB Nucleo Board is working out of the box yet. See also [Mecrisp Cube](https://spyr.ch/twiki/bin/view/MecrispCube).

## Getting Started

These instructions will get you a copy of the project up and running on your local 
machine (STM32 Nucleo board) for development and testing purposes. 

### Prerequisites

* [STM32WB Nucleo Board](https://www.st.com/en/evaluation-tools/p-nucleo-wb55.html) - 
  The highly affordable STM32 Nucleo boards allow anyone to try out 
  new ideas and to quickly create prototypes with any STM32 MCU. 
  The STM32 Nucleo boards integrate an ST-Link debugger/programmer, 
  so there is no need for a separate probe.
* Terminal emulator application for PC, e.g.: 
  * [PuTTY](http://www.putty.org/) - Windows and Linux
  * [Tera Term](http://en.sourceforge.jp/projects/ttssh2/) - Windows
  * [Realterm](http://realterm.sourceforge.net/) - Windows
  * minicom, microcom, screen - Linux
  * Use the built in Eclipse console (but no LF)
 
Flash the Mecrisp-Cube [binary](Release/MecrispCube.bin) to the Nucleo Board.

1. Connect the Nucleo Board USB ST-LINK to the PC
2. Copy [binary](Release/MecrispCube.bin) (`MecrispCube.bin`) to the USB mass 
   storage NODE_WB55RG

Start the terminal emulator application on the PC. 
Check for the serial communication port (e.g. for Linux `/dev/ttyACM0`) 
and set the speed to 115200 baud. 
I set the putty terminal configuration to 

  * Implicit CR in every LF 
  * Local echo: Auto
  * Local line editing: Auto
  
```
Mecrisp-Stellaris RA 2.5.4 by Matthias Koch. 

Mecrisp-Cube 1.4.1 for STM32WB55, 63/128 KiB RAM/FLASH dictionary (C) 2020 peter@spyr.ch
  * Firmware Package STM32Cube FW_WB V1.10.0, USB-CDC, BLE Stack 5.0 (C) 2020 STMicroelectronics 
  * CMSIS-RTOS V2 FreeRTOS wrapper, FreeRTOS Kernel V10.2.1 (C) 2017 Amazon.com
  * FatFs for internal flash and microSD - Generic FAT fs module  R0.12c (C) 2017 ChaN
  * tiny vi - part of BusyBox (C) 2000, 2001 Sterling Huxley
23 5 / .[CR] 4  ok.
: hello ." World" ;[CR]  ok.
hello[CR] World ok.
```

### Installing

A step by step series of examples that tell you how to get a development env running

Install the IDE [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html), 
it is Eclipse and GCC based. 
[STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html)
is included in the IDE, you need a stand alone installation only if 
you do not want to use the STM32CubeIDE.  

Get the sources from github:

```
psi@homer:~> git clone https://github.com/spyren/Mecrisp-Cube
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
### v1.4.1 
* Mecrisp-Stellaris RA 
* Flash Drive
* OLED driver, I2C, timer, RTC
* Markdown docs

## Authors

* **Matthias Koch** - *Forth on ARM Cortex M0/M4* - [Mecrisp Stellaris](http://mecrisp.sourceforge.net/)
* **Peter Schmid** - *Initial work* - [Mecrisp Cube](https://spyr.ch/twiki/bin/view/MecrispCube)

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


