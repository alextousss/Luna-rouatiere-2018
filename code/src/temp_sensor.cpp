#include "temp_sensor.hpp"

TempSensors::TempSensors(byte pin) : _onewirelib(pin), _sensor(&_onewirelib)
{
    _sensor.begin();
    _sensor.setWaitForConversion(false);
    // OneWire _onewirelib(pin);
    // DallasTemperature _sensor(&_onewirelib);
}

void TempSensors::requestTemperatures()
{
    _sensor.requestTemperatures();
}

float TempSensors::getCelcius(byte id)
{
    float celcius_temp = _sensor.getTempCByIndex(id);
    return celcius_temp;
}
