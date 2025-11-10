#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>
#include "Sensor.h"  // For SensorData typedef
#include <unordered_map>
#include <variant>

using ConfigMap = std::unordered_map<std::string, std::variant<int, double, bool>>;

class Output {
public:
    virtual ~Output() {}  // Virtual destructor for proper cleanup in polymorphism
    virtual void begin() = 0;  // Initialize the output hardware
    virtual void actuate(const SensorData& data) = 0;  // Actuate based on sensor data
    virtual void setConfig(const ConfigMap& config) = 0;

    void setOnStateChange(std::function<void(Output*, bool newState)> callback) {_onStateChange = callback;}

protected:
    std::function<void(Output*, bool newState)> _onStateChange = nullptr;
};

// Common config key constants for consistency
constexpr const char* CONFIG_SET_POINT = "setPoint";
constexpr const char* CONFIG_HYSTERESIS = "hysteresis";

#endif  // OUTPUT_H