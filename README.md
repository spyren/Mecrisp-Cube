# Mecrisp-Cube

Mecrisp-Stellaris Forth for the STM32 Cube ecosystem. 
The STM32WB Nucleo Board is working out of the box yet. 

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
 
Flash the [binary](Release/minimal.bin) to the Nucleo Board.

1. Connect the Nucleo Board USB ST-LINK to the PC
2. Copy [binary](Release/minimal.bin) (`minimal.bin`) to the USB mass 
   storage NODE_WB55RG

Start the terminal emulator application on the PC. 
Check for the serial communication port (e.g. for Linux `/dev/ttyACM0`) 
and set the speed to 115200 baud. 
I set the putty terminal configuration to 

  * Implicit CR in every LF 
  * Local echo: Auto
  * Local line editing: Auto
  
```forth
Mecrisp-Stellaris 2.5.2 for STM32WB55 by Matthias Koch
23 5 / . 4<RETURN>  ok.
: hello ." World" ;<RETURN>  ok.
hello<RETURN> World ok.
```

### Installing

A step by step series of examples that tell you how to get a development env running

Say what the step will be

```
Give the example
```

And repeat

```
until finished
```

End with an example of getting some data out of the system or using it for a little demo


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


## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/your/project/tags). 

## Authors

* **Matthias Koch** - *Forth on ARM Cortex M0/M4* - [Mecrisp Stellaris](http://mecrisp.sourceforge.net/)
* **Peter Schmid** - *Initial work* - [Mecrisp Cube](http://spyr.ch/twiki/bin/view/Cosmac/ForthSTM32WB)

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

* Hat tip to anyone whose code was used
* Inspiration
* etc

