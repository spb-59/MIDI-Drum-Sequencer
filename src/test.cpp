// #include <Arduino.h>
// #include <Audio.h>
// #include <SD.h>
// #include <SPI.h>
// #include <SerialFlash.h>
// #include <Synth.h>
// #include <USB_MIDI.h>
// #include <Wire.h>
// #include <mtof.h>
// #include <scales.h>

// Synth synth(16);
// Synth saw_synth(16,WAVEFORM_SAWTOOTH);

// AudioOutputMQS mqs1;

// AudioMixer4              mixer;
// AudioMixer4* sine = synth.output_setup();
// AudioMixer4* saw = saw_synth.output_setup();
// AudioConnection          patchCord1(*sine, 0, mixer, 0);
// AudioConnection          patchCord2(*saw, 0, mixer, 1);
// AudioConnection          patchCord3(mixer, 0, mqs1, 0);
// AudioConnection          patchCord4(mixer, 0, mqs1, 1);

// bool note_status = false;
// bool sustain_status = false;

// void myControlChange(byte channel, byte control, byte value) {
//   if (control == 64) {
//     sustain_status = (value >= 64);

//     if (channel==1)synth.set_sustain(sustain_status);
//     if (channel==2) saw_synth.set_sustain(sustain_status);
//   }
// }

// void myNoteOn(byte channel, byte note, byte velocity) {
//   if (velocity < 0) {
//     return;
//   }

//   digitalWrite(13, HIGH);
//   if (channel==1) synth.note_on(note, velocity);
//   if (channel==2) saw_synth.note_on(note, velocity);

// }

// void myNoteOff(byte channel, byte note, byte velocity) {
//   if (channel==1)  synth.note_off(note, velocity);
//   if (channel==2)  saw_synth.note_off(note, velocity);
//   digitalWrite(13, LOW);
// }

// void setup() {
//   Serial.begin(57600);
//   AudioMemory(128);


//   pinMode(13, OUTPUT);
//   usbMIDI.setHandleNoteOn(myNoteOn);
//   usbMIDI.setHandleNoteOff(myNoteOff);
//   usbMIDI.setHandleControlChange(myControlChange);
// }

// void loop() {
//   usbMIDI.read();

//   // Debug output
//   static unsigned long lastDebugTime = 0;
//   if (millis() - lastDebugTime > 5000) {
//     Serial.print("Audio status - CPU: ");
//     Serial.print(AudioProcessorUsage());
//     Serial.print("%, Memory: ");
//     Serial.print(AudioMemoryUsage());
//     Serial.print("/");
//     Serial.println(AudioMemoryUsageMax());
//     lastDebugTime = millis();
//   }
// }
