#include <Arduino.h>
#include <mtof.h>
#include <scales.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#include <AudioSampleClap.h>
#include <AudioSampleSamples2kickyolo.h>
#include <AudioSampleSamples3snarevinyldlj3.h>
#include <AudioSampleSamples5hihatcloseddomain.h>
#include <AudioSampleSamples6tomanalog5.h>
#include <AudioSampleSamples7congamiddrive.h>
#include <AudioSampleSamplesclap909.h>
#include <AudioSampleSampleskickyolo.h>



// GUItool: begin automatically generated code
AudioPlayMemory          playMem1;       //xy=127,680
AudioAmplifier           amp1;           //xy=289,680
AudioOutputMQS           mqs1;           //xy=482,686
AudioConnection          patchCord1(playMem1, amp1);
AudioConnection          patchCord2(amp1, 0, mqs1, 0);
AudioConnection          patchCord3(amp1, 0, mqs1, 1);
// GUItool: end automatically generated code

const unsigned int* audioSamples[] = {
  AudioSampleClap,
  AudioSampleSamples2kickyolo,
  AudioSampleSamples3snarevinyldlj3,
  AudioSampleSamples5hihatcloseddomain,
  AudioSampleSamples6tomanalog5,
  AudioSampleSamples7congamiddrive,
  AudioSampleSamplesclap909,
  AudioSampleSampleskickyolo
};

int pattern[]={
  1,0,2,1,5,6
};

int pattern_num=0;

void setup() {
  Serial.begin(57600);
  AudioMemory(128);


  pinMode(13, OUTPUT);
}


void loop() {
  
  static unsigned long last_pattern_time = 0;
  static unsigned long last_time = 0;

  if(millis()-last_time>=100){
    playMem1.play(audioSamples[3]);
    last_time=millis();
  }

  if(millis()-last_pattern_time>=500){
  playMem1.play(audioSamples[pattern[pattern_num]]);
  pattern_num=(pattern_num+1)%6;
  last_pattern_time=millis();
  }
  



}
