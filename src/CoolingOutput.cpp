#include "CoolingOutput.h"
#include "OutputFactory.h"

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

        if (_onStateChange) {
            _onStateChange(this, _isActive);
        }
    }
}

void CoolingOutput::setConfig(const ConfigMap& config) {
    auto it = config.find(CONFIG_SET_POINT);
    if (it != config.end() && std::holds_alternative<double>(it->second)) {
        setSetPoint(std::get<double>(it->second));
    }

    it = config.find(CONFIG_HYSTERESIS);
    if (it != config.end() && std::holds_alternative<double>(it->second)) {
        setHysteresis(std::get<double>(it->second));
    }
}

static OutputRegistrar _coolingRegistrar("cooling", [](const JsonObjectConst& params) -> Output* {
    uint8_t pin = params["pin"] | -1;  // Default -1 if missing
    if (pin == -1) {  // Basic validation
        Serial.println("Missing 'pin' for cooling output");
        return nullptr;
    }
    return new CoolingOutput(pin);
});