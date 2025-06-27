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
  int RAND_TIMING=0;
  
  bool external_clock_enabled = false;
  unsigned long last_midi_clock_time = 0;
  float midi_bpm = 120.0f;
  int midi_clock_counter = 0;
  bool use_midi_clock = false;
  
  public:
  SamplePlayer* player=nullptr;
  Mode mode=Mode::MIDI;
  Global(SamplePlayer* player);
  void print_states();
  void print_all_layer_encoders();
  int current_layer = 0;
  ControlsState states;
  LEDController leds;
  Layer* layers[8];
  int RAND_VELO=0;


void loop() {
  usbMIDI.read();
  
  bool do_next = false;
  const int CLOCKS_PER_BEAT = 24;

  if (!external_clock_enabled || !use_midi_clock || mode != Mode::MIDI) {
    const unsigned long beat_us = 60000000UL / (BPM*NUM_BEATS);
    const unsigned long clock_us = beat_us / CLOCKS_PER_BEAT;
    static unsigned long lastTimestamp = 0;
    unsigned long now = micros();
    if (lastTimestamp == 0) lastTimestamp = now;

<<<<<<< HEAD
    long time_off = 0;
    if (RAND_TIMING > 0) {
      if (random(100) < RAND_TIMING) {
        int max_offset = map(RAND_TIMING, 1, 100, 200, 600);
        time_off = map(random(10), 0, 9, 0, max_offset);
        time_off *= random(2) > 0 ? -1 : 1;
      }
=======
  long time_off = 0;
  if (RAND_TIMING > 0) {

    if (random(100) < RAND_TIMING) {

      int max_offset = map(RAND_TIMING, 1, 100, 200, 800);
      time_off = map(random(10), 0, 9, 0, max_offset);
      time_off *= random(2) > 0 ? -1 : 1;
>>>>>>> 0bf015d (refactor: update timing and gain settings, improve layer handling, and adjust MIDI handling)
    }

    if ((long)(now - (lastTimestamp+time_off)) >= clock_us) {
      clock_count = (clock_count + 1) % CLOCKS_PER_BEAT;
      
      int pulsesPerStep;
      if (NUM_BEATS == 1) pulsesPerStep = 24;
      else if (NUM_BEATS == 2) pulsesPerStep = 12;
      else if (NUM_BEATS == 4) pulsesPerStep = 6;
      else if (NUM_BEATS == 8) pulsesPerStep = 3;
      else pulsesPerStep = 6;
      
      if (clock_count % pulsesPerStep == 0) {
        for (int i = 0; i < 8; i++) layers[i]->playBeat();
        do_next=true;
      }
      
      int divisionInStep = (clock_count % pulsesPerStep) * (24 / pulsesPerStep);
      for (int i = 0; i < 8; i++) layers[i]->playDiv(divisionInStep);
      
      lastTimestamp += clock_us;
    }
<<<<<<< HEAD
=======
    if (clock_count==0){
      do_next=true;// to improve visual feedback
    }

    div_count = (div_count + 1) % 24;
    for (int i = 0; i < 8; i++) layers[i]->playDiv(div_count);

    lastTimestamp += clock_us;
>>>>>>> 0bf015d (refactor: update timing and gain settings, improve layer handling, and adjust MIDI handling)
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
  
  void handleMidiClock();
  void handleMidiStart();
  void handleMidiStop();
  void handleMidiContinue();
  void sendBpmToDaw();
  void toggleClockSource();
};
