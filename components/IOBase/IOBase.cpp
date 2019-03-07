#include "IOBase.h"
#include "config.h"

IOBase::IOBase()
{
    state = false;
    newState = false;
    defaultState = false;
    prevStateChangeMillis = millis();
    onMillis = 0;
    offMillis = 0;
}

bool IOBase::getState()
{
    return state;
}
void IOBase::setState(bool pnewState)
{
    if (pnewState != state)
    {
        state = pnewState;
        newState = true;
    }
}
bool IOBase::hasNewState()
{
    return newState;
}
bool IOBase::readState()
{
    //ensures MQTT pub only sent once per state change since last readState
    newState = false; //indicate data read and used e.g MQTT pub
    return state;
}

void IOBase::setOnMillis(long ponMillis)
{
    onMillis = ponMillis;
}
void IOBase::setOffMillis(long poffMillis)
{
    offMillis = poffMillis;
}
