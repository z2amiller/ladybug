#include <LadybugActions.h>
#include <Bounce2.h>
#include <Adafruit_NeoPixel.h>
#include <avr/sleep.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

enum color_mode {
  MODE_RAINBOW,
  MODE_BREATHE,
  MODE_RANDOM
};

#define LED_PIN 0
#define RANDOM_PIN 4
#define WAKEUP_PIN 3
#define RAINBOW_PIN 2
#define BREATHE_PIN 1

Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, LED_PIN, NEO_GRB + NEO_KHZ800);
const long on_time = 35 * 60 * 1000L;
volatile long last_stop = 0L;
color_mode mode = MODE_RAINBOW;

Bounce rainbow_switch = Bounce();
Bounce breathe_switch = Bounce();
Bounce random_switch = Bounce();

Rainbow rainbow = Rainbow(&strip, 5000);
Breather breathe = Breather(&strip, 30);
FadeCycler randomizer = FadeCycler(&strip, 5000, 10);

ISR(PCINT0_vect) {
  last_stop = millis();
}

void powerDown () {
  cbi(ADCSRA, ADEN); // Turn off ADC
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_bod_disable();
  sleep_mode();
  sbi(ADCSRA, ADEN); // Turn on ADC
}

void shutDown() {
  strip.setPixelColor(0, 0);
  strip.show();
  sbi(GIMSK, PCIE);  // Turn on Pin Change interrupt
  sbi(PCMSK, PCINT3); // Which pins are affected by the interru                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   pt
  powerDown();
  cbi(GIMSK, PCIE);  // Turn on Pin Change interrupt
  cbi(PCMSK, PCINT3); // Which pins are affected by the interrupt
}

void setup() {
  pinMode(WAKEUP_PIN, INPUT_PULLUP);
  pinMode(RAINBOW_PIN, INPUT_PULLUP);
  pinMode(BREATHE_PIN, INPUT_PULLUP);
  pinMode(RANDOM_PIN, INPUT_PULLUP);
  rainbow_switch.attach(RAINBOW_PIN);
  rainbow_switch.interval(75);
  breathe_switch.attach(BREATHE_PIN);
  breathe_switch.interval(75);
  random_switch.attach(RANDOM_PIN);
  random_switch.interval(75);

  strip.begin();
  strip.setBrightness(255);
  sei();             // Enable interrupts.
  shutDown();        // Start in powered-off state, ready to be woken by power button.
}

void loop() {
  if (last_stop + on_time <= millis()) {
    shutDown();
  }
  rainbow_switch.update();
  breathe_switch.update();
  random_switch.update();
  // Debouncing this is a little more complicated because it is also the wakeup pin.
  if (digitalRead(WAKEUP_PIN) == LOW) {
    if (last_stop + 1000 <= millis()) {
      while (digitalRead(WAKEUP_PIN) == LOW)
        delay(25);
      shutDown();
    }
  }
  if (rainbow_switch.read() == LOW) {
    last_stop = millis();
    mode = MODE_RAINBOW;
  }
  if (breathe_switch.read() == LOW) {
    breathe.cycle();
    last_stop = millis();
    mode = MODE_BREATHE;
  }
  if (random_switch.read() == LOW) {
    randomSeed(micros());
    last_stop = millis();
    mode = MODE_RANDOM;
  }
  switch (mode) {
    case MODE_RAINBOW:
      rainbow.update();
      break;
    case MODE_BREATHE:
      breathe.update();
      break;
    case MODE_RANDOM:
      randomizer.update();
      break;
  }

}



