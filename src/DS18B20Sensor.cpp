#include "DS18B20Sensor.h"
#include "SensorFactory.h"

DS18B20Sensor::DS18B20Sensor(uint8_t pin) : _oneWire(pin), _sensors(&_oneWire) {}

void DS18B20Sensor::begin() {
  _sensors.begin();
  Serial.println("DS18B20 sensor initialized.");
}

bool DS18B20Sensor::updateData() {
  _sensors.requestTemperatures();
  float temp = _sensors.getTempCByIndex(0);

  if (temp == DEVICE_DISCONNECTED_C) {
    Serial.println("Failed to read from DS18B20 sensor!");
    return false;
  }

  _data[Sensor::TEMP_KEY] = temp;
  return true;
}

static SensorRegistrar _ds18b20Registrar("ds18b20", [](const JsonObjectConst& params) -> Sensor* {
  uint8_t pin = params["pin"] | -1;
  if (pin == -1) {
    Serial.println("Missing 'pin' for ds18b20");
    return nullptr;
  }
  return new DS18B20Sensor(pin);
});
