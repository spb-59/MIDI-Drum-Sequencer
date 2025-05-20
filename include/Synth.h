#pragma once

#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>
#include <mtof.h>

#include <unordered_map>
class Synth {
 private:
  int num_voices;
  int free_index = 0;
  int num_mixers = 0;
  std::unordered_map<byte, int> voices_map;
  bool* voices_active;
  AudioMixer4* voice_mixers;
  AudioMixer4* final_mixer;
  AudioConnection** connections;

int waveform=WAVEFORM_SINE;
  bool sustain = false;

 public:
  AudioSynthWaveform** voices;
  Synth(int voices = 16,int waveform=WAVEFORM_SINE);
  void note_on(byte note, byte velocity);
  void note_off(byte note, byte velocity);
  void set_sustain(bool on);
  
  AudioMixer4* output_setup();
  ~Synth();
};

Synth::Synth(int num_voices = 16,int waveform=WAVEFORM_SINE) {
  this->num_voices = num_voices;
  this->num_mixers = (num_voices + 3) / 4;
  this->waveform=waveform;
  
  voices = new AudioSynthWaveform*[num_voices];
  voices_active = new bool[num_voices];
  
  for (int i = 0; i < num_voices; i++) {
    voices[i] = new AudioSynthWaveform();
    voices[i]->begin(0.0, 0, waveform);
    voices_active[i] = false;
  }
  
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
    free_index = (free_index + 1) % num_voices;
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

AudioMixer4* Synth::output_setup() {
  voice_mixers = new AudioMixer4[num_mixers];
  final_mixer = new AudioMixer4();

  int total_connections = num_voices + num_mixers + 2;
  connections = new AudioConnection*[total_connections];


  for (int i = 0; i < num_voices; i++) {
    int mixerNum = i / 4;
    int mixerInput = i % 4;

    connections[i] =
        new AudioConnection(*voices[i], 0, voice_mixers[mixerNum], mixerInput);
    voice_mixers[mixerNum].gain(mixerInput, 0.25);  // 25% mix for each input
  }


  for (int i = 0; i < num_mixers; i++) {
    connections[num_voices + i] =
        new AudioConnection(voice_mixers[i], 0, *final_mixer, i);
    final_mixer->gain(i, 0.25);  // Equal gain distribution
  }

  return final_mixer;
}

Synth::~Synth() {
  for (int i = 0; i < num_voices; i++) {
    delete voices[i];
  }
  delete[] voices;
  delete[] voices_active;
}