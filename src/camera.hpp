#ifndef CAMERA
#define CAMERA
#define SERVO_OPEN_POS   45
#define SERVO_CLOSED_POS 0

#include <Arduino.h>
#include <Servo.h>

class Camera
{
public:
    Camera(byte pin);
    void record();
    void stop();
private:
    byte _pin;
    Servo _servo;
};

#endif
