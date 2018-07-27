#include <Arduino.h>
#include "camera.hpp"


Camera::Camera(byte pin)
{
    _pin = pin;
}

void Camera::record()
{
    Serial.println("record!");
    _servo.attach(_pin);
    _servo.write(SERVO_CLOSED_POS);
    delay(300);
    _servo.write(SERVO_OPEN_POS);
    delay(3000);
    _servo.write(SERVO_CLOSED_POS);
    delay(2000);
    _servo.write(SERVO_OPEN_POS);
    _servo.detach();
}

void Camera::stop()
{   
    _servo.attach(_pin);
    for( unsigned int i = 0 ; i < 2 ; i++ )
    {
        Serial.println("stop record!");
        _servo.write(SERVO_CLOSED_POS);
        delay(300);
        _servo.write(SERVO_OPEN_POS);
        delay(3000);
    }
    _servo.detach();
}
