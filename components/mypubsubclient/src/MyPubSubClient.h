
#ifndef MyPubSubClient_h
#define MyPubSubClient_h

 #include <Arduino.h>
// #include "IPAddress.h"
 #include "Client.h"
// #include "Stream.h"
#include "PubSubClient.h"

class MyPubSubClient : public PubSubClient {
public:
   //MyPubSubClient(WiFiClient& cl);
      MyPubSubClient(Client& client);

   boolean publish(const char* topic, const char* payload);
   // boolean publish(const char* topic, const char* payload, boolean retained);
   // boolean publish(const char* topic, const uint8_t * payload, unsigned int plength);
   // boolean publish(const char* topic, const uint8_t * payload, unsigned int plength, boolean retained);
   // boolean publish_P(const char* topic, const char* payload, boolean retained);
   // boolean publish_P(const char* topic, const uint8_t * payload, unsigned int plength, boolean retained);
};


#endif
