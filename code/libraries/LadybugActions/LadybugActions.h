#include <Adafruit_NeoPixel.h>
#ifndef LADYBUG_ACTIONS_H
#define LADYBUG_ACTIONS_H


class Color {
 public:
  Color(uint8_t red, uint8_t green, uint8_t blue);
  Color(const Color& other);
  uint32_t AsNeoPixelColor();
  void Scale(const uint8_t scale);
  uint8_t r, g, b;
};

const Color RainbowColors[] = {
  Color(255, 0, 0),    // RED
  Color(255, 125, 0),  // ORANGE
  Color(255, 255, 0),  // YELLOW
  Color(0, 255, 0),    // GREEN
  Color(0, 0, 255),    // FOLLOWED BY BLUE
  Color(125, 0, 255),  // AND VIOLET TOO
};

const uint8_t RainbowColorLength = 6;

class IntermediateColor {
 public:
  IntermediateColor(int red, int green, int blue);
  IntermediateColor(const Color& color);
  Color AsColor();
  int r, g, b;
  void Subtract(const IntermediateColor& other);
  void Add(const IntermediateColor& other);
  void Scale(const uint8_t scale);
};

class Fader {
 public:
  Fader(const uint8_t steps, const Color& start);
  Color Step();
  bool Done();
  void Next(const Color& color);
  void SetColor(const Color& color) {
    current_color_ = IntermediateColor(color);
  }
 protected:
  const uint8_t num_steps_;
  uint8_t current_step_;
  IntermediateColor current_color_;
  IntermediateColor step_increment_;
};

class Breather {
 public:
  Breather(Adafruit_NeoPixel* strip, int delay);
  void update();
  void cycle();
 protected:
  static Color DarkColor(const uint8_t idx);
  Fader fader_;
  const uint8_t breathe_delay_;
  Adafruit_NeoPixel* strip_;
  bool ascending_;
  uint8_t cycle_position_;
  long last_breathe_time_;
  long last_cycle_time_;
};

class FadeCycler {
 public:
  FadeCycler(Adafruit_NeoPixel* strip,
             const int hold_delay,
             const int update_delay);
  virtual void update();
 protected:
  virtual Color NextColor();
  const int hold_delay_;
  const int update_delay_;
  Fader fader_;
  Adafruit_NeoPixel* strip_;
  long last_update_time_;
  bool in_transition_;
};

class Rainbow : public FadeCycler {
 public:
  Rainbow(Adafruit_NeoPixel* strip, const int delay);
 protected:
  Color NextColor();
  void Init();
  uint8_t current_color_;
};

#endif
