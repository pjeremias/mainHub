#ifndef OUTPUT_FACTORY_H
#define OUTPUT_FACTORY_H

#include <Arduino.h>
#include <functional>
#include <unordered_map>
#include <string>
#include "ArduinoJson.h"
#include "Output.h"

class OutputFactory {
public:
    using CreateFunc = std::function<Output*(const JsonObjectConst& params)>;

    void registerType(const std::string& type, CreateFunc creator);
    Output* create(const std::string& type, const JsonObjectConst& params);

    static OutputFactory& instance() {
        static OutputFactory factory;
        return factory;
    }

private:
    std::unordered_map<std::string, CreateFunc> _registry;
    OutputFactory() = default;
};

class OutputRegistrar {
public:
    OutputRegistrar(const std::string& type, OutputFactory::CreateFunc creator) {
        OutputFactory::instance().registerType(type, creator);
    }
};

#endif  // OUTPUT_FACTORY_H
