#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <map>  // For SensorData

typedef std::map<String, float> SensorData;  // Generic key-value for sensor readings (e.g., "temperature": 25.5)

class Sensor {
public:
    virtual ~Sensor() {}  // Virtual destructor for proper cleanup in polymorphism
    virtual void begin() = 0;  // Initialize the sensor hardware
    virtual SensorData readData() = 0;  // Read and return data; return empty map on error
    virtual String getId() const = 0;  // Unique identifier for the sensor (for topics, etc.)
};

#endif  // SENSOR_H