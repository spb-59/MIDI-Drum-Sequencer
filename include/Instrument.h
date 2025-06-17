#pragma once

#include <Audio.h>

class Instrument {
 protected:
  int num_sources;
  int num_mixers;
  AudioStream** sources;
  AudioMixer4* mixers;
  AudioMixer4* final_mixer;
  AudioConnection** connections;
  AudioConnection* ampConnection;
  AudioConnection left;
  AudioConnection right;
  float gain=25;
  AudioOutputMQS  mqs;


 public:
  Instrument(int num_sources) {
    this->num_sources = num_sources;
    sources = new AudioStream*[num_sources];
    for (int i = 0; i < num_sources; i++) sources[i] = nullptr;
  }

  void setup_mixer() {
    num_mixers = (num_sources + 3) / 4;
    mixers = new AudioMixer4[num_mixers];
    final_mixer = new AudioMixer4();
    connections = new AudioConnection*[num_sources + num_mixers];

    for (int i = 0; i < num_sources; i++) {
      int mixerNum = i / 4;
      int mixerInput = i % 4;
      connections[i] = new AudioConnection(*sources[i], 0, mixers[mixerNum], mixerInput);
      mixers[mixerNum].gain(mixerInput, 0.25f);
    }

    for (int i = 0; i < num_mixers; i++) {
      connections[num_sources + i] = new AudioConnection(mixers[i], 0, *final_mixer, i);
      final_mixer->gain(i, 0.25f);
    }
    output();
  }

AudioAmplifier output() { 

    AudioAmplifier amp; 
    amp.gain(gain);
    ampConnection = new AudioConnection(*final_mixer, amp);

    AudioConnection          patchCord1(amp, 0, mqs, 0);
    AudioConnection          patchCord2(amp, 0, mqs, 1);
    left=patchCord1;
    right=patchCord2;

}

  virtual ~Instrument() {
    delete[] sources;
    delete[] mixers;
    for (int i = 0; i < num_sources + num_mixers; i++) delete connections[i];
    delete[] connections;
    delete final_mixer;
  }
};
