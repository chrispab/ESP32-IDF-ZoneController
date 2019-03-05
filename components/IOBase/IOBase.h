#ifndef __IOBASE_H
#define __IOBASE_H

//#include <Arduino.h>

class IOBase
{
protected:
  bool state;
  bool newState;
  bool defaultState;

  int pin;
  long prevStateChangeMillis;
  long onMillis;
  long offMillis;
  float upperOffset;
  float lowerOffset;

public:
  IOBase();
  bool getState();
  bool readState();
  bool hasNewState();
  bool getSpeedState();
  //void control(float , float , bool , long );
};

#endif
