#include "version.h"


#include <Display.h>
extern Display myDisplay;

void displayInit(void)
{
    myDisplay.begin();
    myDisplay.setFont(SYS_FONT);
    myDisplay.wipe();
    myDisplay.writeLine(1, TITLE_LINE1);
    myDisplay.writeLine(2, TITLE_LINE2);
    myDisplay.writeLine(3, TITLE_LINE3);
    myDisplay.writeLine(4, TITLE_LINE4);
    myDisplay.writeLine(5, TITLE_LINE5);
    myDisplay.writeLine(6, TITLE_LINE6);
    myDisplay.refresh();
}

/////////////////////////////////////////////////////////////////////////////
/**
 * @brief currently unused arduino func - moving to esp-idf native funcs
 * 
 */
// void setupArd()
// {
//     Serial.begin(115200);
//     Serial.println("==========running setup==========");

//     /* connect to wifi */
//     WiFi.mode(WIFI_STA);
//     WiFi.begin(ssid, password);
//     /* Wait for connection */
//     while (WiFi.status() != WL_CONNECTED)
//     {
//         delay(500);
//         Serial.print(".");
//     }
//     /* set LED as output */
//     pinMode(fanPin, OUTPUT);
//     pinMode(ventPin, OUTPUT);
//     pinMode(heaterPin, OUTPUT);

//     setupOTA();

//     // Send Email
//     e.send("<cbattisson@gmail.com>", "<cbattisson@gmail.com>", "ESP32 zone controller start",
//            "ESP32 zone controller  started/restarted");

//     myDisplay.begin();
//     myDisplay.setFont(SYS_FONT);
//     myDisplay.wipe();
//     myDisplay.writeLine(1, TITLE_LINE1);
//     myDisplay.writeLine(2, TITLE_LINE2);
//     myDisplay.writeLine(3, TITLE_LINE3);
//     myDisplay.writeLine(4, TITLE_LINE4);
//     myDisplay.writeLine(5, TITLE_LINE5);
//     myDisplay.writeLine(6, TITLE_LINE6);
//     myDisplay.refresh();
//     //delay(500);

//     DHT22Sensor.setup(DHTPIN, DHT22Sensor.AM2302);
// }