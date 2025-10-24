// #include <WiFi.h>
// #include <PubSubClient.h>
// #include <DHT.h>

// // Wi-Fi credentials
// const char* ssid = "Martincasa2.4";
// const char* password = "0014440625";

// // MQTT broker details
// const char* mqtt_server = "test.mosquitto.org";
// const int mqtt_port = 1883;
// const char* mqtt_state = "myhome/sensor/state";
// const char* mqtt_temperature_topic = "myhome/sensor/temperature";
// const char* mqtt_humidity_topic = "myhome/sensor/humidity";
// const char* mqtt_target_temp_topic = "myhome/control/target_temperature";
// const char* mqtt_target_hum_topic = "myhome/control/target_humidity";
// // MQTT topics for state changes
// const char* mqtt_cooling_state_topic = "myhome/control/cooling_state";
// const char* mqtt_dehumidifier_state_topic = "myhome/control/dehumidifier_state";

// // DHT22 sensor setup
// #define DHTPIN 25        // GPIO pin connected to the DHT22 data pin
// #define DHTTYPE DHT22   // DHT22 sensor type
// DHT dht(DHTPIN, DHTTYPE);

// // Control pins (e.g., for relays)
// #define COOLING_RELAY_PIN 2  // Relay for cooling unit
// #define DEHUMIDIFIER_RELAY_PIN 18  // Relay for dehumidifier

// // Wi-Fi and MQTT clients
// WiFiClient espClient;
// PubSubClient client(espClient);

// // Sensor readings
// float temp = 0.0;
// float hum = 0.0;

// // Control variables
// enum SystemMode {
//   MODE_REFRIGERATION,
//   MODE_DEHUMIDIFYING,
//   MODE_AUTO
// };
// SystemMode systemMode = MODE_AUTO;

// enum ControlState { CONTROL_OFF, CONTROL_ON };
// ControlState coolingState = CONTROL_OFF;
// ControlState dehumidifierState = CONTROL_OFF;
// float tempSP = 20.0;
// float humSP = 50.0;
// const float tempHysteresis = 1.0;
// const float humHysteresis = 5.0;

// // Timing for non-blocking delays
// unsigned long lastSensorRead = 0;
// const unsigned long sensorInterval = 2000;

// void callback(char* topic, byte* payload, unsigned int length);
// void setup_wifi();

// void setup() {
//   Serial.begin(115200);

//   // Initialize DHT22 sensor
//   dht.begin();

//   // Initialize relay pins
//   pinMode(COOLING_RELAY_PIN, OUTPUT);
//   pinMode(DEHUMIDIFIER_RELAY_PIN, OUTPUT);
//   digitalWrite(COOLING_RELAY_PIN, LOW);
//   digitalWrite(DEHUMIDIFIER_RELAY_PIN, LOW);

//   // Connect to Wi-Fi
//   setup_wifi();

//   // Set up MQTT
//   client.setServer(mqtt_server, mqtt_port);
//   client.setCallback(callback);
// }

// void setup_wifi() {
//   delay(10);
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);

//   WiFi.begin(ssid, password);

//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.println("WiFi connected");
//   Serial.println("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// void callback(char* topic, byte* payload, unsigned int length) {
//   String message;
//   for (unsigned int i = 0; i < length; i++) {
//     message += (char)payload[i];
//   }

//   if (String(topic) == mqtt_target_temp_topic) {
//     tempSP = message.toFloat();
//     Serial.print("Received target temperature: ");
//     Serial.println(tempSP);
//   } else if (String(topic) == mqtt_target_hum_topic) {
//     humSP = message.toFloat();
//     Serial.print("Received target humidity: ");
//     Serial.println(humSP);
//   }
// }

// void reconnect() {
//   int retryCount = 0;
//   while (!client.connected() && retryCount < 5) {
//     Serial.print("Attempting MQTT connection...");
//     String clientId = "ESP32Client-";
//     clientId += String(random(0xffff), HEX);
    
//     if (client.connect(clientId.c_str())) {
//       Serial.println("connected");
//       client.subscribe(mqtt_target_temp_topic);
//       client.subscribe(mqtt_target_hum_topic);
//     } else {
//       Serial.print("failed, rc=");
//       Serial.print(client.state());
//       Serial.println(" try again in 5 seconds");
//       delay(5000);
//       retryCount++;
//     }
//   }
//   if (retryCount >= 5) {
//     Serial.println("Failed to connect to MQTT after 5 attempts. Restarting...");
//     ESP.restart();
//   }
// }

// void control_process() {
//   if (systemMode == MODE_REFRIGERATION || systemMode == MODE_AUTO) {
//     if (coolingState == CONTROL_OFF) {
//       if (temp > tempSP + tempHysteresis) {
//         coolingState = CONTROL_ON;
//         digitalWrite(COOLING_RELAY_PIN, HIGH);
//         Serial.println("Cooling ON");
//         client.publish(mqtt_cooling_state_topic, "ON");  // Publish state change
//       }
//     } else {
//       if (temp < tempSP - tempHysteresis) {
//         coolingState = CONTROL_OFF;
//         digitalWrite(COOLING_RELAY_PIN, LOW);
//         Serial.println("Cooling OFF");
//         client.publish(mqtt_cooling_state_topic, "OFF");  // Publish state change
//       }
//     }
//   }

//   if (systemMode == MODE_DEHUMIDIFYING || systemMode == MODE_AUTO) {
//     if (dehumidifierState == CONTROL_OFF) {
//       if (hum > humSP + humHysteresis) {
//         dehumidifierState = CONTROL_ON;
//         digitalWrite(DEHUMIDIFIER_RELAY_PIN, HIGH);
//         Serial.println("Dehumidifier ON");
//         client.publish(mqtt_dehumidifier_state_topic, "ON");  // Publish state change
//       }
//     } else {
//       if (hum < humSP - humHysteresis) {
//         dehumidifierState = CONTROL_OFF;
//         digitalWrite(DEHUMIDIFIER_RELAY_PIN, LOW);
//         Serial.println("Dehumidifier OFF");
//         client.publish(mqtt_dehumidifier_state_topic, "OFF");  // Publish state change
//       }
//     }
//   }
// }

// void loop() {
//   if (!client.connected()) {
//     reconnect();
//   }
//   client.loop();

//   unsigned long currentMillis = millis();
//   if (currentMillis - lastSensorRead >= sensorInterval) {
//     lastSensorRead = currentMillis;

//     temp = dht.readTemperature();
//     hum = dht.readHumidity();

//     if (isnan(temp) || isnan(hum)) {
//       Serial.println("Failed to read from DHT sensor!");
//       return;
//     }

//     Serial.print("Temperature: ");
//     Serial.print(temp);
//     Serial.print(" °C, Humidity: ");
//     Serial.print(hum);
//     Serial.println(" %");

//     char tempString[8];
//     char humString[8];
//     dtostrf(temp, 6, 2, tempString);
//     dtostrf(hum, 6, 2, humString);
//     client.publish(mqtt_temperature_topic, tempString);
//     client.publish(mqtt_humidity_topic, humString);

//     control_process();
//   }
// }