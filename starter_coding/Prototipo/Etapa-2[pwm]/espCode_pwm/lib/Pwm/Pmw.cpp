#include "Arduino.h"
#include "Pwm.h"

Pwm::Pwm(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
  _previousMillis = 0;
}

void Pwm::write(int value){
  if(value != _value){
    _value = value;
    value = map(value,0,255,50,0);
    if(value >10){

      unsigned long currentMillis = millis();

      if (currentMillis - _previousMillis >= value) {
        // save the last time you blinked the LED
        _previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (digitalRead(_pin) == LOW) {
          digitalWrite(_pin, HIGH);
        } else {
          digitalWrite(_pin, LOW);
        }
      }
    }else{
      digitalWrite(_pin, HIGH);
    }
  }
}
