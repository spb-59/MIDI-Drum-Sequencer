
#include "Global.h"

Global::Global(/* args */) {
  initPins();
  readControls(states);
  for (int i=0;i<8;i++){
    layers[i]=new Layer(this,i);
  }
}

void Global::print_states() {
  Serial.println("Ring Buttons:");
  for (int i = 0; i < 16; i++) {
    Serial.print(statusToString(states.ringButtons[i].status));
    Serial.print(i < 15 ? ", " : "\n");
  }

  Serial.println("Layer Buttons:");
  for (int i = 0; i < 8; i++) {
    Serial.print(statusToString(states.layerButtons[i].status));
    Serial.print(i < 7 ? ", " : "\n");
  }

  Serial.println("Pots:");
  for (int i = 0; i < 4; i++) {
    Serial.print(states.pots[i]);
    Serial.print(i < 3 ? ", " : "\n");
  }
}
void Global::update_states(){
    readControls(states);

    //check if layer has changed
    for (int i=0;i<8;i++){
        if(states.layerButtons[i].status!=ButtonStatus::None){
            states.layerButtons[i].status=ButtonStatus::None;
            current_layer=i;
            layers[i]->switchLayer();
        }
    }



    
}

Global::~Global() {}
