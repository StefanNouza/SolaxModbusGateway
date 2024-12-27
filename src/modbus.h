#ifndef SOLAXMODBUS_H
#define SOLAXMODBUS_H

#include "commonlibs.h"
#include "baseconfig.h"
#include "mqtt.h"
#include <vector>
#include <ArduinoJson.h>
#include <StreamUtils.h>
#include <ArduinoQueue.h>
#include <HardwareSerial.h>
#include <iomanip>
#include <sstream>

#define DEBUGMODE

#define JSON_MQTT               // define to get MQTT-data in one big JSON-answer (good for FHEM-based systems)
#define JSON_MSG_BUFFER 4000    // [byte] set up a buffer big enough to handle the JSON-answer

#define RS485_READ_TIMEOUT 850  // [ms] timeout between modbus request and answer, Growatt demands 800ms, so we set 850ms

class modbus {

  typedef struct { 
      String Name;
      String RealName;
      String value;
      String unit;
      bool active;
      String openwb;
  } reg_t;

  typedef struct {
    String command = "";
    std::vector<byte> request; 
  } subscription_t;

  // available inverter register json files
  typedef struct {
    String name;
    String filename;
  } regfiles_t;

  #define RS485Transmit    HIGH
  #define RS485Receive     LOW
  #define DATAISID   (byte) 0x01
  #define DATAISLIVE (byte) 0x02

  public:
    modbus();
    void                    init(bool firstrun);
    void                    LoadJsonConfig(bool firstrun);
    void                    LoadJsonItemConfig();

    void                    loop();

    const String&           GetInverterType()   const {return InverterType.name;}

    void                    enableMqtt(MQTT* object);
    void                    GetInitData(AsyncResponseStream *response);
    void                    GetInitRawData(AsyncResponseStream *response);
    String                  GetInverterSN();
    void                    GetLiveDataAsJson(AsyncResponseStream *response, String action);
    void                    GetRegisterAsJson(AsyncResponseStream *response);
    void                    SetItemActiveStatus(String item, bool newstate);
    void                    ReceiveMQTT(String topic, int msg);

  private:
    uint8_t                 pin_RX;               // Serial Receive pin
    uint8_t                 pin_TX;               // Serial Transmit pin
    uint8_t                 pin_RTS;              // Direction control pin
    uint8_t                 default_pin_RX;       // Serial Receive pin
    uint8_t                 default_pin_TX;       // Serial Transmit pin
    uint8_t                 default_pin_RTS;      // Direction control pin
    
    uint8_t          	      pin_Relay1;           // Pin zum Abfragen des ersten potentialfreien Kontakts
    uint8_t           	    default_pin_Relay1;   // Pin zum Abfragen des ersten potentialfreien Kontakts
    uint8_t           	    pin_Relay2;           // Pin zum Abfragen des zweiten potentialfreien Kontakts
    uint8_t           	    default_pin_Relay2;   // Pin zum Abfragen des zweiten potentialfreien Kontakts
    uint8_t           	    state_Relay1;         // Status des ersten potentialfreien Kontakts
    uint8_t          	      state_Relay2;         // Status des zweiten potentialfreien Kontakts
    bool                    enableRelays;         // enable reading Relays

    uint8_t                 ClientID;             // 0x01
    uint32_t                Baudrate;             // 19200
    uint16_t                TxIntervalLiveData;   // 5
    uint16_t                TxIntervalIdData;     // 3600
    regfiles_t              InverterType;         //Solax-X1
    bool                    enableCrcCheck;       //check CRC of invertr answer
    bool                    enableLengthCheck;    // check correct datframe length

    unsigned long           LastTxLiveData = 0;
    unsigned long           LastTxIdData = 0;
    unsigned long           LastTxInverter = 0;

    int                     BpsFactor = 1;        // factor for transmission-timings depending on the baudrate, "1" at 19200bps

    std::vector<byte>*      DataFrame;            // storing read results as hexdata to parse
    std::vector<reg_t>*     InverterIdData;       // storing readable results
    std::vector<reg_t>*     InverterLiveData;     // storing readable results
    std::vector<regfiles_t>*AvailableInverters;   // available inverters from JSON
    std::vector<subscription_t>* Setters;         // available set Options from JSON register 

    
    MQTT*                   mqtt = NULL;
    
    String                  PrintHex(byte num);
    String                  PrintDataFrame(std::vector<byte>* frame);
    String                  PrintDataFrame(byte* frame, uint8_t len);
    uint16_t                Calc_CRC(uint8_t* message, uint8_t len);
    uint16_t                Calc_CRC(std::vector<byte>* message, uint16_t startpos, uint16_t endpos);
    int                     JsonPosArrayToInt(JsonArray posArray, JsonArray posArray2);
    void                    QueryLiveData();
    void                    QueryIdData();
    void                    QueryQueueToInverter();
    void                    ReceiveReadData();
    bool                    ReceiveSetData(std::vector<byte>* SendHexFrame);
    void                    ParseData();
    void                    LoadInvertersFromJson();
    void                    LoadInverterConfigFromJson();
    void                    GenerateMqttSubscriptions();
    String                  GetMqttSetTopic(String command);
    void                    ChangeRegItem(std::vector<reg_t>* vector, reg_t item);
    void                    LoadRegItems(std::vector<reg_t>* vector, String type);
    String                  MapItem(JsonArray map, String value);
    void                    ReadRelays();

    // inverter config, in sync with register.h ->config
    ArduinoQueue<std::vector<byte>>* ReadQueue;
    ArduinoQueue<std::vector<byte>>* SetQueue;

    std::vector<std::vector<byte>>*  Conf_RequestLiveData;
    std::vector<std::vector<byte>>*  Conf_RequestIdData;
		uint8_t                 Conf_ClientIdPos;
    //uint8_t                 Conf_LiveDataStartsAtPos;
		//uint8_t                 Conf_IdDataStartsAtPos;
		uint8_t                 Conf_LiveDataErrorPos;
		byte                    Conf_LiveDataErrorCode;
		uint8_t                 Conf_IdDataErrorPos;
		byte                    Conf_IdDataErrorCode;
		//uint8_t                 Conf_LiveDataSuccessPos;
		//byte                    Conf_LiveDataSuccessCode;
		//uint8_t                 Conf_IdDataSuccessPos;
		//byte                    Conf_IdDataSuccessCode;
    //byte                    Conf_LiveDataFunctionCode;
    //byte                    Conf_IdDataFunctionCode;
    //uint8_t                 Conf_LiveDataFunctionCodePos;
    //uint8_t                 Conf_IdDataFunctionCodePos;

    bool                    Conf_EnableOpenWBTopic;
    bool                    Conf_EnableSetters;

    byte                    String2Byte(String s);

    std::vector<byte>*      SaveIdDataframe;
    std::vector<byte>*      SaveLiveDataframe;

    HardwareSerial*         RS485Serial;

    #ifdef JSON_MQTT
    String                  mqttJson = ((char *)0); // set up a null-string first, length will be given in constructor
    #endif
};

extern modbus* mb;

#endif
