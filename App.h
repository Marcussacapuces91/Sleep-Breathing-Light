/*
Copyright 2018 Marc SIBERT

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include <Arduino.h>

class App {
public:
  App() {}

  void setup() {
    pinMode(BLUE_LED, OUTPUT);
    pinMode(BUTTON, INPUT_PULLUP);  

    const unsigned vBat = getVoltage();
    if (vBat < 2500) flash(1);
    else flash(vBat / 1000);
  }

  void loop() {
    analogWrite(BLUE_LED, 16);
    delay(100);                    
    analogWrite(BLUE_LED, 0);    
    delay(100);                    
  }

protected:
  void flash(const int n) const {
    for (int i = 0 ; i < n; ++i) {
      analogWrite(BLUE_LED, 16);
      delay(100);
      analogWrite(BLUE_LED, 0);
      delay(100);
    }
  }
  
  unsigned getVoltage() const {
    ADCSRA |= _BV(ADEN);  // Enable ADC
    ADMUX =  0b01001110;  //  vref = AVcc ; analog input = 1.1V (VBG)
    delayMicroseconds(500); // after Vref change
    ADCSRA |= _BV(ADSC);  // This starts the conversion.
/* This is an idle loop that just wait around until the conversion
 * is finished.  It constantly checks ADCSRA's ADSC bit, which we just
 * set above, to see if it is still set.  This bit is automatically
 * reset (zeroed) when the conversion is ready so if we do this in
 * a loop the loop will just go until the conversion is ready. */
    while ( (ADCSRA & _BV(ADSC)) );
    ADCSRA &= !_BV(ADEN);  // Disable ADC
    return (1100UL * 1023 / ADC);     // AVcc = Vbg/ADC*1023 = 1.1V*1023/ADC 
  }

private:
  enum {
    BLUE_LED = 10,
    BUTTON = 9
  };
 
};
