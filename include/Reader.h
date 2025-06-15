#pragma once

#include <Arduino.h>
#include <FastLED.h>

enum class ButtonStatus { None, Pressed, Double, Triple, Quad, Hold };

const char* statusToString(ButtonStatus status);
CRGB statusToColor(ButtonStatus status);

struct ButtonState {
  int current = 1, previous = 1;
  unsigned long lastChangeTime = 0, lastPressTime = 0;
  int pressCount = 0;
  ButtonStatus status = ButtonStatus::None;

  void update(bool newState, unsigned long now);
};

struct ControlsState {
  ButtonState ringButtons[16];
  ButtonState layerButtons[8];
  int pots[4];
};

int readMux(int s0, int s1, int s2, int com, int channel);
int readMuxAnalog(int s0, int s1, int s2, int com, int channel);
void scanMux(int s0, int s1, int s2, int com, int* states, int n);
void scanMuxAnalog(int s0, int s1, int s2, int com, int* values, int n);
void readControls(ControlsState& state);
void initPins();
