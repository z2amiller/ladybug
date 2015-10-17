#include <Adafruit_NeoPixel.h>
#include "LadybugActions.h"

static Color RandomColor() {
  return Color(random(0, 256), random(0, 256), random(0, 256));
}

Color::Color(uint8_t red, uint8_t green, uint8_t blue)
    : r(red),
      g(green),
      b(blue) {}

Color::Color(const Color& other)
    : r(other.r),
      g(other.g),
      b(other.b) {}

void Color::Scale(const uint8_t scale) {
  r /= scale;
  g /= scale;
  b /= scale;
}

uint32_t Color::AsNeoPixelColor() {
  return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | b;
}

IntermediateColor::IntermediateColor(int red, int green, int blue)
    : r(red),
      g(green),
      b(blue) {}

IntermediateColor::IntermediateColor(const Color& color)
    : r(color.r << 6),
      g(color.g << 6),
      b(color.b << 6) {}

void IntermediateColor::Subtract(const IntermediateColor& other) {
  r -= other.r;
  g -= other.g;
  b -= other.b;
}

void IntermediateColor::Add(const IntermediateColor& other) {
  r = ((r + other.r) < 32767) ? r + other.r : 32767;
  g = ((g + other.g) < 32767) ? g + other.g : 32767;
  b = ((b + other.b) < 32767) ? b + other.b : 32767;
}

void IntermediateColor::Scale(const uint8_t scale) {
  r /= scale;
  b /= scale;
  g /= scale;
}

Color IntermediateColor::AsColor() {
  return Color(
      (uint8_t)((r >> 6) & 255),
      (uint8_t)((g >> 6) & 255),
      (uint8_t)((b >> 6) & 255));
}

Fader::Fader(const uint8_t steps, const Color& start)
    : num_steps_(steps),
      current_color_(start),
      step_increment_(0, 0, 0) {}

void Fader::Next(const Color& color) {
  current_step_ = 0;
  current_color_ = IntermediateColor(current_color_.AsColor());
  step_increment_ = IntermediateColor(color);
  step_increment_.Subtract(current_color_);
  step_increment_.Scale(num_steps_);
}

Color Fader::Step() {
  current_step_++;
  if (current_step_ <= num_steps_) {
    current_color_.Add(step_increment_);
  }
  return current_color_.AsColor();
}

bool Fader::Done() {
  return current_step_ >= num_steps_;
}

Breather::Breather(Adafruit_NeoPixel* strip, int delay)
  : fader_(128, RainbowColors[0]),
    breathe_delay_(delay),
    cycle_position_(0),
    strip_(strip),
    ascending_(false),
    last_breathe_time_(millis()),
    last_cycle_time_(millis() + 1000) {
  fader_.Next(DarkColor(cycle_position_));
}

Color Breather::DarkColor(const uint8_t idx) {
  Color t(RainbowColors[idx]);
  t.Scale(10);
  return t;
}

void Breather::update() {
  if (last_breathe_time_ + breathe_delay_ <= millis()) {
    last_breathe_time_ = millis();
    Color color = fader_.Step();
    if (fader_.Done()) {
      ascending_ = !ascending_;
      if (ascending_) {
        fader_.Next(RainbowColors[cycle_position_]);
      } else {
        fader_.Next(DarkColor(cycle_position_));
      }
    }
    strip_->setPixelColor(0, color.AsNeoPixelColor());
    strip_->show();
  }
}

void Breather::cycle() {
  if (last_cycle_time_ + 1000 <= millis())  {
    last_cycle_time_ = millis();
    cycle_position_++;
    if (cycle_position_ >= RainbowColorLength) {
      cycle_position_ = 0;
    }
  }
}

FadeCycler::FadeCycler(Adafruit_NeoPixel* strip,
                       const int hold_delay,
                       const int update_delay)
  : hold_delay_(hold_delay),
    update_delay_(update_delay),
    fader_(100, NextColor()),
    strip_(strip),
    last_update_time_(millis()),
    in_transition_(true) {
  fader_.Next(NextColor());
}

Color FadeCycler::NextColor() {
  return RandomColor();
}

void FadeCycler::update() {
  if (in_transition_ && last_update_time_ + update_delay_ <= millis()) {
    last_update_time_ = millis();
    Color color = fader_.Step();
    strip_->setPixelColor(0, color.AsNeoPixelColor());
    strip_->show();
    if (fader_.Done()) {
      fader_.Next(NextColor());
      in_transition_ = false;
    }
  }
  if (last_update_time_ + hold_delay_ <= millis()) {
    in_transition_ = true;
    last_update_time_ = millis();
  }
}

Rainbow::Rainbow(Adafruit_NeoPixel* strip, const int delay)
    : FadeCycler(strip, delay, 10),
      current_color_(0) {
  fader_.SetColor(NextColor());
  fader_.Next(NextColor());
}

Color Rainbow::NextColor() {
  current_color_++;
  if (current_color_ >= RainbowColorLength) {
    current_color_ = 0;
  }
  return RainbowColors[current_color_];
}
