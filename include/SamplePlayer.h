#pragma once

#include <Audio.h>
#include <string>
#include <unordered_map>
#include "Instrument.h"

class SamplePlayer : public Instrument {
 private:
  int num_samples;
  int sample_idx = 0;

  AudioPlayMemory** players;
  AudioEffectReverb** reverbs;
  AudioMixer4** mixers;
  AudioConnection** connections;

  std::unordered_map<std::string, const unsigned int*> samples;
  std::unordered_map<std::string, int> sample_index;

 public:
  SamplePlayer(int num_voices) : Instrument(num_voices) {
    num_samples = num_voices;

    players = new AudioPlayMemory*[num_sources];
    reverbs = new AudioEffectReverb*[num_sources];
    mixers = new AudioMixer4*[num_sources];
    connections = new AudioConnection*[num_sources * 3];

    for (int i = 0; i < num_sources; i++) {
      players[i] = new AudioPlayMemory();
      reverbs[i] = new AudioEffectReverb();
      mixers[i] = new AudioMixer4();

      reverbs[i]->reverbTime(0.5f);

      connections[i * 3 + 0] = new AudioConnection(*players[i], 0, *reverbs[i], 0);
      connections[i * 3 + 1] = new AudioConnection(*players[i], 0, *mixers[i], 0);     // dry
      connections[i * 3 + 2] = new AudioConnection(*reverbs[i], 0, *mixers[i], 1);     // wet

      mixers[i]->gain(0, 1.0f);
      mixers[i]->gain(1, 0.0f);

      sources[i] = mixers[i];
    }

    setup_mixer();
  }

  void add_sample(const std::string& name, const unsigned int* data) {
    if (sample_index.size() >= num_samples) return;
    int index = sample_idx++;
    sample_index[name] = index;
    samples[name] = data;
  }

  void trigger(const std::string& name) {
    auto it = samples.find(name);
    auto idx = sample_index.find(name);
    if (it == samples.end() || idx == sample_index.end()) return;
    int index = idx->second;
    players[index]->play(it->second);
  }

  void stop(const std::string& name) {
    auto idx = sample_index.find(name);
    if (idx == sample_index.end()) return;
    int index = idx->second;
    players[index]->stop();
  }

  void set_reverb_time(const std::string& name, float t) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    reverbs[it->second]->reverbTime(t);
  }

  void set_dry_wet(const std::string& name, float dry, float wet) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    mixers[it->second]->gain(0, dry);
    mixers[it->second]->gain(1, wet);
  }

  ~SamplePlayer() {
    for (int i = 0; i < num_sources; i++) {
      delete players[i];
      delete reverbs[i];
      delete mixers[i];
    }
    for (int i = 0; i < num_sources * 3; i++) delete connections[i];
    delete[] players;
    delete[] reverbs;
    delete[] mixers;
    delete[] connections;
  }
};
