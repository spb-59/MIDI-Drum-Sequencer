/*
  mtof.h
 
  Created by Sebastian Tomczak, 25 March 2017
 
*/

#ifndef mtof_h
#define mtof_h

#include "Arduino.h"

class mtof_class
{
	public: 
		double toFrequency(double incoming_note);
		double toPitch(double incoming_frequency);
};

double base_frequency = 440.0;
double base_pitch = 69.0;

double mtof_class::toFrequency(double incoming_note) {
  return base_frequency * pow (2.0, (incoming_note - base_pitch) / 12.0);
}

double mtof_class::toPitch(double incoming_frequency) {
  return base_pitch + (12.0 * log(incoming_frequency / base_frequency) / log(2));
}

mtof_class mtof; 

extern mtof_class mtof;

#endif