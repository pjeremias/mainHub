#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <unordered_map>  // For SensorData
#include <string>

typedef std::unordered_map<std::string, float> SensorData;  // Generic key-value for sensor readings (e.g., "temperature": 25.5)

class Sensor {
public:
    virtual ~Sensor() {}  // Virtual destructor for proper cleanup in polymorphism
    virtual void begin() = 0;  // Initialize the sensor hardware
    virtual SensorData readData() = 0;  // Read and return data; return empty map on error
};

#endif  // SENSOR_H