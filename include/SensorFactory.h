#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include <Arduino.h>
#include <functional>
#include <map>
#include "ArduinoJson.h"
#include "Sensor.h"

class SensorFactory {
public:
    using CreateFunc = std::function<Sensor*(const JsonObjectConst& params)>;

    void registerType(const String& type, CreateFunc creator);
    Sensor* create(const String& type, const JsonObjectConst& params);

    static SensorFactory& instance() {
        static SensorFactory factory;
        return factory;
    }

private:
    std::map<String, CreateFunc> _registry;
    SensorFactory() = default;
};

class SensorRegistrar {
public:
    SensorRegistrar(const String& type, SensorFactory::CreateFunc creator) {
        SensorFactory::instance().registerType(type, creator);
    }
};

#endif  // SENSOR_FACTORY_H