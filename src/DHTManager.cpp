#include "DHTManager.h"

DHTManager::DHTManager(MQTTManager& mqttMgr) : mqttManager(mqttMgr), dht(dhtPin, dhtType) {}

void DHTManager::begin() {
  dht.begin();
  Serial.println("DHT22 sensor initialized.");
}

void DHTManager::loop() {
  unsigned long now = millis();
  if (now - lastReadTime > readInterval) {
    lastReadTime = now;
    readAndPublish();
  }
}

void DHTManager::readAndPublish() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();  // Celsius by default

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT22 sensor!");
    return;
  }

  // Convert to strings for publishing
  char tempStr[8];
  dtostrf(temperature, 6, 2, tempStr);  // Format to 2 decimal places
  char humStr[8];
  dtostrf(humidity, 6, 2, humStr);

  // Publish
  mqttManager.publish(tempTopic, tempStr);
  mqttManager.publish(humTopic, humStr);
}