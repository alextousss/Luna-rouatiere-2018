#ifndef TEMP_SENSOR
#define TEMP_SENSOR

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

class TempSensors
{
public:
    TempSensors(byte pin);
    void requestTemperatures();
    float getCelcius(byte id);

private:
    OneWire _onewirelib;
    DallasTemperature _sensor;
};


#endif
