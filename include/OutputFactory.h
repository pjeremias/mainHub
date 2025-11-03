#ifndef OUTPUT_FACTORY_H
#define OUTPUT_FACTORY_H

#include <Arduino.h>
#include <functional>
#include <map>
#include "ArduinoJson.h"
#include "Output.h"

class OutputFactory {
public:
    using CreateFunc = std::function<Output*(const JsonObjectConst& params)>;

    void registerType(const String& type, CreateFunc creator);
    Output* create(const String& type, const JsonObjectConst& params);

    static OutputFactory& instance() {
        static OutputFactory factory;
        return factory;
    }

private:
    std::map<String, CreateFunc> _registry;
    OutputFactory() = default;
};

class OutputRegistrar {
public:
    OutputRegistrar(const String& type, OutputFactory::CreateFunc creator) {
        OutputFactory::instance().registerType(type, creator);
    }
};

#endif  // OUTPUT_FACTORY_H
