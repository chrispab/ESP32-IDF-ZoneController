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
//#include <DHT.h>
//DHT DHT22Sensor;
#include <THSensor.h>
THSensor myTHSensor;

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
//PubSubClient MQTTclient(espClient);

#include "MyPubSubClient.h"
MyPubSubClient MQTTclient(espClient);

long lastMsg = 0;
char msg[50];
long value = 0;

//NTP time stuff
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
// By default 'time.nist.gov' is used with 60 seconds update interval and
// no offset
NTPClient timeClient(ntpUDP);

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
        // Create a random MQTTclient ID
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
void insertVTaskDelay()
{
    static long lastVTaskDelay = -5000;
    long VTaskNnow = millis();

    if (VTaskNnow - lastVTaskDelay > 10)
    {
        vTaskDelay(10);
        lastVTaskDelay = VTaskNnow;
    }
}

bool getAllSensorReadings()
{
    static long lastRead = -5000;
    long THnow = millis();

    if (THnow - lastRead > SENSOR_READ_PERIOD_MS)
    {
        //vTaskDelay(30);
        lastRead = THnow;
        //fresh read all sensors
        myLight.sampleState();       //initiate light level sample aquisition and state update
        myTHSensor.getTemperature(); // initiate get new T and H readings

        if (
            (myLight.hasNewState()) ||
            (myTHSensor.hasNewTemperature()) // ||
            //(*humidity != myTHSensor.getHumidity()))
        )
        {
            return true;
        }
    }
    return false;
}

bool processOPs()
{
    //static bool lastHeaterState = false;
    //static bool lastVentState = false;
    //static bool lastFanState = false;
    //static bool lastVentSpeedState = false;

    long currentMillis = millis();
    float targetTemp;

    myLight.getState() ? targetTemp = TSP_LON : targetTemp = TSP_LOFF;

    myVent.control(myTHSensor.getTemperature(), targetTemp, myLight.getState(), currentMillis);
    digitalWrite(VENT_PIN, myVent.getState());
    digitalWrite(VENT_SPEED_PIN, myVent.getSpeedState());

    //ArduinoOTA.handle();
    myFan.control(currentMillis);
    digitalWrite(FAN_PIN, myFan.getState());
    //speed also
    //ArduinoOTA.handle();
    myHeater.control(myTHSensor.getTemperature(), targetTemp, myLight.getState(), currentMillis);
    digitalWrite(HEATER_PIN, myHeater.getState());
    //ArduinoOTA.handle();
    //check for ant changes
    if ((myHeater.hasNewState()) || (myVent.hasNewState()) || (myFan.hasNewState()) || (myVent.hasNewSpeedState()))
    {
        return 1;
    }
    return 0;
}
boolean MQTTreconnect()
{
    if (MQTTclient.connect("arduinoClient"))
    {
        // Once connected, publish an announcement...
        MQTTclient.publish("outTopic", "hello world");
        // ... and resubscribe
        MQTTclient.subscribe("inTopic");
    }
    return MQTTclient.connected();
}
void updateDisplayClock()
{
    long currentMillis;
    currentMillis = millis();
    int newSecs;
    static int oldSecs = 0;

    newSecs = timeClient.getSeconds();
    if (newSecs != oldSecs)
    {
        oldSecs = newSecs;
        timeClient.update();

        strcpy(msg, timeClient.getFormattedTime().c_str());
        myDisplay.writeLine(5, msg);
        //myDisplay.writeLine(5, msg);
        myDisplay.refresh();
        Serial.println(msg);
    }

    //do 1/4 sec update

    static int oldStep = 0;
    int totalSteps;
    int step;
    int sequence[] = {0x2190, 0x2191, 0x2192, 0x2193};
    //get the last 1000ms - 0 to 999 result
    int last1000ths = millis() % 1000;
    if (last1000ths >= 0 && last1000ths < 250)
    {
        step = 1;
    }
    if (last1000ths >= 250 && last1000ths < 500)
    {
        step = 2;
    }
    if (last1000ths >= 500 && last1000ths < 750)
    {
        step = 3;
    }
    if (last1000ths >= 750 && last1000ths < 1000)
    {
        step = 4;
    }
    if (oldStep != step)
    {
        oldStep = step;
        //Serial.println(timeClient.getFormattedTime());
        sprintf(msg, "%u", step);
        //Serial.println(msg);
        //myDisplay.writeLine(4, sequence[step-1]);
        myDisplay.writeLine(4, msg);
        // myDisplay.drawGlyph(5, 20, 0x2603); /* dec 9731/hex 2603 Snowman */
        // delay(1000);
        myDisplay.refresh();
    }
}

extern "C" int app_main(void)
{
    initArduino(); //required by esp-idf

    //float temperature = 0;
    //float humidity = 0;
    //bool lightState = false;
    char lineBuf[30];
    char readingStr[6];

    //int newSecs = 1;
    //int oldSecs = 0;
    //initESPSys();
    WiFi.begin(MYSSID, MYWIFIPASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        Serial.println("! WiFi connected at STARTUP !.");
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
    myTHSensor.setup(DHTPIN, myTHSensor.AM2302);

    timeClient.begin();
    long lastReconnectAttempt = 0;

    long currentMillis = 0;
    int newSensorReading;
    int mode = CONTROL;
    while (true)
    {
        insertVTaskDelay();

        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.print("!!! WiFi Disconnected !!!");

            Serial.print("Connecting to ");
            Serial.print(MYSSID);
            Serial.println("...");
            WiFi.begin(MYSSID, MYWIFIPASSWORD);

            // if (WiFi.waitForConnectResult() != WL_CONNECTED)
            //     return;
            Serial.println("WiFi connected");
        }

        // if (!MQTTclient.connected())
        // {
        //     reconnect();
        // }
        // MQTTclient.loop();

        if (!MQTTclient.connected())
        {
            long now = millis();
            if (now - lastReconnectAttempt > 5000)
            {
                lastReconnectAttempt = now;
                // Attempt to reconnect
                if (MQTTreconnect())
                {
                    lastReconnectAttempt = 0;
                }
            }
        }
        else
        {
            // Client connected

            MQTTclient.loop();
        }

        if (mode == CONTROL)
        {
            //doControl();
            // ArduinoOTA();
            /* this function will handle incomming chunk of SW, flash and respond sender */
            ArduinoOTA.handle();
            currentMillis = millis();

            newSensorReading = getAllSensorReadings();

            if (myLight.hasNewState())
            {
                MQTTclient.publish("Zone2/LightStatus", myLight.readState() ? "1" : "0");

                sprintf(msg, "%d", myLight.getLightSensor());
                MQTTclient.publish("Zone2/LightSensor", msg);
            }

            if (myTHSensor.hasNewTemperature())
            {
                dtostrf(myTHSensor.readTemperature(), 4, 1, msg);
                MQTTclient.publish("Zone2/TemperatureStatus", msg);

                dtostrf(myTHSensor.readHumidity(), 4, 1, msg);
                MQTTclient.publish("Zone2/HumidityStatus", msg);
            }
            if (true)
            {
                strcpy(lineBuf, "T:");
                strcat(lineBuf, dtostrf(myTHSensor.getTemperature(), 4, 1, readingStr));
                strcat(lineBuf, "\xb0");
                strcat(lineBuf, "C");
                strcat(lineBuf, "  H:");
                strcat(lineBuf, dtostrf(myTHSensor.getHumidity(), 4, 1, readingStr));
                strcat(lineBuf, "%");
                myDisplay.writeLine(1, lineBuf);

                //myDisplay.writeLine(6, TITLE_LINE6);
                myDisplay.refresh();
            }

            //modify ops if 1 or more has changed its op
            bool OPsChanged = processOPs();
            if (OPsChanged || newSensorReading)
            {
                newSensorReading = false;
                strcpy(lineBuf, "H  V  F  S  L  VT");
                myDisplay.writeLine(2, lineBuf);

                strcpy(lineBuf, myHeater.getState() ? "1" : "0");
                strcat(lineBuf, "  ");
                strcat(lineBuf, myVent.getState() ? "1" : "0");
                strcat(lineBuf, "  ");
                strcat(lineBuf, myFan.getState() ? "1" : "0");
                strcat(lineBuf, "  ");
                strcat(lineBuf, myVent.getSpeedState() ? "1" : "0");
                strcat(lineBuf, "  ");
                strcat(lineBuf, myLight.getState() ? "1" : "0");
                strcat(lineBuf, "  ");

                if (myVent.hasNewState())
                {
                    MQTTclient.publish("Zone2/VentStatus", myVent.readState() ? "1" : "0");
                } //sprintf(msg, "%d", lightState);
                if (myFan.hasNewState())
                {
                    MQTTclient.publish("Zone2/FanStatus", myFan.readState() ? "1" : "0");
                }
                if (myHeater.hasNewState())
                {
                    MQTTclient.publish("Zone2/HeaterStatus", myHeater.readState() ? "1" : "0");
                }
                if (myVent.hasNewSpeedState())
                {
                    MQTTclient.publish("Zone2/VentSpeedStatus", myVent.readSpeedState() ? "1" : "0");
                }
                myDisplay.writeLine(3, lineBuf);
                myDisplay.refresh();
            }

            //update display clock

            updateDisplayClock();
        }
        if (mode == TEST)
        {
            testRoutine();
        }
    }
    return 0;
}
