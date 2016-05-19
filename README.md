# ladybug
attiny85 + WS2812 replacement for CloudB ladybug nightlight.

## Installation
Album showing disassembly [here](http://imgur.com/a/Ccf88).

The ladybug has six screws that hold the shell on.  They're
under the fabric.  I had to cut a small slit in the fabric to
access some of them, others I was able to get at through
the hole for the battery pack.

## Programming

The ladybug can be programmed in-system with an 8 pin SOIC clip
like [this](http://www.amazon.com/dp/B015W4PKR6).  I have used
a [small programming shield](http://www.electrodragon.com/product/attiny-1385-programming-shield/)
plus an Arduino UNO as the programmer.   You could also make
a custom cable to hook onto a dedicated programmer like the
[USBASP](http://www.electrodragon.com/product/8051-avr-mcu-programmer-usbasp/)
programmer.

## Future work

It turns out that the WS2812b LEDs (aka Neopixels) have quite a high
quiescent current - around 800uA.  This ends up eating batteries
moderately quickly.

One idea would be to connect the wakeup button to a
[soft latching circuit](http://electronics.stackexchange.com/questions/4060/make-a-momentary-switch-control-a-toggle)
and use the output of the toggle control the enable pin of a power
regulator end connect the current WAKE pin on the attiny85 as a
second momentary input to turn it off after 30 minutes.

For example, that circuit controlling the enable pin of a 5V boost
regulator would make the following improvements:

* Vastly reduced quiescent current, probably under 20ua.  Only the
  latch would be consuming measureable power.
* Simplified programming, less need for fancy interrupt and sleep
  routines - at the end of the cycle, the attiny85 would just
  poke the toggle and turn the power off.
* Better performance with low batteries.  Right now, as the batteries
  decay, the ladybug gets wonky.  The blue LED has a higher forward
  voltage, so as the batteries start to run down, the blue LED inside
  the WS2818b will no longer light up, though the green and red channels
  continue to work.
* Probably a little bit brighter running at full 5v instead of the 4.5v
  or less coming from 3xAAA batteries.

There is plenty of room on the board for a boost regulator and toggle circuit.
