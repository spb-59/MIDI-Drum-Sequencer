#include "Layer.h"
#include "Global.h"
#include <string>
std::string sampleMap[] = {"KICK", "SNARE", "HAT", "HAT2", "BOP", "CHORD1", "CHORD2", "BACK"};


Layer::Layer(Global* g,int i)
{
    global=g;
    LAYER_NUMBER=i+36;
    for (int i = 0; i < 16; i++) {
        beats[i] = ButtonStatus::None;
    }
    current_beat=0;
    
    for (int i = 0; i < 24; i++) {
        divs[i] = false;
    }
    
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
    // Clear all 24 divisions
    for (int i = 0; i < 24; i++) {
        divs[i] = false;
    }

    if (beats[current_beat] != ButtonStatus::None) {
        switch (beats[current_beat]) {
            case ButtonStatus::Pressed:
                divs[0] = true;
                break;
            case ButtonStatus::Double:
                divs[0] = true;
                divs[12] = true;
                break;
            case ButtonStatus::Triple:
                divs[0] = true;
                divs[8] = true;
                divs[16] = true;
                break;
            case ButtonStatus::Quad:
                divs[0] = true;
                divs[6] = true;
                divs[12] = true;
                divs[18] = true;
                break;
            case ButtonStatus::Hold:
                for (int i = 0; i < 24; i += 4) {
                    divs[i] = true;
                }
                break;
            default:
                break;
        }
    } else{
         usbMIDI.sendNoteOff(LAYER_NUMBER,127,1); 
    }

    current_beat = (current_beat + 1) % 16;
}
void Layer::sendEnc() {
    for (int i=1; i<4; i++) {
        if (layerEncoders[i].value != previousEncoderValues[i]) {
            usbMIDI.sendControlChange(14+LAYER_NUMBER*4+i, layerEncoders[i].value, 1);
            previousEncoderValues[i] = layerEncoders[i].value;
        }
    }
    
    if (global->mode==Mode::OUT) {
      std::string name=sampleMap[LAYER_NUMBER];
      global->player->set_gain(name,layerEncoders[0].value);
      global->player->set_bitcrusher(name,layerEncoders[1].value);
      global->player->set_dry_wet(name,layerEncoders[2].value);
      global->player->set_filter_frequency(name,layerEncoders[3].value);
    }
}

void Layer::playDiv(int numDiv){
    if (divs[numDiv]){
    int velo_off=global->RAND_VELO>random(100)? map(random(10),0,9,25,50):0;
     velo_off*=random(2)>0?-1:1;
     int velo=constrain(layerEncoders[0].value+velo_off,0,127);
    if (global->mode==Mode::MIDI){
        usbMIDI.sendNoteOff(LAYER_NUMBER,velo,1); 
        usbMIDI.sendNoteOn(LAYER_NUMBER,velo,1); }
      else{
        global->player->set_gain(sampleMap[LAYER_NUMBER],constrain(velo,0,127));
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
