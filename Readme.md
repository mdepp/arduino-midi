## About
This project streams 8-bit 8hkz sound to an Arduino through its serial connection. A double buffer and a hardware timer are used to co-ordinate playing and receiving sound data. The *music_player* directory contains the Python script used to stream data and receiver code that runs on the Arduino. The *timer_example* directory contains a simple example I made while learning about AVR timers.

## Wiring Instructions
Ports 22 through 29 are connected to an 8-bit R-2R resistor ladder for digital-to-analog conversion (R=280 Ohm) with pin 22 as the least-significant bit. The output of this is connected to ground through a speaker. A 20uF capacitor is placed parallel to the speaker to filter out an annoying high-pitched buzzing noise that seems to result from the resistor ladder, although this makes the speaker quiet enough that it is best replaced with some kind of headphone mount.

## Building
The sender script is in Python 3 and requires no additional modules beyond the standard libraries. All programs that run on the Arduino were compiled with the Arduino extension for Visual Studio Code, although something like [this Makefile](https://github.com/sudar/Arduino-Makefile) ought to work as well.
