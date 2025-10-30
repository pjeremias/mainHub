// src/MQTTManager.cpp
#include "MQTTManager.h"

MQTTManager::MQTTManager() : mqttClient(espClient) {
  // Generate client ID once
  generateClientId();
}

void MQTTManager::generateClientId() {
  static char clientId[20];
  snprintf(clientId, sizeof(clientId), "esp32-hub-%04X", random(0xFFFF));
  mqttClientId = clientId;
}

void MQTTManager::begin() {
  mqttClient.setServer(mqttServer, mqttPort);
  reconnect();  // Initial connection
}

void MQTTManager::reconnect() {
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqttClient.connect(mqttClientId)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void MQTTManager::loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();

  // Publish status message periodically for testing
  unsigned long now = millis();
  if (now - lastPublishTime > publishInterval) {
    lastPublishTime = now;
    if (mqttClient.publish(statusTopic, "connected")) {
      Serial.println("Published status: connected");
    } else {
      Serial.println("Failed to publish status");
    }
  }
}

bool MQTTManager::publish(const char* topic, const char* payload, bool retained) {
  if (!mqttClient.connected()) {
    reconnect();
  }
  bool success = mqttClient.publish(topic, (const uint8_t*)payload, strlen(payload), retained);  // Pass retained flag
  if (success) {
    Serial.printf("Published to %s: %s (retained: %s)\n", topic, payload, retained ? "true" : "false");
  } else {
    Serial.printf("Failed to publish to %s\n", topic);
  }
  return success;
}