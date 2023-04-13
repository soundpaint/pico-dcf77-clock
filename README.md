# pico-dcf77-clock

A (for now) very simple DCF77-based radio clock for the Raspberry Pico.

Note: There exist already various similar projects for other microcontrollers like Arduino (see below [bibliography](#bibliography)).  This project however specifically focuses on the Raspberry Pico as microcontroller for building a DCF77 radio clock, and using code written in C/C++ rather than in MicroPython.  The software has been written from scratch rather than just porting an existing Arduino library in order to not take over unneccessary pecularities and legacy code from the Arduino world.

## Hardware Requirements

* Obviously a Rapberry Pi Pico with RP2040,
* 1602 LCD with HD44780 driver chip and HW-061 I2C interface
* DCF77 radio clock receiver

## Wiring

The software default configuration assumes that on the Pico, GPIO pins 20 and 21 are used as SDA and SCL, respectively, for communication with the HW-061 interface of the LCD display.
The DCF77 receiver board should operate with 3.3V, drawn from Pico pin 36, such that it's output signal signal TCO will not exceed the range 0.0V to 3.3V.  The TCO signal is fed into the Pico, using pin GPIO 16, and assuming an inverse signal (i.e. with positive pulses).  Most DCF77 receivers have a power on / off pin <code>PON</code> that you may need to connect with GND to turn on the receiver.  If you have a DCF77 receiver that instead produces negative pulses, then simply change in the software the <code>inverse_tco</code> Boolean parameter that is passed into the constructor of the <code>DCF77</code> class.

For the receiver to reliably work, you should

* add a 10µF capacitor between the DCF77 receiver board's GND and VCC pin,
* optionally add another 10µF capacitor directly at the Pico's incoming power supply,
* keep, if possible, the antenna a few centimers away from the Pico and the DCF77 receiver board,
* keep the whole clock (Pico + DCF77 receiver board + LCD display + antenna) at least 1-2 meters away from other electronics such as computer monitors or notebooks.

For best results, the antenna should be aligned tangential to an imagined circle on a map around the sender's location in Mainflingen near Frankfurt/Main.

## Software Environment

This code compiles on a Raspberry Pi 4 with Pico SDK 1.5 installed.
It should also compile on any other platforms that support developing code for the Pico.

## Compiling

Assuming a Raspberry Pi as development environment, ensure you have installed the C/C++ development environment as provided by the Raspberry foundation.  The software of this project expects the <code>pico-sdk</code> and <code>pico-examples</code> directories to be found in the parent directory of this project's root directory.  It should be ok to use symlinks that point to the installed development environment, if it is not acceptable to place this project side-by-side with the environment installation.

From within the root directory of this project, just invoke

```$ build.sh```

to compile the project.  On success, continue with deploying the compiled code to your Pico radio clock.

## Deploying

Make sure your Pico is ready to receive a new <code>.uf2</code> flash image file.  Your Pico should be visible as a mounted file system, and for now, the script assumes that this file system appears in the home directory of user <code>pi</code>, called <code>RPI-RP2</code>.  If this is not the case, just copy the <code>.uf2</code>> flash image from the <code>build</code> directory onto the Pico as described in the Pico docs.

Note that any other previously existing code will be overwritten.  If you have previously uploaded files using the Python developing environment, the complete environment on the Pico, including all uploaded files, will be overwritten when uploading the DCF77 Radio Clock flash image.

To upload and run the radio clock flash image, simply run the <code>deploy.sh</code> script from within the root directory of the project:

```$ deploy.sh```

## Debugging

If everything went ok so far, the LCD should light on, and the you should see a clock that starts counting at 00:00:00 as of January 1st, 0001, since it takes some time before a valid timestamp has been received.  If you do not see anything at all being displayed on the LCD, try adjusting the contrast trimming potentiometer on the HW-061 board mounted onto the display.

By default, the LED on the Pico will visualize impulses from the receiver.  Ideally, the LED should regularly light up every second for a short pulse of 100ms or 200ms.  Only once per minute, the impulse should be ommited, signaling that the next minute is starting.  If you do not see this regular blinking of the LED, signal quality is probably poor, and you should trying to relocate the clock or adjust the direction of the antenna, until you will observe regular blinking of the LED.

Once, the software has correctly detected all impulses of a minute (usually after at most 2 minutes, provided signal quality is sufficiently good), the display will be updated to show the correct time.  If the time does not correctly show up even after a couple of minutes, the receiver still does not receive a sufficiently clean signal.  In this case, again try to change the position of the clock and the direction of the antenna.  Remember to keep away from electronics such as computer monitors or notebooks at least 1-2 meters.

If the Pico is connected via USB to a Raspberry Pi, you also can call the script <code>screen.sh</code> on the Raspberry Pi to monitor the frequency of detected errors to get some impression of the signal quality.  For stopping and exiting this script, press CTRL+A, and then `\`.

## TODOs

A lot of things are to be done, among them at least:

* Support I18N.  Currently, for date and time display, always German conventions are used.
* Improve robustness / fault tolerance.  Currently, even if the signal from the receiver is fairly good, but only a little bit degraded, the software will still fail extracting the current time.  In particular,
** the software should not require all 59 bits per minute to be correctly received, but should be content with those bits strictly necessary to extract the time and date (or even support partial update of only the time or only the date);
** the software should be able to detect the overall time grid of pulses by averaging the start positions of detected and as correct assumed pulses relative to the grid;
** based on this grid, the software should be able to detect and ignore sporadic short impulses that occur outside of the regular pattern of one pulse per second;
** maybe visualize estimated DCF77 signal quality, based on the grid analysis and number of detected errors vs. recognized bits.
* Maybe receive and update the time only now and then, in case battery power is critical (though, power consumption of the receiver is neglectible as compared to powering the pulsing LED and LCD display).
* Maybe visualize summer time announcement (bit 16 of the DCF77 time code).
* Maybe visualize leap second announcement (bit 19 of the DCF77 time code).
* Support display of additional information, such as civil warning bits.
* Check and maybe correct behavior for leap seconds and leap years, as needed.
* Add features for individual configuration and customization, including configuration of all of the before mentioned features.

## Bibliography

### DCF77 Manuals &amp; Specifications

[1] Wikipedia: _DCF77_.
URL: <a href="https://en.wikipedia.org/wiki/DCF77" target="_blank">https://en.wikipedia.org/wiki/DCF77</a> (accessed on 2023-04-13)

[2] Wikipedia: _DCF77_.
URL: <a href="https://de.wikipedia.org/wiki/DCF77" target="_blank">https://de.wikipedia.org/wiki/DCF77</a> (accessed on 2023-04-13)

[3] Physikalisch-Technische Bundesanstalt: _DCF77 time code_.
URL: <a href="https://www.ptb.de/cms/en/ptb/fachabteilungen/abt4/fb-44/ag-442/dissemination-of-legal-time/dcf77/dcf77-time-code.html" target="_blank">https://www.ptb.de/cms/en/ptb/fachabteilungen/abt4/fb-44/ag-442/dissemination-of-legal-time/dcf77/dcf77-time-code.html</a> (accessed on 2023-04-13)

[4] Meteo Time GmbH: _Wetterdatenbeschreibung des Systems Meteotime_.
URL: <a href="https://web.archive.org/web/20091229063350/http://www.hkw-elektronik.de/pdf/DB%20W-Protokoll-V%201.pdf" target="_blank">https://web.archive.org/web/20091229063350/http://www.hkw-elektronik.de/pdf/DB%20W-Protokoll-V%201.pdf</a> (accessed on 2023-04-13)

### Related Applications and DCF Libraries

[5] Wolfgang Ewald: _DCF77 - Radio Controlled Clock_.
URL: <a href="https://wolles-elektronikkiste.de/en/dcf77-radio-controlled-clock" target="_blank">https://wolles-elektronikkiste.de/en/dcf77-radio-controlled-clock</a> (accessed on 2023-04-13)

[6] RoboterNETZ: _DCF77-Decoder als Bascom-Library_.
URL: <a href="https://rn-wissen.de/wiki/index.php/DCF77-Decoder_als_Bascom-Library" target="_blank">https://rn-wissen.de/wiki/index.php/DCF77-Decoder_als_Bascom-Library</a> (accessed on 2023-04-13)

[7] Udo Klein: _Noise Resiliant DCF77 Decoder Library for Arduino_.
URL: <a href="https://github.com/udoklein/dcf77" target="_blank">https://github.com/udoklein/dcf77</a> (accessed on 2023-04-13)

[8] GitHub User <em>veebch</em>: _Running an Old Secondary Clock, Using DCF77 or WWVB and Raspberry Pi Pico_.
URL: <a href="https://github.com/veebch/clock" target="_blank">https://github.com/veebch/clock</a> (accessed on 2023-04-13)

[9] GitHub User <em>demogorgi</em>: _Micropython Script to Set Real-Time Clock (RTC) Using DCF77 receiver Module DCF1_.
URL: <a href="https://github.com/demogorgi/set-rtc-using-dcf77-via-dcf1" target="_blank">https://github.com/demogorgi/set-rtc-using-dcf77-via-dcf1</a> (accessed on 2023-04-13)

### HD44780 / HW-061 LCD Display with I2C Interface Manuals &amp; Specifications

[10] Wikipedia: _Hitachi HD44780 LCD Controller_.
URL: <a href="https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller" target="_blank">https://en.wikipedia.org/wiki/Hitachi_HD44780_LCD_controller</a> (accessed on 2023-04-13)

[11] Hitachi: _HD780U (LCD-II)_.
URL: <a href="https://www.sparkfun.com/datasheets/LCD/HD44780.pdf" target="_blank">https://www.sparkfun.com/datasheets/LCD/HD44780.pdf</a>,
<a href="https://cdn-shop.adafruit.com/datasheets/HD44780.pdf" target="_blank">https://cdn-shop.adafruit.com/datasheets/HD44780.pdf</a> (accessed on 2023-04-13)

[12] Texas Instruments: _PCF8574 Remote 8-Bit I/O Expander for I2C Bus Datasheet (Rev. J)_.
URL: <a href="https://www.ti.com/lit/ds/symlink/pcf8574.pdf" target="_blank">https://www.ti.com/lit/ds/symlink/pcf8574.pdf</a> (accessed on 2023-04-13)

[13] NXP Semiconductors: _PCF8574; PCF8574A Remote 8-Bit I/O Expander for I2C-Bus with Interrupt_.
URL: <a href="https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf" target="_blank">https://www.nxp.com/docs/en/data-sheet/PCF8574_PCF8574A.pdf</a> (accessed on 2023-04-13)

[14] Handson Technology: _I2C Serial Interface 1602 LCD Module_.
URL: <a href="https://www.handsontec.com/dataspecs/module/I2C_1602_LCD.pdf" target="_blank">https://www.handsontec.com/dataspecs/module/I2C_1602_LCD.pdf</a> (accessed on 2023-04-13)

[15] GitHub User <em>duinoWitchery</em>: _Extensible HD44780 LCD Library_.
URL: <a href="https://github.com/duinoWitchery/hd44780" target="_blank">https://github.com/duinoWitchery/hd44780</a> (accessed on 2023-04-13)

[16] GitHub User <em>Testato</em>: _PCF8574+HD44780 LCD I2C Arduino Library_.
URL: <a href="https://github.com/Testato/PCF8574_HD44780_I2C" target="_blank">https://github.com/Testato/PCF8574_HD44780_I2C</a> (accessed on 2023-04-13)

[17] Frank de Brabander: _Library for the LiquidCrystal LCD Display Connected to an Arduino Board_.
URL: <a href="https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library" target="_blank">https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library</a> (accessed on 2023-04-13)

[18] John Rickman: _LiquidCrystal Arduino Library for the DFRobot I2C LCD Displays_.
URL: <a href="https://github.com/johnrickman/LiquidCrystal_I2C" target="_blank">https://github.com/johnrickman/LiquidCrystal_I2C</a> (accessed on 2023-04-13)

### Hardware Resources

Note: The below hyperlinks point to example resources for the components used in this project.  I am in no way affiliated with the resource providers, nor is this list of links in any sense representative or complete.  These links are no sponsored links; I am just providing these links for convenience of the reader as example resources as is; the links may be become outdated or otherwise invalid at any time without prior notice.

[19] Pollin Electronic.  _DCF 77 Empfangsmodul_.
URL: <a href="https://www.pollin.de/p/dcf-77-empfangsmodul-dcf1-810054" target="_blank">https://www.pollin.de/p/dcf-77-empfangsmodul-dcf1-810054</a> (accessed on 2023-04-13)

[20] Berrybase Maker Shop.  _Alphanumerisches LCD 16x2, blau / weiß, mit I2C Backpack._
URL: <a href="https://www.berrybase.de/alphanumerisches-lcd-16x2-blau/weiss-mit-i2c-backpack" target="_blank">https://www.berrybase.de/alphanumerisches-lcd-16x2-blau/weiss-mit-i2c-backpack</a> (accessed on 2023-04-13)

[21] Berrybase Maker Shop.  _Alphanumerisches LCD 16x2, blau._
URL: <a href="https://www.berrybase.de/alphanumerisches-lcd-16x2-blau/weiss" target="_blank">https://www.berrybase.de/alphanumerisches-lcd-16x2-blau/weiss</a> (accessed on 2023-04-13)

[22] Berrybase Maker Shop.  _IIC / I2C Interface für 1602 / 2004 Displays._
URL: <a href="https://www.berrybase.de/iic/i2c-interface-fuer-1602/2004-displays" target="_blank">https://www.berrybase.de/iic/i2c-interface-fuer-1602/2004-displays</a> (accessed on 2023-04-13)
