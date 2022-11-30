#ifndef MQTT_H
#define MQTT_H

#include "commonlibs.h" 
#include <PubSubClient.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <vector>
#include "baseconfig.h"

#ifdef ESP8266
  //#define SetHostName(x) wifi_station_set_hostname(x);
  #define WIFI_getChipId() ESP.getChipId() 
#elif ESP32
  //#define SetHostName(x) WiFi.getHostname(x); --> MQTT.cpp TODO
  #define WIFI_getChipId() (uint32_t)ESP.getEfuseMac()   // Unterschied zu ESP.getFlashChipId() ???
#endif

#if defined(ESP8266) || defined(ESP32)
  #include <functional>
  #define CALLBACK_FUNCTION std::function<void(char*, uint8_t*, unsigned int)> MyCallback
#else
  #define CALLBACK_FUNCTION void (*MyCallback)(char*, uint8_t*, unsigned int)
#endif

class MQTT {

  public:

    MQTT(const char* server, uint16_t port, String basepath, String root);
    void          loop();
    void          Publish_Bool(const char* subtopic, bool b);
    void          Publish_Int(const char* subtopic, int number);
    void          Publish_Float(const char* subtopic, float number);
    void          Publish_String(const char* subtopic, String value);
    void          Publish_IP();
    void          setCallback(CALLBACK_FUNCTION);
    void          disconnect();
    String        GetRoot();
    
    
    const bool&   GetConnectStatusWifi()      const {return ConnectStatusWifi;}
    const bool&   GetConnectStatusMqtt()      const {return ConnectStatusMqtt;}

  private:
    WiFiClient    espClient;
    PubSubClient* mqtt;
    CALLBACK_FUNCTION;
    void          reconnect();
    void          callback(char* topic, byte* payload, unsigned int length);
    
    String        mqtt_root = "";
    String        mqtt_basepath = "";
    unsigned long mqttreconnect_lasttry = 0;
    unsigned long last_keepalive = 0;
    bool          ConnectStatusWifi;
    bool          ConnectStatusMqtt;
  
};

extern MQTT* mqtt;

#endif
