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
    coolingState = false;
    defaultState = false;
    speedState = false;
    prevStateChangeMillis = millis();
    onMillis = VENT_ON_MILLIS;
    offMillis = VENT_OFF_MILLIS;
}

bool Vent::getState()
{
    return this->coolingState || this->defaultState;
}

bool Vent::getSpeedState()
{
    return this->speedState;
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
            Serial.println("TURNING V OFF");
            this->defaultState = false;
            prevStateChangeMillis = currentMillis;
            Serial.println(this->state);
            Serial.println(this->defaultState);
        }
    }
    else
    {
        if (currentMillis - prevStateChangeMillis >= offMillis)
        {
            Serial.println("TURNING V ON");
            this->defaultState = true;
            prevStateChangeMillis = currentMillis;
            Serial.println(this->state);
            Serial.println(this->defaultState);
        }
    }

    //now OR the states to get final state
    //    this->state = this->state || this->defaultState;
    //this->state = this->defaultState;
    //    this->state = this->defaultState;

    //Serial.println(this->state);
}
