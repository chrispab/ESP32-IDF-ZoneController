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
    // Serial.print("get temp temp : ");
    // Serial.println(temperature);
    //delay(1000);

    //check cos gethumidity elsewhere may have updated temp with NAN
    if (isnan(temperature))
    {
        temperature = previousTemperature;
        return temperature;
    }

    previousTemperature = temperature;
    //take a new reading
    newTemperature = DHT::getTemperature();
    if (isnan(newTemperature))
    {
        //newTemperature = previousTemperature;
        Serial.println("-NAN-");
        return temperature;
    }
    //if its a fresh - diff value from old then flag hasnewstate
    if (newTemperature != previousTemperature)
    {
        // Serial.println("FRESH TEMP!!! : ");

        // Serial.print("PREv TEMP : ");
        // Serial.println(previousTemperature);

        // Serial.print("NEW TEMP : ");
        // Serial.println(newTemperature);

        temperature = newTemperature;
        newTemperatureFlag = true;

        //return newTemperature;
    }
    return temperature;
}

//read temp - reset new temp flag if set to indicate new data has been read
float THSensor::readTemperature()
{
    //Serial.println(newTemperatureFlag);
    //delay(1000);

    //ensures MQTT pub only sent once per state change since last readState
    newTemperatureFlag = false; //indicate data read and used e.g MQTT pub
    //Serial.println("FLAG:SET FALSE");
    //delay(1000);

    return temperature;
}