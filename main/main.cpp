#include "Arduino.h"

#include "driver/gpio.h"

#include "version.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"

#include "secret.h"

#include "MyOTA.h"
#include "mylib.h"
#include "config.h"

#include "Display.h"
Display myDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/OLED_CLOCK_PIN,
                  /* data=*/OLED_DATA_PIN);
#include "graphics_demo.h"
//Triangle polygon(3, 4, 5);  //<--- example class

#include "control.h"
// create object
// #include "sendemail.h"

// SendEmail e("smtp.gmail.com", 465, GMAILTO, GAPPPASSWORD,
//             5000, true);

// const char *ssid = MYSSID;
// const char *password = MYWIFIPASSWORD;
// DHT22 stuff
// #define DHTPIN GPIO_NUM_25 // LHS_P_8 what digital pin we're connected to
// #include "DHT.h"
// DHT DHT22Sensor;
/* LED */
//int led = 2;
int fanPin = 33;
int ventPin = 27; // 2 is on board led
int heaterPin = 26;
#define VENT_SPEED_PIN GPIO_NUM_27

#define CONTROL 1
#define TEST 2

#include "Light.h"
#define ADC1_CH0 36
Light myLight(ADC1_CH0);

// DHT22 stuff
#define DHTPIN GPIO_NUM_25 // LHS_P_8 what digital pin we're connected to
#include <DHT.h>
DHT DHT22Sensor;

#include "Vent.h"
Vent myVent;

#include "Fan.h"
Fan myFan;

#include "Heating.h"
Heating myHeater;

#include "esp_task_wdt.h"

void readAllSensors(void)
{
        long lastRead = 0;

        float temperature = 0;
    float humidity = 0;
        bool lightState = false;
    int analog_value = 0;
    long THnow = millis();
    if (THnow - lastRead > 4500)
    {
        vTaskDelay(1);
        lastRead = THnow;

        lightState = myLight.getLightState();
        Serial.print("lightState: ");
        Serial.println(lightState);
        analog_value = analogRead(ADC1_CH0);
        Serial.print("lightLevel: ");
        Serial.println(analog_value);
        //myFan.setOnMillis(analog_value);

        temperature = DHT22Sensor.getTemperature();
        Serial.print("Temp: ");
        Serial.println(temperature);
        humidity = DHT22Sensor.getHumidity();
        Serial.print("Humi: ");
        Serial.println(humidity);
    }
}
extern "C" int app_main(void)
{
    initArduino(); //required by esp-idf
    displayInit();
    Serial.begin(115200);
    Serial.println("==========running setup==========");

    initESPSys();
    /* set LED as output */
    pinMode(fanPin, OUTPUT);
    pinMode(ventPin, OUTPUT);
    pinMode(heaterPin, OUTPUT);

    setupOTA();
    DHT22Sensor.setup(DHTPIN, DHT22Sensor.AM2302);



    long currentMillis = 0;

    int mode = CONTROL;
    while (true)
    {
        //  vTaskDelay(1);
        //esp_task_wdt_reset();
        if (mode == CONTROL)
        {
            //doControl();
            // ArduinoOTA();
            /* this function will handle incomming chunk of SW, flash and respond sender */
            ArduinoOTA.handle();
            currentMillis = millis();

            //*************************************************************************
            readAllSensors();
            //REad all sensors and states

            //delay(5000);
            //read sensors
            //get latest I/O states
            float targetTemp;
            myLight.getLightState() ? targetTemp = TSP_LON : targetTemp = TSP_LOFF;

            myVent.control(temperature, targetTemp, lightState, currentMillis);
            digitalWrite(ventPin, myVent.getState());
            ArduinoOTA.handle();

            myFan.control(currentMillis);
            digitalWrite(fanPin, myFan.getState());
            //speed also
            ArduinoOTA.handle();

            myHeater.control(temperature, targetTemp, lightState, currentMillis);
            digitalWrite(heaterPin, myHeater.getState());
            ArduinoOTA.handle();
        }
        if (mode == TEST)
        {
            testRoutine();
        }
    }
    return 0;
}
