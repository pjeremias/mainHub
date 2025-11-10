#ifndef SENSOR_FACTORY_H
#define SENSOR_FACTORY_H

#include <Arduino.h>
#include <functional>
#include <unordered_map>
#include <string>
#include "ArduinoJson.h"
#include "Sensor.h"

class SensorFactory {
public:
    using CreateFunc = std::function<Sensor*(const JsonObjectConst& params)>;

    void registerType(const std::string& type, CreateFunc creator);
    Sensor* create(const std::string& type, const JsonObjectConst& params);

    static SensorFactory& instance() {
        static SensorFactory factory;
        return factory;
    }

private:
    std::unordered_map<std::string, CreateFunc> _registry;
    SensorFactory() = default;
};

class SensorRegistrar {
public:
    SensorRegistrar(const std::string& type, SensorFactory::CreateFunc creator) {
        SensorFactory::instance().registerType(type, creator);
    }
};

#endif  // SENSOR_FACTORY_H