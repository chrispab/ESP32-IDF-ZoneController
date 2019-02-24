#include "Light.h"
////

Light::Light(uint8_t ADC_Pin):
pin(ADC_Pin)
{
    //pin = 
    this->state = false;
}

bool Light::getLightState()
{
    int threshold = 2300;
    int analog_value = analogRead(pin);
    this->state = false;
    if (analog_value > threshold)
    {
        this->state = true;
    }
    return this->state;
}
