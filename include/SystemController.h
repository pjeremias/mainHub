// src/SystemController.h
#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include <Arduino.h>
#include <map>
#include <vector>
#include "Sensor.h"
#include "Output.h"
#include "MQTTManager.h"
#include "ArduinoJson.h"

class SystemController {
public:
    SystemController(MQTTManager& mqttManager);
    ~SystemController();  // Destructor for potential cleanup (no deletes since not owning)

    void begin();

    void addSensor(const String& id, const String& type, const JsonObjectConst& params);    
    void addOutput(const String& id, const String& type, const JsonObjectConst& params);
    void attachOutputToSensor(const String& outputId, const String& sensorId);

    void loop();

private:
    MQTTManager& _mqttManager;
    std::map<String, Sensor*> _sensorById;
    std::map<String, Output*> _outputById;
    std::map<String, std::vector<Output*>> _linkages;  // Keyed by sensor ID

    unsigned long _lastReadTime = 0;
    const unsigned long _readInterval = 5000;  // 5 seconds

    void handleCommand(const char* payload);
};

#endif  // SYSTEM_CONTROLLER_H