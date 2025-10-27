#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DHTManager.h"

WiFiManager wifiManager;
MQTTManager mqttManager;
DHTManager sensorManager(mqttManager);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Hub Firmware");

  // Initialize WiFi
  wifiManager.begin();
  mqttManager.begin();
  sensorManager.begin();

  Serial.println("Setup complete. Proceeding to main loop.");
}

void loop() {
  mqttManager.loop();
  sensorManager.loop();
}