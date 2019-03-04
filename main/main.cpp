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

//MQTT
#include <PubSubClient.h>
const char *mqtt_server = "192.168.0.200";
WiFiClient espClient;
PubSubClient MQTTclient(espClient);
long lastMsg = 0;
char msg[50];
long value = 0;

void pubSubCallback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
    {
        Serial.print((char)payload[i]);
    }
    Serial.println();

    // Switch on the LED if an 1 was received as first character
    if ((char)payload[0] == '1')
    {
        //        digitalWrite(BUILTIN_LED, LOW); // Turn the LED on (Note that LOW is the voltage level
        // but actually the LED is on; this is because
        // it is active low on the ESP-01)
        Serial.println("MQTT RX 1");
    }
    else
    {
        //digitalWrite(BUILTIN_LED, HIGH); // Turn the LED off by making the voltage HIGH
        Serial.println("MQTT RX 0");
    }
}

void reconnect()
{
    // Loop until we're reconnected
    while (!MQTTclient.connected())
    {
        Serial.print("Attempting MQTT connection...");
        // Create a random client ID
        String clientId = "ESP32Zone2Cl";
        //clientId += String(random(0xffff), HEX);
        // Attempt to connect
        Serial.print("Trying...");

        if (MQTTclient.connect(clientId.c_str()))
        {
            Serial.println("connected");
            // Once connected, publish an announcement...
            MQTTclient.publish("outTopic", "hello world");
            // ... and resubscribe
            MQTTclient.subscribe("inTopic");
        }
        else
        {
            Serial.print("failed, rc=");
            Serial.print(MQTTclient.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

int getAllSensors(float *temperature, float *humidity, bool *lightState, int *lightSensor)
{
    static long lastRead = 0;
    //int analog_value = 0;
    long THnow = millis();
    if (THnow - lastRead > SENSOR_READ_PERIOD_MS)
    {
        vTaskDelay(1);
        lastRead = THnow;
        //if any changes return 1
        if (
            (*lightState != myLight.getState()) ||
            (*temperature != DHT22Sensor.getTemperature()) ||
            (*humidity != DHT22Sensor.getHumidity()))
        {
            *lightState = myLight.getState();
            *lightSensor = myLight.getLightSensor();
            *temperature = DHT22Sensor.getTemperature();
            *humidity = DHT22Sensor.getHumidity();
            //else return 0
            return 1;
        }
    }
    return 0;
}
bool changeOPs(float temperature, float humidity, bool lightState, long currentMillis)
{
    static bool lastHeaterState = false;
    static bool lastVentState = false;
    static bool lastFanState = false;
    static bool lastVentSpeedState = false;

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
    //check for ant changes
    if ((lastHeaterState != myHeater.getState()) || (lastVentState != myVent.getState()) || (lastFanState != myFan.getState()) || (lastVentSpeedState != myVent.getSpeedState()))
    {
        lastHeaterState = myHeater.getState();
        lastVentState = myVent.getState();
        lastFanState = myFan.getState();
        lastVentSpeedState = myVent.getSpeedState();
        return 1;
    }
    return 0;
}
extern "C" int app_main(void)
{

    initArduino(); //required by esp-idf

    //long lastRead = 0;
    float temperature = 0;
    float humidity = 0;
    bool lightState = false;
    int lightSensor = 0;
    char line1[30];
    char readingStr[6];

    //initESPSys();
    WiFi.begin(MYSSID, MYWIFIPASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    //WiFi.mode(WIFI_STA);
    displayInit();
    Serial.begin(115200);
    Serial.println("==========running setup==========");

    //! MQTT
    IPAddress brokerIP(192, 168, 0, 200);
    MQTTclient.setServer(brokerIP, 1883);
    MQTTclient.setCallback(pubSubCallback);
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
        //! MQTT pubsub bit
        if (!MQTTclient.connected())
        {
            reconnect();
        }
        MQTTclient.loop();

        //long now = millis();
        // if (now - lastMsg > 5000)
        // {
        //     lastMsg = now;
        //     ++value;
        //     snprintf(msg, 50, "hello world #%ld", value);
        //     Serial.print("Publish message: ");
        //     Serial.println(msg);
        //     MQTTclient.publish("outTopic", msg);
        // }

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
                //snprintf(msg, 50, "Zone2/LightStatus/%ld", value);
                sprintf(msg, "%d", lightState);
                MQTTclient.publish("Zone2/LightStatus", msg);

                Serial.print("lightLevel: ");
                Serial.println(lightSensor);
                sprintf(msg, "%d", lightSensor);
                MQTTclient.publish("Zone2/LightSensor", msg);

                Serial.print("Temp: ");
                Serial.println(temperature);
                sprintf(msg, "%f", temperature);
                dtostrf(temperature, 4, 1, msg);
                MQTTclient.publish("Zone2/TemperatureStatus", msg);

                Serial.print("Humi: ");
                Serial.println(humidity); //delay(5000);
                sprintf(msg, "%f", humidity);
                dtostrf(humidity, 4, 1, msg);
                MQTTclient.publish("Zone2/HumidityStatus", msg);

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
                sprintf(msg, "%d", lightState);
                MQTTclient.publish("Zone2/HeaterStatus", myHeater.getState() ? "1" : "0");
                sprintf(msg, "%d", lightState);
                MQTTclient.publish("Zone2/VentStatus", myVent.getState() ? "1" : "0");
                sprintf(msg, "%d", lightState);
                MQTTclient.publish("Zone2/FanStatus", myFan.getState() ? "1" : "0");
                sprintf(msg, "%d", lightState);
                MQTTclient.publish("Zone2/VentSpeedStatus", myVent.getSpeedState() ? "1" : "0");
                //sprintf(msg, "%d", lightState);
                //MQTTclient.publish("Zone2/LightStatus", myLight.getState() ? "1" : "0");

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
