#include "Global.h"


Global::Global(SamplePlayer* player){
  this->player=player;
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
  
  Serial.println("Encoders:");
  for (int i = 0; i < 4; i++) {
    Serial.print(states.encoders[i].value);
    Serial.print(i < 3 ? ", " : "\n");
  }
}
void Global::update_states(){
    readControls(states);

    bool reset=false;

    //check if layer has changed
    for (int i=0;i<8;i++){
        if(i>=4 && states.layerButtons[i].status==ButtonStatus::Hold){
            if (i==4) NUM_BEATS=1;
            if (i==5) NUM_BEATS=2;
            if (i==6) NUM_BEATS=4;
            if (i==7) NUM_BEATS=8;
            states.layerButtons[i].status=ButtonStatus::None;
            reset=true;
            break;
        }
        if(states.layerButtons[i].status!=ButtonStatus::None && states.layerButtons[i].status!=ButtonStatus::Hold ){
            states.layerButtons[i].status=ButtonStatus::None;
            current_layer=i;
            layers[i]->switchLayer();
        }
    }

    print_states();

    BPM=map(states.pots[0],0,1023,50,350);
    RAND_TIMING=map(states.pots[1],0,1023,0,99);
    RAND_VELO=map(states.pots[2],0,1023,0,99);
    int knob3=map(states.pots[3],0,1023,0,127);

    player->set_amp_gain(knob3);
    // usbMIDI.sendControlChange(7,knob3,1);
   
    


    if (reset){
        resetLayers();
        current_layer=0;
        leds.reset();
        clock_count=-1;
        div_count=-1;

    }


    
}

void Global::resetLayers(){
      for (int i=0;i<8;i++){
    layers[i]->reset();
  }
}

void Global::print_all_layer_encoders() {
  Serial.println("=== All Layer Encoder Values ===");
  for (int i = 0; i < 8; i++) {
    layers[i]->printLayerEncoders();
  }
  Serial.println("==============================");
}

Global::~Global() {}
