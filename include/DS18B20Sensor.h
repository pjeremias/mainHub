#ifndef DS18B20_SENSOR_H
#define DS18B20_SENSOR_H

#include "Sensor.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class DS18B20Sensor : public Sensor {
public:
    DS18B20Sensor(uint8_t pin);
    void begin() override;
    bool updateData() override;

private:
    OneWire _oneWire;
    DallasTemperature _sensors;
};

#endif  // DS18B20_SENSOR_H
