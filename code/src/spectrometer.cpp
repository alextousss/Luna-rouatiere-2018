#include "spectrometer.hpp"


Spectrometer::Spectrometer(byte pin_servo, byte pin_photodiod)
{
    _pin_servo     = pin_servo;
    _pin_photodiod = pin_photodiod;
}

void Spectrometer::getValues(unsigned int* values)
{
    Servo _servo;
    _servo.attach(_pin_servo);
    pinMode(_pin_photodiod, INPUT);

    for(unsigned int i = 0; i < SPECTROMETER_N_VALUES ; i++)
    {
        _servo.write(i + SPECTROMETER_START_ANGLE);
        unsigned long median_value = 0;
        for( unsigned int i = 0 ; i < SPECTROMETER_SAMPLING ; i++ )
            median_value += analogRead(PIN_PHOTODIODE);
        median_value /= SPECTROMETER_SAMPLING;
        values[i] = median_value;
    }
    _servo.detach();
}
