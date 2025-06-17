#include "Layer.h"
#include "Global.h"


Layer::Layer(Global* g,int i)
{
    global=g;
    LAYER_NUMBER=i;
    for (int i = 0; i < 16; i++) {
        beats[i] = ButtonStatus::None;
    }
    current_beat=0;


}

void Layer::switchLayer(){
for (int i = 0; i < 16; ++i) {
  global->states.ringButtons[i].status = beats[i];
   }
    

    
}
void Layer::update(){
  for (int i = 0; i < 16; ++i) {
 beats[i]= global->states.ringButtons[i].status ;
   }


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


void Layer::playDiv(int numDiv){
    if (divs[numDiv]){
        usbMIDI.sendNoteOff(LAYER_NUMBER,127,1); 
        usbMIDI.sendNoteOn(LAYER_NUMBER,127,1); 
    }
}
