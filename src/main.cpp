#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "SystemController.h"
#include "DHTSensor.h"
#include "CoolingOutput.h"

WiFiManager wifiManager;
MQTTManager mqttManager;
SystemController systemController(mqttManager);

DHTSensor dhtSensor(27);  // Pin 27 for DHT22
CoolingOutput coolingOutput(14);  // Example pin 14 for relay/output (change as needed)

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Hub Firmware");

  wifiManager.begin();
  mqttManager.begin();
  
  // This functions should begin the sensors! If not, when adding after init of the controller
  // the sensors won't be initialized.
  systemController.addSensor("dht22", &dhtSensor);
  systemController.addOutput("fridge", &coolingOutput);
  systemController.attachOutputToSensor("fridge", "dht22");
  
  coolingOutput.setSetPoint(25.0);
  coolingOutput.setHysteresis(2.0);
    
  Serial.println("Setup complete. Proceeding to main loop.");
}

void loop() {
  mqttManager.loop();  // Handle MQTT connection
  systemController.loop();  // Handle reading, publishing, and actuation
}