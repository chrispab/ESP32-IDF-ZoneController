#include "Heating.h"
#include "config.h"

Heating::Heating()
{
    onMillis = HEATING_ON_MILLIS;
    offMillis = HEATING_OFF_MILLIS;
    spOffset = HEATING_SP_OFFSET;
}

void Heating::control(float currentTemp, float targetTemp, bool lightState, long currentMillis)
{
    onMillis = ((targetTemp - currentTemp) * 80 * 1000) + HEATING_ON_MILLIS;
    if (lightState)
    {
        setState(false);
    }
    if (!lightState)
    {
        if (currentTemp >= targetTemp + spOffset)
        {
            setState(false);
            prevStateChangeMillis = currentMillis;
        }
        else if (state) //# t below tsp if time is up, so check if change the state to OFF
        {
            if (currentMillis - prevStateChangeMillis >= onMillis)
            {
                setState(false);
                prevStateChangeMillis = currentMillis;
            }
        }
        else if (currentMillis - prevStateChangeMillis >= offMillis)
        {
            setState(true);
            prevStateChangeMillis = currentMillis;
        }
    }
}