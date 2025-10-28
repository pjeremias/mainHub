#include "CoolingOutput.h"

CoolingOutput::CoolingOutput(uint8_t pin) : _pin(pin) {}

void CoolingOutput::begin() {
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, LOW);  // Start off
    Serial.println("Cooling output initialized.");
}

void CoolingOutput::actuate(const SensorData& data) {
    auto it = data.find("temp");
    if (it == data.end()) {
        Serial.println("No temperature data available for actuation.");
        return;
    }

    float currentTemp = it->second;

    bool shouldActivate = false;
    if (currentTemp > _setPoint + _hysteresis) {
        shouldActivate = true;  // Turn on cooling
    } else if (currentTemp < _setPoint - _hysteresis) {
        shouldActivate = false;  // Turn off cooling
    } else {
        shouldActivate = _isActive;  // Maintain current state within hysteresis band
    }

    if (shouldActivate != _isActive) {
        _isActive = shouldActivate;
        digitalWrite(_pin, _isActive ? HIGH : LOW);  // Assume HIGH activates relay/cooling
        Serial.printf("Cooling output %s.\n", _isActive ? "activated" : "deactivated");
    }
}