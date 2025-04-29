#include <Arduino.h>
#include <USB_MIDI.h>


int knobs_curr[3]={0,0,0};
int knobs_prev[3]={0,0,0};
int notes[] = { 63,60,61, 67,69,67,61,60,63};
int curr_note=-1;
bool sig = false;
bool onOFF = false;
int velocity = 127;
int channel = 1;
bool sustain = false;
int interval=5;
bool start=false;
bool playing = false;
unsigned long lastClock = 0;




void arp() {
  curr_note = curr_note<8? curr_note+1:0;     
  int velocity = random(80, 92);        

  int interval = (6000/87)*1;              

  usbMIDI.sendNoteOn(notes[curr_note], velocity, channel);
  delay(interval*0.8 );                
  usbMIDI.sendNoteOff(notes[curr_note], 0, channel);
  delay(interval*0.2);                
}


  void setup() {
    Serial.begin(57600);
    analogReadAveraging(64);
    pinMode(33, INPUT_PULLUP);
    pinMode(34, INPUT_PULLUP);
    pinMode(35, INPUT_PULLUP);
  }
  
  // the loop() methor runs over and over again,
  // as long as the board has power
  
  void loop() {


    knobs_curr[0]=analogRead(A0);
    if (knobs_curr[0]!=knobs_prev[0]){
      // Serial.print("Knob 1:");
      // Serial.println(knobs_curr[0]);

       usbMIDI.sendControlChange(1,knobs_curr[0]/8,channel);
       knobs_prev[0]=knobs_curr[0];

    }
    
    delay(5);
    knobs_curr[1]=analogRead(A2);
    if (knobs_curr[1]!=knobs_prev[1]){
      // Serial.print("Knob 2:");
      // Serial.println(knobs_curr[1]);

       usbMIDI.sendControlChange(2,knobs_curr[1]/8,channel);
       knobs_prev[1]=knobs_curr[1];
    }
    delay(5);

    int data = digitalRead(33);

    if (onOFF) {
      arp();
      digitalWrite(13, 1);
    } else {
      digitalWrite(13, 0);
    }
  
    if (data == LOW && !sig) {
      // Serial.print("Button Pressed");
      sig = true;
      onOFF = !onOFF;
      delay(2);
    }
  
    if (data == HIGH && sig) {
      sig = false;
      delay(2);
    }


    //sustain pedal on/off
    int sustainData = digitalRead(34);
    if (sustainData == LOW && !sustain) {
      usbMIDI.sendControlChange(64,100,channel);
      sustain = true;
      delay(2);
    }
    
    if (sustainData == HIGH && sustain) {
    
      usbMIDI.sendControlChange(64,0,channel);
      sustain = false;
      delay(2);
    }

    int startData = digitalRead(35);
    if (startData == LOW && !start) {

        Serial.println("Button Pressed: START");
        usbMIDI.sendRealTime(usbMIDI.Start); // start from beginning
        start = true;


      delay(2); 
    }
    if (startData == HIGH && start) {
      start = false;

      delay(2);
    }
  
    if (start) {
      unsigned long now = millis();
      if (now - lastClock >= 14.45) { 
        usbMIDI.sendRealTime(usbMIDI.Clock);
        lastClock = now;
      }
    }
    usbMIDI.read();
  }
  
