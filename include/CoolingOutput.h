#ifndef COOLING_OUTPUT_H
#define COOLING_OUTPUT_H

#include "Output.h"

class CoolingOutput : public Output {
public:
    CoolingOutput(uint8_t pin);
    void begin() override;
    void actuate(const SensorData& data) override;
    String getId() const override { return "cooling"; }  // Default ID; can be set via constructor later

private:
    uint8_t _pin;
    bool _isActive = false;  // Track current state to avoid unnecessary writes
};

#endif  // COOLING_OUTPUT_H