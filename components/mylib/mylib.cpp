#include <version.h>

#include <Display.h>
extern Display myDisplay;

void displayInit(void)
{
    myDisplay.begin();
    myDisplay.setFont(SYS_FONT);
    myDisplay.setFont(FONT1);
    myDisplay.wipe();
    myDisplay.writeLine(1, TITLE_LINE1);
    myDisplay.writeLine(2, TITLE_LINE2);
    myDisplay.writeLine(3, TITLE_LINE3);
    myDisplay.writeLine(4, TITLE_LINE4);
    myDisplay.writeLine(5, TITLE_LINE5);
    //myDisplay.writeLine(6, TITLE_LINE6);
    myDisplay.refresh();
}

///
#include "esp_wifi.h"
#include "secret.h"
#include "esp_event_loop.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "esp_system.h"
#include "nvs_flash.h"

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void initESPSys(void)
{
    nvs_flash_init();
    //system_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

    //(const uint8_t*)

    wifi_config_t sta_config;
    sprintf((char *)sta_config.sta.ssid, MYSSID); //or strcpy also works
    sprintf((char *)sta_config.sta.password, MYWIFIPASSWORD);
    sta_config.sta.bssid_set = false;
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_connect());
} /////

#include "DHT.h"

extern DHT DHT22Sensor;

void testRoutine(void)
{
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    static int level = 0;

    static int counter = 0;

    gpio_set_level(GPIO_NUM_2, level);
    level = !level;
    char s[11];

    sprintf(s, "%d", counter);
    // myDisplay.writeLine(6, s);
    // myDisplay.refresh();
    //doit();

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
} /////////////////////////////////////////////////////////////////////
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