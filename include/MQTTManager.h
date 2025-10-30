// src/MQTTManager.h
#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <map>
#include <functional>

class MQTTManager {
public:
  MQTTManager();
  void begin();
  void loop();

  bool publish(const char* topic, const char* payload, bool retained = false);
  void subscribe(const char* topic, std::function<void(const char* payload)> callback);
  void unsubscribe(const char* topic);

private:
  WiFiClient _espClient;
  PubSubClient _mqttClient;

  const char* mqttServer = "test.mosquitto.org";
  const int mqttPort = 1883;
  const char* mqttClientId = nullptr;  // Will be generated
  const char* statusTopic = "esp32/hub/status";

  unsigned long lastPublishTime = 0;
  const unsigned long publishInterval = 5000;  // Publish every 5 seconds for testing

  std::map<String, std::function<void(const char* payload)>> _subscriptions;

  void reconnect();
  void generateClientId();
};

#endif  // MQTT_MANAGER_H