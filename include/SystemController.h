#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include <Arduino.h>
#include <map>
#include <vector>
#include "Sensor.h"
#include "Output.h"
#include "MQTTManager.h"

class SystemController {
public:
    SystemController(MQTTManager& mqttManager);
    ~SystemController();  // Destructor for potential cleanup (no deletes since not owning)

    void addSensor(const String& id, Sensor* sensor);
    void addOutput(const String& id, Output* output);
    void attachOutputToSensor(const String& outputId, const String& sensorId);

    void begin();
    void loop();

private:
    MQTTManager& _mqttManager;
    std::map<String, Sensor*> _sensorById;
    std::map<String, Output*> _outputById;
    std::map<String, std::vector<Output*>> _linkages;  // Keyed by sensor ID

    unsigned long _lastReadTime = 0;
    const unsigned long _readInterval = 5000;  // 5 seconds
};

#endif  // SYSTEM_CONTROLLER_H