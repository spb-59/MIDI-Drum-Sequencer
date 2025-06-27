#include "Reader.h"


Encoder encoderObjs[4] = {
    Encoder(APins[0], BPins[0]),
    Encoder(APins[1], BPins[1]),
    Encoder(APins[2], BPins[2]),
    Encoder(APins[3], BPins[3])
};
long lastRawValues[4] = {0, 0, 0, 0};

int DEBOUNCE = 25;

const char* statusToString(ButtonStatus status) {
  switch (status) {
    case ButtonStatus::Pressed: return "Pressed";
    case ButtonStatus::Double: return "Double";
    case ButtonStatus::Triple: return "Triple";
    case ButtonStatus::Quad: return "Quad";
    case ButtonStatus::Hold: return "Hold";
    default: return "None";
  }
}



CRGB statusToColor(ButtonStatus status) {
  switch (status) {
    case ButtonStatus::Pressed: return CRGB::Green;
    case ButtonStatus::Double: return CRGB::Blue;
    case ButtonStatus::Triple: return CRGB::Purple;
    case ButtonStatus::Quad: return CRGB::Orange;
    case ButtonStatus::Hold: return CRGB::Red;
    default: return CRGB::Black;
  }
}

void ButtonState::update(bool newState, unsigned long now) {
  previous = current;
  current = newState;

  if (current == 0 && previous == 1) {
    if (now - lastPressTime > 400 && status != ButtonStatus::None) {
      pressCount = 0;
      status = ButtonStatus::None;
    } else if (now - lastPressTime > 400) {
      pressCount = 1;
      status = ButtonStatus::Pressed;
    } else {
      pressCount++;
      if (pressCount == 2) status = ButtonStatus::Double;
      else if (pressCount == 3) status = ButtonStatus::Triple;
      else if (pressCount >= 4) status = ButtonStatus::Quad;
    }
    lastPressTime = now;
    lastChangeTime = now;
  }

  if (current == 0 && now - lastChangeTime > 1000)
    status = ButtonStatus::Hold;
}

CRGB* statesToColors(ButtonState* states , int num=16){
  CRGB ret[num];
  for(int i=0;i<num;i++){
    ret[i]=statusToColor(states[i].status);
  }
  return ret;
}
int readMuxBase(int s0, int s1, int s2, int channel) {
  digitalWrite(s0, channel & 1);
  digitalWrite(s1, (channel >> 1) & 1);
  digitalWrite(s2, (channel >> 2) & 1);
  delayMicroseconds(DEBOUNCE);
  return channel;
}

int readMux(int s0, int s1, int s2, int com, int channel) {
  readMuxBase(s0, s1, s2, channel);
  return digitalRead(com);
}

int readMuxAnalog(int s0, int s1, int s2, int com, int channel) {
  readMuxBase(s0, s1, s2, channel);
  return analogRead(com);
}

void scanMux(int s0, int s1, int s2, int com, int* states, int n) {
  for (int i = 0; i < n; i++) states[i] = readMux(s0, s1, s2, com, i);
}

void scanMuxAnalog(int s0, int s1, int s2, int com, int* values, int n) {
  for (int i = 0; i < n; i++) values[i] = readMuxAnalog(s0, s1, s2, com, i);
}
void readControls(ControlsState& state) {
  int layerRaw[8], ringBottomRaw[8], ringTopRaw[8], potsRaw[4];
  scanMux(3, 4, 5, 11, layerRaw, 8);
  scanMux(39, 40, 41, 13, ringBottomRaw, 8);
  scanMux(36, 37, 38, 35, ringTopRaw, 8);
  scanMuxAnalog(0, 1, 2, 23, potsRaw, 4);  

  static const int topMap[8] = {14, 15, 0, 13, 1, 4, 2, 3};
  static const int bottomMap[8] = {6, 7, 8, 5, 9, 12, 10, 11};
  static const int layerMap[8] = {2, 1, 0, 3, 4, 7, 5, 6};
  static const int potMap[4] = {2, 1, 0, 3};
  

  static int filteredPots[4] = {0, 0, 0, 0};

  const float alpha = 0.2; 

  unsigned long now = millis();

  for (int i = 0; i < 8; i++) {
    state.ringButtons[topMap[i]].update(ringTopRaw[i], now);
    state.ringButtons[bottomMap[i]].update(ringBottomRaw[i], now);
    state.layerButtons[layerMap[i]].update(layerRaw[i], now);
  }
  
  for (int i = 0; i < 4; i++) {
    int reversedValue = 1023 - potsRaw[i];
    filteredPots[i] = (alpha * reversedValue) + ((1 - alpha) * filteredPots[i]);
    state.pots[potMap[i]] = filteredPots[i];
  }
  
  // Read encoders
  for (int i = 0; i < 4; i++) {
    long position = encoderObjs[i].read();
    state.encoders[i].update(position, lastRawValues[i], 2);
  }
}

void initPins() {
  // Setup mux control pins as output
  pinMode(0, OUTPUT);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(36, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(38, OUTPUT);
  pinMode(39, OUTPUT);
  pinMode(40, OUTPUT);
  pinMode(41, OUTPUT);
  
  // Setup pins connected to the common pin of the mux as input
  pinMode(11, INPUT_PULLUP);  
  pinMode(35, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(23, INPUT);
  

  for (int i = 0; i < 4; i++) {
    pinMode(APins[i], INPUT_PULLUP);
    pinMode(BPins[i], INPUT_PULLUP);
    encoderObjs[i].write(0);
    lastRawValues[i] = 0;
  }
}


void EncoderState::update(long rawPosition, long& lastRawValue, int sensitivity) {
  int diff = rawPosition - lastRawValue;
  
  if (abs(diff) >= sensitivity) {
    int steps = diff / sensitivity;
    
    int newValue = value + steps;
    value = constrain(newValue, 0, 127);
    
    lastRawValue = rawPosition - (diff % sensitivity);
  }
}
