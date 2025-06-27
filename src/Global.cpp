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
            if (i=7) NUM_BEATS=8;
            states.layerButtons[i].status=ButtonStatus::None;
            reset=true;
            break;
        }
<<<<<<< HEAD
                if(i<4 && states.layerButtons[i].status==ButtonStatus::Hold){
            if (i==3) {
            toggleClockSource();
}
            states.layerButtons[i].status=ButtonStatus::None;
            current_layer=0;


=======
        if(i==0 && states.layerButtons[i].status==ButtonStatus::Hold){
            states.layerButtons[i].status=ButtonStatus::None;
            mode=Mode::OUT;
            reset=true;
>>>>>>> 0bf015d (refactor: update timing and gain settings, improve layer handling, and adjust MIDI handling)
            break;
        }
        if(states.layerButtons[i].status!=ButtonStatus::None && states.layerButtons[i].status!=ButtonStatus::Hold ){
            states.layerButtons[i].status=ButtonStatus::None;
            current_layer=i;
            layers[i]->switchLayer();
        }
    }
<<<<<<< HEAD
    
    print_states();
=======

    // print_states();
>>>>>>> 0bf015d (refactor: update timing and gain settings, improve layer handling, and adjust MIDI handling)

    int newBPM = map(states.pots[0],0,1023,50,350);
    if (newBPM != BPM) {
      BPM = newBPM;
    }
    
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

void Global::handleMidiClock() {
  if (mode != Mode::MIDI || !use_midi_clock) {
    external_clock_enabled = false;
    return;
  }
  
  external_clock_enabled = true;
  unsigned long current_time = millis();
  
  if (last_midi_clock_time > 0) {
    unsigned long time_diff = current_time - last_midi_clock_time;
    if (time_diff > 0) {
      midi_bpm = 60000.0f / (time_diff * 24);
    }
  }
  
  last_midi_clock_time = current_time;
  
  midi_clock_counter++;
  
  int pulsesPerStep;
  if (NUM_BEATS == 1) pulsesPerStep = 24;
  else if (NUM_BEATS == 2) pulsesPerStep = 12;
  else if (NUM_BEATS == 4) pulsesPerStep = 6;
  else if (NUM_BEATS == 8) pulsesPerStep = 3;
  else pulsesPerStep = 6;
  
  int currentStep = (midi_clock_counter / pulsesPerStep) % 16;
  int divisionInStep = (midi_clock_counter % pulsesPerStep) * (24 / pulsesPerStep);
  
  if (midi_clock_counter % pulsesPerStep == 0) {
    for (int i = 0; i < 8; i++) layers[i]->playBeat();
    leds.next_beat();
  }
  
  for (int i = 0; i < 8; i++) layers[i]->playDiv(divisionInStep);
}

void Global::handleMidiStart() {
  if (mode != Mode::MIDI || !use_midi_clock) return;
  
  external_clock_enabled = true;
  midi_clock_counter = 0; 
  resetLayers();
  leds.reset();
}

void Global::handleMidiStop() {
  if (mode != Mode::MIDI || !use_midi_clock) return;
  
  external_clock_enabled = false;
  for (int i = 0; i < 8; i++) {
    usbMIDI.sendNoteOff(i, 0, 1);
  }
}

void Global::handleMidiContinue() {
  if (mode != Mode::MIDI || !use_midi_clock) return;
  
  external_clock_enabled = true;
}


void Global::toggleClockSource() {
  use_midi_clock = !use_midi_clock;
  
  if (!use_midi_clock) {
    external_clock_enabled = false;
    current_layer=0;
    clock_count=-1;
    div_count=-1;
    leds.reset();
  }
  
  Serial.print("Clock source: ");
  Serial.println(use_midi_clock ? "MIDI" : "Internal");
}
