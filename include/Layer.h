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





   
public:
    Layer(Global* g,int i);
    ~Layer();
    void playBeat();
    void switchLayer();
    void update();

};
