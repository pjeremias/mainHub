#include "SystemController.h"
#include "SensorFactory.h"
#include "OutputFactory.h"

SystemController::SystemController(MQTTManager& mqttManager) : _mqttManager(mqttManager) {}

SystemController::~SystemController() {
    // Delete owned sensors
    for (auto& pair : _sensorById) {
        delete pair.second;
    }
    // Delete owned outputs
    for (auto& pair : _outputById) {
        delete pair.second;
    }
}

void SystemController::begin() {
    _mqttManager.subscribe("esp32/hub/commands", [this](const char* payload) {
        this->handleCommand(payload);
    });
}

void SystemController::addSensor(const String& id, const String& type, const JsonObjectConst& params) {
    if (id.isEmpty()) {
        Serial.println("Cannot add sensor with empty ID. Skipping.");
        return;
    }
    if (_sensorById.find(id) != _sensorById.end()) {
        Serial.printf("Sensor ID '%s' already exists. Overwriting...\n", id.c_str());
        delete _sensorById[id];  // Clean old owned sensor
    }

    Sensor* sensor = SensorFactory::instance().create(type, params);
    if (sensor == nullptr) {
        Serial.printf("Failed to create sensor of type '%s' for ID '%s'.\n", type.c_str(), id.c_str());
        return;
    }
    _sensorById[id] = sensor;
    sensor->begin();  // Initialize immediately
}

void SystemController::addOutput(const String& id, const String& type, const JsonObjectConst& params) {
    if (id.isEmpty()) {
        Serial.println("Cannot add output with empty ID. Skipping.");
        return;
    }
    if (_outputById.find(id) != _outputById.end()) {
        Serial.printf("Output ID '%s' already exists. Overwriting...\n", id.c_str());
        delete _outputById[id];  // Clean old owned output
    }

    Output* output = OutputFactory::instance().create(type, params);
    if (output == nullptr) {
        Serial.printf("Failed to create output of type '%s' for ID '%s'.\n", type.c_str(), id.c_str());
        return;
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
            _mqttManager.publish(topic.c_str(), newState ? "on" : "off", true);
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

void SystemController::handleCommand(const char* payload) {
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.printf("JSON parse error: %s\n", error.c_str());
        return;
    }

    String command = doc["command"];
    
    if (command == "add_sensor") {
        String id = doc["id"];
        String type = doc["type"];
        JsonObjectConst params = doc["params"];
        
        if (id.isEmpty() || type.isEmpty()) {
            Serial.println("Missing 'id' or 'type' for add_sensor command");
            return;
        }
        
        addSensor(id, type, params);
        Serial.printf("Added sensor '%s' of type '%s' via command.\n", id.c_str(), type.c_str());
        
    } else if (command == "add_output") {
        String id = doc["id"];
        String type = doc["type"];
        JsonObjectConst params = doc["params"];
        
        if (id.isEmpty() || type.isEmpty()) {
            Serial.println("Missing 'id' or 'type' for add_output command");
            return;
        }
        
        addOutput(id, type, params);
        Serial.printf("Added output '%s' of type '%s' via command.\n", id.c_str(), type.c_str());
        
    } else if (command == "set_output_param") {
        String outputId = doc["output_id"];
        auto it = _outputById.find(outputId);
        if (it == _outputById.end()) {
            Serial.printf("Output ID '%s' not found.\n", outputId.c_str());
            return;
        }
        Output* output = it->second;

        if (doc["set_setpoint"].is<double>()) {
            double value = doc["set_setpoint"];
            output->setSetPoint(value);
        }
        if (doc["set_hysteresis"].is<double>()) {
            double value = doc["set_hysteresis"];
            output->setHysteresis(value);
        }
        Serial.printf("Applied config to output '%s'.\n", outputId.c_str());
    } else {
        Serial.printf("Unknown command: %s\n", command.c_str());
    }
}