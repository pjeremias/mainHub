// src/main.cpp
#include <Arduino.h>
#include "WiFiManager.h"
#include "nvs_flash.h"

WiFiManager wifiManager;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting ESP32 Hub Firmware");

  nvs_flash_init();

  // Initialize WiFi
  wifiManager.begin();
  
  // If we reach here, WiFi is connected or configuration is done
  Serial.println("WiFi setup complete. Proceeding to main loop.");
}

void loop() {
  // Main loop for sensors/actuators will go here later
  delay(1000);  // Placeholder
}