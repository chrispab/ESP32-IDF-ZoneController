#ifndef __THSENSOR_H
#define __THSENSOR_H

#include <Arduino.h>
#include "DHT.h"

class THSensor : public DHT
{
private:
  //bool state;
  //int pin;
  float newTemperature;
  bool newTemperatureFlag;
  float previousTemperature;
  float newHumidity;
  bool newHumidityFlag;
  float previousHumidity;

public:
  //THSensor(uint8_t pin);
  // bool getState();
  //bool sampleState();
  float getTemperature();
  bool hasNewTemperature();
  float readTemperature();

  float getHumidity();
  bool hasNewHumidity();
  float readHumidity();
  //int getLightSensor();
};

#endif //__THSENSOR_H
