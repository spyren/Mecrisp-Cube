![](img/mecrisp-cube-4th-logo-red-h.svg)

# Displays

I really like the neat little OLED displays. They are crisp and draw only some miliamps. 
Cheap OLED display have a resolution 128x32 that's enough for about 4 lines and 20 characters.
The maximum resolution seems to be at about 128x128.

The little displays use serial interfaces like I2C and SPI. Usually the display RAM is write only.
Therefore you need a frame buffer in the MCU RAM.

There are other displays with different technologies:
  * LCD Liquid Crystall Display (e.g. Flipper Zero)
  * EPD Electronic Paper Display
  * MIP
  * QUAD alphanumeric display (LED segment display)

Driver [oled.c](/peripherals/oled.c), fonts from https://www.mikrocontroller.net/topic/54860.

[Code page 850](https://en.wikipedia.org/wiki/Code_page_850)

# OLED Words

`oled-emit` works like the standard word `emit`. It blocks the calling thread, 
as long as the character is not written to the OLED display (less than 300 us 
for a 6x8 character and 400 kHz I2C). Horizontal (x) position is in pixel (0 to 127), 
vertical position (y) is in lines, a line consists of 8 pixels. 0, 0 is upper 
left corner. Larger fonts takes more than one line.

<pre>
oled-emit    ( c -- )           Emits a character (writes a character to the OLED display)
oled-emit?   ( -- f )           OLED ready to get a character (I2C not busy)

hook-emit    ( -- a-addr )      Hooks for redirecting terminal IO on the fly
hook-emit?   ( -- a-addr )    

oledpos!     ( x y -- )         Set OLED cursor position, 
                                x (column) horizontal position, max. 127  
                                y (row) vertical position (a line consists of 8 pixels), max. 3 for 128x32 or 7 for 128x64 displays.
oledpos@     (  -- x y )        Get the current OLED cursor position
oledcmd      ( c-addr -- )      Send command to the OLED controller SSD1306. First byte contains the length of the command.
oledclr      (  --  )           Clears the OLED display, sets the cursor to 0, 0
oledfont     ( u --  )          Select the font, u: 0 6x8, 1 8x8, 2 8X16 , 3 12X16
oledcolumn!  ( u -- )           Write a column (8 pixels) to the current position. Increment position. Bit 0 on top
oledcolumn@  ( -- u )           Read a column (8 pixels) from the current position

>oled        ( -- a1 a2 )       redirect to oled *)
>epd         ( -- a1 a2 )       redirect to epd *)
>lcd         ( -- a1 a2 )       redirect to lcd *)
>term        ( a1 a2 -- )       terminate redirection *)

*) part of redirection.fs
</pre>


# Usage

It is easy to redirect the terminal output to the OLED display, to use the string formatting words.
```forth
: oled-hallo (  -- )
  hook-emit @              \ save emit hook
  ['] oled-emit hook-emit ! \ redirect terminal to oled-emit
  ." Hallo Velo! " cr
  ." ciao"
  hook-emit !         \ restore old hook
;
```

or even simpler
```forth
: oled-hallo (  -- )
  >oled         \ redirect terminal to oled-emit
  ." Hallo Velo! " cr
  ." ciao"
  >term         \ terminate redirection
;
```

or on a command line
<pre>
>oled .( Hallo Velo!) >term
</pre>

show date and time on the Flipper LCD (see [How to Use Tasks](CmsisRtos.md#how-to-use-tasks) for a background task).
```forth
: clock (  -- )
  lcdclr
  3 lcdfont
  >lcd
    -1 -1 -1 alarm!  \ set an alarm every second
    begin
      wait-alarm     \ wait a second
      0 0 lcdpos!
      .time 
    key? until
    key drop
  >term
;
```

To change the display type you have to edit the 
[oled.h](/peripherals/oled.c) and rebuild the project:
<pre>
#define OLED_4PIN_128X32	  0		// the cheap one with 4 pins, Adafruit Raspberry Pi #3527
#define	OLED_BONNET_128X64	1		// Adafruit Bonnet for Raspberry Pi #3531
#define OLED_FEATHER_128X32	2		// Adafruit FeatherWing #2900
#define OLED_FEATHER_128X64	3		// Adafruit FeatherWing #4650
#define OLED_OCTOPUS_128X64	4		// Octopus PIS-1277

#define OLED_LANDSCAPE		0
#define	OLED_PORTRAIT		1

// configure your OLED type
#define OLED_DISPLAY_TYPE	<b>OLED_OCTOPUS_128X64</b>
#define	OLED_ORIENTATION	<b>OLED_LANDSCAPE</b>
</pre>

# Links

## SSD1306 Driver (OLED)

### Driver SW
   * U8g2 https://github.com/olikraus/
   * STM32 Graphical User Interface TOUCHGFX https://www.st.com/content/st_com/en/stm32-graphic-user-interface.html
   * https://lexus2k.github.io/ssd1306/ https://github.com/lexus2k/ssd1306
   * https://github.com/bitbank2/ss_oled
   * SSD1306xLED https://git.x2software.net/pub/GameCounter/-/tree/master/Source/lib/ssd1306xled

### Other OLEDs with SSD1306 
   * OLED B click 96x39 mono https://www.mikroe.com/oled-b-click, SSD1306
      * I2C address 60 / 0x3c or SPI
      * Manual [[https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf][SSD1306]]
      * https://learn.adafruit.com/adafruit-pioled-128x32-mini-oled-for-raspberry-pi
      * Manual [[https://download.mikroe.com/documents/datasheets/ssd1351-revision-1.3.pdf][SSD1351]]
         * Idles at low CPOL=0  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW
         * CPHA=0 hspi1.Init.CLKPhase = SPI_PHASE_1EDGE

   * Adafruit 128x64 Bonnet 
      * https://learn.adafruit.com/adafruit-128x64-oled-bonnet-for-raspberry-pi/overview
      * https://pinout.xyz/pinout/i2c# 

   * Adafruit FeatherWing 128x64 OLED
      * https://learn.adafruit.com/adafruit-128x64-oled-featherwing

   * Adafruit FeatherWing 128x32 OLED
      * https://www.adafruit.com/product/2900
      * Schematics https://learn.adafruit.com/assets/71319
      * D/C pin is not available -> data read is not possible

   * https://www.lcd-module.de/fileadmin/pdf/grafik/oledm128-6.pdf

   * Octopus
      * https://www.distrelec.ch/de/octopus-128x64-oled-display-pi-supply-pis-1277/p/30163414 
      ![](img/nucleo-feather-adaptor.jpg)

## SH1107 Driver (OLED)

Slave address bit (SA0), D/C pin acts as SA0. 

   * [[https://cdn-learn.adafruit.com/assets/assets/000/094/580/original/SH1107_datasheet.pdf][SH1107 Datasheet]]
   * https://www.displayfuture.com/Display/datasheet/controller/SH1107.pdf
   * https://github.com/adafruit/Adafruit_CircuitPython_DisplayIO_SH1107/blob/main/adafruit_displayio_sh1107.py

FeatherWing 128x64 OLED
   * https://www.adafruit.com/product/4650
   * Schematics https://learn.adafruit.com/assets/94578
   * I2C address selector D11 (JP3.6). 0 command, 1 data
   
