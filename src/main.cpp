#include <Arduino.h>
#include <USB_MIDI.h>

#include <Global.h>
#include <SamplePlayer.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <AudioSampleHat2.h> 
#include <AudioSampleSnare.h> 
#include <AudioSampleHat.h> 
#include <AudioSampleKick.h> 
#include <AudioSampleBop.h> 
#include <AudioSampleChord1.h> 
#include <AudioSampleChord2.h> 


SamplePlayer player(8);
Global global(&player);

void setup() {
  Serial.begin(9600);
  AudioMemory(256);
  player.add_sample("KICK", AudioSampleKick);
  player.add_sample("SNARE", AudioSampleSnare);
  player.add_sample("HAT", AudioSampleHat);
  player.add_sample("HAT2", AudioSampleHat2);
  player.add_sample("BOP", AudioSampleBop);
  player.add_sample("CHORD1", AudioSampleChord1);
  player.add_sample("CHORD2", AudioSampleChord2);

  // usbMIDI.setHandleClock([]() { global.handleMidiClock(); });
  // usbMIDI.setHandleStart([]() { global.handleMidiStart(); });
  // usbMIDI.setHandleStop([]() { global.handleMidiStop(); });
  // usbMIDI.setHandleContinue([]() { global.handleMidiContinue(); });
}

void loop() {
  global.loop();
}