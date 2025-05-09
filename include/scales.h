/*
  mtof.h
 
  Created by Sebastian Tomczak, 25 March 2017
 
*/

#ifndef scales_h
#define scales_h

#include "Arduino.h"

class scales_class
{
	public: 
		byte major(byte incoming_note);
		byte minor(byte incoming_note);
};

extern scales_class scales;



const byte scale_minor[] = {
	0, 0, 2, 2, 3, 5, 5, 7, 8, 8, 10, 10
};

const byte scale_major[] = {
	0, 0, 2, 2, 4, 5, 5, 7, 7, 9, 9, 11
};

byte scales_class::minor(byte incoming_note) {
	return ((incoming_note / 12) * 12) + scale_minor[incoming_note % 12];

}

byte scales_class::major(byte incoming_note) {
	return ((incoming_note / 12) * 12) + scale_major[incoming_note % 12];
}

scales_class scales; 


#endif
