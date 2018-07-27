#ifndef STATUSLED
#define STATUSLED

#include <Arduino.h>

class StatusLed
{
public:
    StatusLed(byte pin, unsigned int period);
    void updateBlink();
    void error();
    
private:
    byte _pin;
    bool _status;
    unsigned long _last_toggle;
    unsigned int _blink_period; 
};

#endif
