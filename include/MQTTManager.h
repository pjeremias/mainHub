// src/MQTTManager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>

class MQTTManager {
public:
  MQTTManager();
  void begin();
  void loop();

  bool publish(const char* topic, const char* payload, bool retained = false);

private:
  WiFiClient espClient;
  PubSubClient mqttClient;

  const char* mqttServer = "test.mosquitto.org";
  const int mqttPort = 1883;
  const char* mqttClientId = nullptr;  // Will be generated
  const char* statusTopic = "esp32/hub/status";

  unsigned long lastPublishTime = 0;
  const unsigned long publishInterval = 5000;  // Publish every 5 seconds for testing

  void reconnect();
  void generateClientId();
};

#endif  // MQTT_MANAGER_H