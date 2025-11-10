#ifndef COOLING_OUTPUT_H
#define COOLING_OUTPUT_H

#include "Output.h"

class CoolingOutput : public Output {
public:
    CoolingOutput(uint8_t pin);
    void begin() override;
    void actuate(const SensorData& data) override;
    void setConfig(const ConfigMap& config) override;

    // Setters for control parameters
    void setSetPoint(double value) {_setPoint = value;}
    void setHysteresis(double value) {_hysteresis = value;}

private:
    uint8_t _pin;
    bool _isActive = false;  // Track current state to avoid unnecessary writes

protected:
    double _setPoint = 25.0;    // Default set point (e.g., Celsius)
    double _hysteresis = 1.0;   // Default hysteresis (e.g., degrees)
};

#endif  // COOLING_OUTPUT_H