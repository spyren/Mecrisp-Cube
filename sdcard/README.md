# Pocket Power Pack (Mecrisp-Cube ppp branch)

Model railroading on the go. A battery operated power pack gives you the opportunity to take your model railroad with you. 
No mess with outlets and cables. Build your Pocket Power Pack, get some sectional track, a locomotive (DCC or DC) and a few cars and you are in business.

From the DCC point of view the Pocket Power Pack combines _Throttle_, _Command Station_, and _Booster_ in the same hand held unit. 
No power supply is required as long as the battery lasts. You can recharge the battery with a standard smart phone charger.

### General
 - Max. current 0.5 A (limited by the DCDC-converter Purecrea MT3608)
 - Short circuit proof
 - Adjustable rail voltage between 5 V and 20 V
 - With 1000 mA LiPo battery
   - Operating time
     - Small HOe or N-scale locomotive (50 mA): 5 h 
     - Small HO locomotive (100 mA): 2.5 h
     - Medium HO locomotive (250 mA): 1 h
   - Charging time about 2 h (micro USB charger, 500 mA)
 - Display shows
   - PWM duty cycle 0 .. 100 % (DC) or speed 0 .. 126 (DCC)
   - Direction
   - Current consumption
   - Rail voltage
   - Battery voltage
   - Menu

### DC-Mode
 - PWM frequencies: 250 Hz, 500 Hz, 1 kHz, 2 kHz, 4 kHz, 8 kHz, and 16 kHz
 - Brake mode (fast/slow [decay mode](https://learn.adafruit.com/improve-brushed-dc-motor-performance/current-decay-mode))

### DCC-Mode
 - 4 locomotive slots (address range 0 .. 9999), similar to the DCC-EX reminders
 - Speed 0 .. 126
 - Direction
 - Light (F0), bell (F1), airhorn (F2), mute (F8)
 - 8 programmable functions (out of F0 .. F28)
   
### Bluetooth Remote Control (Throttle)
  - Use your smart phone as a additional remote throttle
  - [DCC-EX commands](https://dcc-ex.com/reference/software/command-summary-consolidated.html) over BLE
  - [JMRI Engine Driver Throttle](https://enginedriver.mstevetodd.com/)

### DCC-EX serial (USB CDC)
  - [DCC-EX commands](https://dcc-ex.com/reference/software/command-summary-consolidated.html) over serial line (USB CDC)


## How to Use the Pocket Power Pack
It could not be simpler. Connect the power to the rails and turn the knob. 
Change direction with the arrow buttons. 
Choose the menu line with the M button, select the option with the button A, B, C, or D.

## Authors

* **Peter Schmid** - *Initial work* - [Mecrisp Cube](https://spyr.ch/twiki/bin/view/MecrispCube)
* **Matthias Koch** - *Forth on ARM Cortex M0/M4* - [Mecrisp Stellaris](http://mecrisp.sourceforge.net/)


## License

This project *Pocket Power Pack* is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

*Pocket Power Pack* is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with *Pocket Power Pack*. If not, see http://www.gnu.org/licenses/.


## Acknowledgments

* http://mecrisp.sourceforge.net/ Original Mecrisp project, **Matthias Koch**
* https://mecrisp-stellaris-folkdoc.sourceforge.io/ Mecrisp Stellaris Unofficial UserDoc, **Terry Porter**


## Documentation

https://github.com/spyren/Mecrisp-Cube/tree/ppp
