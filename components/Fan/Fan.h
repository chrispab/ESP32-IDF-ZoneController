#ifndef __FAN_H
#define __FAN_H

//#include "config.h"


class Fan
{
  private:
    bool state;
    int pin;
    long currentMillis;
    long prevStateChangeMillis;
    long onMillis;
    long offMillis;

  public:
    Fan();
    void control(long millis);
    bool getState();
    void setOnMillis(long onMillis);
    void setOffMillis(long offMillis);
    
};

#endif
