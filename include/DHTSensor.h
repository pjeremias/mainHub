#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "Sensor.h"
#include <DHT.h>

class DHTSensor : public Sensor {
public:
    DHTSensor(uint8_t pin, uint8_t type = DHT22);
    void begin() override;
    bool updateData() override;

private:
    DHT _dht;
};

#endif  // DHT_SENSOR_H