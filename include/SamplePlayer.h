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
  AudioMixer4** bitcrusher_mixers;
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
    bitcrusher_mixers = new AudioMixer4*[num_sources];
    connections = new AudioConnection*[num_sources * 8]; // Increased connections count

    for (int i = 0; i < num_sources; i++) {
      players[i] = new AudioPlayMemory();
      amps[i] = new AudioAmplifier();
      bitcrushers[i] = new AudioEffectBitcrusher();
      filters[i] = new AudioFilterStateVariable(); 
      reverbs[i] = new AudioEffectReverb();
      mixers[i] = new AudioMixer4();
      bitcrusher_mixers[i] = new AudioMixer4();

      amps[i]->gain(1.0f);
      bitcrushers[i]->bits(4);
      bitcrushers[i]->sampleRate(8000);
      filters[i]->frequency(20000); 
      filters[i]->resonance(0.7);   
      reverbs[i]->reverbTime(0.5f);

      // Set up connections with parallel path for bitcrusher
      connections[i * 8 + 0] = new AudioConnection(*players[i], 0, *amps[i], 0);
      
      // Clean path to bitcrusher mixer (input 0)
      connections[i * 8 + 1] = new AudioConnection(*amps[i], 0, *bitcrusher_mixers[i], 0);
      
      // Processed path through bitcrusher to mixer (input 1)
      connections[i * 8 + 2] = new AudioConnection(*amps[i], 0, *bitcrushers[i], 0);
      connections[i * 8 + 3] = new AudioConnection(*bitcrushers[i], 0, *bitcrusher_mixers[i], 1);
      
      // From bitcrusher mixer to filter
      connections[i * 8 + 4] = new AudioConnection(*bitcrusher_mixers[i], 0, *filters[i], 0);
      
      // Filter to reverb and mixer
      connections[i * 8 + 5] = new AudioConnection(*filters[i], 0, *reverbs[i], 0);
      connections[i * 8 + 6] = new AudioConnection(*filters[i], 0, *mixers[i], 0);
      connections[i * 8 + 7] = new AudioConnection(*reverbs[i], 0, *mixers[i], 1);


      bitcrusher_mixers[i]->gain(0, 1.0f); // Dry 
      bitcrusher_mixers[i]->gain(1, 0.0f); // Wet 


      mixers[i]->gain(0, 1.0f); // Dry 
      mixers[i]->gain(1, 0.0f); // Wet 

      sources[i] = mixers[i];
    }

    setup_mixer();
  }

  void set_gain(const std::string& name, float gain) {
    gain = constrain(gain,0,127) / 127.0f;
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    mixers[it->second]->gain(0, gain);
  }

  void set_filter_frequency(const std::string& name, float value) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    
    // Scale the value between 50 and 20000 Hz (typical filter range)
    float freq = map(constrain(value, 0, 127), 0, 127, 50, 20000);
    filters[it->second]->frequency(freq);
  }

  void set_bitcrusher(const std::string& name, float value) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    

    float wet = constrain(value, 0, 127) / 127.0f;
    float dry = 1.0f - wet;
    

    bitcrusher_mixers[it->second]->gain(0, dry);  // dry 
    bitcrusher_mixers[it->second]->gain(1, wet);  // wet 
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
    
    float reverbTime = constrain(value, 0, 127) / 127.0f * 5.0f; // Scale to 0-5 seconds
    reverbs[it->second]->reverbTime(reverbTime);
  }
  
  void set_dry_wet(const std::string& name, float wetValue) {
    auto it = sample_index.find(name);
    if (it == sample_index.end()) return;
    
    float wet = constrain(wetValue, 0, 127) / 127.0f;
    float dry = 1.0f - wet;
    
    mixers[it->second]->gain(0, dry);  // dry 
    mixers[it->second]->gain(1, wet);  // wet 
  }

  ~SamplePlayer() {
    for (int i = 0; i < num_sources; i++) {
      delete players[i];
      delete amps[i];
      delete bitcrushers[i];
      delete filters[i];
      delete reverbs[i];
      delete mixers[i];
      delete bitcrusher_mixers[i];
    }
    for (int i = 0; i < num_sources * 8; i++) delete connections[i];
    delete[] players;
    delete[] amps;
    delete[] bitcrushers;
    delete[] filters;
    delete[] reverbs;
    delete[] mixers;
    delete[] bitcrusher_mixers;
    delete[] connections;
  }
};