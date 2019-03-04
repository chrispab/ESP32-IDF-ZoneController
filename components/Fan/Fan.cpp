#include "config.h"
#include "Fan.h"

Fan::Fan()
{
    state = false;
    prevStateChangeMillis = 0;
    onMillis = FAN_ON_MILLIS;
    offMillis = FAN_OFF_MILLIS;
}
void Fan::control(long currentMillis)
{
    if (state)
    {
        if (currentMillis - prevStateChangeMillis >= onMillis)
        {
            state = false;
            prevStateChangeMillis = currentMillis;
        }
    }
    else
    {
        if (currentMillis - prevStateChangeMillis >= offMillis)
        {
            state = true;
            prevStateChangeMillis = currentMillis;
        }
    }
}

bool Fan::getState()
{
    return this->state;
}

void Fan::setOnMillis(long ponMillis)
{
    onMillis = ponMillis;
}
void Fan::setOffMillis(long poffMillis)
{
    offMillis = poffMillis;
}
