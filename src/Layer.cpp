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

void Layer::playBeat(){
    if(beats[current_beat]!=ButtonStatus::None){
        usbMIDI.sendNoteOn(LAYER_NUMBER,127,1);
    }
    else{
          usbMIDI.sendNoteOff(LAYER_NUMBER,127,1); 
    }

    current_beat=(++current_beat)%16;
}
