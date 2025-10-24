#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <WebServer.h>
#include <Preferences.h>

class WiFiManager {
public:
  WiFiManager();
  void begin();

private:
  Preferences preferences;
  WebServer server;

  const char* defaultSSID = "default_ssid";  // Replace with your default SSID
  const char* defaultPassword = "default_password";  // Replace with your default password

  const char* apSSID = "ESP32-Config";
  const char* apPassword = nullptr;  // Open AP for simplicity, or set a password

  const unsigned long connectionTimeout = 10000;  // 10 seconds to connect to WiFi
  const unsigned long apTimeout = 300000;  // 5 minutes for AP mode

  String storedSSID;
  String storedPassword;

  bool loadCredentials();
  void saveCredentials(const String& ssid, const String& password);
  bool connectToWiFi(const String& ssid, const String& password);
  void startAPMode();
  void handleRoot();
  void handleSave();
  void handleNotFound();
  String generateNetworkList();
};

#endif  // WIFI_MANAGER_H