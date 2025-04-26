#include <Arduino.h>
#include <USB_MIDI.h>


int knobs_curr[3]={0,0,0};
int knobs_prev[3]={0,0,0};

  
  void setup() {
    Serial.begin(57600);
    analogReadAveraging(64);
  }
  
  // the loop() methor runs over and over again,
  // as long as the board has power
  
  void loop() {
    knobs_curr[0]=analogRead(A14);
    if (knobs_curr[0]!=knobs_prev[0]){
      // Serial.print("Knob 1:");
      // Serial.println(knobs_curr[0]);

       usbMIDI.sendControlChange(1,knobs_curr[0]/8,1);
       knobs_prev[0]=knobs_curr[0];

    }
    
    delay(5);
    knobs_curr[1]=analogRead(A0);
    if (knobs_curr[1]!=knobs_prev[1]){
      // Serial.print("Knob 2:");
      // Serial.println(knobs_curr[1]);

       usbMIDI.sendControlChange(2,knobs_curr[1]/8,1);
       knobs_prev[1]=knobs_curr[1];


    }
    delay(5);

    knobs_curr[2]=analogRead(A1);
    if (knobs_curr[2]!=knobs_prev[2]){
      Serial.print("Knob 3:");
      Serial.println(knobs_curr[2]);

       usbMIDI.sendControlChange(3,knobs_curr[2]/8,1);
       knobs_prev[2]=knobs_curr[2];


    }


    delay(5);

   


  }
  
