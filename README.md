# pico-dcf77-clock
A (for now) very simple DCF77-based radio clock for the Raspberry Pico

### Hardware Requirements

* Obviously a Rapberry Pi Pico with RP2040,
* 1602 LCD with HD44780 driver chip and HW-061 I2C interface
* DCF77 radio clock receiver

### Wiring

The software default configuration assumes that on the Pico, GPIO pins 20 and 21 are used as SDA and SCL, respectively, for communication with the HW-061 interface of the LCD display.
The DCF77 receiver board should operate with 3.3V, drawn from Pico pin 36, such that it's output signal signal TCO will not exceed the range 0.0V to 3.3V.  The TCO signal is fed into the Pico, using pin GPIO 16, and assuming an inverse signal (i.e. with positive pulses).  Most DCF77 receivers have a power on / off pin <code>PON</code> that you may need to connect with GND to turn on the receiver.  If you have a DCF77 receiver that instead produces negative pulses, then simply change in the software the <code>inverse_tco</code> Boolean parameter that is passed into the constructor of the <code>DCF77</code> class.

For the receiver to reliably work, you should

* add a 10µF capacitor between the DCF77 receiver board's GND and VCC pin,
* optionally add another 10µF capacitor directly at the Pico's incoming power supply,
* keep, if possible, the antenna a few centimers away from the Pico and the DCF77 receiver board,
* keep the whole clock (Pico + DCF77 receiver board + LCD display + antenna) at least 1-2 meters away from other electronics such as computer monitors or notebooks.

For best results, the antenna should be aligned tangential to an imagined circle on a map around the sender's location near Frankfurt/Main.

### Software Environment

This code compiles on a Raspberry Pi 4 with Pico SDK 1.5 installed.
It should also compile on any other platforms that support developing code for the Pico.

### Compiling

Assuming a Raspberry Pi as development environment, ensure you have installed the C/C++ development environment as provided by the Raspberry foundation.  The software of this project expects the <code>pico-sdk</code> and <code>pico-examples</code> directories to be found in the parent directory of this project's root directory.  It should be ok to use symlinks that point to the installed development environment, if it is not acceptable to place this project side-by-side with the environment installation.

From within the root directory of this project, just invoke

```$ build.sh```

to compile the project.  On success, continue with deploying the compiled code to your Pico radio clock.

### Deploying

Make sure your Pico is ready to receive a new <code>.uf2</code> flash image file.  Your Pico should be visible as a mounted file system, and for now, the script assumes that this file system appears in the home directory of user <code>pi</code>, called <code>RPI-RP2</code>.  If this is not the case, just copy the <code>.uf2</code>> flash image from the <code>build</code> directory onto the Pico as described in the Pico docs.

Note that any other previously existing code will be overwritten.  If you have previously uploaded files using the Python developing environment, the complete environment on the Pico, including all uploaded files, will be overwritten when uploading the DCF77 Radio Clock flash image.

To upload and run the radio clock flash image, simply run the <code>deploy.sh</code> script from within the root directory of the project:

```$ deploy.sh```

### Debugging

If everything went ok so far, the LCD should light on, and the you should see a clock that starts counting at 00:00:00 as of January 1st, 0001, since it takes some time before a valid timestamp has been received.  If you do not see anything at all being displayed on the LCD, try adjusting the contrast trimming potentiometer on the HW-061 board mounted onto the display.

By default, the LED on the Pico will visualize impulses from the receiver.  Ideally, the LED should regularly light up every second for a short pulse of 100ms or 200ms.  Only once per minute, the impulse should be ommited, signaling that the next minute is starting.  If you do not see this regular blinking of the LED, signal quality is probably poor, and you should trying to relocate the clock or adjust the direction of the antenna, until you will observe regular blinking of the LED.

Once, the software has correctly detected all impulses of a minute (usually after at most 2 minutes, provided signal quality is sufficiently good), the display will be updated to show the correct time.  If the time does not correctly show up even after a couple of minutes, the receiver still does not receive a sufficiently clean signal.  In this case, again try to change the position of the clock and the direction of the antenna.  Remember to keep away from electronics such as computer monitors or notebooks at least 1-2 meters.

If the Pico is connected via USB to a Raspberry Pi, you also can call the script <code>screen.sh</code> on the Raspberry Pi to monitor the frequency of detected errors to get some impression of the signal quality.  For stopping and exiting this script, press CTRL+A, and then `\`.

### TODOs

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
