#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "SystemController.h"
#include "DHTSensor.h"
#include "CoolingOutput.h"
#include <ArduinoJson.h> // delete

WiFiManager wifiManager;
MQTTManager mqttManager;
SystemController systemController(mqttManager);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Hub Firmware");

  wifiManager.begin();
  mqttManager.begin();
  systemController.begin();
  
  JsonDocument doc;
  doc["pin"] = 27;
  systemController.addSensor("tempSensor", "dht22", doc.as<JsonObjectConst>());
  
  JsonDocument outputDoc;
  outputDoc["pin"] = 14;
  systemController.addOutput("fridge", "cooling", outputDoc.as<JsonObjectConst>());
  
  systemController.attachOutputToSensor("fridge", "tempSensor");
    
  Serial.println("Setup complete. Proceeding to main loop.");
}

void loop() {
  mqttManager.loop();  // Handle MQTT connection
  systemController.loop();  // Handle reading, publishing, and actuation
}