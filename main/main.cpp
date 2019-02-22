#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "Arduino.h"

#include "version.h"

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "config.h"

#include "version.h"
#include "secret.h"

#include "MyOTA.h"

#include <Display.h>
#define SYS_FONT u8g2_font_6x12_tf       // 7 px high
#define BIG_TEMP_FONT u8g2_font_fub30_tf //30px hieght

#define OLED_CLOCK_PIN GPIO_NUM_22 //RHS_P_14 SCL
#define OLED_DATA_PIN GPIO_NUM_21  //RHS_P_11 SDA
Display myDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/OLED_CLOCK_PIN,
                  /* data=*/OLED_DATA_PIN);
//Triangle polygon(3, 4, 5);  //<--- example class

// DHT22 stuff
#define DHTPIN GPIO_NUM_25 // LHS_P_8 what digital pin we're connected to
#include "DHT.h"
DHT DHT22Sensor;

// create object
#include "sendemail.h"

SendEmail e("smtp.gmail.com", 465, GMAILTO, GAPPPASSWORD,
            5000, true);

const char *ssid = MYSSID;
const char *password = MYWIFIPASSWORD;

/* LED */
//int led = 2;
int fanPin = 33;
int ventPin = 2;
int heaterPin = 26;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

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

void setupArd()
{
    Serial.begin(115200);
    Serial.println("==========running setup==========");
    /* connect to wifi */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    /* Wait for connection */
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    /* set LED as output */
    pinMode(fanPin, OUTPUT);
    pinMode(ventPin, OUTPUT);
    pinMode(heaterPin, OUTPUT);

    setupOTA();

    // Send Email
    e.send("<cbattisson@gmail.com>", "<cbattisson@gmail.com>", "ESP32 zone controller start",
           "ESP32 zone controller  started/restarted");

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
    //delay(500);

    DHT22Sensor.setup(DHTPIN, DHT22Sensor.AM2302);
}


extern "C" int app_main(void)
{
    initArduino();
    displayInit();
   // setupArd();
    Serial.begin(115200);
    Serial.println("==========running setup==========");

    nvs_flash_init();
    system_init();
     tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    //(const uint8_t*)

    wifi_config_t sta_config;
    sprintf((char *)sta_config.sta.ssid, "notwork"); //or strcpy also works
    sprintf((char *)sta_config.sta.password, "a new router can solve many problems");
    sta_config.sta.bssid_set = false;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());


    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    int level = 0;

    while (true)
    {
        gpio_set_level(GPIO_NUM_2, level);
        level = !level;
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        Serial.print(".");
    }
    return 0;
}
