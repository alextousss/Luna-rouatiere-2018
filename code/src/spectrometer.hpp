#ifndef SPECTROMETER
#define SPECTROMETER

#include <Arduino.h>
#include <Servo.h>
#include "constants.hpp"

class Spectrometer
{
public:
    Spectrometer(byte pin_servo, byte pin_photodiod);
    void getValues(unsigned int *values);
private:
    byte _pin_servo;
    byte _pin_photodiod;
};

#endif
