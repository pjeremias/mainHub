#include "DHTSensor.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type) : _dht(_pin, _type), _pin(pin), _type(type){}

void DHTSensor::begin() {
  _dht.begin();
  Serial.println("DHT22 sensor initialized.");
}

SensorData DHTSensor::readData() {
  SensorData data;
  // float temperature = _dht.readTemperature();  // Celsius
  // float humidity = _dht.readHumidity();

  // if (isnan(temperature) || isnan(humidity)) {
  //     Serial.println("Failed to read from DHT22 sensor!");
  //     return data;  // Return empty on error
  // }

  // data["temp"] = temperature;
  // data["hum"] = humidity;
  data["temp"] = 25.1;
  data["hum"] = 80;
  return data;
}