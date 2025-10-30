// src/MQTTManager.cpp
#include "MQTTManager.h"

MQTTManager::MQTTManager() : _mqttClient(_espClient) {
  // Generate client ID once
  generateClientId();
}

void MQTTManager::generateClientId() {
  static char clientId[20];
  snprintf(clientId, sizeof(clientId), "esp32-hub-%04X", random(0xFFFF));
  mqttClientId = clientId;
}

void MQTTManager::begin() {
  _mqttClient.setServer(mqttServer, mqttPort);
  _mqttClient.setCallback([this](char* topic, uint8_t* payload, unsigned int length) {
    payload[length] = '\0';  // Null-terminate payload
    String strTopic = String(topic);
    auto it = _subscriptions.find(strTopic);
    if (it != _subscriptions.end()) {
        it->second(reinterpret_cast<char*>(payload));
    } else {
        Serial.printf("No callback for topic: %s\n", topic);
    }
  });
  reconnect();  // Initial connection
}

void MQTTManager::reconnect() {
  while (!_mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (_mqttClient.connect(mqttClientId)) {
      Serial.println("connected");
      // Resubscribe to all stored topics
      for (const auto& pair : _subscriptions) {
        _mqttClient.subscribe(pair.first.c_str());
        Serial.printf("Resubscribed to %s\n", pair.first.c_str());
      }
    } else {
      Serial.print("failed, rc=");
      Serial.print(_mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void MQTTManager::loop() {
  if (!_mqttClient.connected()) {
    reconnect();
  }
  _mqttClient.loop();

  // Publish status message periodically for testing
  unsigned long now = millis();
  if (now - lastPublishTime > publishInterval) {
    lastPublishTime = now;
    if (_mqttClient.publish(statusTopic, "connected")) {
      Serial.println("Published status: connected");
    } else {
      Serial.println("Failed to publish status");
    }
  }
}

bool MQTTManager::publish(const char* topic, const char* payload, bool retained) {
  if (!_mqttClient.connected()) {
    reconnect();
  }
  bool success = _mqttClient.publish(topic, (const uint8_t*)payload, strlen(payload), retained);  // Pass retained flag
  if (success) {
    Serial.printf("Published to %s: %s (retained: %s)\n", topic, payload, retained ? "true" : "false");
  } else {
    Serial.printf("Failed to publish to %s\n", topic);
  }
  return success;
}

void MQTTManager::subscribe(const char* topic, std::function<void(const char* payload)> callback) {
  String strTopic = String(topic);
  if (_subscriptions.find(strTopic) != _subscriptions.end()) {
    Serial.printf("Topic '%s' already subscribed. Skipping new subscription.\n", topic);
    return;  // Prevent overwrite or duplicate
  }

  _subscriptions[strTopic] = callback;
  if (_mqttClient.connected()) {
    if (_mqttClient.subscribe(topic)) {
      Serial.printf("Subscribed to %s\n", topic);
    } else {
      Serial.printf("Failed to subscribe to %s\n", topic);
    }
  } else {
    Serial.printf("Queued subscribe to %s (will apply on reconnect)\n", topic);
  }
}

void MQTTManager::unsubscribe(const char* topic) {
  String strTopic = String(topic);
  auto it = _subscriptions.find(strTopic);
  if (it != _subscriptions.end()) {
    _subscriptions.erase(it);
    if (_mqttClient.connected()) {
      _mqttClient.unsubscribe(topic);
      Serial.printf("Unsubscribed from %s\n", topic);
    }
  }
}