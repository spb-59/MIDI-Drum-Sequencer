#pragma once

#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>
#include <mtof.h>

#include <unordered_map>
#include "Instrument.h"

class Synth: public Instrument {
 private:
  int free_index = 0;
  std::unordered_map<byte, int> voices_map;
  bool* voices_active;
  int waveform=WAVEFORM_SINE;
  bool sustain = false;

 public:
  AudioSynthWaveform** voices;
  Synth(int voices = 16,int waveform=WAVEFORM_SINE);
  void note_on(byte note, byte velocity);
  void note_off(byte note, byte velocity);
  void set_sustain(bool on);
  
};

Synth::Synth(int num_voices, int waveform)
    : Instrument(num_voices) {
  this->waveform = waveform;
  voices = new AudioSynthWaveform*[num_voices];
  voices_active = new bool[num_voices];

  for (int i = 0; i < num_voices; i++) {
    voices[i] = new AudioSynthWaveform();
    voices[i]->begin(0.0, 0, waveform);
    voices_active[i] = false;
    sources[i] = voices[i];
  }

  setup_mixer();
}

void Synth::set_sustain(bool on) {
  if (on) {
    this->sustain = on;
  } else {
    this->sustain = false;
    for (auto it = voices_map.begin(); it != voices_map.end();) {
      int index = it->second;
      voices[index]->amplitude(0);
      if (voices_active[index] == true) {
        voices_active[index] = false;
        it = voices_map.erase(it);
      } else {
        ++it;
      }
    }
  }
}

void Synth::note_on(byte note, byte velocity) {
  int index;
  auto it = voices_map.find(note);
  if (it != voices_map.end()) {
    index = it->second;
    Serial.print("Reusing voice ");
  } else {
    index = free_index;
    free_index = (free_index + 1) % num_sources;
    Serial.print("New voice ");
  }
  Serial.print(index);
  Serial.print(" for note ");
  Serial.print(note);
  Serial.print(" freq ");
  Serial.print(mtof.toFrequency(note));
  Serial.print(" vel ");
  Serial.println(velocity);


  float amp = velocity / 127.0f;
  float freq = mtof.toFrequency(note);
  

  voices[index]->begin(amp, freq, waveform);
  
  voices_map[note] = index;
  voices_active[index] = true;
}

void Synth::note_off(byte note, byte velocity) {
  auto it = voices_map.find(note);
  if (it != voices_map.end()) {
    int index = it->second;
    voices_active[index] = false;
    if (!sustain) {

      voices[index]->begin(0, 0, waveform);
      voices_map.erase(it);
    }
  }
}
