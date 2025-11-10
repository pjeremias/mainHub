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
    virtual bool updateData() = 0;  // Update the internal data cache and return success
    SensorData getData() { return _data; }  // Return the cached data

    // Unified data key constants
    inline static const std::string TEMP_KEY = "temp";
    inline static const std::string HUM_KEY = "hum";

protected:
    SensorData _data;  // Cached sensor data
};

#endif  // SENSOR_H