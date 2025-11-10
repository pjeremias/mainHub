#include "DHTSensor.h"
#include "SensorFactory.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type) : _dht(pin, type){}

void DHTSensor::begin() {
  _dht.begin();
  Serial.println("DHT22 sensor initialized.");
}

bool DHTSensor::updateData() {
  // float temp = _dht.readTemperature();  // Celsius
  // float hum = _dht.readHumidity();

  // if (isnan(temp) || isnan(hum)) {
  //     Serial.println("Failed to read from DHT22 sensor!");
  //     return false;  // Do not update _data on error
  // }

  // _data[Sensor::TEMP_KEY] = temp;
  // _data[Sensor::HUM_KEY] = hum;
  _data[Sensor::TEMP_KEY] = 20.0;
  _data[Sensor::HUM_KEY] = 80.0;
  return true;
}

static SensorRegistrar _dhtRegistrar("dht22", [](const JsonObjectConst& params) -> Sensor* {
  uint8_t pin = params["pin"] | -1;  // Default -1 if missing
  if (pin == -1) {  // Basic validation
    Serial.println("Missing 'pin' for dht22");
    return nullptr;
  }
  return new DHTSensor(pin, DHT22);
});