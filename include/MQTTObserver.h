#ifndef MQTT_OBSERVER_H
#define MQTT_OBSERVER_H

#include "HubObserver.h"
#include "MQTTManager.h"
#include "HubSystem.h"
#include <ArduinoJson.h>

class MQTTObserver : public HubObserver {
public:
    MQTTObserver(MQTTManager& mqttManager, HubSystem& hub);
    ~MQTTObserver() override = default;

    void begin();  // Subscribe to commands topic

    // Implement only the required methods for publishing data
    void onSensorDataUpdated(const std::string& sensorId, const SensorData& data) override;
    void onOutputStateChanged(const std::string& outputId, bool newState) override;

    // Stub implementations for other methods (do nothing for now)
    void onSensorAdded(const std::string& sensorId, const std::string& type) override {}
    void onSensorRemoved(const std::string& sensorId) override {}
    void onOutputConfigChanged(const std::string& outputId, const ConfigMap& newConfig) override {}
    void onOutputAdded(const std::string& outputId, const std::string& type) override {}
    void onOutputRemoved(const std::string& outputId) override {}
    void onOutputLinked(const std::string& outputId, const std::string& sensorId) override {}
    void onOutputUnlinked(const std::string& outputId) override {}
    void onError(const std::string& message) override {}
    void onLog(const std::string& message) override {}

private:
    MQTTManager& _mqttManager;
    HubSystem& _hub;
};

#endif  // MQTT_OBSERVER_H
