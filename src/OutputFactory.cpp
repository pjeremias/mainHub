#include "OutputFactory.h"

void OutputFactory::registerType(const String& type, CreateFunc creator) {
    _registry[type] = creator;
}

Output* OutputFactory::create(const String& type, const JsonObjectConst& params) {
    auto it = _registry.find(type);
    if (it != _registry.end()) {
        return it->second(params);
    }
    Serial.printf("Unknown output type: %s\n", type.c_str());
    return nullptr;
}
