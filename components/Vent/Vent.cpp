#include "Vent.h"
#include "config.h"
#include <Arduino.h>

Vent::Vent()
{
    coolingState = false;
    speedState = false;
    setOnMillis(VENT_ON_MILLIS);
    setOffMillis(VENT_OFF_MILLIS);
}

void Vent::setSpeedState(bool pnewSpeedState)
{
    if (pnewSpeedState != state)
    {
        speedState = pnewSpeedState;
        newSpeedState = true;
    }
}
bool Vent::getSpeedState()
{
    return this->speedState;
}
bool Vent::readSpeedState()
{
    //ensures MQTT pub only sent once per state change since last readState
    newSpeedState = false; //indicate data read and used e.g MQTT pub
    return speedState;
}
bool Vent::hasNewSpeedState()
{
    return newSpeedState;
}
void Vent::control(float currentTemp, float targetTemp, bool lightState, long currentMillis)
{
    speedState = false;

    if (VENT_ADAPTIVE)
    {
        if (lightState)
        {
            speedState = true;
            if (currentTemp > targetTemp + upperOffset)
            {
                coolingState = true;
                prevStateChangeMillis = currentMillis;
            }
            else //is below sp
            {
                coolingState = false;
            }
        }
        else // light is off
        {
            speedState = false;
            if (currentTemp > targetTemp + lowerOffset)
            {
                coolingState = true;
                prevStateChangeMillis = currentMillis;
            }
            else // L off and below lower sp
            {
                coolingState = false;
            }
        }
    }
    //overlay the normal vent background default loop
    if (defaultState) //defaultState is the background vent loop
    {
        if (currentMillis - prevStateChangeMillis >= onMillis)
        {
            //Serial.println("TURNING V OFF");
            this->defaultState = false;
            prevStateChangeMillis = currentMillis;
            //Serial.println(this->state);
            // Serial.println(this->defaultState);
        }
    }
    else
    {
        if (currentMillis - prevStateChangeMillis >= offMillis)
        {
            //Serial.println("TURNING V ON");
            this->defaultState = true;
            prevStateChangeMillis = currentMillis;
            //Serial.println(this->state);
            //Serial.println(this->defaultState);
        }
    }
    setState(this->coolingState || this->defaultState);
}
