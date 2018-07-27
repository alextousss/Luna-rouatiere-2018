#ifndef ULTRASONIC
#define ULTRASONIC

#include <Arduino.h>

class UltrasonicSensor
{
public:
    UltrasonicSensor(byte trig, byte echo);
    unsigned int getRTT();
private:
    byte _trig_pin;
    byte _echo_pin;
};

#endif
