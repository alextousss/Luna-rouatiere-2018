#include "ultrasonic.hpp"


UltrasonicSensor::UltrasonicSensor(byte trig, byte echo)
{
    _trig_pin = trig;
    _echo_pin = echo;
    pinMode(_trig_pin, OUTPUT);
    pinMode(_echo_pin, INPUT);
}


unsigned int UltrasonicSensor::getRTT()
{
    digitalWrite(_trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trig_pin, LOW);
    return pulseIn(_echo_pin, HIGH, 2000);     
}
