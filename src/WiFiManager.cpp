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
    <html lang="en">
    <head>
      <meta charset="UTF-8">
      <meta name="viewport" content="width=device-width, initial-scale=1.0">
      <title>ESP32 WiFi Configuration</title>
      <style>
        body {
          font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, Helvetica, Arial, sans-serif;
          background-color: #121212;
          color: #e0e0e0;
          max-width: 400px;
          margin: 0 auto;
          padding: 20px;
          display: flex;
          flex-direction: column;
          align-items: center;
          min-height: 100vh;
        }
        h1 {
          color: #ffffff;
          text-align: center;
          margin-bottom: 20px;
        }
        p {
          text-align: center;
          margin-bottom: 30px;
          color: #b0b0b0;
        }
        form {
          width: 100%;
          display: flex;
          flex-direction: column;
          gap: 15px;
        }
        label {
          font-weight: 600;
          color: #d0d0d0;
        }
        select, input[type="password"] {
          background-color: #1e1e1e;
          color: #e0e0e0;
          border: 1px solid #333333;
          border-radius: 6px;
          padding: 12px;
          font-size: 16px;
          appearance: none;
          transition: border-color 0.3s ease;
        }
        select:focus, input[type="password"]:focus {
          border-color: #4CAF50;
          outline: none;
        }
        button {
          background-color: #4CAF50;
          color: #ffffff;
          border: none;
          border-radius: 6px;
          padding: 12px;
          font-size: 16px;
          font-weight: 600;
          cursor: pointer;
          transition: background-color 0.3s ease, transform 0.1s ease;
        }
        button:hover {
          background-color: #45a049;
        }
        button:active {
          transform: scale(0.98);
        }
        @media (prefers-color-scheme: light) {
          body { background-color: #f5f5f5; color: #333333; }
          h1 { color: #222222; }
          p { color: #555555; }
          label { color: #444444; }
          select, input[type="password"] { background-color: #ffffff; color: #333333; border: 1px solid #dddddd; }
          select:focus, input[type="password"]:focus { border-color: #4CAF50; }
        }
      </style>
    </head>
    <body>
      <h1>Configure WiFi</h1>
      <p>Select a network and enter the password if required.</p>
      <form action="/save" method="post">
        <label for="ssid">Available Networks:</label>
        <select id="ssid" name="ssid" required>
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