#ifndef DHT_MANAGER_H
#define DHT_MANAGER_H

#include <DHT.h>
#include "MQTTManager.h"  // For reference to MQTTManager

class DHTManager {
public:
  DHTManager(MQTTManager& mqttMgr);
  void begin();
  void loop();

private:
  MQTTManager& mqttManager;  // Reference to MQTTManager for publishing

  DHT dht;
  const uint8_t dhtPin = 25;
  const uint8_t dhtType = DHT22;

  const char* tempTopic = "esp32/hub/temp";
  const char* humTopic = "esp32/hub/hum";

  unsigned long lastReadTime = 0;
  const unsigned long readInterval = 5000;

  void readAndPublish();
};

#endif  // DHT_MANAGER_H