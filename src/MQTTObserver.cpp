#include "MQTTObserver.h"

MQTTObserver::MQTTObserver(MQTTManager& mqttManager, HubSystem& hub) : _mqttManager(mqttManager), _hub(hub) {}

void MQTTObserver::begin() {
    _mqttManager.subscribe("esp32hub/commands", [this](const char* payload) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);
        if (error) {
            Serial.printf("JSON parse error in MQTTObserver: %s\n", error.c_str());
            return;
        }

        std::string command = doc["command"];
        if (command == "set_output_config") {
            std::string outputId = doc["id"];
            JsonObjectConst configObj = doc["config"];

            ConfigMap configMap;
            for (JsonPairConst kv : configObj) {
                std::string key = kv.key().c_str();
                if (kv.value().is<int>()) {
                    configMap[key] = kv.value().as<int>();
                } else if (kv.value().is<double>()) {
                    configMap[key] = kv.value().as<double>();
                } else if (kv.value().is<bool>()) {
                    configMap[key] = kv.value().as<bool>();
                } else {
                    Serial.printf("Unsupported config type for key '%s'\n", key.c_str());
                }
            }

            _hub.setOutputConfig(outputId, configMap);
        } else {
            Serial.printf("Unknown command: %s\n", command.c_str());
        }
    });
}

void MQTTObserver::onSensorDataUpdated(const std::string& sensorId, const SensorData& data) {
    for (const auto& pair : data) {
        std::string topic = "esp32hub/sensors/" + sensorId + "/" + pair.first;
        char valueStr[16];
        snprintf(valueStr, sizeof(valueStr), "%.2f", pair.second);
        _mqttManager.publish(topic.c_str(), valueStr);
    }
}

void MQTTObserver::onOutputStateChanged(const std::string& outputId, bool newState) {
    std::string topic = "esp32hub/outputs/" + outputId + "/state";
    const char* stateStr = newState ? "on" : "off";
    _mqttManager.publish(topic.c_str(), stateStr, true);
}
