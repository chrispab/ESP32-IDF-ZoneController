#include "Heating.h"
#include "config.h"

Heating::Heating()
{
    //         self.state = OFF
    //         self.heater_off_delta = cfg.getItemValueFromConfig('heater_off_t')  # min time heater is on or off for
    //         self.heater_on_delta = cfg.getItemValueFromConfig('heater_on_t')  # min time heater is on or off for
    //         self.prev_heater_millis = 0  # last time heater switched on or off
    //         self.heater_sp_offset = cfg.getItemValueFromConfig('heater_sp_offset')
    state = false;
    defaultState = false;
    speedState = false;
    prevStateChangeMillis = 0;
    onMillis = HEATING_ON_MILLIS;
    offMillis = HEATING_OFF_MILLIS;
    spOffset = HEATING_SP_OFFSET;
    //offMillis = Heating_OFF_MILLIS;
}

bool Heating::getState()
{
    return this->state;
}

void Heating::control(float currentTemp, float targetTemp, bool lightState, long currentMillis)
{
    onMillis = ((targetTemp - currentTemp) * 80 * 1000) + HEATING_ON_MILLIS;
    // Serial.print("onMillis : ");

    // Serial.println(onMillis);
    if(lightState){
        state = false;
    }
    if (!lightState)
    {
        if (currentTemp >= targetTemp + spOffset) 
        {
            state = false;
            prevStateChangeMillis = currentMillis;
        }
        else if (state)//# t below tsp if time is up, so check if change the state to OFF
        { 
            if (currentMillis - prevStateChangeMillis >= onMillis)
            {
                state = false;
                prevStateChangeMillis = currentMillis;
            }
        }
        else if (currentMillis - prevStateChangeMillis >= offMillis)
        {
            state = true;
            prevStateChangeMillis = currentMillis;
        }
    }
}