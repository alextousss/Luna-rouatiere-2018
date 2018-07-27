#include "statusled.hpp"

StatusLed::StatusLed(byte pin, unsigned int period)
{
    _pin = pin;
    _status = LOW;
    _last_toggle = millis();
    _blink_period = period;
    pinMode(_pin, OUTPUT);
}
void StatusLed::updateBlink()
{
    if( millis() - _last_toggle > _blink_period )
    {
        _last_toggle = millis();
        _status = !_status;
        digitalWrite(_pin, _status);
    }        
}

void StatusLed::error()
{
    digitalWrite(_pin, HIGH);
}
