#include "Vent.h"
#include "config.h"

Vent::Vent()
{
    //         self.vent_pulse_active = OFF  # settings.ventPulseActive
    //         self.vent_pulse_delta = 0  # ventPulseDelta
    //         self.vent_pulse_on_delta = cfg.getItemValueFromConfig('ventPulseOnDelta')
    //         self.vent_loff_sp_offset = cfg.getItemValueFromConfig('vent_loff_sp_offset')
    //         self.vent_lon_sp_offset = cfg.getItemValueFromConfig('vent_lon_sp_offset')
    //         self.vent_override = OFF  # settings.ventOverride
    state = false;
    defaultState = false;
    speedState = false;
    prevStateChangeMillis = millis();
    onMillis = VENT_ON_MILLIS;
    offMillis = VENT_OFF_MILLIS;
}

bool Vent::getState()
{
    return this->state;
}

bool Vent::getSpeedState()
{
    return this->speedState;
}
void Vent::control(float currentTemp, float targetTemp, bool lightState, long currentMillis)
{
    speedState = false;
    if (lightState)
    {
        speedState = true;
        if (currentTemp > targetTemp + upperOffset)
        {
            state = true;
            prevStateChangeMillis = currentMillis;
        }
        else //is below sp
        {
            state = false;
        }
    }
    else // light is off
    {
        speedState = false;
        if (currentTemp > targetTemp + lowerOffset)
        {
            state = true;
            prevStateChangeMillis = currentMillis;
        }
        else // L off and below lower sp
        {
            state = false;
        }
    }

    //overlay the normal vent background default loop
    if (!defaultState)//defaultState is the background vent loop
    {
        if (currentMillis - prevStateChangeMillis >= offMillis)
        {
            defaultState = true;
            prevStateChangeMillis = currentMillis;
        }
    }
    else
    {
        if (currentMillis - prevStateChangeMillis >= onMillis)
        {
            defaultState = false;
            prevStateChangeMillis = currentMillis;
        }
    }

    //now OR the states to get final state
    state = state || defaultState;

}

