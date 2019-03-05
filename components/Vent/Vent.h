#ifndef __VENT_H
#define __VENT_H

#include <Arduino.h>
#include "IOBase.h"

class Vent: public IOBase
{
private:
  //bool state;
  bool coolingState;
  //bool defaultState;
  //int pin;
  bool speedState;
  //long prevStateChangeMillis;
  //long onMillis;
  //long offMillis;
  float upperOffset;
  float lowerOffset;

public:
  Vent();
  bool getState();
  bool getSpeedState();
  void control(float , float , bool , long );
};

#endif
