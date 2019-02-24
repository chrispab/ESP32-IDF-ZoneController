#ifndef __LIGHT_H
#define __LIGHT_H

#include <Arduino.h>


class Light
{
  private:
    bool state;
    int pin;
    // const String host;
    // const int port;
    // const String user;
    // const String passwd;
    // const int timeout;
    // const bool ssl;
    // WiFiClient *client;
    // String readClient();

  public:
    Light(uint8_t pin);
    bool getLightState();
};

#endif
