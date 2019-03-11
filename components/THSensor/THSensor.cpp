#include "THSensor.h"
////

// Light::Light(uint8_t ADC_Pin) : pin(ADC_Pin)
// {
//     state = false;
//     newState = true;
// }
bool THSensor::hasNewTemperature()
{
    return newTemperatureFlag;
};

float THSensor::getTemperature()
{
    previousTemperature = temperature;
    //take a new reading
    newTemperature = DHT::getTemperature();
    //if its a fresh - diff value from old then flag hasnewstate
    if (newTemperature != previousTemperature)
    {
        temperature = newTemperature;
        newTemperatureFlag = true;
        return newTemperature;
    }
    return temperature;
}

//read temp - reset new temp flag if set to indicate new data has been read
float THSensor::readTemperature()
{
    //ensures MQTT pub only sent once per state change since last readState
    newTemperatureFlag = false; //indicate data read and used e.g MQTT pub
    return temperature;
}