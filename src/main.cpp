#include <Arduino.h>
#include <USB_MIDI.h>
#include <scales.h>

int knobs_curr[6]={0,0,0,0,0,0};
int knobs_prev[6]={0,0,0,0,0,0};
int knobs[6]={A0,A1,A2,A3,A4,A5};
int velocity = 127;
int delay_time=10;
int current_pitch_bend = 0;
int prev_pitch_bend = 0;


unsigned long previousTime = 0;
unsigned long previousLEDTime = 0;
unsigned long ledInterval = 200; 
int currentKnob = 0;
bool isNoteOn = false;
int currentNote = 0;

void readKnob(int knobIndex, int analogPin) {
  knobs_curr[knobIndex] = analogRead(analogPin);
  if (knobs_curr[knobIndex] != knobs_prev[knobIndex]) {
    Serial.print("Knob ");
    Serial.print(knobIndex + 1);
    Serial.print(": ");
    Serial.println(knobs_curr[knobIndex]);
    knobs_prev[knobIndex] = knobs_curr[knobIndex];
  }
}

void setup() {
    Serial.begin(57600);
    analogReadAveraging(64);
    pinMode(13, OUTPUT);
    
}
  
void loop() {
  unsigned long currentTime = millis();
  

  for (int i = 0; i < 6; i++) {
    readKnob(i, knobs[i]);
  }
  
  velocity = random(50, 120);
  delay_time = constrain(map(knobs_curr[4], 0, 1023, 50, 300), 50, 300);
  
 
  int pitch_bend = map(knobs_curr[5], 0, 1023, -8192, 8191);
  

  //to avoid noise in pitch bend
  if (abs(pitch_bend) < 500) {
    current_pitch_bend = 0; 
  } else {
    current_pitch_bend = pitch_bend;
  }
  
  
  if (abs(current_pitch_bend - prev_pitch_bend) > 200) {
    usbMIDI.sendPitchBend(current_pitch_bend, 1);
    prev_pitch_bend = current_pitch_bend;
  }
  

  if (currentTime - previousTime >= delay_time) {
    previousTime = currentTime;
    
    if (isNoteOn) {
      // ff note is on, turn it off and change to next knob
      usbMIDI.sendNoteOff(currentNote, velocity, 1);
      isNoteOn = false;
      
      currentKnob = (currentKnob + 1) % 4;
    } else {
  
      currentNote = constrain(map(knobs_curr[currentKnob], 0, 1023, 60, 72), 60, 72);
      currentNote = scales.minor(currentNote);
      usbMIDI.sendNoteOn(currentNote, velocity, 1);
      isNoteOn = true;
    }
  }

  // led hould be independent of midi
  if (currentTime - previousLEDTime >= ledInterval) {
    previousLEDTime = currentTime;
    digitalWriteFast(13, !digitalReadFast(13)); 
  }
  
}
