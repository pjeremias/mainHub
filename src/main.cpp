#include <Arduino.h>
#include "WiFiManager.h"
#include "MQTTManager.h"
#include "DHTSensor.h"
#include "CoolingOutput.h"

WiFiManager wifiManager;
MQTTManager mqttManager;
DHTSensor dhtSensor(27);
CoolingOutput coolingOutput(14);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Hub Firmware");

  wifiManager.begin();
  mqttManager.begin();
  dhtSensor.begin();
  coolingOutput.begin();
  coolingOutput.setSetPoint(25.0);  // Example set point: 25°C
  coolingOutput.setHysteresis(2.0);  // Example hysteresis: ±2°C

  Serial.println("Setup complete. Proceeding to main loop.");
}

void loop() {
  mqttManager.loop();
  
  // Temporary: Read and publish DHT data every 5 seconds (will move to SystemController)
  static unsigned long lastReadTime = 0;
  if (millis() - lastReadTime > 5000) {
    lastReadTime = millis();
    SensorData data = dhtSensor.readData();
    if (!data.empty()) {
      for (const auto& pair : data) {
        char dataStr[8];
        dtostrf(pair.second, 6, 2, dataStr);
        String topic = "esp32/hub/" + pair.first;
        mqttManager.publish(topic.c_str(), dataStr);
      }

      // Actuate the output based on sensor data
      coolingOutput.actuate(data);
    }
  }
}