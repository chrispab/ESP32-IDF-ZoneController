#include "MyPubSubClient.h"
#include "Arduino.h"

MyPubSubClient::MyPubSubClient(Client &client) : PubSubClient(client)
{
    // this->_state = MQTT_DISCONNECTED;
    // setClient(client);
    // this->stream = NULL;
}
boolean MyPubSubClient::publish(const char *topic, const char *payload)
{
    Serial.print("PUB MQTT : ");

    Serial.print(topic);
    Serial.print(" : ");
    Serial.println(payload);
    return PubSubClient::publish(topic, (const uint8_t *)payload, strlen(payload), false);
}