#ifndef HUB_SYSTEM_H
#define HUB_SYSTEM_H

#include <Arduino.h>
#include <array>
#include <string>
#include "Sensor.h"
#include "Output.h"

class HubSystem {
public:
    HubSystem();
    ~HubSystem();

    // Device management
    bool addSensor(const std::string& id, Sensor* sensor);
    bool removeSensor(const std::string& id);
    bool addOutput(const std::string& id, Output* output);
    bool removeOutput(const std::string& id);

    // Linkage management
    bool linkOutputToSensor(const std::string& outputId, const std::string& sensorId);
    bool unlinkOutput(const std::string& outputId);
    std::string getLinkedSensorId(const std::string& outputId);

    // Main loop for updating sensors and actuating outputs
    void loop();

    // Query methods
    Sensor* getSensor(const std::string& id);
    Output* getOutput(const std::string& id);

private:
    // Fixed-size storage to avoid dynamic memory
    static const size_t MAX_SENSORS = 20;
    static const size_t MAX_OUTPUTS = 10;

    std::array<std::pair<std::string, Sensor*>, MAX_SENSORS> _sensors;
    size_t _sensorCount = 0;

    std::array<std::pair<std::string, Output*>, MAX_OUTPUTS> _outputs;
    size_t _outputCount = 0;

    // Linkage storage: outputId -> sensorId (empty string if unlinked)
    std::array<std::pair<std::string, std::string>, MAX_OUTPUTS> _outputToSensorLinks;
    size_t _linkCount = 0;

    // Timer for periodic updates
    unsigned long _lastUpdate = 0;
    static const unsigned long UPDATE_INTERVAL = 5000;  // 5 seconds

    // Helper methods
    size_t findSensorIndex(const std::string& id) const;
    size_t findOutputIndex(const std::string& id) const;
    size_t findLinkIndex(const std::string& outputId) const;
};

#endif  // HUB_SYSTEM_H
