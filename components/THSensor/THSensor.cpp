#include "THSensor.h"


float THSensor::getTemperature()
{
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
        Serial.println("TEMP-NAN-");
        return temperature;
    }
    //if its a fresh - diff value from old then flag hasnewstate
    if (newTemperature != previousTemperature)
    {
        temperature = newTemperature;
        newTemperatureFlag = true;
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

bool THSensor::hasNewTemperature()
{
    return newTemperatureFlag;
};



float THSensor::getHumidity()
{
    if (isnan(humidity))
    {
        humidity = previousHumidity;
        return humidity;
    }

    previousHumidity = humidity;
    //take a new reading
    newHumidity = DHT::getHumidity();
    if (isnan(newHumidity))
    {
        Serial.println("HUMI-NAN-");
        return humidity;
    }
    //if its a fresh - diff value from old then flag hasnewstate
    if (newHumidity != previousHumidity)
    {
        humidity = newHumidity;
        newHumidityFlag = true;
   }
    return humidity;
}

//read temp - reset new temp flag if set to indicate new data has been read
float THSensor::readHumidity()
{
    //ensures MQTT pub only sent once per state change since last readState
    newHumidityFlag = false; //indicate data read and used e.g MQTT pub
    return humidity;
}

bool THSensor::hasNewHumidity()
{
    return newHumidityFlag;
};