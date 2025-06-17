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
  bool divs[4]={false,false,false,false};
  EncoderState layerEncoders[4];
   
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
