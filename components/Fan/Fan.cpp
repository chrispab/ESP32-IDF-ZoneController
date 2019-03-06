#include "config.h"
#include "Fan.h"

Fan::Fan()
{
    //state = false;
    prevStateChangeMillis = 0;
    onMillis = FAN_ON_MILLIS;
    offMillis = FAN_OFF_MILLIS;
}
void Fan::control(long currentMillis)
{
    if (getState())
    {
        if (currentMillis - prevStateChangeMillis >= onMillis)
        {
            setState(false);
            prevStateChangeMillis = currentMillis;
        }
    }
    else
    {
        if (currentMillis - prevStateChangeMillis >= offMillis)
        {
            setState(true);
            prevStateChangeMillis = currentMillis;
        }
    }
}

