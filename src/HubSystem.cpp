#include "HubSystem.h"

HubSystem::HubSystem() {}

HubSystem::~HubSystem() {
    // Clean up owned devices
    for (size_t i = 0; i < _sensorCount; ++i) {
        delete _sensors[i].second;
    }
    for (size_t i = 0; i < _outputCount; ++i) {
        delete _outputs[i].second;
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

    // Unlink any outputs linked to this sensor
    for (size_t i = 0; i < _linkCount; ++i) {
        if (_outputToSensorLinks[i].second == id) {
            _outputToSensorLinks[i].second = "";  // Unlink
            Serial.printf("Unlinked output '%s' from removed sensor '%s'\n", _outputToSensorLinks[i].first.c_str(), id.c_str());
        }
    }

    // Delete the sensor
    delete _sensors[index].second;

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

    // Add to end
    _outputs[_outputCount] = {id, output};
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

    // Unlink the output
    unlinkOutput(id);

    // Delete the output
    delete _outputs[index].second;

    // Shift elements to keep array contiguous
    for (size_t i = index; i < _outputCount - 1; ++i) {
        _outputs[i] = _outputs[i + 1];
    }
    _outputCount--;
    Serial.printf("Removed output '%s'\n", id.c_str());
    return true;
}

bool HubSystem::linkOutputToSensor(const std::string& outputId, const std::string& sensorId) {
    if (outputId.empty() || sensorId.empty()) {  // Allow empty sensorId for unlinking
        Serial.println("Invalid outputId or sensorId");
        return false;
    }

    // Check if output exists
    if (getOutput(outputId) == nullptr) {
        Serial.printf("Output ID '%s' not found\n", outputId.c_str());
        return false;
    }

    // Check if sensor exists (unless unlinking)
    if (getSensor(sensorId) == nullptr) {
        Serial.printf("Sensor ID '%s' not found\n", sensorId.c_str());
        return false;
    }

    // Find existing link or add new
    size_t linkIndex = findLinkIndex(outputId);
    if (linkIndex < MAX_OUTPUTS) {
        // Update existing
        _outputToSensorLinks[linkIndex].second = sensorId;
    } else {
        // Add new link
        if (_linkCount >= MAX_OUTPUTS) {
            Serial.println("Maximum links reached");
            return false;
        }
        _outputToSensorLinks[_linkCount] = {outputId, sensorId};
        _linkCount++;
    }
    Serial.printf("Linked output '%s' to sensor '%s'\n", outputId.c_str(), sensorId.empty() ? "none" : sensorId.c_str());
    return true;
}

bool HubSystem::unlinkOutput(const std::string& outputId) {
    size_t linkIndex = findLinkIndex(outputId);
    if (linkIndex >= MAX_OUTPUTS) {
        Serial.printf("No link found for output '%s'\n", outputId.c_str());
        return false;
    }

    // Shift elements to keep array contiguous
    for (size_t i = linkIndex; i < _linkCount - 1; ++i) {
        _outputToSensorLinks[i] = _outputToSensorLinks[i + 1];
    }
    _linkCount--;
    Serial.printf("Unlinked output '%s'\n", outputId.c_str());
    return true;
}

std::string HubSystem::getLinkedSensorId(const std::string& outputId) {
    size_t linkIndex = findLinkIndex(outputId);
    return (linkIndex < _linkCount) ? _outputToSensorLinks[linkIndex].second : "";
}

void HubSystem::loop() {
    if (millis() - _lastUpdate < UPDATE_INTERVAL) return;  // Not time yet

    _lastUpdate = millis();

    // Update all sensors unconditionally
    for (size_t i = 0; i < _sensorCount; ++i) {
        _sensors[i].second->updateData();
    }

    // Actuate linked outputs
    for (size_t i = 0; i < _linkCount; ++i) {
        const std::string& outputId = _outputToSensorLinks[i].first;
        const std::string& sensorId = _outputToSensorLinks[i].second;

        if (sensorId.empty()) continue;  // Skip unlinked outputs

        Sensor* sensor = getSensor(sensorId);
        Output* output = getOutput(outputId);

        if (sensor && output) {
            SensorData data = sensor->getData();
            output->actuate(data);
        }
    }
}

Sensor* HubSystem::getSensor(const std::string& id) {
    size_t index = findSensorIndex(id);
    return (index < _sensorCount) ? _sensors[index].second : nullptr;
}

Output* HubSystem::getOutput(const std::string& id) {
    size_t index = findOutputIndex(id);
    return (index < _outputCount) ? _outputs[index].second : nullptr;
}

size_t HubSystem::findSensorIndex(const std::string& id) const {
    for (size_t i = 0; i < _sensorCount; ++i) {
        if (_sensors[i].first == id) {
            return i;
        }
    }
    return MAX_SENSORS;  // Not found
}

size_t HubSystem::findOutputIndex(const std::string& id) const {
    for (size_t i = 0; i < _outputCount; ++i) {
        if (_outputs[i].first == id) {
            return i;
        }
    }
    return MAX_OUTPUTS;  // Not found
}

size_t HubSystem::findLinkIndex(const std::string& outputId) const {
    for (size_t i = 0; i < _linkCount; ++i) {
        if (_outputToSensorLinks[i].first == outputId) {
            return i;
        }
    }
    return MAX_OUTPUTS;  // Not found
}
