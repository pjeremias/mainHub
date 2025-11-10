#include "SensorFactory.h"

void SensorFactory::registerType(const std::string& type, CreateFunc creator) {
    _registry[type] = creator;
}

Sensor* SensorFactory::create(const std::string& type, const JsonObjectConst& params) {
    auto it = _registry.find(type);
    if (it != _registry.end()) {
        return it->second(params);
    }
    Serial.printf("Unknown sensor type: %s\n", type.c_str());
    return nullptr;
}