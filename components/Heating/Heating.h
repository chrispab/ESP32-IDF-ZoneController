#ifndef __HEATING_H
#define __HEATING_H

//#include <Arduino.h>
#include "IOBase.h"

class Heating: public IOBase
{
private:
  float spOffset;

  float upperOffset;
  float lowerOffset;
  long heatingOnMillis;

public:
  Heating();
  bool getState();
  //bool getSpeedState();
  void control(float , float , bool , long );
};

#endif
