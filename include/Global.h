#pragma once
#include <Arduino.h>
#include <USB_MIDI.h>

#include "LedControlller.h"
#include "Layer.h"
#include "Reader.h"

class Global {
 private:
  void update_states();
 public:
  Global();
  void print_states();
  int current_layer=0;
  ControlsState states;
  LEDController leds;
  Layer* layers[8];
  void loop() {
    static long t=0;
    update_states();
    layers[current_layer]->update();
    for (int button=0;button<16;button++){
    leds.setLED(button, statusToColor(states.ringButtons[button].status));
    }

    if (millis()-t>=100){

      t=millis();
      for(int i=0;i<8;i++){
        layers[i]->playBeat();
      }
      leds.next_beat();
    }

  }
  ~Global();
};
