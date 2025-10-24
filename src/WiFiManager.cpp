#include "WiFiManager.h"

WiFiManager::WiFiManager() : server(80) {

}

void WiFiManager::begin() {
  // Load stored credentials or use defaults if none stored
  if (!loadCredentials()) {
    storedSSID = defaultSSID;
    storedPassword = defaultPassword;
  }

  // Attempt to connect with stored/default credentials
  if (connectToWiFi(storedSSID, storedPassword)) {
    Serial.println("Connected to WiFi successfully.");
    return;
  }

  // If connection fails, start AP mode for configuration
  Serial.println("Failed to connect to WiFi. Starting AP mode for configuration.");
  startAPMode();
}

bool WiFiManager::loadCredentials() {
  if (!preferences.begin("wifi-creds", false)) {
    Serial.println("Failed to open preferences for loading.");
    return false;
  }
  storedSSID = preferences.getString("ssid", "");
  storedPassword = preferences.getString("password", "");

  preferences.end();

  return !storedSSID.isEmpty() && !storedPassword.isEmpty();
}

void WiFiManager::saveCredentials(const String& ssid, const String& password) {
  if (!preferences.begin("wifi-creds", false)) {
    Serial.println("Failed to open preferences for saving.");
    return;
  }

  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  
  preferences.end();  // Commit changes
}

bool WiFiManager::connectToWiFi(const String& ssid, const String& password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), password.c_str());

  unsigned long startTime = millis();
  while (millis() - startTime < connectionTimeout) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected to ");
      Serial.println(ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connection failed.");
  return false;
}

void WiFiManager::startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(apSSID, apPassword);

  Serial.print("AP started. IP address: ");
  Serial.println(WiFi.softAPIP());

  // Set up web server routes
  server.on("/", HTTP_GET, [this]() { handleRoot(); });
  server.on("/save", HTTP_POST, [this]() { handleSave(); });
  server.onNotFound([this]() { handleNotFound(); });

  server.begin();
  Serial.println("Web server started.");

  // Run AP mode with timeout
  unsigned long startTime = millis();
  while (millis() - startTime < apTimeout) {
    server.handleClient();
    delay(10);  // Small delay to allow handling clients
  }

  // After timeout, stop server and AP
  server.stop();
  WiFi.mode(WIFI_OFF);
  Serial.println("AP mode timeout reached. Restarting WiFi setup.");

  // Recursively retry the begin process (or handle as needed)
  begin();
}

void WiFiManager::handleRoot() {
  String html = R"(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 WiFi Configuration</title>
      <style>
        body { font-family: Arial, sans-serif; max-width: 600px; margin: 0 auto; padding: 20px; }
        h1 { text-align: center; }
        form { display: flex; flex-direction: column; gap: 10px; }
        label { font-weight: bold; }
        select, input { padding: 8px; font-size: 16px; }
        button { padding: 10px; background-color: #4CAF50; color: white; border: none; cursor: pointer; }
        button:hover { background-color: #45a049; }
      </style>
    </head>
    <body>
      <h1>Configure WiFi</h1>
      <p>Select a network and enter the password if required.</p>
      <form action="/save" method="post">
        <label for="ssid">Available Networks:</label>
        <select id="ssid" name="ssid">
          )" + generateNetworkList() + R"(
        </select>
        <label for="password">Password:</label>
        <input type="password" id="password" name="password" required>
        <button type="submit">Save and Connect</button>
      </form>
    </body>
    </html>
  )";

  server.send(200, "text/html", html);
}

void WiFiManager::handleSave() {
  if (server.hasArg("ssid") && server.hasArg("password")) {
    String newSSID = server.arg("ssid");
    String newPassword = server.arg("password");

    saveCredentials(newSSID, newPassword);
    server.send(200, "text/html", "<h1>Credentials saved. Restarting...</h1>");

    delay(1000);
    ESP.restart();
  } else {
    server.send(400, "text/html", "<h1>Invalid request.</h1>");
  }
}

void WiFiManager::handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

String WiFiManager::generateNetworkList() {
  String options = "";
  int numNetworks = WiFi.scanNetworks();
  if (numNetworks == 0) {
    options = "<option value=''>No networks found</option>";
  } else {
    for (int i = 0; i < numNetworks; ++i) {
      options += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm)</option>";
    }
  }
  WiFi.scanDelete();  // Clean up scan results
  return options;
}