#ifndef HUB_OBSERVER_H
#define HUB_OBSERVER_H

#include <string>
#include "Sensor.h"  // For SensorData typedef
#include "Output.h"  // For ConfigMap typedef

class HubObserver {
public:
    virtual ~HubObserver() = default;

    // Sensor events
    virtual void onSensorDataUpdated(const std::string& sensorId, const SensorData& data) = 0;
    virtual void onSensorAdded(const std::string& sensorId, const std::string& type) = 0;
    virtual void onSensorRemoved(const std::string& sensorId) = 0;

    // Output events
    virtual void onOutputStateChanged(const std::string& outputId, bool newState) = 0;
    virtual void onOutputConfigChanged(const std::string& outputId, const ConfigMap& newConfig) = 0;
    virtual void onOutputAdded(const std::string& outputId, const std::string& type) = 0;
    virtual void onOutputRemoved(const std::string& outputId) = 0;

    // Linkage events
    virtual void onOutputLinked(const std::string& outputId, const std::string& sensorId) = 0;
    virtual void onOutputUnlinked(const std::string& outputId) = 0;

    // General events
    virtual void onError(const std::string& message) = 0;
    virtual void onLog(const std::string& message) = 0;
};

#endif  // HUB_OBSERVER_H
