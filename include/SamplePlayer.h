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
  AudioAmplifier** amps;
  AudioEffectBitcrusher** bitcrushers;
  AudioFilterStateVariable** filters;  
  AudioEffectReverb** reverbs;
  AudioMixer4** mixers;
  AudioConnection** connections;

  std::unordered_map<std::string, const unsigned int*> samples;
  std::unordered_map<std::string, int> sample_index;

 public:
    SamplePlayer(int num_voices) : Instrument(num_voices) {
    num_samples = num_voices;

    players = new AudioPlayMemory*[num_sources];
    amps = new AudioAmplifier*[num_sources];   
    bitcrushers = new AudioEffectBitcrusher*[num_sources];
    filters = new AudioFilterStateVariable*[num_sources];  
    reverbs = new AudioEffectReverb*[num_sources];
    mixers = new AudioMixer4*[num_sources];
    connections = new AudioConnection*[num_sources * 6];

    for (int i = 0; i < num_sources; i++) {
      players[i] = new AudioPlayMemory();
      amps[i] = new AudioAmplifier();
      bitcrushers[i] = new AudioEffectBitcrusher();
      filters[i] = new AudioFilterStateVariable(); 
      reverbs[i] = new AudioEffectReverb();
      mixers[i] = new AudioMixer4();

      amps[i]->gain(1.0f);
      bitcrushers[i]->bits(16);
      bitcrushers[i]->sampleRate(44100);
      filters[i]->frequency(20000); 
      filters[i]->resonance(0.7);   
      reverbs[i]->reverbTime(0.5f);

      connections[i * 6 + 0] = new AudioConnection(*players[i], 0, *amps[i], 0);
      connections[i * 6 + 1] = new AudioConnection(*amps[i], 0, *bitcrushers[i], 0);
      connections[i * 6 + 2] = new AudioConnection(*bitcrushers[i], 0, *filters[i], 0);
      connections[i * 6 + 3] = new AudioConnection(*filters[i], 0, *reverbs[i], 0);
      connections[i * 6 + 4] = new AudioConnection(*filters[i], 0, *mixers[i], 0);
      connections[i * 6 + 5] = new AudioConnection(*reverbs[i], 0, *mixers[i], 1);

      mixers[i]->gain(0, 1.0f);
      mixers[i]->gain(1, 0.0f);

      sources[i] = mixers[i];
    }

    setup_mixer();
  }

  void set_gain(const std::string& name, float gain) {
    gain = constrain(gain,0,127) / 127.0f;
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    amps[it->second]->gain(gain);
  }

  void set_filter_frequency(const std::string& name, float value) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    
    const float min_freq = 80.0f;   
    const float max_freq = 18000.0f; 
    
    float normalized = value / 127.0f;
    
    float freq = normalized*min_freq/max_freq;
    
    filters[it->second]->frequency(freq);
  }

  void set_bitcrusher(const std::string& name, float value) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    
    value = constrain(value, 0, 127);
    float normalized = value / 127.0f;
    
    int bits = 16 - (int)(normalized * 12.0f);
    
    float sampleRate = 44100.0f * pow(0.045f, normalized);
    sampleRate = constrain(sampleRate, 2000.0f, 44100.0f);
    
    bitcrushers[it->second]->bits(bits);
    bitcrushers[it->second]->sampleRate((int)sampleRate);
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

  void set_reverb_time(const std::string& name, float value) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    
    float reverbTime = (value / 127.0f) * 5.0f;
    reverbs[it->second]->reverbTime(reverbTime);
  }
  
  void set_dry_wet(const std::string& name, float wetValue) {
      auto it = sample_index.find(name);
      if (it == sample_index.end()) return;
    
      wetValue = constrain(wetValue, 0, 127);
      
      // Map 0-127 to 0-0.7 for wet
      float wet = (wetValue / 127.0f) * 0.7f;
      float dry = 1.0f;  // Keep dry signal at full volume
      
      mixers[it->second]->gain(0, dry);
      mixers[it->second]->gain(1, wet);
  }

  ~SamplePlayer() {
    for (int i = 0; i < num_sources; i++) {
      delete players[i];
      delete amps[i];
      delete bitcrushers[i];
      delete filters[i];
      delete reverbs[i];
      delete mixers[i];
    }
    for (int i = 0; i < num_sources * 6; i++) delete connections[i];
    delete[] players;
    delete[] amps;
    delete[] bitcrushers;
    delete[] filters;
    delete[] reverbs;
    delete[] mixers;
    delete[] connections;
  }
};