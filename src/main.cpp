#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "HubSystem.h"
#include "DHTSensor.h"
#include "CoolingOutput.h"
#include "MQTTObserver.h"

WiFiManager wifiManager;
MQTTManager mqttManager;
HubSystem hub;
DHTSensor tempSensor(4);
CoolingOutput fridge(24);
MQTTObserver mqttObserver(mqttManager, hub);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Hub Firmware");

  wifiManager.begin();
  mqttManager.begin();
  mqttObserver.begin();

  hub.setObserver(&mqttObserver);

  hub.addSensor("tempSensor1", &tempSensor);
  hub.addOutput("fridge", &fridge);
  hub.linkOutputToSensor("fridge", "tempSensor1");
  
  Serial.println("Setup complete. Proceeding to main loop.");
}

void loop() {
  mqttManager.loop();  // Handle MQTT connection
  hub.loop();  // Handle sensor updates and output actuation
}