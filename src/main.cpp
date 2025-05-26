#include <Arduino.h>
#include <Audio.h>
#include <AudioSampleBack.h>
#include <AudioSampleBop.h>
#include <AudioSampleChord1.h>
#include <AudioSampleChord2.h>
#include <AudioSampleHat.h>
#include <AudioSampleHat2.h>
#include <AudioSampleKick.h>
#include <AudioSampleSnare.h>
#include <SD.h>
#include <SPI.h>
#include <SamplePlayer.h>
#include <SerialFlash.h>
#include <Wire.h>
#include <mtof.h>
#include <scales.h>

SamplePlayer player(16);

AudioAmplifier player_amp = player.output(5);

AudioOutputMQS mqs1;
AudioConnection patchCord2(player_amp, 0, mqs1, 0);
AudioConnection patchCord3(player_amp, 0, mqs1, 1);

int pattern_num = 0;

void myNoteOn(byte channel, byte note, byte velocity) {
  if (velocity < 0) {
    return;
  }

  if (note == 60) player.trigger("KICK");
  if (note == 61) player.trigger("SNARE");
  if (note == 62) player.trigger("CHAT");
  if (note == 63) player.trigger("CHAT2");
  if (note == 64) player.trigger("BACK");
  if (note == 65) player.trigger("C1");
  if (note == 66) player.trigger("C2");
  if (note == 67) player.trigger("BOP");

  digitalWrite(13, HIGH);
}

void myNoteOff(byte channel, byte note, byte velocity) {
  digitalWrite(13, LOW);

  if (note == 60) player.stop("KICK");
  if (note == 61) player.stop("SNARE");
  if (note == 62) player.stop("CHAT");
  if (note == 63) player.stop("CHAT2");
  if (note == 64) player.stop("BACK");
  if (note == 65) player.stop("C1");
  if (note == 66) player.stop("C2");
  if (note == 67) player.stop("BOP");
}
void setup() {
  Serial.begin(57600);
  Serial.println("Here");
  AudioMemory(256);
  
  player.add_sample("KICK", AudioSampleKick);
  player.add_sample("SNARE", AudioSampleHat2);
  player.add_sample("CHAT", AudioSampleHat);
  player.add_sample("CHAT2", AudioSampleHat2);
  player.add_sample("BACK", AudioSampleBack);
  player.add_sample("C1", AudioSampleChord1);
  player.add_sample("C2", AudioSampleChord2);
  player.add_sample("BOP", AudioSampleBop);
  
  Serial.println("Here everythin int");
  
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  
  // player.effects["KICK"].reverb->roomsize(0.5);
  // player.effects["SNARE"].reverb->roomsize(0.5);
  
  Serial.println("Here everythin int 3");
  pinMode(13, OUTPUT);
}

void loop() {
  Serial.print("Here before midi");
  usbMIDI.read();
  Serial.print("Here after midi");

  // Debug outputk
  static unsigned long lastDebugTime = 0;

  if (millis() - lastDebugTime > 5000) {
    Serial.print("Audio status - CPU: ");
    Serial.print(AudioProcessorUsage());
    Serial.print("%, Memory: ");
    Serial.print(AudioMemoryUsage());
    Serial.print("/");
    Serial.println(AudioMemoryUsageMax());
    lastDebugTime = millis();
  }
}
