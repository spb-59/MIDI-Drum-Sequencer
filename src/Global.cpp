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
                // Serial.print("Mode changed to: ");
                // Serial.println((mode == Mode::MIDI) ? "MIDI" : "OUT");
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
    
    // Print current MIDI mode status periodically
    static unsigned long lastMidiStatusTime = 0;
    if (millis() - lastMidiStatusTime > 5000) { // Every 5 seconds
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
  
  // First check if we're in MIDI mode
  if (mode != Mode::MIDI) {
    // If we get a MIDI clock but we're not in MIDI mode, let's switch
    mode = Mode::MIDI;
    Serial.println("Detected MIDI clock, switching to MIDI mode");
    external_clock_enabled = true;
  }
  
  // Auto-enable MIDI clock when we receive clock messages
  if (use_midi_clock) {
    external_clock_enabled = true;
  }
  
  // If not enabled, don't process clock messages
  if (!external_clock_enabled || !use_midi_clock) {
    return;
  }
  
  // Process the clock tick
  unsigned long current_time = millis();
  
  // Check if this is the first clock message after start
  if (midi_clock_counter == 23 && !first_clock_after_start) {
    // This is the first clock after a MIDI start command
    first_clock_after_start = true;
    Serial.println("First MIDI clock after start - aligning timing");
  } else {
    first_clock_after_start = false;
  }
  
  // Calculate BPM from timing between clock messages
  if (last_midi_clock_time > 0) {
    unsigned long time_diff = current_time - last_midi_clock_time;
    if (time_diff > 0) {
      midi_bpm = 60000.0f / (time_diff * 24);
    }
  }
  
  last_midi_clock_time = current_time;
  
  // Increment the clock counter
  midi_clock_counter++;
  
  // Keep track of the actual number of MIDI clocks received
  // Adjust clock_counter based on NUM_BEATS to ensure proper playback rate
  // For NUM_BEATS = 1, standard rate (every 24 clocks)
  // For NUM_BEATS = 2, twice as fast (every 12 clocks)
  // For NUM_BEATS = 4, four times as fast (every 6 clocks)
  // For NUM_BEATS = 8, eight times as fast (every 3 clocks)
  int clockDivisor;
  if (NUM_BEATS == 1) clockDivisor = 1;
  else if (NUM_BEATS == 2) clockDivisor = 2;
  else if (NUM_BEATS == 4) clockDivisor = 4;
  else if (NUM_BEATS == 8) clockDivisor = 8;
  else clockDivisor = 4; // Default
  
  const int clocksPerStep = 24;
  
  // Scale the step trigger frequency based on NUM_BEATS
  if (midi_clock_counter % (clocksPerStep / clockDivisor) == 0) {
    int step_num = (midi_clock_counter / (clocksPerStep / clockDivisor)) % 16;
    Serial.print("MIDI Clock Step: ");
    Serial.println(step_num);
    
    for (int i = 0; i < 8; i++) {
      layers[i]->playBeat();
    }
    leds.next_beat();
  }
  
  // Process the divisions (subdivisions of the beat)
  int div_index = midi_clock_counter % 24;
  
  // Add debug print for quarter notes
  if (div_index == 0) {
    Serial.println("MIDI Beat marker (quarter note)");
  }
  
  // Scale the division frequency based on NUM_BEATS
  // This approach ensures divisions are processed at the right rate
  // relative to the current NUM_BEATS setting
  for (int i = 0; i < 8; i++) {
    layers[i]->playDiv((div_index * clockDivisor) % 24);
  }
  
  // Ensure all MIDI messages are sent immediately
  usbMIDI.send_now();
}


void Global::handleMidiStart() {
  Serial.println("MIDI Start received");
  
  if (mode != Mode::MIDI) {
    Serial.println("Not in MIDI mode, ignoring start message");
    return;
  }
  
  // Force mode to MIDI
  mode = Mode::MIDI;
  
  // Enable external clock
  external_clock_enabled = true;
  use_midi_clock = true;
  
  Serial.println("MIDI Start - resetting system");
  
  // Reset all notes first
  for (int i = 0; i < 8; i++) {
    usbMIDI.sendNoteOff(36+i, 0, 1);
  }
  usbMIDI.send_now();
  
  // Initialize the timing system with current time
  // This prevents timing offset when first clock messages arrive
  last_midi_clock_time = millis();
  
  // Set counter to -1 so the first clock tick will bring it to 0
  // This aligns better with DAW expectations where first tick after start = position 0
  midi_clock_counter = 0; // 24-1, so next tick will be beat 0
  clock_count = -1;
  div_count = -1;
  

  // Reset LEDs
  leds.reset();
  
  // Force current layer to 0
  current_layer = 0;
  
  // Let the user know we're ready
  Serial.println("System reset and ready for MIDI clock");
}

void Global::handleMidiStop() {
  Serial.println("MIDI Stop received");
  
  if (mode != Mode::MIDI) {
    Serial.println("Not in MIDI mode, ignoring stop message");
    return;
  }
  
  // Always stop on MIDI stop message when in MIDI mode
  external_clock_enabled = false;
  
  // Turn off all MIDI notes to prevent stuck notes
  Serial.println("MIDI Stop: Turning off all notes");
  for (int i = 0; i < 16; i++) {
    usbMIDI.sendNoteOff(36+i, 0, 1);
  }
  usbMIDI.send_now();
  
  // Reset timing variables for clean restart
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

  // Enable external MIDI clock
  external_clock_enabled = true;
  use_midi_clock = true;
  
  // Don't reset counters, this preserves position
  // But do make sure all notes are off
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
    // When switching to MIDI clock, be ready to sync immediately
    // But don't start until we receive a MIDI Start or Clock message
    external_clock_enabled = false;
  }
  
  Serial.print("Clock source: ");
  Serial.println(use_midi_clock ? "MIDI" : "Internal");
}
