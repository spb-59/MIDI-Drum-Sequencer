#include <Arduino.h>
#include <USB_MIDI.h>

#include <Global.h>

Global global;
void setup() {
  Serial.begin(9600);


}

void loop() {
    global.loop();
}