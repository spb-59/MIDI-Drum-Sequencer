
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

    bool reset=false;

    //check if layer has changed
    for (int i=0;i<8;i++){
        if(states.layerButtons[i].status!=ButtonStatus::None && states.layerButtons[i].status!=ButtonStatus::Hold ){
            states.layerButtons[i].status=ButtonStatus::None;
            current_layer=i;
            layers[i]->switchLayer();
        }
        if(states.layerButtons[i].status==ButtonStatus::Hold){
            if (i==4) NUM_BEATS=1;
            if (i==5) NUM_BEATS=2;
            if (i==6) NUM_BEATS=4;
            if (i==7) NUM_BEATS=8;
            states.layerButtons[i].status=ButtonStatus::None;
            reset=true;

        }
    }

    print_states();

    BPM=map(states.pots[0],0,1023,50,300);



    
}

Global::~Global() {}
