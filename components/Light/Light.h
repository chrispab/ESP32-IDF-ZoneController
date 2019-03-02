#ifndef __LIGHT_H
#define __LIGHT_H

#include <Arduino.h>


class Light
{
  private:
    bool state;
    int pin;

  public:
    Light(uint8_t pin);
    bool getState();
    int getLightSensor();

};

#endif
