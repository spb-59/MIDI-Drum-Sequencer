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
  bool use_midi_clock = true;
  
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
    // Calculate microseconds per quarter note (scaled by NUM_BEATS)
    const unsigned long quarter_note_us = 60000000UL / (BPM * NUM_BEATS);
    // Then divide by CLOCKS_PER_BEAT to get microseconds per MIDI clock
    const unsigned long clock_us = quarter_note_us / CLOCKS_PER_BEAT;
    static unsigned long lastTimestamp = 0;
    unsigned long now = micros();
    if (lastTimestamp == 0) lastTimestamp = now;

    long time_off = 0;
    if (RAND_TIMING > 0) {
      if (random(100) < RAND_TIMING) {
        int max_offset = map(RAND_TIMING, 1, 100, 200, 600);
        time_off = map(random(10), 0, 9, 0, max_offset);
        time_off *= random(2) > 0 ? -1 : 1;
      }
    }

    if ((long)(now - (lastTimestamp+time_off)) >= (long)clock_us) {
      clock_count = (clock_count + 1) % CLOCKS_PER_BEAT;
      
      const int clocksPerStep = 24; // Always 24 clocks per step
      
      if (clock_count % clocksPerStep == 0) {
        for (int i = 0; i < 8; i++) layers[i]->playBeat();
        do_next=true;
      }
      
      // Process divisions on every clock tick
      div_count = (div_count + 1) % 24;
      for (int i = 0; i < 8; i++) layers[i]->playDiv(div_count);
      
      lastTimestamp += clock_us;
    }
  }

  update_states();

  layers[current_layer]->update();

  for (int button = 0; button < 16; button++) {
    leds.setLED(button, statusToColor(states.ringButtons[button].status));
  }
  if (do_next){
    leds.next_beat();
  }
  leds.render();
}



  ~Global();
  
  void handleMidiClock();
  void handleMidiStart();
  void handleMidiStop();
  void handleMidiContinue();
  void sendBpmToDaw();
  void toggleClockSource();
};
