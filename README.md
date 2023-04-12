# pico-dcf77-clock
A (for now) very simple DCF77-based radio clock for the Raspberry Pico

### Hardware Requirements

* Obviously a Rapberry Pi Pico with RP2040,
* 1602 LCD with HD44780 driver chip and HW-061 I2C interface
* DCF77 radio clock receiver

### Wiring

The software default configuration assumes that on the Pico, GPIO pins 20 and 21 are used as SDA and SCL, respectively, for communication with the HW-061 interface of the LCD display.
The DCF77 receiver board should operate with 3.3V, drawn from Pico pin 36, such that it's output signal signal TCO will not exceed the range 0.0V to 3.3V.  The TCO signal is fed into the Pico, using pin GPIO 16, and assuming an inverse signal (i.e. with positive pulses).  If you have a DCF77 receiver that instead produces negative pulses, then simply change in the software the inverse_tco Boolean parameter that is passed into the constructor of the DCF77 class.

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

If everything went ok so far, the LCD should light on, and the you should see a clock that starts counting at 00:00:00 as of January 1st, 1970, since it takes some time before a valid timestamp has been received.  If you do not see anything at all being displayed on the LCD, try adjusting the contrast trimming potentiometer on the HW-061 board mounted onto the display.

As soon as the clock has received a complete DCF77 data record (usually after at most 2 minutes), the display will be updated to show the correct time.
If the time does not correctly show up even after a couple of minutes, the receiver does not receive a sufficiently clean signal.  In this case, try to change the position of the clock and the direction of the antenna.  Remember to keep away from electronics such as computer monitors or notebooks at least 1-2 meters.

If the Pico is connected via USB to a Raspberry Pi, you may want to call the script <code>screen.sh</code> on the Raspberry Pi to monitor the frequency of detected errors to get some impression of the signal quality.  For stopping and exiting this script, press CTRL+A, and then `\`.
