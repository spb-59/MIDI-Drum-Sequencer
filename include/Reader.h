#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include <Encoder.h>

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
CRGB* statesToColors(ButtonState* status, int num=16);

struct EncoderState {
    int value = 64;
    
    void update(long rawPosition, long& lastRawValue, int sensitivity = 2);
};

struct ControlsState {
  ButtonState ringButtons[16];
  ButtonState layerButtons[8];
  int pots[4];
  EncoderState encoders[4];
};

int readMux(int s0, int s1, int s2, int com, int channel);
int readMuxAnalog(int s0, int s1, int s2, int com, int channel);
void scanMux(int s0, int s1, int s2, int com, int* states, int n);
void scanMuxAnalog(int s0, int s1, int s2, int com, int* values, int n);
void readControls(ControlsState& state);
void initPins();

const int APins[4] = {16, 18, 20, 22};
const int BPins[4] = {15, 17, 19, 21};
extern Encoder encoderObjs[4];
extern long lastRawValues[4];
