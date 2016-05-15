# ladybug
attiny85 + WS2812 replacement for CloudB ladybug nightlight.

TODO(z2amiller):
It turns out that the WS2812b LEDs (aka Neopixels) have quite a high
quiescent current - around 800uA.  This ends up eating batteries
moderately quickly.  I should add a transistor to switch the LED on
only when the attiny85 is awake.

This may be moderately complicated, since I'm out of IO pins on the
attiny85 without trying to use the reset pin as IO.
