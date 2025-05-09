#include <Arduino.h>
#include <USB_MIDI.h>
#include <scales.h>


int knobs_curr[6]={0,0,0,0,0,0};
int knobs_prev[6]={0,0,0,0,0,0};
int knobs[5]={A0,A1,A2,A3,A4};
int velocity = 127;
int delay_time=10;
int offset=0;



void readKnob(int knobIndex, int analogPin) {
  knobs_curr[knobIndex] = analogRead(analogPin);
  if (knobs_curr[knobIndex] != knobs_prev[knobIndex]) {
    Serial.print("Knob ");
    Serial.print(knobIndex + 1);
    Serial.print(": ");
    Serial.println(knobs_curr[knobIndex]);
    knobs_prev[knobIndex] = knobs_curr[knobIndex];
  }
  delay(1);
}
void sendMidi(int i) {
  int data = constrain(map(knobs_curr[i], 0, 1023, 60, 72), 60, 72);
  data = scales.minor(data)+offset;
  usbMIDI.sendNoteOn(data, velocity, 1);
  delay(delay_time);
  usbMIDI.sendNoteOff(data, velocity, 1);
}

void setup() {
    Serial.begin(57600);
    analogReadAveraging(64);
  }
  
  // the loop() methor runs over and over again,
  // as long as the board has power
  
  void loop() {

    for (int i=0;i<4;i++){
      readKnob(i,knobs[i]);
      readKnob(4,4);
      readKnob(5,5);

      velocity=random(50,120);
      delay_time=constrain(map(knobs_curr[4],0,1023,120,200),80,300);
      offset=scales.minor(constrain(map(knobs_curr[5],0,1023,-12,12),-12,12));

      sendMidi(i);
    }

    usbMIDI.read();
  }
