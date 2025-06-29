#pragma once
#include "Reader.h"
class Global; 



class Layer
{
private:
  int LAYER_NUMBER;
  Global* global;
  int current_beat;
  ButtonStatus beats[16];
  bool divs[24] = {false};
  EncoderState layerEncoders[4];
  int previousEncoderValues[4] = {64, 64, 64, 64};
   
public:
    Layer(Global* g,int i);
    ~Layer();
    void playBeat();
    void switchLayer();
    void update();
    void playDiv(int numDiv);
    void reset();
    void printLayerEncoders();
    void sendEnc();

};
