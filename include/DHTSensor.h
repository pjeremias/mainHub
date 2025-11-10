#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "Sensor.h"
#include <DHT.h>

class DHTSensor : public Sensor {
public:
    DHTSensor(uint8_t pin, uint8_t type = DHT22);
    void begin() override;
    SensorData readData() override;

private:
    DHT _dht;
    uint8_t _pin;
    uint8_t _type;
};

#endif  // DHT_SENSOR_H