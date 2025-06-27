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
        if(i<4 && states.layerButtons[i].status==ButtonStatus::Hold){
            if (i==3) {
                toggleClockSource();
            }
            if (i==0) {
                mode = (mode == Mode::MIDI) ? Mode::OUT : Mode::MIDI;
                reset=true;
            }
            states.layerButtons[i].status=ButtonStatus::None;
            current_layer=0;
            break;
        }
        if(states.layerButtons[i].status!=ButtonStatus::None && states.layerButtons[i].status!=ButtonStatus::Hold ){
            states.layerButtons[i].status=ButtonStatus::None;
            current_layer=i;
            layers[i]->switchLayer();
        }
    }
    
    print_states();

    int newBPM = map(states.pots[0],0,1023,50,350);
    if (newBPM != BPM) {
      BPM = newBPM;
    }
    
    RAND_TIMING=map(states.pots[1],0,1023,0,99);
    RAND_VELO=map(states.pots[2],0,1023,0,99);
    int knob3=map(states.pots[3],0,1023,0,127);

    player->set_amp_gain(knob3);
    
    static unsigned long lastMidiStatusTime = 0;
    if (millis() - lastMidiStatusTime > 5000) {
        Serial.print("Current mode: ");
        Serial.println(mode == Mode::MIDI ? "MIDI" : "OUT");
        Serial.print("MIDI clock enabled: ");
        Serial.println(use_midi_clock ? "YES" : "NO");
        Serial.print("External clock active: ");
        Serial.println(external_clock_enabled ? "YES" : "NO");
        lastMidiStatusTime = millis();
    }

    if (reset){
        resetLayers();
        current_layer=0;
        leds.reset();
        clock_count=-1;
        div_count=-1;
    }


    
}



void Global::resetLayers(){
  Serial.println("Resetting all layers");
  

  
  for (int i = 0; i < 8; i++) {
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
  static unsigned long debug_time = 0;
  static bool first_clock_after_start = false;
  
  if (mode != Mode::MIDI) {
    mode = Mode::MIDI;
    Serial.println("Detected MIDI clock, switching to MIDI mode");
    external_clock_enabled = true;
  }
  
  if (use_midi_clock) {
    external_clock_enabled = true;
  }
  
  if (!external_clock_enabled || !use_midi_clock) {
    return;
  }
  
  unsigned long current_time = millis();
  
  if (midi_clock_counter == 23 && !first_clock_after_start) {
    first_clock_after_start = true;
    Serial.println("First MIDI clock after start - aligning timing");
  } else {
    first_clock_after_start = false;
  }
  
  if (last_midi_clock_time > 0) {
    unsigned long time_diff = current_time - last_midi_clock_time;
    if (time_diff > 0) {
      midi_bpm = 60000.0f / (time_diff * 24);
    }
  }
  
  last_midi_clock_time = current_time;
  
  midi_clock_counter++;
  
  int clockDivisor;
  if (NUM_BEATS == 1) clockDivisor = 1;
  else if (NUM_BEATS == 2) clockDivisor = 2;
  else if (NUM_BEATS == 4) clockDivisor = 4;
  else if (NUM_BEATS == 8) clockDivisor = 8;
  else clockDivisor = 4;
  
  const int clocksPerStep = 24;
  
  if (midi_clock_counter % (clocksPerStep / clockDivisor) == 0) {
    int step_num = (midi_clock_counter / (clocksPerStep / clockDivisor)) % 16;
    Serial.print("MIDI Clock Step: ");
    Serial.println(step_num);
    
    for (int i = 0; i < 8; i++) {
      layers[i]->playBeat();
    }
    leds.next_beat();
  }
  
  int div_index = midi_clock_counter % 24;
  
  if (div_index == 0) {
    Serial.println("MIDI Beat marker (quarter note)");
  }
  
  for (int i = 0; i < 8; i++) {
    layers[i]->playDiv((div_index * clockDivisor) % 24);
  }
  
  usbMIDI.send_now();
}


void Global::handleMidiStart() {
  Serial.println("MIDI Start received");
  
  if (mode != Mode::MIDI) {
    Serial.println("Not in MIDI mode, ignoring start message");
    return;
  }
  
  mode = Mode::MIDI;
  
  external_clock_enabled = true;
  use_midi_clock = true;
  
  Serial.println("MIDI Start - resetting system");
  
  for (int i = 0; i < 8; i++) {
    usbMIDI.sendNoteOff(36+i, 0, 1);
  }
  usbMIDI.send_now();
  
  last_midi_clock_time = millis();
  
  midi_clock_counter = 0;
  clock_count = -1;
  div_count = -1;  leds.reset();
  
  current_layer = 0;
  
  Serial.println("System reset and ready for MIDI clock");
}

void Global::handleMidiStop() {
  Serial.println("MIDI Stop received");
  
  if (mode != Mode::MIDI) {
    Serial.println("Not in MIDI mode, ignoring stop message");
    return;
  }
  
  external_clock_enabled = false;
  
  Serial.println("MIDI Stop: Turning off all notes");
  for (int i = 0; i < 16; i++) {
    usbMIDI.sendNoteOff(36+i, 0, 1);
  }
  usbMIDI.send_now();
  
  last_midi_clock_time = 0;
  
  resetLayers();
  leds.reset();
}

void Global::handleMidiContinue() {
  Serial.println("MIDI Continue received");
  
  if (mode != Mode::MIDI) {
    Serial.println("Not in MIDI mode, switching to MIDI mode");
    mode = Mode::MIDI;
  }

  external_clock_enabled = true;
  use_midi_clock = true;
  
  for (int i = 0; i < 8; i++) {
    usbMIDI.sendNoteOff(36+i, 0, 1);
  }
  usbMIDI.send_now();
  
  Serial.println("MIDI Continue: Ready to continue playback");
}


void Global::toggleClockSource() {
  use_midi_clock = !use_midi_clock;
  
  if (!use_midi_clock) {
    external_clock_enabled = false;
    clock_count = -1;
    div_count = -1;
  } else {
    external_clock_enabled = false;
  }
  
  Serial.print("Clock source: ");
  Serial.println(use_midi_clock ? "MIDI" : "Internal");
}
