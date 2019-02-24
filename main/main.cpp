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

#include "secret.h"

#include "MyOTA.h"
#include "mylib.h"

#include <Display.h>

Display myDisplay(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/OLED_CLOCK_PIN,
                  /* data=*/OLED_DATA_PIN);
#include "graphics_demo.h"
//Triangle polygon(3, 4, 5);  //<--- example class

// DHT22 stuff
#define DHTPIN GPIO_NUM_25 // LHS_P_8 what digital pin we're connected to
#include "DHT.h"
DHT DHT22Sensor;
// create object
// #include "sendemail.h"

// SendEmail e("smtp.gmail.com", 465, GMAILTO, GAPPPASSWORD,
//             5000, true);

// const char *ssid = MYSSID;
// const char *password = MYWIFIPASSWORD;

/* LED */
//int led = 2;
int fanPin = 33;
int ventPin = 2;
int heaterPin = 26;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

extern "C" int app_main(void)
{
    initArduino();
    displayInit();
    // setupArd();
    Serial.begin(115200);
    Serial.println("==========running setup==========");
    DHT22Sensor.setup(GPIO_NUM_25);

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
    sprintf((char *)sta_config.sta.ssid, ssid); //or strcpy also works
    sprintf((char *)sta_config.sta.password, password);
    sta_config.sta.bssid_set = false;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());

    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    int level = 0;

    int counter = 0;
    while (true)
    {
        gpio_set_level(GPIO_NUM_2, level);
        level = !level;
        char s[11];

        sprintf(s, "%d", counter);
        // myDisplay.writeLine(6, s);
        // myDisplay.refresh();
        doit();

        vTaskDelay(DHT22Sensor.getMinimumSamplingPeriod() / portTICK_PERIOD_MS);
        Serial.print(".");
        counter++;

        //delay(DHT22Sensor.getMinimumSamplingPeriod());

        float humidity = DHT22Sensor.getHumidity();
        float temperature = DHT22Sensor.getTemperature();

        Serial.print(DHT22Sensor.getStatusString());
        Serial.print("\t");
        Serial.print(humidity, 1);
        Serial.print("\t\t");
        Serial.print(temperature, 1);
        Serial.print("\t\t");
        Serial.println(DHT22Sensor.toFahrenheit(temperature), 1);
    }
    return 0;
}
