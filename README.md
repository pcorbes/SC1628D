SC1628D
=======
Arduino library for SC1628D (LED Driver)


Description
-----------
An Arduino library for 7-digit 11-segments, or 6-digit 12-segments display modules, based on the SC1628D chip.
The SC1628D chip also has keyboard input capability, but it's not implemented in this library.

Hardware Connection
-------------------
The display modules has three signal connection (and two power connections) which are STB, CLK and DIO. These pins can be connected to any digital pins on the Arduino. When an object is created, the pins should be configured. There is no limitation on the number of instances used concurrently (as long as each instance has a pin pair of its own)

Installation
------------
The library is installed as any Arduino library, by copying the files into a directory on the library search path of the Arduino IDE

Usage
-----
The library provides a single class named SC1628D. An instance of this class provides the following functions:

* `setSegments` - Set the raw value of the segments of each digit
* `showNumberDec` - Display a decimal number
* `showNumberDecEx` - Display a decimal number with decimal points or colon
* `setBrightness` - Sets the brightness of the display

The information given above is only a summary. Please refer to SC1628D.h for more information. An example is included, demonstrating the operation of most of the functions.
