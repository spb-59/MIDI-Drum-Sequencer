#pragma once
#include <FastLED.h>


class LEDController {
public:
  static const int NUM_LEDS = 16;
  static const int LED_PIN = 34;
  static const int LED_BRIGHTNESS = 16;

  LEDController();

  void setState(const CRGB* colors);
  void setLED(int index, CRGB color);
  void clear();
  void setBeatPos(int pos);
  void next_beat();
  void render();

private:
  CRGB leds[NUM_LEDS];
  CRGB currentState[NUM_LEDS];
  int beatPos = 0;

};
