#include "Layer.h"
#include "Global.h"
#include <string>
std::string sampleMap[] = {"KICK", "SNARE", "HAT", "HAT2", "BOP", "CHORD1", "CHORD2", "BACK"};


Layer::Layer(Global* g,int i)
{
    global=g;
    LAYER_NUMBER=36+i; //36
    for (int i = 0; i < 16; i++) {
        beats[i] = ButtonStatus::None;
    }
    current_beat=0;
    
    for (int i = 0; i < 24; i++) {
        divs[i] = false;
    }
    
    for (int i = 0; i < 4; i++) {
        layerEncoders[i].value = 32;
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
    Serial.print("Resetting layer ");
    Serial.println(LAYER_NUMBER - 36);
    
    // Reset all beats
    for (int i = 0; i < 16; ++i) {
      beats[i] = ButtonStatus::None;
    }
   
    // Clear all division markers
    for (int i = 0; i < 24; i++) {
      divs[i] = false;
    }
   
    // Reset beat position
    current_beat = 0;
   
    // Make sure any playing notes are turned off
    if (global->mode == Mode::MIDI) {
      usbMIDI.sendNoteOff(LAYER_NUMBER, 0, 1);
      usbMIDI.send_now();
    }
    
    // Set default encoder values if needed
    if (layerEncoders[0].value < 1 || layerEncoders[0].value > 127) {
      layerEncoders[0].value = 64; // Default velocity to middle value
    }
    
    // Reset previous encoder values
    for (int i = 0; i < 4; i++) {
      previousEncoderValues[i] = layerEncoders[i].value;
    }
    
    sendEnc(); // Send encoder values via MIDI
}
void Layer::playBeat() {
    // First, clear all 24 divisions
    for (int i = 0; i < 24; i++) {
        divs[i] = false;
    }
    
    // Debug the current beat with corrected layer number
    Serial.print("Layer ");
    Serial.print(LAYER_NUMBER - 36);
    Serial.print(" Beat ");
    Serial.print(current_beat);
    Serial.print(" Status: ");
    Serial.println((int)beats[current_beat]);

    // Set up new divisions based on the current beat
    if (beats[current_beat] != ButtonStatus::None) {
        switch (beats[current_beat]) {
            case ButtonStatus::Pressed:
                divs[0] = true;
                Serial.println("  Setting division 0");
                break;
            case ButtonStatus::Double:
                divs[0] = true;
                divs[12] = true;
                Serial.println("  Setting divisions 0, 12");
                break;
            case ButtonStatus::Triple:
                divs[0] = true;
                divs[8] = true;
                divs[16] = true;
                Serial.println("  Setting divisions 0, 8, 16");
                break;
            case ButtonStatus::Quad:
                divs[0] = true;
                divs[6] = true;
                divs[12] = true;
                divs[18] = true;
                Serial.println("  Setting divisions 0, 6, 12, 18");
                break;
            case ButtonStatus::Hold:
                for (int i = 0; i < 24; i += 4) {
                    divs[i] = true;
                }
                Serial.println("  Setting divisions every 4th step");
                break;
            default:
                break;
        }
        
        // For active beats, we'll immediately trigger division 0
        // This ensures that notes play immediately on the beat
        if (divs[0] && global->mode == Mode::MIDI) {
            int velo = constrain(layerEncoders[0].value, 1, 127);
            Serial.print("  Immediate trigger: Note ");
            Serial.print(LAYER_NUMBER);
            Serial.print(" Velocity ");
            Serial.println(velo);
            
            usbMIDI.sendNoteOff(LAYER_NUMBER, 0, 1);
            usbMIDI.sendNoteOn(LAYER_NUMBER, velo, 1);
            usbMIDI.send_now();
            
            // We've handled division 0, so clear it
            divs[0] = false;
        }
    } else {
        // Make sure note is off for empty beats
        if (global->mode == Mode::MIDI) {
            usbMIDI.sendNoteOff(LAYER_NUMBER, 0, 1);
            usbMIDI.send_now();
            Serial.print("  No beat - sending Note Off for layer ");
            Serial.println(LAYER_NUMBER - 36);
        }
    }

    // Move to next beat
    current_beat = (current_beat + 1) % 16;
}
void Layer::sendEnc() {


    if (global->mode==Mode::OUT) {
      std::string name=sampleMap[LAYER_NUMBER-36];
      global->player->set_gain(name,layerEncoders[0].value);
      global->player->set_bitcrusher(name,layerEncoders[1].value);
      global->player->set_dry_wet(name,layerEncoders[2].value);
      global->player->set_filter_frequency(name,layerEncoders[3].value);
    }else{

    for (int i=1; i<4; i++) {
        if (layerEncoders[i].value != previousEncoderValues[i]) {
            usbMIDI.sendControlChange(14+LAYER_NUMBER*4+i, layerEncoders[i].value, 1);
            previousEncoderValues[i] = layerEncoders[i].value;
        }
    }
    
    }
}

void Layer::playDiv(int numDiv){
    // Check if this division should play a note
    if (divs[numDiv]){
        int velo_off = (int)global->RAND_VELO > (int)random(100) ? map(random(10), 0, 9, 25, 50) : 0;
        velo_off *= (int)random(2) > 0 ? -1 : 1;
        int velo = constrain(layerEncoders[0].value + velo_off, 0, 127);
        
        if (global->mode == Mode::MIDI) {
            // Debug print to verify MIDI notes are sent
            Serial.print("MIDI Note: Layer ");
            Serial.print(LAYER_NUMBER - 36);  // Subtract 36 to show layer number 0-7 instead of MIDI note number
            Serial.print(" (Note ");
            Serial.print(LAYER_NUMBER);
            Serial.print("), Velocity: ");
            Serial.println(velo);
            
            // Send MIDI messages
            usbMIDI.sendNoteOff(LAYER_NUMBER, 0, 1);
            usbMIDI.sendNoteOn(LAYER_NUMBER, velo, 1);
            
            // Force immediate send
            usbMIDI.send_now();
        } else {
            int sampleIndex = LAYER_NUMBER - 36;
            if (sampleIndex >= 0 && sampleIndex < 8) {
                global->player->set_gain(sampleMap[sampleIndex], constrain(velo, 0, 127));
                global->player->trigger(sampleMap[sampleIndex]);
            }
        }
        
        // Don't reset the division flag yet - let it be reset by playBeat on the next step
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
