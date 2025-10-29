#include "SystemController.h"

SystemController::SystemController(MQTTManager& mqttManager) : _mqttManager(mqttManager) {}

SystemController::~SystemController() {
    // No deletes: Pointers are not owned (instantiated externally)
}

void SystemController::addSensor(const String& id, Sensor* sensor) {
    if (id.isEmpty()) {
        Serial.println("Cannot add sensor with empty ID. Skipping.");
        return;
    }
    if (_sensorById.find(id) != _sensorById.end()) {
        Serial.printf("Sensor ID '%s' already exists. Overwriting...\n", id.c_str());
    }
    _sensorById[id] = sensor;
    sensor->begin();
}

void SystemController::addOutput(const String& id, Output* output) {
    if (id.isEmpty()) {
        Serial.println("Cannot add output with empty ID. Skipping.");
        return;
    }
    if (_outputById.find(id) != _outputById.end()) {
        Serial.printf("Output ID '%s' already exists. Overwriting...\n", id.c_str());
    }
    _outputById[id] = output;

    output->setOnStateChange([this](Output* out, bool newState) {
        // Reverse lookup ID from pointer
        String outputId;
        for (const auto& pair : _outputById) {
            if (pair.second == out) {
                outputId = pair.first;
                break;
            }
        }
        if (!outputId.isEmpty()) {
            String topic = "esp32/hub/" + outputId + "/state";
            _mqttManager.publish(topic.c_str(), newState ? "on" : "off");
            Serial.printf("Published state change for output '%s': %s\n", outputId.c_str(), newState ? "on" : "off");
        }
    });

    output->begin();
}

void SystemController::attachOutputToSensor(const String& outputId, const String& sensorId) {
    auto sensorIt = _sensorById.find(sensorId);
    auto outputIt = _outputById.find(outputId);
    if (sensorIt == _sensorById.end()) {
        Serial.printf("Sensor ID '%s' not found for attachment.\n", sensorId.c_str());
        return;
    }
    if (outputIt == _outputById.end()) {
        Serial.printf("Output ID '%s' not found for attachment.\n", outputId.c_str());
        return;
    }

    _linkages[sensorId].push_back(outputIt->second);
    Serial.printf("Attached output '%s' to sensor '%s'.\n", outputId.c_str(), sensorId.c_str());
}

void SystemController::loop() {
    unsigned long now = millis();
    if (now - _lastReadTime > _readInterval) {
        _lastReadTime = now;

        for (const auto& sensorPair : _sensorById) {
            String sensorId = sensorPair.first;
            Sensor* sensor = sensorPair.second;
            SensorData data = sensor->readData();

            if (!data.empty()) {
                // Generic publishing with sensor ID in topic
                for (const auto& dataPair : data) {
                    char dataStr[8];
                    dtostrf(dataPair.second, 6, 2, dataStr);
                    String topic = "esp32/hub/" + sensorId + "/" + dataPair.first;
                    _mqttManager.publish(topic.c_str(), dataStr);
                }

                // Actuate linked outputs
                auto linkageIt = _linkages.find(sensorId);
                if (linkageIt != _linkages.end()) {
                    for (Output* output : linkageIt->second) {
                        output->actuate(data);
                    }
                }
            }
        }
    }
}