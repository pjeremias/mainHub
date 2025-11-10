#include "HubSystem.h"

HubSystem::HubSystem() {}

HubSystem::~HubSystem() {
    // Clean up owned devices
    for (size_t i = 0; i < _sensorCount; ++i) {
        delete _sensors[i].sensor;
    }
    for (size_t i = 0; i < _outputCount; ++i) {
        delete _outputs[i].output;
    }
}

bool HubSystem::addSensor(const std::string& id, Sensor* sensor) {
    if (id.empty() || sensor == nullptr) {
        Serial.println("Cannot add sensor with empty ID or null pointer");
        return false;
    }

    // Check if already exists
    if (findSensorIndex(id) < MAX_SENSORS) {
        Serial.printf("Sensor ID '%s' already exists\n", id.c_str());
        return false;
    }

    // Check capacity
    if (_sensorCount >= MAX_SENSORS) {
        Serial.println("Maximum sensors reached");
        return false;
    }

    // Add to end (keeps array contiguous)
    _sensors[_sensorCount] = {id, sensor};
    _sensorCount++;
    sensor->begin();  // Initialize immediately
    Serial.printf("Added sensor '%s'\n", id.c_str());
    return true;
}

bool HubSystem::removeSensor(const std::string& id) {
    size_t index = findSensorIndex(id);
    if (index >= MAX_SENSORS) {
        Serial.printf("Sensor ID '%s' not found\n", id.c_str());
        return false;
    }

    // Clear attachedSensor in outputs if they were attached to this sensor
    for (size_t i = 0; i < _outputCount; ++i) {
        if (_outputs[i].attachedSensor == id) {
            _outputs[i].attachedSensor = "";
            Serial.printf("Cleared attachment for output '%s'\n", _outputs[i].id.c_str());
        }
    }

    // Delete the sensor
    delete _sensors[index].sensor;

    // Shift elements to keep array contiguous
    for (size_t i = index; i < _sensorCount - 1; ++i) {
        _sensors[i] = _sensors[i + 1];
    }
    _sensorCount--;
    Serial.printf("Removed sensor '%s'\n", id.c_str());
    return true;
}

bool HubSystem::addOutput(const std::string& id, Output* output) {
    if (id.empty() || output == nullptr) {
        Serial.println("Cannot add output with empty ID or null pointer");
        return false;
    }

    // Check if already exists
    if (findOutputIndex(id) < MAX_OUTPUTS) {
        Serial.printf("Output ID '%s' already exists\n", id.c_str());
        return false;
    }

    // Check capacity
    if (_outputCount >= MAX_OUTPUTS) {
        Serial.println("Maximum outputs reached");
        return false;
    }

    // Add to end (keeps array contiguous)
    _outputs[_outputCount] = {id, output, ""};  // attachedSensor starts empty
    _outputCount++;
    output->begin();  // Initialize immediately
    Serial.printf("Added output '%s'\n", id.c_str());
    return true;
}

bool HubSystem::removeOutput(const std::string& id) {
    size_t index = findOutputIndex(id);
    if (index >= MAX_OUTPUTS) {
        Serial.printf("Output ID '%s' not found\n", id.c_str());
        return false;
    }

    // Delete the output
    delete _outputs[index].output;

    // Shift elements to keep array contiguous
    for (size_t i = index; i < _outputCount - 1; ++i) {
        _outputs[i] = _outputs[i + 1];
    }
    _outputCount--;
    Serial.printf("Removed output '%s'\n", id.c_str());
    return true;
}

bool HubSystem::linkOutputToSensor(const std::string& outputId, const std::string& sensorId) {
    if (outputId.empty() || sensorId.empty()) {
        Serial.println("Invalid outputId or sensorId");
        return false;
    }

    size_t outputIndex = findOutputIndex(outputId);
    if (outputIndex >= MAX_OUTPUTS) {
        Serial.printf("Output ID '%s' not found\n", outputId.c_str());
        return false;
    }

    if (getSensor(sensorId) == nullptr) {
        Serial.printf("Sensor ID '%s' not found\n", sensorId.c_str());
        return false;
    }

    _outputs[outputIndex].attachedSensor = sensorId;
    Serial.printf("Linked output '%s' to sensor '%s'\n", outputId.c_str(), sensorId.c_str());
    return true;
}

bool HubSystem::unlinkOutput(const std::string& outputId) {
    size_t outputIndex = findOutputIndex(outputId);
    if (outputIndex >= MAX_OUTPUTS) {
        Serial.printf("Output ID '%s' not found\n", outputId.c_str());
        return false;
    }

    _outputs[outputIndex].attachedSensor = "";
    Serial.printf("Unlinked output '%s'\n", outputId.c_str());
    return true;
}

void HubSystem::loop() {
    if (millis() - _lastUpdate < UPDATE_INTERVAL) return;  // Not time yet

    _lastUpdate = millis();

    // Update all sensors unconditionally and log data
    for (size_t i = 0; i < _sensorCount; ++i) {
        _sensors[i].sensor->updateData();
        SensorData data = _sensors[i].sensor->getData();
        Serial.printf("Sensor '%s' updated: ", _sensors[i].id.c_str());
        for (const auto& pair : data) {
            Serial.printf("%s=%.2f ", pair.first.c_str(), pair.second);
        }
        Serial.println();
    }

    // Iterate through outputs and actuate if attached to a sensor
    for (size_t i = 0; i < _outputCount; ++i) {
        if (!_outputs[i].attachedSensor.empty()) {
            Sensor* sensor = getSensor(_outputs[i].attachedSensor);
            if (sensor) {
                SensorData data = sensor->getData();
                Serial.printf("Actuating output '%s' based on sensor '%s'\n", _outputs[i].id.c_str(), _outputs[i].attachedSensor.c_str());
                _outputs[i].output->actuate(data);
            }
        }
    }
}

Sensor* HubSystem::getSensor(const std::string& id) {
    size_t index = findSensorIndex(id);
    return (index < _sensorCount) ? _sensors[index].sensor : nullptr;
}

Output* HubSystem::getOutput(const std::string& id) {
    size_t index = findOutputIndex(id);
    return (index < _outputCount) ? _outputs[index].output : nullptr;
}

size_t HubSystem::findSensorIndex(const std::string& id) const {
    for (size_t i = 0; i < _sensorCount; ++i) {
        if (_sensors[i].id == id) {
            return i;
        }
    }
    return MAX_SENSORS;  // Not found
}

size_t HubSystem::findOutputIndex(const std::string& id) const {
    for (size_t i = 0; i < _outputCount; ++i) {
        if (_outputs[i].id == id) {
            return i;
        }
    }
    return MAX_OUTPUTS;  // Not found
}
