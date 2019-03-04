#include "IOBase.h"
#include "config.h"
#include <Arduino.h>

IOBase::IOBase()
{
    //         self.IOBase_pulse_active = OFF  # settings.IOBasePulseActive
    //         self.IOBase_pulse_delta = 0  # IOBasePulseDelta
    //         self.IOBase_pulse_on_delta = cfg.getItemValueFromConfig('IOBasePulseOnDelta')
    //         self.IOBase_loff_sp_offset = cfg.getItemValueFromConfig('IOBase_loff_sp_offset')
    //         self.IOBase_lon_sp_offset = cfg.getItemValueFromConfig('IOBase_lon_sp_offset')
    //         self.IOBase_override = OFF  # settings.IOBaseOverride
    state = false;
    newData = false;
    // coolingState = false;
    // defaultState = false;
    // speedState = false;
    prevStateChangeMillis = millis();
    onMillis = 0;
    offMillis = 0;
}

bool IOBase::getState()
{
    return this->state;
}
bool IOBase::hasNewData()
{
    return newData;
}
bool IOBase::readState()
{
    //ensures MQTT pub only sent once per state change since last readState
    newData = false; //indicate data read and used e.g MQTT pub
    return this->state;
}
// bool IOBase::getSpeedState()
// {
//     return this->speedState;
// }
void IOBase::control(float currentTemp, float targetTemp, bool lightState, long currentMillis)
{
    // speedState = false;

    // if (IOBase_ADAPTIVE)
    // {

    //     if (lightState)
    //     {
    //         speedState = true;
    //         if (currentTemp > targetTemp + upperOffset)
    //         {
    //             coolingState = true;
    //             prevStateChangeMillis = currentMillis;
    //         }
    //         else //is below sp
    //         {
    //             coolingState = false;
    //         }
    //     }
    //     else // light is off
    //     {
    //         speedState = false;
    //         if (currentTemp > targetTemp + lowerOffset)
    //         {
    //             coolingState = true;
    //             prevStateChangeMillis = currentMillis;
    //         }
    //         else // L off and below lower sp
    //         {
    //             coolingState = false;
    //         }
    //     }
    // }
    // //overlay the normal IOBase background default loop
    // if (defaultState) //defaultState is the background IOBase loop
    // {
    //     if (currentMillis - prevStateChangeMillis >= onMillis)
    //     {
    //         Serial.println("TURNING V OFF");
    //         this->defaultState = false;
    //         prevStateChangeMillis = currentMillis;
    //         Serial.println(this->state);
    //         Serial.println(this->defaultState);
    //     }
    // }
    // else
    // {
    //     if (currentMillis - prevStateChangeMillis >= offMillis)
    //     {
    //         Serial.println("TURNING V ON");
    //         this->defaultState = true;
    //         prevStateChangeMillis = currentMillis;
    //         Serial.println(this->state);
    //         Serial.println(this->defaultState);
    //     }
}

//now OR the states to get final state
//    this->state = this->state || this->defaultState;
//this->state = this->defaultState;
//    this->state = this->defaultState;

//Serial.println(this->state);
//}
