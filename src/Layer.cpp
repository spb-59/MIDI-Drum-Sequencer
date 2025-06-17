#include "Layer.h"
#include "Global.h"
#include <string>
std::string sampleMap[] = {"KICK", "SNARE", "HAT", "HAT2", "BOP", "CHORD1", "CHORD2", "BACK"};


Layer::Layer(Global* g,int i)
{
    global=g;
    LAYER_NUMBER=i;
    for (int i = 0; i < 16; i++) {
        beats[i] = ButtonStatus::None;
    }
    current_beat=0;
    
    for (int i = 0; i < 4; i++) {
        layerEncoders[i].value = 64;
    }
}

void Layer::switchLayer(){
for (int i = 0; i < 16; ++i) {
  global->states.ringButtons[i].status = beats[i];
   }
   
  for (int i = 0; i < 4; i++) {
    global->states.encoders[i].value = layerEncoders[i].value;
  }
}
void Layer::update(){
  for (int i = 0; i < 16; ++i) {
 beats[i]= global->states.ringButtons[i].status ;
   }

  for (int i = 0; i < 4; i++) {
    layerEncoders[i].value = global->states.encoders[i].value;
  }
  sendEnc();
}
void Layer::reset(){
    
   for (int i = 0; i < 16; ++i) {
 beats[i]= ButtonStatus::None ;
   }
   current_beat=0;
  
}
void Layer::playBeat() {
    divs[0] = divs[1] = divs[2] = divs[3] = false;

    if (beats[current_beat] != ButtonStatus::None) {
        switch (beats[current_beat]) {
            case ButtonStatus::Pressed:
                divs[0] = true;
                break;
            case ButtonStatus::Double:
                divs[0] = true;
                divs[2] = true;
                break;
            case ButtonStatus::Quad:
                divs[0] = true;
                divs[1] = true;
                divs[2] = true;
                divs[3] = true;
                break;
            default:
                break;
        }
    } else{
         usbMIDI.sendNoteOff(LAYER_NUMBER,127,1); 
    }

    current_beat = (current_beat + 1) % 16;
}
void Layer::sendEnc(){
    for (int i=0;i<4;i++) usbMIDI.sendControlChange(LAYER_NUMBER*4+i,layerEncoders[i].value,1);
}


void Layer::playDiv(int numDiv){
    if (divs[numDiv]){
      if (global->mode==Mode::MIDI){
        usbMIDI.sendNoteOff(LAYER_NUMBER,127,1); 
        usbMIDI.sendNoteOn(LAYER_NUMBER,127,1); }
      else{
        global->player->trigger(sampleMap[LAYER_NUMBER]);
      }
    }
}
void Layer::printLayerEncoders() {
  Serial.print("Layer ");
  Serial.print(LAYER_NUMBER);
  Serial.println(" Encoders:");
  for (int i = 0; i < 4; i++) {
    Serial.print(layerEncoders[i].value);
    Serial.print(i < 3 ? ", " : "\n");
  }
}
