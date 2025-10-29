#ifndef OUTPUT_H
#define OUTPUT_H

#include <Arduino.h>
#include "Sensor.h"  // For SensorData typedef

class Output {
public:
    virtual ~Output() {}  // Virtual destructor for proper cleanup in polymorphism
    virtual void begin() = 0;  // Initialize the output hardware
    virtual void actuate(const SensorData& data) = 0;  // Actuate based on sensor data
    virtual String getId() const = 0;  // Unique identifier for the output (for logging, etc.)

    // Concrete setters for common control parameters
    void setSetPoint(double value) {_setPoint = value;}
    void setHysteresis(double value) {_hysteresis = value;}

    void setOnStateChange(std::function<void(Output*, bool newState)> callback) {_onStateChange = callback;}

protected:
    double _setPoint = 25.0;    // Default set point (e.g., Celsius)
    double _hysteresis = 1.0;   // Default hysteresis (e.g., degrees)
    std::function<void(Output*, bool newState)> _onStateChange = nullptr;
};

#endif  // OUTPUT_H