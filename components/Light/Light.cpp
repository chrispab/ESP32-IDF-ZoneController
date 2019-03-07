#include "Light.h"
////

Light::Light(uint8_t ADC_Pin) : pin(ADC_Pin)
{
    state = false;
    newState = true;
}

// bool Light::getState()
// {

// }

bool Light::sampleState(){
    int threshold = 500;
    //int sensorValue = getLightSensor();
    bool currentState;

    currentState = (getLightSensor() > threshold);

    if (currentState != state) //new reading value
    {
        newState = true;
        state = currentState;
    
        Serial.print("NEW sample state lightState: ");
        Serial.println(state);
    }
    return state;
}

int Light::getLightSensor()
{
    return analogRead(pin);
}