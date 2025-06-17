#pragma once
#include <Arduino.h>
#include <USB_MIDI.h>

#include "Layer.h"
#include "LedControlller.h"
#include "Reader.h"
#include "SamplePlayer.h"

enum class Mode{
  MIDI,OUT
};
class Global {
 private:
  void update_states();
  void resetLayers();

  int clock_count=-1;
  int div_count = -1;
  int NUM_BEATS=2;
  int BPM=100;
  
  
  
  public:
  SamplePlayer* player=nullptr;
  Mode mode=Mode::OUT;
  Global(SamplePlayer* player);
  void print_states();
  void print_all_layer_encoders();
  int current_layer = 0;
  ControlsState states;
  LEDController leds;
  Layer* layers[8];

void loop() {
  // unsigned long t0 = micros();

  bool do_next = false;

  const int CLOCKS_PER_BEAT = 24;
  const unsigned long beat_us = 60000000UL / (BPM*NUM_BEATS);
  const unsigned long clock_us = beat_us / CLOCKS_PER_BEAT;
  static unsigned long lastTimestamp = 0;
  unsigned long now = micros();
  if (lastTimestamp == 0) lastTimestamp = now;

  // unsigned long t1 = micros();
  //Serial.print("Setup: "); //Serial.print(t1 - t0); //Serial.println(" us");

  if ((long)(now - lastTimestamp) >= clock_us) {
    clock_count = (clock_count + 1) % CLOCKS_PER_BEAT;
    if (clock_count == 0) {
      for (int i = 0; i < 8; i++) layers[i]->playBeat();
    }
    if (clock_count==0){
      do_next=true;// to improve visual feedback
    }
    if (clock_count % 6 == 0) {
      div_count = (div_count + 1) % 4;
      for (int i = 0; i < 8; i++) layers[i]->playDiv(div_count);
    }
    lastTimestamp += clock_us;
  }

  // unsigned long t2 = micros();
  //Serial.print("Timing block: "); //Serial.print(t2 - t1); //Serial.println(" us");

  update_states();

  // unsigned long t4 = micros();
  //Serial.print("update_states: "); //Serial.print(t4 - t2); //Serial.println(" us");

  layers[current_layer]->update();

  // unsigned long t5 = micros();
  //Serial.print("layer update: "); //Serial.print(t5 - t4); //Serial.println(" us");

  // leds.setState(statesToColors(states.ringButtons));
  for (int button = 0; button < 16; button++) {
    leds.setLED(button, statusToColor(states.ringButtons[button].status));
  }
  if (do_next){
    leds.next_beat();
  }
  leds.render();
  // unsigned long t6 = micros();
  //Serial.print("setLEDs: "); //Serial.print(t6 - t5); //Serial.println(" us");

  //Serial.print("Total: "); //Serial.print(t6 - t0); //Serial.println(" us");
  //Serial.println("---");
}



  ~Global();
};
