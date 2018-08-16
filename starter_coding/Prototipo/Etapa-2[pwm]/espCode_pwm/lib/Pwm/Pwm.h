#ifndef Pwm_h
#define Pwm_h

#include "Arduino.h"

class Pwm
{
  public:
    Pwm(int pin);
    void write(int value);
  private:
    int _pin;
    unsigned long _previousMillis;
    int _value;
};

#endif
