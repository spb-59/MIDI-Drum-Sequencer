#pragma once

#include <Audio.h>

#include <string>
#include <unordered_map>

#include "Instrument.h"

class SamplePlayer : public Instrument {
 private:
  int num_samples;

  AudioPlayMemory** players;
  int sample_idx=0;
bool* players_active;

  std::unordered_map<std::string, const unsigned int*> samples;
  std::unordered_map<std::string, int> sample_index;

 public:
   SamplePlayer(int num_voices) : Instrument(num_voices) {
    num_samples = num_voices;
    players = new AudioPlayMemory*[num_sources];
    players_active = new bool[num_sources];

    for (int i = 0; i < num_sources; i++) {
        players[i] = new AudioPlayMemory();
        sources[i] = players[i];
    }

    setup_mixer();
}

void add_sample(std::string name,const unsigned int* data){
    if (sample_index.size()>num_samples) return;
    sample_index[name]=sample_idx++;
    samples[name]=data;
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
};