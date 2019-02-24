#ifndef __HEATING_H
#define __HEATING_H

//#include <Arduino.h>

class Heating
{
private:
  bool state;
  bool defaultState;
  int pin;
  bool speedState;
  long prevStateChangeMillis;
  long onMillis;
  long offMillis;
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
