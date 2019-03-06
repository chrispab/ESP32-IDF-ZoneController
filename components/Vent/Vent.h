#ifndef __VENT_H
#define __VENT_H

#include "IOBase.h"

class Vent : public IOBase
{
private:
  bool coolingState;
  bool speedState;
  bool newSpeedState;
  float upperOffset;
  float lowerOffset;

public:
  Vent();
  bool getSpeedState();
  void setSpeedState(bool newSpeedState);
  bool readSpeedState();
  bool hasNewSpeedState();
  void control(float, float, bool, long);
};

#endif
