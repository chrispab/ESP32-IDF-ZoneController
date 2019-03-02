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
//int fanPin = 33;
#define FAN_PIN GPIO_NUM_33
//int ventPin = 27; // 2 is on board led
#define VENT_PIN GPIO_NUM_27
#define HEATER_PIN GPIO_NUM_26
#define VENT_SPEED_PIN GPIO_NUM_2

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

int getAllSensors(float *temperature, float *humidity, bool *lightState, int *lightSensor)
{
    static long lastRead = 0;
    //int analog_value = 0;
    long THnow = millis();
    if (THnow - lastRead > 4500)
    {
        vTaskDelay(1);
        lastRead = THnow;

        *lightState = myLight.getState();
        *lightSensor = myLight.getLightSensor();
        *temperature = DHT22Sensor.getTemperature();
        *humidity = DHT22Sensor.getHumidity();
        return 1;
    }
    return 0;
}
bool changeOPs(float temperature, float humidity, bool lightState, long currentMillis)
{
    float targetTemp;
    myLight.getState() ? targetTemp = TSP_LON : targetTemp = TSP_LOFF;
    
    myVent.control(temperature, targetTemp, lightState, currentMillis);
    digitalWrite(VENT_PIN, myVent.getState());
    digitalWrite(VENT_SPEED_PIN, myVent.getSpeedState());

    //ArduinoOTA.handle();
    myFan.control(currentMillis);
    digitalWrite(FAN_PIN, myFan.getState());
    //speed also
    //ArduinoOTA.handle();
    myHeater.control(temperature, targetTemp, lightState, currentMillis);
    digitalWrite(HEATER_PIN, myHeater.getState());
    //ArduinoOTA.handle();
    return 1;
}
extern "C" int app_main(void)
{
    //long lastRead = 0;
    float temperature = 0;
    float humidity = 0;
    bool lightState = false;
    int lightSensor = 0;
    char line1[30];
    char readingStr[6];

    initArduino(); //required by esp-idf
    displayInit();
    Serial.begin(115200);
    Serial.println("==========running setup==========");

    initESPSys();
    /* set LED as output */
    pinMode(FAN_PIN, OUTPUT);
    pinMode(VENT_PIN, OUTPUT);
    pinMode(VENT_SPEED_PIN, OUTPUT);
    pinMode(HEATER_PIN, OUTPUT);

    setupOTA();
    DHT22Sensor.setup(DHTPIN, DHT22Sensor.AM2302);

    long currentMillis = 0;
    int IOChanged;
    int mode = CONTROL;
    while (true)
    {
        if (mode == CONTROL)
        {
            //doControl();
            // ArduinoOTA();
            /* this function will handle incomming chunk of SW, flash and respond sender */
            ArduinoOTA.handle();
            currentMillis = millis();

            IOChanged = getAllSensors(&temperature, &humidity, &lightState, &lightSensor);
            //REad all sensors and states
            if (IOChanged)
            {
                Serial.print("lightState: ");
                Serial.println(lightState);
                Serial.print("lightLevel: ");
                Serial.println(lightSensor);
                Serial.print("Temp: ");
                Serial.println(temperature);
                Serial.print("Humi: ");
                Serial.println(humidity); //delay(5000);
                                          //update display
                //assemble topline
                strcpy(line1, "T:");
                strcat(line1, dtostrf(temperature, 4, 1, readingStr));
                strcat(line1, "\xb0");
                strcat(line1, "C");
                strcat(line1, "  H:");
                strcat(line1, dtostrf(humidity, 4, 1, readingStr));
                strcat(line1, "%");
                myDisplay.writeLine(1, line1);

                //myDisplay.writeLine(6, TITLE_LINE6);
                myDisplay.refresh();
            }
            //modify ops if reqd
            bool OPsChanged = changeOPs(temperature, humidity, lightState, currentMillis);
            if (OPsChanged)
            {
                strcpy(line1, "H  V  F  S  L  VT");
                myDisplay.writeLine(2, line1);

                strcpy(line1, myHeater.getState() ? "1" : "0");
                strcat(line1, "  ");
                strcat(line1, myVent.getState() ? "1" : "0");
                strcat(line1, "  ");
                strcat(line1, myFan.getState() ? "1" : "0");
                strcat(line1, "  ");
                strcat(line1, myVent.getSpeedState() ? "1" : "0");
                strcat(line1, "  ");
                strcat(line1, myLight.getState() ? "1" : "0");
                strcat(line1, "  ");
                // strcpy(line1,myLight.getLightState());
                // strcpy(line1, "  ");

                myDisplay.writeLine(3, line1);

                myDisplay.writeLine(4, "");
                myDisplay.writeLine(5, "");
                //myDisplay.writeLine(6, TITLE_LINE6);
                myDisplay.refresh();
            }
        }
        if (mode == TEST)
        {
            testRoutine();
        }
    }
    return 0;
}
